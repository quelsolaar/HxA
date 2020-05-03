#include "hxa.h"
#include "hxa_utils.h"

#define NULL (void *)0

unsigned int hxa_util_array_export_type_sizes[HXA_UAET_COUNT] = {sizeof(char), sizeof(unsigned char), sizeof(short), sizeof(unsigned short), sizeof(int), sizeof(unsigned int), sizeof(short), sizeof(float), sizeof(double)};
void hxa_type_convert_vertex_uint8_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp > 127)
				tmp = 127;
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_vertex_uint8_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(unsigned char) * component_count);
	}
}


void hxa_type_convert_vertex_uint8_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_vertex_uint8_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_vertex_uint8_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_vertex_uint8_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_vertex_uint8_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_vertex_uint8_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_vertex_uint8_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	unsigned char *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_vertex_int32_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < -127)
				tmp = -127;
			if(tmp > 127)
				tmp = 127;
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_vertex_int32_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > 255)
				tmp = 255;
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_vertex_int32_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < -32767)
				tmp = -32767;
			if(tmp > 32767)
				tmp = 32767;
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_vertex_int32_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > 65535)
				tmp = 65535;
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_vertex_int32_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(int) * component_count);
	}
}


void hxa_type_convert_vertex_int32_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_vertex_int32_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_vertex_int32_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_vertex_int32_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	int *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_vertex_float32_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 127.0);
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_vertex_float32_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 255.0);
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_vertex_float32_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 32767.0);
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_vertex_float32_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 65535.0);
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_vertex_float32_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 2147483647.0);
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_vertex_float32_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 4294967295.0);
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_vertex_float32_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_vertex_float32_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(float) * component_count);
	}
}


void hxa_type_convert_vertex_float32_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	float *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_vertex_double64_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 127.0);
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_vertex_double64_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 255.0);
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_vertex_double64_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 32767.0);
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_vertex_double64_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 65535.0);
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_vertex_double64_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 2147483647.0);
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_vertex_double64_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 4294967295.0);
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_vertex_double64_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_vertex_double64_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_vertex_double64_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)
{
	double *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < vertex_length; i++)
	{
		read_type = &read[i * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(double) * component_count);
	}
}


void hxa_type_convert_reference_uint8_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp > 127)
				tmp = 127;
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_reference_uint8_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(unsigned char) * component_count);
	}
}


void hxa_type_convert_reference_uint8_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_reference_uint8_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_reference_uint8_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_reference_uint8_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_reference_uint8_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_reference_uint8_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_reference_uint8_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_reference_int32_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < -127)
				tmp = -127;
			if(tmp > 127)
				tmp = 127;
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_reference_int32_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > 255)
				tmp = 255;
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_reference_int32_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < -32767)
				tmp = -32767;
			if(tmp > 32767)
				tmp = 32767;
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_reference_int32_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > 65535)
				tmp = 65535;
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_reference_int32_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(int) * component_count);
	}
}


void hxa_type_convert_reference_int32_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_reference_int32_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_reference_int32_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_reference_int32_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_reference_float32_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 127.0);
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_reference_float32_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 255.0);
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_reference_float32_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 32767.0);
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_reference_float32_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 65535.0);
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_reference_float32_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 2147483647.0);
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_reference_float32_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 4294967295.0);
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_reference_float32_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_reference_float32_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(float) * component_count);
	}
}


void hxa_type_convert_reference_float32_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_reference_double64_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 127.0);
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_reference_double64_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 255.0);
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_reference_double64_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 32767.0);
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_reference_double64_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 65535.0);
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_reference_double64_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 2147483647.0);
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_reference_double64_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 4294967295.0);
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_reference_double64_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_reference_double64_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_reference_double64_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = 0; i < ref_length; i++)
	{
		read_type = &read[hxa_ref(reference, i) * read_stride];
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(double) * component_count);
	}
}


void hxa_type_convert_face_uint8_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp > 127)
				tmp = 127;
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_face_uint8_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(unsigned char) * component_count);
	}
}


