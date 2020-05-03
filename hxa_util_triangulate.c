#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE

typedef enum{
	HXA_UTIL_TEC_VALID,
	HXA_UTIL_TEC_CONCAVE,
	HXA_UTIL_TEC_OVERLAPING
}HxATrinagulateEdgeCategory;

typedef struct{
	int vertex;
	unsigned int origin;
	HxATrinagulateEdgeCategory category;
	double weight;
	unsigned int worst_vertex;
}HxATrinagulateEdge;


void hxa_util_triangulate_ngon_edge_evaluate(HxATrinagulateEdge *edges, unsigned int edge_count, unsigned int active, double *vertex)
{
	double normal[3], v0[3], v1[3], v2[3], t0[3], t1[3], t2[3], *base0, *base1, *middle, *test, f;
	unsigned int i, first = TRUE;
	base0 = &vertex[edges[active].vertex * 3];
	middle = &vertex[edges[(active + 1) % edge_count].vertex * 3];
	base1 = &vertex[edges[(active + 2) % edge_count].vertex * 3];
	
	t0[0] = base0[0] - base1[0];
	t0[1] = base0[1] - base1[1];
	t0[2] = base0[2] - base1[2];
	t1[0] = middle[0] - base1[0];
	t1[1] = middle[1] - base1[1];
	t1[2] = middle[2] - base1[2];
	normal[0] = t0[1] * t1[2] - t0[2] * t1[1];
	normal[1] = t0[2] * t1[0] - t0[0] * t1[2];
	normal[2] = t0[0] * t1[1] - t0[1] * t1[0];
	f = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] /= f;
	normal[1] /= f;
	normal[2] /= f;
	
	v0[0] = base1[0] - base0[0];
	v0[1] = base1[1] - base0[1];
	v0[2] = base1[2] - base0[2];
	v1[0] = base0[0] - middle[0];
	v1[1] = base0[1] - middle[1];
	v1[2] = base0[2] - middle[2];
	v2[0] = middle[0] - base1[0];
	v2[1] = middle[1] - base1[1];
	v2[2] = middle[2] - base1[2];

	t0[0] = v0[1] * normal[2] - v0[2] * normal[1];
	t0[1] = v0[2] * normal[0] - v0[0] * normal[2];
	t0[2] = v0[0] * normal[1] - v0[1] * normal[0];

	t1[0] = v1[1] * normal[2] - v1[2] * normal[1];
	t1[1] = v1[2] * normal[0] - v1[0] * normal[2];
	t1[2] = v1[0] * normal[1] - v1[1] * normal[0];

	t2[0] = v2[1] * normal[2] - v2[2] * normal[1];
	t2[1] = v2[2] * normal[0] - v2[0] * normal[2];
	t2[2] = v2[0] * normal[1] - v2[1] * normal[0];

	edges[active].category = HXA_UTIL_TEC_VALID;
	edges[active].weight = -1.0;
	edges[active].worst_vertex = -1;
	for(i = 0; i < edge_count - 3; i++)
	{
		test = &vertex[edges[(active + i + 3) % edge_count].vertex * 3];
		f = t0[0] * (test[0] - base0[0]) + t0[1] * (test[1] - base0[1]) + t0[2] * (test[2] - base0[2]);
		if(f > 0.0)
		{
			if(t1[0] * (test[0] - base0[0]) + t1[1] * (test[1] - base0[1]) + t1[2] * (test[2] - base0[2]) > 0 &&
				t2[0] * (test[0] - base1[0]) + t2[1] * (test[1] - base1[1]) + t2[2] * (test[2] - base1[2]) > 0)
			{
				f = normal[0] * (test[0] - base0[0]) + normal[1] * (test[1] - base0[1]) + normal[2] * (test[2] - base0[2]);
				if(f < 0)
					f = f * -0.1;
				if(edges[active].category < HXA_UTIL_TEC_OVERLAPING || f > edges[active].weight)
				{
					edges[active].category = HXA_UTIL_TEC_OVERLAPING;
					edges[active].weight = f;
					edges[active].worst_vertex = edges[(i + 3) % edge_count].vertex;
				}
			}else if(edges[active].category < HXA_UTIL_TEC_CONCAVE || (f > edges[active].weight && edges[active].category != HXA_UTIL_TEC_OVERLAPING))
			{
				edges[active].category = HXA_UTIL_TEC_CONCAVE;
				edges[active].weight = -f;
				edges[active].worst_vertex = edges[(i + 3) % edge_count].vertex;
			}
		}
	}
	if(edges[active].category == HXA_UTIL_TEC_VALID)
	{
		f = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
		v1[0] /= f;
		v1[1] /= f;
		v1[2] /= f;
		f = sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]);
		v2[0] /= f;
		v2[1] /= f;
		v2[2] /= f;
		edges[active].weight = 1 + v1[0] * -v2[0] + v1[1] * -v2[1] + v1[2] * -v2[2];
	}
}


