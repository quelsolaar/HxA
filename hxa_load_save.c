#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "hxa.h"

#define FALSE 0
#define TRUE !FALSE

void hxa_util_free_stack(HXALayerStack *stack)
{
	unsigned int i;
	for(i = 0; i < stack->layer_count; i++)
		free(stack->layers[i].data.uint8_data);
	if(stack->layer_count != 0)
		free(stack->layers);
}

void hxa_util_free_meta(HXAMeta *meta)
{
	unsigned int i;
	switch(meta->type)
	{
		case HXA_MDT_INT64 :
			free(meta->value.int64_value);
		break;
		case HXA_MDT_DOUBLE :
			free(meta->value.double_value);
		break;
		case HXA_MDT_NODE :
			free(meta->value.node_value);
		break;
		case HXA_MDT_TEXT :
			for(i = 0; i < meta->array_length; i++)
				free(meta->value.text_value[i]);
			free(meta->value.text_value);
		break;
		case HXA_MDT_META :
			for(i = 0; i < meta->array_length; i++)
				hxa_util_free_meta(&(((HXAMeta *)meta->value.array_of_meta)[i]));
			free(meta->value.array_of_meta);
		break;
	}
}

void hxa_util_free_node_content(HXANode *node)
{
	unsigned int i;
	for(i = 0; i < node->meta_data_count; i++)
		hxa_util_free_meta(&node->meta_data[i]);
	if(node->meta_data_count != 0)
		free(node->meta_data);
	switch(node->type)
	{
		case HXA_NT_GEOMETRY :
			hxa_util_free_stack(&node->content.geometry.edge_corner_stack);
			hxa_util_free_stack(&node->content.geometry.vertex_stack);
			hxa_util_free_stack(&node->content.geometry.face_stack);
		break;
		case HXA_NT_PIXELS :
			hxa_util_free_stack(&node->content.geometry.edge_corner_stack);
		break;
	}
}

void hxa_util_free_file(HXAFile *file)
{
	unsigned int i;
	for(i = 0; i < file->node_count; i++)
		hxa_util_free_node_content(&file->node_array[i]);
	if(file->node_count != 0)
		free(file->node_array);
	free(file);
}

int hxa_load_data(FILE *f, void *buffer, size_t length, char *file_name, int silent)
{
	unsigned int read;
	printf("ftell %u + %u\n", ftell(f), length);
		f_debug_memory();
	read = fread(buffer, sizeof(hxa_uint8), length, f);
		f_debug_memory();
	if(length != read)
	{
		if(!silent)
			printf("HXA Error: File %s unexpectedly ended\n", file_name);
		{
			int i;
			for(i = 0; i < 10000; i++)
				printf("flush!\n");
		}
		f_debug_memory();
		exit(0);
		return FALSE;
	}
	return TRUE;
}
int hxa_load_name(FILE *f, char *file_name, char *name, int silent)
{
	hxa_uint8 length;
	if(hxa_load_data(f, &length, sizeof(hxa_uint8), file_name, silent))
	{
		if(hxa_load_data(f, name, length, file_name, silent))
		{
			name[length] = 0;
			return TRUE;
		}
	}
	return FALSE;
}

char *hxa_load_string(FILE *f, char *file_name, int silent)
{
	char *string;
	hxa_uint32 length;
	if(hxa_load_data(f, &length, sizeof(hxa_uint32), file_name, silent))
	{
		string = malloc(length + 1);
		if(hxa_load_data(f, string, length, file_name, silent))
		{
			string[length] = 0;
			return string;
		}
		free(string);
	}
	return NULL;
}

