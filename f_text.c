
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "forge.h"

#define FORGE_NXT	0x80
#define FORGE_SEQ2	0xc0
#define FORGE_SEQ3	0xe0
#define FORGE_SEQ4	0xf0
#define FORGE_SEQ5	0xf8
#define FORGE_SEQ6	0xfc
#define FORGE_BOM	0xfeff

uint32 f_utf8_to_uint32(char *c, uint *pos)
{
	uint i, bits, character_count = 1, high;
	uint32 out;

	if((c[*pos] & 0x80) == 0)
		high = (wchar_t)c[*pos];
	else if((c[*pos] & 0xe0) == FORGE_SEQ2)
	{
		character_count = 2;
		high = (wchar_t)(c[*pos] & 0x1f);
	}else if((c[*pos] & 0xf0) == FORGE_SEQ3)
	{
		character_count = 3;
		high = (wchar_t)(c[*pos] & 0x0f);
	}else if((c[*pos] & 0xf8) == FORGE_SEQ4)
	{
		character_count = 4;
		high = (wchar_t)(c[*pos] & 0x07);
	}else if((c[*pos] & 0xfc) == FORGE_SEQ5)
	{
		character_count = 5;
		high = (wchar_t)(c[*pos] & 0x03);
	}else if((c[*pos] & 0xfe) == FORGE_SEQ6)
	{
		character_count = 6;
		high = (wchar_t)(c[*pos] & 0x01);
	}else
	{
		(*pos)++;
		return 0;
	}
	out = 0;
	bits = 0;
	for(i = 1; i < character_count; i++)
	{
		out |= (wchar_t)(c[character_count - i] & 0x3f) << bits;
		bits += 6;		/* 6 low bits in every byte */
	}
	out |= high << bits;
	(*pos) += character_count;
	return out;
}

uint f_uint32_to_utf8(uint32 character, char *out)
{
	uint8 bytes[4];
	bytes[0] = (character >> 24) & 0xFF;
	bytes[1] = (character >> 16) & 0xFF;
	bytes[2] = (character >> 8) & 0xFF;
	bytes[3] = character & 0xFF;
	if(character <= 0x0000007f)
	{
		out[0] = bytes[3];
		return 1;
	}else if(character <= 0x000007ff)
	{
		out[1] = FORGE_NXT | bytes[3] & 0x3f;
		out[0] = FORGE_SEQ2 | (bytes[3] >> 6) | ((bytes[2] & 0x07) << 2);
		return 2;
	}else if(character <= 0x0000ffff)
	{
		out[2] = FORGE_NXT | bytes[3] & 0x3f;
		out[1] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[0] = FORGE_SEQ3 | ((bytes[2] & 0xf0) >> 4);
		return 3;
	}else if(character <= 0x001fffff)
	{
		out[3] = FORGE_NXT | bytes[3] & 0x3f;
		out[2] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[1] = FORGE_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[0] = FORGE_SEQ4 | ((bytes[1] & 0x1f) >> 2);
		return 4;
	}else if(character <= 0x03ffffff)
	{
		out[4] = FORGE_NXT | bytes[3] & 0x3f;
		out[3] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[2] = FORGE_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[1] = FORGE_NXT | (bytes[1] >> 2);
		out[0] = FORGE_SEQ5 | bytes[0] & 0x03;
		return 5;
	}else /* if (*w <= 0x7fffffff) */
	{
		out[5] = FORGE_NXT | bytes[3] & 0x3f;
		out[4] = FORGE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[3] = FORGE_NXT | (bytes[2] >> 4) | ((bytes[1] & 0x03) << 4);
		out[2] = FORGE_NXT | (bytes[1] >> 2);
		out[1] = FORGE_NXT | bytes[0] & 0x3f;
		out[0] = FORGE_SEQ6 | ((bytes[0] & 0x40) >> 6);
		return 6;
	}
}

uint f_find_next_word(char *text)
{
	uint i;
	for(i = 1; text[i] > 32; i++);
	return i;
}

boolean f_find_word_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] > 32 && text_b[i] > 32; i++);
	return (text_a[i] < 32 && text_b[i] < 32);
}

boolean f_text_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] != 0; i++);
	return text_a[i] == text_b[i];
}

