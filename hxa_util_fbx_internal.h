#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
	FBX_PROPERTY_TYPE_BOOLEAN,
	FBX_PROPERTY_TYPE_INT8,
	FBX_PROPERTY_TYPE_INT16,
	FBX_PROPERTY_TYPE_INT32,
	FBX_PROPERTY_TYPE_INT64,
	FBX_PROPERTY_TYPE_FLOAT32,
	FBX_PROPERTY_TYPE_FLOAT64,
	FBX_PROPERTY_TYPE_TEXT,
	FBX_PROPERTY_TYPE_RAW,
	FBX_PROPERTY_TYPE_UNKNOWN,
	FBX_PROPERTY_TYPE_COUNT
}FBXPropertyType;

extern const unsigned int fbx_property_size[FBX_PROPERTY_TYPE_COUNT];

typedef struct{
	unsigned int length;
	union{
		unsigned char  raw[1];
		char text[1];
	}buffer;
}FBXBuffer;

typedef struct{
	FBXPropertyType type;
	unsigned int array_length;
	union{
		char boolean_type;
		char int8_type;
		short int16_type;
		unsigned int int32_type;
		long long int64_type;
		float float32_type;
		double float64_type;
		void *array;
		FBXBuffer *buffer;
	}data;
}FBXProperty;

typedef struct{
	unsigned long long end_ofset;
	unsigned long long property_count;
	unsigned long long property_list_length;
	unsigned char name_length;
	char name[256];
	FBXProperty *property;
	void *children;
	unsigned int allocated;
	unsigned int used;
}FBXRecord;


extern HXAFile *hxa_fbx_convert(FBXRecord *record, HXAFile *file);
extern void fbx_record_print(FBXRecord *record, unsigned int generation);
extern void fbx_record_print_small(FBXRecord *record, unsigned int generation);