
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hxa_upi.h"

extern void hxa_print(HXAFile *file, int data);

HXAFile *hxa_upi_plugin_execute_func(HXAUPIInterfaceParam *params, void *instance, void *user)
{
	hxa_print(params[0].content.hxa, params[1].content.boolean);
	return NULL;
}



HxALibExport void hxa_upi_plugin_library_initialize(void (*hxa_upi_library_register_func)(HxAUPIPlugin *plugin))
{
	HxAUPIPlugin p;
	p.params = malloc(sizeof(HXAUPIInterfaceParam) * 2);	
	p.params[0].type = HXA_UPI_IPT_HXA_READ;
	p.params[0].name = "HxA";
	p.params[0].description = "The HxH structure to be printed";
	p.params[0].content.hxa = NULL;	
	p.params[1].type = HXA_UPI_IPT_BOOLEAN;
	p.params[1].name = "Data";
	p.params[1].description = "If set to false the content of layers will not be printed.";
	p.params[1].content.boolean = 1;
	p.param_count = 2;
	p.name = "print";
	p.descrtiption = "Prints out a text description of the content of a HxA file structure";
	p.has_output = 0;
	p.instance_create_func = NULL;
	p.instance_destroy_func = NULL;
	p.instance_update_func = NULL;
	p.execute_func = hxa_upi_plugin_execute_func;
	p.user_data = NULL;
	hxa_upi_library_register_func(&p);
}