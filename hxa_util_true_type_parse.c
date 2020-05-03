#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "hxa.h"


#define FALSE 0
#define TRUE !FALSE

typedef unsigned int uint32;
typedef signed short int16;
typedef unsigned short uint16;
typedef unsigned char uint8;


typedef struct{
	float *vertex_array;
	unsigned int vertex_count;
	unsigned int ref_count;
	unsigned int *ref;
	unsigned int *material;
}ProcessOutput;

extern void mo_edit_process_deploy(float *vertex_array, unsigned int *loop_sizes, unsigned int loop_count, ProcessOutput *output);

typedef struct{
	char tag[5];
	uint32 checksum;
	uint32 offset;
	uint32 length;
}HxAFontTable;

typedef struct{
	uint32 character_id;
	uint32 ingest_glyph_id;
	uint32 export_glyph_id;
}HxAFontCharacterLookup;


typedef struct{
	float *vertex_array;
	unsigned int vertex_count;
	uint8 *crease;
	unsigned int *shapes;
	int16 shape_count;
	ProcessOutput poygonized;
	float kerning[8];
}HxAFontCharacter;

typedef struct{
	HxAFontCharacterLookup *lookup;
	unsigned int lookup_count;
	HxAFontCharacter *glyphs;
	unsigned int glyph_count;
}HxAFont;


HxAFontCharacter global_character;


uint8 hxa_util_true_type_parse_read_uint8(uint8 *array, unsigned int *pos)
{
	uint8 out;
	out = array[*pos];
	(*pos)++;
	return out;
}

uint16 hxa_util_true_type_parse_read_uint16(uint8 *array, unsigned int *pos)
{
	uint16 out;
	out = ((uint16) array[*pos]) << 8;
	(*pos)++;
	out |= (uint16) array[*pos];
	(*pos)++;
	return out;
}

uint32 hxa_util_true_type_parse_read_uint32(uint8 *array, unsigned int *pos)
{
	uint32 out;
	out = ((uint32) array[*pos]) << 24;
	(*pos)++;
	out |= ((uint32) array[*pos]) << 16;
	(*pos)++;
	out |= ((uint32) array[*pos]) << 8;
	(*pos)++;
	out |= (uint32) array[*pos];
	(*pos)++;
	return out;
}

void f_intersect2f(float *output, float *line_a0, float *line_a1, float *line_b0, float *line_b1)
{
	output[0] = (line_a0[0] * line_a1[1] - line_a0[1] * line_a1[0])	* (line_b0[0] - line_b1[0]) - 
				(line_b0[0] * line_b1[1] - line_b0[1] * line_b1[0])	* (line_a0[0] - line_a1[0]);
	output[0] /= (line_a0[0] - line_a1[0]) * (line_b0[1] - line_b1[1]) - (line_a0[1] - line_a1[1]) * (line_b0[0] - line_b1[0]);
	output[1] = (line_a0[0] * line_a1[1] - line_a0[1] * line_a1[0])	* (line_b0[1] - line_b1[1]) - 
				(line_b0[0] * line_b1[1] - line_b0[1] * line_b1[0])	* (line_a0[1] - line_a1[1]);
	output[1] /= (line_a0[0] - line_a1[0]) * (line_b0[1] - line_b1[1]) - (line_a0[1] - line_a1[1]) * (line_b0[0] - line_b1[0]);
}

#ifdef OLD_SAVER
void hxa_util_true_type_parse_save_pff(HxAFont *font, char *file_name)
{
	uint8 *data;
	FILE *f;
	uint i, j, ref_count = 0, vertex_count = 0, size, pos = 0;
	for(i = 0; i < font->glyph_count; i++)
	{
		ref_count += font->glyphs[i].poygonized.ref_count;
		vertex_count += font->glyphs[i].poygonized.vertex_count;
	}
	size = 32 + 4 + 4 + 4; /* Name, glyph count lookup count, vertex_count, */
	size += font->lookup_count * 4 * 2; /* Character ID */
	size += font->glyph_count * 4; /* ref_size */
	size += vertex_count * 4 * 2; /* vertex data*/
	size += ref_count * 4; /* reference data */
	size += font->glyph_count * 4 * 8; /* Kerning */
	data = malloc(size);
	for(i = 0; i < 32; i++)
		data[i] = 0;
	pos = 32;
	hxa_util_true_type_parse_write_uint32(data, &pos, font->glyph_count);
	hxa_util_true_type_parse_write_uint32(data, &pos, font->lookup_count);
	hxa_util_true_type_parse_write_uint32(data, &pos, vertex_count);
	for(i = 0; i < font->lookup_count; i++)
	{
		hxa_util_true_type_parse_write_uint32(data, &pos, font->lookup[i].character_id);
		hxa_util_true_type_parse_write_uint32(data, &pos, font->lookup[i].export_glyph_id);
	}
	for(i = 0; i < font->glyph_count; i++)
		hxa_util_true_type_parse_write_uint32(data, &pos, font->glyphs[i].poygonized.ref_count);
	for(i = 0; i < font->glyph_count; i++)
		for(j = 0; j < font->glyphs[i].poygonized.vertex_count * 2; j++)
			hxa_util_true_type_parse_write_float(data, &pos, font->glyphs[i].poygonized.vertex_array[j]);

	vertex_count = 0;
	for(i = 0; i < font->glyph_count; i++)
	{
		for(j = 0; j < font->glyphs[i].poygonized.ref_count; j++)
			hxa_util_true_type_parse_write_uint32(data, &pos, font->glyphs[i].poygonized.ref[j] + vertex_count);
		vertex_count += font->glyphs[i].poygonized.vertex_count;
	}
	for(i = 0; i < font->glyph_count; i++)
		for(j = 0; j < 8; j++)
			hxa_util_true_type_parse_write_float(data, &pos, font->glyphs[i].kerning[j]);
	f = fopen(file_name, "wb");
	fwrite(data, 1, size, f);
	fclose(f);
	free(data);
}



