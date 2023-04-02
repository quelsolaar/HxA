#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_upi.h"

#define FALSE 0
#define TRUE !FALSE

extern void hxa_serialize_free_file(HXAFile *file);
extern HXAFile *hxa_unserialize(hxa_uint8 *buffer, size_t size, int silent);

int hxa_convert_param_to_int(char *string, int *output)
{
	unsigned int i = 0;
	*output = 0;
	if(string[i] == '-')
		i++;

	if(string[i] < '0' && string[i] > '9')
		return 0;
	*output += (unsigned int)string[i++] - (unsigned int)'0';
	while(string[i] < '0' && string[i] > '9')
	{
		*output *= 10;
		*output += (unsigned int)string[i++] - (unsigned int)'0';
	}
	if(string[0] == '-')
		*output = -*output;
	return i;
}

int hxa_convert_param_to_double(char *string, double *output)
{
	unsigned int i = 0, pos = 0;
	double factor = 1.0;
	int integer;
	if(*string == '-')
		pos++;
	pos += hxa_convert_param_to_int(&string[pos], &integer);
	*output = (double)integer;
	if(string[pos] == '.')
	{
		pos++;
		while(string[pos] < '0' && string[pos] > '9')
		{
			factor *= 0.1;
			*output += factor * (double)((unsigned int)string[pos++] - (unsigned int)'0');
		}
	}
	if(string[0] == '-')
		*output = -*output;
	return pos;
}


