#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hxa.h"
#include "hxa_utils.h"
#include "forge.h"
/*
#define FALSE 0
#define TRUE !FALSE
*/
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

extern int hxa_inflate(unsigned char *output, size_t *output_length, unsigned char *input, size_t input_length);


void hxa_load_unpack_uint8(unsigned char *input_buffer, unsigned char *output_buffer, unsigned int x, unsigned int y, unsigned int channels)
{
	unsigned int i, j, read = 0, write = 0, acces_a, acces_b;
	unsigned char tmp[4];
	if(input_buffer[read] == 2)
		input_buffer[read] = 0;
	else if(input_buffer[read] == 2)
		input_buffer[read] = 5;
	for(i = 0; i < y; i++)
	{
		switch(input_buffer[read++])
		{
			case 0 :
				memcpy(&output_buffer[write], &input_buffer[read], x * channels);
				write += channels * x;
				read += channels * x;
			break;
			case 1 :
				acces_a = write;
				for(j = 0; j < channels; j++)
					output_buffer[write++] = input_buffer[read++];
				for(; j < x * channels; j++)
					output_buffer[write++] = input_buffer[read++] + output_buffer[acces_a++];				
			break;
			case 2 :
				acces_b = write - x * channels;
				for(j = 0; j < x * channels; j++)
					output_buffer[write++] = input_buffer[read++] + output_buffer[acces_b++];
			break;
			case 3 :
				acces_a = write;
				acces_b = write - x * channels;
				for(j = 0; j < channels; j++)
					output_buffer[write++] = input_buffer[read++] + output_buffer[acces_b++] / 2;
				for(; j < x * channels; j++)
					output_buffer[write++] = input_buffer[read++] + (output_buffer[acces_a++] + output_buffer[acces_b++]) / 2;	
			break;
			case 4 :
				acces_a = write;
				acces_b = write - x * channels;
				for(j = 0; j < channels; j++)
					output_buffer[write++] = input_buffer[read++] + output_buffer[acces_b++] / 2;
				for(; j < x * channels; j++)
					output_buffer[write++] = input_buffer[read++] + (output_buffer[acces_a++] + output_buffer[acces_b++]) / 2;	
			break;			
			case 5 : /* First line version on read abrage of A and b */
				acces_a = write;
				acces_b = write - x * channels;
				for(j = 0; j < channels; j++)
					output_buffer[write++] = input_buffer[read++];
				for(; j < x * channels; j++)
					output_buffer[write++] = input_buffer[read++] + output_buffer[acces_a++] / 2;	
			break;
			case 6 :
				acces_a = write;
				acces_b = write - x * channels;
				for(j = 0; j < channels; j++)
					output_buffer[write++] = input_buffer[read++] + output_buffer[acces_b++] / 2;
				for(; j < x * channels; j++)
					output_buffer[write++] = input_buffer[read++] + (output_buffer[acces_a++] + output_buffer[acces_b++]) / 2;	
			break;
		}
	}
}