void hxa_util_true_type_parse_save_source(HxAFont *font, char *name)
{
	uint8 *data;
	char file_name[64], capital[64];
	FILE *f;
	uint i, j, k, ref_count = 0, vertex_count = 0, size, pos = 0;
	sprintf(file_name, "s_font_%s.c", name);
	f = fopen(file_name, "w");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "#include \"forge.h\"\n\n");
	fprintf(f, "#include \"relinquish.h\"\n\n");
	fprintf(f, "#include \"s_text.h\"\n\n");
	for (i = 0; i < font->glyph_count; i++)
	{
		ref_count += font->glyphs[i].poygonized.ref_count;
		vertex_count += font->glyphs[i].poygonized.vertex_count;
	}
	
	for(i = 0; name[i] != 0; i++)
	{
		capital[i] = name[i];
		if(capital[i] >= 'a' && capital[i] <= 'z')
			capital[i] -= 32;
		if(capital[i] == '-')
			capital[i] = '_';
	}
	capital[i] = 0;
	
	fprintf(f, "#define SEDUCE_%s_GLYPH_COUNT %u\n", capital, font->glyph_count);
	fprintf(f, "#define SEDUCE_%s_CHARACTER_COUNT %u\n", capital, font->lookup_count);
	fprintf(f, "#define SEDUCE_%s_REFERENCE_COUNT %u\n", capital, ref_count);
	fprintf(f, "#define SEDUCE_%s_VERTEX_COUNT %u\n\n", capital, vertex_count);


	fprintf(f, "float seduce_kerning_%s[] = {%ff, %ff, %ff, %ff, %ff, %ff, %ff, %ff", name,
				font->glyphs[0].kerning[0],
				font->glyphs[0].kerning[1],
				font->glyphs[0].kerning[2],
				font->glyphs[0].kerning[3],
				font->glyphs[0].kerning[4],
				font->glyphs[0].kerning[5],
				font->glyphs[0].kerning[6],
				font->glyphs[0].kerning[7]);
	
	for(i = 1; i < font->glyph_count; i++)
		fprintf(f, ",\n\t\t\t\t%ff, %ff, %ff, %ff, %ff, %ff, %ff, %ff", font->glyphs[i].kerning[0],
				font->glyphs[i].kerning[1],
				font->glyphs[i].kerning[2],
				font->glyphs[i].kerning[3],
				font->glyphs[i].kerning[4],
				font->glyphs[i].kerning[5],
				font->glyphs[i].kerning[6],
				font->glyphs[i].kerning[7]);
	fprintf(f, "};\n\n");

	fprintf(f, "uint seduce_lookup_%s[] = {%u, %u", name, font->lookup[0].character_id, font->lookup[0].export_glyph_id);
	
	for(i = 1; i < font->lookup_count; i++)
		fprintf(f, ",\n\t\t\t\t%u, %u", font->lookup[i].character_id, font->lookup[i].export_glyph_id);

	fprintf(f, "};\n\n");

	
	fprintf(f, "uint seduce_ref_count_%s[] = {", name);
	for (i = 0; i < font->glyph_count - 1; i++)
	{
		fprintf(f, "%u, ", font->glyphs[i].poygonized.ref_count);
		if(i % 16 == 15)
			fprintf(f, "\n\t\t\t\t");
	}
	fprintf(f, "%u};\n", font->glyphs[i].poygonized.ref_count);



	vertex_count = 0;
	fprintf(f, "uint seduce_ref_%s[] = {", name);
	for(i = k = 0; i < font->glyph_count; i++)
	{
		for(j = 0; j < font->glyphs[i].poygonized.ref_count; j++)
		{
			fprintf(f, "%u, ", font->glyphs[i].poygonized.ref[j] + vertex_count);
			if(k++ % 16 == 15)
				fprintf(f, "\n\t\t\t\t");
		}
		vertex_count += font->glyphs[i].poygonized.vertex_count;
	}
	fprintf(f, "0};\n");


	fprintf(f, "float seduce_vertex_%s[] = {", name);
	for(i = k = 0; i < font->glyph_count; i++)
	{
		for(j = 0; j < font->glyphs[i].poygonized.vertex_count * 2; j++)
		{
			fprintf(f, "%ff, ", font->glyphs[i].poygonized.vertex_array[j]);
			if(k++ % 16 == 15)
				fprintf(f, "\n\t\t\t\t");
		}
	}
	fprintf(f, "0};\n\n");


	fprintf(f, "void hxa_util_true_type_parse_%s_init(SeduceRenderFont *font)\n", name);
	fprintf(f, "{\n");
	fprintf(f, "\tSeduceFontCharacter *glyphs;\n");
	fprintf(f, "\tRFormats vertex_format_types = R_FLOAT;\n");
	fprintf(f, "\tuint i, j,  pos = 0;\n");
	fprintf(f, "\tfloat *vertex_section;\n");
	fprintf(f, "\tchar *name = \"%s\";\n\n", name);

	fprintf(f, "\tfor(i = 0; name[i] != 0 && i < 31; i++)\n");
	fprintf(f, "\t\tfont->name[i] = name[i];\n");
	fprintf(f, "\tfont->name[i] = 0;\n");
	fprintf(f, "\ti = 2;\n");
	fprintf(f, "\tfont->pool = r_array_allocate(SEDUCE_%s_VERTEX_COUNT, &vertex_format_types, &i, 1, SEDUCE_%s_REFERENCE_COUNT);\n", capital, capital);
	fprintf(f, "\tvertex_section = r_array_section_allocate_vertex(font->pool, SEDUCE_%s_VERTEX_COUNT); \n", capital);
	fprintf(f, "\tr_array_load_vertex(font->pool, vertex_section, seduce_vertex_%s, 0, SEDUCE_%s_VERTEX_COUNT);\n", name, capital);
	fprintf(f, "\tglyphs = malloc((sizeof *glyphs) * SEDUCE_%s_GLYPH_COUNT);\n", capital);
	fprintf(f, "\tfor(i = j = 0; i < SEDUCE_%s_GLYPH_COUNT; i++)\n", capital);
	fprintf(f, "\t{\n");
 	fprintf(f, "\t\tglyphs[i].sections = r_array_section_allocate_reference(font->pool, seduce_ref_count_%s[i]);\n", name);
	fprintf(f, "\t\tr_array_load_reference(font->pool, glyphs[i].sections, vertex_section, &seduce_ref_%s[j], seduce_ref_count_%s[i]);\n", name, name);
	fprintf(f, "\t\tj += seduce_ref_count_%s[i];\n", name);
	fprintf(f, "\t}\n");
	fprintf(f, "\tfor(i = j = 0; i < SEDUCE_%s_GLYPH_COUNT; i++)\n", capital);
	fprintf(f, "\t{\n");
	fprintf(f, "\t\tglyphs[i].size = 0;\n");
	fprintf(f, "\t\tfor(j = 0; j < 8; j++)\n");
	fprintf(f, "\t\t{\n");
	fprintf(f, "\t\t\tglyphs[i].kerning[j] = seduce_kerning_%s[i * 8 + j];\n", name);
	fprintf(f, "\t\t\tif(glyphs[i].kerning[j] > glyphs[i].size)\n");
	fprintf(f, "\t\t\t\tglyphs[i].size = glyphs[i].kerning[j];\n");
	fprintf(f, "\t\t}\n");
	fprintf(f, "\t}\n");
	fprintf(f, "\tfont->glyphs = malloc((sizeof *font->glyphs) * SEDUCE_%s_CHARACTER_COUNT);\n", capital);
	fprintf(f, "\tfor(i = 0; i < SEDUCE_%s_CHARACTER_COUNT; i++)\n", capital);
	fprintf(f, "\t\tfont->glyphs[i] = glyphs[seduce_lookup_%s[i * 2 + 1]];\n", name);
	fprintf(f, "\tfree(glyphs);\n");
	fprintf(f, "\thxa_util_true_type_parse_load_construct_graph(font, seduce_lookup_%s, SEDUCE_%s_CHARACTER_COUNT);\n", name, capital);
	fprintf(f, "}\n");
}
#endif

