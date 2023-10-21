## HxA 3D asset format
---------------------

HxA is a interchangeable graphics asset format. Written by Eskil Steenberg. @quelsolaar / eskil 'at' obsession 'dot' se / www.quelsolaar.com

# Rationale
-----------

- Does the world need another Graphics file format?

Unfortunately, Yes. All existing formats are either too large and complicated to be implemented from scratch, or don't have some basic features needed in modern computer graphics.

- Who is this format for?

For people who want a capable open Graphics format that can be implemented from scratch in a few hours. It is ideal for graphics researchers, game developers or other people who wants to build custom graphics pipelines. Given how easy it is to parse and write, it should be easy to write utilities that process assets to perform tasks like: generating normals, lightmaps, tangent spaces, Error detection, GPU optimization, LOD generation, and UV mapping.

- Why store images in the format when there are so many good image formats already?

Yes there are, but only for 2D RGB/RGBA images. A lot of computer graphics rendering rely on 1D, 3D, CUBE, Multilayer, multi channel, floating point bitmap buffers. There almost no formats for this kind of data. Also 3D files that reference separate image files rely on file paths, and this often creates issues when the assets are moved. By including the texture data in the files directly the assets become self contained.

- Why doesn't the format support <insert whatever>?

The entire point of HxA is to make a format that is practical to implement. Features like NURBSs, Construction history, or BSP trees would make the format too large to serve its purpose. The facilities of the formats to store meta data should make the format flexible enough for most uses. Adding HxA support should be something anyone can do in a days work.

# Structure
------------

HxA is designed to be extremely simple to parse, and is therefore based around conventions. It has a few basic structures, and depending on how they are used they mean different things. This means that you can implement a tool that loads the entire file, modifies the parts it cares about and leaves the rest intact. It is also possible to write a tool that makes all data in the file editable without the need to understand its use. It is also possible for anyone to use the format to store data axillary data. Anyone who wants to store data not covered by a convention can submit a convention to extend the format. There should never be a convention for storing the same data in two differed ways.

The data is stored in a number of nodes themselves stored in an array. Each node stores an array of meta data. Meta data can describe anything you want, and a lot of conventions will use meta data to store additional information, for things like transforms, lights, shaders and animation.
Data for Vertices, Corners, Faces, and Pixels are stored in named layer stacks. Each stack consists of a number of named layers. All layers in the stack have the same number of elements. Each layer describes one property of the primitive. Each layer can have multiple channels and each layer can store data of a different type.

HaX stores 3 kinds of nodes:

- Pixel data.
- Polygon geometry data.
- Meta data only.

Pixel Nodes stores pixels in a layer stack. A layer may store things like Albedo, Roughness, Reflectance, Light maps, Masks, Normal maps, and Displacement. Layers use the channels of the layers to store things like color. The length of the layer stack is determined by the type and dimensions stored in the array.

Geometry data is stored in 3 separate layer stacks for: vertex data, corner data and face data. The vertex data stores things like vertices, blend shapes, weight maps, and vertex colors. The first layer in a vertex stack has to be a 3 channel layer named "position" describing the base position of the vertices. The corner stack describes data per corner or edge of the polygons. It can be used for things like UV, normals, and adjacency. The first layer in a corner stack has to be a 1 channel integer layer named "index" describing the vertices used to form polygons. The last value in each polygon has a negative index to indicate the end of the polygon.

Example:

A quad and a tri with the vertex index:
```
		[0, 1, 2, 3] [1, 4, 2]
```
is stored:
```
		[0, 1, 2, -4, 1, 4, -3]
```

The face stack stores values per face. the length of the face stack has to match the number of negative values in the index layer in the corner stack. The face stack can be used to store things like material index.

# Storage
---------

All data is stored in little endian byte order with no padding. The layout mirrors the structs defined below with a few exceptions.
All names are stored as a 8bit unsigned integer indicating the length of the name followed by that many characters. Termination is not stored in the file.
Text strings stored in meta data are stored the same way as names, but instead of a 8bit unsigned integer a 32bit unsigned integer is used.

