#include "hxa.h"



/* ------- disk stoarge ------------
Functions for saving and loading HxA files to and from disk. */

extern HXAFile	*hxa_load(char *file_name, int silent); /* Load a Hxa file in to memory.*/
extern int		hxa_save(char *file_name, HXAFile *data); /* Save a HxA structure to disk. */


extern void		hxa_util_free_node_content(HXANode *node); /* frees the content of a node but does not free the node pointer itself */
extern void		hxa_util_free_file(HXAFile *file); /* Frees an entire HxA structure. */

/* ------- validation ------------
Validates that the content of HxA structure. */
extern int		hxa_util_validate(HXAFile *file, int silent); /* Validates the HxA file to contain valid data. */
extern int		hxa_util_validate_pow(HXAFile *file, int silent); /* Validate that all images are of Power of two resolution. */


/* ------- validation ------------
Validates that the content of HxA structure. */
extern void		hxa_print(HXAFile *file, int data); /* Prints the content of a Hxa structure to stdout in a human readable form. If data is set to TRUE, Array content will be printed, otherwise not.*/
extern void		hxa_print_meta_find(HXAFile *file, char *key); 

extern void		hxa_util_meta_add(HXAMeta **meta_data, hxa_uint32 *meta_data_count, char *name, HXAMetaDataType type, void *data, unsigned int length, int copy); /* Adds a Meta data entry to a node. If copy is set to TRUE the function will allocate the memory needed to store the data and copy the data to it. If copy is set to FALSE the user provided pointer will be used by the structure.*/
extern void		*hxa_util_meta_get(HXAMeta *meta_data, hxa_uint32 meta_data_count, char *name, HXAMetaDataType type, unsigned int *length, int recursive); /* Searches for a Meta tag with a specific type and name and returns a pointer to it. It will also write the*/
extern unsigned int hxa_util_meta_get_next(HXAMeta *meta_data, hxa_uint32 start, hxa_uint32 meta_data_count, char *name, HXAMetaDataType type);

#define			hxa_ref(ref, a) (ref[a] >= 0 ? ref[a] : (-ref[a] - 1)) /* A macro that converts all ref values to positive references. */
extern int		hxa_corner_get_next(int *ref, unsigned int corner); /* Returns the position of the next reference value in a polygon from one corner. */
extern int		hxa_corner_get_previous(int *ref, unsigned int corner); /* Returns the position of the next reference value in a polygon from one corner.  */
extern int		hxa_edge_get_next(int *ref, unsigned int *neighbour, unsigned int edge);
extern int		hxa_edge_get_previous(int *ref, unsigned int *neighbour, unsigned int edge);
extern unsigned int *hxa_neighbour_node(HXANode *node); /* Generates edge adjacensy information for a geometry mesh. */
extern void		hxa_neighbour_file(HXAFile *file); /* Generates neighbour data for all geometry nodes in a HxA structure. */

extern HXAFile *hxa_util_merge(HXAFile *file_a, HXAFile *file_b); /* Merges two HxA structures in to one. */


extern void		hxa_util_convert_layer_float_to_double(HXALayer *layer, unsigned int count); /* Converts a layer from single precission float to double precission floats. */
extern void		hxa_util_convert_layer_double_to_float(HXALayer *layer, unsigned int count); /* Converts a layer from double precission float to single precission floats. */
extern void		hxa_util_convert_stack_float_to_double(HXALayerStack *stack, unsigned int count); /* Converts a layer stack from single precission float to double precission floats. */
extern void		hxa_util_convert_stack_double_to_float(HXALayerStack *stack, unsigned int count); /* Converts a layer stack from double precission float to single precission floats. */
extern void		hxa_util_convert_node_float_to_double(HXANode *node); /* Converts all layer in a node from single precission float to double precission floats. */
extern void		hxa_util_convert_node_double_to_float(HXANode *node); /* Converts all layer in a node from double precission float to single precission floats. */

extern void		hxa_util_node_vertex_purge(HXANode *node); /* removes any unused vertices from a node */
extern void		hxa_util_vertex_purge(HXAFile *file); /* removes any unused vertices from a structure */

extern void		hxa_util_triangulate_node(HXANode *node, unsigned int max_sides); /* Splits all n-gons with more sides then max_sides in to triangles. */

extern void		hxa_corner_to_vertex(HXANode *node); /**/

extern void		hxa_close_node(HXANode *node); /* Closes all holes in a polygon mesh */
extern void		hxa_close_file(HXAFile *file); /* Closes all holes in all polygon meshes */


extern HXAFile *hxa_fbx_load(char *file_name, HXAFile *hxa_file);