int hxa_util_true_type_parse_glyph(HxAFont *font, uint8 *data, unsigned int *pos)
{
	HxAFontCharacter character, *c;
	unsigned int /*i, */j, k, count;
	int min_x, min_y, max_x, max_y;
	unsigned int contour_count, point_count, instruction_count, vertex_count;
	float origo, f;
	uint8 flags;
	if(font->glyph_count % 16 == 0)
		font->glyphs = realloc(font->glyphs, (sizeof *font->glyphs) * (font->glyph_count + 16));
	c = &font->glyphs[font->glyph_count];
//	c->shape_count = testify_unpack_int16(h, "type"); 
	c->shape_count = hxa_util_true_type_parse_read_uint16(data, pos);
	if(c->shape_count < 0)
	{
//		printf("Skipping out compund shape %i", (unsigned int)c->shape_count);
		return FALSE;
	}
	if(c->shape_count == 0)
	{
		c->vertex_count = 0;
		return FALSE;
	}
	font->glyph_count++;
	c->shapes = malloc((sizeof *c->shapes) * c->shape_count);

	
	min_x = hxa_util_true_type_parse_read_uint16(data, pos);
	min_y = hxa_util_true_type_parse_read_uint16(data, pos);
	max_x = hxa_util_true_type_parse_read_uint16(data, pos);
	max_y = hxa_util_true_type_parse_read_uint16(data, pos);

/*	min_x = testify_unpack_int16(h, "type");
	min_y = testify_unpack_int16(h, "type");
	max_x = testify_unpack_int16(h, "type");
	max_y = testify_unpack_int16(h, "type");*/

//	printf("contour_count %u\n", c->shape_count);



	vertex_count = 0;
	for(j = 0; j < c->shape_count; j++)
	{
	//	c->shapes[j] = testify_unpack_int16(h, "type") + 1;
		c->shapes[j] = hxa_util_true_type_parse_read_uint16(data, pos) + 1;
//		printf("c->shapes[j].vertex_count %u\n", c->shapes[j]);
	}
	vertex_count = c->shapes[j - 1];
	c->vertex_count = vertex_count;
	c->vertex_array = malloc((sizeof *c->vertex_array) * c->vertex_count * 2);
	c->crease = malloc((sizeof *c->crease) * c->vertex_count);
	
//	instruction_count = testify_unpack_uint16(h, "type");
	instruction_count = hxa_util_true_type_parse_read_uint16(data, pos);
//	printf("instruction_count %u\n", instruction_count);

	for(j = 0; j < instruction_count; j++)
		hxa_util_true_type_parse_read_uint8(data, pos);
//	pos[0] += instruction_count;
//	for(j = 0; j < instruction_count; j++)
//		testify_unpack_int8(h, "type");

//	printf("vertex_count %u\n", vertex_count);
	for(j = 0; j < vertex_count; j++)
	{
	//	c->crease[j] = testify_unpack_uint8(h, "type");
		c->crease[j] = hxa_util_true_type_parse_read_uint8(data, pos);
//		printf("crease %u\n", c->crease[j]);
//		printf("point is %u %u in size\n", c->crease[j] & 2, c->crease[j] & 4);
		if(c->crease[j] & 8)
		{
	//		count = testify_unpack_uint8(h, "type");
			count = hxa_util_true_type_parse_read_uint8(data, pos);
	//		printf("count %u\n", count);
			for(k = 1; k <= count && k + j < c->vertex_count; k++)
				c->crease[j + k] = c->crease[j];
			j += count;
		}
	}

	origo = 0;
	for(j = 0; j < vertex_count; j++)
	{
	/*	printf("bits %u - ", c->crease[j]);
		for(k = 1; k < 256; k *= 2)
		{
			if(c->crease[j] & k)
				printf("1 ");
			else
				printf("0 ");
		}
		printf("\n");*/
		if(c->crease[j] & 2)
		{
		//	f = (float)testify_unpack_uint8(h, "type");
			f = (float)hxa_util_true_type_parse_read_uint8(data, pos);
			if(c->crease[j] & 16)
				c->vertex_array[j * 2] = origo + f;
			else
				c->vertex_array[j * 2] = origo - f;
		//	printf("x_short %f (%f)\n", c->vertex_array[j * 2], f);
		}else
		{	
			f = 0;
			if(c->crease[j] & 16)
				c->vertex_array[j * 2] = origo;
			else
			{
			//	f = testify_unpack_int16(h, "type");
				f = (float)((int16)hxa_util_true_type_parse_read_uint16(data, pos));
				c->vertex_array[j * 2] = origo + f;
			}
		//	printf("x_long %f (%f)\n", c->vertex_array[j * 2], f);
		}
		origo = c->vertex_array[j * 2];
	}

	origo = 0;
	for(j = 0; j < vertex_count; j++)
	{
	/*	printf("bits %u - ", c->crease[j]);
		for(k = 1; k < 256; k *= 2)
		{
			if(c->crease[j] & k)
				printf("1 ");
			else
				printf("0 ");
		}
		printf("\n");*/
		if(c->crease[j] & 4)
		{
		//	f = (float)testify_unpack_uint8(h, "type");
			f = (float)hxa_util_true_type_parse_read_uint8(data, pos);
			if(c->crease[j] & 32)
				c->vertex_array[j * 2 + 1] = origo + f;
			else
				c->vertex_array[j * 2 + 1] = origo - f;
	//		printf("y_short %f (%f)\n", c->vertex_array[j * 2 + 1], f);
		}else
		{
			f = 0;
			if(c->crease[j] & 32)
				c->vertex_array[j * 2 + 1] = origo;
			else
			{
			//	f = testify_unpack_int16(h, "type");
				f = (float)((int16)hxa_util_true_type_parse_read_uint16(data, pos));
				c->vertex_array[j * 2 + 1] = origo + f;
			}
	//		printf("y_long %f (%f)\n", c->vertex_array[j * 2 + 1], f);
		}
		origo = c->vertex_array[j * 2 + 1];
	}

/*	for(j = 0; j < vertex_count; j++)
		printf("%f %f, ", c->vertex_array[j * 2], c->vertex_array[j * 2 + 1]);
*/
//	testify_file_position_set(h, (j / 4) * 4);
	return TRUE;
}