```c
#ifndef HAX_INCLUDE
#define HAX_INCLUDE

#define HXA_VERSION_API "0.3"
#define HXA_VERSION_FORMAT 3

typedef unsigned char hxa_uint8;
typedef signed int hxa_int32;
typedef unsigned int hxa_uint32;
typedef signed long long hxa_int64;
typedef unsigned long long hxa_uint64;

/*
HaX stores 3 types of nodes: 
*/

typedef enum{
	HXA_NT_META_ONLY = 0, // node only containing meta data.
	HXA_NT_GEOMETRY = 1, // node containing a geometry mesh, and meta data.
	HXA_NT_IMAGE = 2, // node containing a 1D, 2D, 3D, or Cube image, and meta data.
	HXA_NT_COUNT = 3 // the number of different nodes that can be stored in the file.
}HXANodeType;

/*
HaX stores layer data in the following types: 
*/
typedef enum{
	HXA_LDT_UINT8 = 0, /* 8bit unsigned integer, */
	HXA_LDT_INT32 = 1, /* 32bit signed integer */
	HXA_LDT_FLOAT = 2, /* 32bit IEEE 754 floating point value */
	HXA_LDT_DOUBLE = 3, /* 64bit IEEE 754 floating point value */
	HXA_LDT_COUNT = 4 /* number of types supported by layers */
}HXALayerDataType;
/*
Pixel data is arranged in the following configurations
*/
typedef enum{
	HXA_IT_CUBE_IMAGE = 0, /* 6 sided qube, in the order of: +x, -x, +y, -y, +z, -z. */
	HXA_IT_1D_IMAGE = 1, /* One dimentional pixel data. */
	HXA_IT_2D_IMAGE = 2, /* Two dimentional pixel data. */
	HXA_IT_3D_IMAGE = 3, /* Three dimentional pixel data. */
}HXAImageType;

typedef enum{
	HXA_MDT_INT64 = 0,
	HXA_MDT_DOUBLE = 1,
	HXA_MDT_NODE = 2,
	HXA_MDT_TEXT = 3,
	HXA_MDT_BINARY = 4,
	HXA_MDT_META = 5,
	HXA_MDT_COUNT = 6
}HXAMetaDataType;

#define HXA_NAME_MAX_LENGTH 256 //

typedef struct{
	char name[HXA_NAME_MAX_LENGTH]; // name of the meta data value.
	HXAMetaDataType type; // type of value. Stored in the file as a uint8.
	hxa_uint32 array_length; // how many values are stored / the length of the stored text string (excluding termination)
	union{
		hxa_uint64 *int64_value; // integer values
		double *double_value; // double values
		hxa_uint32 *node_value; // a reference to another node 
		char *text_value; // text string
		unsigned char *bin_value; // binary data string
		void *array_of_meta; // Meta structures
	}value;
}HXAMeta; /* meta data key/value store */


/* Layers are arrays of data used to store geometry and pixel data */

typedef struct{
	char name[HXA_NAME_MAX_LENGTH]; // name of the layer. List of predefined names for common usages like uv, reference, blendshapes, weights ...
	hxa_uint8 components; // 2 for uv, 3 for xyz or rgb, 4 for rgba;
	HXALayerDataType type; // Stored in the file as a uint8.
	union{
		hxa_uint8 *uint8_data;
		hxa_int32 *int32_data;
		float *float_data;
		double *double_data;
	}data;
}HXALayer;

/* Layers stacks are arrays of layers where all the layers have the same number of entries (polygons, edges, vertices or pixels) */

typedef struct{
	hxa_uint32 layer_count; /* the number of loayers in a stack. */
	HXALayer *layers; /* An array of layers. */
}HXALayerStack;

/* A file consists of an array of nodes, All noides have meta data. Geometry nodes have geometry, image nodes have pixels*/

typedef struct{
	HXANodeType type; // what type of node is this? Stored in the file as a uint8.
	hxa_uint32 meta_data_count; // how many meta data key/values are stored in the node
	HXAMeta *meta_data; // array of key/values
	union{
		struct{
			hxa_uint32 vertex_count; // number of vertices
			HXALayerStack vertex_stack; // stack of vertex arrays. the first layer is always the vertex positions
			hxa_uint32 edge_corner_count; // number of corners
			HXALayerStack corner_stack; // stack of corner arrays, the first layer is allways a reference array (see below)
			HXALayerStack edge_stack; // stack of edge arrays
			hxa_uint32 face_count; // number of polygons
			HXALayerStack face_stack; // stack of per polygon data.
		}geometry; 
		struct{
			HXAImageType type; // type of image 
			hxa_uint32 resolution[3]; // resolytion i X, Y and Z dimention;
			HXALayerStack image_stack; // the number of values in the stack is equal to the number of pixels depending on resolution
		}image; 
	}content;
}HXANode;

#define HAX_MAGIC_NUMBER (*(hxa_uint32)"HaX") // 4290632

typedef struct{
//	hxa_uint32 magic_number; The file begins with a file identifyer. it always has to be the 4 bytes "HxA", See definition of HAX_MAGIC_NUMBER. Since the magic number is always the same we dont store it in this structure even if it is always present in files.
	hxa_uint8 version;
	hxa_uint32 node_count; // number of nodes in the file
	HXANode *node_array; // array of nodes.
}HXAFile;
```

