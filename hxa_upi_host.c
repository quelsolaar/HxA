#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

#include "hxa_upi.h"
#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#include <mmsystem.h>
#include <tchar.h>
#include <wchar.h>

#define HXA_UPI_DEBUG_PRINT
#define HXA_UPI_WINDOWS_PATH_LENGHT_MAX (256 * 128 - 1)

#undef UNICODE

#ifdef UNICODE
#define hxa_upi_internal_lib_load LoadLibraryW
#else
#define hxa_upi_internal_lib_load LoadLibraryA
#endif
#define hxa_upi_internal_lib_get_address GetProcAddress
#define hxa_upi_internal_lib_unload FreeLibrary
#else
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#define hxa_upi_internal_lib_load(a) dlopen(a, RTLD_LAZY);
#define hxa_upi_internal_lib_get_address dlsym
#define hxa_upi_internal_lib_unload dlclose
#endif
typedef unsigned int uint;

HxAUPIPlugin *hxa_upi_host_temp_loaded_plugins = NULL;
unsigned int hxa_upi_host_temp_loaded_plugin_count = 0;
void *hxa_upi_host_temp_loaded_library = NULL;


#define HXA_UPI_NXT	0x80
#define HXA_UPI_SEQ2 0xc0
#define HXA_UPI_SEQ3 0xe0
#define HXA_UPI_SEQ4 0xf0
#define HXA_UPI_SEQ5 0xf8
#define HXA_UPI_SEQ6 0xfc
#define HXA_UPI_BOM	0xfeff

unsigned int hxa_upi_utf8_to_uint32(char *c, uint *pos)
{
	uint i, bits, character_count = 1, high;
	unsigned int out;

	if((c[*pos] & 0x80) == 0)
		high = (wchar_t)c[*pos];
	else if((c[*pos] & 0xe0) == HXA_UPI_SEQ2)
	{
		character_count = 2;
		high = (wchar_t)(c[*pos] & 0x1f);
	}else if((c[*pos] & 0xf0) == HXA_UPI_SEQ3)
	{
		character_count = 3;
		high = (wchar_t)(c[*pos] & 0x0f);
	}else if((c[*pos] & 0xf8) == HXA_UPI_SEQ4)
	{
		character_count = 4;
		high = (wchar_t)(c[*pos] & 0x07);
	}else if((c[*pos] & 0xfc) == HXA_UPI_SEQ5)
	{
		character_count = 5;
		high = (wchar_t)(c[*pos] & 0x03);
	}else if((c[*pos] & 0xfe) == HXA_UPI_SEQ6)
	{
		character_count = 6;
		high = (wchar_t)(c[*pos] & 0x01);
	}else
	{
		(*pos)++;
		return 0;
	}
	out = 0;
	bits = 0;
	for(i = 1; i < character_count; i++)
	{
		out |= (wchar_t)(c[character_count - i] & 0x3f) << bits;
		bits += 6;		/* 6 low bits in every byte */
	}
	out |= high << bits;
	(*pos) += character_count;
	return out;
}

uint hxa_upi_uint32_to_utf8(unsigned int character, char *out)
{
	unsigned char bytes[4];
	bytes[0] = (character >> 24) & 0xFF;
	bytes[1] = (character >> 16) & 0xFF;
	bytes[2] = (character >> 8) & 0xFF;
	bytes[3] = character & 0xFF;
	if(character <= 0x0000007f)
	{
		out[0] = bytes[3];
		return 1;
	}else if(character <= 0x000007ff)
	{
		out[1] = HXA_UPI_NXT | bytes[3] & 0x3f;
		out[0] = HXA_UPI_SEQ2 | (bytes[3] >> 6) | ((bytes[2] & 0x07) << 2);
		return 2;
	}else if(character <= 0x0000ffff)
	{
		out[2] = HXA_UPI_NXT | bytes[3] & 0x3f;
		out[1] = HXA_UPI_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[0] = HXA_UPI_SEQ3 | ((bytes[2] & 0xf0) >> 4);
		return 3;
	}else if(character <= 0x001fffff)
	{
		out[3] = HXA_UPI_NXT | bytes[3] & 0x3f;
		out[2] = HXA_UPI_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[1] = HXA_UPI_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[0] = HXA_UPI_SEQ4 | ((bytes[1] & 0x1f) >> 2);
		return 4;
	}else if(character <= 0x03ffffff)
	{
		out[4] = HXA_UPI_NXT | bytes[3] & 0x3f;
		out[3] = HXA_UPI_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[2] = HXA_UPI_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[1] = HXA_UPI_NXT | (bytes[1] >> 2);
		out[0] = HXA_UPI_SEQ5 | bytes[0] & 0x03;
		return 5;
	}else /* if (*w <= 0x7fffffff) */
	{
		out[5] = HXA_UPI_NXT | bytes[3] & 0x3f;
		out[4] = HXA_UPI_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[3] = HXA_UPI_NXT | (bytes[2] >> 4) | ((bytes[1] & 0x03) << 4);
		out[2] = HXA_UPI_NXT | (bytes[1] >> 2);
		out[1] = HXA_UPI_NXT | bytes[0] & 0x3f;
		out[0] = HXA_UPI_SEQ6 | ((bytes[0] & 0x40) >> 6);
		return 6;
	}
}