typedef struct{
	uint16 platform_id;
	uint16 platform_specific_id;
	uint32 offset;
}SeduceFontSubtable;

HxAFontCharacterLookup *hxa_util_true_type_parse_character_lookup_add(HxAFontCharacterLookup *lookup, unsigned int *lookup_count, unsigned int code_id, unsigned int glyph_id, unsigned int glyph_count)
{
	unsigned int i;
	if(glyph_id >= glyph_count)
		return lookup;
	for(i = 0; i < *lookup_count && lookup[i].character_id != code_id; i++);
	if(i < *lookup_count)
		return lookup;
	if(*lookup_count % 256 == 0)
		lookup = realloc(lookup, (sizeof *lookup) * (*lookup_count + 256));
	lookup[*lookup_count].character_id = code_id;
	lookup[*lookup_count].ingest_glyph_id = glyph_id;
	(*lookup_count)++;
	return lookup;
}


HxAFontCharacterLookup *hxa_util_true_type_parse_character_encoding_table(HxAFont *font, HxAFontCharacterLookup *lookup, unsigned int *lookup_count, unsigned int glyph_count, uint8 *data, unsigned int *pos)
{
	unsigned int i, j, k, character;
	uint16 format, length, language, mapping[256];
	format = hxa_util_true_type_parse_read_uint16(data, pos);
	switch(format)
	{
		case 0 :
			length = hxa_util_true_type_parse_read_uint16(data, pos);
			language = hxa_util_true_type_parse_read_uint16(data, pos);
			for(j = 0; j < 256; j++)
			{
				character = hxa_util_true_type_parse_read_uint8(data, pos);
				lookup = hxa_util_true_type_parse_character_lookup_add(lookup, lookup_count, j, character, glyph_count);
			}
		break;
		case 2 :
		{
			struct {
				uint16  first_code;
				uint16  entry_count;
				int16   id_delta;
				uint16  id_range_offset;
			}sub_header[256];
			uint16 subHeaderKeys[256];
			length = hxa_util_true_type_parse_read_uint16(data, pos);
			language = hxa_util_true_type_parse_read_uint16(data, pos);
			for(j = 0; j < 256; j++)
				subHeaderKeys[j] = hxa_util_true_type_parse_read_uint16(data, pos);
			for(j = 0; j < 256; j++)
			{
				sub_header[j].first_code = hxa_util_true_type_parse_read_uint16(data, pos);
				sub_header[j].entry_count = hxa_util_true_type_parse_read_uint16(data, pos);
				sub_header[j].id_delta = hxa_util_true_type_parse_read_uint16(data, pos);
				sub_header[j].id_range_offset = hxa_util_true_type_parse_read_uint16(data, pos);
			}
					
		}
		break;
		case 4 :
		{
			uint16 	segCountX2;
			uint16 	searchRange;
			uint16 	entrySelector;
			uint16 	rangeShift;
			uint16 	*end_code;
			uint16 	reservedPad;
			uint16 	*start_code;
			uint16 	*id_delta;
			uint16 	*id_range_offset;
			length = hxa_util_true_type_parse_read_uint16(data, pos);
			language = hxa_util_true_type_parse_read_uint16(data, pos);
			segCountX2 = hxa_util_true_type_parse_read_uint16(data, pos);
			searchRange = hxa_util_true_type_parse_read_uint16(data, pos);
			entrySelector = hxa_util_true_type_parse_read_uint16(data, pos);
			rangeShift = hxa_util_true_type_parse_read_uint16(data, pos);
			end_code = malloc((sizeof *end_code) * segCountX2 / 2);
			for(j = 0; j < segCountX2 / 2; j++)
				end_code[j] = hxa_util_true_type_parse_read_uint16(data, pos);
			reservedPad = hxa_util_true_type_parse_read_uint16(data, pos);
			start_code = malloc((sizeof *start_code) * segCountX2 / 2);
			for(j = 0; j < segCountX2 / 2; j++)
				start_code[j] = hxa_util_true_type_parse_read_uint16(data, pos);
			id_delta = malloc((sizeof *id_delta) * segCountX2 / 2);
			for(j = 0; j < segCountX2 / 2; j++)
				id_delta[j] = hxa_util_true_type_parse_read_uint16(data, pos);
			id_range_offset = &data[*pos];

			for(j = 0; j < segCountX2 / 2; j++)
			{
				for(k = start_code[j]; k <= end_code[j]; k++)
				{
				//	if((k + (uint)id_delta[j]) % 65536 < font->character_count)
					{
						if(id_range_offset[j] == 0)
						{
						//	printf("code = %u %u %u %u\n", k, (unsigned int)id_delta[j], (unsigned int)id_range_offset[j], j);
							lookup = hxa_util_true_type_parse_character_lookup_add(lookup, lookup_count, k, (k + (unsigned int)id_delta[j]) % 65536, glyph_count);
						}else
						{
							unsigned int offset, temp = 0, read, glyph;
							uint16 *glyph_index;
							temp = 2 * j; 
							read = hxa_util_true_type_parse_read_uint16(id_range_offset, &temp);
							glyph_index =  &id_range_offset[j];
							temp = read + (k - start_code[j]) * 2;
							glyph = hxa_util_true_type_parse_read_uint16(glyph_index, &temp);
						//	printf("--------------------------------- glyph = %u\n", glyph);
							lookup = hxa_util_true_type_parse_character_lookup_add(lookup, lookup_count, k, glyph, glyph_count);

						}
					}
				}
			}
			free(end_code);
			free(start_code);
			free(id_delta);
		}
		break;
		case 6 :
		{
			uint16 first_code, code_count;
			length = hxa_util_true_type_parse_read_uint16(data, pos);
			language = hxa_util_true_type_parse_read_uint16(data, pos);
			first_code = hxa_util_true_type_parse_read_uint16(data, pos);
			code_count = hxa_util_true_type_parse_read_uint16(data, pos);
			for(j = 0; j < code_count; j++)
			{
				character = hxa_util_true_type_parse_read_uint8(data, pos);
				lookup = hxa_util_true_type_parse_character_lookup_add(lookup, lookup_count, first_code + j, character, glyph_count);
			}
		}
		break;
	}
	return lookup;
}



