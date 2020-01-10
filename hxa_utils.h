#include "hxa.h"

extern HXAFile	*hxa_load(char *file_name, int silent);
extern int		hxa_save(char *file_name, HXAFile *data);

extern void		hxa_util_free_node_content(HXANode *node);
extern void		hxa_util_free_file(HXAFile *file);

extern int		hxa_util_validate(HXAFile *file, int silent); /* Validates the HxA file to contain valid data. */
extern int		hxa_util_validate_pow(HXAFile *file, int silent); /* Validate that all images are of Power of two resolution. */

extern void		hxa_print(HXAFile *file, int data); 
extern void		hxa_print_meta_find(HXAFile *file, char *key);

#define			hxa_ref(a) (ref[a] >= 0 ? ref[a] : (-ref[a] - 1))
extern int		hxa_corner_get_next(int *ref, unsigned int corner);
extern int		hxa_corner_get_previous(int *ref, unsigned int corner);
extern int		hxa_edge_get_next(int *ref, unsigned int *neighbour, unsigned int edge);
extern int		hxa_edge_get_previous(int *ref, unsigned int *neighbour, unsigned int edge);

extern HXAFile *hxa_util_merge(HXAFile *file_a, HXAFile *file_b);

extern unsigned int *hxa_neighbour_node(HXANode *node);
extern void		hxa_neighbour_file(HXAFile *file);
extern void		hxa_util_convert_layer_float_to_double(HXALayer *layer, unsigned int count);
extern void		hxa_util_convert_layer_double_to_float(HXALayer *layer, unsigned int count);
extern void		hxa_util_convert_stack_float_to_double(HXALayerStack *stack, unsigned int count);
extern void		hxa_util_convert_stack_double_to_float(HXALayerStack *stack, unsigned int count);
extern void		hxa_util_convert_node_float_to_double(HXANode *node);
extern void		hxa_util_convert_node_double_to_float(HXANode *node);

extern void		hxa_util_node_vertex_purge(HXANode *node);
extern void		hxa_util_vertex_purge(HXAFile *file);

extern void		hxa_util_triangulate_node(HXANode *node, unsigned int max_sides);

extern void		hxa_close_node(HXANode *node);
extern void		hxa_close_file(HXAFile *file);