int hxa_type_test(char *path, char *type)
{
	unsigned int i, j;
	for(i = 0; path[i] != 0; i++)
	{
		if(path[i] == '.')
		{
			i++;
			for(j = 0; type[j] != 0 && path[i + j] != 0 && (path[i + j] == type[j] || path[i + j] + 32 == type[j]); j++);
			if(type[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}

extern int hxa_load_png(HXAFile *file, char *file_name);
extern void hxa_type_convert_gen();

unsigned int hxa_upi_param_count[] = {1, // HXA_UPI_IPT_BOOLEAN
										1, // HXA_UPI_IPT_SIGNED_INTEGER
										1, // HXA_UPI_IPT_UNSIGNED_INTEGER
										1, // HXA_UPI_IPT_DOUBLE_BOUND
										1, // HXA_UPI_IPT_DOUBLE_UNBOUND
										2, // HXA_UPI_IPT_POS_2D
										3, // HXA_UPI_IPT_POS_3D
										2, // HXA_UPI_IPT_VECTOR_2D
										3, // HXA_UPI_IPT_VECTOR_3D
										16, // HXA_UPI_IPT_MATRIX
										3, // HXA_UPI_IPT_COLOR
										1, // HXA_UPI_IPT_TEXT
										1, // HXA_UPI_IPT_PATH_READ
										1, // HXA_UPI_IPT_PATH_WRITE
										1, // HXA_UPI_IPT_SELECT
										1, // HXA_UPI_IPT_HXA_READ
										1}; // HXA_UPI_IPT_HXA_CONSUME 

HXAFile *hxa_upi_parse(HxAUPIPlugin *plugins, unsigned int plugin_count, int argc, char **argv, int *arg_pos)
{
	HXAUPIInterfaceParam *params = NULL;
	HXAFile *output;
	HxAUPIPlugin *plugin;
	void *instance = NULL;
	int integer;
	unsigned int i, j;
	for(i = 0; i < plugin_count && 0 != strcmp(argv[*arg_pos], plugins[i].name); i++);
	if(i < plugin_count)
	{
		(*arg_pos)++;
		plugin = &plugins[i];
		if(plugin->param_count != 0)
		{
			params = malloc((sizeof *params) * plugin->param_count);
			memcpy(params, plugin->params, (sizeof *params) * plugin->param_count);
			for(i = 0; i < plugin->param_count; i++)
			{
				if(hxa_upi_param_count[plugin->params[i].type] + *arg_pos > argc)
				{
					printf("Error: Argument %u of plugin %s \"%s\" is not a valid boolean value\n", i, plugin->name, argv[*arg_pos]);
					exit(0);
				}
				switch(plugin->params[i].type)
				{
					case HXA_UPI_IPT_BOOLEAN :
						if(0 == strcmp(argv[*arg_pos], "TRUE") || 0 == strcmp(argv[*arg_pos], "true") || 0 == strcmp(argv[*arg_pos], "t"))
						{
							params[i].content.boolean = TRUE;
						}else if(0 == strcmp(argv[*arg_pos], "FALSE") || 0 == strcmp(argv[*arg_pos], "false") || 0 == strcmp(argv[*arg_pos], "f"))
						{		
							params[i].content.boolean = FALSE;
						}else if(0 == hxa_convert_param_to_int(argv[*arg_pos], &integer))
						{
							printf("Error: Argument %u of plugin %s \"%s\" is not a valid boolean value\n", i, plugin->name, argv[*arg_pos]);
							exit(0);
						}else
							params[i].content.boolean = integer != FALSE;
					break;
					case HXA_UPI_IPT_SIGNED_INTEGER :
						if(0 == hxa_convert_param_to_int(argv[*arg_pos], &params[i].content.signed_integer))
						{
							printf("Error: Argument %u of plugin %s \"%s\" is not a valid integer value\n", i, plugin->name, argv[*arg_pos]);
							exit(0);
						}		
					break;
					case HXA_UPI_IPT_UNSIGNED_INTEGER :
						if(0 == hxa_convert_param_to_int(argv[*arg_pos], &params[i].content.unsigned_integer))
						{
							printf("Error: Argument %u of plugin %s \"%s\" is not a valid integer value\n", i, plugin->name, argv[*arg_pos]);
							exit(0);
						}		
					break;
					case HXA_UPI_IPT_DOUBLE_BOUND :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.double_value))
						{
							printf("Error: Argument %u of plugin %s \"%s\" is not a valid floating point value\n", i, plugin->name, argv[*arg_pos]);
							exit(0);
						}
						if(params[i].content.double_value > 1.0)
							params[i].content.double_value = 1.0;
						if(params[i].content.double_value < 0.0)
							params[i].content.double_value = 0.0;
					break;
					case HXA_UPI_IPT_DOUBLE_UNBOUND :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.double_value))
						{
							printf("Error: Argument %u of plugin %s \"%s\" is not a valid floating point value\n", i, plugin->name, argv[*arg_pos]);
							exit(0);
						}	
					break;
					case HXA_UPI_IPT_POS_2D :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.point_2d[0]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 1], &params[i].content.point_2d[1]))
						{
							printf("Error: Argument %u of plugin %s \"%s\" \"%s\" are not valid floating point values\n", i, plugin->name, argv[*arg_pos], argv[*arg_pos + 1]);
							exit(0);
						}
					break;
					case HXA_UPI_IPT_POS_3D :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.point_3d[0]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 1], &params[i].content.point_3d[1]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 2], &params[i].content.point_3d[2]))
						{
							printf("Error: Argument %u of plugin %s \"%s\" \"%s\" \"%s\" are not valid floating point values\n", i, plugin->name, argv[*arg_pos], argv[*arg_pos + 1], argv[*arg_pos + 2]);
							exit(0);
						}
					break;
					case HXA_UPI_IPT_VECTOR_2D :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.vector_2d[0]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 1], &params[i].content.vector_2d[1]))
						{
							printf("Error: Argument %u of plugin %s \"%s\" \"%s\" are not valid floating point values\n", i, plugin->name, argv[*arg_pos], argv[*arg_pos + 1]);
							exit(0);
						}
					break;
					case HXA_UPI_IPT_VECTOR_3D :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.vector_3d[0]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 1], &params[i].content.vector_3d[1]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 2], &params[i].content.vector_3d[2]))
						{
							printf("Error: Argument %u of plugin %s \"%s\" \"%s\" \"%s\" are not valid floating point values\n", i, plugin->name, argv[*arg_pos], argv[*arg_pos + 1], argv[*arg_pos + 2]);
							exit(0);
						}
					break;
					case HXA_UPI_IPT_MATRIX :
						for(j = 0; j < 16 && 0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.matrix[j]); j++);
						if(j < 16)
						{
							printf("Error: Argument %u of plugin %s \"%s\" are not valid floating point values\n", i, plugin->name, argv[*arg_pos + j]);
							exit(0);
						}
					break;
					case HXA_UPI_IPT_COLOR :
						if(0 == hxa_convert_param_to_double(argv[*arg_pos], &params[i].content.color[0]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 1], &params[i].content.color[1]) ||
							0 == hxa_convert_param_to_double(argv[*arg_pos + 2], &params[i].content.color[2]))
						{
							printf("Error: Argument %u of plugin %s \"%s\" \"%s\" \"%s\" are not valid floating point values\n", i, plugin->name, argv[*arg_pos], argv[*arg_pos + 1], argv[*arg_pos + 2]);
							exit(0);
						}
					break;
					case HXA_UPI_IPT_TEXT :
					case HXA_UPI_IPT_PATH_READ :
					case HXA_UPI_IPT_PATH_WRITE :
						plugin->params[i].content.text = argv[*arg_pos];
					break;
					case HXA_UPI_IPT_SELECT :	
						for(params[i].content.select.selected = 0; 
							params[i].content.select.selected < params[i].content.select.option_count && 
							!strcmp(argv[*arg_pos], plugin->params[i].content.select.option_names[plugin->params[i].content.select.selected]);
							params[i].content.select.selected++);
						if(params[i].content.select.selected == params[i].content.select.option_count)
						{
							if(0 == hxa_convert_param_to_int(argv[*arg_pos], &params[i].content.select.selected))
							{
								printf("Error: Argument %u of plugin %s \"%s\" is not a valid integer value or option string\n", i, plugin->name, argv[*arg_pos]);
								exit(0);
							}
						}
					break;
					case HXA_UPI_IPT_HXA_READ :
					case HXA_UPI_IPT_HXA_CONSUME :
						params[i].content.hxa = hxa_upi_parse(plugins, plugin_count, argc, argv, arg_pos);
						(*arg_pos)--;
					break;
				}
				*arg_pos += hxa_upi_param_count[plugin->params[i].type];
			}
		}
		if(plugin->instance_create_func != NULL)
			instance = plugin->instance_create_func(params, plugin->user_data);
		output = plugin->execute_func(params, instance, plugin->user_data);
		if(plugin->instance_destroy_func != NULL && instance != NULL)
			plugin->instance_destroy_func(params, plugin->user_data, instance);
		for(i = 0; i < plugin->param_count; i++)
			if(plugin->params[i].type == HXA_UPI_IPT_HXA_READ)
				hxa_serialize_free_file(plugin->params[i].content.hxa);
		if(plugin->param_count != 0)
			free(params);	
	}else
	{
		unsigned char *buffer;
		unsigned long long allocation, i;
		FILE *f;
		f = fopen(argv[*arg_pos], "rb");
		if(f == NULL)
		{
			printf("Error: Argument \"%s\" is not a valid plugin or HxA file.\n", argv[*arg_pos]);
			exit(0);
		}
		fseek(f, 0, SEEK_END);
		allocation = ftell(f);
		if(allocation == 0)
		{
			fclose(f);
			printf("Error: Argument \"%s\" file is empty.\n", argv[*arg_pos]);
			exit(0);
		}
		(*arg_pos)++;
		rewind(f);
		buffer = malloc(allocation);
		memset(buffer, 0, allocation);
		fread(buffer, 1, allocation, f);
		fclose(f);
		output = hxa_unserialize(buffer, allocation, FALSE);
		free(buffer);
		if(output == NULL)
			exit(0);
	}		
	return output;
}