HxAFontCharacterLookup *hxa_util_true_type_parse_character_encoding(HxAFont *font, uint8 *data, unsigned int *pos, unsigned int *lookup_count, unsigned int glyph_count)
{
	SeduceFontSubtable *tables;
	unsigned int i, j, k, version, sub_tables, start;
	HxAFontCharacterLookup *lookup = NULL;
	uint32 character;
	*lookup_count = 0;
	start = *pos;	
	version = hxa_util_true_type_parse_read_uint16(data, pos);
	sub_tables = hxa_util_true_type_parse_read_uint16(data, pos);
	tables = malloc((sizeof *tables) * sub_tables);
	for(i = 0; i < sub_tables; i++)
	{
		tables[i].platform_id = hxa_util_true_type_parse_read_uint16(data, pos);
		tables[i].platform_specific_id = hxa_util_true_type_parse_read_uint16(data, pos);
		tables[i].offset = hxa_util_true_type_parse_read_uint32(data, pos);
	}
	for(i = 0; i < sub_tables && tables[i].platform_id != 0; i++);
	if(i < sub_tables)
	{
		*pos = start + tables[i].offset;
		lookup = hxa_util_true_type_parse_character_encoding_table(font, lookup, lookup_count, glyph_count, data, pos);
	}

	for(i = 0; i < sub_tables; i++)
	{
		if(tables[i].platform_id != 0)
		{
			*pos = start + tables[i].offset;
			lookup = hxa_util_true_type_parse_character_encoding_table(font, lookup, lookup_count, glyph_count, data, pos);
		}
	}
	return lookup;
}

void seduce_spline(float *out, float *a, float *b, float *c, float f)
{
	float temp_a[2], temp_b[2];
	temp_a[0] =	a[0] + (b[0] - a[0]) * f;
	temp_a[1] = a[1] + (b[1] - a[1]) * f;
	temp_b[0] =	b[0] + (c[0] - b[0]) * f;
	temp_b[1] = b[1] + (c[1] - b[1]) * f;
	out[0] = temp_a[0] + (temp_b[0] - temp_a[0]) * f;
	out[1] = temp_a[1] + (temp_b[1] - temp_a[1]) * f;
}

/*


*/


float *hxa_util_true_type_parse_smooth(float *vertex_array, uint8 *flags, unsigned int *loop_sizes, unsigned int loop_count, unsigned int tesseleation)
{
	float *output, a[2], b[2];
	unsigned int i, j, jj, jjj, k, used = 0, last = 0;
	output = malloc((sizeof *output) * 2 * tesseleation * loop_sizes[loop_count - 1]);
	for(i = j = 0; i < loop_count; i++)
	{
		for(; j < loop_sizes[i]; j++)
		{
			jj = last + (j - last + 1) % (loop_sizes[i] - last);
			jjj = last + (j - last + 2) % (loop_sizes[i] - last);

			if((flags[j] & 1) && (flags[jj] & 1))
			{
				output[used * 2] = vertex_array[j * 2];
				output[used * 2 + 1] = vertex_array[j * 2 + 1];
				used++;
			}else if((flags[j] & 1) && !(flags[jj] & 1) && (flags[jjj] & 1))
			{
				for(k = 0; k < tesseleation * 2; k++)
					seduce_spline(&output[used++ * 2], &vertex_array[j * 2], &vertex_array[jj * 2], &vertex_array[jjj * 2], (float)k / (float)(tesseleation * 2));
				j++;
			}else if((flags[j] & 1) && !(flags[jj] & 1) && !(flags[jjj] & 1))
			{
				b[0] = (vertex_array[jj * 2] + vertex_array[jjj * 2]) * 0.5;
				b[1] = (vertex_array[jj * 2 + 1] + vertex_array[jjj * 2 + 1]) * 0.5;
				for(k = 0; k < tesseleation * 3 / 2; k++)
					seduce_spline(&output[used++ * 2], &vertex_array[j * 2], &vertex_array[jj * 2], b, (float)k / (float)(tesseleation * 3 / 2));
			}else if(!(flags[j] & 1) && !(flags[jj] & 1) && !(flags[jjj] & 1))
			{
				
				a[0] = (vertex_array[j * 2] + vertex_array[jj * 2]) * 0.5;
				a[1] = (vertex_array[j * 2 + 1] + vertex_array[jj * 2 + 1]) * 0.5;
				b[0] = (vertex_array[jj * 2] + vertex_array[jjj * 2]) * 0.5;
				b[1] = (vertex_array[jj * 2 + 1] + vertex_array[jjj * 2 + 1]) * 0.5;
				for(k = 0; k < tesseleation; k++)
					seduce_spline(&output[used++ * 2], a, &vertex_array[jj * 2], b, (float)k / (float)(tesseleation));
			}else if(!(flags[j] & 1) && !(flags[jj] & 1) && (flags[jjj] & 1))
			{
				a[0] = (vertex_array[j * 2] + vertex_array[jj * 2]) * 0.5;
				a[1] = (vertex_array[j * 2 + 1] + vertex_array[jj * 2 + 1]) * 0.5;
				for(k = 0; k < tesseleation * 3 / 2; k++)
					seduce_spline(&output[used++ * 2], a, &vertex_array[jj * 2], &vertex_array[jjj * 2], (float)k / (float)(tesseleation * 3 / 2));
				j++;
			}
		}
		last = loop_sizes[i];
		loop_sizes[i] = used;	
	}
	return output;
}

