#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "forge.h"
#include "imagine.h"
#include "relinquish.h"
#include "seduce.h"
#include "hxa.h"


uint *hxa_persuade_image_test_texture_id = NULL;
float *hxa_persuade_image_test_aspects = NULL;
uint hxa_persuade_image_test_count = 0;


void hxa_persuade_image_test_init()
{
	HXAFile file;
	char path[1024];
	uint i;
	file.node_array = NULL;
	file.node_count = 0;
	for(i = 0; imagine_path_search(".png", TRUE, IMAGINE_DIR_HOME_PATH, FALSE, i, path, 1024); i++)
		hxa_load_png(&file, path);
	hxa_persuade_image_test_texture_id = malloc((sizeof *hxa_persuade_image_test_texture_id) * file.node_count);
	hxa_persuade_image_test_aspects = malloc((sizeof *hxa_persuade_image_test_aspects) * file.node_count);
	for(i = 0; i < file.node_count; i++)
	{
		if(file.node_array[i].type == HXA_NT_IMAGE &&
			file.node_array[i].content.image.type == HXA_IT_2D_IMAGE &&
			file.node_array[i].content.image.image_stack.layer_count > 0 &&
			file.node_array[i].content.image.image_stack.layers[0].components < 5 &&
			(file.node_array[i].content.image.image_stack.layers[0].type == HXA_LDT_UINT8 ||
			file.node_array[i].content.image.image_stack.layers[0].type == HXA_LDT_FLOAT))
		{
			uint j, k;
			for(j = 0; j < file.node_array[i].content.image.resolution[1]; j++)
			{
				for(k = 0; k < file.node_array[i].content.image.resolution[0]; k++)
					printf("(%u %u %u)",
					(uint)file.node_array[i].content.image.image_stack.layers[0].data.uint8_data[(file.node_array[i].content.image.resolution[0] * j + k) * 3 + 0],
					(uint)file.node_array[i].content.image.image_stack.layers[0].data.uint8_data[(file.node_array[i].content.image.resolution[0] * j + k) * 3 + 1],
					(uint)file.node_array[i].content.image.image_stack.layers[0].data.uint8_data[(file.node_array[i].content.image.resolution[0] * j + k) * 3 + 2]);

				printf("\n");
			}
			printf("\n");
			hxa_persuade_image_test_aspects[hxa_persuade_image_test_count] = (float)file.node_array[i].content.image.resolution[0] / (float)file.node_array[i].content.image.resolution[1];
			if(file.node_array[i].content.image.image_stack.layers[0].type == HXA_LDT_UINT8)
				hxa_persuade_image_test_texture_id[hxa_persuade_image_test_count] = r_texture_allocate(file.node_array[i].content.image.image_stack.layers[0].components - 1 + R_IF_R_UINT8 ,
																						file.node_array[i].content.image.resolution[0], file.node_array[i].content.image.resolution[1], 1, FALSE, FALSE, 
																						file.node_array[i].content.image.image_stack.layers[0].data.uint8_data);
			else
				hxa_persuade_image_test_texture_id[hxa_persuade_image_test_count] = r_texture_allocate(file.node_array[i].content.image.image_stack.layers[0].components - 1 + R_IF_R_FLOAT32 ,
																						file.node_array[i].content.image.resolution[0], file.node_array[i].content.image.resolution[1], 1, FALSE, FALSE, 
																						file.node_array[i].content.image.image_stack.layers[0].data.float_data);

			hxa_persuade_image_test_count++;
		}
	}
}

void hxa_persuade_image_test(BInputState *input)
{
	static boolean init = FALSE;
	if(!init)
	{
		hxa_persuade_image_test_init();
		init = TRUE;
	}
	if(hxa_persuade_image_test_count != 0)
		seduce_background_gallery_vertical_draw(input,  -0.5, 0.5, -1, 1, NULL, 0.2, hxa_persuade_image_test_texture_id, hxa_persuade_image_test_aspects, hxa_persuade_image_test_count, 0, NULL);

//	r_primitive_image(0, 0, -1, 1, 1, 0, 0, 1, 1, hxa_persuade_image_test_texture_id[0], 1, 1, 1, 1); /*draws a textured and shaded rectangle.*/

}