void hxa_upi_host_register_func(HxAUPIPlugin *plugin)
{
	if(hxa_upi_host_temp_loaded_plugin_count % 16 == 0)
		hxa_upi_host_temp_loaded_plugins = realloc(hxa_upi_host_temp_loaded_plugins, (sizeof *hxa_upi_host_temp_loaded_plugins) * (hxa_upi_host_temp_loaded_plugin_count + 16));
	hxa_upi_host_temp_loaded_plugins[hxa_upi_host_temp_loaded_plugin_count] = *plugin;
	hxa_upi_host_temp_loaded_plugins[hxa_upi_host_temp_loaded_plugin_count].params = malloc((sizeof *plugin->params) * plugin->param_count);
	memcpy(hxa_upi_host_temp_loaded_plugins[hxa_upi_host_temp_loaded_plugin_count].params, plugin->params, (sizeof *plugin->params) * plugin->param_count);
	hxa_upi_host_temp_loaded_plugins[hxa_upi_host_temp_loaded_plugin_count].library = hxa_upi_host_temp_loaded_library;
	hxa_upi_host_temp_loaded_plugin_count++;
}

void hxa_upi_host_load_library_internal(char *path)
{
	void (*hxa_upi_plugin_library_initialize)(void (*register_func)(HxAUPIPlugin *plugin));
	char *name;
	unsigned int i, pre_count;
#ifdef UNICODE
	short u_path[1025];
	for(i = 0; i < 1024 && path[i] !=0 ; i++)
		u_path[i] = (char)path[i];
	u_path[i] = 0;
	hxa_temp_loaded_library = hxa_upi_internal_lib_load(u_path);
#else
	hxa_upi_host_temp_loaded_library = hxa_upi_internal_lib_load(path);
#endif
	if(hxa_upi_host_temp_loaded_library == NULL)
	{
#ifdef HXA_UPI_DEBUG_PRINT
#ifdef _WIN32
		printf("HxA UPI ERROR: library %s not found. Windows error code: %u\n", path, GetLastError());
#else
		printf("HxA UPI ERROR: library %s not found.\n", path);
#endif
#endif
		return NULL;
	}
#ifdef WIN32
	hxa_upi_plugin_library_initialize = (char *(*)(void))GetProcAddress(hxa_upi_host_temp_loaded_library, "hxa_upi_plugin_library_initialize");
#else
	hxa_upi_plugin_library_initialize = dlsym(hxa_upi_host_temp_loaded_library, "hxa_upi_plugin_library_initialize");
#endif
	if(hxa_upi_plugin_library_initialize == NULL)
	{
#ifdef HXA_UPI_DEBUG_PRINT
		printf("HxA UPI ERROR: imagine_lib_main not found in %s.\n", path);
#endif
		hxa_upi_internal_lib_unload(hxa_upi_host_temp_loaded_library);
		return NULL;
	}
	pre_count = hxa_upi_host_temp_loaded_plugin_count;
	hxa_upi_plugin_library_initialize(hxa_upi_host_register_func);
	if(pre_count == hxa_upi_host_temp_loaded_plugin_count)
		hxa_upi_internal_lib_unload(hxa_upi_host_temp_loaded_library);
}



void hxa_upi_host_unload_library(char *path, HxAUPIPlugin *plugins, unsigned int plugin_count)
{
	uint i, j;
	for(i = 0; i < plugin_count; i++)
	{
		free(plugins[i].params);
		if(i == 0 || plugins[i].library != plugins[i - 1].library)
			hxa_upi_internal_lib_unload(plugins[i - 1].library);
	}
	free(plugins);
}