void hxa_util_true_type_parse_size(float *vertex, unsigned int count, float *min, float *max)
{
	float *output;
	unsigned int i, j;
	*min = *max = vertex[0];
	count *= 2;
	for(i = 2; i < count; i += 2)
	{
		if(vertex[i] < *min)
		{
			*min = vertex[i];
		}else if(vertex[i] > *max)
			*max = vertex[i];
	}
}

int hxa_util_true_type_parse_loader(HxAFont *font, uint8 *data)
{
	HxAFontTable *tables;
	uint32 type, i, j, character_count, pos = 0, lookup_count, *sorted, *locations;
	uint16 number_of_tables, search_range, entry_selector, range_shift, head, loca, glyf, maxp, cmap, loca_type;
	HxAFontCharacterLookup *lookup, *lookup2;
	float *array;

	
	type = hxa_util_true_type_parse_read_uint32(data, &pos);
	number_of_tables = hxa_util_true_type_parse_read_uint16(data, &pos);
	search_range = hxa_util_true_type_parse_read_uint16(data, &pos);
	entry_selector = hxa_util_true_type_parse_read_uint16(data, &pos);
	range_shift = hxa_util_true_type_parse_read_uint16(data, &pos);

//	printf("Font Data %u %u %u %u %u\n", type, (unsigned int)number_of_tables, (unsigned int)search_range, (unsigned int)entry_selector, (unsigned int)range_shift);
	tables = malloc((sizeof *tables) * number_of_tables);
	for(i = 0; i < number_of_tables; i++)
	{
		tables[i].tag[0] = hxa_util_true_type_parse_read_uint8(data, &pos);
		tables[i].tag[1] = hxa_util_true_type_parse_read_uint8(data, &pos);
		tables[i].tag[2] = hxa_util_true_type_parse_read_uint8(data, &pos);
		tables[i].tag[3] = hxa_util_true_type_parse_read_uint8(data, &pos);
		tables[i].tag[4] = 0;
		tables[i].checksum = hxa_util_true_type_parse_read_uint32(data, &pos);
		tables[i].offset = hxa_util_true_type_parse_read_uint32(data, &pos);
		tables[i].length = hxa_util_true_type_parse_read_uint32(data, &pos);
/*		printf("Tag[%u] %s\n", i, tables[i].tag);
		printf("\tchecksum %u\n", tables[i].checksum);
		printf("\toffset %u\n", tables[i].offset);
		printf("\tlength %u\n", tables[i].length);
*/	}
	
	for(cmap = 0; cmap < number_of_tables && !f_text_compare(tables[cmap].tag, "cmap"); cmap++);
	for(maxp = 0; maxp < number_of_tables && !f_text_compare(tables[maxp].tag, "maxp"); maxp++);
	for(head = 0; head < number_of_tables && !f_text_compare(tables[head].tag, "head"); head++);
	for(loca = 0; loca < number_of_tables && !f_text_compare(tables[loca].tag, "loca"); loca++);
	for(glyf = 0; glyf < number_of_tables && !f_text_compare(tables[glyf].tag, "glyf"); glyf++);

	if(cmap >= number_of_tables ||
		maxp >= number_of_tables || 
		head >= number_of_tables || 
		loca >= number_of_tables || 
		glyf >= number_of_tables)
		return FALSE;	

	pos = tables[maxp].offset + 4;/* Read out the character count from the maxp table*/
	character_count = hxa_util_true_type_parse_read_uint16(data, &pos);
	pos = tables[head].offset + 50; /* Read out the location type from the head table*/
	loca_type = hxa_util_true_type_parse_read_uint16(data, &pos);


	pos = tables[cmap].offset;
	lookup = hxa_util_true_type_parse_character_encoding(font, data, &pos, &lookup_count, character_count);

	if(lookup == NULL)
		return FALSE;

	locations = malloc((sizeof *locations) * character_count);
		
	pos = tables[loca].offset; /* Read out the location arrays */
	if(loca_type == 1)
	{
		for(i = 0; i < character_count; i++)
			locations[i] = hxa_util_true_type_parse_read_uint32(data, &pos);
	}else
		for(i = 0; i < character_count; i++)
			locations[i] = hxa_util_true_type_parse_read_uint16(data, &pos) * 2;

	font->lookup = lookup2 = malloc((sizeof *lookup2) * lookup_count);
/*	sorted = f_sort_ids(lookup_count, NULL, hxa_util_true_type_parse_sort_compate_func, lookup);
	for(i = 0; i < lookup_count; i++)
		lookup2[i] = lookup[sorted[i]];
*/	for(i = 0; i < lookup_count; i++)
		lookup2[i] = lookup[i];
//	free(sorted);
	free(lookup);
		
	for(i = 0; i < lookup_count; i++)
	{
		for(j = 0; lookup2[i].ingest_glyph_id != lookup2[j].ingest_glyph_id; j++);
		if(j < i)
			lookup2[i].export_glyph_id = lookup2[j].export_glyph_id;
		else
		{
			lookup2[i].export_glyph_id = font->glyph_count;
			pos = locations[lookup2[i].ingest_glyph_id] + tables[glyf].offset;
			if(!hxa_util_true_type_parse_glyph(font, data, &pos))
			{
//				printf("Failed at reading glyph %u\n", lookup2[i].ingest_glyph_id);
				lookup_count--;
				for(j = i; j < lookup_count; j++)
					lookup2[j] = lookup2[j + 1];
				i--;
			}
		}
	}

	for(i = 0; i < font->glyph_count; i++)
		j = font->glyphs[i].vertex_array[0];
		

	font->lookup_count = lookup_count;		
/*	for(i = 0; i < lookup_count; i++)
	{
		printf("Lookup[%u] %u->%u %u\n", i, lookup2[i].ingest_glyph_id, lookup2[i].export_glyph_id, lookup2[i].character_id);
	}*/
	for(i = 0; i < font->glyph_count; i++)
		j = font->glyphs[i].vertex_array[0];
	return TRUE;
}



