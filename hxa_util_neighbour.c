#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE


int hxa_corner_get_next(int *ref, unsigned int corner)
{
	unsigned int i;
	if(ref[corner] >= 0)
	{
		return corner + 1;
	}else
	{
		for(i = corner - 1; i != 0; i--)
			if(ref[i] < 0)
				return i + 1;
		return 0;
	}
}

int hxa_corner_get_previous(int *ref, unsigned int corner)
{
	unsigned int i;
	if(corner == 0 || ref[corner - 1] < 0)
	{
		for(i = corner + 1; ref[i] >= 0; i++);
		return i;
	}else
		return corner - 1;
}


int hxa_edge_get_next(int *ref, unsigned int *neighbour, unsigned int edge)
{
	if(neighbour[edge] != -1)
		return edge;
		
	edge = hxa_corner_get_next(ref, edge);
	while(neighbour[edge] != -1)
	{
		edge = neighbour[edge];
		edge = hxa_corner_get_next(ref, edge);
	}
	return edge;
}

int hxa_edge_get_previous(int *ref, unsigned int *neighbour, unsigned int edge)
{
	if(neighbour[edge] != -1)
		return edge;	
	edge = hxa_corner_get_previous(ref, edge);
	while(neighbour[edge] != -1)
	{
		edge = neighbour[edge];
		edge = hxa_corner_get_previous(ref, edge);
	}
	return edge;
}


unsigned int *hxa_neighbour_node(HXANode *node)
{
	char *name = HXA_CONVENTION_HARD_EDGE_NEIGHBOUR_LAYER_NAME;
	unsigned int j, cor, clear = 0, *n, *v, r, a, b, vertex_count, edge_length;
	unsigned int counter = 0, laps = 0;
	int i, *ref;
	vertex_count = node->content.geometry.vertex_count;
	edge_length = node->content.geometry.edge_corner_count;
	ref = node->content.geometry.corner_stack.layers[0].data.int32_data;
	n = malloc((sizeof *n) * edge_length);
	for(i = 0; i < edge_length; i++)
		n[i] = -1;
	v = malloc((sizeof *v) * vertex_count);
	for(i = 0; i < vertex_count; i++)
		v[i] = -1;
	while(clear < edge_length)
	{
		for(i = 0; i < edge_length && clear < edge_length; i++)
		{
			counter++;
			clear++;
			r = hxa_ref(ref, i);
			cor = v[r];
			if(cor == -1)
			{
				 if(n[i] == -1 || n[hxa_corner_get_previous(ref, i)] == -1)
					v[r] = i;
			}else if(cor == i)
				v[r] = -1;
			else 
			{
				a = hxa_corner_get_previous(ref, i);
				b = hxa_corner_get_next(ref, cor);
				if(n[cor] == -1 && n[a] == -1 && hxa_ref(ref, a) == hxa_ref(ref, b))
				{
					n[a] = cor;
					n[cor] = a;
					clear = 0;
					if(n[hxa_corner_get_previous(ref, cor)] != -1)
					{
						if(n[i] == -1)
							v[r] = i;
						else
							v[r] = -1;
					}
				}
				a = hxa_corner_get_next(ref, i);
				b = hxa_corner_get_previous(ref, cor);
				if(n[i] == -1 && n[b] == -1 && hxa_ref(ref, a) == hxa_ref(ref, b))
				{
					n[i] = b;						
					n[b] = i;
					clear = 0;
					if(n[cor] != -1)
					{
						if(n[hxa_corner_get_previous(ref, i)] == -1)
							v[r] = i;
						else
							v[r] = -1;
					}
				}
			}
		}
		laps++;
	}
	free(v);
	for(i = 0; i < node->content.geometry.edge_stack.layer_count; i++)
	{
		for(j = 0; node->content.geometry.edge_stack.layers[i].name[j] == name[j] && name[j] != 0; j++);
		if(node->content.geometry.edge_stack.layers[i].name[j] == name[j])
		{
			free(node->content.geometry.edge_stack.layers[i].data.int32_data);
			node->content.geometry.edge_stack.layers[i].data.int32_data = n;
			return n;
		}
	}
	node->content.geometry.edge_stack.layers = realloc(node->content.geometry.edge_stack.layers, (sizeof *node->content.geometry.edge_stack.layers) * (node->content.geometry.edge_stack.layer_count + 1));
	node->content.geometry.edge_stack.layers[node->content.geometry.edge_stack.layer_count].data.int32_data = n;
	node->content.geometry.edge_stack.layers[node->content.geometry.edge_stack.layer_count].components = 1;
	node->content.geometry.edge_stack.layers[node->content.geometry.edge_stack.layer_count].type = HXA_LDT_INT32;
	for(i = 0; name[i] != 0; i++)
		node->content.geometry.edge_stack.layers[node->content.geometry.edge_stack.layer_count].name[i] = name[i];
	node->content.geometry.edge_stack.layers[node->content.geometry.edge_stack.layer_count].name[i] = 0;
	node->content.geometry.edge_stack.layer_count++;
	return n;
}

void hxa_neighbour_file(HXAFile *file)
{
	unsigned int i;
	for(i = 0; i < file->node_count; i++)
		if(file->node_array[i].type == HXA_NT_GEOMETRY)
			hxa_neighbour_node(&file->node_array[i]);
}