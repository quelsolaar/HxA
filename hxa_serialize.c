#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hxa.h"
#include "forge.h"

#define FALSE 0
#define TRUE !FALSE

void hxa_serialize_free_stack(HXALayerStack *stack)
{
	unsigned int i;
	for(i = 0; i < stack->layer_count; i++)
		free(stack->layers[i].data.uint8_data);
	if(stack->layer_count != 0)
		free(stack->layers);
}

void hxa_serialize_free_meta(HXAMeta *meta)
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
			free(meta->value.text_value);
		break;
		case HXA_MDT_BINARY :
			free(meta->value.bin_value);
		break;
		case HXA_MDT_META :
			for(i = 0; i < meta->array_length; i++)
				hxa_serialize_free_meta(&(((HXAMeta *)meta->value.array_of_meta)[i]));
			free(meta->value.array_of_meta);
		break;
	}
}

void hxa_serialize_free_node_content(HXANode *node)
{
	unsigned int i;
	for(i = 0; i < node->meta_data_count; i++)
		hxa_serialize_free_meta(&node->meta_data[i]);
	if(node->meta_data_count != 0)
		free(node->meta_data);
	switch(node->type)
	{
		case HXA_NT_GEOMETRY :
			hxa_serialize_free_stack(&node->content.geometry.corner_stack);
			hxa_serialize_free_stack(&node->content.geometry.edge_stack);
			hxa_serialize_free_stack(&node->content.geometry.vertex_stack);
			hxa_serialize_free_stack(&node->content.geometry.face_stack);
		break;
		case HXA_NT_IMAGE :
			hxa_serialize_free_stack(&node->content.image.image_stack);
		break;
	}
}

void hxa_serialize_free_file(HXAFile *file)
{
	unsigned int i;
	for(i = 0; i < file->node_count; i++)
		hxa_serialize_free_node_content(&file->node_array[i]);
	if(file->node_count != 0)
		free(file->node_array);
	free(file);
}



int hxa_unserialize_meta(hxa_uint8 *buffer, size_t *pos, size_t size, HXAMeta **meta, hxa_uint32 *count, int silent)
{
	HXAMeta *m;
	unsigned int i, c;
	hxa_uint8 type;
	size_t p, length;
	p = *pos;
	c = *count;
	*count = 0;
	m = malloc((sizeof *m) * c);
	*meta = m;
	for(i = 0; i < c; i++)
	{
		memcpy(m->name, &buffer[p + 1], buffer[p]);
		m->name[buffer[p]] = 0;
		p += buffer[p] + 1;
		type = buffer[p++];
		memcpy(&m->array_length, &buffer[p], sizeof(hxa_uint32));
		p += sizeof(hxa_uint32);

		if(type >= HXA_MDT_COUNT)
		{
			if(!silent)
				printf("HXA Error: Buffer has meta data of type %u. There is only %u types of meta data\n", type, HXA_MDT_COUNT);
			return FALSE;	
		}
		m->type = type;
		if(type == HXA_MDT_META)
		{	
			m->value.array_of_meta = NULL;
			if(!hxa_unserialize_meta(buffer, &p, size, (HXAMeta **)&m->value.array_of_meta, &m->array_length, silent))
				return FALSE;
		}else
		{
			switch(type)
			{	
				case HXA_MDT_INT64 :
				length = sizeof(hxa_int64) * m->array_length;
				break;
				case HXA_MDT_DOUBLE :
				length = sizeof(double) * m->array_length;
				break;
				case HXA_MDT_NODE :
				length = sizeof(hxa_uint32) * m->array_length;
				break;
				case HXA_MDT_TEXT :
				length = sizeof(char) * m->array_length;
				break;
				case HXA_MDT_BINARY :
				length = sizeof(char) * m->array_length;
				break;
			}
			m->value.int64_value = malloc(length);
			if(length > size - p)
				return FALSE;
			memcpy(m->value.int64_value, &buffer[p], length);	
			if(type == HXA_MDT_TEXT)
				m->value.text_value[m->array_length - 1] = 0;
			p += length;
		}
		(*count)++;
		m++;
	}
	*pos = p;
	return TRUE;
}

