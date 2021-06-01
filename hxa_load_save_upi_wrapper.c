#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)
#include <stdlib.h>
#include <stdio.h>
#include "hxa_upi.h"

#define FALSE 0
#define TRUE !FALSE

extern HXAFile *hxa_load(char *file_name, int silent);
extern int hxa_save(char *file_name, HXAFile *data);

HXAFile *hxa_upi_plugin_load_wrapper(HXAUPIInterfaceParam *params, void *instance, void *user)
{
	return hxa_load(params[0].content.path_read, TRUE);
}

HXAFile *hxa_upi_plugin_save_wrapper(HXAUPIInterfaceParam *params, void *instance, void *user)
{
	if(params[1].content.hxa != NULL)
		hxa_save(params[1].content.path_read, params[0].content.hxa);
	return NULL;
}

void hxa_upi_plugin_library_initialize(void (*register_func)(HxAUPIPlugin *plugin))
{
	HXAUPIInterfaceParam params[2];
	HxAUPIPlugin plugin;
	/* set up load plugin */
	plugin.params = params;
	plugin.param_count = 1;
	plugin.name = "HxA File loader";
	plugin.descrtiption = "HxA File loader"; 
	plugin.has_output = TRUE;
	plugin.instance_create_func = NULL;
	plugin.instance_destroy_func = NULL;
	plugin.instance_update_func = NULL;
	plugin.execute_func = hxa_upi_plugin_load_wrapper;
	plugin.user = NULL; /* user pointer that will be given to all above function pointers when called. */
	params[0].type = HXA_IPT_PATH_READ;
	params[0].name = "HxA File loader";
	params[0].description = "HxA File loader"; 
	params[0].content.path_read = NULL;
	register_func(&plugin);
	
	/* set up save plugin */
	plugin.params = params;
	plugin.param_count = 2;
	plugin.name = "HxA File Saver";
	plugin.descrtiption = "HxA File Saver"; 
	plugin.has_output = TRUE;
	plugin.instance_create_func = NULL;
	plugin.instance_destroy_func = NULL;
	plugin.instance_update_func = NULL;
	plugin.execute_func = hxa_upi_plugin_save_wrapper;
	plugin.user = NULL; /* user pointer that will be given to all above function pointers when called. */
	params[0].type = HXA_IPT_HXA;
	params[0].name = "Input HxA";
	params[0].description = "HxA structure thto be saved"; 
	params[0].content.hxa = NULL;
	params[1].type = HXA_IPT_PATH_WRITE;
	params[1].name = "HxA Fie path";
	params[1].description = "Local file path used to save HxA structure"; 
	params[1].content.path_read = NULL;
	register_func(&plugin);
}