uint f_text_copy(uint length, char *dest, char *source)
{
	uint i;
	for(i = 0; i < length && source[i] != 0; i++)
		dest[i] = source[i];
	if(i == length)
		return 0;
	dest[i] = source[i];
	return i;
}


char *f_text_copy_allocate(char *source)
{
	uint i;
	char *text;
	for(i = 0; source[i] != 0; i++);
	text = malloc((sizeof *text) * (i + 1));
	for(i = 0; source[i] != 0; i++)
		text[i] = source[i];
	text[i] = 0;
	return text;
}

uint f_word_copy(uint length, char *dest, char *source)
{
	uint i = 0, j;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] > 32)
		dest[i++] = source[j++];
	if(i == length)
		return 0;
	dest[i] = 0;
	return j;
}


uint f_text_copy_until(uint length, char *dest, char *source, char *until)
{
	uint i = 0, j, k, l;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] != 0)
	{
		for(k = 0; source[j + k] == until[k] && until[k] != 0 ; k++);
		if(until[k] == 0)
		{
			for(i--; dest[i] < 32; i--);
			dest[i] = 0;
			return j + k;
		}
		else
			dest[i++] = source[j++];
	}
	return 0;
}

boolean	f_text_filter(char *text, char *filter)
{
	uint i, j;
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == *filter)
		{
			for(j = 1; text[j] != 0 && text[i + j] == filter[j]; j++);
			if(filter[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}

boolean	f_text_unique(char *text, uint buffer_length, char *compare)
{
	uint i, length, multiplyer, sum;
	for(length = 0; text[length] != 0 && text[length] == compare[length]; length++);
	if(text[length] != compare[length])
		return TRUE;
	if(length == 0)
	{
		text[length] = '1';
		text[length + 1] = 0;
		return FALSE;
	}
	else if(text[length - 1] > '9' || text[length - 1] < '0')
	{
		if(length < buffer_length - 1)
		{
			text[length] = '2';
			text[length + 1] = 0;
		}else
			 text[length - 1] = '2';
		return FALSE;
	}
	if(text[length - 1] == '9')
	{
		multiplyer = 1;
		sum = 0;
		for(i = length; i != 0 && text[i - 1]  >= '0' && text[i - 1] <= '9'; i--)
		{
			sum += (text[i - 1] - '0') * multiplyer;
			multiplyer *= 10;
		}
		sum++;
		if(sum != multiplyer)
			multiplyer /= 10;
		else if(length + 1 >= buffer_length)
			i--;
			
		for(; multiplyer != 0; i++)
		{
			text[i] = '0' + sum / multiplyer;
			sum %= multiplyer;
			multiplyer /= 10;
		}
		text[i] = 0;
	}else
		text[length - 1]++;
	return FALSE;
}

char *f_text_load(char *file_name, size_t *size)
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


uint f_text_parce_hex(char *text, uint64 *output)
{
	uint64 out = 0;
	uint i;
	for(i = 0; ;i++)
	{
		if(text[i] != 'x')
		if(text[i] >= '0' && text[i] <= '9')
		{
			out *= 16;
			out += text[i] - '0';
		}else if(text[i] >= 'A' && text[i] <= 'F')
		{
			out *= 16;
			out += text[i] - 'A' + 10;
		}else if(text[i] >= 'a' && text[i] <= 'f')
		{
			out *= 16;
			out += text[i] - 'a' + 10;
		}else if(text[i] != 'x')
		{
			*output = out;
			return i;
		}
	}
	*output = 0;
	return 0;
}

uint f_text_parce_decimal(char *text, uint64 *output)
{
	uint64 out = 0;
	uint i;
	for(i = 0; ;i++)
	{
		if(text[i] >= '0' && text[i] <= '9')
		{
			out *= 10;
			out += text[i] - '0';
		}else
		{
			*output = out;
			return i;
		}
	}
	*output = 0;
	return 0;
}

uint f_text_parce_real(char *text, int64 *integer_output, double *real_output, boolean *decimal)
{
	int64 out = 0, exponent = 0;
	boolean comma = FALSE, neg = FALSE, sientific_neg = FALSE;
	double divider = 0.1;
	uint i = 0, j;
	*integer_output = 0;

	if(text[0] == '-')
	{
		neg = TRUE;
		i = 1;
	}

	if(text[i] == '0' && (text[i + 1] == 'x' || text[i + 1] == 'X'))
	{
		*decimal = FALSE;
		i += 2;
		i += f_text_parce_hex(&text[i], integer_output);
		if(neg)
			*integer_output = -*integer_output;
		*real_output = (double)*integer_output;
		return i;
	}
	

	for(; ;i++)
	{
		if(text[i] >= '0' && text[i] <= '9')
		{
			out *= 10;
			out += text[i] - '0';
		}else if(text[i] == '.')
			break;
		else if((text[i] == 'e' || text[i] == 'E') && ((text[i + 1] >= '0' && text[i + 1] <= '9') || (text[i + 1] == '-' && (text[i + 2] >= '0' && text[i + 2] <= '9'))))
		{
			i++;
			if(neg)
				out = -out;
			*real_output = (double)out;
			*integer_output = out;
			if(text[i] == '-')
			{
				sientific_neg = TRUE;
				*decimal = TRUE;
				i++;
				while(text[i] >= '0' && text[i] <= '9')
				{
					exponent *= 10;
					exponent += text[i++] - '0';
				}
				for(j = 0; j < exponent; j++)
				{
					*real_output /= 10.0;
					*integer_output /= 10;
				}
				return i;
			}
			while(text[i] >= '0' && text[i] <= '9')
			{
				exponent *= 10;
				exponent += text[i++] - '0';
			}
			for(j = 0; j < exponent; j++)
			{
				*real_output *= 10.0;
				*integer_output *= 10;
			}
			return i;
		}else
		{
			*decimal = FALSE;
			if(neg)
				out = -out;
			*real_output = (double)out;
			*integer_output = out;
			return i;
		}
	}
	*decimal = TRUE;
	*real_output = (double)out;
	*integer_output = out;
	for(i++; ;i++)
	{
		if(text[i] >= '0' && text[i] <= '9')
			*real_output += divider * (double)(text[i] - '0');
		else
			break;
		divider /= 10.0;
	}
	if(neg)
	{
		*real_output = -*real_output;
		*integer_output = -*integer_output;
	}

	if((text[i] == 'e' || text[i] == 'E') && ((text[i + 1] >= '0' && text[i + 1] <= '9') || (text[i + 1] == '-' && (text[i + 2] >= '0' && text[i + 2] <= '9'))))
	{
		i++;
		if(text[i] == '-')
		{
			sientific_neg = TRUE;
			*decimal = TRUE;
			i++;
			while(text[i] >= '0' && text[i] <= '9')
			{
				exponent *= 10;
				exponent += text[i++] - '0';
			}
			for(j = 0; j < exponent; j++)
			{
				*real_output /= 10.0;
				*integer_output /= 10;
			}
			return i;
		}
		while(text[i] >= '0' && text[i] <= '9')
		{
			exponent *= 10;
			exponent += text[i] - '0';
		}
		for(j = 0; j < exponent; j++)
		{
			*real_output *= 10.0;
			*integer_output *= 10;
		}
	}
	return i;
}


uint f_text_parce_double(char *text, double *real_output)
{
	uint64 integer_output;
	boolean decimal;
	return f_text_parce_real(text, &integer_output, real_output, &decimal);
}


char *f_ascii_codes[] = {"NULL",
		"Start of header",
		"Start of text",
		"End of text",
		"End of transmission",
		"Enquiry",
		"Acknowledgment",
		"Bell",
		"Backspace",
		"Horizontal Tab",
		"Line feed",
		"Vertical tab",
		"Form feed",
		"Carriage return",
		"Shift out",
		"Shift in",
		"Data Link Escape",
		"Device control 1",
		"Device control 2",
		"Device control 3",
		"Device control 4",
		"Negative Acknowledgment",
		"Syncronus idle",
		"End of transmission block",
		"Cansel",
		"End of medium",
		"Substitute",
		"Ecscape",
		"File separator",
		"Group separator",
		"Record separator",
		"Unit separator",
		"Space"};


void f_print_raw(uint8 *data, uint size)
{
	uint i, j;
	boolean start = TRUE;
	char text[2] = {0, 0};
	for(i = 0; i < size; i++)
	{
		if(data[i] < 33)
		{
			if(data[i] != 32)
				start = TRUE;
			printf("data[%u] = %u - %s\n", i, (uint)data[i], f_ascii_codes[(uint)data[i]]);
		}else if(data[i] < 127)
		{
			text[0] = data[i];
			printf("data[%u] = %u - %s", i, (uint)data[i], text);
			if(start && i + 1 < size && data[i + 1] < 127 && data[i + 1] >= 32)
			{
				printf(" - \"");
				for(j = i; j < size && data[j] < 127 && data[j] >= 32; j++)
				{
					text[0] = data[j];
					printf("%s", text);
				}
				printf("\"\n");
			}else
				printf("\n");
			start = FALSE;
		}else
		{		
			printf("data[%u] = %u\n", i, (uint)data[i]);
			start = FALSE;
		}
	}
}

void f_fprint_raw(FILE *file, uint8 *data, uint size)
{
	uint i, j;
	boolean start = TRUE;
	char text[2] = {0, 0};
	for(i = 0; i < size; i++)
	{
		if(data[i] < 33)
		{
			if(data[i] != 32)
				start = TRUE;
			fprintf(file, "data[%u] = %u - %s\n", i, (uint)data[i], f_ascii_codes[(uint)data[i]]);
		}else if(data[i] < 127)
		{
			text[0] = data[i];
			fprintf(file, "data[%u] = %u - %s", i, (uint)data[i], text);
			if(start && i + 1 < size && data[i + 1] < 127 && data[i + 1] >= 32)
			{
				fprintf(file, " - \"");
				for(j = i; j < size && data[j] < 127 && data[j] >= 32; j++)
				{
					text[0] = data[j];
					fprintf(file, "%s", text);
				}
				fprintf(file, "\"\n");
			}else
				fprintf(file, "\n");
			start = FALSE;
		}else
		{
			fprintf(file, "data[%u] = %u\n", i, (uint)data[i]);
			start = FALSE;
		}
	}
}

/*
void f_generate_obfuscation_table()
{
	uint8 from_list[256], to_list[256], letters[10 + 26 + 26];
	uint i, new_letter;
	for(i = 0; i < 256; i++)
		from_list[i] = to_list[i] = i;
		
	for(i = 0; i < 10; i++)
		letters[i] = '0' + i;
	for(; i < 10 + 26; i++)
		letters[i] = 'A' + i - 10;
	for(; i < 10 + 26 + 26; i++)
		letters[i] = 'a' + i - 10 - 26;
		
	for(i = 0; i < 10 + 26 + 26; i++)
	{
		if(from_list[letters[i]] == letters[i])
		{
			new_letter = 1 + (f_randi(i) % (10 + 26 + 26 - 1));
			while(from_list[letters[new_letter]] != letters[new_letter])
				new_letter++;
			from_list[letters[i]] = letters[new_letter];
			from_list[letters[new_letter]] = letters[i];
		}
	}
	
	for(i = 0; i < 256; i++)
		to_list[from_list[i]] = i;

	printf("char f_obfuscation_table[256] = {0");
	for(i = 1; i < 256; i++)
		printf(", %u", from_list[i]);
	printf("};\n", from_list[i]);

}
*/

char f_obfuscation_table[256] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 84, 78, 98, 71, 79, 122, 66, 97, 72, 75, 58, 59, 60, 61, 62, 63, 64, 107, 54, 112, 99, 119, 114, 51, 56, 85, 80, 57, 113, 120, 49, 52, 74, 82, 81, 86, 48, 73, 83, 88, 87, 115, 100, 91, 92, 93, 94, 95, 96, 55, 50, 68, 90, 103, 108, 101, 105, 104, 109, 65, 102, 106, 111, 110, 67, 76, 70, 89, 117, 116, 121, 69, 77, 118, 53, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

boolean f_text_obfuscate(char *out_buffer, uint buffer_size, char *in_buffer)
{
	uint i;
	buffer_size--;
	for(i = 0; i < buffer_size && in_buffer[i] != 0; i++)
		out_buffer[i] = f_obfuscation_table[(uint)in_buffer[i]];
	out_buffer[i] = 0;
	return in_buffer[i] == 0;
}

void f_text_obfuscate_print(char *in_buffer)
{
	uint i;
	for(i = 0; in_buffer[i] != 0; i++)
	{
		if(in_buffer[i] < 32 || in_buffer[i] == '\"' || in_buffer[i] == '\'' || in_buffer[i] == '\\' || in_buffer[i] == '\\')
			printf("\\%u", f_obfuscation_table[(uint)in_buffer[i]]);
		else
			printf("%c", f_obfuscation_table[(uint)in_buffer[i]]);
	}
}


#define FORGE_BITS_TO_TEXT_SKIP 32
#define FORGE_BITS_TO_TEXT_ERROR 33

unsigned int f_text_to_bits_lookup_table[] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32, 32, 14, 8, 25, 26, 27, 28, 29, 30, 31, 32, 33, 33, 33, 33, 33, 33, 33, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 33, 32, 33, 33, 33, 33, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33};
char f_bit_to_text_lookup_table[] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 87, 88, 89, 90, 50, 51, 52, 53, 54, 55, 56};
/* A B C D E F G H I J K L M N O P Q R S T U W X Y Z 2 3 4 5 6 7 8 */

