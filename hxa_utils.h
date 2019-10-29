#include "hxa.h"

extern HXAFile	*hxa_load(char *file_name, int silent);
extern int		hxa_save(char *file_name, HXAFile *data);

extern void		hxa_print(HXAFile *file, int data);
extern void		hxa_print_meta_find(HXAFile *file, char *key);