void hxa_util_true_type_parse_kern(float *vertex_array, unsigned int *ref, unsigned int poly_count, float top, float bottom, float *min_max)
{
	float array[4] = {0, 0, 1, 0}, output[2];
	unsigned int i, j, k;
	poly_count *= 3;
	for(i = 0; i < poly_count; i++)
	{
		if(vertex_array[ref[i] * 2 + 1] >= bottom && vertex_array[ref[i] * 2 + 1] <= top)
		{
			if(min_max[0] > vertex_array[ref[i] * 2])
				min_max[0] = vertex_array[ref[i] * 2];
			if(min_max[1] < vertex_array[ref[i] * 2])
				min_max[1] = vertex_array[ref[i] * 2];
		}
	}
	array[1] = array[3] = top;
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < poly_count; j += 3)
		{
			for(k = 0; k < 3; k++)
			{
				if((vertex_array[ref[j + k] * 2 + 1] > array[1] && vertex_array[ref[j + (k + 1) % 3] * 2 + 1] < array[1]) ||
					(vertex_array[ref[j + k] * 2 + 1] < array[1] && vertex_array[ref[j + (k + 1) % 3] * 2 + 1] > array[1]))
				{
					f_intersect2f(output, &vertex_array[ref[j + k] * 2], &vertex_array[ref[j + (k + 1) % 3] * 2], array, &array[2]);

					if(min_max[0] > output[0])
						min_max[0] = output[0];
					if(min_max[1] < output[0])
						min_max[1] = output[0];
				}
			}
		}
		array[1] = array[3] = bottom;
	}
}

void hxa_util_true_type_parse_process(HxAFont *font, unsigned int tesselation, unsigned int character_code)
{
	float base, scale, min, max, *array, f;
	unsigned int i, j, count;

	for(i = 0; i < font->glyph_count; i++)
		j = font->glyphs[i].vertex_array[0];


	for(i = 0; i < font->lookup_count && font->lookup[i].character_id != character_code; i++);
	if(i == font->lookup_count)
	{
		printf("Seduce Font ERROR Character code %u not fopund in font. Unable to normalize font size. Exiting\n", character_code);
		return;
	}
	i = font->lookup[i].export_glyph_id;
	hxa_util_true_type_parse_size(&font->glyphs[i].vertex_array[1], font->glyphs[i].vertex_count - 1, &base, &scale);
	scale -= base;
		
	for(i = 0; i < font->glyph_count; i++)
	{
		hxa_util_true_type_parse_size(font->glyphs[i].vertex_array, font->glyphs[i].shapes[font->glyphs[i].shape_count - 1], &min, &max);
		font->glyphs[i].poygonized.ref_count = 0;
		count = font->glyphs[i].shapes[font->glyphs[i].shape_count - 1];
		for(j = 0; j < count; j++)
		{
			font->glyphs[i].vertex_array[j * 2 + 0] = (font->glyphs[i].vertex_array[j * 2 + 0] - min) / scale;
			font->glyphs[i].vertex_array[j * 2 + 1] = (font->glyphs[i].vertex_array[j * 2 + 1] - base) / scale;
		}
		
		array = hxa_util_true_type_parse_smooth(font->glyphs[i].vertex_array, 
									font->glyphs[i].crease, 
									font->glyphs[i].shapes, 
									font->glyphs[i].shape_count, 
									tesselation);

		mo_edit_process_deploy(array, 
								font->glyphs[i].shapes, 
								font->glyphs[i].shape_count, 
								&font->glyphs[i].poygonized);
		free(array);
		
		f = (max - min) * 0.5 / scale;
		for(j = 0; j < 8; j++)
			font->glyphs[i].kerning[j] = f;
		
		hxa_util_true_type_parse_kern(font->glyphs[i].poygonized.vertex_array, font->glyphs[i].poygonized.ref, font->glyphs[i].poygonized.ref_count / 3, 1000, 1.05, &font->glyphs[i].kerning[0]);
		hxa_util_true_type_parse_kern(font->glyphs[i].poygonized.vertex_array, font->glyphs[i].poygonized.ref, font->glyphs[i].poygonized.ref_count / 3, 1.05, 0.35, &font->glyphs[i].kerning[2]);
		hxa_util_true_type_parse_kern(font->glyphs[i].poygonized.vertex_array, font->glyphs[i].poygonized.ref, font->glyphs[i].poygonized.ref_count / 3, 0.35, -0.05, &font->glyphs[i].kerning[4]);
		hxa_util_true_type_parse_kern(font->glyphs[i].poygonized.vertex_array, font->glyphs[i].poygonized.ref, font->glyphs[i].poygonized.ref_count / 3, -0.05, -1000.0, &font->glyphs[i].kerning[6]);
		
	}
}

