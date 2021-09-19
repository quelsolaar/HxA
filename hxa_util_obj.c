#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hxa.h"

#define FALSE 0
#define TRUE !FALSE


double hxa_util_parce_floating_point(char *text, size_t *position)
{
	double out = 0, mult = 0.1;
	int neg = FALSE;
	size_t i;
	for(i = *position; (text[i] < '0' || text[i] < '0') && text[i] != '\n' && text[i] != 0 && text[i] != '-'; i++);
	if(text[i] == 0 || text[i] == '\n')
	{
		*position = i;
		return out;
	}
	if(text[i] == '-')
	{
		neg = TRUE;
	}
	while(text[i] >= '0' && text[i] <= '9')
	{
		out *= 10;
		out += text[i++] - '0';
	}
	if(text[i] == '.')
	{
		i++;
		while(text[i] >= '0' && text[i] <= '9')
		{
			out += mult * (double)(text[i++] - '0');
			mult *= 0.1;
		}
	}
	if(neg)
		out = -out;
	*position = i;
	return out;
}


hxa_int32 hxa_util_parce_integer(char *text, size_t *position)
{
	hxa_int32 out = 0, neg = FALSE;
	size_t i;
	for(i = *position; (text[i] < '0' || text[i] < '0') && text[i] != '\n' && text[i] != 0 && text[i] != '-'; i++);
	if(text[i] == 0 || text[i] == '\n')
	{
		*position = i;
		return out;
	}
	if(text[i] == '-')
	{
		neg = TRUE;
	}
	while(text[i] >= '0' && text[i] <= '9')
	{
		out *= 10;
		out += text[i++] - '0';
	}
	if(neg)
		out = -out;
	*position = i;
	return out;
}