HxAUPIPlugin *hxa_upi_host_load_library(char *path, unsigned int *plugin_count)
{
	hxa_upi_host_temp_loaded_plugins = NULL;
	hxa_upi_host_temp_loaded_plugin_count = 0;
	hxa_upi_host_load_library_internal(path);
	*plugin_count = hxa_upi_host_temp_loaded_plugin_count;
	return hxa_upi_host_temp_loaded_plugins;
}


#ifdef _WIN32
 /* implement path searching for windows */
typedef struct{
	char			d_name[HXA_UPI_WINDOWS_PATH_LENGHT_MAX];
}idirent;
 
typedef struct{
	WIN32_FIND_DATAW data; 
	HANDLE			handle;
	idirent			ent;
}DIR;

DIR *hxa_upi_opendir(char *path) 
{ 
	DIR *dir;
	unsigned int pos, i;
	WIN32_FIND_DATAW data; 
	HANDLE handle; 
	wchar_t unicode_path[HXA_UPI_WINDOWS_PATH_LENGHT_MAX];
	char *post_fix = "\\*.*";
	if(path == NULL)
	{
		unicode_path[0] = ".";
		unicode_path[1] = 0;
	}else
	{		
		for(i = pos = 0; i < HXA_UPI_WINDOWS_PATH_LENGHT_MAX - 5 && path[pos] != 0; i++)
			unicode_path[i] = hxa_upi_utf8_to_uint32(path, &pos);
	}
	for(pos = 0; post_fix[pos] != 0; i++)
		unicode_path[i] = (wchar_t)post_fix[pos++];
	unicode_path[i] = 0;
	if((handle = FindFirstFileW(unicode_path, &data)) != INVALID_HANDLE_VALUE)
	{ 
		dir = malloc(sizeof *dir);
		dir->handle = handle;
		dir->data = data;
		dir->ent.d_name[0] = 0;
		return dir;
	}
	return NULL;
}

void hxa_upi_closedir(DIR *dir) 
{
	FindClose(dir->handle);
	free(dir);
}

idirent *hxa_upi_readdir(DIR *dir) 
{
	uint i, pos;
	if(FindNextFileW(dir->handle, &dir->data) != TRUE)
		return NULL;
	for(i = pos = 0; dir->data.cFileName[i] != 0 && pos < HXA_UPI_WINDOWS_PATH_LENGHT_MAX - 6; i++)
		pos += hxa_upi_uint32_to_utf8(dir->data.cFileName[i], &dir->ent.d_name[pos]);
	dir->ent.d_name[pos] = 0;
	return &dir->ent;
	
}

#else
 /* use unix APIs */
#define idirent struct dirent
#define hxa_upi_opendir opendir;
#define hxa_upi_closedir closedir
#define hxa_upi_readdir readdir
#endif

int hxa_upi_host_filter(char *path, char *filter)
{
	unsigned int i, j;
	for(i = 0; path[i] != 0; i++)
	{
		for(j = 0; path[i + j] != 0 && filter[j] != 0 && (filter[j] == path[i + j] || filter[j] + 32 == path[i + j]); j++);
		if(filter[j] == 0)
			return TRUE;
	}
	return FALSE;
}

HxAUPIPlugin *hxa_upi_host_load_directory(char *path, unsigned int *plugin_count)
{
	DIR *d;
	idirent *ent;
	char p[1024];
	unsigned int i, start;
	hxa_upi_host_temp_loaded_plugins = NULL;
	hxa_upi_host_temp_loaded_plugin_count = 0;
	for(start = 0; start < 1024 - 1 && path[start] != 0; start++)
		p[start] = path[start];
	d = hxa_upi_opendir(path);
	if(d != NULL)
	{
		ent = hxa_upi_readdir(d);
		if(ent != NULL)
		{
			while(TRUE)
			{	
				if(hxa_upi_host_filter(ent->d_name, ".DLL") ||
					hxa_upi_host_filter(ent->d_name, ".SO") ||
					hxa_upi_host_filter(ent->d_name, ".LIB"))
				{ 
					for(i = 0; i + start < 1024 - 1 && ent->d_name[i] != 0; i++)
						p[start + i] = ent->d_name[i];
					if(i + start < 1024 - 1)
					{
						p[start + i] = 0;
						hxa_upi_host_load_library_internal(p);
					}
				}
				ent = hxa_upi_readdir(d);
				if(ent == NULL)
					break;
			}
		}
		hxa_upi_closedir(d);
	}
	*plugin_count = hxa_upi_host_temp_loaded_plugin_count;
	return hxa_upi_host_temp_loaded_plugins;
}