void hxa_type_convert_face_uint8_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_face_uint8_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_face_uint8_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_face_uint8_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_face_uint8_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_face_uint8_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_face_uint8_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	unsigned char *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_face_int32_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < -127)
				tmp = -127;
			if(tmp > 127)
				tmp = 127;
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_face_int32_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > 255)
				tmp = 255;
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_face_int32_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < -32767)
				tmp = -32767;
			if(tmp > 32767)
				tmp = 32767;
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_face_int32_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > 65535)
				tmp = 65535;
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_face_int32_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(int) * component_count);
	}
}


void hxa_type_convert_face_int32_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)read_type[j];
			if(tmp < 0)
				tmp = 0;
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_face_int32_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_face_int32_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_face_int32_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	int *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_face_float32_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 127.0);
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_face_float32_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 255.0);
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_face_float32_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 32767.0);
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_face_float32_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 65535.0);
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_face_float32_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 2147483647.0);
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_face_float32_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 4294967295.0);
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_face_float32_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_face_float32_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(float) * component_count);
	}
}


void hxa_type_convert_face_float32_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	float *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (double)read_type[j];
	}
}


void hxa_type_convert_face_double64_to_int8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 127.0);
			write_type[j] = (char)tmp;
		}
	}
}


void hxa_type_convert_face_double64_to_uint8(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned char *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 255.0);
			write_type[j] = (unsigned char)tmp;
		}
	}
}


void hxa_type_convert_face_double64_to_int16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 32767.0);
			write_type[j] = (short)tmp;
		}
	}
}


void hxa_type_convert_face_double64_to_uint16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned short *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 65535.0);
			write_type[j] = (unsigned short)tmp;
		}
	}
}


void hxa_type_convert_face_double64_to_int32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 2147483647.0);
			write_type[j] = (int)tmp;
		}
	}
}


void hxa_type_convert_face_double64_to_uint32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned int *write_type;
	signed long long tmp;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
		{
			tmp = (signed long long)(read_type[j] * 4294967295.0);
			if(tmp > -1)
				tmp = -1;
			write_type[j] = (unsigned int)tmp;
		}
	}
}


void hxa_type_convert_face_double64_to_half16(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	unsigned short *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = hxa_float32_to_float16((float)read_type[j]);
	}
}


void hxa_type_convert_face_double64_to_float32(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	float *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		for(j = 0; j < component_count; j++)
			write_type[j] = (float)read_type[j];
	}
}


void hxa_type_convert_face_double64_to_double64(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)
{
	double *read_type;
	double *write_type;
	unsigned int i, j, k;
	for(i = k = 0; i < ref_length; i++)
	{
		read_type = &read[k * read_stride];
		if(reference[i] < 0)
			k++;
		write_type = &write[i * write_stride];
		memcpy(write_type, read_type, sizeof(double) * component_count);
	}
}


