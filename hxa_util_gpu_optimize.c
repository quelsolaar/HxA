#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE


void hxa_corner_to_vertex(HXANode *node)
{
	size_t type_sizes[HXA_LDT_COUNT] = {sizeof(unsigned char), sizeof(int), sizeof(float), sizeof(double)};
	int *new_ref, *old_ref, vertex_count;
	unsigned int i, j, *vertex_ref, section;
	unsigned char *read, *write;
	if(node->content.geometry.corner_stack.layer_count == 1)
		return;
	new_ref = malloc((sizeof *new_ref) * node->content.geometry.edge_corner_count);
	vertex_ref = malloc((sizeof *vertex_ref) * node->content.geometry.edge_corner_count);
	old_ref = node->content.geometry.corner_stack.layers->data.int32_data;
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		vertex_ref[i] = -1;
		new_ref[i] = -1;
	}
	for(i = vertex_count = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		if(new_ref[i] == -1)
		{
			if(old_ref[i] < 0)
				new_ref[i] = -1 - vertex_count;
			else
				new_ref[i] = vertex_count;
			vertex_ref[vertex_count++] = i;
		}
	}
	
	for(i = 0; i < node->content.geometry.vertex_stack.layer_count; i++)
	{	
		section = node->content.geometry.vertex_stack.layers[i].components * type_sizes[node->content.geometry.vertex_stack.layers[i].type];
		write = malloc(section * vertex_count);
		read = node->content.geometry.vertex_stack.layers[i].data.uint8_data;
		for(j = 0; j < vertex_count; j++)
			memcpy(&write[j * section], &read[old_ref[vertex_ref[j]] * section], section);
		free(read);
		node->content.geometry.vertex_stack.layers[i].data.uint8_data = write;
	}
	node->content.geometry.vertex_stack.layers = realloc(node->content.geometry.vertex_stack.layers, (sizeof *node->content.geometry.vertex_stack.layers) * (node->content.geometry.vertex_stack.layer_count + node->content.geometry.corner_stack.layer_count - 1));
	for(i = 1; i < node->content.geometry.corner_stack.layer_count; i++)
	{	
		node->content.geometry.vertex_stack.layers[node->content.geometry.vertex_stack.layer_count] = node->content.geometry.corner_stack.layers[i];
		section = node->content.geometry.corner_stack.layers[i].components * type_sizes[node->content.geometry.corner_stack.layers[i].type];
		write = malloc(section * vertex_count);
		node->content.geometry.vertex_stack.layers[node->content.geometry.vertex_stack.layer_count].data.uint8_data = read = node->content.geometry.vertex_stack.layers[i].data.uint8_data;
		for(j = 0; j < vertex_count; j++)
			memcpy(&write[j * section], &read[vertex_ref[j] * section], section);
	}
	for(i = 0; i < node->content.geometry.corner_stack.layer_count; i++)
		free(node->content.geometry.corner_stack.layers[i].data.uint8_data);
	node->content.geometry.corner_stack.layers = realloc(node->content.geometry.corner_stack.layers, (sizeof *node->content.geometry.corner_stack.layers));
	node->content.geometry.corner_stack.layers->data.int32_data = new_ref;
}