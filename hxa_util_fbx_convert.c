#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hxa.h"
#include "hxa_util_fbx_internal.h"

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint;
typedef float real32;
typedef double real64;
typedef unsigned char boolean;
#define FALSE 0
#define TRUE !FALSE

FBXRecord *hxa_fbx_convert_record_find(FBXRecord *record, char *name, unsigned int start, unsigned int prefix)
{
	FBXRecord *child;
	unsigned int i, j, k;
	for(i = k = 0; i < record->used; i++)
	{
		child = &((FBXRecord *)record->children)[i];
//		if(prefix)
//			printf("child->name %s\n", child->name);
		for(j = 0; name[j] != 0 && name[j] == child->name[j]; j++);
		if(name[j] == 0 && (prefix || child->name[j] == 0))
		{
			if(start == k++)
				return child;
		}
	}
	return NULL;
}

int hxa_fbx_convert_property_test(FBXRecord *record, char *property_name)
{
	FBXProperty *p;
	uint i, j;
	for(i = 0; i < record->property_count; i++)
	{
		if(record->property[i].type == FBX_PROPERTY_TYPE_TEXT)
		{
			for(j = 0; property_name[j] != 0 && record->property[i].data.buffer->buffer.text[j] == property_name[j]; j++);
			if(property_name[j] == record->property[i].data.buffer->buffer.text[j])
				return TRUE;
		}
	}
	return FALSE;
}

