#include "hxa.h"

/*	HxA Universal Pluging Interface (UPI)
	-------------------------------------

This is the HxA universal plugin interface. The idea is to use the simple HxA data structure to create a plugin interface for asset 
manipulation that is so simple that it can be implemented in any DCC application. It lets you wrap code that reads and writes HxA 
structures in to dynamicly loadable libraries. You can specify what parameters the code takes, so that an applictaion can build an 
interface that acceeses the functionality. This makes it easy to build a node, stack or filter based tool that uses any HxA 
processing algorithm by dynamicly loading them. It also means that a HxA plugin wraped as a UPI can be loaded in any applictaion that 
suppiorts HxA plugins. One Plugin AIP, multiple applications? Hell Yeah!!

*/

/* qualifyer needed to expose a function to a library loader*/
#ifdef _WIN32
#define HxALibExport _declspec (dllexport)
#else
#define IMAGINE_ATOMIC_EMULATION
#define ILibExport
#endif

/* Parameter types
--------------------*/

typedef enum{
	HXA_UPI_IPT_BOOLEAN, /* checkbox */
	HXA_UPI_IPT_SIGNED_INTEGER, /* int */
	HXA_UPI_IPT_UNSIGNED_INTEGER, /* insigned integer */
	HXA_UPI_IPT_DOUBLE_BOUND, /* double 0 - 1.0  (slider)*/
	HXA_UPI_IPT_DOUBLE_UNBOUND, /* double */
	HXA_UPI_IPT_POS_2D, /* 2D position */
	HXA_UPI_IPT_POS_3D, /* 3D position */
	HXA_UPI_IPT_VECTOR_2D, /* 2D vector */
	HXA_UPI_IPT_VECTOR_3D, /* 3D vector */
	HXA_UPI_IPT_MATRIX, /* 4x4 colum major transform Matrix*/
	HXA_UPI_IPT_COLOR, /* RGB value */
	HXA_UPI_IPT_TEXT, /* text input. Can be NULL */
	HXA_UPI_IPT_PATH_READ, /* File path for reading. Can be NULL */
	HXA_UPI_IPT_PATH_WRITE, /* File path for writing. Can be NULL */
	HXA_UPI_IPT_SELECT, /* multi option select */
	HXA_UPI_IPT_HXA_READ, /* A HXA structure to be read only. Can be NULL */
	HXA_UPI_IPT_HXA_CONSUME, /* A HXA structure, that will be read and freed/returned by the plugin. Can be NULL */
	HXA_UPI_IPT_COUNT,
}HXAUPIInterfaceParamType;


/* Parameters descriptions
---------------------------*/

typedef struct{
	HXAUPIInterfaceParamType type; // what type of node is this? Stored in the file as a uint8.
	char *name; // What is the parameter named. keep it short so it fits in a UI.
	char *description; // Longer desriptive text. Used for things like tooltips and generated documentation. 
	/* the union below describes either the value of a parameter provided by the user at runtime, or the default value provided by the plugin at initialization. */
	union{
		int boolean; // checkbox
		int signed_integer; // signed integer 
		unsigned int unsigned_integer; // unsigned integer
		double double_bound; // double value raning from 0 - 1. Can be shown as a slider. may be expresed as % in the UI.
		double double_value; // double value with infinit range. Can not be a NaN!
		double point_2d[2]; // 2D position. can be used for positions on a image.
		double point_3d[3]; // 3D position. Can be a 3D mainipulator.
		double vector_2d[2]; // 2D vector. if placed in a 2D UI, and following a HXA_IPT_POS_2D, start the vector at the previous position.
		double vector_3d[3]; // 3D vector. if placed in a 3D UI, and following a HXA_IPT_POS_3D or HXA_IPT_MATRIX, start the vector at the previous position.
		double matrix[16]; // 3D transform. includes Translation, Rotation and Scale.
		double color[3]; // RGB value. UIs are expected to have some sort of color picker UI.
		char *text; // Null is legal
		char *path_read; // Null is legal
		char *path_write; // Null is legal
		struct{
			char **option_names; // an array of option names.
			unsigned int option_count; // the number of options available. Must be at least 1.
			unsigned int selected; // currently selected. Must be no more then one less then option_count.
		}select;
		HXAFile *hxa; // A pointer to a HxA structure. This structure is write protected and should not be modified or returned !!
	}content;
}HXAUPIInterfaceParam;

/* Plugin API 
---------------*/

/* This is the initialization function that a HxA UPI plugin needs to implement. An applictaion that wants
 to support HxA plugins will look for dunamicly loaded libraries that have this function implemented. 
The functions main first parameter is a function pointer to a function used by the plugin to register
all things needed to describe a plugins interface and execution. Se exampel plugin below.*/

typedef struct{
	HXAUPIInterfaceParam *params; /* parameters used by the plugin. */
	unsigned int param_count; /* numer of params taken by the plugin. */
	const char *name; /* name of the plugin. */
	const char *descrtiption;  /* text string describing the functionality of the plugin. */
	int has_output; /* does the plugin return a HxAFile Struct pointer. */
	void *(*instance_create_func)(HXAUPIInterfaceParam *params, void *user); /* creates an instance of the plugin. May be NULL. */
	void (*instance_destroy_func)(HXAUPIInterfaceParam *params, void *instance, void *user); /* destroys an instance of the plugin. May be NULL if instance_create_func is also NULL. */
	int (*instance_update_func)(HXAUPIInterfaceParam *params, void *instance, void *user); /*tests if the instance needs updating. May be NULL. */
	HXAFile *(*execute_func)(HXAUPIInterfaceParam *params, void *instance, void *user); /**/
	void *user; /* user pointer that will be given to all above function pointers when called. */
	void *library; /* internal handle for the loaded library, so that it can be unloaded. */
}HxAUPIPlugin;

extern void hxa_upi_plugin_library_initialize(void (*register_func)(HxAUPIPlugin *plugin));


/* Reference host API 
-----------------------*/

extern HxAUPIPlugin *hxa_upi_host_load_library(char *path, unsigned int *plugin_count); /* Loads a single library file. */
extern HxAUPIPlugin *hxa_upi_host_load_directory(char *path, unsigned int *plugin_count); /* loads all libraries in a specific path (like a plugin directory).*/
extern void hxa_upi_host_unload_library(char *path, HxAUPIPlugin *plugins, unsigned int plugin_count); /* unloads all libraries and frees memory */