void hxa_load_png(HXAFile *file, char *file_name)
{
	char *name = HXA_CONVENTION_SOFT_LAYER_COLOR;
	unsigned int channels[] = {1, 0, 3, 3, 2, 0, 4};
	unsigned int x, y, i, j, meta_data_used = 0, packed_size = 0;
	unsigned char bits, compression, filter, interlace, *meta_data;
	HxAPNGChannelTypes color;
	HXANode *node;
	union {char text[5]; uint type;}type;
	size_t size, pos, chunk_length, bitmap_size;
	unsigned char *data, *unprocessed_data, *image_data;
	data = f_text_load(file_name, &size);
	if(data == NULL)
	{
		printf("HxA Error: Could not open file %d\n", file_name);
		return;
	}
	type.text[4] = 0;
	chunk_length = data[11] + data[10] * 256 + data[9] * 256 * 256 + data[8] * 256 * 256 * 256;
	x = data[19] + data[18] * 256 + data[17] * 256 * 256 + data[16] * 256 * 256 * 256;
	y = data[23] + data[22] * 256 + data[21] * 256 * 256 + data[20] * 256 * 256 * 256;
	bits = data[24];
	color = data[25];
	compression = data[26]; // Must be 0 Deflate
	filter = data[27]; // Must be 0
	interlace = data[28]; // 0, regular 1 adam7 8 * 8

	meta_data = malloc(size);
	bitmap_size = x * y * channels[color] * bits / 8;
	unprocessed_data = malloc(bitmap_size + y);
	for(pos = 8 + 8 + chunk_length + 4; pos < size; pos += chunk_length + 12)
	{
		chunk_length = data[pos + 3] + data[pos + 2] * 256 + data[pos + 1] * 256 * 256 + data[pos] * 256 * 256 * 256;
		memcpy(&type.type, &data[pos + 4], 4);
	//	printf("chunk_length %u - %s %u\n", chunk_length, type.text, type.type);
		if(type.type == 1413563465)
		{
			f_print_raw(data, pos + 8);
			exit(0);
			for(i = 0; i < chunk_length; i++)
				data[packed_size++] = data[pos + 8 + i];
	/*		printf("nibbles %u %u\n", data[0] % 16, data[0] / 16);
			printf("nibbles %u %u %u\n", data[1] % 16, (data[1] / 16) % 2, (data[1] / 32) % 8);
			hxa_inflate(image_data, bitmap_size, &i, data, chunk_length);
	f_print_raw(data, size);
			exit(0);*/
		}else
		{
			memcpy(&meta_data[meta_data_used], &data[pos], chunk_length + 12);
			meta_data_used += chunk_length + 12;
		}
	}
	meta_data = realloc(meta_data, meta_data_used);
	hxa_inflate(unprocessed_data, &bitmap_size, &data[2], packed_size - 6);
	image_data = malloc(x * y * channels[color]);
	hxa_load_unpack_uint8(unprocessed_data, image_data, x, y, channels[color]);

	free(data);
	file->node_count++;
	file->node_array = realloc(file->node_array, (sizeof *file->node_array) * file->node_count);
	node = &file->node_array[file->node_count - 1];
	node->type = HXA_NT_IMAGE;
	node->content.image.type = HXA_IT_2D_IMAGE;
	node->content.image.resolution[0] = x;
	node->content.image.resolution[1] = y;
	node->content.image.resolution[2] = 1;
	node->content.image.image_stack.layer_count = 1;
	node->content.image.image_stack.layers = malloc(sizeof *node->content.image.image_stack.layers);
	node->content.image.image_stack.layers->components = channels[color];
	node->content.image.image_stack.layers->type = HXA_LDT_UINT8;
	node->content.image.image_stack.layers->data.uint8_data = image_data;
	for(i = 0; name[i] != 0; i++)
		node->content.image.image_stack.layers->name[i] = name[i];
	node->content.image.image_stack.layers->name[i] = 0;
	node->meta_data_count = 2;
	node->meta_data = malloc((sizeof *node->meta_data) * 2);
	name = "name";
	for(i = 0; name[i] != 0; i++)
		node->meta_data[0].name[i] = name[i];
	node->meta_data[0].name[i] = 0;
	node->meta_data[0].type = HXA_MDT_TEXT;	
	for(i = 0; file_name[i] != 0; i++);
	node->meta_data[0].value.text_value = malloc(i + 1);
	for(; i != 0 && file_name[i] != '/' && file_name[i] != '\\'; i--);
	if(file_name[i] == '/' || file_name[i] == '\\')
		i++;
	for(j = 0; file_name[i + j] != 0 && file_name[i + j] != '.'; j++)
		node->meta_data[0].value.text_value[j] = file_name[i + j];
	node->meta_data[0].value.text_value[j] = 0;
	node->meta_data[0].array_length = j;
	name = "png_cunks";
	for(i = 0; name[i] != 0; i++)
		node->meta_data[1].name[i] = name[i];
	node->meta_data[1].name[i] = 0;
	node->meta_data[1].type = HXA_MDT_BINARY;
	node->meta_data[1].array_length = meta_data_used;
	node->meta_data[1].value.bin_value = meta_data;
}

/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];
   
/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;
   