unsigned int hxa_util_triangulate_ngon(int *ref_ngon, unsigned int *ref_output, unsigned int *origin_output, unsigned int origin_start, unsigned int count, double *vertex, unsigned int vertex_length)
{
	HxATrinagulateEdge edge_buffers[1024], *e;
	unsigned int i, found, output_count = 0, update;

	if(count < 1024)
		e = edge_buffers;
	else
		e = malloc((sizeof *e) * count);
	for(i = 0; i < count - 1; i++)
	{
		e[i].vertex = ref_ngon[i];
		e[i].origin = origin_start + i;
	}	
	e[i].vertex = (unsigned int)(-ref_ngon[i] - 1);
	e[i].origin = origin_start + i;

	for(i = 0; i < count; i++)
		hxa_util_triangulate_ngon_edge_evaluate(e, count, i, vertex);

	while(TRUE)
	{
		found = 0;
		for(i = 1; i < count; i++)
			if(e[i].category < e[found].category || (e[i].category == e[found].category && e[i].weight > e[found].weight))
				found = i;
		ref_output[output_count] = e[found].vertex;
		ref_output[output_count + 1] = e[(found + 1) % count].vertex;
		ref_output[output_count + 2] = -e[(found + 2) % count].vertex - 1;
		if(origin_output != NULL)
		{
			origin_output[output_count] = e[found].origin;
			origin_output[output_count + 1] = e[(found + 1) % count].origin;
			origin_output[output_count + 2] = e[(found + 2) % count].origin;
		}
		output_count += 3;
		update = e[(found + 1) % count].vertex;
		found = (found + 1) % count;
		for(i = found + 1; i < count; i++)
			e[i - 1] = e[i];
		count--;
		if(count == 3)
			break;		
		hxa_util_triangulate_ngon_edge_evaluate(e, count, (found + count - 2) % count, vertex);
		hxa_util_triangulate_ngon_edge_evaluate(e, count, (found + count - 1) % count, vertex);
		for(i = 0; i < count; i++)
			if(e[i].worst_vertex == update)
				hxa_util_triangulate_ngon_edge_evaluate(e, count, i, vertex);
	}
	ref_output[output_count] = e[0].vertex;
	ref_output[output_count + 1] = e[1].vertex;
	ref_output[output_count + 2] = -e[2].vertex - 1;
	if(origin_output != NULL)
	{
		origin_output[output_count] = e[0].origin;
		origin_output[output_count + 1] = e[1].origin;
		origin_output[output_count + 2] = e[2].origin;
	}
	output_count += 3;
	if(e != edge_buffers)
		free(e);
	return output_count;
}

