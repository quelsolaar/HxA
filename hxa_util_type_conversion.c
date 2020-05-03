#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "hxa.h"
#include "hxa_utils.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma warning(disable:4703)
#pragma warning(disable:4996)
#pragma warning(disable:4664)

#define FALSE 0
#define TRUE !FALSE


short hxa_float32_to_float16(float value)
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

char *export_type_strings[] = {"char", "unsigned char", "short", "unsigned short", "int", "unsigned int", "unsigned short", "float", "double"};
char *export_type_names[] = {"int8", "uint8", "int16", "uint16", "int32", "uint32", "half16", "float32", "double64"}; 
char *export_type_enum_names[] = {"uint8_data", "int32_data", "float_data", "double_data"};
long long export_type_scale[] = {127, 255, (256 * 128 - 1), (256 * 256 - 1), (256 * 256  * 256  * 128 - 1), (256 * 256  * 256  * 256 - 1)};
long long export_type_min[] = {-127, 0, -(256 * 128 - 1), 0, -(256 * 256  * 256  * 128 - 1), 0, -(256 * 256  * 256  * 256), -(256 * 256  * 256  * 256), -(256 * 256  * 256  * 256)};
long long export_type_max[] = {127, 255, (256 * 128 - 1), (256 * 256 - 1), (256 * 256  * 256  * 128 - 1), (256 * 256  * 256  * 256 - 1), (256 * 256  * 256  * 256), (256 * 256  * 256  * 256), (256 * 256  * 256  * 256)};