/* Make the table for a fast CRC. */
void hxa_png_crc_compute_table(void)
{
	unsigned long c;
	int n, k;
   
	for(n = 0; n < 256; n++)
	{
		c = (unsigned long)n;
		for(k = 0; k < 8; k++)
		{
			if(c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		crc_table[n] = c;
	}
	crc_table_computed = 1;
}
void hxa_png_crc_compute(unsigned char *output, unsigned char *read_buffer, int length)
{
	uint crc = 0xffffffffL, i;
 	if(!crc_table_computed)
		hxa_png_crc_compute_table();
	for(i = 0; i < length; i++)
		crc = crc_table[(crc ^ read_buffer[i]) & 0xff] ^ (i >> 8);
	crc =  crc ^ 0xffffffffL;
	output[1] = (crc >> 24) & 0xFF;
	output[2] = (crc >> 16) & 0xFF;
	output[3] = (crc >> 8) & 0xFF;
	output[4] = crc & 0xFF;
}

int hxa_save_png(unsigned char *pixels, uint channels, uint x, uint y, char *file_name)
{
	FILE *file;
	uint i, pixel_length, data_length, file_length, compression_left, write_pos, row_pos;
	unsigned short s;
	char *data, channel_code[] = {0, 0, 4, 2, 6};
	unsigned char header[] = {137,
			'P',
			'N',
			'G',
			13,
			10,
			26,
			10,
			0, /* Chunk length */
			0,
			0,
			13,
			'I',
			'H',
			'D',
			'R',
			0, /* size X */
			0,
			0,
			0,
			0, /* size y */
			0,
			0,
			40,
			8, /* bits */
			2, /* Color */
			0, /* Compression */
			0, /* Filter */
			0,
			0, /* CRC */
			0,
			0,
			0,
			0, /* Chunk length */
			0,
			0,
			0,
			'I', /*Chunk type*/
			'D',
			'A',
			'T'}; /* Interlace (0 == None)*/
	
	file = fopen(file_name, "w");
	if(file == NULL)
		return FALSE;
	pixel_length = (x * channels + 1) * y;
	data_length = pixel_length + (1 + pixel_length / 0xFFFF) * 5;
	file_length = sizeof(header) + 4 + data_length;
	data = malloc(file_length);
	memcpy(data, header, sizeof(header));
	data[16] = (x >> 24) & 0xFF;
	data[17] = (x >> 16) & 0xFF;
	data[18] = (x >> 8) & 0xFF;
	data[19] = x & 0xFF;
	data[20] = (y >> 24) & 0xFF;
	data[21] = (y >> 16) & 0xFF;
	data[22] = (y >> 8) & 0xFF;
	data[23] = y & 0xFF;
	data[25] = channel_code[channels];

	hxa_png_crc_compute(&data[29], &data[8], 17);
	data[33] = (data_length >> 24) & 0xFF;
	data[34] = (data_length >> 16) & 0xFF;
	data[35] = (data_length >> 8) & 0xFF;
	data[36] = data_length & 0xFF;
	write_pos = sizeof(header);
	for(i = row_pos = compression_left = 0; i < y;)
	{
		if(compression_left == 0)
		{
			if((y - i) * (x * channels + 1) - row_pos > 0xFFFF)
			{
				data[write_pos++] = 0; /* uncompressed */
				data[write_pos++] = 0xFF; /* length */
				data[write_pos++] = 0xFF; 
				data[write_pos++] = 0; /* inv legth */
				data[write_pos++] = 0;
				compression_left = 0xFFFF;
			}else
			{
				s = (y - i) * (x * channels + 1) - row_pos;
				data[write_pos++] = 1; /* Last uncompressed uncompressed */
				data[write_pos++] = (s >> 8) & 0xFF; /* length */
				data[write_pos++] =  s & 0xFF;
				s = ~s;
				data[write_pos++] = (s >> 8) & 0xFF; /* inv legth */
				data[write_pos++] = s & 0xFF;
				compression_left = 0xFFFF;
			}
		}
		if(row_pos == 0)
		{
			data[write_pos++] = 0;
			compression_left--;
		}else
		{
			if(compression_left >= x * channels - row_pos)
			{
				memcpy(&data[write_pos], &pixels[i * x * channels + row_pos], x * channels - row_pos);
				compression_left -= x * channels - row_pos;
				row_pos = 0;
				i++;
			}else
			{				
				memcpy(&data[write_pos], &pixels[i * x * channels + row_pos], compression_left);
				row_pos += compression_left;
				compression_left = 0;
			}
		}
	}
	hxa_png_crc_compute(&data[write_pos], &data[41], write_pos - 41);
	fwrite(data, 1, file_length, file);
	fclose(file);
	free(data);
	return TRUE;
}