void hxa_util_triangulate_re_ref_layer(HXALayer *layers, unsigned int count, unsigned int *re_ref)
{
	unsigned int i, j, components;
	components = layers->components;
	switch(layers->type)
	{			
		case HXA_LDT_UINT8 : 
		{
			unsigned char *read, *write;
			read = layers->data.uint8_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.uint8_data = write; 
			for(i = 0; i < count; i++)
				for(j = 0; j < components; j++)
					write[i * components + j] = read[re_ref[i] * components + j];
			free(read);
		}
		break;
		case HXA_LDT_INT32 :
		{
			int *read, *write;
			read = layers->data.int32_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.int32_data = write; 
			for(i = 0; i < count; i++)
				for(j = 0; j < components; j++)
					write[i * components + j] = read[re_ref[i] * components + j];
			free(read);
		}
		break;
		case HXA_LDT_FLOAT :
		{
			float *read, *write;
			read = layers->data.float_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.float_data = write; 
			for(i = 0; i < count; i++)
				for(j = 0; j < components; j++)
					write[i * components + j] = read[re_ref[i] * components + j];
			free(read);
		}
		break;
		case HXA_LDT_DOUBLE :
		{
			double *read, *write;
			read = layers->data.double_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.double_data = write; 
			for(i = 0; i < count; i++)
				for(j = 0; j < components; j++)
					write[i * components + j] = read[re_ref[i] * components + j];
			free(read);
		}
		break;
	}
}

extern int hxa_corner_get_next(int *ref, unsigned int corner);

void hxa_util_triangulate_re_ref_edge_layer(HXALayer *layers, unsigned int count, unsigned int *new_ref, unsigned int *old_ref, unsigned int *re_ref)
{
	unsigned int i, j, components;
	int new_next, old_next;
	components = layers->components;
	switch(layers->type)
	{			
		case HXA_LDT_UINT8 : 
		{
			unsigned char *read, *write;
			read = layers->data.uint8_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.uint8_data = write; 
			for(i = 0; i < count; i++)
			{
				old_next = old_ref[hxa_corner_get_next(old_ref, i)];
				new_next = new_ref[hxa_corner_get_next(new_ref, i)];
				if(old_next < 0)
					old_next = -old_next - 1;
				if(new_next < 0)
					new_next = -new_next - 1;
				if(old_next == new_next)
				{
					for(j = 0; j < components; j++)
						write[i * components + j] = read[re_ref[i] * components + j];
				}else
					for(j = 0; j < components; j++)
						write[i * components + j] = 0;
			}
			free(read);
		}
		break;
		case HXA_LDT_INT32 :
		{
			int *read, *write;
			read = layers->data.int32_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.int32_data = write; 
			for(i = 0; i < count; i++)
			{
				old_next = old_ref[hxa_corner_get_next(old_ref, i)];
				new_next = new_ref[hxa_corner_get_next(new_ref, i)];
				if(old_next < 0)
					old_next = -old_next - 1;
				if(new_next < 0)
					new_next = -new_next - 1;
				if(old_next == new_next)
				{
					for(j = 0; j < components; j++)
						write[i * components + j] = read[re_ref[i] * components + j];
				}else
					for(j = 0; j < components; j++)
						write[i * components + j] = 0;
			}
			free(read);
		}
		break;
		case HXA_LDT_FLOAT :
		{
			float *read, *write;
			read = layers->data.float_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.float_data = write; 
			for(i = 0; i < count; i++)
			{
				old_next = old_ref[hxa_corner_get_next(old_ref, i)];
				new_next = new_ref[hxa_corner_get_next(new_ref, i)];
				if(old_next < 0)
					old_next = -old_next - 1;
				if(new_next < 0)
					new_next = -new_next - 1;
				if(old_next == new_next)
				{
					for(j = 0; j < components; j++)
						write[i * components + j] = read[re_ref[i] * components + j];
				}else
					for(j = 0; j < components; j++)
						write[i * components + j] = 0;
			}
			free(read);
		}
		break;
		case HXA_LDT_DOUBLE :
		{
			double *read, *write;
			read = layers->data.double_data;
			write = malloc((sizeof *write) * components * count);
			layers->data.double_data = write; 
			for(i = 0; i < count; i++)
			{
				old_next = old_ref[hxa_corner_get_next(old_ref, i)];
				new_next = new_ref[hxa_corner_get_next(new_ref, i)];
				if(old_next < 0)
					old_next = -old_next - 1;
				if(new_next < 0)
					new_next = -new_next - 1;
				if(old_next == new_next)
				{
					for(j = 0; j < components; j++)
						write[i * components + j] = read[re_ref[i] * components + j];
				}else
					for(j = 0; j < components; j++)
						write[i * components + j] = 0;
			}
			free(read);
		}
		break;
	}
}