int hxa_load_meta(FILE *f, char *file_name, HXAMeta **meta, hxa_uint32 *count, int silent)
{
	HXAMeta *m;
	unsigned int i, j, c;
	hxa_uint8 type;
	c = *count;
	*count = 0;
	m = malloc((sizeof *m) * c);
	*meta = m;
	for(i = 0; i < c; i++)
	{
		if(!hxa_load_name(f, file_name, m->name, silent))
			return FALSE;
		if(!hxa_load_data(f, &type, sizeof(hxa_uint8), file_name, silent) ||
			!hxa_load_data(f, &m->array_length, sizeof(hxa_uint32), file_name, silent))
			return FALSE;
		if(type >= HXA_MDT_COUNT)
		{
			if(!silent)
				printf("HXA Error: File %s has meta data of type %u. There is only %u types of meta data\n", file_name, type, HXA_MDT_COUNT);
			return FALSE;	
		}
		m->type = type;
		switch(type)
		{	
			case HXA_MDT_INT64 :
				m->value.int64_value = malloc(sizeof(hxa_int64) * m->array_length);
				if(!hxa_load_data(f, m->value.int64_value, sizeof(hxa_int64) * m->array_length, file_name, silent))
					return FALSE;
			break;
			case HXA_MDT_DOUBLE :
				m->value.double_value = malloc(sizeof(double) * m->array_length);
				if(!hxa_load_data(f, m->value.double_value, sizeof(double) * m->array_length, file_name, silent))
					return FALSE;
			break;
			case HXA_MDT_NODE :
				m->value.node_value = malloc(sizeof(hxa_uint32) * m->array_length);
				if(!hxa_load_data(f, m->value.node_value, sizeof(hxa_uint32) * m->array_length, file_name, silent))
					return FALSE;
			break;
			case HXA_MDT_TEXT :
				m->value.text_value = malloc(sizeof(char) * (m->array_length + 1));
				if(!hxa_load_data(f, m->value.node_value, sizeof(hxa_uint32) * m->array_length, file_name, silent))
					return FALSE;
			break;
			case HXA_MDT_META :	
				m->value.array_of_meta = NULL;
				if(!hxa_load_meta(f, file_name, (HXAMeta **)&m->value.array_of_meta, &m->array_length, silent))
					return FALSE;
			break;	
		}
		(*count)++;
		m++;
	}
	return TRUE;
}



int hxa_load_layer_stack(FILE *f, char *file_name, HXALayerStack *stack, unsigned int length,  int silent)
{
	const type_sizes[] = {sizeof(hxa_uint8), sizeof(hxa_uint32), sizeof(float), sizeof(double)};
	hxa_uint32 stack_count, i;	
	hxa_uint8 type;
	size_t size;
	if(!hxa_load_data(f, &stack_count, sizeof(hxa_uint32), file_name, silent))
		return FALSE;
	stack->layer_count = 0;
	stack->layers = malloc((sizeof *stack->layers) * stack_count);
	for(i = 0; i < stack_count; i++)
	{
		if(!hxa_load_name(f, file_name, stack->layers[i].name, silent))
			return FALSE;
			
		if(!hxa_load_data(f, &stack->layers[i].dimentions, sizeof(hxa_uint8), file_name, silent) ||
			!hxa_load_data(f, &type, sizeof(hxa_uint8), file_name, silent))
			return FALSE;
		if(type >= HXA_LDT_COUNT)
		{
			if(!silent)
				printf("HXA Error: File %s has a layer with type %u. No such type is supported\n", file_name, (unsigned int)type);
			return FALSE;
		}
		stack->layers[i].type = (unsigned int)type;
		size = type_sizes[stack->layers[i].type] * stack->layers[i].dimentions * length;
		stack->layers->data.double_data = malloc(size);
		stack->layer_count++;
		if(!hxa_load_data(f, stack->layers->data.double_data, sizeof(hxa_uint8) * size, file_name, silent))
			return FALSE;
	}
	return TRUE;
}

