#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "aces.h"

#define FALSE 0
#define TRUE !FALSE

#define ACES_IS_BIG_ENDIAN (*(unsigned short *)"\0\xff" < 0x100)

void aces_copy16(void *a, void *b)
{
	if(ACES_IS_BIG_ENDIAN)
		*((unsigned short *)a) = ((unsigned short)((unsigned char *)b)[1] << 8) |
									(unsigned short)((unsigned char *)b)[0];
	else
		memcpy(a, b, 2);
}
void aces_copy32(void *a, void *b)
{
	if(ACES_IS_BIG_ENDIAN)
		*((unsigned int *)a) = ((unsigned int)((unsigned char *)b)[3] << 24) |
								((unsigned int)((unsigned char *)b)[2] << 16) |
								((unsigned int)((unsigned char *)b)[1] << 8) |
								(unsigned int)((unsigned char *)b)[0];
	else
		memcpy(a, b, 4);
}
void aces_copy64(void *a, void *b)
{
	if(ACES_IS_BIG_ENDIAN)
		*((unsigned long long *)a) = ((unsigned long long)((unsigned char *)b)[7] << 56) |
									((unsigned long long)((unsigned char *)b)[6] << 48) |
									((unsigned long long)((unsigned char *)b)[5] << 40) |
									((unsigned long long)((unsigned char *)b)[4] << 32) |
									((unsigned long long)((unsigned char *)b)[3] << 24) |
									((unsigned long long)((unsigned char *)b)[2] << 16) |
									((unsigned long long)((unsigned char *)b)[1] << 8) |
									(unsigned long long)((unsigned char *)b)[0];
	else
		memcpy(a, b, 8);
}


char *aces_type_names[] = {"box2i",
							"chlist",
							"chromaticities",
							"compression",
							"double",
							"float",
							"half",
							"int",
							"lineOrder",
							"keycode",
							"rational",
							"short",
							"string",
							"stringVector",
							"timecode",
							"unsignedChar",
							"unsignedInt",
							"unsignedLong",
							"unsignedShort",
							"v2f",
							"v3f"};
unsigned int aces_type_min_size[] = {16, /* ACES_ATTRIBUTE_TYPE_BOX2I */
									18, /* ACES_ATTRIBUTE_TYPE_CHLIST */
									4 * 8, /* ACES_ATTRIBUTE_TYPE_CHROMATICITIES */
									1, /* ACES_ATTRIBUTE_TYPE_COMPRESSION */
									8, /* ACES_ATTRIBUTE_TYPE_DOUBLE */
									4, /* ACES_ATTRIBUTE_TYPE_FLOAT */
									2, /* ACES_ATTRIBUTE_TYPE_HALF */
									4, /* ACES_ATTRIBUTE_TYPE_INT */
									1, /* ACES_ATTRIBUTE_TYPE_LINEORDER */
									4 * 7, /* ACES_ATTRIBUTE_TYPE_KEYCODE */
									4 * 2, /* ACES_ATTRIBUTE_TYPE_RATIONAL */
									2, /* ACES_ATTRIBUTE_TYPE_SHORT */
									2, /* ACES_ATTRIBUTE_TYPE_STRING */
									2, /* ACES_ATTRIBUTE_TYPE_STRINGVECTOR */
									4 * 4, /* ACES_ATTRIBUTE_TYPE_TIMECODE */
									1, /* ACES_ATTRIBUTE_TYPE_UNSIGNEDCHAR */
									4, /* ACES_ATTRIBUTE_TYPE_UNSIGNEDINT */
									8, /* ACES_ATTRIBUTE_TYPE_UNSIGNEDLONG */
									2, /* ACES_ATTRIBUTE_TYPE_UNSIGNEDSHORT */
									4 * 2, /* ACES_ATTRIBUTE_TYPE_V2F */
									4 * 3, /* ACES_ATTRIBUTE_TYPE_V3F */
									0}; /* ACES_ATTRIBUTE_TYPE_UNKNOWN */




typedef struct{
	char *name;
	ACESAttributeTypes type;
}ACESAttributePredefined;

typedef struct{
	unsigned int layer;
	unsigned int channel;
	ACESAttributeClistPixelType type;
}HxAChannelRead;

//"endOfHeaderOffset", ACES_ATTRIBUTE_TYPE_UNSIGNED_LONG
//"free", ACES_ATTRIBUTE_TYPE_STRING,
//"headerChecksum", ACES_ATTRIBUTE_TYPE_STRING,
//"imageChecksum", ACES_ATTRIBUTE_TYPE_STRING,

