#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE

int main(int argc, char **argv)
{
	HXAFile *file;
	unsigned int i, silent, save;
	if(argc < 2)
	{
		printf("HxA utility. Written by Eskil Steenberg www.quelsolaar.com. MIT License\n");
		printf("Usage: %s <file in> <optional file out> <arguments>\n", argv[0]);
		printf("argument operations are executed in order.\n");
		printf("Argument list:\n");
		printf(" - \"save\" <filename> write out the file.\n");
		printf(" - \"print\" prints the structure of the file.\n");
		printf(" - \"printall\" prints the structure and data the file.\n");
		printf(" - \"printmeta\" <key> prints the value of meta entries of a specific name.\n");
		return TRUE;
	}
	for(i = 3; i < argc && strcmp(argv[i], "silent") != 0; i++);
	silent = i != argc;
	if(!silent)
		printf("Loading HxA file: %s\n", argv[1]);
	file = hxa_load(argv[1], silent);
	if(file == NULL)
	{			
		if(!silent)
			printf("HxA Error: Loading file failed. Exiting...\n");
		return FALSE;
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
				if(!hxa_save(argv[2], file) && !silent)
				{
					printf("Saving file failed. Exiting...\n");
					return FALSE;
				}
			}else
			{
				printf("HxA Error: \"save\" is missing a filename parameter. Exiting...\n");
				return FALSE;
			}
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