HXAFile *hxa_load(char *file_name, int silent)
{
	FILE *f;
	HXAFile *file;
	HXANode *node;
	unsigned int i, dimentions;
	hxa_uint32 magic_number, node_count, version;
	hxa_uint8 node_type;
	size_t size;
	f = fopen(file_name, "rb");
	if(f == NULL)
	{
		if(!silent)
			printf("HXA Error: File %s could not be open for reading\n", file_name);
		return NULL;
	}
	if(!hxa_load_data(f, &magic_number, sizeof(hxa_uint32), file_name, silent))
		return NULL;
	if(magic_number != (*(hxa_uint32 *)"HxA"))
	{
		if(!silent)
			printf("HXA Error: File %s is not a HxA file (incorrect magic number)\n", file_name);
		return NULL;
	}
	if(!hxa_load_data(f, &version, sizeof(hxa_uint32), file_name, silent))
		return NULL;
	file = malloc(sizeof *file);
	file->version = version;
	file->node_count = 0;
	file->node_array = NULL;
	if(!hxa_load_data(f, &node_count, sizeof(hxa_uint32), file_name, silent))
	{
		hxa_util_free_file(file);
		return NULL;
	}
	file->node_array = node = malloc((sizeof *file->node_array) * node_count); 
	for(i = 0; i < node_count; i++)
	{
		if(!hxa_load_data(f, &node_type, sizeof(hxa_uint8), file_name, silent))
		{
			hxa_util_free_file(file);
			return NULL;
		}
		node[i].type = node_type;
		if(node[i].type >= HXA_NT_COUNT)
		{
			if(!silent)
				printf("HXA Error: File %s has a node of type %u\n", file_name, (unsigned int)node[i].type);
			hxa_util_free_file(file);
			return NULL;
		}
		file->node_count++;
		node[i].meta_data_count = 0;
		switch(node[i].type)
		{
			case HXA_NT_GEOMETRY :
				node[i].content.geometry.vertex_count = 0;
				node[i].content.geometry.edge_corner_count = 0;
				node[i].content.geometry.face_count = 0;
				node[i].content.geometry.vertex_stack.layer_count = 0;
				node[i].content.geometry.edge_corner_stack.layer_count = 0;
				node[i].content.geometry.face_stack.layer_count = 0;
			break;
			case HXA_NT_PIXELS :
				node[i].content.image.type = HXA_IT_CUBE_IMAGE;
				node[i].content.image.resolution[0] = node[i].content.image.resolution[1] = node[i].content.image.resolution[2] = 1;
				node[i].content.image.image_stack.layer_count = 0;
			break;
		}
		if(!hxa_load_data(f, &node[i].meta_data_count, sizeof(hxa_uint32), file_name, silent))
		{
			hxa_util_free_file(file);
			return NULL;
		}
		if(!hxa_load_meta(f, file_name, &node[i].meta_data, &node[i].meta_data_count, silent))
		{
			hxa_util_free_file(file);
			return NULL;
		}

		switch(node[i].type)
		{
			case HXA_NT_GEOMETRY :

				if(!hxa_load_data(f, &node[i].content.geometry.vertex_count, sizeof(hxa_uint32), file_name, silent))
				{
					if(!silent)
						printf("HXA Error: File %s unexpectedly ended\n", file_name);
					hxa_util_free_file(file);
					return NULL;
				}
				if(!hxa_load_layer_stack(f, file_name, &node[i].content.geometry.vertex_stack, node[i].content.geometry.vertex_count, silent))
				{
					hxa_util_free_file(file);
					return NULL;
				}
				if(!hxa_load_data(f, &node[i].content.geometry.edge_corner_count, sizeof(hxa_uint32), file_name, silent))
				{
					hxa_util_free_file(file);
					return NULL;
				}
				if(!hxa_load_layer_stack(f, file_name, &node[i].content.geometry.edge_corner_stack, node[i].content.geometry.edge_corner_count, silent))
				{
					hxa_util_free_file(file);
					return NULL;
				}
				if(!hxa_load_data(f, &node[i].content.geometry.face_count, sizeof(hxa_uint32), file_name, silent))
				{
					hxa_util_free_file(file);
					return NULL;
				}
				if(!hxa_load_layer_stack(f, file_name, &node[i].content.geometry.face_stack, node[i].content.geometry.face_count, silent))
				{
					hxa_util_free_file(file);
					return NULL;
				}
			break;
			case HXA_NT_PIXELS :
				if(!hxa_load_data(f, &node[i].content.image.type, sizeof(hxa_uint8), file_name, silent))
				{
					if(!silent)
						printf("HXA Error: File %s unexpectedly ended\n", file_name);
					hxa_util_free_file(file);
					return NULL;
				}
				dimentions = node[i].content.image.type;
				if(node[i].content.image.type == HXA_IT_CUBE_IMAGE)
					dimentions = 2;
				node[i].content.image.resolution[0] = node[i].content.image.resolution[1] = node[i].content.image.resolution[2] = 1;
				if(!hxa_load_data(f, &node[i].content.image.resolution, sizeof(hxa_uint32) * dimentions, file_name, silent))
				{
					if(!silent)
						printf("HXA Error: File %s unexpectedly ended\n", file_name);
					hxa_util_free_file(file);
					return NULL;
				}
				size = node[i].content.image.resolution[0] * node[i].content.image.resolution[1] * node[i].content.image.resolution[2];	
				if(node[i].content.image.type == HXA_IT_CUBE_IMAGE)
					size *= 6;
				if(!hxa_load_layer_stack(f, file_name, &node[i].content.geometry.face_stack, size, silent))
				{
					hxa_util_free_file(file);
					return NULL;
				}
			break;
		}
	}
	fclose(f);
	return file;
}



