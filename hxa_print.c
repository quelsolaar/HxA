#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hxa.h"

#define FALSE 0
#define TRUE !FALSE

extern void hxa_print_meta(HXAMeta *meta, unsigned int tab_count, unsigned int data);

void hxa_print_meta_data(HXAMeta *meta, unsigned int length, unsigned int tab_count, unsigned int data)
{
	unsigned int i;
	switch(meta->type)
	{
		case HXA_MDT_INT64 :
			for(i = 0; i < length; i++)
				printf(" %lli", meta->value.int64_value[i]);
		break; 
		case HXA_MDT_DOUBLE :
			for(i = 0; i < length; i++)
				printf(" %f", meta->value.double_value[i]);
		break; 
		case HXA_MDT_NODE :
			for(i = 0; i < length; i++)
				printf( "%i", meta->value.node_value[i]);
		break; 
		case HXA_MDT_TEXT :			
			printf(" %s", meta->value.text_value);
		break; 
		case HXA_MDT_BINARY :
			for(i = 0; i < length; i++)
				printf(" %x", meta->value.bin_value[i]);
		break; 
		case HXA_MDT_META :
			printf("\n");
			for(i = 0; i < meta->array_length; i++)
				hxa_print_meta(&((HXAMeta *)meta->value.array_of_meta)[i], tab_count + 1, data);
		break; 
	}	
}

void hxa_print_meta_meta_find(HXAMeta *meta, char *key)
{
	unsigned int i, j;
	if(strcmp(meta->name, key) == 0)
		hxa_print_meta_data(meta, meta->array_length, 0, TRUE);
	if(meta->type == HXA_MDT_META)
		for(i = 0; i < meta->array_length; i++)
			hxa_print_meta_meta_find(&((HXAMeta *)meta->value.array_of_meta)[i], key);
}

void hxa_print_meta_find(HXAFile *file, char *key)
{
	unsigned int i, j;
	for(i = 0; i < file->node_count; i++)
		for(j = 0; j < file->node_array[i].meta_data_count; j++)
			hxa_print_meta_meta_find(&file->node_array[i].meta_data[j], key);
}

void hxa_print_meta(HXAMeta *meta, unsigned int tab_count, unsigned int data)
{
	char *type_name[] = {"HXA_MDT_INT64", "HXA_MDT_DOUBLE", "HXA_MDT_NODE", "HXA_MDT_TEXT", "HXA_MDT_BINARY", "HXA_MDT_META"};
	char tabs[16] = {'\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', '\t', 0};
	unsigned int i, length;
	if(tab_count < 15)
		tabs[tab_count] = 0;
	if(meta->array_length == 1)
		printf("%s-Meta %s \"%s\" :", tabs, type_name[meta->type], meta->name);
	else
		printf("%s-Meta %s \"%s\"[%u] :", tabs, type_name[meta->type], meta->name, meta->array_length);
	length = meta->array_length;
	if(!data && length > 16)
		length = 16;
	hxa_print_meta_data(meta, length, tab_count, data);
	if(meta->type != HXA_MDT_META)
	{
		if(length < meta->array_length && meta->type != HXA_MDT_TEXT)
			printf(" ...\n");
		else 
			printf("\n");
	}		
}

