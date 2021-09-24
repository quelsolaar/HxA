#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hxa.h"
#include "hxa_util_fbx_internal.h"


typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint;
typedef float real32;
typedef double real64;
typedef unsigned char boolean;
#define FALSE 0
#define TRUE !FALSE

extern int hxa_inflate(unsigned char *output, size_t *output_length, unsigned char *input, size_t input_length);

const unsigned int fbx_property_size[FBX_PROPERTY_TYPE_COUNT] = {sizeof(char), //	FBX_PROPERTY_TYPE_BOOLEAN
														sizeof(char), //	FBX_PROPERTY_TYPE_INT8
														sizeof(short), //	FBX_PROPERTY_TYPE_INT16
														sizeof(int), //	FBX_PROPERTY_TYPE_INT32
														sizeof(long long), //	FBX_PROPERTY_TYPE_INT64
														sizeof(float), //	FBX_PROPERTY_TYPE_FLOAT32
														sizeof(double), //	FBX_PROPERTY_TYPE_FLOAT64
														sizeof(char), //	FBX_PROPERTY_TYPE_TEXT
														sizeof(unsigned char), //	FBX_PROPERTY_TYPE_RAW
														0}; //	FBX_PROPERTY_TYPE_UNKNOWN

const uint	fbx_property_code[256] = {FBX_PROPERTY_TYPE_UNKNOWN, /* NULL */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Start of header */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Start of text */
										FBX_PROPERTY_TYPE_UNKNOWN, /* End of text */
										FBX_PROPERTY_TYPE_UNKNOWN, /* End of transmission */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Enquiry */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Acknowledgment */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Bell */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Backspace */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Horizontal Tab */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Line feed */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Vertical tab */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Form feed */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Carriage return */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Shift out */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Shift in */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Data Link Escape */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Device control 1 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Device control 2 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Device control 3 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Device control 4 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Negative Acknowledgment */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Syncronus idle */
										FBX_PROPERTY_TYPE_UNKNOWN, /* End of transmission block */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Cansel */
										FBX_PROPERTY_TYPE_UNKNOWN, /* End of medium */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Substitute */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Ecscape */
										FBX_PROPERTY_TYPE_UNKNOWN, /* File separator */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Group separator */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Record separator */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Unit separator */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Space */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ! */
										FBX_PROPERTY_TYPE_UNKNOWN, /* " */
										FBX_PROPERTY_TYPE_UNKNOWN, /* # */
										FBX_PROPERTY_TYPE_UNKNOWN, /* $ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* % */
										FBX_PROPERTY_TYPE_UNKNOWN, /* & */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ' */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ( */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ) */
										FBX_PROPERTY_TYPE_UNKNOWN, /* * */
										FBX_PROPERTY_TYPE_UNKNOWN, /* + */
										FBX_PROPERTY_TYPE_UNKNOWN, /* , */
										FBX_PROPERTY_TYPE_UNKNOWN, /* - */
										FBX_PROPERTY_TYPE_UNKNOWN, /* . */
										FBX_PROPERTY_TYPE_UNKNOWN, /* / */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 0 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 1 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 2 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 3 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 4 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 5 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 6 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 7 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 8 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 9 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* : */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ; */
										FBX_PROPERTY_TYPE_UNKNOWN, /* < */
										FBX_PROPERTY_TYPE_UNKNOWN, /* = */
										FBX_PROPERTY_TYPE_UNKNOWN, /* > */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ? */
										FBX_PROPERTY_TYPE_UNKNOWN, /* @ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* A */
										FBX_PROPERTY_TYPE_UNKNOWN, /* B */
										FBX_PROPERTY_TYPE_BOOLEAN, /* C */
										FBX_PROPERTY_TYPE_FLOAT64, /* D */
										FBX_PROPERTY_TYPE_UNKNOWN, /* E */
										FBX_PROPERTY_TYPE_FLOAT32, /* F */
										FBX_PROPERTY_TYPE_UNKNOWN, /* G */
										FBX_PROPERTY_TYPE_UNKNOWN, /* H */
										FBX_PROPERTY_TYPE_INT32, /* I */
										FBX_PROPERTY_TYPE_UNKNOWN, /* J */
										FBX_PROPERTY_TYPE_UNKNOWN, /* K */
										FBX_PROPERTY_TYPE_INT64, /* L */
										FBX_PROPERTY_TYPE_UNKNOWN, /* M */
										FBX_PROPERTY_TYPE_UNKNOWN, /* N */
										FBX_PROPERTY_TYPE_UNKNOWN, /* O */
										FBX_PROPERTY_TYPE_UNKNOWN, /* P */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Q */
										FBX_PROPERTY_TYPE_RAW, /* R */
										FBX_PROPERTY_TYPE_TEXT, /* S */
										FBX_PROPERTY_TYPE_UNKNOWN, /* T */
										FBX_PROPERTY_TYPE_UNKNOWN, /* U */
										FBX_PROPERTY_TYPE_UNKNOWN, /* V */
										FBX_PROPERTY_TYPE_UNKNOWN, /* W */
										FBX_PROPERTY_TYPE_UNKNOWN, /* X */
										FBX_PROPERTY_TYPE_INT16, /* Y */
										FBX_PROPERTY_TYPE_UNKNOWN, /* Z */
										FBX_PROPERTY_TYPE_UNKNOWN, /* [ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* \ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ] */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ^ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* _ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ` */
										FBX_PROPERTY_TYPE_UNKNOWN, /* a */
										FBX_PROPERTY_TYPE_BOOLEAN, /* b */
										FBX_PROPERTY_TYPE_UNKNOWN, /* c */
										FBX_PROPERTY_TYPE_FLOAT64, /* d */
										FBX_PROPERTY_TYPE_UNKNOWN, /* e */
										FBX_PROPERTY_TYPE_FLOAT32, /* f */
										FBX_PROPERTY_TYPE_UNKNOWN, /* g */
										FBX_PROPERTY_TYPE_UNKNOWN, /* h */
										FBX_PROPERTY_TYPE_INT32, /* i */
										FBX_PROPERTY_TYPE_UNKNOWN, /* j */
										FBX_PROPERTY_TYPE_UNKNOWN, /* k */
										FBX_PROPERTY_TYPE_INT64, /* l */
										FBX_PROPERTY_TYPE_UNKNOWN, /* m */
										FBX_PROPERTY_TYPE_UNKNOWN, /* n */
										FBX_PROPERTY_TYPE_UNKNOWN, /* o */
										FBX_PROPERTY_TYPE_UNKNOWN, /* p */
										FBX_PROPERTY_TYPE_UNKNOWN, /* q */
										FBX_PROPERTY_TYPE_UNKNOWN, /* r */
										FBX_PROPERTY_TYPE_UNKNOWN, /* s */
										FBX_PROPERTY_TYPE_UNKNOWN, /* t */
										FBX_PROPERTY_TYPE_UNKNOWN, /* u */
										FBX_PROPERTY_TYPE_UNKNOWN, /* v */
										FBX_PROPERTY_TYPE_UNKNOWN, /* w */
										FBX_PROPERTY_TYPE_UNKNOWN, /* x */
										FBX_PROPERTY_TYPE_INT16, /* y */
										FBX_PROPERTY_TYPE_UNKNOWN, /* z */
										FBX_PROPERTY_TYPE_UNKNOWN, /* { */
										FBX_PROPERTY_TYPE_UNKNOWN, /* | */
										FBX_PROPERTY_TYPE_UNKNOWN, /* } */
										FBX_PROPERTY_TYPE_UNKNOWN, /* ~ */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 127 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 128 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 129 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 130 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 131 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 132 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 133 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 134 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 135 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 136 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 137 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 138 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 139 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 140 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 141 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 142 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 143 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 144 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 145 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 146 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 147 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 148 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 149 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 150 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 151 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 152 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 153 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 154 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 155 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 156 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 157 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 158 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 159 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 160 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 161 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 162 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 163 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 164 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 165 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 166 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 167 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 168 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 169 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 170 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 171 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 172 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 173 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 174 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 175 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 176 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 177 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 178 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 179 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 180 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 181 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 182 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 183 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 184 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 185 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 186 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 187 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 188 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 189 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 190 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 191 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 192 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 193 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 194 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 195 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 196 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 197 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 198 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 199 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 200 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 201 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 202 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 203 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 204 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 205 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 206 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 207 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 208 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 209 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 210 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 211 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 212 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 213 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 214 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 215 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 216 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 217 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 218 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 219 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 220 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 221 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 222 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 223 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 224 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 225 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 226 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 227 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 228 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 229 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 230 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 231 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 232 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 233 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 234 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 235 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 236 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 237 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 238 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 239 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 240 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 241 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 242 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 243 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 244 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 245 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 246 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 247 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 248 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 249 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 250 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 251 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 252 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 253 */
										FBX_PROPERTY_TYPE_UNKNOWN, /* 254 */
										FBX_PROPERTY_TYPE_UNKNOWN}; /* 255 */


static const union { uint8 bytes[4]; uint32 value; } forge_host_order = {{0, 1, 2, 3}};

void fbx_unpack_uint64_big(uint64 *output, uint8 *input)
{
	uint64 data;
	data = ((uint64) input[3]) << 56;
	data |= ((uint64) input[2]) << 48;
	data |= ((uint64) input[1]) << 40;
	data |= ((uint64) input[0]) << 32;
	data |= ((uint64) input[7]) << 24;
	data |= ((uint64) input[6]) << 16;
	data |= ((uint64) input[5]) << 8;
	data |= (uint64) input[4];
	*output = data;
}

void fbx_unpack_uint32_big(uint32 *output, uint8 *input)
{
	uint32 data;
	data = ((uint32) input[3]) << 24;
	data |= ((uint32) input[2]) << 16;
	data |= ((uint32) input[1]) << 8;
	data |= (uint32) input[0];
	*output = data;
}
void fbx_unpack_uint16_big(uint16 *output, uint8 *input)
{
	uint16 data;
	data = ((uint16) input[1]) << 8;
	data |= (uint16) input[0];
	*output = data;
}

#if _M_IX86 || _M_X64 || __x86_64__ || __i386 ||_X86_ || _X86_ || __INTEL__
#define FBX_LITTLE_ENDIAN 
#else
#define FBX_UNKNOWN_ENDIAN 
#endif

#ifdef FBX_LITTLE_ENDIAN
#define fbx_unpack_uint16(a, b) memcpy(a, b, sizeof(uint16))
#define fbx_unpack_uint32(a, b) memcpy(a, b, sizeof(uint32))
#define fbx_unpack_uint64(a, b) memcpy(a, b, sizeof(uint64))
#else
#define fbx_unpack_uint16 fbx_unpack_uint16_big
#define fbx_unpack_uint32 fbx_unpack_uint32_big
#define fbx_unpack_uint64 fbx_unpack_uint64_big

#endif

void fbx_record_print(FBXRecord *record, uint generation)
{
	void *array;
	char tabs[64];
	uint i, j;
	for(i = 0; i < generation && i < 64 - 1; i++)
		tabs[i] = ' ';
	tabs[i] = 0;
	printf("%send_ofset %llu\n", tabs, record->end_ofset);

	printf("%sproperty_count %llu\n", tabs, record->property_count);
	printf("%sproperty_list_length %llu\n", tabs, record->property_list_length);
	printf("%sname_length %u\n", tabs, (uint)record->name_length);
	printf("%sname %s\n", tabs, record->name);
	for(i = 0; i < record->property_count; i++)
	{
		printf("%sproperty[%u] type %u\n", tabs, i, record->property[i].type);
		if(record->property[i].array_length > 1)
			printf("%sproperty[%u] array_length %u\n", tabs, i, record->property[i].array_length);
		if(record->property[i].array_length == 1)
			array = &record->property[i].data.boolean_type;
		else
			array = record->property[i].data.array;

		printf("%sproperty[%u] value =", tabs, i);
		switch(record->property[i].type)
		{
			case FBX_PROPERTY_TYPE_BOOLEAN :
			for(j = 0; j < record->property[i].array_length; j++)
			{
				if(record->property[i].data.boolean_type)
					printf(" TRUE");
				else
					printf(" FALSE");
			}
			break;
			case FBX_PROPERTY_TYPE_INT8 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int8 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_INT16 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int16 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_INT32 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int32 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_INT64 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int64 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_FLOAT32 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %f", (float)(((float *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_FLOAT64 :
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %f", (float)(((double *)array)[j]));			
			break;
			case FBX_PROPERTY_TYPE_TEXT :
				printf(" \"%s\"", record->property[i].data.buffer->buffer.text);
			break;
			case FBX_PROPERTY_TYPE_RAW :
				printf(" RAW");
			break;
			case FBX_PROPERTY_TYPE_UNKNOWN :
				printf(" UNKNOWN");
			break;
		}
		printf("\n");
	}
	for(i = 0; i < record->used; i++)
		fbx_record_print(&((FBXRecord *)record->children)[i], generation + 1);
}


void fbx_record_print_small(FBXRecord *record, uint generation)
{
	void *array;
	uint i, j;
	for(i = 0; i < generation; i++)
		printf("  ");
	printf("name %s = ", record->name);
	for(i = 0; i < record->property_count; i++)
	{
	
		if(record->property[i].array_length == 1)
			array = &record->property[i].data.boolean_type;
		else
			array = record->property[i].data.array;
		if(i != 0)
			printf(",");
		switch(record->property[i].type)
		{
			case FBX_PROPERTY_TYPE_BOOLEAN :
			for(j = 0; j < record->property[i].array_length; j++)
			{
				if(record->property[i].data.boolean_type)
					printf(" TRUE");
				else
					printf(" FALSE");
			}
			break;
			case FBX_PROPERTY_TYPE_INT8 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int8 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_INT16 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int16 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_INT32 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int32 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_INT64 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %i", (int)(((int64 *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_FLOAT32 : 
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %f", (float)(((float *)array)[j]));
			break;
			case FBX_PROPERTY_TYPE_FLOAT64 :
				for(j = 0; j < record->property[i].array_length; j++)
					printf(" %f", (float)(((double *)array)[j]));			
			break;
			case FBX_PROPERTY_TYPE_TEXT :
				printf(" \"%s\"", record->property[i].data.buffer->buffer.text);
			break;
			case FBX_PROPERTY_TYPE_RAW :
				printf(" RAW");
			break;
			case FBX_PROPERTY_TYPE_UNKNOWN :
				printf(" UNKNOWN");
			break;
		}
	}
	printf("\n");
//	for(i = 0; i < record->used; i++)
//		fbx_record_print_small(&((FBXRecord *)record->children)[i], generation + 1);
}

/*
data[51] = 18 - Device control 2
data[52] = 70 - F - "FBXHeaderExtensiont"
*/

uint fbx_load_record(FBXRecord *record, uint8 *data, uint read_pos, uint version)
{
	uint i, j, k, type, encoding, compressed_length;
	uint32 read32;
	uint8 *array;
	uint8 *write, *read;
	record->allocated = 0;
	record->used = 0;
	record->children = 0;
	if(version < 7500)
	{
		fbx_unpack_uint32(&read32, &data[read_pos + 0]);
		record->end_ofset = (uint64)read32;
		fbx_unpack_uint32(&read32, &data[read_pos + 4]);
		record->property_count = (uint64)read32;
		fbx_unpack_uint32(&read32, &data[read_pos + 8]);
		record->property_list_length = (uint64)read32;
		record->name_length = data[read_pos + 12];
		for(i = 0; i < record->name_length; i++)
			record->name[i] = data[read_pos + 13 + i];
		record->name[i] = 0;
		i += 13;
	}else
	{	
		fbx_unpack_uint64(&record->end_ofset, &data[read_pos + 0]);
		fbx_unpack_uint64(&record->property_count, &data[read_pos + 8]);
		fbx_unpack_uint64(&record->property_list_length, &data[read_pos + 16]);
		record->name_length = data[read_pos + 24];
		for(i = 0; i < record->name_length; i++)
			record->name[i] = data[read_pos + 25 + i];
		record->name[i] = 0;
		i += 25;
	}
	if(record->property_list_length > 100000)
	{	
//		f_print_raw(data, 2000);
		record->property_list_length = 0;
		exit(0);
	}
//	if(record->property_list_length != 0)
//		f_print_raw(&data[i], record->property_list_length);
	if(record->property_count != 0)
	{
		record->property = malloc((sizeof *record->property) * record->property_count);
		for(j = 0; j < record->property_count; j++)
		{
			record->property[j].type = fbx_property_code[data[read_pos + i]];
		//	if(record->property[j].type == FBX_PROPERTY_TYPE_FLOAT64)
		//		printf("type %u %u %u\n", record->property[j].type, (uint)data[i], i);
			if(data[read_pos + i] > 'Z')
			{
				fbx_unpack_uint32(&record->property[j].array_length, &data[read_pos + i + 1]);
				fbx_unpack_uint32(&encoding, &data[read_pos + i + 5]);
				fbx_unpack_uint32(&compressed_length, &data[read_pos + i + 9]);
				i += 1 + sizeof(uint32) * 3;
				record->property[j].data.array = malloc(fbx_property_size[record->property[j].type] * record->property[j].array_length);
				if(compressed_length != 0 && encoding != 0)
				{
					uint output_length;
					output_length = compressed_length * 4;
					k = 0;
					while(1)
					{
				/*		uint8 *test;
						test = malloc(fbx_property_size[record->property[j].type] * record->property[j].array_length);
						hxa_inflate(test, &output_length, &data[read_pos + i + 2], compressed_length);
						if(TINF_OK == tinf_uncompress(record->property[j].data.array, &output_length, &data[read_pos + i + 2], compressed_length))
						{
							uint ii;
							for(ii = 0; ii < output_length; ii++)
								if(((uint8 *)record->property[j].data.array)[ii] != test[ii])
									ii++;
							break;
						}*/
						if(hxa_inflate(record->property[j].data.array, &output_length, &data[read_pos + i + 2], compressed_length))
							break;
						i++;
						k++;
					}
					i += compressed_length;
#ifdef FBX_LITTLE_ENDIAN
					read = write = NULL;
#else
					read = record->property[j].data.array;
					write = record->property[j].data.array;
#endif
				}else
				{
					read = &data[read_pos + i];
					write = record->property[j].data.array;
					i += fbx_property_size[record->property[j].type] * record->property[j].array_length;
				}
			}else
			{	 
				record->property[j].array_length = 1;
				i++;
				if(record->property[j].type == FBX_PROPERTY_TYPE_TEXT ||
					record->property[j].type == FBX_PROPERTY_TYPE_RAW)
				{
					uint32 length;
					fbx_unpack_uint32(&length, &data[read_pos + i]);
					i += sizeof(uint32);
					record->property[j].data.buffer = malloc(sizeof(uint32) + length + 1);
					record->property[j].data.buffer->length = length;
					memcpy(record->property[j].data.buffer->buffer.raw, &data[read_pos + i], length);
					record->property[j].data.buffer->buffer.raw[length] = 0;
					i += length;
					read = write = NULL;
				}else
				{
					write = &record->property[j].data;
					read = &data[read_pos + i];
					i += fbx_property_size[record->property[j].type];
				}
			}
			if(write != NULL)
			{
#ifdef FBX_LITTLE_ENDIAN
				memcpy(write, read, fbx_property_size[record->property[j].type] * record->property[j].array_length);
#else
				switch(fbx_property_size[record->property[j].type])
				{
					case 1 :
						memcpy(write, read, record->property[j].array_length);
					break;	
					case 2 :
						for(k = 0; k < record->property[j].array_length; k++)
							fbx_unpack_uint16((uint16 *)&write[k * sizeof(int16)], &read[k * sizeof(int16)]);
					break;	
					case 4 :
						for(k = 0; k < record->property[j].array_length; k++)
							fbx_unpack_uint32((uint16 *)&write[k * sizeof(int32)], &read[k * sizeof(int32)]);
					break;	
					case 8 :
						for(k = 0; k < record->property[j].array_length; k++)
							fbx_unpack_uint64((uint64 *)&write[k * sizeof(int64)], &read[k * sizeof(int64)]);
					break;			
				}
#endif
			}
		}
	}else
		record->property = NULL;
	read_pos += i;
	while(read_pos < record->end_ofset)
	{
		if(record->used == record->allocated)
		{
			record->allocated += 16;
			record->children = realloc(record->children, sizeof(FBXRecord) * record->allocated);
		}
		read_pos = fbx_load_record(&((FBXRecord *)record->children)[record->used++], data, read_pos, version);
	}	
	return read_pos;
}


char *hxa_util_fbx_file_load(char *file_name, size_t *size)
{
	char *buffer;
	uint allocation, i;
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

HXAFile *hxa_util_fbx_load(char *file_name, HXAFile *hxa_file)
{
	uint levels[1024], level_end[1024];
	FBXRecord record;
	uint32 version = 0;
	size_t size, i;
	uint8 *file;
//	freopen("output.txt", "w", stdout);
//	setbuf(stdout, NULL);
	file = hxa_util_fbx_file_load(file_name, &size);
	if(file == NULL)
		return NULL;
	record.end_ofset = size;
	record.property_count = 0;
	record.property_list_length = 0;
	record.name_length = 0;
	record.name[0] = 0;
	record.property = NULL;
	record.children = NULL;
	record.allocated = 0;
	record.used = 0;
	if(file == NULL)
		return NULL;
	fbx_unpack_uint32(&version, &file[23]);
	i = 27;
	while(i < record.end_ofset - 1000)
	{
		if(record.used == record.allocated)
		{
			record.allocated += 16;
			record.children = realloc(record.children, sizeof(FBXRecord) * record.allocated);
		}
		i = fbx_load_record(&((FBXRecord *)record.children)[record.used++], file, i, version);
	}	
	for(i = 0; i < record.used; i++)
		fbx_record_print_small(&((FBXRecord *)record.children)[i], 0);
	hxa_file = hxa_fbx_convert(&record, hxa_file);
	free(file);
	return hxa_file;
}