int hxa_unserialize_layer_stack(hxa_uint8 *buffer, size_t *pos, size_t size, HXALayerStack *stack, size_t length, int silent)
{
	const type_sizes[] = {sizeof(hxa_uint8), sizeof(hxa_uint32), sizeof(float), sizeof(double)};
	hxa_uint32 stack_count, i;	
	size_t s, p, text_length;
	p = *pos;
	memcpy(&stack_count, &buffer[p], sizeof(hxa_uint32));
	p += sizeof(hxa_uint32);

	stack->layer_count = 0;
	stack->layers = malloc((sizeof *stack->layers) * stack_count);
	for(i = 0; i < stack_count; i++)
	{
		if(p >= size)
			return FALSE;
		text_length = buffer[p++];
		if(p + text_length + 2 > size)
			return FALSE;
		memcpy(stack->layers[i].name, &buffer[p], text_length);
		stack->layers[i].name[text_length] = 0;
		p += text_length;

		stack->layers[i].components = buffer[p++];
		stack->layers[i].type = buffer[p++];
		if(stack->layers[i].type >= HXA_LDT_COUNT)
		{
			if(!silent)
				printf("HXA Error: Buffer has a layer with type %u. No such type is supported\n", (unsigned int)stack->layers[i].type);
			return FALSE;
		}

		s = type_sizes[stack->layers[i].type] * stack->layers[i].components * length;
		stack->layers[i].data.double_data = malloc(s);
		stack->layer_count++;
		if(p + s > size)
			return FALSE;
		memcpy(stack->layers[i].data.double_data, &buffer[p], s);
		p += s;

	}
	*pos = p;
	return TRUE;
}

