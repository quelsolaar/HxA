
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hxa_upi.h"


extern void hxa_util_primitive_cube(HXAFile *file, double x, double y, double z);
extern void hxa_util_primitive_grid(HXAFile *file, double x_size, double y_size, unsigned int x_grid, unsigned int y_grid, unsigned int u_axis, unsigned int v_axis);


HXAFile *hxa_upi_plugin_primitive_cube_execute_func(HXAUPIInterfaceParam *params, void *instance, void *user)
{
	HXAFile *file;
	file = malloc(sizeof *file);
	file->node_array = NULL;
	file->node_count = 0;
	file->version = HXA_VERSION_FORMAT;
	hxa_util_primitive_cube(file, params[0].content.vector_3d[0], params[0].content.vector_3d[1], params[0].content.vector_3d[2]);
	return file;
}


HxALibExport void hxa_upi_plugin_library_initialize(void (*hxa_upi_library_register_func)(HxAUPIPlugin *plugin))
{
	HxAUPIPlugin p;
	p.params = malloc(sizeof(HXAUPIInterfaceParam) * 1);
	p.params[0].type = HXA_UPI_IPT_VECTOR_3D;
	p.params[0].name = "Size";
	p.params[0].description = "The size of the cube.";
	p.params[0].content.hxa = NULL;
	p.param_count = 1;
	p.name = "cube primitive";
	p.descrtiption = "Creates a cube mesh.";
	p.has_output = 1;
	p.instance_create_func = NULL;
	p.instance_destroy_func = NULL;
	p.instance_update_func = NULL;
	p.execute_func = hxa_upi_plugin_primitive_cube_execute_func;
	p.user_data = NULL;
	hxa_upi_library_register_func(&p);
	
	p.param_count = 5;
	p.params = malloc(sizeof(HXAUPIInterfaceParam) * p.param_count);
	p.params[0].type = HXA_UPI_IPT_VECTOR_2D;
	p.params[0].name = "Size";
	p.params[0].description = "The size of the cube.";
	p.params[0].content.vector_2d[0] = 1;
	p.params[0].content.vector_2d[1] = 1;

	p.params[1].type = HXA_UPI_IPT_UNSIGNED_INTEGER;
	p.params[1].name = "U Spans";
	p.params[1].description = "The number of spans in the U direction.";
	p.params[1].content.unsigned_integer = 1;

	p.params[2].type = HXA_UPI_IPT_UNSIGNED_INTEGER;
	p.params[2].name = "V Spans";
	p.params[2].description = "The number of spans in the V direction.";
	p.params[2].content.unsigned_integer = 1;

	p.params[3].type = HXA_UPI_IPT_SELECT;
	p.params[3].name = "U axis";
	p.params[3].description = "The axis of the U direction";
	p.params[3].content.select.selected = 0;
	p.params[3].content.select.option_count = 3;
	p.params[3].content.select.option_names = malloc((sizeof *p.params[3].content.select.option_names) * p.params[3].content.select.option_count);
	p.params[3].content.select.option_names[0] = "X";
	p.params[3].content.select.option_names[1] = "Y";
	p.params[3].content.select.option_names[2] = "Z";
	p.params[3].content.select.selected = 0;

	p.params[4].type = HXA_UPI_IPT_SELECT;
	p.params[4].name = "V axis";
	p.params[4].description = "The axis of the U direction";
	p.params[4].content.select.selected = 2;
	p.params[4].content.select.option_count = 3;
	p.params[4].content.select.option_names = malloc((sizeof *p.params[3].content.select.option_names) * p.params[3].content.select.option_count);
	p.params[4].content.select.option_names[0] = "X";
	p.params[4].content.select.option_names[1] = "Y";
	p.params[4].content.select.option_names[2] = "Z";
	p.name = "Grid primitive";
	p.descrtiption = "Creates a grid mesh.";
	p.has_output = 1;
	p.instance_create_func = NULL;
	p.instance_destroy_func = NULL;
	p.instance_update_func = NULL;
	p.execute_func = hxa_upi_plugin_primitive_cube_execute_func;
	p.user_data = NULL;
	hxa_upi_library_register_func(&p);
}