void hxa_print_layer_stack(HXALayerStack *stack, char *name, int data, unsigned int count)
{
	char *type_names[] = {"HXA_LDT_UINT8", "HXA_LDT_INT32", "HXA_LDT_FLOAT", "HXA_LDT_DOUBLE"};
	unsigned int i, j, k;
	printf("\t-%s layer count: %u\n", name, stack->layer_count);
	for(i = 0; i < stack->layer_count; i++)
	{
		printf("\t\tLayer name: %s\n", stack->layers[i].name);
		printf("\t\tLayer components: %u\n", (unsigned int)stack->layers[i].components);
		printf("\t\tLayer type: %s\n", type_names[stack->layers[i].type]);
		if(data)
		{
			switch(stack->layers[i].type)
			{
				case HXA_LDT_UINT8 :
					for(j = 0; j < count; j++)
					{
						printf("\t\t\t");
						for(k = 0; k < stack->layers[i].components; k++)
							printf("%u ", (unsigned int)stack->layers[i].data.uint8_data[j * stack->layers[i].components + k]);
						printf("\n");
					}
				break;
				case HXA_LDT_INT32 :
					if(i == 0 && strcmp(stack->layers[i].name, "reference") == 0 && stack->layers[i].components == 1)
					{
						printf("\t\t\t");
						for(j = 0; j < count; j++)
						{							
							printf("%i ", stack->layers[i].data.int32_data[j]);
							if(stack->layers[i].data.int32_data[j] < 0)
							{
								printf("\n");
								if(j + 1 < count)
									printf("\t\t\t");
							}
						}
					}else
					{
						for(j = 0; j < count; j++)
						{
							printf("\t\t\t");
							for(k = 0; k < stack->layers[i].components; k++)
								printf("%i ", stack->layers[i].data.int32_data[j * stack->layers[i].components + k]);
							printf("\n");
						}
					}
				break;
				case HXA_LDT_FLOAT :
					for(j = 0; j < count; j++)
					{
						printf("\t\t\t");
						for(k = 0; k < stack->layers[i].components; k++)
							printf("%f ", stack->layers[i].data.float_data[j * stack->layers[i].components + k]);
						printf("\n");
					}
				break;
				case HXA_LDT_DOUBLE :
					for(j = 0; j < count; j++)
					{
						printf("\t\t\t");
						for(k = 0; k < stack->layers[i].components; k++)
							printf("%f ", stack->layers[i].data.double_data[j * stack->layers[i].components + k]);
						printf("\n");
					}
				break;
			}
		}
	}
}

void hxa_print(HXAFile *file, int data)
{
	char *node_type_names[3] = {"HXA_NT_META_ONLY", "HXA_NT_GEOMETRY", "HXA_NT_IMAGE"};
	unsigned int i, j;
	printf("File version: %u\n", file->version);
	printf("Node count: %u\n", file->node_count);
	for(i = 0; i < file->node_count; i++)
	{	
		printf("-Node id: %u\n", i);
		printf("\t-Node type: %s\n", node_type_names[file->node_array[i].type]);
		printf("\t-Node meta count: %u\n", file->node_array[i].meta_data_count);
		for(j = 0; j < file->node_array[i].meta_data_count; j++)
			hxa_print_meta(&file->node_array[i].meta_data[j], 2, data);
		switch(file->node_array[i].type)
		{
			case HXA_NT_GEOMETRY :
				printf("\t-Geometry vertex count: %u\n", file->node_array[i].content.geometry.vertex_count);
				hxa_print_layer_stack(&file->node_array[i].content.geometry.vertex_stack, "Vertex", data, file->node_array[i].content.geometry.vertex_count);
				printf("\t-Geometry edge count: %u\n", file->node_array[i].content.geometry.edge_corner_count);
				hxa_print_layer_stack(&file->node_array[i].content.geometry.corner_stack, "Corner", data, file->node_array[i].content.geometry.edge_corner_count);
				hxa_print_layer_stack(&file->node_array[i].content.geometry.edge_stack, "Edge", data, file->node_array[i].content.geometry.edge_corner_count);
				printf("\t-Geometry face count: %u\n", file->node_array[i].content.geometry.face_count);
				hxa_print_layer_stack(&file->node_array[i].content.geometry.face_stack, "Face", data, file->node_array[i].content.geometry.face_count);
			break;
			case HXA_NT_IMAGE :
				switch(file->node_array[i].content.image.type)
				{
					case HXA_IT_CUBE_IMAGE :
						printf("\t-Pixel type: HXA_IT_CUBE_IMAGE\n");
						printf("\t-Pixel resolution: %u x %u x 6\n", file->node_array[i].content.image.resolution[0], file->node_array[i].content.image.resolution[1]);
					break;
					case HXA_IT_1D_IMAGE :
						printf("\t-Pixel type: HXA_IT_1D_IMAGE\n");
						printf("\t-Pixel resolution: %u\n", file->node_array[i].content.image.resolution[0]);
					break;
					case HXA_IT_2D_IMAGE :
						printf("\t-Pixel type: HXA_IT_2D_IMAGE\n");
						printf("\t-Pixel resolution: %u x %u\n", file->node_array[i].content.image.resolution[0], file->node_array[i].content.image.resolution[1]);
					break;
					case HXA_IT_3D_IMAGE :
						printf("\t-Pixel type: HXA_IT_3D_IMAGE\n");
						printf("\t-Pixel resolution: %u x %u x %u\n", file->node_array[i].content.image.resolution[0], file->node_array[i].content.image.resolution[1], file->node_array[i].content.image.resolution[2]);
					break;
				}
			break;
		}
	}
}