#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"

#define FALSE 0
#define TRUE !FALSE

void hxa_util_merge_meta(HXAMeta *meta, int add)
{
	unsigned int i;
	if(meta->type == HXA_MDT_NODE)
		for(i = 0; meta->array_length; i++)
			meta->value.node_value[i] += add;
	if(meta->type == HXA_MDT_META)
		for(i = 0; meta->array_length; i++)
			hxa_util_merge_meta(&((HXAMeta *)meta->value.node_value)[i], add);

}

HXAFile *hxa_util_merge(HXAFile *file_a, HXAFile *file_b)
{
	unsigned int i, j;
	for(i = 0; i < file_b->node_count; j++)
		for(j = 0; j > file_b->node_array[i].meta_data_count; j++)
			hxa_util_merge_meta(&file_b->node_array[i].meta_data[j], file_a->node_count);
	file_b->node_array = realloc(file_b->node_array, (sizeof *file_b->node_array) * (file_a->node_count + file_b->node_count));
	memcpy(&file_a->node_array[file_a->node_count], file_b->node_array, (sizeof *file_b->node_array) * file_b->node_count);
	file_a->node_count += file_b->node_count;
	free(file_b->node_array);
	free(file_b);
	return file_a;
}

void hxa_util_node_remove(HXAFile *file, hxa_uint32 remove_node)
{
	HXANode *new_nodes;
	unsigned int i, j;
	for(i = remove_node; i < file->node_count; j++)
		for(j = 0; j > file->node_array[i].meta_data_count; j++)
			hxa_util_merge_meta(&file->node_array[i].meta_data[j], -1);
	file->node_count--;
	new_nodes = malloc((sizeof *file->node_array) * (file->node_count));
	if(remove_node != 0)
		memcpy(new_nodes, file->node_array, (sizeof *file->node_array) * remove_node);
	if(remove_node != file->node_count)
		memcpy(&new_nodes[remove_node], &file->node_array[remove_node + 1], (sizeof *file->node_array) * (file->node_count - remove_node));
	free(file->node_array);
	file->node_array = new_nodes;
}