char *hxa_util_obj_file_load(char *file_name, size_t *size)
{
	char *buffer;
	unsigned int allocation;
	FILE *f;
	f = fopen(file_name, "rb");
	if(f == NULL)
	{
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	allocation = ftell(f);
	rewind(f);
	buffer = malloc(allocation + 3);
	buffer[0] = '\n'; /**/
	allocation = fread(&buffer[1], 1, allocation, f) + 1;
	buffer[allocation] = '\n'; 
	buffer[allocation + 1] = 0;
	fclose(f);
	if(size != NULL)
		*size = allocation;
	return buffer;
}

HXANode *hxa_util_obj_add(HXAFile *file)
{
	HXANode *node;
	file->node_array = realloc(file->node_array, (sizeof *file->node_array) * (file->node_count + 1));
	node = &file->node_array[file->node_count++];
	node->type = HXA_NT_GEOMETRY;
	node->meta_data_count = 0;
	node->meta_data = NULL;
	node->content.geometry.vertex_count = 0;
	node->content.geometry.vertex_stack.layer_count = 0;
	node->content.geometry.vertex_stack.layers = NULL;
	node->content.geometry.edge_corner_count = 0;
	node->content.geometry.corner_stack.layer_count = 0;
	node->content.geometry.corner_stack.layers = NULL;
	node->content.geometry.edge_stack.layer_count = 0;
	node->content.geometry.edge_stack.layers = NULL;
	node->content.geometry.face_count = 0;
	node->content.geometry.face_stack.layer_count = 0;
	node->content.geometry.face_stack.layers = NULL;
	return node;
}

void hxa_util_obj_load_layer_set(HXALayer *layer, unsigned int count, HXALayerDataType type, unsigned char components, char *name)
{
	unsigned int i;
	layer->components = components;
	layer->type = type;
	if(type == HXA_LDT_INT32)
		layer->data.int32_data = malloc((sizeof *layer->data.int32_data) * count * (unsigned int)components);
	else if(type == HXA_LDT_FLOAT)
		layer->data.float_data = malloc((sizeof *layer->data.float_data) * count * (unsigned int)components);
	else if(type == HXA_LDT_DOUBLE)
		layer->data.double_data = malloc((sizeof *layer->data.double_data) * count * (unsigned int)components);
	else
		layer->data.double_data = NULL;
	for(i = 0; name[i] != 0; i++)
		layer->name[i] = name[i];
	layer->name[i] = 0;
}

void hxa_util_obj_build_vertex(HXANode *node, unsigned int vertex_count, unsigned int *used, double *vertex_array, unsigned int vertex_used)
{
	unsigned int i;
	double *v;
	node->content.geometry.vertex_count = vertex_used;
	v = node->content.geometry.vertex_stack.layers[0].data.double_data = malloc((sizeof *node->content.geometry.vertex_stack.layers[0].data.double_data) * 3 * vertex_used);
	for(i = 0; i < vertex_count; i++)
	{
		if(used[i] != -1)
		{
			*v++ = vertex_array[i * 3 + 0];
			*v++ = vertex_array[i * 3 + 1];
			*v++ = vertex_array[i * 3 + 2];
			used[i] = -1;
		}
	}
}
void hxa_util_obj_layers_get(HXANode *node, hxa_int32 **mat, hxa_int32 **group, hxa_int32 **ref, float **uv, float **n)
{
	char *group_name = HXA_CONVENTION_SOFT_LAYER_GROUP_ID, *material_name = HXA_CONVENTION_SOFT_LAYER_MATERIAL_ID, *uv_name = HXA_CONVENTION_SOFT_LAYER_UV0, *normals_name = HXA_CONVENTION_SOFT_LAYER_NORMALS;
	unsigned int i, j;
	node->content.geometry.edge_corner_count = 0;
	node->content.geometry.face_count = 0;
	*mat = NULL;
	*group = NULL;
	for(i = 0; i < node->content.geometry.face_stack.layer_count; i++)
	{
		for(j = 0; group_name[j] != 0 && node->content.geometry.face_stack.layers[i].name[j] == group_name[j]; j++);
		if(node->content.geometry.face_stack.layers[i].name[j] == group_name[j])
			*group = node->content.geometry.face_stack.layers[i].data.int32_data;
		for(j = 0; material_name[j] != 0 && node->content.geometry.face_stack.layers[i].name[j] == material_name[j]; j++);
		if(node->content.geometry.face_stack.layers[i].name[j] == material_name[j])
			*mat = node->content.geometry.face_stack.layers[i].data.int32_data;
	}
	
	*ref = node->content.geometry.corner_stack.layers[HXA_CONVENTION_HARD_BASE_CORNER_LAYER_ID].data.int32_data;
	*uv = *n = NULL;
	for(i = 1; i < node->content.geometry.corner_stack.layer_count; i++)
	{
		for(j = 0; uv_name[j] != 0 && node->content.geometry.corner_stack.layers[i].name[j] == uv_name[j]; j++);
		if(node->content.geometry.corner_stack.layers[i].name[j] == uv_name[j])
			*uv = node->content.geometry.corner_stack.layers[i].data.float_data;
		for(j = 0; normals_name[j] != 0 && node->content.geometry.corner_stack.layers[i].name[j] == normals_name[j]; j++);
		if(node->content.geometry.corner_stack.layers[i].name[j] == normals_name[j])
			*n = node->content.geometry.corner_stack.layers[i].data.float_data;
	}
}

HXAFile *hxa_util_obj_load(char *file_name)
{
	char *buffer;
	HXAFile *file;
	HXANode *node;
	size_t i, j, temp, size;
	double f, *vertex = NULL;
	unsigned int white_space, slash_count, coord_count, normal_count, *used, primitive, vertex_used;
	hxa_int32 r, *write_mat, *write_group, *write_ref, current_mat = 0, current_group = 0, vertex_count;
	float *write_uv, *write_normals, *uv = NULL, *normals = NULL;
	HXALayer *layer; 
	char *t;
	buffer = hxa_util_obj_file_load(file_name, &size);
	if(buffer == NULL)
		return NULL;
	size--;
	file = malloc(sizeof *file);
	file->node_count = 0;
	file->node_array = malloc(sizeof *file->node_array);
	node = hxa_util_obj_add(file);
	vertex_count = coord_count = normal_count = 0;
	for(i = 0; i < size; i++)
	{
		if(buffer[i] == '\n')
		{
			i++;
			if(buffer[i] == 'g' && buffer[i + 1] <= ' ') /* new objects */
			{
				if(node->content.geometry.face_count != 0)
					node = hxa_util_obj_add(file);
			}
			if(buffer[i] == 'f' && buffer[i + 1] <= ' ') /* we have faces */
			{
				white_space = TRUE;
				j = slash_count = 0;
				do {
					i++;
					if(buffer[i] == '/')
					{
						if(buffer[i + 1] == '/')
						{
							node->content.geometry.vertex_count |= 1;/* borrow vertex count to store use of vertex normals*/
							slash_count = 42;
						}else
							slash_count++;
					}

					if(white_space != (buffer[i] <= ' '))
					{
						if(white_space)
							node->content.geometry.edge_corner_count++;
						if(slash_count == 1)
							node->content.geometry.vertex_count |= 2; /* borrow vertex count to store use of vertex coordinates*/
						else if(slash_count == 2)
							node->content.geometry.vertex_count |= 3; /* borrow vertex count to store use of vertex coordinates ad normals*/
						slash_count = 0;
						white_space = !white_space;
						j++;
					}
				}while(buffer[i] != '\n');
				node->content.geometry.edge_corner_count += j / 2;
				node->content.geometry.face_count++;
			}
			if(buffer[i] == 's' && buffer[i + 1] <= ' ') /* we have groups */
			{
				node->content.geometry.face_stack.layer_count |= 1;
			}
			if(buffer[i] == 'u' && buffer[i + 1] == 's' && buffer[i + 2] == 'e' && buffer[i + 3] == 'm' && buffer[i + 4] == 't' && buffer[i + 5] == 'l' && buffer[i + 6] <= ' ') /* we have groups */
			{
				node->content.geometry.face_stack.layer_count |= 2;
			}
			if(buffer[i] == 'v') /* we have groups */
			{
				i++;
				if(buffer[i] <= ' ')	
					vertex_count++;
				else
				{	
					if(buffer[i] == 't' && buffer[i + 1] <= ' ')	
						coord_count++;
					else if(buffer[i] == 'n' && buffer[i + 1] <= ' ')	
						normal_count++;
				}
			}
		}
	}
	for(i = 0; i < file->node_count; i++)
	{
		/* face stack */
		node = &file->node_array[i];
		temp = node->content.geometry.face_stack.layer_count;
		if(temp == (1 | 2))
			node->content.geometry.face_stack.layers = malloc((sizeof *node->content.geometry.face_stack.layers) * 2);
		else if(temp != 0)
			node->content.geometry.face_stack.layers = malloc(sizeof *node->content.geometry.face_stack.layers);
		node->content.geometry.face_stack.layer_count = 0;
		if(temp & 1)
			hxa_util_obj_load_layer_set(&node->content.geometry.face_stack.layers[node->content.geometry.face_stack.layer_count++], node->content.geometry.face_count, HXA_LDT_INT32, 1, HXA_CONVENTION_SOFT_LAYER_GROUP_ID);
		if(temp & 2)
			hxa_util_obj_load_layer_set(&node->content.geometry.face_stack.layers[node->content.geometry.face_stack.layer_count++], node->content.geometry.face_count, HXA_LDT_INT32, 1, HXA_CONVENTION_SOFT_LAYER_MATERIAL_ID);
		/* vertex stack */
		node->content.geometry.vertex_stack.layers = malloc(sizeof *node->content.geometry.vertex_stack.layers);
		hxa_util_obj_load_layer_set(&node->content.geometry.vertex_stack.layers[node->content.geometry.vertex_stack.layer_count++], 0, HXA_LDT_DOUBLE, HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_COMPONENTS, HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME);

		/* corner stack */
		node->content.geometry.corner_stack.layer_count = 1;
		if((node->content.geometry.vertex_count & 1) && normal_count != 0)
			node->content.geometry.corner_stack.layer_count++;
		if((node->content.geometry.vertex_count & 2) && coord_count != 0)
			node->content.geometry.corner_stack.layer_count++;
		node->content.geometry.corner_stack.layers = malloc((sizeof *node->content.geometry.corner_stack.layers) * node->content.geometry.corner_stack.layer_count);
		node->content.geometry.corner_stack.layer_count = 0;
		hxa_util_obj_load_layer_set(&node->content.geometry.corner_stack.layers[node->content.geometry.corner_stack.layer_count++], node->content.geometry.edge_corner_count, HXA_LDT_INT32, HXA_CONVENTION_HARD_BASE_CORNER_LAYER_COMPONENTS, HXA_CONVENTION_HARD_BASE_CORNER_LAYER_NAME);
		if((node->content.geometry.vertex_count & 1) && normal_count != 0)
			hxa_util_obj_load_layer_set(&node->content.geometry.corner_stack.layers[node->content.geometry.corner_stack.layer_count++], node->content.geometry.edge_corner_count, HXA_LDT_FLOAT, 3, HXA_CONVENTION_SOFT_LAYER_NORMALS);
		if((node->content.geometry.vertex_count & 2) && coord_count != 0)
			hxa_util_obj_load_layer_set(&node->content.geometry.corner_stack.layers[node->content.geometry.corner_stack.layer_count++], node->content.geometry.edge_corner_count, HXA_LDT_FLOAT, 2, HXA_CONVENTION_SOFT_LAYER_UV0);
		node->content.geometry.face_count = 0;
	}
	vertex = malloc((sizeof *vertex) * vertex_count * 3);
	if(coord_count != 0)
		uv = malloc((sizeof *uv) * coord_count * 2);
	if(normal_count != 0)
		normals = malloc((sizeof *normals) * normal_count * 3);
	vertex_count = coord_count = normal_count = 0;
	for(i = 0; i < size; i++)
	{
		if(buffer[i] == '\n')
		{
			i++;
			if(buffer[i] == 'v') /* we have groups */
			{
				i++;
				if(buffer[i] <= ' ')	
				{
					vertex[vertex_count * 3] = vertex[vertex_count * 3 + 1] = vertex[vertex_count * 3 + 2] = 0.0;					
					vertex[vertex_count * 3] = hxa_util_parce_floating_point(buffer, &i);
					if(buffer[i] != 0 && buffer[i] != 0)
					{
						vertex[vertex_count * 3 + 1] = hxa_util_parce_floating_point(buffer, &i);
						if(buffer[i] != 0 && buffer[i] != 0)
							vertex[vertex_count * 3 + 2] = hxa_util_parce_floating_point(buffer, &i);
					}
					i--;
					vertex_count++;
				}
				else if(buffer[i] == 't' && buffer[i + 1] <= ' ')	
				{
					uv[coord_count * 2] = uv[coord_count * 2 + 1] = 0.0;					
					uv[coord_count * 2] = hxa_util_parce_floating_point(buffer, &i);
					if(buffer[i] != 0 && buffer[i] != 0)
						uv[coord_count * 2 + 1] = hxa_util_parce_floating_point(buffer, &i);
					i--;
					coord_count++;
				}
				else if(buffer[i] == 'n' && buffer[i + 1] <= ' ')	
				{
					normals[normal_count * 3] = normals[normal_count * 3 + 1] = normals[normal_count * 3 + 2] = 1.0;					
					normals[normal_count * 3] = hxa_util_parce_floating_point(buffer, &i);
					if(buffer[i] != 0 && buffer[i] != 0)
					{
						normals[normal_count * 3 + 1] = hxa_util_parce_floating_point(buffer, &i);
						if(buffer[i] != 0 && buffer[i] != 0)
							normals[normal_count * 3 + 2] = hxa_util_parce_floating_point(buffer, &i);
					}
					i--;
					f = sqrt(normals[normal_count * 3] * normals[normal_count * 3] + 
							 normals[normal_count * 3 + 1] * normals[normal_count * 3 + 1] +
							 normals[normal_count * 3 + 2] * normals[normal_count * 3 + 2]);
					if(f > 0.0001)
					{
						normals[normal_count * 3] /= f;
						normals[normal_count * 3 + 1] /= f;
						normals[normal_count * 3 + 2] /= f;
					}else
					{
						normals[normal_count * 3] = normals[normal_count * 3 + 1] = 0.0;
						normals[normal_count * 3 + 2] = 1.0;	
					}
					normal_count++;
				}
			}
		}
	}
	used = malloc((sizeof *used) * vertex_count);
	for(i = 0; i < vertex_count; i++)
		used[i] = -1;
	node = file->node_array;
	file->node_count = 0;
	hxa_util_obj_layers_get(node, &write_mat, &write_group, &write_ref, &write_uv, &write_normals);

	vertex_used = 0;
	for(i = 0; i < size; i++)
	{
		if(buffer[i] == '\n')
		{
			i++;
			if(buffer[i] == 'g' && buffer[i + 1] <= ' ')
			{			
				f_debug_memory();
				if(node->content.geometry.face_count != 0)
				{
					hxa_util_obj_build_vertex(node, vertex_count, used, vertex, vertex_used);
					vertex_used = 0;
					
					file->node_count++;
					node++;
				}
				hxa_util_obj_layers_get(node, &write_mat, &write_group, &write_ref, &write_uv, &write_normals);
				for(i++; buffer[i] <= ' ' && buffer[i] != '\n'; i++);
				if(buffer[i] != '\n')
				{
					char *name = HXA_CONVENTION_SOFT_NAME;
					node->meta_data = realloc(node->meta_data, sizeof *node->meta_data);
					for(j = 0; j < HXA_NAME_MAX_LENGTH && name[j] != 0; j++)
						node->meta_data[0].name[j] = name[j];
					node->meta_data[0].name[j] = 0;
					node->meta_data[0].type = HXA_MDT_TEXT;
					for(j = 0; buffer[i + j] != 0 && buffer[i + j] != '\n'; j++);
					node->meta_data[0].array_length = j;
					node->meta_data[0].value.text_value = malloc(node->meta_data[0].array_length + 1);
					for(j = 0; j < node->meta_data[0].array_length; j++)
						node->meta_data[0].value.text_value[j] = buffer[i + j];
					node->meta_data[0].value.text_value[j] = 0;
					printf("-%s-\n", node->meta_data[0].value.text_value);
					node->meta_data_count = 1;
				}
				f_debug_memory();
			}else if(buffer[i] == 'f')
			{
				i++;
				primitive = 0;
				while(buffer[i] != '\n' && buffer[i] != 0)
				{
					if(buffer[i] >= '0' && buffer[i] <= '9')
					{
						r = hxa_util_parce_integer(buffer, &i) - 1;
						if(r < vertex_count && r >= 0)
						{
							if(used[r] == -1)
								used[r] = vertex_used++;
							write_ref[primitive] = used[r];
							if(buffer[i] == '/')
							{
								r = hxa_util_parce_integer(buffer, &i) - 1;
								if(r < coord_count && r >= 0 && write_uv != NULL)
								{
									write_uv[primitive * 2] = uv[r * 2];
									write_uv[primitive * 2 + 1] = uv[r * 2 + 1];
								}
								if(buffer[i] == '/')
								{
									r = hxa_util_parce_integer(buffer, &i);
									if(r < normal_count && r >= 0 && write_normals != NULL)
									{
										write_normals[primitive * 3] = normals[r * 3];
										write_normals[primitive * 3 + 1] = normals[r * 3 + 1];
										write_normals[primitive * 3 + 2] = normals[r * 3 + 2];
									}
								}
							}
							primitive++;
						}
					}else
						i++;
					f_debug_memory();
				}
				if(primitive > 2)
				{
					f_debug_memory();
					write_ref[primitive - 1] = -1 - write_ref[primitive - 1];
					write_ref += primitive;
					node->content.geometry.edge_corner_count += primitive;
					if(write_uv != NULL)
						write_uv += primitive * 2;
					if(write_normals != NULL)
						write_normals += primitive * 3;
					f_debug_memory();
					if(write_mat != NULL)
						write_mat[node->content.geometry.face_count] = current_mat;
					f_debug_memory();
					if(write_group != NULL) 
						write_group[node->content.geometry.face_count] = current_group;
					node->content.geometry.face_count++;
					f_debug_memory();

				}
			}else if(buffer[i] == 's' && buffer[i + 1] <= ' ') /* we have groups */
			{
				current_group = hxa_util_parce_integer(buffer, &i);
				i--;
			}else if(buffer[i] == 'u' && buffer[i + 1] == 's' && buffer[i + 2] == 'e' && buffer[i + 3] == 'm' && buffer[i + 4] == 't' && buffer[i + 5] == 'l' && buffer[i + 6] <= ' ') /* we have groups */
			{
				current_mat = hxa_util_parce_integer(buffer, &i);
				i--;
			}
		}
	}
	if(node->content.geometry.face_count != 0)
	{
		hxa_util_obj_build_vertex(node, vertex_count, used, vertex, vertex_used);	
		file->node_count++;
	}
	if(vertex != 0)
		free(vertex);
	if(uv != 0)
		free(uv);
	if(normals != 0)
		free(normals);
	free(buffer);
	return file;
}

/*
void hxa_print(HXAFile *file, int data);

int main(void)
{
	HXAFile *file;
	file = hxa_util_obj_load("object2.obj");
	hxa_print(file, FALSE);
	file = NULL;
}*/