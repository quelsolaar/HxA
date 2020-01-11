#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hxa.h"
#include "hxa_utils.h"

typedef struct{
	unsigned int edge;
	unsigned int original;
	float value;
	void *next;
	void *previous;
}HxAHoleEdge; 

float un_hole_vector_normalized3f(float *vec, float *start, float *end)
{
	float f;
	vec[0] = end[0] - start[0];
	vec[1] = end[1] - start[1];
	vec[2] = end[2] - start[2];
	f = sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= f;
	vec[1] /= f;
	vec[2] /= f;
	return f;
}

void un_hole_normal3f(float *output, float *a, float *b, float *c)
{
	float a2[3], b2[3], f;
	a2[0] = a[0] - c[0];
	a2[1] = a[1] - c[1];
	a2[2] = a[2] - c[2];
	b2[0] = b[0] - c[0];
	b2[1] = b[1] - c[1];
	b2[2] = b[2] - c[2];
	f = sqrtf(a2[0] * a2[0] + a2[1] * a2[1] + a2[2] * a2[2]);
	a2[0] /= f;
	a2[1] /= f;
	a2[2] /= f;
	f = sqrtf(b2[0] * b2[0] + b2[1] * b2[1] + b2[2] * b2[2]);
	b2[0] /= f;
	b2[1] /= f;
	b2[2] /= f;
	output[0] = a2[1] * b2[2] - a2[2] * b2[1];
	output[1] = a2[2] * b2[0] - a2[0] * b2[2];
	output[2] = a2[0] * b2[1] - a2[1] * b2[0];
	f = sqrtf(output[0] * output[0] + output[1] * output[1] + output[2] * output[2]);
	output[0] /= f;
	output[1] /= f;
	output[2] /= f;
}

float un_hole_edge_length(int *ref, float *vertex, unsigned int edge)
{
	float vec[3];
	int v1, v2;
	v1 = ref[edge];
	if(v1 < 0)
		v1 = -1 - v1;
	edge = hxa_corner_get_next(ref, edge);
	v2 = ref[edge];
	if(v2 < 0)
		v2 = -1 - v2;
	v1 *= 3;
	v2 *= 3;
	vec[0] = vertex[v1 + 0] - vertex[v2 + 0];
	vec[1] = vertex[v1 + 1] - vertex[v2 + 1];
	vec[2] = vertex[v1 + 2] - vertex[v2 + 2];
	return vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
}


float un_hole_edge_rate(int *ref, float *vertex, unsigned int *neighbour, unsigned int edge_a, unsigned int edge_b, unsigned int edge_c)
{
	float n[3], n2[3], f, v0[3], v1[3];
	unsigned int next;
	int vr[4];
	vr[0] = (hxa_ref(ref, edge_a)) * 3;
	vr[1] = (hxa_ref(ref, edge_b)) * 3;
	vr[2] = (hxa_ref(ref, edge_c)) * 3;
	un_hole_normal3f(n, &vertex[vr[1]], &vertex[vr[0]], &vertex[vr[2]]);	
	next = hxa_corner_get_previous(ref, edge_a);
	vr[3] = (hxa_ref(ref, next)) * 3;
	un_hole_normal3f(n2, &vertex[vr[0]], &vertex[vr[1]], &vertex[vr[3]]);
	
	f = 1.0 + n[0] * n2[0] + n[1] * n2[1] + n[2] * n2[2];
	if(f != f)
		f = 0.1;
	next = hxa_corner_get_previous(ref, edge_b);
	vr[3] = (hxa_ref(ref, next)) * 3;
	un_hole_normal3f(n2, &vertex[vr[1]], &vertex[vr[2]], &vertex[vr[3]]);
	f *= 1.0 + n[0] * n2[0] + n[1] * n2[1] + n[2] * n2[2];
	if(f != f)
		f = 0.1;
	un_hole_vector_normalized3f(v0, &vertex[vr[1]], &vertex[vr[0]]);
	un_hole_vector_normalized3f(v1, &vertex[vr[1]], &vertex[vr[2]]);
	f *= 1.0 + v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
	if(f != f)
		f = 0.0;
	return f;
}