void hxa_util_triangulate_node(HXANode *node, unsigned int max_sides)
{
	unsigned int i, j, k, poly_count, ref_count, sides, *corner_reference, *poly_reference, count;
	double *vertex;
	float *vertex_f = NULL;
	int *ref, *new_ref;
	if(node->type != HXA_NT_GEOMETRY)
		return;
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
	{
		vertex_f = node->content.geometry.vertex_stack.layers[0].data.float_data;
		count = node->content.geometry.vertex_count * 3;
		vertex = malloc((sizeof *vertex) * count);
		for(i = 0; i < count; i++)
			vertex[i] = (double)vertex_f[i];
	}else
		vertex = node->content.geometry.vertex_stack.layers[0].data.double_data;
	ref = node->content.geometry.corner_stack.layers[0].data.int32_data;
	poly_count = ref_count = 0;
	for(i = 0; i < node->content.geometry.edge_corner_count; i += sides)
	{
		for(sides = 0; ref[i + sides] >= 0; sides++);
		sides++;
		if(sides > max_sides)
		{
			poly_count += sides + 1 - 3;
			ref_count += (sides + 1 - 3) * 3;
		}else
		{
			poly_count++;
			ref_count += sides;
		}
	}
	corner_reference = malloc((sizeof *corner_reference) * ref_count);
	new_ref = malloc((sizeof *new_ref) * ref_count);
	poly_reference = malloc((sizeof *poly_reference) * poly_count);
	poly_count = ref_count = 0;

	count = node->content.geometry.vertex_count;
	for(i = k = 0; i < node->content.geometry.edge_corner_count; i += sides)
	{
		for(sides = 0; ref[i + sides] >= 0; sides++);
		sides++;
		if(sides > max_sides)
		{
			hxa_util_triangulate_ngon(&ref[i], &new_ref[ref_count], &corner_reference[ref_count], i, sides, vertex, count);
			ref_count += (sides + 1 - 3) * 3;
			for(j = max_sides - 1; j < sides; j++)
				poly_reference[poly_count++] = k;
		}else
		{
			for(j = 0; j < sides; j++)
			{
				new_ref[ref_count] = ref[i + j];
				corner_reference[ref_count++] = i + j;
			}
			poly_reference[poly_count++] = k;
		}
		k++;
	}
	node->content.geometry.corner_stack.layers[0].data.int32_data = new_ref;
	for(i = 0; i < ref_count; i++)
		printf("corner_reference[%u] = %u\n", i, corner_reference[i]);
	for(i = 1; i < node->content.geometry.corner_stack.layer_count; i++)
		hxa_util_triangulate_re_ref_layer(&node->content.geometry.corner_stack.layers[i], ref_count, corner_reference);
	for(i = 0; i < node->content.geometry.edge_stack.layer_count; i++)
		hxa_util_triangulate_re_ref_edge_layer(&node->content.geometry.edge_stack.layers[i], ref_count, new_ref, ref, corner_reference);
	for(i = 0; i < node->content.geometry.face_stack.layer_count; i++)
		hxa_util_triangulate_re_ref_layer(&node->content.geometry.face_stack.layers[i], poly_count, poly_reference);
	node->content.geometry.face_count = poly_count;
	node->content.geometry.edge_corner_count = ref_count;
	free(ref);
	free(corner_reference);
	free(poly_reference);
	if(vertex_f != NULL)
		free(vertex);
}