#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE

int hxa_util_validate_meta(HXAMeta *meta, unsigned int node, unsigned int count, int silent)
{
	unsigned int i;
	if(meta->type == HXA_MDT_NODE)
	{
		for(i = 0; meta->array_length; i++)
		{
			if(meta->value.node_value[i] >= count)
			{
				if(!silent)
					printf("HxA Verify Error: Node %u has meta data %s that is referencing a non existant node (%u out of %u)\n", node, meta->name, meta->value.node_value[i], count);
				return FALSE;
			}
		}	
	}
	if(meta->type == HXA_MDT_META)
		for(i = 0; meta->array_length; i++)
			if(!hxa_util_validate_meta(&((HXAMeta *)meta->value.node_value)[i], node, count, silent))
				return FALSE;
	return TRUE;
}

int hxa_util_validate(HXAFile *file, int silent)
{
	char *layer_type_names[4] = {"HXA_LDT_UINT8", "HXA_LDT_INT32", "HXA_LDT_FLOAT", "HXA_LDT_DOUBLE"}, *test;
	unsigned int i, j, poly_count, reference;
	int *ref;
	HXANode *node;
	node = file->node_array;
	for(i = 0; i < file->node_count; i++)
	{
		for(i = 0; i < node->meta_data_count; i++)
			if(!hxa_util_validate_meta(&node->meta_data[i], i, file->node_count, silent))
				return FALSE;
		switch(node->type)
		{
			case HXA_NT_META_ONLY :
			break;
			case HXA_NT_GEOMETRY :
			if(node->content.geometry.vertex_stack.layer_count == 0)
			{
				if(!silent)
					printf("HxA Verify Error: Node %u has no vertex layer\n", i);
				return FALSE;
			}
			if(node->content.geometry.vertex_stack.layers[0].components != 3)
			{
				if(!silent)
					printf("HxA Verify Error: Node %u vertex layer vertex layer has %u compinents. Must be HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_COMPONENTS (%u).\n", i, node->content.geometry.vertex_stack.layers[0].components, HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_COMPONENTS);
				return FALSE;
			}
			if(node->content.geometry.vertex_stack.layers[0].type != HXA_LDT_FLOAT &&
				node->content.geometry.vertex_stack.layers[0].type != HXA_LDT_DOUBLE)
			{
				if(!silent)
					printf("HxA Verify Error: Node %u first vertex layer is %s, must be HXA_LDT_FLOAT or HXA_LDT_DOUBLE\n", i, layer_type_names[node->content.geometry.vertex_stack.layers[0].type]);
				return FALSE;
			}
			test = HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME;
			for(j = 0; test[j] != 0 && node->content.geometry.vertex_stack.layers[0].name[j] != test[j]; j++);
			if(node->content.geometry.vertex_stack.layers[0].name[j] != test[j])
			{
				if(!silent)
					printf("HxA Verify Error: Node %u vertex layer is named %s. Must be HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME \"%s\".\n", i, node->content.geometry.vertex_stack.layers[0].name, HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME);
				return FALSE;
			}
			if(node->content.geometry.corner_stack.layer_count != 0)
			{
				if(node->content.geometry.corner_stack.layers[0].components != 1)
				{
					if(!silent)
						printf("HxA Verify Error: Node %u reference layer has %u compinents. Must be 1.\n", i, node->content.geometry.corner_stack.layers[0].components);
					return FALSE;
				}
				if(node->content.geometry.corner_stack.layers[0].type != HXA_LDT_INT32)
				{
					if(!silent)
						printf("HxA Verify Error: Node %u reference layer is of type %s, must be HXA_LDT_INT32\n", i, layer_type_names[node->content.geometry.corner_stack.layers[0].type]);
					return FALSE;
				}
				test = HXA_CONVENTION_HARD_BASE_CORNER_LAYER_NAME;
				for(j = 0; test[j] != 0 && node->content.geometry.corner_stack.layers[0].name[j] != test[j]; j++);
				if(node->content.geometry.corner_stack.layers[0].name[j] != test[j])
				{
					if(!silent)
						printf("HxA Verify Error: Node %u reference layer is named %s. Must be HXA_CONVENTION_HARD_BASE_EDGE_CORNER_LAYER_NAME \"%s\".\n", i, node->content.geometry.corner_stack.layers[0].name, HXA_CONVENTION_HARD_BASE_CORNER_LAYER_NAME);
					return FALSE;
				}
				ref = node->content.geometry.corner_stack.layers[0].data.int32_data;
				for(j = poly_count = 0; j < node->content.geometry.edge_corner_count; j++)
				{
					if(ref[j] < 0)
					{
						reference = -ref[j] - 1;
						poly_count++;
					}else
						reference = ref[j];
					if(reference >= node->content.geometry.vertex_count)
					{
						if(!silent)
							printf("HxA Verify Error: Node %u has a reference value referencing a non existing vertex (%u).\n", i, reference);
						return FALSE;
					}
				}
				if(node->content.geometry.face_count != poly_count)
				{
					if(!silent)
						printf("HxA Verify Error: Node %u claims to have %u faces but the reference data has %u faces.\n", i, node->content.geometry.face_count , poly_count);
					return FALSE;
				}
			}
			break;
			case HXA_NT_IMAGE :
			break;
		}
		node++;
	}
	return TRUE;
}



int hxa_util_validate_pow(HXAFile *file, int silent)
{
	unsigned int i, j, k, dim, success = TRUE;
	for(i = 0; i < file->node_count; i++)
	{
		if(file->node_array[i].type == HXA_NT_IMAGE)
		{
			dim = file->node_array[i].content.image.type;
			if(dim == HXA_IT_CUBE_IMAGE)
				dim = 2;
			for(j = 0; j < dim; j++)
			{
				for(k = 1; k < file->node_array[i].content.image.resolution[j]; k *= 2);
				if(k > file->node_array[i].content.image.resolution[j])
				{
					if(!silent)
						printf("HxA Verify Error: Node has non power of two resolution (%u)\n", file->node_array[i].content.image.resolution[j]);
					success = FALSE;
				}
			}	
		}
	}
	return success;
}