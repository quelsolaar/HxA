#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FALSE 0
#define TRUE !FALSE

typedef struct{
	unsigned char *stream;
	unsigned char *end;
	unsigned bit;
}HxAInflateBitStream;


unsigned int hxa_inflate_read_bits(HxAInflateBitStream *stream, unsigned int bits)
{
	unsigned int output, progress, read;
	output = *stream->stream >> stream->bit;
	memcpy(&read, stream->stream, 4);
	output = read >> stream->bit;
	output &= (1 << bits) - 1;
	stream->stream += (stream->bit + bits) / 8;
	stream->bit = (stream->bit + bits) & 7;
	return output;
}


int hxa_inflate_uncompressed(HxAInflateBitStream *stream, unsigned char *output, size_t *output_length)
{
	unsigned int length;
	stream->bit = 0;
	stream->stream++;
	if(stream->stream + 4 > stream->end)
	{
		printf("HxA Error: Deflate error. Block header of uncompressed block is out of bounds.\n");
		return FALSE;
	}
	length = 256 * stream->stream[1] + stream->stream[0];
	if(length != ~(256 * (unsigned int)stream->stream[3] + (unsigned int)stream->stream[2]))
	{
		printf("HxA Error: Deflate error. Uncompressed block has non matching complement.\n");
		return FALSE;
	}
	stream->stream += 4;
	if(stream->stream + length > stream->end)
	{
		printf("HxA Error: Deflate error. Uncompressed block has length that is out of bounds.\n");
		return FALSE;
	}
	memcpy(&output[*output_length], stream->stream, length);
	stream->stream += length;
	*output_length += length;
	return TRUE;
}