#define ACES_ATTRIBUTES_PREDEFINED 54
#define ACES_ATTRIBUTES_REQUIRED_OPENEXR 8
#define ACES_ATTRIBUTES_REQUIRED_ACES 11
#define ACES_ATTRIBUTES_IGNORE 4

ACESAttributePredefined aces_attribute_predefined[] = {
	/* OpenEXR Required attributes.*/
	"channels", ACES_ATTRIBUTE_TYPE_CHLIST,
	"compression", ACES_ATTRIBUTE_TYPE_COMPRESSION,
	"dataWindow", ACES_ATTRIBUTE_TYPE_BOX2I,
	"displayWindow", ACES_ATTRIBUTE_TYPE_BOX2I,
	"lineOrder", ACES_ATTRIBUTE_TYPE_LINEORDER,
	"pixelAspectRatio", ACES_ATTRIBUTE_TYPE_FLOAT,
	"screenWindowCenter", ACES_ATTRIBUTE_TYPE_V2F,
	"screenWindowWidth", ACES_ATTRIBUTE_TYPE_FLOAT,
	/* ACES Required attributes.*/
	"acesImageContainerFlag", ACES_ATTRIBUTE_TYPE_INT,
	"adoptedNeutral", ACES_ATTRIBUTE_TYPE_V2F,
	"chromaticities", ACES_ATTRIBUTE_TYPE_CHROMATICITIES,
	/* ignore the following*/
	"endOfHeaderOffset", ACES_ATTRIBUTE_TYPE_UNSIGNEDLONG,
	"free", ACES_ATTRIBUTE_TYPE_STRING,
	"headerChecksum", ACES_ATTRIBUTE_TYPE_STRING,
	"imageChecksum", ACES_ATTRIBUTE_TYPE_STRING,
	/* make sure the following has right name/type combo */
	"altitude", ACES_ATTRIBUTE_TYPE_FLOAT,
	"aperture", ACES_ATTRIBUTE_TYPE_FLOAT,
	"cameraFirmwareVersion", ACES_ATTRIBUTE_TYPE_STRING,
	"cameraIPAddress", ACES_ATTRIBUTE_TYPE_STRING,
	"cameraLabel", ACES_ATTRIBUTE_TYPE_STRING,
	"cameraMake", ACES_ATTRIBUTE_TYPE_STRING,
	"cameraModel", ACES_ATTRIBUTE_TYPE_STRING,
	"cameraOrientation", ACES_ATTRIBUTE_TYPE_V3F,
	"comments", ACES_ATTRIBUTE_TYPE_STRING,
	"convergenceDistance", ACES_ATTRIBUTE_TYPE_FLOAT,
	"creator", ACES_ATTRIBUTE_TYPE_STRING,
	"expTime", ACES_ATTRIBUTE_TYPE_FLOAT,
	"focalLength", ACES_ATTRIBUTE_TYPE_FLOAT,
	"focus", ACES_ATTRIBUTE_TYPE_FLOAT,
	"framesPerSecond", ACES_ATTRIBUTE_TYPE_RATIONAL,
	"imageCounter", ACES_ATTRIBUTE_TYPE_INT,
	"imageRotation", ACES_ATTRIBUTE_TYPE_FLOAT,
	"interocularDistance", ACES_ATTRIBUTE_TYPE_FLOAT,
	"isoSpeed", ACES_ATTRIBUTE_TYPE_FLOAT,
	"keyCode", ACES_ATTRIBUTE_TYPE_KEYCODE,
	"latitude", ACES_ATTRIBUTE_TYPE_FLOAT,
	"lensAttributes", ACES_ATTRIBUTE_TYPE_STRING,
	"lensMake", ACES_ATTRIBUTE_TYPE_STRING,
	"lensModel", ACES_ATTRIBUTE_TYPE_STRING,
	"lensSerialNumber", ACES_ATTRIBUTE_TYPE_STRING,
	"longitude", ACES_ATTRIBUTE_TYPE_FLOAT,
	"multiView", ACES_ATTRIBUTE_TYPE_STRINGVECTOR,
	"originalImageFlag", ACES_ATTRIBUTE_TYPE_INT, // Support
	"owner", ACES_ATTRIBUTE_TYPE_STRING,
	"recorderFirmwareVersion", ACES_ATTRIBUTE_TYPE_STRING,
	"recorderMake", ACES_ATTRIBUTE_TYPE_STRING,
	"recorderModel", ACES_ATTRIBUTE_TYPE_STRING,
	"recorderSerialNumber", ACES_ATTRIBUTE_TYPE_STRING,
	"reelName", ACES_ATTRIBUTE_TYPE_STRING,
	"storageMediaSerialNumber", ACES_ATTRIBUTE_TYPE_STRING,
	"timeCode", ACES_ATTRIBUTE_TYPE_TIMECODE,
	"timecodeRate", ACES_ATTRIBUTE_TYPE_INT, 
	"utcOffset", ACES_ATTRIBUTE_TYPE_FLOAT, 
	"uuid", ACES_ATTRIBUTE_TYPE_STRING};