int hxa_fbx_convert_layer_read(FBXRecord *record, HXALayerStack *layer_stack, uint length)
{
	char *conversion_from[] = {"Normals", "Binormals", "Tangents", "Colors", "UV", "Smoothing", "Material", "PolygonGroup", "EdgeCrease"};
	char *conversion_to[] = {HXA_CONVENTION_SOFT_LAYER_NORMALS, HXA_CONVENTION_SOFT_LAYER_BINORMAL, HXA_CONVENTION_SOFT_LAYER_TANGENT, HXA_CONVENTION_SOFT_LAYER_COLOR, HXA_CONVENTION_SOFT_LAYER_UV0, "smoothing", HXA_CONVENTION_SOFT_LAYER_MATERIAL_ID, "group", HXA_CONVENTION_SOFT_LAYER_CREASES};
	char *index = "Index";
	HXALayer *layer;
	FBXProperty *p, *index_property = NULL;
	FBXRecord *param;
	uint i, j, k, l, m, type_size, data_size, vertex_range, components;
	unsigned char *data = NULL, *temp_buff = NULL;
	size_t type_sizes[HXA_LDT_COUNT] = {sizeof(unsigned char), sizeof(int), sizeof(float), sizeof(double)};
	unsigned int fbx_type_convesion[FBX_PROPERTY_TYPE_COUNT] = {HXA_LDT_COUNT, // FBX_PROPERTY_TYPE_BOOLEAN,
																HXA_LDT_INT32, // FBX_PROPERTY_TYPE_INT8,
																HXA_LDT_INT32, // FBX_PROPERTY_TYPE_INT16,
																HXA_LDT_INT32, // FBX_PROPERTY_TYPE_INT32,
																HXA_LDT_INT32, // FBX_PROPERTY_TYPE_INT64,
																HXA_LDT_FLOAT, // FBX_PROPERTY_TYPE_FLOAT32,
																HXA_LDT_DOUBLE, // FBX_PROPERTY_TYPE_FLOAT64,
																HXA_LDT_COUNT, // FBX_PROPERTY_TYPE_TEXT,
																HXA_LDT_COUNT, // FBX_PROPERTY_TYPE_RAW,
																HXA_LDT_COUNT}; // FBX_PROPERTY_TYPE_UNKNOWN,



	param = hxa_fbx_convert_record_find(record, "ReferenceInformationType", 0, FALSE);

	if(param != NULL && (hxa_fbx_convert_property_test(param, "IndexToDirect") || hxa_fbx_convert_property_test(param, "Index")))
	{
		printf("IndexToDirect/n");
		for(i = 0; i < record->used && index_property == NULL; i++)
		{
			param = &((FBXRecord *)record->children)[i];
			for(j = 0; j < param->property_count && index_property == NULL; j++)
			{
				if(param->property[j].array_length % length == 0)
				{
					if(param->property[j].type == FBX_PROPERTY_TYPE_INT8 ||
						param->property[j].type == FBX_PROPERTY_TYPE_INT16 ||
						param->property[j].type == FBX_PROPERTY_TYPE_INT32 ||
						param->property[j].type == FBX_PROPERTY_TYPE_INT64)
					{
						for(k = 0; k < param->name[k] != 0; k++)
						{
							for(l = 0; index[l] != 0 && index[l] == param->name[k + l]; l++);
							if(index[l] != 0)
							{
								index_property = &param->property[j];
								vertex_range = 0;
								if(index_property->type == FBX_PROPERTY_TYPE_INT8)
									for(k = 0; k < index_property->array_length; k++)
										if(vertex_range < ((char *)index_property->data.array)[k])
											vertex_range = ((char *)index_property->data.array)[k];
								if(index_property->type == FBX_PROPERTY_TYPE_INT16)
									for(k = 0; k < index_property->array_length; k++)
										if(vertex_range < ((short *)index_property->data.array)[k])
											vertex_range = ((short *)index_property->data.array)[k];
								if(index_property->type == FBX_PROPERTY_TYPE_INT32)
									for(k = 0; k < index_property->array_length; k++)
										if(vertex_range < ((int *)index_property->data.array)[k])
											vertex_range = ((int *)index_property->data.array)[k];
								if(index_property->type == FBX_PROPERTY_TYPE_INT64)
									for(k = 0; k < index_property->array_length; k++)
										if(vertex_range < ((long long *)index_property->data.array)[k])
											vertex_range = ((long long *)index_property->data.array)[k];
								vertex_range++;
								break;
							}
						}
					}
				}
			}
		}
		if(index_property == NULL)
			return FALSE;
	}else
		vertex_range = length;


	for(i = 0; data == NULL && i < record->used; i++)
	{
		param = &((FBXRecord *)record->children)[i];
		for(j = 0; j < param->property_count; j++)
		{
			if(&param->property[j] != index_property && 
				param->property[j].array_length >= vertex_range && 
				fbx_type_convesion[param->property[j].type] != HXA_LDT_COUNT)
			{

				data_size = param->property[j].array_length;
				data = param->property[j].data.array;
				++layer_stack->layer_count;
				layer_stack->layers = realloc(layer_stack->layers, (sizeof *layer_stack->layers) * layer_stack->layer_count);
				layer = &layer_stack->layers[layer_stack->layer_count - 1];	

				for(l = 0; l < 9; l++)
				{
					for(m = 0; conversion_from[l][m] == param->name[m] && param->name[m] != 0; m++);
					if(conversion_from[l][m] == param->name[m])
					{
						for(m = 0; conversion_to[l][m] != 0; m++)
							layer->name[m] = conversion_to[l][m];
						layer->name[m] = 0;
						break;
					}
				}
				if(l == 9)
				{
					for(k = 0; param->name[k] != 0; k++)
						layer->name[k] = param->name[k];		
					layer->name[k] = 0;
				}
				if(layer->name[0] == 'U' && layer->name[1] == 'V')
					k = 0;
				layer->type = fbx_type_convesion[param->property[j].type];
				type_size = type_sizes[layer->type];
				if(index_property == NULL)
					layer->components = param->property[j].array_length / length;
				else
					layer->components = param->property[j].array_length / vertex_range;
				layer->data.uint8_data = malloc(type_size * length * layer->components);
			
				if(param->property[j].type == FBX_PROPERTY_TYPE_INT8)
				{
					temp_buff = malloc((sizeof *temp_buff) * data_size);
					for(k = 0; k < 0; k++)
						temp_buff[k] = (int)((char *)param->property[j].data.array)[k];
				}else if(param->property[j].type == FBX_PROPERTY_TYPE_INT16)
				{
					temp_buff = malloc((sizeof *temp_buff) * data_size);
					for(k = 0; k < 0; k++)
						temp_buff[k] = (int)((short *)param->property[j].data.array)[k];
				}else if(param->property[j].type == FBX_PROPERTY_TYPE_INT64)
				{
					temp_buff = malloc((sizeof *temp_buff) * data_size);
					for(k = 0; k < 0; k++)
						temp_buff[k] = (int)((long long *)param->property[j].data.array)[k];
				}
				break;
			}
		}
	}
	if(data == NULL)
		return FALSE;

	
	if(index_property != NULL)
	{
		layer->data.uint8_data = malloc((sizeof *layer->data.uint8_data) * layer->components * length * type_size);
		if(index_property->type == FBX_PROPERTY_TYPE_INT8)
			for(k = 0; k < index_property->array_length; k++)
				memcpy(&layer->data.uint8_data[layer->components * k * type_size], &data[layer->components * (size_t)((char *)index_property->data.array)[k] * sizeof(char)], layer->components * type_size);
		if(index_property->type == FBX_PROPERTY_TYPE_INT16)
			for(k = 0; k < index_property->array_length; k++)
				memcpy(&layer->data.uint8_data[layer->components * k * type_size], &data[layer->components * (size_t)((short *)index_property->data.array)[k] * sizeof(short)], layer->components * type_size);
		if(index_property->type == FBX_PROPERTY_TYPE_INT32)
			for(k = 0; k < index_property->array_length; k++)
				memcpy(&layer->data.uint8_data[layer->components * k * type_size], &data[layer->components * (size_t)((int *)index_property->data.array)[k] * sizeof(int)], layer->components * type_size);
		if(index_property->type == FBX_PROPERTY_TYPE_INT64)
			for(k = 0; k < index_property->array_length; k++)
				memcpy(&layer->data.uint8_data[layer->components * k * type_size], &data[layer->components * (size_t)((long long *)index_property->data.array)[k] * sizeof(long long)], layer->components * type_size);
	}else
		memcpy(layer->data.uint8_data, data, type_size * length * layer->components);
	if(temp_buff != NULL)
		free(temp_buff);
	return FALSE;
}