HXAFile *hxa_unserialize(hxa_uint8 *buffer, size_t size, int silent)
{
	HXAFile *file;
	HXANode *node;
	unsigned int i, dimentions;
	hxa_uint32 node_count, version;
	size_t pos, layer_size;

	if(size < sizeof(hxa_uint32) * 3)
		return NULL;
	if(buffer[0] != 'H' || buffer[1] != 'x' || buffer[2] != 'A' || buffer[3] != 0)
	{
		if(!silent)
			printf("HXA Error: Buffer is not a HxA file (incorrect magic number)\n");
		return NULL;
	}
	memcpy(&version, &buffer[4], sizeof(hxa_uint32));
	file = malloc(sizeof *file);
	file->version = version;
	file->node_count = 0;
	file->node_array = NULL;
	memcpy(&node_count, &buffer[8], sizeof(hxa_uint32));
	pos = sizeof(hxa_uint32) * 3;
	file->node_array = node = malloc((sizeof *file->node_array) * node_count); 
	for(i = 0; i < node_count; i++)
	{

		if(size - pos < 1 + sizeof(hxa_uint32))
		{
			hxa_serialize_free_file(file);
			return NULL;
		}

		node[i].type = buffer[pos++];
		if(node[i].type >= HXA_NT_COUNT)
		{
			if(!silent)
				printf("HXA Error: Buffer has a node of type %u\n", (unsigned int)node[i].type);
			hxa_serialize_free_file(file);
			return NULL;
		}
		file->node_count++;
		node[i].meta_data_count = 0;
		node[i].meta_data = NULL;
		switch(node[i].type)
		{
			case HXA_NT_GEOMETRY :
				node[i].content.geometry.vertex_count = 0;
				node[i].content.geometry.edge_corner_count = 0;
				node[i].content.geometry.face_count = 0;
				node[i].content.geometry.vertex_stack.layer_count = 0;
				node[i].content.geometry.corner_stack.layer_count = 0;
				node[i].content.geometry.edge_stack.layer_count = 0;
				node[i].content.geometry.face_stack.layer_count = 0;
			break;
			case HXA_NT_IMAGE :
				node[i].content.image.type = HXA_IT_CUBE_IMAGE;
				node[i].content.image.resolution[0] = node[i].content.image.resolution[1] = node[i].content.image.resolution[2] = 1;
				node[i].content.image.image_stack.layer_count = 0;
			break;
		}
		memcpy(&node[i].meta_data_count, &buffer[pos], sizeof(hxa_uint32));
		pos += sizeof(hxa_uint32);

		if(!hxa_unserialize_meta(buffer, &pos, size, &node[i].meta_data, &node[i].meta_data_count, silent))
		{
			hxa_serialize_free_file(file);
			return NULL;
		}

		switch(node[i].type)
		{
			case HXA_NT_GEOMETRY :
				memcpy(&node[i].content.geometry.vertex_count, &buffer[pos], sizeof(hxa_uint32));
				pos += sizeof(hxa_uint32);

				if(!hxa_unserialize_layer_stack(buffer, &pos, size, &node[i].content.geometry.vertex_stack, node[i].content.geometry.vertex_count, silent))
				{
					hxa_serialize_free_file(file);
					return NULL;
				}

				memcpy(&node[i].content.geometry.edge_corner_count, &buffer[pos], sizeof(hxa_uint32));
				pos += sizeof(hxa_uint32);

				if(!hxa_unserialize_layer_stack(buffer, &pos, size, &node[i].content.geometry.corner_stack, node[i].content.geometry.edge_corner_count, silent))
				{
					hxa_serialize_free_file(file);
					return NULL;
				}
				if(file->version > 2)
				{
					if(!hxa_unserialize_layer_stack(buffer, &pos, size, &node[i].content.geometry.edge_stack, node[i].content.geometry.edge_corner_count, silent))
					{
						hxa_serialize_free_file(file);
						return NULL;
					}
				}
				memcpy(&node[i].content.geometry.face_count, &buffer[pos], sizeof(hxa_uint32));
				pos += sizeof(hxa_uint32);

				if(!hxa_unserialize_layer_stack(buffer, &pos, size, &node[i].content.geometry.face_stack, node[i].content.geometry.face_count, silent))
				{
					hxa_serialize_free_file(file);
					return NULL;
				}
			break;
			case HXA_NT_IMAGE :
				node[i].content.image.type = buffer[pos++];

				dimentions = node[i].content.image.type;
				if(node[i].content.image.type == HXA_IT_CUBE_IMAGE)
					dimentions = 2;
				node[i].content.image.resolution[0] = node[i].content.image.resolution[1] = node[i].content.image.resolution[2] = 1;
				memcpy(&node[i].content.image.resolution, &buffer[pos], sizeof(hxa_uint32) * dimentions);
				pos += sizeof(hxa_uint32) * dimentions;
				layer_size = node[i].content.image.resolution[0] * node[i].content.image.resolution[1] * node[i].content.image.resolution[2];	
				if(node[i].content.image.type == HXA_IT_CUBE_IMAGE)
					layer_size *= 6;
				if(!hxa_unserialize_layer_stack(buffer, &pos, size, &node[i].content.image.image_stack, layer_size, silent))
				{
					hxa_serialize_free_file(file);
					return NULL;
				}
			break;
		}
	}
	return file;
}



hxa_uint8 *hxa_serialize_meta(hxa_uint8 *b, HXAMeta *meta, hxa_uint32 count)
{
	unsigned int i, j;
	hxa_uint8 name_lenght;
	for(i = 0; i < count; i++)
	{
		for(j = 0; j < 255 && meta[i].name[j] != 0; j++);
		name_lenght = j;
		*b++ = name_lenght;
		memcpy(b, meta[i].name, name_lenght);
		b += name_lenght;
		*b++ = meta[i].type;
		memcpy(b, &meta[i].array_length, sizeof(hxa_uint32));
		b += sizeof(hxa_uint32);
		switch(meta[i].type)
		{	
			case HXA_MDT_INT64 :
				memcpy(b, meta[i].value.int64_value, sizeof(hxa_int64) * meta[i].array_length);
				b += sizeof(hxa_int64) * meta[i].array_length;
			break;
			case HXA_MDT_DOUBLE :
				memcpy(b, meta[i].value.double_value, sizeof(double) * meta[i].array_length);
				b += sizeof(double) * meta[i].array_length;
			break;
			case HXA_MDT_NODE :
				memcpy(b, meta[i].value.node_value, sizeof(hxa_uint32) * meta[i].array_length);
				b += sizeof(hxa_uint32) * meta[i].array_length;
			break;
			case HXA_MDT_TEXT :
				memcpy(b, meta[i].value.text_value, sizeof(char) * meta[i].array_length);
				b += sizeof(char) * meta[i].array_length;
			break;
			case HXA_MDT_BINARY :
				memcpy(b, meta[i].value.bin_value, sizeof(char) * meta[i].array_length);
				b += sizeof(char) * meta[i].array_length;
			break;
			case HXA_MDT_META :	
				b = hxa_serialize_meta(b, meta[i].value.array_of_meta, meta[i].array_length);
			break;	
		}
	}
	return b;
}