void aces_header_attribute_free(ACESAttributeType *attrib_array, unsigned int attrib_count)
{
	unsigned int i;
	for(i = 0; i < attrib_count; i++)
	{
		switch(attrib_array[i].type)
		{
			case ACES_ATTRIBUTE_TYPE_CHLIST :
				free(attrib_array[i].data.chlist.list);
			break;
			case ACES_ATTRIBUTE_TYPE_STRING :
				free(attrib_array[i].data.string.string);
			break;
			case ACES_ATTRIBUTE_TYPE_STRINGVECTOR :
				free(attrib_array[i].data.stringVector.string);
			break;
			case ACES_ATTRIBUTE_TYPE_UNKNOWN :
				free(attrib_array[i].data.unknown.type_name);
			break;
		}
	}
	free(attrib_array);
}

int aces_header_parse(ACESAttributeType **attrib_array, unsigned int *attrib_count, unsigned int *attrib_allocated, char *type, char *name, int length, unsigned char *data, int silent)
{
	ACESAttributeTypes aces_type;
	unsigned int i, pos = 0, pre_define;
	ACESAttributeType *attribute;
	for(aces_type = 0; aces_type < ACES_ATTRIBUTE_TYPE_COUNT; aces_type++)
	{
		for(i = 0; aces_type_names[aces_type][i] == type[i] && type[i] != 0; i++);
		if(aces_type_names[aces_type][i] == type[i])
			break;
	}
	for(pre_define = 0; pre_define < ACES_ATTRIBUTES_PREDEFINED; pre_define++)
	{
		for(i = 0; aces_attribute_predefined[pre_define].name[i] == name[i] && name[i] != 0; i++);
		if(aces_attribute_predefined[pre_define].name[i] == name[i])
		{
			if(aces_attribute_predefined[pre_define].type != aces_type)
			{
				if(!silent)
					printf("ACES Error: Predefined attibute %s found to have wrong type: %s. Standard requires attribute %s to be of type %s\n", name, aces_type_names[aces_type], name, aces_type_names[aces_attribute_predefined[pre_define].type]);
				return FALSE;
			}
			break;
		}
	}
	if(pre_define >= ACES_ATTRIBUTES_REQUIRED_OPENEXR && pre_define < ACES_ATTRIBUTES_REQUIRED_OPENEXR + ACES_ATTRIBUTES_IGNORE)
		return TRUE;

	if(aces_type_min_size[aces_type] > length)
	{
		if(!silent)
			printf("ACES Error: Attibute %s of type %s only has %u bytes of storage but require at least %u bytes.\n", name, type, length, aces_type_min_size[aces_type]);
		return FALSE;
	}
	if(pre_define < ACES_ATTRIBUTES_REQUIRED_OPENEXR)
		attribute = &(*attrib_array)[pre_define];
	else if(*attrib_count < *attrib_allocated)
		attribute = &(*attrib_array)[(*attrib_count)++];
	else
	{
		*attrib_allocated += 16;
		*attrib_array = realloc(*attrib_array, (sizeof *attribute) * *attrib_allocated);			
		attribute = &(*attrib_array)[(*attrib_count)++];
	}

	for(i = 0; name[i] != 0; i++)
		attribute->name[i] = name[i];
	attribute->name[i] = 0;
	attribute->type = aces_type;
	switch(aces_type)
	{
		case ACES_ATTRIBUTE_TYPE_BOX2I :
			aces_copy32(&attribute->data.box2i.x_min, &data[0]);
			aces_copy32(&attribute->data.box2i.y_min, &data[4]);
			aces_copy32(&attribute->data.box2i.x_max, &data[8]);
			aces_copy32(&attribute->data.box2i.y_max, &data[12]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_CHLIST :
			attribute->data.chlist.length = 0;
			attribute->data.chlist.list = malloc((sizeof *attribute->data.chlist.list) * (length / (2 + 4 + 4 + 4 + 4)));
			while(data[pos] != 0)
			{
				for(i = 0; i < 255 && pos + 16 < length && data[pos] != 0; i++)
					attribute->data.chlist.list[attribute->data.chlist.length].name[i] = data[pos++];
				if(i >= 255 || pos + 16 + 1 > length)
				{
					if(!silent)
						printf("ACES Error: Attibute %s of type clist is not correctly null terminated\n", name);
					return FALSE;
				}
				pos++;
				attribute->data.chlist.list[attribute->data.chlist.length].name[i] = 0;
				aces_copy32(&attribute->data.chlist.list[attribute->data.chlist.length].pixelType, &data[pos]);
				aces_copy32(&attribute->data.chlist.list[attribute->data.chlist.length].pLinear, &data[pos + 4]);
				aces_copy32(&attribute->data.chlist.list[attribute->data.chlist.length].xSampling, &data[pos + 8]);
				aces_copy32(&attribute->data.chlist.list[attribute->data.chlist.length].ySampling, &data[pos + 12]);
				pos += 16;
				attribute->data.chlist.length++;
			}
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_CHROMATICITIES :
			aces_copy32(&attribute->data.chromaticities.redX, &data[0]);
			aces_copy32(&attribute->data.chromaticities.redY, &data[4]);
			aces_copy32(&attribute->data.chromaticities.greenX, &data[8]);
			aces_copy32(&attribute->data.chromaticities.greenY, &data[12]);
			aces_copy32(&attribute->data.chromaticities.blueX, &data[16]);
			aces_copy32(&attribute->data.chromaticities.blueY, &data[20]);
			aces_copy32(&attribute->data.chromaticities.whiteX, &data[24]);
			aces_copy32(&attribute->data.chromaticities.whiteY, &data[28]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_COMPRESSION :
			attribute->data.compression = data[0];
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_DOUBLE :
			aces_copy64(&attribute->data.type_double, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_FLOAT :
			aces_copy32(&attribute->data.type_float, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_HALF :
			aces_copy16(&attribute->data.box2i.x_min, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_INT :
			aces_copy32(&attribute->data.type_int, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_LINEORDER :
			attribute->data.lineOrder = data[0];
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_KEYCODE :
			aces_copy32(&attribute->data.keycode.filmMfCCode, &data[0]);
			aces_copy32(&attribute->data.keycode.filmType, &data[4]);
			aces_copy32(&attribute->data.keycode.prefix, &data[8]);
			aces_copy32(&attribute->data.keycode.count, &data[12]);
			aces_copy32(&attribute->data.keycode.perfOffset, &data[16]);
			aces_copy32(&attribute->data.keycode.perfsPerFrame, &data[20]);
			aces_copy32(&attribute->data.keycode.perfsPerCount, &data[24]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_RATIONAL :
			aces_copy32(&attribute->data.rational.n, &data[0]);
			aces_copy32(&attribute->data.rational.b, &data[4]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_SHORT :
			aces_copy16(&attribute->data.type_short, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_STRING :
			attribute->data.string.length = length;
			attribute->data.string.string = malloc(length + 1);
			memcpy(attribute->data.string.string, data, length);
			attribute->data.string.string[length] = 0;
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_STRINGVECTOR :
			attribute->data.string.length = length;
			attribute->data.string.string = malloc(length + 1);
			memcpy(attribute->data.string.string, data, length);
			attribute->data.string.string[length] = 0;
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_TIMECODE :
			aces_copy32(&attribute->data.timecode.timeAndFlags, &data[0]);
			aces_copy32(&attribute->data.timecode.userData, &data[4]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_UNSIGNEDCHAR :
			attribute->data.unsignedChar = data[0];
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_UNSIGNEDINT :
			aces_copy32(&attribute->data.unsignedInt, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_UNSIGNEDLONG :
			aces_copy64(&attribute->data.unsignedLong, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_UNSIGNEDSHORT :
			aces_copy16(&attribute->data.unsignedShort, &data[0]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_V2F :
			aces_copy32(&attribute->data.v2f[0], &data[0]);
			aces_copy32(&attribute->data.v2f[1], &data[4]);
		return TRUE;
		case ACES_ATTRIBUTE_TYPE_V3F :
			aces_copy32(&attribute->data.v3f[0], &data[0]);
			aces_copy32(&attribute->data.v3f[1], &data[4]);
			aces_copy32(&attribute->data.v3f[2], &data[8]);
		return TRUE;
		case  ACES_ATTRIBUTE_TYPE_UNKNOWN :
			for(i = 0; type[i] != 0; i++);
			attribute->data.unknown.type_name = malloc(++i + length);
			memcpy(attribute->data.unknown.type_name, type, i);
			attribute->data.unknown.data = &attribute->data.unknown.type_name[i];
			attribute->data.unknown.length = length;
			memcpy(attribute->data.unknown.data, data, length);
		return TRUE;
	}
	return FALSE;
}

char *aces_file_load(char *file_name, size_t *size)
{
	char *buffer;
	unsigned int allocation, i;
	FILE *f;
	f = fopen(file_name, "rb");
	if(f == NULL)
	{
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	allocation = ftell(f);
	rewind(f);
	buffer = malloc(allocation + 1);
	memset(buffer, 0, allocation + 1);
	fread(buffer, 1, allocation, f);
	i = ftell(f);
	fclose(f);
	buffer[allocation] = 0;
	if(size != NULL)
		*size = allocation;
	return buffer;
}


float aces_float16_to_float32(unsigned short value)
{
	union { int integer; float real; }convert;
	int sign, exponent, sig;
	sign = (value >> 15) & 0x00000001;
	exponent = (value >> 10) & 0x0000001f;
	sig = value & 0x000003ff;

	if (exponent == 0)
	{
		if (sig == 0)
		{
			convert.integer = sign << 31;
			return convert.real;
		}
		else
		{
			while (!(sig & 0x00000400))
			{
				sig <<= 1;
				exponent -= 1;
			}
			exponent += 1;
			sig &= ~0x00000400;
		}
	}
	else if (exponent == 31)
	{
		if (sig == 0)
		{
			convert.integer = (sign << 31) | 0x7f800000;
			return convert.real;
		}
		else
		{
			convert.integer = (sign << 31) | 0x7f800000 | (sig << 13);
			return convert.real;
		}
	}

	exponent = exponent + (127 - 15);
	sig = sig << 13;

	convert.integer = (sign << 31) | (exponent << 23) | sig;
	return convert.real;
}

short aces_float32_to_float16(float value)
{
	union{int integer; float real;}convert;
	int sign, exponent, sig, t, a, b;
	convert.real = value;

	sign =  (convert.integer >> 16) & 0x00008000;
	exponent = ((convert.integer >> 23) & 0x000000ff) - (127 - 15);
	sig = convert.integer & 0x007fffff;

    if(exponent <= 0)
    {
		if(exponent < -10)
			return sign;
		sig = sig | 0x00800000;
		t = 14 - exponent;
		a = (1 << (t - 1)) - 1;
		b = (sig >> t) & 1;

		sig = (sig + a + b) >> t;
		return sign | sig;
	}else if(exponent == 0xff - (127 - 15))
	{
		if(sig == 0)
			return sign | 0x7c00;
		else
		{
			sig >>= 13;
			return sign | 0x7c00 | sig | (sig == 0);
		}
    }else
    {
		sig = sig + 0x00000fff + ((sig >> 13) & 1);

		if(sig & 0x00800000)
		{
			sig =  0;
			exponent += 1;
		}
		if (exponent > 30)
			return sign | 0x7c00;	/*inf*/
		return sign | (exponent << 10) | (sig >> 13);
    }
}


/*
	ACES_TC_NO_NOT_CONVERT,
	ACES_TC_CONVERT_ALL_FLOAT16_TO_FLOAT32,
	ACES_TC_CONVERT_ALL_FLOAT32_TO_FLOAT16
*/

int aces_scanline_uncompressed(ACESImage *image, unsigned char *data, size_t size, unsigned int x, unsigned int y, HxAChannelRead *channel_reads, int silent, ACESTypeConvesion conversion)
{
	unsigned int i, j, pos = 0, line, line_size, stride, *uint32_write, *uint32_read;
	unsigned short *uint16_read, *uint16_write, uint16_tmp;
	float *float32_read, *float32_write, uint32_tmp;
	if(size < 8)
	{
		if(!silent)
			printf("ACES Error: Scanline out of file rage.\n");
		return FALSE;			
	}
	aces_copy32(&line, &data[pos]);
	aces_copy32(&line_size, &data[pos + 4]);
	pos += 8;
	for(i = 0; i < image->attributes[0].data.chlist.length; i++)
	{
		if(channel_reads[i].type == ACES_ACPT_UINT32 ||  channel_reads[i].type == ACES_ACPT_FLOAT32)
		{
			if(size < pos + 4 * x)
			{
				if(!silent)
					printf("ACES Error: Scanline out of file rage.\n");
				return FALSE;	
			}
		}

		if(channel_reads[i].type == ACES_ACPT_UINT32 || (channel_reads[i].type == ACES_ACPT_FLOAT32 && conversion != ACES_TC_CONVERT_ALL_FLOAT32_TO_FLOAT16))
		{
			stride = image->layers[channel_reads[i].layer].channel_count;
			uint32_write = &image->layers[channel_reads[i].layer].pixels.integer_buffer[stride * line * x + channel_reads[i].channel];
			uint32_read = &data[pos];
			for(j = 0; j < x; j++)
			{
				aces_copy32(uint32_write, uint32_read);
				uint32_read++;
				uint32_write += stride;
			}
			pos += x * sizeof(unsigned int);
		}else if(channel_reads[i].type == ACES_ACPT_FLOAT32)
		{
			stride = image->layers[channel_reads[i].layer].channel_count;
			uint16_write = &image->layers[channel_reads[i].layer].pixels.half_buffer[stride * line * x + channel_reads[i].channel];
			uint32_read = (unsigned int *)&data[pos];
			for(j = 0; j < x; j++)
			{
				aces_copy32(&uint16_tmp, uint32_read);
				uint32_read++;
				*uint16_write = aces_float16_to_float32(uint16_tmp);
				uint16_write += stride;
			}
			pos += x * sizeof(unsigned int);
		}else if(channel_reads[i].type == ACES_ACPT_HALF16 && conversion == ACES_TC_CONVERT_ALL_FLOAT16_TO_FLOAT32)
		{
			stride = image->layers[channel_reads[i].layer].channel_count;
			float32_write = &image->layers[channel_reads[i].layer].pixels.float_buffer[stride * line * x + channel_reads[i].channel];
			uint16_read = (unsigned short *)&data[pos];
			for(j = 0; j < x; j++)
			{
				aces_copy16(&uint16_tmp, uint16_read);
				uint16_read++;
				*float32_write = aces_float16_to_float32(uint16_tmp);
				float32_write += stride;
			}
			pos += x * sizeof(unsigned short);
		}else
		{
			stride = image->layers[channel_reads[i].layer].channel_count;
			uint16_write = &image->layers[channel_reads[i].layer].pixels.half_buffer[stride * line * x + channel_reads[i].channel];
			uint16_read = &data[pos];
			for(j = 0; j < x; j++)
			{
				aces_copy16(uint16_write, uint16_read);
				uint16_read++;
				uint16_write += stride;
			}
			pos += x * sizeof(unsigned short);
		}
	}	
	return TRUE;		
}

void aces_free(ACESImage *image)
{
	unsigned int i;
	aces_header_attribute_free(image->attributes, image->attribute_count);
	for(i = 0; i < image->layer_count; i++)
		if(image->layers[i].pixels.float_buffer != NULL)
			free(image->layers[i].pixels.float_buffer);
	free(image->layers);
	free(image);
}



void aces_channel_list(ACESImage *file, ACESAttributeClist *list, unsigned int list_length, HxAChannelRead *reads, unsigned int silent, ACESTypeConvesion conversion)
{
	char channel_names[] = {'R', 'G', 'B', 'A', 'X', 'Y', 'Z'};
	int period;
	unsigned int i, j, k, channel;
	file->layers = malloc((sizeof *file->layers) * list_length);
	file->layer_count = 0;	 
	for(i = 0; i < list_length; i++)
	{
		period = FALSE;
		channel = 0;
		for(j = 0; list[i].name[j] != 0; j++)	
			if(list[i].name[j] == '.')
				period = TRUE;
		if(j == 1 || period)
		{
			for(k = 0; k < 7; k++)
			{
				if(list[i].name[j - 1] == channel_names[k])
				{
					period = TRUE;
					list[i].name[j - 1] = 0;
					channel = k % 4;
					break;
				}
			}
		}
		if(period)
		{
			for(j = 0; j < file->layer_count; j++)
			{
				for(k = 0; list[i].name[k] != 0 && list[i].name[k] == file->layers[j].name[k]; k++);
				if(list[i].name[k] == file->layers[j].name[k])
					break;
			}
		}else
			j = file->layer_count;
		if(j == file->layer_count)
		{
			file->layer_count++;
			for(k = 0; k < list[i].name[k]; k++)
				file->layers[j].name[k] = list[i].name[k];
			file->layers[j].name[k] = 0;
			file->layers[j].channel_count = channel + 1;
		}
		else
			if(channel + 1 > file->layers[j].channel_count) 
				file->layers[j].channel_count = channel + 1;

		reads[i].layer = j;
		reads[i].channel = channel;
		reads[i].type = list[i].pixelType;
	}

	for(i = 0; i < file->layer_count; i++)
	{
		if((reads[i].type == ACES_ACPT_HALF16 && conversion != ACES_TC_CONVERT_ALL_FLOAT16_TO_FLOAT32) || 
			(reads[i].type == ACES_ACPT_FLOAT32 && conversion == ACES_TC_CONVERT_ALL_FLOAT32_TO_FLOAT16))
			file->layers[i].pixels.half_buffer = malloc(sizeof(unsigned short) * file->layers[j].channel_count * (unsigned int)(file->attributes[2].data.box2i.x_max + 1 - file->attributes[2].data.box2i.x_min) * (unsigned int)(file->attributes[2].data.box2i.y_max + 1 - file->attributes[2].data.box2i.y_min));
		else
			file->layers[i].pixels.float_buffer = malloc(sizeof(float) * file->layers[j].channel_count * (unsigned int)(file->attributes[2].data.box2i.x_max + 1 - file->attributes[2].data.box2i.x_min) * (unsigned int)(file->attributes[2].data.box2i.y_max + 1 - file->attributes[2].data.box2i.y_min));

	}
}


ACESImage *aces_load(char *file_path, int silent, ACESTypeConvesion conversion)
{
	unsigned char *data;
	size_t size, pos;
	unsigned long long offset;
	unsigned int i, magic_number = 0, version = 0, x, y;
	HxAChannelRead *channel_reads, channel_reads_buffer[64];
	ACESAttributeType *attrib_array;
	ACESImage *image;
	unsigned int attribute_count, attrib_allocated = ACES_ATTRIBUTES_PREDEFINED;
	attribute_count = ACES_ATTRIBUTES_REQUIRED_ACES;
	data = aces_file_load(file_path, &size);
	f_print_raw(data, 4096);
	if(data == NULL)
	{
		if(!silent)
			printf("ACES Error: File %s not found.\n", file_path);
		return FALSE;
	}
	if(size < 4)
	{
		if(!silent)
			printf("ACES Error: Unexpected end of file found.\n");
		free(data);
		return FALSE;
	}
	aces_copy32(&magic_number, data);
	if(magic_number != 20000630)
	{
		if(!silent)
			printf("ACES Error: Filw is not a valid OpenEXR/ACES file (wrong magic number).\n");
		free(data);
		return FALSE;
	}
	pos = 4;
	aces_copy32(&version, &data[pos]);

/*	if(magic_number != 20000630)
	{
		if(!silent)
			printf("ACES Error: Filw is not a valid OpenEXR/ACES file (wrong magic number).\n");
		return FALSE;
	}*/
	pos += 4;
	attrib_array = malloc((sizeof *attrib_array) * attrib_allocated);
	for(i = 0; i < ACES_ATTRIBUTES_REQUIRED_ACES; i++)
		attrib_array[i].type = ACES_ATTRIBUTE_TYPE_UNUSED;
	while(data[pos] != 0)
	{
		char name[256], type[256];
		int length;
		for(i = 0; data[pos] != 0; i++)
		{
			if(i == 255)
			{
				name[255] = 0; 
				if(!silent)
					printf("ACES Error: File attribute <%s> is not termnated\n", name);
				aces_header_attribute_free(attrib_array, attribute_count);
				free(data);
				return NULL;
			}
			name[i] = data[pos++];
		}
		name[i] = 0;
		pos++;
		for(i = 0; data[pos] != 0; i++)
		{
			if(i == 255)
			{
				type[255] = 0; 
				if(!silent)
					printf("ACES Error: File attribute %s has a non termnated type <%s>\n", name, type);
				aces_header_attribute_free(attrib_array, attribute_count);
				free(data);
				return NULL;
			}
			type[i] = data[pos++];
		}
		type[i] = 0;
		pos++;
		memcpy(&length, &data[pos], 4);
		if(length < 0 || length > 1024 * 1024 + 4 - pos)
		{
			if(!silent)
				printf("ACES Error: Unexpected end of file at attribute %s with length <%u>\n", name, length);
			aces_header_attribute_free(attrib_array, attribute_count);
			free(data);
			return NULL;
		}
		pos += 4;
		if(!aces_header_parse(&attrib_array, &attribute_count, &attrib_allocated, type, name, length, &data[pos], silent))
		{
			aces_header_attribute_free(attrib_array, attribute_count);
			free(data);
			return NULL;
		}	
		pos += length;
	}
	pos++;
	if(attrib_array[0].type != ACES_ATTRIBUTE_TYPE_CHLIST)
	{
		if(!silent)
			printf("ACES Error: no chanel list found in file.\n");
		aces_header_attribute_free(attrib_array, attribute_count);
		free(data);
		return NULL;
	}
	if(attrib_array[1].type != ACES_ATTRIBUTE_TYPE_COMPRESSION)
	{
		if(!silent)
			printf("ACES Error: no compression attribute found in file.\n");
		aces_header_attribute_free(attrib_array, attribute_count);
		free(data);
		return NULL;
	}
	if(attrib_array[1].data.compression != 0)
	{
		if(!silent)
			printf("ACES Error: The compression (%u) in this file is not supported by ACES. ACES Only supports uncompressed files (0).\n", (unsigned int)attrib_array[1].data.compression);
		aces_header_attribute_free(attrib_array, attribute_count);
		free(data);
		return NULL;
	}
	if(attrib_array[2].type != ACES_ATTRIBUTE_TYPE_BOX2I)
	{
		if(!silent)
			printf("ACES Error: no data window found in file.\n");
		aces_header_attribute_free(attrib_array, attribute_count);
		free(data);
		return NULL;
	}
	if(attrib_array[2].data.box2i.x_max < attrib_array[2].data.box2i.x_min || attrib_array[2].data.box2i.y_max < attrib_array[2].data.box2i.y_min)
	{
		if(!silent)
			printf("ACES Error: File has negative image size.\n");
		aces_header_attribute_free(attrib_array, attribute_count);
		free(data);
		return NULL;
	}
	image = malloc(sizeof *image);
	image->attributes = attrib_array;
	image->attribute_allocate = attrib_allocated;
	image->attribute_count = attribute_count;
	x = (unsigned int)(attrib_array[2].data.box2i.x_max + 1 - attrib_array[2].data.box2i.x_min);
	y = (unsigned int)(attrib_array[2].data.box2i.y_max + 1 - attrib_array[2].data.box2i.y_min);
	if(attrib_array[0].data.chlist.length <= 64)
		channel_reads = channel_reads_buffer;
	else
		channel_reads = malloc((sizeof *channel_reads) * attrib_array[0].data.chlist.length);
	aces_channel_list(image, attrib_array[0].data.chlist.list, attrib_array[0].data.chlist.length, channel_reads, silent, conversion);

	if(pos + y * 8 > size)
	{
		if(!silent)
			printf("ACES Error: Unexpected end of file in scanline table.\n");
		aces_free(image);
		free(data);
		return NULL;
	}

	for(i = 0; i < y; i++)
	{
		aces_copy64(&offset, &data[pos + i * 8]);
		if(size < offset)
		{
			if(!silent)
				printf("ACES Error: Scanline table points to a position outside file\n");
			aces_free(image);
			free(data);
			return NULL;
		}
		if(!aces_scanline_uncompressed(image, &data[offset], size - offset, x, y, channel_reads, silent, conversion))
		{
			aces_free(image);
			free(data);
			return NULL;
		}
	}
	if(channel_reads != channel_reads_buffer)
		free(channel_reads);
	free(data);
	return image;
}

/*
void *aces_load_test(char *file_path, int silent, unsigned int *x, unsigned int *y)
{
	ACESImage *image;
	image = aces_load(file_path, TRUE, ACES_TC_CONVERT_ALL_FLOAT32_TO_FLOAT16);	
	*x = (unsigned int)(image->attributes[2].data.box2i.x_max + 1 - image->attributes[2].data.box2i.x_min);
	*y = (unsigned int)(image->attributes[2].data.box2i.y_max + 1 - image->attributes[2].data.box2i.y_min);
	return image->layers[0].pixels.float_buffer;
}
*/