void hxa_upi_help(HxAUPIPlugin *plugins)
{
	char *typen_names[] = {"Boolean", 
		"Signed integer", 
		"Unsigned integer", 
		"value between 0 and 1", 
		"floating point value", 
		"2D position", 
		"3D Position", 
		"2D vector", 
		"3D vector", 
		"4x4 matrix", 
		"RGB color", 
		"Text String", 
		"Load path", 
		"Save path", 
		"option select", 
		"HxA structure or file", 
		"HxA structure or file"};
	unsigned int i, j;
	printf("Command: %s\n", plugins->name);
	printf("\tDescription: %s\n", plugins->descrtiption);
	if(plugins->param_count != 0)
		printf("\tParameters:\n");
	for(i = 0; i < plugins->param_count; i++)
	{
		printf("\t\t-%s : %s\n", plugins->params[i].name, typen_names[plugins->params[i].type]);
		if(plugins->params[i].type == HXA_UPI_IPT_SELECT)
		{
			for(j = 0; j < plugins->params[i].content.select.option_count; j++)
				printf("\t\t\toption %u : %s\n", j, plugins->params[i].content.select.option_names[j]);

		}
		printf("\t\t Param description: %s\n", plugins->params[i].description);
	}	
	if(plugins->has_output)
		printf("\tOutputs HxA structure\n");
}

int main(int argc, char **argv)
{
	HXAFile *file;
	HxAUPIPlugin *plugins;
	char *path = ".\\";
	unsigned int i, used = 1, plugin_count = 0;
	if(used < argc && 0 == strcmp(argv[used], "path"))
	{
		if(argc < 3)
		{
			printf("Error: Missing path parameter.\n");
			return FALSE;
		}
		path = argv[used + 1]; 
		used += 2;
	}
	plugins = hxa_upi_host_load_directory(path, &plugin_count);
	if(plugin_count == 0)
	{
		printf("Error: No UBI plugins found at path: %s\n", path);
		return FALSE;
	}
	if(used == argc || (used + 1 == argc && 0 == 0 == strcmp(argv[used], "help")))
	{
		printf("Command list:\n");
		printf("-------------\n");

		for(i = 0; i < plugin_count; i++)
			hxa_upi_help(&plugins[i]);
		return TRUE;
	}
	if(0 == strcmp(argv[used], "help"))
	{
		used++;
		for(i = 0; i < plugin_count; i++)
		{
			if(0 == strcmp(argv[used], plugins[i].name))
			{
				hxa_upi_help(&plugins[i]);
				return TRUE;
			}
		}
		printf("Error: No UBI plugins with the name %s found.\n", argv[used]);
		return FALSE;
	}

	file = hxa_upi_parse(plugins, plugin_count, argc, argv, &used);
	if(file != NULL)
		hxa_serialize_free_file(file);
	printf("Sucsess!!");
	return TRUE;
}
