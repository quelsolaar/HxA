#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE


/* use include and defines to creat float and double versions of the normal functions. */
#define NORMAL_FACE
#define HXATYPE_DATA float_data
#define HXATYPE float
#define hxa_util_normal_face_macro hxa_util_normal_facef
#include "hxa_util_normals.h"
#define HXATYPE_DATA double_data
#define HXATYPE double
#define hxa_util_normal_face_macro hxa_util_normal_faced
#include "hxa_util_normals.h"
#undef NORMAL_FACE
#define NORMAL_CORNER

#define HXATYPE_DATA float_data
#define HXATYPE float
#define hxa_util_normal_corner_macro hxa_util_normal_cornerf
#include "hxa_util_normals.h"
#define HXATYPE_DATA double_data
#define HXATYPE double
#define hxa_util_normal_corner_macro hxa_util_normal_cornerd
#include "hxa_util_normals.h"
#define HXATYPE_DATA float_data
#define HXATYPE float
#define HXA_CREASE_TYPE unsigned char
#define hxa_util_normal_corner_macro hxa_util_normal_cornerf_u8
#include "hxa_util_normals.h"
#define HXA_CREASE_TYPE int
#define hxa_util_normal_corner_macro hxa_util_normal_cornerf_i32
#include "hxa_util_normals.h"
#define HXA_CREASE_TYPE float
#define hxa_util_normal_corner_macro hxa_util_normal_cornerf_f
#include "hxa_util_normals.h"
#define HXA_CREASE_TYPE double
#define hxa_util_normal_corner_macro hxa_util_normal_cornerf_d
#include "hxa_util_normals.h"
#define HXATYPE_DATA double_data
#define HXATYPE double
#define HXA_CREASE_TYPE unsigned char
#define hxa_util_normal_corner_macro hxa_util_normal_cornerd_u8
#include "hxa_util_normals.h"
#define HXA_CREASE_TYPE int
#define hxa_util_normal_corner_macro hxa_util_normal_cornerd_i32
#include "hxa_util_normals.h"
#define HXA_CREASE_TYPE float
#define hxa_util_normal_corner_macro hxa_util_normal_cornerd_f
#include "hxa_util_normals.h"
#define HXA_CREASE_TYPE double
#define hxa_util_normal_corner_macro hxa_util_normal_cornerd_d
#include "hxa_util_normals.h"



void hxa_util_normal_face(HXANode *node)
{
	HXALayer *layer;
	node->content.geometry.face_stack.layer_count++;
	node->content.geometry.face_stack.layers = realloc(node->content.geometry.face_stack.layers, (sizeof *node->content.geometry.face_stack.layers) * node->content.geometry.face_stack.layer_count);
	layer = &node->content.geometry.face_stack.layers[node->content.geometry.face_stack.layer_count - 1];
	layer->type = node->content.geometry.vertex_stack.layers[0].type;
	layer->components = 3;
	sprintf(layer->name, HXA_CONVENTION_SOFT_LAYER_NORMALS);
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
		layer->data.float_data = hxa_util_normal_facef(node);
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
		layer->data.double_data = hxa_util_normal_faced(node);
}

void hxa_util_normal_corner(HXANode *node)
{
	char *crease_name = HXA_CONVENTION_SOFT_LAYER_CREASES;
	HXALayer *layer;
	unsigned int i, j, *neighbour, *face_ref;
	int *ref;
	void *face_data;
	node->content.geometry.corner_stack.layer_count++;
	node->content.geometry.corner_stack.layers = realloc(node->content.geometry.corner_stack.layers, (sizeof *node->content.geometry.corner_stack.layers) * node->content.geometry.corner_stack.layer_count);
	layer = &node->content.geometry.corner_stack.layers[node->content.geometry.corner_stack.layer_count - 1];
	layer->type = node->content.geometry.vertex_stack.layers[0].type;
	layer->components = 3;
	sprintf(layer->name, HXA_CONVENTION_SOFT_LAYER_NORMALS);
	face_ref = malloc((sizeof *face_ref) * node->content.geometry.edge_corner_count);
	ref = node->content.geometry.corner_stack.layers[0].data.int32_data;
	for(i = j = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		face_ref[i] = j;
		if(ref[i] < 0)
			j++;
	}
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
		face_data = hxa_util_normal_facef(node);
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
		face_data = hxa_util_normal_faced(node);
	neighbour = hxa_neighbour_node(node);
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
		if(neighbour[i] < node->content.geometry.edge_corner_count && neighbour[neighbour[i]] != i)
			i += 0;
	for(i = 0; i < node->content.geometry.edge_stack.layer_count; i++)
	{
		for(j = 0; node->content.geometry.edge_stack.layers[i].name[j] == crease_name[j] && crease_name[j] != 0; j++);
		if(node->content.geometry.edge_stack.layers[i].name[j] == crease_name[j])
		{
			switch(node->content.geometry.edge_stack.layers[i].type)
			{
				case HXA_LDT_UINT8 :
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
						layer->data.float_data = hxa_util_normal_cornerf_u8(node, neighbour, node->content.geometry.edge_stack.layers[i].data.uint8_data, (float *)face_data, face_ref);
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
						layer->data.double_data = hxa_util_normal_cornerd_u8(node, neighbour, node->content.geometry.edge_stack.layers[i].data.uint8_data, (double *)face_data, face_ref);
				break;
				case HXA_LDT_INT32 :
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
						layer->data.float_data = hxa_util_normal_cornerf_i32(node, neighbour, node->content.geometry.edge_stack.layers[i].data.int32_data, (float *)face_data, face_ref);
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
						layer->data.double_data = hxa_util_normal_cornerd_i32(node, neighbour, node->content.geometry.edge_stack.layers[i].data.int32_data, (double *)face_data, face_ref);
				break;
				case HXA_LDT_FLOAT :
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
						layer->data.float_data = hxa_util_normal_cornerf_f(node, neighbour, node->content.geometry.edge_stack.layers[i].data.float_data, (float *)face_data, face_ref);
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
						layer->data.double_data = hxa_util_normal_cornerd_f(node, neighbour, node->content.geometry.edge_stack.layers[i].data.float_data, (double *)face_data, face_ref);
				break;
				case HXA_LDT_DOUBLE :
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
						layer->data.float_data = hxa_util_normal_cornerf_d(node, neighbour, node->content.geometry.edge_stack.layers[i].data.double_data, (float *)face_data, face_ref);
					if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
						layer->data.double_data = hxa_util_normal_cornerd_d(node, neighbour, node->content.geometry.edge_stack.layers[i].data.double_data, (double *)face_data, face_ref);
				break;
			}
			free(face_ref);
			free(face_data);
			return;
		}
	}
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_FLOAT)
		layer->data.float_data = hxa_util_normal_cornerf(node, neighbour,  (float *)face_data, face_ref);
	if(node->content.geometry.vertex_stack.layers[0].type == HXA_LDT_DOUBLE)
		layer->data.double_data = hxa_util_normal_cornerd(node, neighbour, (double *)face_data, face_ref);
	free(face_ref);
	free(face_data);
}