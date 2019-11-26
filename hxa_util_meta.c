#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE


void hxa_util_meta_add(HXAMeta **meta_data, hxa_uint32 *meta_data_count, char *name, HXAMetaDataType type, void *data, unsigned int length, int copy)
{
	HXAMeta *m;
	unsigned int i;
	*meta_data = realloc(*meta_data, (sizeof **meta_data) * (*meta_data_count + 1));
	m = &(*meta_data)[(*meta_data_count)++];
	for(i = 0; i < HXA_NAME_MAX_LENGTH - 1 && name[i] != 0; i++)
		m->name[i] = name[i];
	m->name[i] = 0;
	m->type = type;
	switch(type)
	{
		case HXA_MDT_INT64 :
			if(copy)
			{
				m->value.int64_value = malloc((sizeof *m->value.int64_value) * length);
				memcpy(m->value.int64_value, data, (sizeof *m->value.int64_value) * length);
			}else
				m->value.int64_value = data;
		break;
		case HXA_MDT_DOUBLE :
			if(copy)
			{
				m->value.double_value = malloc((sizeof *m->value.double_value) * length);
				memcpy(m->value.double_value, data, (sizeof *m->value.double_value) * length);
			}else
				m->value.int64_value = data;
		break;
		case HXA_MDT_NODE :
			if(copy)
			{
				m->value.node_value = malloc((sizeof *m->value.node_value) * length);
				memcpy(m->value.node_value, data, (sizeof *m->value.node_value) * length);
			}else
				m->value.int64_value = data;
		break;
		case HXA_MDT_TEXT :
			for(length = 0; ((char *)data)[length] != 0; length++);
			length++;
			if(copy)
			{
				m->value.text_value = malloc((sizeof *m->value.text_value) * length);
				memcpy(m->value.text_value, data, (sizeof *m->value.text_value) * length);
			}else
				m->value.text_value = data;
		break;
		case HXA_MDT_BINARY :
			if(copy)
			{
				m->value.bin_value = malloc((sizeof *m->value.bin_value) * length);
				memcpy(m->value.bin_value, data, (sizeof *m->value.bin_value) * length);
			}else
				m->value.bin_value = data;
		break;
		case HXA_MDT_META :
			if(copy)
			{
				exit(0); // FIX ME!
			}else
				m->value.array_of_meta = data;
		break;
	}
	m->array_length = length;
}

void *hxa_util_meta_get(HXAMeta *meta_data, hxa_uint32 meta_data_count, char *name, HXAMetaDataType type, unsigned int *length, int recursive)
{
	void *output;
	unsigned int i, j;
	for(i = 0; i < meta_data_count; i++)
	{
		if(meta_data[i].type == type)
		{
			for(j = 0; meta_data[i].name[j] == name[j] && name[j] != 0; j++);
			if(meta_data[i].name[j] == name[j])
			{
				if(length != NULL)
					*length = meta_data[i].array_length;
				return meta_data[i].value.array_of_meta;
			}
			
		}
		if(recursive && meta_data[i].type == HXA_MDT_META)
			if((output = hxa_util_meta_get(meta_data[i].value.array_of_meta, meta_data[i].array_length, name, type, length, TRUE)) != NULL)
				return output;
	}
	return NULL;
}


unsigned int hxa_util_meta_get_next(HXAMeta *meta_data, hxa_uint32 start, hxa_uint32 meta_data_count, char *name, HXAMetaDataType type)
{
	unsigned int i, j;
	for(i = start; i < meta_data_count; i++)
	{
		if(meta_data[i].type == type)
		{
			for(j = 0; meta_data[i].name[j] == name[j] && name[j] != 0; j++);
			if(meta_data[i].name[j] == name[j])
				return i;			
		}
	}
	return -1;
}