unsigned char *hxa_type_vertex_convert(HXANode *node, unsigned char *buffer, unsigned int param_count, char **param_names, HxAUtilArrayExportTypes *param_types, unsigned int *param_dimentions)
{
	unsigned int i, j, k, stride, offset;
	for(i = stride = 0; i < param_count; i++)
		stride += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];
	if(buffer == NULL)
		buffer = malloc(stride * node->content.geometry.vertex_count);
	for(i = offset = 0; i < param_count; i++)
	{
		for(j = 0; j < node->content.geometry.vertex_stack.layer_count; j++)
		{
			for(k = 0; 0 != node->content.geometry.vertex_stack.layers[j].name[k] && node->content.geometry.vertex_stack.layers[j].name[k] == param_names[i][k]; k++);
			if(node->content.geometry.vertex_stack.layers[j].name[k] == param_names[i][k])
			{
				switch(node->content.geometry.vertex_stack.layers[j].type)
				{
					case 0 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_uint8_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 1 :
								hxa_type_convert_vertex_uint8_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 2 :
								hxa_type_convert_vertex_uint8_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 3 :
								hxa_type_convert_vertex_uint8_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 4 :
								hxa_type_convert_vertex_uint8_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 5 :
								hxa_type_convert_vertex_uint8_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 6 :
								hxa_type_convert_vertex_uint8_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 7 :
								hxa_type_convert_vertex_uint8_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 8 :
								hxa_type_convert_vertex_uint8_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
						}
					break;
					case 1 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_int32_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 1 :
								hxa_type_convert_vertex_int32_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 2 :
								hxa_type_convert_vertex_int32_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 3 :
								hxa_type_convert_vertex_int32_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 4 :
								hxa_type_convert_vertex_int32_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 5 :
								hxa_type_convert_vertex_int32_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 6 :
								hxa_type_convert_vertex_int32_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 7 :
								hxa_type_convert_vertex_int32_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 8 :
								hxa_type_convert_vertex_int32_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
						}
					break;
					case 2 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_float32_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 1 :
								hxa_type_convert_vertex_float32_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 2 :
								hxa_type_convert_vertex_float32_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 3 :
								hxa_type_convert_vertex_float32_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 4 :
								hxa_type_convert_vertex_float32_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 5 :
								hxa_type_convert_vertex_float32_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 6 :
								hxa_type_convert_vertex_float32_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 7 :
								hxa_type_convert_vertex_float32_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 8 :
								hxa_type_convert_vertex_float32_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
						}
					break;
					case 3 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_double64_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 1 :
								hxa_type_convert_vertex_double64_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 2 :
								hxa_type_convert_vertex_double64_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 3 :
								hxa_type_convert_vertex_double64_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 4 :
								hxa_type_convert_vertex_double64_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 5 :
								hxa_type_convert_vertex_double64_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 6 :
								hxa_type_convert_vertex_double64_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 7 :
								hxa_type_convert_vertex_double64_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
							case 8 :
								hxa_type_convert_vertex_double64_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.vertex_count);
							break;
						}
					break;
				}
			}
		}
		offset += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];
	}
	return buffer;
}
unsigned char *hxa_type_reference_convert(HXANode *node, unsigned char *buffer, unsigned int param_count, char **param_names, HxAUtilArrayExportTypes *param_types, unsigned int *param_dimentions)
{
	unsigned int i, j, k, stride, offset;
	for(i = stride = 0; i < param_count; i++)
		stride += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];
	if(buffer == NULL)
		buffer = malloc(stride * node->content.geometry.edge_corner_count);
	for(i = offset = 0; i < param_count; i++)
	{
		for(j = 0; j < node->content.geometry.vertex_stack.layer_count; j++)
		{
			for(k = 0; 0 != node->content.geometry.vertex_stack.layers[j].name[k] && node->content.geometry.vertex_stack.layers[j].name[k] == param_names[i][k]; k++);
			if(node->content.geometry.vertex_stack.layers[j].name[k] == param_names[i][k])
			{
				switch(node->content.geometry.vertex_stack.layers[j].type)
				{
					case 0 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_reference_uint8_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_reference_uint8_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_reference_uint8_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_reference_uint8_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_reference_uint8_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_reference_uint8_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_reference_uint8_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_reference_uint8_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_reference_uint8_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 1 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_reference_int32_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_reference_int32_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_reference_int32_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_reference_int32_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_reference_int32_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_reference_int32_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_reference_int32_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_reference_int32_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_reference_int32_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 2 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_reference_float32_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_reference_float32_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_reference_float32_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_reference_float32_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_reference_float32_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_reference_float32_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_reference_float32_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_reference_float32_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_reference_float32_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 3 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_reference_double64_to_int8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_reference_double64_to_uint8(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_reference_double64_to_int16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_reference_double64_to_uint16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_reference_double64_to_int32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_reference_double64_to_uint32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_reference_double64_to_half16(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_reference_double64_to_float32(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_reference_double64_to_double64(&buffer[offset],
									node->content.geometry.vertex_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.vertex_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
				}
			}
		}
		offset += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];
	}
	for(i = offset = 0; i < param_count; i++)
	{
		for(j = 0; j < node->content.geometry.corner_stack.layer_count; j++)
		{
			for(k = 0; 0 != node->content.geometry.corner_stack.layers[j].name[k] && node->content.geometry.corner_stack.layers[j].name[k] == param_names[i][k]; k++);
			if(node->content.geometry.corner_stack.layers[j].name[k] == param_names[i][k])
			{
				switch(node->content.geometry.corner_stack.layers[j].type)
				{
					case 0 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_uint8_to_int8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_vertex_uint8_to_uint8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_vertex_uint8_to_int16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_vertex_uint8_to_uint16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_vertex_uint8_to_int32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_vertex_uint8_to_uint32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_vertex_uint8_to_half16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_vertex_uint8_to_float32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_vertex_uint8_to_double64(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 1 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_int32_to_int8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_vertex_int32_to_uint8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_vertex_int32_to_int16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_vertex_int32_to_uint16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_vertex_int32_to_int32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_vertex_int32_to_uint32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_vertex_int32_to_half16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_vertex_int32_to_float32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_vertex_int32_to_double64(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 2 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_float32_to_int8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_vertex_float32_to_uint8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_vertex_float32_to_int16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_vertex_float32_to_uint16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_vertex_float32_to_int32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_vertex_float32_to_uint32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_vertex_float32_to_half16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_vertex_float32_to_float32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_vertex_float32_to_double64(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 3 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_vertex_double64_to_int8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_vertex_double64_to_uint8(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_vertex_double64_to_int16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_vertex_double64_to_uint16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_vertex_double64_to_int32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_vertex_double64_to_uint32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_vertex_double64_to_half16(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_vertex_double64_to_float32(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_vertex_double64_to_double64(&buffer[offset],
									node->content.geometry.corner_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.corner_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
				}
			}
		}
		offset += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];
	}
	for(i = offset = 0; i < param_count; i++)
	{
		for(j = 0; j < node->content.geometry.face_stack.layer_count; j++)
		{
			for(k = 0; 0 != node->content.geometry.face_stack.layers[j].name[k] && node->content.geometry.face_stack.layers[j].name[k] == param_names[i][k]; k++);
			if(node->content.geometry.face_stack.layers[j].name[k] == param_names[i][k])
			{
				switch(node->content.geometry.face_stack.layers[j].type)
				{
					case 0 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_face_uint8_to_int8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_face_uint8_to_uint8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_face_uint8_to_int16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_face_uint8_to_uint16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_face_uint8_to_int32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_face_uint8_to_uint32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_face_uint8_to_half16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_face_uint8_to_float32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_face_uint8_to_double64(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.uint8_data,
									stride,
									sizeof(unsigned char) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 1 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_face_int32_to_int8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_face_int32_to_uint8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_face_int32_to_int16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_face_int32_to_uint16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_face_int32_to_int32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_face_int32_to_uint32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_face_int32_to_half16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_face_int32_to_float32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_face_int32_to_double64(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.int32_data,
									stride,
									sizeof(int) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 2 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_face_float32_to_int8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_face_float32_to_uint8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_face_float32_to_int16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_face_float32_to_uint16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_face_float32_to_int32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_face_float32_to_uint32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_face_float32_to_half16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_face_float32_to_float32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_face_float32_to_double64(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.float_data,
									stride,
									sizeof(float) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
					case 3 :
						switch(param_types[i])
						{
							case 0 :
								hxa_type_convert_face_double64_to_int8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 1 :
								hxa_type_convert_face_double64_to_uint8(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 2 :
								hxa_type_convert_face_double64_to_int16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 3 :
								hxa_type_convert_face_double64_to_uint16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 4 :
								hxa_type_convert_face_double64_to_int32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 5 :
								hxa_type_convert_face_double64_to_uint32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 6 :
								hxa_type_convert_face_double64_to_half16(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 7 :
								hxa_type_convert_face_double64_to_float32(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
							case 8 :
								hxa_type_convert_face_double64_to_double64(&buffer[offset],
									node->content.geometry.face_stack.layers[j].data.double_data,
									stride,
									sizeof(double) * node->content.geometry.face_stack.layers[j].components,
									param_dimentions[i],
									node->content.geometry.corner_stack.layers->data.int32_data,
									node->content.geometry.edge_corner_count);
							break;
						}
					break;
				}
			}
		}
		offset += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];
	}
	return buffer;
}