HXAFile *hxa_util_true_type_convert(HxAFont *font)
{
	HXAFile *file;
	char *name_front = "kerning_front", *name_back = "kerning_back", *name_unicode = "unicode_character", *name_vertex = HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME, *name_reference = HXA_CONVENTION_HARD_BASE_CORNER_LAYER_NAME;
	unsigned int i, j, charcter_count;
	file = malloc(sizeof *file);
	file->version = HXA_VERSION_FORMAT;
	file->node_count = font->glyph_count;
	file->node_array = malloc((sizeof *file->node_array) * file->node_count);
	for(i = 0; i < file->node_count; i++)
	{
		file->node_array[i].meta_data_count = 0;
		file->node_array[i].meta_data = NULL;
		file->node_array[i].meta_data_count = 2;
		file->node_array[i].meta_data = malloc((sizeof *file->node_array[i].meta_data) * 3);
		/* Kerning front */
		for(j = 0; name_front[j] != 0; j++)
			file->node_array[i].meta_data[0].name[j] = name_front[j];
		file->node_array[i].meta_data[0].name[j] = 0;
		file->node_array[i].meta_data[0].array_length = 4;
		file->node_array[i].meta_data[0].value.double_value = malloc((sizeof *file->node_array[i].meta_data[0].value.double_value) * file->node_array[i].meta_data[0].array_length);
		for(j = 0; j < 4; j++)
			file->node_array[i].meta_data[0].value.double_value[j] = font->glyphs[i].kerning[j];
		/* Kerning back */
		for(j = 0; name_back[j] != 0; j++)
			file->node_array[i].meta_data[1].name[j] = name_back[j];
		file->node_array[i].meta_data[1].name[j] = 0;
		file->node_array[i].meta_data[1].array_length = 4;
		file->node_array[i].meta_data[1].value.double_value = malloc((sizeof *file->node_array[i].meta_data[1].value.double_value) * file->node_array[i].meta_data[1].array_length);
		for(j = 0; j < 4; j++)
			file->node_array[i].meta_data[1].value.double_value[j] = font->glyphs[i].kerning[j + 4];

		/* Unicode characters */
		charcter_count = 0;
		for(j = 0; j < font->lookup_count; j++)
			if(font->lookup[j].export_glyph_id == i)
				charcter_count++;
		if(charcter_count > 0)
		{	
			file->node_array[i].meta_data_count = 3;
			file->node_array[i].meta_data[2].array_length = charcter_count;
			for(j = 0; name_unicode[j] != 0; j++)
				file->node_array[i].meta_data[2].name[j] = name_unicode[j];
			file->node_array[i].meta_data[2].name[j] = 0;
			file->node_array[i].meta_data[2].array_length = 0;
			file->node_array[i].meta_data[2].value.int64_value = malloc((sizeof *file->node_array[i].meta_data[2].value.int64_value) * file->node_array[i].meta_data[2].array_length);
			file->node_array[i].meta_data[2].array_length = 0;
			for(j = 0; j < font->lookup_count; j++)
				if(font->lookup[j].export_glyph_id == i)
					file->node_array[i].meta_data[2].value.int64_value[file->node_array[i].meta_data[2].array_length++] = font->lookup[j].character_id;
		}
				
		file->node_array[i].type = HXA_NT_GEOMETRY;
		file->node_array[i].content.geometry.vertex_stack.layer_count = 1; 
		file->node_array[i].content.geometry.vertex_stack.layers = malloc(sizeof *file->node_array[i].content.geometry.vertex_stack.layers);
		file->node_array[i].content.geometry.vertex_stack.layers[0].components = HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_COMPONENTS;
		for(j = 0; name_vertex[j] != 0; j++)
			file->node_array[i].content.geometry.vertex_stack.layers[0].name[j] = name_vertex[j];
		file->node_array[i].content.geometry.vertex_stack.layers[0].name[j] = 0;
		file->node_array[i].content.geometry.vertex_stack.layers[0].type = HXA_LDT_FLOAT;
		file->node_array[i].content.geometry.vertex_stack.layers[0].data.float_data = malloc(sizeof(float) * 3 * font->glyphs[i].poygonized.vertex_count);
		for(j = 0; j < font->glyphs[i].poygonized.vertex_count; j++)
		{
			file->node_array[i].content.geometry.vertex_stack.layers[0].data.float_data[j * 3] = font->glyphs[i].poygonized.vertex_array[j * 2];
			file->node_array[i].content.geometry.vertex_stack.layers[0].data.float_data[j * 3 + 1] = font->glyphs[i].poygonized.vertex_array[j * 2 + 1];
			file->node_array[i].content.geometry.vertex_stack.layers[0].data.float_data[j * 3 + 2] = 0;
		}
		file->node_array[i].content.geometry.corner_stack.layer_count = 1; 
		file->node_array[i].content.geometry.corner_stack.layers = malloc(sizeof *file->node_array[i].content.geometry.corner_stack.layers);
		file->node_array[i].content.geometry.corner_stack.layers[0].components = HXA_CONVENTION_HARD_BASE_CORNER_LAYER_COMPONENTS;
		for(j = 0; name_reference[j] != 0; j++)
			file->node_array[i].content.geometry.corner_stack.layers[0].name[j] = name_reference[j];
		file->node_array[i].content.geometry.corner_stack.layers[0].name[j] = 0;
		file->node_array[i].content.geometry.corner_stack.layers[0].type = HXA_CONVENTION_HARD_BASE_CORNER_LAYER_TYPE;
		file->node_array[i].content.geometry.corner_stack.layers[0].data.float_data = malloc(sizeof(int) * font->glyphs[i].poygonized.ref_count);
		for(j = 0; j < font->glyphs[i].poygonized.ref_count; j += 3)
		{
			file->node_array[i].content.geometry.corner_stack.layers[0].data.int32_data[j] = (int)font->glyphs[i].poygonized.ref[j];
			file->node_array[i].content.geometry.corner_stack.layers[0].data.int32_data[j + 1] = (int)font->glyphs[i].poygonized.ref[j + 1];
			file->node_array[i].content.geometry.corner_stack.layers[0].data.int32_data[j + 2] = -1 - (int)font->glyphs[i].poygonized.ref[j + 2];
		}
		file->node_array[i].content.geometry.vertex_count = font->glyphs[i].poygonized.vertex_count;
		file->node_array[i].content.geometry.edge_corner_count = font->glyphs[i].poygonized.ref_count;
		file->node_array[i].content.geometry.edge_stack.layer_count = 0;
		file->node_array[i].content.geometry.edge_stack.layers = NULL;
		file->node_array[i].content.geometry.face_count = font->glyphs[i].poygonized.ref_count / 3;
		file->node_array[i].content.geometry.face_stack.layer_count = 0;
		file->node_array[i].content.geometry.face_stack.layers = NULL;
	}
	return file;
}


HXAFile *hxa_util_true_type_load(char *file_name)
{
	HXAFile *file;
	HxAFont font;
	size_t size;
	FILE *f;
	uint8 *buffer;
	f = fopen(file_name, "rb");
	if(f == NULL)
		return NULL; 
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	if(f == NULL)
	{
		fclose(f);
		return NULL; 
	}
	buffer = malloc(size);
	size = fread(buffer, 1, size, f);
	fclose(f);
	font.glyphs = NULL;
	font.glyph_count = 0;
	font.lookup = NULL;
	font.lookup_count = 0;
	hxa_util_true_type_parse_loader(&font, buffer);
	free(buffer);
	hxa_util_true_type_parse_process(&font, 2, 'v');
	
	file = hxa_util_true_type_convert(&font);
	if(font.glyphs != NULL)
		free(font.glyphs);
	if(font.lookup != NULL)
		free(font.lookup);
	return file;
}