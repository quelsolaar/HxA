#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE


void hxa_util_node_vertex_purge(HXANode *node)
{
	unsigned int type_size[] = {sizeof(hxa_uint8), sizeof(hxa_int32), sizeof(float), sizeof(double)};
	unsigned int i, j, new_vertex_length, unit_size;
	HXALayer *layer, l;
	hxa_uint8 *new_buffer;
	int r, *remap;
	if(node->type != HXA_NT_GEOMETRY)
		return;
	remap = malloc((sizeof *remap) * node->content.geometry.vertex_count);
	for(i = 0; i < node->content.geometry.vertex_count; i++)
		remap[i] = 0;
	layer = node->content.geometry.corner_stack.layers;
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		r = layer->data.int32_data[i];
		if(r < 0)
			r = -1 - r; 
		remap[r] = 1;
	}
	for(new_vertex_length = i = 0; i < node->content.geometry.vertex_count; i++)
		if(remap[i])
			new_vertex_length++;
	if(node->content.geometry.vertex_count == new_vertex_length)
	{
		free(remap);
		return;
	}
	for(i = 0; i < node->content.geometry.vertex_stack.layer_count; i++)
	{
		layer = &node->content.geometry.vertex_stack.layers[i];
		unit_size = type_size[layer->type] * layer->components;
		new_buffer = malloc(unit_size * new_vertex_length);
		for(j = new_vertex_length = 0; j < node->content.geometry.vertex_count; j++)
			if(remap[j])
				memcpy(&new_buffer[new_vertex_length++ * unit_size], &layer->data.uint8_data[j * unit_size], unit_size);
		free(layer->data.uint8_data);
		layer->data.uint8_data = new_buffer;
	}
	node->content.geometry.vertex_count = new_vertex_length;
	for(j = new_vertex_length = 0; j < node->content.geometry.vertex_count; j++)
		if(remap[j])
			remap[j] = new_vertex_length++;
	layer = node->content.geometry.corner_stack.layers;
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		r = layer->data.int32_data[i];
		if(r < 0)
			layer->data.int32_data[i] = -remap[-layer->data.int32_data[i] - 1] - 1;
		else
			layer->data.int32_data[i] = remap[layer->data.int32_data[i]];
	}
}

void hxa_util_vertex_purge(HXAFile *file)
{
	unsigned int i;
	for(i = 0; i < file->node_count; i++)
		hxa_util_node_vertex_purge(&file->node_array[i]);
}