void hxa_type_convert_gen()
{
	unsigned int i, j, k, l, layer_data_types[HXA_LDT_COUNT] = {HXA_UAET_UINT8, HXA_UAET_INT32, HXA_UAET_FLOAT32, HXA_UAET_DOUBLE64};
	char *stack_names[] = {"vertex_stack", "corner_stack", "face_stack"};
	FILE *f;
	f = fopen("hxa_convesion_generated.c", "w");
	fprintf(f, "#include \"hxa.h\"\n");
	fprintf(f, "#include \"hxa_utils.h\"\n\n");
	fprintf(f, "#define NULL (void *)0\n\n");
	fprintf(f, "unsigned int hxa_util_array_export_type_sizes[HXA_UAET_COUNT] = {sizeof(char), sizeof(unsigned char), sizeof(short), sizeof(unsigned short), sizeof(int), sizeof(unsigned int), sizeof(short), sizeof(float), sizeof(double)};\n");
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < HXA_LDT_COUNT; j++)
		{
			for(k = 0; k < HXA_UAET_COUNT; k++)
			{
				switch(i)
				{
					case 0 :
					fprintf(f, "void hxa_type_convert_vertex_%s_to_%s(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, unsigned int vertex_length)\n", export_type_names[layer_data_types[j]], export_type_names[k]);
					break;
					case 1 :
					fprintf(f, "void hxa_type_convert_reference_%s_to_%s(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)\n", export_type_names[layer_data_types[j]], export_type_names[k]);
					break;
					case 2 :
					fprintf(f, "void hxa_type_convert_face_%s_to_%s(unsigned char *write, unsigned char *read, unsigned int write_stride, unsigned int read_stride, unsigned int component_count, int *reference, unsigned int ref_length)\n", export_type_names[layer_data_types[j]], export_type_names[k]);
					break;
				}
				fprintf(f, "{\n");
				fprintf(f, "\t%s *read_type;\n", export_type_strings[layer_data_types[j]]);
				fprintf(f, "\t%s *write_type;\n", export_type_strings[k]);
				if(k < HXA_UAET_HALF16)
					fprintf(f, "\tsigned long long tmp;\n");
				fprintf(f, "\tunsigned int i, j, k;\n");
				switch(i)
				{
					case 0 :
					fprintf(f, "\tfor(i = 0; i < vertex_length; i++)\n\t{\n");
					fprintf(f, "\t\tread_type = &read[i * read_stride];\n");
					break;
					case 1 :
					fprintf(f, "\tfor(i = 0; i < ref_length; i++)\n\t{\n");
					fprintf(f, "\t\tread_type = &read[hxa_ref(reference, i) * read_stride];\n");
					break;
					case 2 :
					fprintf(f, "\tfor(i = k = 0; i < ref_length; i++)\n\t{\n");
					fprintf(f, "\t\tread_type = &read[k * read_stride];\n");
					fprintf(f, "\t\tif(reference[i] < 0)\n");
					fprintf(f, "\t\t\tk++;\n");
					break;
				}
				fprintf(f, "\t\twrite_type = &write[i * write_stride];\n");
				if(layer_data_types[j] == k)
				{
					fprintf(f, "\t\tmemcpy(write_type, read_type, sizeof(%s) * component_count);\n", export_type_strings[layer_data_types[j]]);
				}else
				{
					fprintf(f, "\t\tfor(j = 0; j < component_count; j++)\n");
					if(k == HXA_UAET_HALF16)
						fprintf(f, "\t\t\twrite_type[j] = hxa_float32_to_float16((float)read_type[j]);\n");
					else if(k > HXA_UAET_HALF16)
						fprintf(f, "\t\t\twrite_type[j] = (%s)read_type[j];\n", export_type_strings[k]);
					else
					{
						fprintf(f, "\t\t{\n");
						if(layer_data_types[j] > HXA_UAET_HALF16)
							fprintf(f, "\t\t\ttmp = (signed long long)(read_type[j] * %u.0);\n", export_type_scale[k]);
						else
							fprintf(f, "\t\t\ttmp = (signed long long)read_type[j];\n");
			
						if(export_type_min[k] > export_type_min[layer_data_types[j]])
						{
							fprintf(f, "\t\t\tif(tmp < %lli)\n", export_type_min[k]);
							fprintf(f, "\t\t\t\ttmp = %lli;\n", export_type_min[k]);
						}
						if(export_type_max[k] < export_type_max[layer_data_types[j]])
						{
							fprintf(f, "\t\t\tif(tmp > %lli)\n", export_type_max[k]);
							fprintf(f, "\t\t\t\ttmp = %lli;\n", export_type_max[k]);
						}
						fprintf(f, "\t\t\twrite_type[j] = (%s)tmp;\n", export_type_strings[k]);
						fprintf(f, "\t\t}\n");
					}
				}
				fprintf(f, "\t}\n");
				fprintf(f, "}\n\n\n");	
			}
		}
	}
	for(i = 0; i < 2; i++)
	{
		if(i == 0)
			fprintf(f, "unsigned char *hxa_type_vertex_convert(HXANode *node, unsigned char *buffer, unsigned int param_count, char **param_names, HxAUtilArrayExportTypes *param_types, unsigned int *param_dimentions)\n");
		else
			fprintf(f, "unsigned char *hxa_type_reference_convert(HXANode *node, unsigned char *buffer, unsigned int param_count, char **param_names, HxAUtilArrayExportTypes *param_types, unsigned int *param_dimentions)\n");
		fprintf(f, "{\n");
		fprintf(f, "\tunsigned int i, j, k, stride, offset;\n");
		fprintf(f, "\tfor(i = stride = 0; i < param_count; i++)\n");
		fprintf(f, "\t\tstride += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];\n");
		fprintf(f, "\tif(buffer == NULL)\n");
		if(i == 0)
			fprintf(f, "\t\tbuffer = malloc(stride * node->content.geometry.vertex_count);\n");
		else
			fprintf(f, "\t\tbuffer = malloc(stride * node->content.geometry.edge_corner_count);\n");
		for(l = 0; l < 3 && (i != 0 || l < 1); l++)
		{
			fprintf(f, "\tfor(i = offset = 0; i < param_count; i++)\n");
			fprintf(f, "\t{\n");
			fprintf(f, "\t\tfor(j = 0; j < node->content.geometry.%s.layer_count; j++)\n", stack_names[l]);
			fprintf(f, "\t\t{\n");
			fprintf(f, "\t\t\tfor(k = 0; 0 != node->content.geometry.%s.layers[j].name[k] && node->content.geometry.%s.layers[j].name[k] == param_names[i][k]; k++);\n", stack_names[l], stack_names[l]);
			fprintf(f, "\t\t\tif(node->content.geometry.%s.layers[j].name[k] == param_names[i][k])\n", stack_names[l]);
			fprintf(f, "\t\t\t{\n");
			fprintf(f, "\t\t\t\tswitch(node->content.geometry.%s.layers[j].type)\n", stack_names[l]);
			fprintf(f, "\t\t\t\t{\n");
			for(j = 0; j < HXA_LDT_COUNT; j++)
			{
				fprintf(f, "\t\t\t\t\tcase %u :\n", j);
				fprintf(f, "\t\t\t\t\t\tswitch(param_types[i])\n");
				fprintf(f, "\t\t\t\t\t\t{\n");
				for(k = 0; k < HXA_UAET_COUNT; k++)
				{
					HXANode *node;
			
					fprintf(f, "\t\t\t\t\t\t\tcase %u :\n", k);				
					if(i == 0)
					{
						fprintf(f, "\t\t\t\t\t\t\t\thxa_type_convert_vertex_%s_to_%s(&buffer[offset],\n",  export_type_names[layer_data_types[j]], export_type_names[k]);
						fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.%s.layers[j].data.%s,\n", stack_names[l], export_type_enum_names[j]);
						fprintf(f, "\t\t\t\t\t\t\t\t\tstride,\n");
						fprintf(f, "\t\t\t\t\t\t\t\t\tsizeof(%s) * node->content.geometry.%s.layers[j].components,\n", export_type_strings[layer_data_types[j]], stack_names[l]);
						fprintf(f, "\t\t\t\t\t\t\t\t\tparam_dimentions[i],\n");
						fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.vertex_count);\n");
					}else
					{
						switch(l)
						{ 
							case 0 :
							fprintf(f, "\t\t\t\t\t\t\t\thxa_type_convert_reference_%s_to_%s(&buffer[offset],\n",  export_type_names[layer_data_types[j]], export_type_names[k]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.%s.layers[j].data.%s,\n", stack_names[l], export_type_enum_names[j]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tstride,\n");
							fprintf(f, "\t\t\t\t\t\t\t\t\tsizeof(%s) * node->content.geometry.%s.layers[j].components,\n", export_type_strings[layer_data_types[j]], stack_names[l]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tparam_dimentions[i],\n");
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.corner_stack.layers->data.int32_data,\n", export_type_names[layer_data_types[j]]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.edge_corner_count);\n");
							break;
							case 1 :
							fprintf(f, "\t\t\t\t\t\t\t\thxa_type_convert_vertex_%s_to_%s(&buffer[offset],\n",  export_type_names[layer_data_types[j]], export_type_names[k]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.%s.layers[j].data.%s,\n", stack_names[l], export_type_enum_names[j]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tstride,\n");
							fprintf(f, "\t\t\t\t\t\t\t\t\tsizeof(%s) * node->content.geometry.%s.layers[j].components,\n", export_type_strings[layer_data_types[j]], stack_names[l]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tparam_dimentions[i],\n");
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.edge_corner_count);\n");
							break;
							case 2 :
							fprintf(f, "\t\t\t\t\t\t\t\thxa_type_convert_face_%s_to_%s(&buffer[offset],\n",  export_type_names[layer_data_types[j]], export_type_names[k]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.%s.layers[j].data.%s,\n", stack_names[l], export_type_enum_names[j]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tstride,\n");
							fprintf(f, "\t\t\t\t\t\t\t\t\tsizeof(%s) * node->content.geometry.%s.layers[j].components,\n", export_type_strings[layer_data_types[j]], stack_names[l]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tparam_dimentions[i],\n");
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.corner_stack.layers->data.int32_data,\n", export_type_names[layer_data_types[j]]);
							fprintf(f, "\t\t\t\t\t\t\t\t\tnode->content.geometry.edge_corner_count);\n");
							break;
						}
					}
					fprintf(f, "\t\t\t\t\t\t\tbreak;\n");
				}
				fprintf(f, "\t\t\t\t\t\t}\n");
				fprintf(f, "\t\t\t\t\tbreak;\n");
			}
			fprintf(f, "\t\t\t\t}\n");
			fprintf(f, "\t\t\t}\n");
			fprintf(f, "\t\t}\n");
			fprintf(f, "\t\toffset += param_dimentions[i] * hxa_util_array_export_type_sizes[param_types[i]];\n");
			fprintf(f, "\t}\n");
		}
		fprintf(f, "\treturn buffer;\n");
		fprintf(f, "}\n");
	}
	fclose(f);
	exit(0);
}