# Conventions
------------- 
Much of HxA's use is based on convention. HxA lets users store aritrary data in its structure that can be parsed but whos semantic meaning does not need to be understood.

A few conventions are hard, and some are soft. Hard convention that a user HAS to follow in order to produce a valid file. Hard conventions simplify parsing becaus the parser can make some assumptions. Soft convenbtions are basicly recomendations of how to store common data.

If you use HxA for something not coverd by the conventiosns but need a convention for your usecase. Please let us know so that we can add it!


```c
/* Hard conventions */
/* ---------------- */

#define HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_NAME "vertex"
#define HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_ID 0
#define HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_COMPONENTS 3
#define HXA_CONVENTION_HARD_BASE_CORNER_LAYER_NAME "reference"
#define HXA_CONVENTION_HARD_BASE_CORNER_LAYER_ID 0
#define HXA_CONVENTION_HARD_BASE_CORNER_LAYER_COMPONENTS 1
#define HXA_CONVENTION_HARD_BASE_CORNER_LAYER_TYPE HXA_LDT_INT32
#define HXA_CONVENTION_HARD_EDGE_NEIGHBOUR_LAYER_NAME "neighbour"
#define HXA_CONVENTION_HARD_EDGE_NEIGHBOUR_LAYER_TYPE HXA_LDT_INT32

/* Soft Conventions */
/* ---------------- */

/* geometry layers */

#define HXA_CONVENTION_SOFT_LAYER_SEQUENCE0 "sequence"
#define HXA_CONVENTION_SOFT_LAYER_NAME_UV0 "uv"
#define HXA_CONVENTION_SOFT_LAYER_NORMALS "normal"
#define HXA_CONVENTION_SOFT_LAYER_TANGENT "tangent"
#define HXA_CONVENTION_SOFT_LAYER_CREASES "creases"
#define HXA_CONVENTION_SOFT_LAYER_SELECTION "selection"
#define HXA_CONVENTION_SOFT_LAYER_SKIN_WEIGHT "skining_weight"
#define HXA_CONVENTION_SOFT_LAYER_SKIN_REFERENCE "skining_reference"
#define HXA_CONVENTION_SOFT_LAYER_SKIN_REFERENCE "skining_reference"
#define HXA_CONVENTION_SOFT_LAYER_BLENDSHAPE "blendshape"
#define HXA_CONVENTION_SOFT_LAYER_ADD_BLENDSHAPE "addblendshape"
#define HXA_CONVENTION_SOFT_LAYER_MATERIAL_ID "material"

/* Image layers */

#define HXA_CONVENTION_SOFT_ALBEDO "albedo"
#define HXA_CONVENTION_SOFT_LIGHT "light"
#define HXA_CONVENTION_SOFT_DISPLACEMENT "displacement"
#define HXA_CONVENTION_SOFT_DISTORTION "distortion"
#define HXA_CONVENTION_SOFT_AMBIENT_OCCLUSION "ambient_occlusion"

/* tags layers */

#define HXA_CONVENTION_SOFT_NAME "name"
#define HXA_CONVENTION_SOFT_TRANSFORM "transform"
```