uint f_text_to_bits(uint64 *bits, char *text)
{
	uint64 out = 0, code;
	uint pos = 60, i;	
	for(i = 0; text[i] != 0; i++)
	{
		code = f_text_to_bits_lookup_table[(unsigned char)text[i]];
		if(code < FORGE_BITS_TO_TEXT_SKIP)
		{
			out |= code + code << pos;
			if(pos == 0)
			{
				*bits = out;
				return i + 1;				
			}
			pos -= 3;
		}else if(code == FORGE_BITS_TO_TEXT_ERROR)
			return 0;
		 
	}
	return 0;
}

void f_bits_to_text(uint64 bits, char *text)
{
	uint i, code;	
	for(i = 0; i < 13; i++)
	{
		text[i] = f_bit_to_text_lookup_table[bits & 31];
		bits = bits >> 5;
	}
	text[i] = 0;
}



void f_generate_bits_to_text()
{
	char characters[2] = {0, 0};
	uint read[256], write[32];
	uint i;
	for(i = 0; i <= ' '; i++)
		read[i] = FORGE_BITS_TO_TEXT_SKIP;
	for(; i < 256; i++)
		read[i] = FORGE_BITS_TO_TEXT_ERROR;
	read['/'] = FORGE_BITS_TO_TEXT_SKIP;
	read['\\'] = FORGE_BITS_TO_TEXT_SKIP;
	read['.'] = FORGE_BITS_TO_TEXT_SKIP;
	read['-'] = FORGE_BITS_TO_TEXT_SKIP;
	read['+'] = FORGE_BITS_TO_TEXT_SKIP;
	read[','] = FORGE_BITS_TO_TEXT_SKIP;
	for(i = 'A'; i < 'V'; i++)
		read[i] = read[i + 32] = i - 'A';
	for(i = 'V' + 1; i <= 'Z'; i++)
		read[i] = read[i + 32] = i - 'A' - 1;
	for(i = 0; i <= 32 - ('Z' - 'A'); i++)
		read[i + '2'] = ('Z' - 'A') + i;
	read['V'] = read['U'];
	read['v'] = read['U'];
	read['0'] = read['O'];
	read['1'] = read['I'];
	printf(" */\n");
	printf("unsigned int f_text_to_bits_lookup_table[] = {%u", read[0]);
	for(i = 1; i < 256; i++)
		printf(", %u", read[i]);
	printf("};\n");
	printf("/*");
	for(i = 0; i < 128; i++)
	{
		characters[0] = i;
		if(read[i] == FORGE_BITS_TO_TEXT_SKIP)
			printf(" \'%s\' == Skip\n", characters);
		else if(read[i] == FORGE_BITS_TO_TEXT_ERROR)
			printf(" \'%s\' == Error\n", characters);
		else
			printf(" \'%s\' == %u\n", characters, read[i]);
	}	
	printf(" */");

	for(i = 0; i < 'V' - 'A'; i++)
		write[i] = 'A' + i;
	for(; i < 'Z' - 'A'; i++)
		write[i] = 'A' + i + 1; 
	for(; i < 32; i++)
		write[i] = '2' + i - ('Z' - 'A');
	printf("char f_bit_to_text_lookup_table[] = {%u", write[0]);
	for(i = 1; i < 32; i++)
		printf(", %u", write[i]);
	printf("};\n");
	printf("/*");
	for(i = 0; i < 32; i++)
	{
		characters[0] = write[i];
		printf(" %s", characters);
	}	
	printf(" */");
}
