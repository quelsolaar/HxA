#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"
#include "forge.h"

#define FALSE 0
#define TRUE !FALSE

void hxa_load_png_ihdr(unsigned char *data)
{

}

typedef enum{
	HXA_PNG_CT_GRAYSCALE = 0,
	HXA_PNG_CT_RGB = 2,
	HXA_PNG_CT_INDEXED = 3,
	HXA_PNG_CT_GRAYSCALE_AND_ALPHA = 4,
	HXA_PNG_CT_RGB_AND_ALPHA = 6
}HxAPNGChannelTypes;

/*
A palette is a 3 byte per entry list at the length of chunk.
*/

void hxa_load_png(char *file_name)
{
	unsigned int x, y;
	unsigned char bits, compression, filter, interlace;
	HxAPNGChannelTypes color;
	char text[5] = {0, 0, 0, 0};
	size_t size, pos, chunk_length;
	unsigned char *data;
	data = f_text_load(file_name, &size);
	
	chunk_length = data[11] + data[10] * 256 + data[9] * 256 * 256 + data[8] * 256 * 256 * 256;
	x = data[19] + data[18] * 256 + data[17] * 256 * 256 + data[16] * 256 * 256 * 256;
	y = data[23] + data[22] * 256 + data[21] * 256 * 256 + data[20] * 256 * 256 * 256;
	bits = data[24];
	color = data[25];
	compression = data[26]; // Must be 0 Deflate
	filter = data[27]; // Must be 0
	interlace = data[28]; // 0, regular 1 adam7 8 * 8
	
	for(pos = 8 + 8 + chunk_length + 4; pos < size; pos += chunk_length + 12)
	{
		chunk_length = data[pos + 3] + data[pos + 2] * 256 + data[pos + 1] * 256 * 256 + data[pos] * 256 * 256 * 256;
		text[0] = data[pos + 4];
		text[1] = data[pos + 5];
		text[2] = data[pos + 6];
		text[3] = data[pos + 7];
		printf("chunk_length %u - %s\n", chunk_length, text);
		f_print_raw(&data[pos + 8], 1);
//		f_print_raw(&data[pos], 12);
	}

	f_print_raw(data, size);
}