unsigned int hxa_inflate_length_bits[30] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 6};
unsigned int hxa_inflate_length_base[30] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 323};
unsigned int hxa_inflate_distance_bits[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
unsigned int hxa_inflate_distance_base[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
const unsigned char hxa_inflate_code_length[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

typedef struct {
   unsigned short table[16];
   unsigned short translation[288];
}HXAInflateTree;

static int hxa_tree_lookup(HxAInflateBitStream *stream, HXAInflateTree *tree)
{
   int sum = 0, cur = 0, length = 0;
   while(TRUE)
   {
      cur = 2 * cur + hxa_inflate_read_bits(stream, 1);
      ++length;
      sum += tree->table[length];
      cur -= tree->table[length];
	  if(cur < 0)
		  break;
   }
   return tree->translation[sum + cur];
}

static void hxa_inflate_build_default_tree(HXAInflateTree *lt, HXAInflateTree *dt)
{
	unsigned int i;
	for(i = 0; i < 7; i++)
		lt->table[i] = 0;
	lt->table[7] = 24;
	lt->table[8] = 152;
	lt->table[9] = 112;
	for(i = 0; i < 24; i++)
		lt->translation[i] = 256 + i;
	for(i = 0; i < 144; i++)
		lt->translation[24 + i] = i;
	for(i = 0; i < 8; i++)
		lt->translation[24 + 144 + i] = 280 + i;
	for(i = 0; i < 112; i++)
		lt->translation[24 + 144 + 8 + i] = 144 + i;

	for(i = 0; i < 5; i++)
		dt->table[i] = 0;
	dt->table[5] = 32;
	for(i = 0; i < 32; i++)
		dt->translation[i] = i;
}

static void hxa_inflate_build_tree(HXAInflateTree *tree, const unsigned char *lengths, unsigned int count)
{
   unsigned int offsets[16];
   unsigned int i, sum;

	for(i = 0; i < 16; i++)
		tree->table[i] = 0;

	for(i = 0; i < count; i++)
		tree->table[lengths[i]]++;
	tree->table[0] = 0;

	for(sum = 0, i = 0; i < 16; i++)
	{
		offsets[i] = sum;
		sum += tree->table[i];
	}
	for(i = 0; i < count; i++)
		if(lengths[i])
			tree->translation[offsets[lengths[i]]++] = i;
}

static void hxa_inflate_decode_tree(HxAInflateBitStream *stream, HXAInflateTree *lt, HXAInflateTree *dt)
{
	HXAInflateTree code_tree;
	unsigned char lengths[288 + 32];
	unsigned int hlit, hdist, hclen;
	unsigned int i, count, length, previous, symbol;

	/* get 5 bits HLIT (257-286) */
	hlit = hxa_inflate_read_bits(stream, 5) + 257;

	/* get 5 bits HDIST (1-32) */
	hdist = hxa_inflate_read_bits(stream, 5) + 1;

	/* get 4 bits HCLEN (4-19) */
	hclen = hxa_inflate_read_bits(stream, 4) + 4;

	for(i = 0; i < 19; i++)
		lengths[i] = 0;

	/* read code lengths for code length alphabet */
	for(i = 0; i < hclen; ++i)
		lengths[hxa_inflate_code_length[i]] = hxa_inflate_read_bits(stream, 3);/* get 3 bits code length (0-7) */


	/* build code length tree */
	hxa_inflate_build_tree(&code_tree, lengths, 19);

	/* decode code lengths for the dynamic trees */
	for(count = 0; count < hlit + hdist;)
	{
		symbol = hxa_tree_lookup(stream, &code_tree);
		if(symbol >= 16)
		{
			switch((symbol - 16) % 4)
			{
				case 0:
					previous = lengths[count - 1];
					length = hxa_inflate_read_bits(stream, 2) + 3;
					for(i = 0; i < length; i++)
						lengths[count++] = previous;
				break;
				case 1:
					length = hxa_inflate_read_bits(stream, 3) + 3;
					for(i = 0; i < length; i++)
						lengths[count++] = 0;		
				break; 
				case 2:
					length = hxa_inflate_read_bits(stream, 7) + 11;
					for(i = 0; i < length; i++)
						lengths[count++] = 0;
				break;
			}
		}else
			lengths[count++] = symbol;
	}

	/* build dynamic trees */
	hxa_inflate_build_tree(lt, lengths, hlit);
	hxa_inflate_build_tree(dt, lengths + hlit, hdist);
}


int hxa_inflate_decode(HxAInflateBitStream *stream, unsigned char *output, size_t *output_length, HXAInflateTree *lt, HXAInflateTree *dt)
{
	unsigned int i, value, length, distance, offset;
	for(value = hxa_tree_lookup(stream, lt); value != 256; value = hxa_tree_lookup(stream, lt))
	{
		if(value < 256)
			output[(*output_length)++] = value;
		else if(value > 288)
		{
			printf("HxA Error: Deflate error. Compressed block has length that is out of bounds.\n");
			return FALSE;
		}else
		{
			value -= 257;
			length = hxa_inflate_read_bits(stream, hxa_inflate_length_bits[value]) + hxa_inflate_length_base[value];
			distance = hxa_tree_lookup(stream, dt);
			offset = hxa_inflate_read_bits(stream, hxa_inflate_distance_bits[distance]) + hxa_inflate_distance_base[distance];
			if(offset >= length)
				memcpy(&output[*output_length], &output[*output_length - offset], length);
			else
				for(i = 0; i < length; ++i)
		            output[*output_length + i] = output[*output_length + i - offset];
			*output_length += length;
		}
	}
	return TRUE;
}

int hxa_inflate(unsigned char *output, size_t *output_length, unsigned char *input, size_t input_length)
{
	HXAInflateTree lt, dt;
	HxAInflateBitStream stream;
	unsigned int end = FALSE, code;
	stream.stream = input;
	stream.end = input + input_length;
	stream.bit = 0;
	*output_length = 0;
	hxa_inflate_build_default_tree(&lt, &dt);
	while(!end)
	{
		end = hxa_inflate_read_bits(&stream, 1);
		code = hxa_inflate_read_bits(&stream, 2);
		switch(code % 4)
		{
			case 0 :
				if(!hxa_inflate_uncompressed(&stream, output, output_length))
					return FALSE;
			break;
			case 1 :
				if(!hxa_inflate_decode(&stream, output, output_length, &lt, &dt))
					return FALSE;
			break;
			case 2 :
			{			
				HXAInflateTree dynamic_lt, dynamic_dt;
				hxa_inflate_decode_tree(&stream, &dynamic_lt, &dynamic_dt);
				if(!hxa_inflate_decode(&stream, output, output_length, &dynamic_lt, &dynamic_dt))
					return FALSE;
			}
			break;
			case 3 :
				printf("HxA Error: Deflate error. Code 3.\n");
			return FALSE;
		}
	}
	return TRUE;
}