size_t hxa_serialize_layer_stack_size(HXALayerStack *stack, size_t length)
{
	const type_sizes[] = {sizeof(hxa_uint8), sizeof(hxa_uint32), sizeof(float), sizeof(double)};
	unsigned int i, j;
	hxa_uint8 name_lenght;
	size_t size;
	size = sizeof(hxa_uint32);
	for(i = 0; i < stack->layer_count; i++)
	{
		for(j = 0; j < 255 && stack->layers[i].name[j] != 0; j++);
		name_lenght = j;
		size += name_lenght + 1 + 1 + 1;
		if(stack->layers[i].type >= 4 || stack->layers[i].type < 0)
			exit(0);
		size += stack->layers[i].components * length * type_sizes[stack->layers[i].type];
	}
	return size;
}


hxa_uint8 *hxa_serialize_layer_stack(hxa_uint8 *b, HXALayerStack *stack, unsigned int length)
{
	const type_sizes[] = {sizeof(hxa_uint8), sizeof(hxa_uint32), sizeof(float), sizeof(double)};
	unsigned int i, j;
	hxa_uint8 name_lenght, *test;
	test = b;
	memcpy(b, &stack->layer_count, sizeof(hxa_uint32));
	b += sizeof(hxa_uint32);
	for(i = 0; i < stack->layer_count; i++)
	{
		for(j = 0; j < 255 && stack->layers[i].name[j] != 0; j++);
		name_lenght = j;
		*b++ = name_lenght;
		memcpy(b, stack->layers[i].name, name_lenght);
		b += name_lenght;
		*b++ = stack->layers[i].components;
		*b++ = stack->layers[i].type;
		memcpy(b, stack->layers[i].data.double_data, stack->layers[i].components * length * type_sizes[stack->layers[i].type]);
		b += stack->layers[i].components * length * type_sizes[stack->layers[i].type];
	}
	return b;
}

void hxa_serialize(hxa_uint8 *buffer, HXAFile *data)
{
	HXANode *node;
	hxa_uint8 *b;
	hxa_uint32 version = HXA_VERSION_FORMAT;
	unsigned int i, size, dimentions;

	memcpy(buffer, "HxA", 4);
	memcpy(&buffer[4], &version, sizeof(hxa_uint32));
	memcpy(&buffer[8], &data->node_count, sizeof(hxa_uint32));
	b = &buffer[12];
	node = data->node_array;
	for(i = 0; i < data->node_count; i++)
	{
		*b++ = node->type;
		memcpy(b, &node->meta_data_count, sizeof(hxa_uint32));
		b += sizeof(hxa_uint32);
		b = hxa_serialize_meta(b, node->meta_data, node->meta_data_count);

		switch(node->type)
		{
			case HXA_NT_GEOMETRY :
				memcpy(b, &node->content.geometry.vertex_count, sizeof(hxa_uint32));
				b += sizeof(hxa_uint32);
				b = hxa_serialize_layer_stack(b, &node->content.geometry.vertex_stack, node->content.geometry.vertex_count);
				memcpy(b, &node->content.geometry.edge_corner_count, sizeof(hxa_uint32));
				b += sizeof(hxa_uint32);
				b = hxa_serialize_layer_stack(b, &node->content.geometry.corner_stack, node->content.geometry.edge_corner_count);
				b = hxa_serialize_layer_stack(b, &node->content.geometry.edge_stack, node->content.geometry.edge_corner_count);
				memcpy(b, &node->content.geometry.face_count, sizeof(hxa_uint32));
				b += sizeof(hxa_uint32);
				b = hxa_serialize_layer_stack(b, &node->content.geometry.face_stack, node->content.geometry.face_count);			
			break;
			case HXA_NT_IMAGE :
				*b++ = node->content.image.type;
				dimentions = node->content.image.type;
				if(node->content.image.type == HXA_IT_CUBE_IMAGE)
					dimentions = 2;
				memcpy(b, &node->content.image.resolution, dimentions * sizeof(hxa_uint32));
				b += sizeof(hxa_uint32);
				size = node->content.image.resolution[0] * node->content.image.resolution[1] * node->content.image.resolution[2];	
				if(node->content.image.type == HXA_IT_CUBE_IMAGE)
					size *= 6;
				b = hxa_serialize_layer_stack(b, &node->content.image.image_stack, size);	
			break;
		}
		node++;
	}
}



