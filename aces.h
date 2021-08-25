
/* This is an implementation of The Academy Color Encoding Sytem (ACES)  https://acescentral.com/, 
a sub sextion of OpenEXR a file format for storing Hight Dynamic Range images. It writen with 
the intenetion of creating a Free, small, safe, C89, reference implementation. You are free to use it
for what ever you want but if you do, consider sending an E-mail to eskil@quelsolaar.com and let 
me know if you find it useful as it will surly make my day.*/

typedef enum{
	ACES_ATTRIBUTE_TYPE_BOX2I,
	ACES_ATTRIBUTE_TYPE_CHLIST,
	ACES_ATTRIBUTE_TYPE_CHROMATICITIES,
	ACES_ATTRIBUTE_TYPE_COMPRESSION,
	ACES_ATTRIBUTE_TYPE_DOUBLE,
	ACES_ATTRIBUTE_TYPE_FLOAT,
	ACES_ATTRIBUTE_TYPE_HALF,
	ACES_ATTRIBUTE_TYPE_INT,
	ACES_ATTRIBUTE_TYPE_LINEORDER,
	ACES_ATTRIBUTE_TYPE_KEYCODE,
	ACES_ATTRIBUTE_TYPE_RATIONAL,
	ACES_ATTRIBUTE_TYPE_SHORT,
	ACES_ATTRIBUTE_TYPE_STRING,
	ACES_ATTRIBUTE_TYPE_STRINGVECTOR,
	ACES_ATTRIBUTE_TYPE_TIMECODE,
	ACES_ATTRIBUTE_TYPE_UNSIGNEDCHAR,
	ACES_ATTRIBUTE_TYPE_UNSIGNEDINT,
	ACES_ATTRIBUTE_TYPE_UNSIGNEDLONG,
	ACES_ATTRIBUTE_TYPE_UNSIGNEDSHORT,
	ACES_ATTRIBUTE_TYPE_V2F,
	ACES_ATTRIBUTE_TYPE_V3F,
	ACES_ATTRIBUTE_TYPE_COUNT,
	ACES_ATTRIBUTE_TYPE_UNKNOWN = ACES_ATTRIBUTE_TYPE_COUNT,
	ACES_ATTRIBUTE_TYPE_UNUSED
}ACESAttributeTypes;

typedef enum{
    ACES_ACPT_UINT32 = 0,
    ACES_ACPT_HALF16 = 1,
    ACES_ACPT_FLOAT32 = 2,
}ACESAttributeClistPixelType;

typedef struct{
	char name[256];
	ACESAttributeClistPixelType pixelType;
	unsigned int pLinear;
	int xSampling;
	int ySampling;
}ACESAttributeClist;

typedef struct{
	ACESAttributeTypes type;
	char name[256];
	union{
		struct{
			int x_min;
			int y_min;
			int x_max;
			int y_max;
		}box2i;
		struct{
			unsigned int length;
			ACESAttributeClist *list;
		}chlist;
		struct{
			float redX;
			float redY;
			float greenX;
			float greenY;
			float blueX;
			float blueY;
			float whiteX;
			float whiteY;
		}chromaticities;
		unsigned char compression;
		double type_double;
		float type_float;
		unsigned short type_half;
		unsigned int type_int;
		unsigned char lineOrder;
		struct{
			int filmMfCCode;
			int filmType;
			int prefix;
			int count;
			int perfOffset;
			int perfsPerFrame;
			int perfsPerCount;
		}keycode;
		struct{
			int n;
			unsigned int b;
		}rational;
		short type_short;
		struct{
			char *string;
			unsigned int length;
		}string;
		struct{
			char *string;
			unsigned int length;
		}stringVector;
		struct{
			unsigned int timeAndFlags;
			unsigned int userData;
		}timecode;
		unsigned char unsignedChar;
		unsigned int unsignedInt;
		unsigned long unsignedLong;
		unsigned short unsignedShort;
		float v2f[2];
		float v3f[3];
		struct{
			char *type_name;
			unsigned int length;
			void *data;
		}unknown;
	}data;
}ACESAttributeType;


typedef struct{
	char name[256];
	unsigned int channel_count;
	ACESAttributeClistPixelType pixelType;
	union{
		unsigned int *integer_buffer;
		unsigned short *half_buffer;
		float *float_buffer;
	}pixels;
}ACESLayer;

typedef struct{
	ACESAttributeType *attributes;
	unsigned int attribute_count;
	unsigned int attribute_allocate;
	ACESLayer *layers;
	unsigned int layer_count;
}ACESImage;

typedef enum{
	ACES_TC_NO_NOT_CONVERT,
	ACES_TC_CONVERT_ALL_FLOAT16_TO_FLOAT32,
	ACES_TC_CONVERT_ALL_FLOAT32_TO_FLOAT16
}ACESTypeConvesion;

extern float		aces_float16_to_float32(unsigned short value);
extern short		aces_float32_to_float16(float value);

extern ACESImage	*aces_load(char *file_path, int silent, ACESTypeConvesion conversion);
extern void			aces_free(ACESImage *image);