unsigned int hxa_hole_close_one(int *ref, unsigned int ref_length, float *vertex_array, unsigned int *neighbour, unsigned int edge, HxAHoleEdge *buffer)
{
	HxAHoleEdge *start, *next, *found;
	unsigned int i, count = 0;
	i = edge;		
	while(1)
	{
		buffer[count].edge = i;
		buffer[count].original = i;
		buffer[count].next = &buffer[count + 1];
		buffer[count].previous = &buffer[count - 1];
		i = hxa_edge_get_next(ref, neighbour, i);
		count++;
		if(i == edge)
			break;
	}
	buffer[count - 1].next = &buffer[0];
	buffer[0].previous = &buffer[count - 1];
	
	for(i = 0; i < count; i++)
	{
		buffer[i].value = un_hole_edge_rate(ref, vertex_array, neighbour, buffer[i].edge, buffer[(i + 1) % count].edge, buffer[(i + 2) % count].edge);
		
	}
	start = buffer;


	while(count != 0)
	{
		found = start;
		for(next = start->next; next != start; next = next->next)
			if(found->value < next->value)
				found = next;
				
		
		next = found->next;
		ref[ref_length + 0] = hxa_ref(ref, found->edge);
		ref[ref_length + 1] = hxa_ref(ref, ((HxAHoleEdge *)next->next)->edge);
		ref[ref_length + 2] = -hxa_ref(ref, next->edge) - 1;
	
		neighbour[ref_length + 1] = next->edge;
		neighbour[ref_length + 2] = found->edge;
		
		neighbour[next->edge] = ref_length + 1;
		neighbour[found->edge] = ref_length + 2;
		
		if(count == 3)
		{
			neighbour[ref_length] = ((HxAHoleEdge *)next->next)->edge;
			neighbour[((HxAHoleEdge *)next->next)->edge] = ref_length;
			return ref_length + 3;
		}else
			neighbour[ref_length] = -1;
		if(count == 3)
			break;
		

		found->next = ((HxAHoleEdge *)found->next)->next;
		((HxAHoleEdge *)found->next)->previous = found;
		found->edge = ref_length;
		found->value = un_hole_edge_rate(ref, vertex_array, neighbour, found->edge, ((HxAHoleEdge *)found->next)->edge, ((HxAHoleEdge *)((HxAHoleEdge *)found->next)->next)->edge);
		((HxAHoleEdge *)found->previous)->value = un_hole_edge_rate(ref, vertex_array, neighbour, ((HxAHoleEdge *)found->previous)->edge, found->edge, ((HxAHoleEdge *)found->next)->edge);
		if(un_hole_edge_length(ref, vertex_array, ref_length + 1) < un_hole_edge_length(ref, vertex_array, ref_length + 2))
			found->value *= 0.95;
		else
			((HxAHoleEdge *)found->previous)->value *= 0.95;
		ref_length += 3;
		start = found;
		count--;
	}
	return ref_length;
}



void hxa_close_node_extend_layer(HXALayer *layer, unsigned int last_length, unsigned int new_length)
{
	HXALayer l;
	unsigned int i, length;
	switch(layer->type)
	{
		case HXA_LDT_UINT8 :
			length = (sizeof *l.data.uint8_data) * last_length * layer->components;
			l.data.uint8_data = malloc(length);
			memcpy(l.data.uint8_data, layer->data.uint8_data, length);
			length = new_length * layer->components;
			for(i = new_length * layer->components; i < length; i++)
				l.data.uint8_data[i] = 0;
			free(layer->data.uint8_data);
			layer->data.uint8_data = l.data.uint8_data;
		break;
		case HXA_LDT_INT32 :
			length = (sizeof *l.data.int32_data) * last_length * layer->components;
			l.data.int32_data = malloc(length);
			memcpy(l.data.int32_data, layer->data.int32_data, length);
			length = new_length * layer->components;
			for(i = new_length * layer->components; i < length; i++)
				l.data.int32_data[i] = 0;
			free(layer->data.int32_data);
			layer->data.int32_data = l.data.int32_data;
		break;
		case HXA_LDT_FLOAT :
			length = (sizeof *l.data.float_data) * last_length * layer->components;
			l.data.float_data = malloc(length);
			memcpy(l.data.float_data, layer->data.float_data, length);
			length = new_length * layer->components;
			for(i = new_length * layer->components; i < length; i++)
				l.data.float_data[i] = 0;
			free(layer->data.float_data);
			layer->data.float_data = l.data.float_data;
		break;
		case HXA_LDT_DOUBLE :
			length = (sizeof *l.data.double_data) * last_length * layer->components;
			l.data.double_data = malloc(length);
			memcpy(l.data.double_data, layer->data.double_data, length);
			length = new_length * layer->components;
			for(i = new_length * layer->components; i < length; i++)
				l.data.double_data[i] = 0;
			free(layer->data.double_data);
			layer->data.double_data = l.data.double_data;
		break;
	}
}

