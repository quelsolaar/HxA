#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#include <mmsystem.h>

#undef UNICODE

#ifdef UNICODE
#define i_internal_lib_load LoadLibraryW
#else
#define i_internal_lib_load LoadLibraryA
#endif
#define i_internal_lib_get_address GetProcAddress
#define i_internal_lib_unload FreeLibrary
#else
#include <dlfcn.h>
#define i_internal_lib_load(a) dlopen(a, RTLD_LAZY);
#define i_internal_lib_get_address dlsym
#define i_internal_lib_unload dlclose
#endif
typedef unsigned int uint;

typedef struct{
	void *function;
	char *name;
}IFunction;

typedef struct{
	IFunction *functions;
	uint function_count;
}IInterface;

typedef struct{
	void *dll;
	IInterface *dll_interface;
	IInterface *exe_interface;
}ILib;

IInterface *imagine_library_interface_create()
{
	IInterface *i;
	i = malloc(sizeof *i);
	i->functions = NULL;
	i->function_count = 0;
	return i;
}

void imagine_library_interface_destroy(IInterface *i)
{
	if(i == NULL)
		return;
	if(i->functions != NULL)
		free(i->functions);
	free(i);
}

void imagine_library_interface_register(IInterface *i, void *funtion_pointer, char *name)
{
	uint length;
	if(i->function_count % 16 == 0)
	{
		i->functions = realloc(i->functions, (sizeof *i->functions) * (i->function_count + 16));
	}
	i->functions[i->function_count].function = funtion_pointer;
	for(length = 0; name[length] != 0; length++);
	i->functions[i->function_count].name = malloc((sizeof *i->functions[i->function_count].name) * (length + 1));
	for(length = 0; name[length] != 0; length++)
		i->functions[i->function_count].name[length] = name[length];
	i->functions[i->function_count].name[length] = 0;
	i->function_count++;
}

int	imagine_library_interface_count(IInterface *i)
{
	return i->function_count;
}

char *imagine_library_interface_list(IInterface *i, uint number)
{
	return i->functions[number].name;
}

void *imagine_library_interface_get_by_number(IInterface *i, uint number)
{
	return i->functions[number].function;
}

void *imagine_library_interface_get_by_name(IInterface *i, char *name)
{
	uint j, k;
	for(j = 0; j < i->function_count; j++)
	{
		for(k = 0; name[k] == i->functions[j].name[k] && name[k] != 0; k++);
		if(name[k] == i->functions[j].name[k])
			return i->functions[j].function;
	}
	printf("Imagine Error: Funtion: %s not found %u\n", name, i->function_count);
	return NULL;
}

ILib *imagine_library_load(char *path, IInterface *iinterface, char *interface_name)
{
	IInterface *(*imagine_lib_main)(IInterface *exe_interface);
	char *(*imagine_lib_name)(void);
	void *dll;
	ILib *lib;
	char *name;
	unsigned int i;
#ifdef UNICODE
	short u_path[1025];
	for(i = 0; i < 1024 && path[i] !=0 ; i++)
		u_path[i] = (char)path[i];
	u_path[i] = 0;
	dll = i_internal_lib_load(u_path);
#else
	dll = i_internal_lib_load(path);
#endif
	if(dll == NULL)
	{
#ifdef _WIN32
		printf("Imagine ERROR: library %s not found. Windows error code: %u\n", path, GetLastError());
#else
		printf("Imagine ERROR: library %s not found.\n", path);
#endif
		return NULL;
	}
	if(interface_name != NULL)
	{
		imagine_lib_name = (char *(*)(void))i_internal_lib_get_address(dll, "imagine_lib_name");

		if(imagine_lib_name == NULL)
		{
			i_internal_lib_unload(dll);
			printf("Imagine ERROR: imagine_lib_name not found in %s.\n", path);
			return NULL;
		}
		name = imagine_lib_name();
		for(i = 0; interface_name[i] != 0 && interface_name[i] == name[i]; i++);

		if(interface_name[i] != name[i])
		{
			printf("Imagine ERROR: Dll %s is of type %s not %s.\n", path, name, interface_name);
			i_internal_lib_unload(dll);
			return NULL;
		}
	}
#ifdef WIN32
	imagine_lib_main = (char *(*)(void))GetProcAddress(dll, "imagine_lib_main");
#else
	imagine_lib_main = dlsym(dll, "imagine_lib_main");
#endif
	if(imagine_lib_main == NULL)
	{
		printf("Imagine ERROR: imagine_lib_main not found in %s.\n", path);
		i_internal_lib_unload(dll);
		return NULL;
	}

	lib = malloc(sizeof *lib);

	lib->dll = dll;
	lib->dll_interface = imagine_lib_main(iinterface);
	lib->exe_interface = iinterface;
	return lib;
}

typedef unsigned char boolean;

void imagine_library_unload(ILib *lib, boolean interface_delete)
{
	imagine_library_interface_destroy(lib->dll_interface);
	if(interface_delete)
		imagine_library_interface_destroy(lib->exe_interface);
	i_internal_lib_unload(lib->dll);
	free(lib);
}

