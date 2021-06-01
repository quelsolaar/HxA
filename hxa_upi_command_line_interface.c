#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE

int hxa_convert_param_to_int(char *string, unsigned int *output)
{
	*output = 0;
	if(string[0] < '0' && string[0] > '9')
		return FALSE;
	while(string[0] < '0' && string[0] > '9')
	{
		*output *= 10;
		*output += (unsigned int)string[0] - (unsigned int)'0';
	}
	return TRUE;
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

int main(int argc, char **argv)
{
	HXAFile *file = NULL, *file2;
	unsigned int i, j, silent, save, int_param;

//	hxa_type_convert_gen();
	if(argc < 2)
	{
		printf("HxA utility. Written by Eskil Steenberg www.quelsolaar.com. MIT License\n");
		printf("Usage: %s <file in> <optional file out> <arguments>\n", argv[0]);
		printf("argument operations are executed in order.\n");
		printf("Argument list:\n");
		printf(" - \"validate\" validates the file to make sure it is correct. Non valid files may cause the tools to fail.\n");
		printf(" - \"powvalid\" validates that all images in a file have power of two resolution.\n");
		printf(" - \"neighbour\" compute a neighbour layer storing adjacency.\n");
		printf(" - \"save\" <filename> write out the file.\n");
		printf(" - \"d2f\" converts all double precission layers to float layers.\n");
		printf(" - \"f2d\" converts all float layers to double precission layers.\n");
		printf(" - \"triangulate\" <max sides> Converts all polygons with more sides then <max sides> in to triangles.\n");
		printf(" - \"vertexpurge\" Removes any unreferenced vertex from the file.\n");
		printf(" - \"close\" Closes any holes in meshes.\n");
		printf(" - \"print\" prints the structure of the file.\n");
		printf(" - \"printall\" prints the structure and data the file.\n");
		printf(" - \"printmeta\" <key> prints the value of meta entries of a specific name.\n");
		return TRUE;
	}
	for(i = 3; i < argc && strcmp(argv[i], "silent") != 0; i++);
	silent = i != argc;

		
	if(hxa_type_test(argv[1], "fbx"))
	{
		if(!silent)
			printf("Loading Autodesk FBX file: %s\n", argv[1]);
		file = hxa_util_fbx_load(argv[1], NULL);
		if(!silent && file == NULL)
			printf("HxA Error: FBX load failed.\n");
	}else if(hxa_type_test(argv[1], "ttf") || hxa_type_test(argv[1], "tte") || hxa_type_test(argv[1], "dfont") || hxa_type_test(argv[1], "otf") || hxa_type_test(argv[1], "otc") || hxa_type_test(argv[1], "ttf") || hxa_type_test(argv[1], "ttc"))
	{
		if(!silent)
			printf("Loading TrueType font file: %s\n", argv[1]);
		file = hxa_util_true_type_load(argv[1]);
		if(!silent && file == NULL)
			printf("HxA Error: TrueType load failed.\n");
	}else if(hxa_type_test(argv[1], "png"))
	{
		if(!silent)
			printf("Loading PNG image file: %s\n", argv[1]);
		file = hxa_util_true_type_load(argv[1]);
		if(!silent && file == NULL)
			printf("HxA Error: PNG image load failed.\n");
	}	
	if(file == NULL)
	{
		if(!silent)
			printf("Loading HxA file: %s\n", argv[1]);
		file = hxa_load(argv[1], silent);
		if(!silent && file == NULL)
		{
			printf("HxA Error: Loading file failed. Exiting...\n");
			return 0;
		}
	}

	save = TRUE;
	for(i = 2; i < argc; i++)
	{
		 if(strcmp(argv[i], "save") == 0)
		{
			if(++i < argc)
			{
				if(!silent)
					printf("Saving HxA file: %s\n", argv[i]);
				if(!hxa_save(argv[i], file) && !silent)
				{
					printf("Saving file failed. Exiting...\n");
					return FALSE;
				}
			}else
			{
				printf("HxA Error: \"save\" is missing a filename parameter. Exiting...\n");
				return FALSE;
			}
		}else if(strcmp(argv[i], "validate") == 0)
		{
			if(!hxa_util_validate(file, silent))
				return FALSE;
		}else if(strcmp(argv[i], "powvalid") == 0)
		{
			if(!hxa_util_validate_pow(file, silent))
				return FALSE;
		}else if(strcmp(argv[i], "neighbour") == 0)
		{
			hxa_neighbour_file(file);
		}else if(strcmp(argv[i], "f2d") == 0)
		{
			for(j = 0; j < file->node_count; j++)
				hxa_util_convert_node_float_to_double(&file->node_array[j]);
		}else if(strcmp(argv[i], "d2f") == 0)
		{
			for(j = 0; j < file->node_count; j++)
				hxa_util_convert_node_double_to_float(&file->node_array[j]);
		}else if(strcmp(argv[i], "triangulate") == 0)
		{
			if(hxa_convert_param_to_int(argv[++i], &int_param) && int_param > 2)
			{
				for(j = 0; j < file->node_count; j++)
					hxa_util_triangulate_node(&file->node_array[j], int_param);
			}else
			{
				printf("HxA Error: the command param \"triangulate\" must be a folowed by a integer param set to 3 or larger");
				return FALSE;
			}
		}else if(strcmp(argv[i], "vertexpurge") == 0)
		{
			hxa_util_vertex_purge(file);
		}else if(strcmp(argv[i], "close") == 0)
		{
			hxa_close_file(file);
		}else if(strcmp(argv[i], "print") == 0)
		{
			hxa_print(file, FALSE);
		}else if(strcmp(argv[i], "printall") == 0)
		{
			hxa_print(file, TRUE);
		}else if(strcmp(argv[i], "printmeta") == 0)
		{
			if(++i < argc)
				hxa_print_meta_find(file, argv[i]);
			else
			{
				printf("HxA Error: \"printmeta\" is missing a key parameter. Exiting...\n");
				return FALSE;
			}	
		}else if(strcmp(argv[i], "merge") == 0)
		{
			file2 = hxa_load(argv[++i], silent);
			if(file2 != NULL)
				hxa_util_merge(file, file2);
			else
				return FALSE;
		}else 
		{
			if(i == 2)
				save = TRUE;
			else
			{
				printf("HxA Error: Parameter \"%s\" is invalid. Exiting...\n", argv[i]);
				return FALSE;
			}
		}
	}
	return TRUE;
}