void hxa_close_node(HXANode *node)
{
	char *name = HXA_CONVENTION_HARD_EDGE_NEIGHBOUR_LAYER_NAME;
	HxAHoleEdge *edge_buffer;
	float *vertex_array;
	unsigned int i, j, count, *n, *n2;
	signed int *ref;
	if(node->type != HXA_NT_GEOMETRY || node->content.geometry.vertex_count == 0 || node->content.geometry.edge_corner_count == 0)
		return;

	n =	hxa_neighbour_node(node); 
	for(count = i = 0; i < node->content.geometry.edge_corner_count; i++)
		if(n[i] == -1)
			count++;
	if(count == 0)
	{
		free(n);
		return;
	}
	ref = malloc((sizeof *ref) * (node->content.geometry.edge_corner_count + count * 3));
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		ref[i] = node->content.geometry.corner_stack.layers[0].data.int32_data[i];
	}
	n2 = malloc((sizeof *ref) * (node->content.geometry.edge_corner_count + count * 3));
	memcpy(n2, n, (sizeof *n2) * node->content.geometry.edge_corner_count);
	free(n);
	edge_buffer = malloc((sizeof *edge_buffer) * count);
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
	{
		count = node->content.geometry.vertex_count * 3;
		vertex_array = malloc((sizeof *vertex_array) * count);
		for(i = 0; i < count; i++)
			vertex_array[i] = (float)node->content.geometry.vertex_stack.layers[0].data.double_data[i];
	}else
		vertex_array = node->content.geometry.vertex_stack.layers[0].data.float_data;

	count = node->content.geometry.edge_corner_count;
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
		if(n2[i] == -1)
			count = hxa_hole_close_one(ref, count, vertex_array, n2, i, edge_buffer);
			
	free(node->content.geometry.corner_stack.layers->data.int32_data);
	node->content.geometry.corner_stack.layers->data.int32_data = ref;
	for(i = 1; i < node->content.geometry.corner_stack.layer_count; i++)
		hxa_close_node_extend_layer(&node->content.geometry.corner_stack.layers[i], node->content.geometry.edge_corner_count, count);
	for(i = 0; i < node->content.geometry.edge_stack.layer_count; i++)
	{
		for(j = 0; name[j] == node->content.geometry.corner_stack.layers[i].name[j] && 0 != node->content.geometry.corner_stack.layers[i].name[j]; j++);
		if(name[j] == node->content.geometry.corner_stack.layers[i].name[j])
		{
			memcpy(node->content.geometry.corner_stack.layers[i].data.int32_data, n2, sizeof(hxa_int32) * count);
		}else
			hxa_close_node_extend_layer(&node->content.geometry.edge_stack.layers[i], node->content.geometry.edge_corner_count, count);
	}
	for(i = 0; i < node->content.geometry.face_stack.layer_count; i++)
		hxa_close_node_extend_layer(&node->content.geometry.face_stack.layers[i], node->content.geometry.face_count, node->content.geometry.face_count + (count - node->content.geometry.edge_corner_count) / 3);

	node->content.geometry.face_count += (count - node->content.geometry.edge_corner_count) / 3;
	node->content.geometry.edge_corner_count = count;
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
		free(vertex_array);
	free(n2);
	free(edge_buffer);
}



void hxa_close_file(HXAFile *file)
{
	unsigned int i;
	hxa_print(file, 0);
	for(i = 0; i < file->node_count; i++)
		hxa_close_node(&file->node_array[i]);
	hxa_print(file, 0);
	i = 0;
}