extern void fbx_record_print_small(FBXRecord *record, uint generation);

HXAFile *hxa_fbx_convert(FBXRecord *record, HXAFile *file)
{
	FBXRecord *object, *geometry, *vertices, *polygonvertexindex, *layer, *param;
	HXALayerStack *stack;
	HXANode *node;
	unsigned int i, j, k, *ref, array_length;
	object = hxa_fbx_convert_record_find(record, "Objects", 0, FALSE);
	if(object == NULL)
		return NULL;
	if(file == NULL)
	{
		file = malloc(sizeof *file);
		file->version = HXA_VERSION_FORMAT;
		file->node_count = 0;
		file->node_array = NULL;
	}
	for(i = 0; (geometry = hxa_fbx_convert_record_find(object, "Geometry", i, FALSE)) != NULL; i++)
	{
		fbx_record_print(geometry, 1);
		vertices = hxa_fbx_convert_record_find(geometry, "Vertices", 0, FALSE);
		polygonvertexindex = hxa_fbx_convert_record_find(geometry, "PolygonVertexIndex", 0, FALSE);
		if(vertices != NULL && polygonvertexindex != NULL)
		{

			file->node_array = realloc(file->node_array, (sizeof *file->node_array) * ++file->node_count);
			node = &file->node_array[file->node_count - 1];
			node->meta_data_count = 0;
			node->meta_data = NULL;
			node->type = HXA_NT_GEOMETRY;
			
			node->content.geometry.vertex_stack.layer_count = 1;
			node->content.geometry.vertex_stack.layers = malloc(sizeof *node->content.geometry.vertex_stack.layers);
			node->content.geometry.vertex_stack.layers->components = HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_COMPONENTS;
			node->content.geometry.vertex_count = vertices->property->array_length / 3;
			sprintf(node->content.geometry.vertex_stack.layers->name, HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME);
			if(vertices->property->type == FBX_PROPERTY_TYPE_FLOAT32)
			{				
				node->content.geometry.vertex_stack.layers->type = HXA_LDT_FLOAT;
				node->content.geometry.vertex_stack.layers->data.float_data = malloc((sizeof *node->content.geometry.vertex_stack.layers->data.float_data) * vertices->property->array_length);
				memcpy(node->content.geometry.vertex_stack.layers->data.float_data, vertices->property->data.array, (sizeof *node->content.geometry.vertex_stack.layers->data.float_data) * vertices->property->array_length);
			}
			if(vertices->property->type == FBX_PROPERTY_TYPE_FLOAT64)
			{
				node->content.geometry.vertex_stack.layers->type = HXA_LDT_DOUBLE;
				node->content.geometry.vertex_stack.layers->data.double_data = malloc((sizeof *node->content.geometry.vertex_stack.layers->data.double_data) * vertices->property->array_length);
				memcpy(node->content.geometry.vertex_stack.layers->data.double_data, vertices->property->data.array, (sizeof *node->content.geometry.vertex_stack.layers->data.double_data) * vertices->property->array_length);

			}

			node->content.geometry.corner_stack.layer_count = 1;
			node->content.geometry.corner_stack.layers = malloc(sizeof *node->content.geometry.corner_stack.layers);
			node->content.geometry.corner_stack.layers->data.int32_data = malloc((sizeof *node->content.geometry.vertex_stack.layers->data.int32_data) * polygonvertexindex->property->array_length);
			memcpy(node->content.geometry.corner_stack.layers->data.int32_data, polygonvertexindex->property->data.array, (sizeof *node->content.geometry.vertex_stack.layers->data.int32_data) * polygonvertexindex->property->array_length);
			node->content.geometry.edge_corner_count = polygonvertexindex->property->array_length;
			sprintf(node->content.geometry.corner_stack.layers->name, HXA_CONVENTION_HARD_BASE_CORNER_LAYER_NAME);
			node->content.geometry.corner_stack.layers->components = HXA_CONVENTION_HARD_BASE_CORNER_LAYER_COMPONENTS;
			node->content.geometry.corner_stack.layers->type = HXA_CONVENTION_HARD_BASE_CORNER_LAYER_TYPE;
			node->content.geometry.edge_stack.layers = 0;
			node->content.geometry.edge_stack.layer_count = 0;
			node->content.geometry.face_stack.layers = 0;
			node->content.geometry.face_stack.layer_count = 0;
			node->content.geometry.face_count = 0;
			for(j = 0; j < node->content.geometry.edge_corner_count; j++)
				if(node->content.geometry.corner_stack.layers->data.int32_data[j] < 0)
					node->content.geometry.face_count++;

			j = 0;
			for(layer = hxa_fbx_convert_record_find(geometry, "LayerElement", j++, TRUE); layer != NULL; layer = hxa_fbx_convert_record_find(geometry, "LayerElement", j++, TRUE))
			{
				printf("Layers found %u %s\n", j, layer->name);
				ref = NULL;
				param = hxa_fbx_convert_record_find(layer, "MappingInformationType", 0, FALSE);
				if(param != NULL)
				{
					if(hxa_fbx_convert_property_test(param, "ByPolygonVertex")) // Corner layer
					{
						hxa_fbx_convert_layer_read(layer, &node->content.geometry.corner_stack, node->content.geometry.edge_corner_count);
					}else if(hxa_fbx_convert_property_test(param, "ByVertice")) // Vertex Layer
					{
						hxa_fbx_convert_layer_read(layer, &node->content.geometry.vertex_stack, node->content.geometry.vertex_count);
					}else if(hxa_fbx_convert_property_test(param, "ByPolygon")) // Polygon Layer
					{
						hxa_fbx_convert_layer_read(layer, &node->content.geometry.face_stack, node->content.geometry.face_count);
					}else if(hxa_fbx_convert_property_test(param, "ByEdge")) // EdgeLayer
					{
						hxa_fbx_convert_layer_read(layer, &node->content.geometry.edge_stack, node->content.geometry.edge_corner_count);
					}
				}
				

			}
			j = 0;
		}
	}
	return file;

}