size_t hxa_serialize_meta_size(HXAMeta *meta, hxa_uint32 count)
{
	unsigned int i, j;
	hxa_uint8 name_lenght;
	size_t size = 0;
	for(i = 0; i < count; i++)
	{
		for(j = 0; j < 255 && meta[i].name[j] != 0; j++);
		name_lenght = j;
		size += name_lenght + 1 + 1 + sizeof(hxa_uint32);
		switch(meta[i].type)
		{	
			case HXA_MDT_INT64 :
			size += sizeof(hxa_int64) * meta[i].array_length;
			break;
			case HXA_MDT_DOUBLE :
			size += sizeof(double) * meta[i].array_length;
			break;
			case HXA_MDT_NODE :
			size += sizeof(hxa_uint32) * meta[i].array_length;
			break;
			case HXA_MDT_TEXT :
			size += sizeof(char) * meta[i].array_length;
			break;
			case HXA_MDT_BINARY :
			size += sizeof(char) * meta[i].array_length;
			break;
			case HXA_MDT_META :	
			size += hxa_serialize_meta_size(meta[i].value.array_of_meta, meta[i].array_length);
			break;	
		}
	}
	return size;
}


size_t hxa_serialize_size(HXAFile *data)
{
	HXANode *node;
	hxa_uint32 version = HXA_VERSION_FORMAT;
	unsigned int i, dimentions;
	size_t size, s = 12;
	node = data->node_array;
	for(i = 0; i < data->node_count; i++)
	{
		s += 1 + sizeof(hxa_uint32);
		s += hxa_serialize_meta_size(node->meta_data, node->meta_data_count);
		switch(node->type)
		{
			case HXA_NT_GEOMETRY :
			s += sizeof(hxa_uint32);
			s += hxa_serialize_layer_stack_size(&node->content.geometry.vertex_stack, node->content.geometry.vertex_count);
			s += sizeof(hxa_uint32);
			s += hxa_serialize_layer_stack_size(&node->content.geometry.corner_stack, node->content.geometry.edge_corner_count);
			s += hxa_serialize_layer_stack_size(&node->content.geometry.edge_stack, node->content.geometry.edge_corner_count);
			s += sizeof(hxa_uint32);
			s += hxa_serialize_layer_stack_size(&node->content.geometry.face_stack, node->content.geometry.face_count);			
			break;
			case HXA_NT_IMAGE :
			s += sizeof(hxa_uint8);
			dimentions = node->content.image.type;
			if(node->content.image.type == HXA_IT_CUBE_IMAGE)
				dimentions = 2;
			s += dimentions * sizeof(hxa_uint32);
			s += sizeof(hxa_uint32);
			size = node->content.image.resolution[0] * node->content.image.resolution[1] * node->content.image.resolution[2];	
			if(node->content.image.type == HXA_IT_CUBE_IMAGE)
				size *= 6;
			s += hxa_serialize_layer_stack_size(&node->content.image.image_stack, size);	
			break;
		}
		node++;
	}
	return s;
}