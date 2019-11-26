#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"


void hxa_util_convert_layer_float_to_double(HXALayer *layer, unsigned int count)
{
	unsigned int j, length;
	double *d;
	float *f;
	if(layer->type == HXA_LDT_FLOAT)
	{
		length = layer->components * count;
		f = layer->data.float_data;
		layer->data.double_data = d = malloc((sizeof *d) * length);
		for(j = 0; j < length; j++)
			d[j] = (double)f[j];
		free(f);
		layer->type = HXA_LDT_DOUBLE;
	}
}

void hxa_util_convert_layer_double_to_float(HXALayer *layer, unsigned int count)
{
	unsigned int j, length;
	double *d;
	float *f;
	if(layer->type == HXA_LDT_DOUBLE)
	{
		length = layer->components * count;
		d = layer->data.double_data;
		layer->data.float_data = f = malloc((sizeof *f) * length);
		for(j = 0; j < length; j++)
			f[j] = (double)d[j];
		free(d);
		layer->type = HXA_LDT_FLOAT;
	}
}

void hxa_util_convert_stack_float_to_double(HXALayerStack *stack, unsigned int count)
{
	unsigned int i;
	for(i = 0; i < stack->layer_count; i++)
		hxa_util_convert_layer_float_to_double(&stack->layers[i], count);
}

void hxa_util_convert_stack_double_to_float(HXALayerStack *stack, unsigned int count)
{
	unsigned int i;
	for(i = 0; i < stack->layer_count; i++)
		hxa_util_convert_layer_double_to_float(&stack->layers[i], count);
}


void hxa_util_convert_node_float_to_double(HXANode *node)
{
	if(node->type == HXA_NT_GEOMETRY)
	{
		hxa_util_convert_stack_float_to_double(&node->content.geometry.vertex_stack, node->content.geometry.vertex_count);
		hxa_util_convert_stack_float_to_double(&node->content.geometry.corner_stack, node->content.geometry.edge_corner_count);
		hxa_util_convert_stack_float_to_double(&node->content.geometry.edge_stack, node->content.geometry.edge_corner_count);
		hxa_util_convert_stack_float_to_double(&node->content.geometry.face_stack, node->content.geometry.face_count);
	}
	if(node->type == HXA_NT_IMAGE)
	{
		switch(node->content.image.type)
		{
			case HXA_IT_CUBE_IMAGE :
			hxa_util_convert_stack_float_to_double(&node->content.image.image_stack, node->content.image.resolution[0] * node->content.image.resolution[1] * 6);
			break;
			case HXA_IT_1D_IMAGE :
			hxa_util_convert_stack_float_to_double(&node->content.image.image_stack, node->content.image.resolution[0]);
			break;
			case HXA_IT_2D_IMAGE :
			hxa_util_convert_stack_float_to_double(&node->content.image.image_stack, node->content.image.resolution[0] * node->content.image.resolution[1]);
			break;
			case HXA_IT_3D_IMAGE :
			hxa_util_convert_stack_float_to_double(&node->content.image.image_stack, node->content.image.resolution[0] * node->content.image.resolution[1] * node->content.image.resolution[2]);
			break;
		}	
	}
}

void hxa_util_convert_node_double_to_float(HXANode *node)
{
	if(node->type == HXA_NT_GEOMETRY)
	{
		hxa_util_convert_stack_double_to_float(&node->content.geometry.vertex_stack, node->content.geometry.vertex_count);
		hxa_util_convert_stack_double_to_float(&node->content.geometry.corner_stack, node->content.geometry.edge_corner_count);
		hxa_util_convert_stack_double_to_float(&node->content.geometry.edge_stack, node->content.geometry.edge_corner_count);
		hxa_util_convert_stack_double_to_float(&node->content.geometry.face_stack, node->content.geometry.face_count);
	}
	if(node->type == HXA_NT_IMAGE)
	{
		switch(node->content.image.type)
		{
			case HXA_IT_CUBE_IMAGE :
			hxa_util_convert_stack_double_to_float(&node->content.image.image_stack, node->content.image.resolution[0] * node->content.image.resolution[1] * 6);
			break;
			case HXA_IT_1D_IMAGE :
			hxa_util_convert_stack_double_to_float(&node->content.image.image_stack, node->content.image.resolution[0]);
			break;
			case HXA_IT_2D_IMAGE :
			hxa_util_convert_stack_double_to_float(&node->content.image.image_stack, node->content.image.resolution[0] * node->content.image.resolution[1]);
			break;
			case HXA_IT_3D_IMAGE :
			hxa_util_convert_stack_double_to_float(&node->content.image.image_stack, node->content.image.resolution[0] * node->content.image.resolution[1] * node->content.image.resolution[2]);
			break;
		}	
	}
}