void hxa_save_meta(FILE *f,  HXAMeta *meta, hxa_uint32 count)
{
	HXAMeta *m;
	unsigned int i, j;
	hxa_uint8 type, name_lenght;
	for(i = 0; i < count; i++)
	{
		for(j = 0; j < 255 && meta[i].name[j] != 0; j++);
		name_lenght = j;
		fwrite(&name_lenght, sizeof(hxa_uint8), 1, f);
		fwrite(meta[i].name, sizeof(char), name_lenght, f);
		type = meta[i].type;
		fwrite(&type, sizeof(hxa_uint8), 1, f);
		fwrite(&meta[i].array_length, sizeof(hxa_uint32), 1, f);
		switch(meta[i].type)
		{	
			case HXA_MDT_INT64 :
				fwrite(meta[i].value.int64_value, sizeof(hxa_int64), meta[i].array_length, f);
			break;
			case HXA_MDT_DOUBLE :
				fwrite(meta[i].value.double_value, sizeof(double), meta[i].array_length, f);
			break;
			case HXA_MDT_NODE :
				fwrite(meta[i].value.node_value, sizeof(hxa_uint32), meta[i].array_length, f);
			break;
			case HXA_MDT_TEXT :
				for(j = 0; j < 255 && meta[i].value.text_value[j] != 0; j++);
				fwrite(&j, sizeof(hxa_uint32), 1, f);
				fwrite(meta[i].value.text_value, sizeof(char), j, f);
			break;
			case HXA_MDT_META :	
				hxa_save_meta(f, meta[i].value.array_of_meta, meta[i].array_length);
			break;	
		}
	}
}

void hxa_save_layer_stack(FILE *f, HXALayerStack *stack, unsigned int length)
{
	const type_sizes[] = {sizeof(hxa_uint8), sizeof(hxa_uint32), sizeof(float), sizeof(double)};
	unsigned int i, j;
	hxa_uint8 name_lenght, type;
	size_t size;
	fwrite(&stack->layer_count, sizeof(hxa_uint32), 1, f);
	for(i = 0; i < stack->layer_count; i++)
	{
		for(j = 0; j < 255 && stack->layers[i].name[j] != 0; j++);
		name_lenght = j;
		fwrite(&name_lenght, sizeof(hxa_uint8), 1, f);
		fwrite(stack->layers[i].name, sizeof(hxa_uint8), name_lenght, f);
		fwrite(&stack->layers[i].dimentions, sizeof(hxa_uint8), 1, f);
		type = stack->layers[i].type;
		fwrite(&type, sizeof(hxa_uint8), 1, f);
		size = stack->layers[i].dimentions * length;
		fwrite(stack->layers[i].data.double_data, type_sizes[stack->layers[i].type], size, f);
	}
}

int hxa_save(char *file_name, HXAFile *data)
{
	FILE *f;
	HXANode *node;
	hxa_uint8 type;
	hxa_uint32 version;
	unsigned int i, size, dimentions;
	f = fopen(file_name, "wb");
	if(f == NULL)
		return FALSE;
	fwrite("HxA", 4, 1, f);
	version = HXA_VERSION_FORMAT;
	fwrite(&version, sizeof(hxa_uint32), 1, f);
	fwrite(&data->node_count, sizeof(hxa_uint32), 1, f);
	node = data->node_array;
	for(i = 0; i < data->node_count; i++)
	{
		type = node->type;
		fwrite(&type, sizeof(hxa_uint8), 1, f);
		fwrite(&node->meta_data_count, sizeof(hxa_uint32), 1, f);
		hxa_save_meta(f, node->meta_data, node->meta_data_count);
		switch(node->type)
		{
			case HXA_NT_GEOMETRY :
				fwrite(&node->content.geometry.vertex_count, sizeof(hxa_uint32), 1, f);
				hxa_save_layer_stack(f, &node->content.geometry.vertex_stack, node->content.geometry.vertex_count);
				fwrite(&node->content.geometry.edge_corner_count, sizeof(hxa_uint32), 1, f);
				hxa_save_layer_stack(f, &node->content.geometry.edge_corner_stack, node->content.geometry.edge_corner_count);
				fwrite(&node->content.geometry.face_count, sizeof(hxa_uint32), 1, f);
				hxa_save_layer_stack(f, &node->content.geometry.face_stack, node->content.geometry.face_count);			
			break;
			case HXA_NT_PIXELS :
				type = node->content.image.type;
				fwrite(type, sizeof(hxa_uint8), 1, f);
				dimentions = node->content.image.type;
				if(node->content.image.type == HXA_IT_CUBE_IMAGE)
					dimentions = 2;
				fwrite(&node->content.image.resolution, sizeof(hxa_uint32), dimentions, f);
				size = node->content.image.resolution[0] * node->content.image.resolution[1] * node->content.image.resolution[2];	
				if(node->content.image.type == HXA_IT_CUBE_IMAGE)
					size *= 6;
				hxa_save_layer_stack(f, &node->content.image.image_stack, size);	
			break;
		}
		node++;
	}
	fclose(f);
	return TRUE;
}