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


typedef enum{
	HXA_IPT_BOOLEAN, /* checkbox */
	HXA_IPT_SIGNED_INTEGER, /* int */
	HXA_IPT_UNSIGNED_INTEGER, /* insigned integer */
	HXA_IPT_DOUBLE_BOUND, /* double 0 - 1.0  (slider)*/
	HXA_IPT_DOUBLE_UNBOUND, /* double */
	HXA_IPT_POS_2D, /* 2D position */
	HXA_IPT_POS_3D, /* 3D position */
	HXA_IPT_VECTOR_2D, /* 2D vector */
	HXA_IPT_VECTOR_3D, /* 3D vector */
	HXA_IPT_MATRIX, /* 4x4 colum major transform Matrix*/
	HXA_IPT_COLOR, /* RGB value */
	HXA_IPT_TEXT, /* text input. Can be NULL */
	HXA_IPT_PATH_READ, /* File path for reading. Can be NULL */
	HXA_IPT_PATH_WRITE, /* File path for writing. Can be NULL */
	HXA_IPT_SELECT, /* multi option select */
	HXA_IPT_HXA, /* A HXA structure. Can be NULL */
	HXA_IPT_COUNT,
}HXAInterfaceParamType;

typedef struct{
	HXAInterfaceParamType type; // what type of node is this? Stored in the file as a uint8.
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
}HXAInterfaceParam;

/* This is the initialization function that a HxA plugin needs to implement. An applictaion that wants
 to support HxA plugins will look for dunamicly loaded libraries that have this function implemented. 
The functions main first parameter is a function pointer to a function used by the plugin to register
all things needed to describe a plugins interface and execution. Se exampel plugin below.*/

extern void hxa_plugin_library_initialize(void (*register_func)(HXAInterfaceParam *params, unsigned int param_count, char *name, char descrtiption, int has_output, HXAFile *(*execute_func)(HXAInterfaceParam *params, void *user), void *user));



// Example:

// Function that implements the plugin.

HXAFile *hxa_distort_execute_func(HXAInterfaceParam *params, void *user)
{
	// ... Code here that does what ever the plugin does. Reads inpuits from "params".
}

// Initialization funicon of a plugin. This library registers one plugin, but a library can contain many plugins.

void hxa_plugin_library_initialize(void (*register_func)(HXAInterfaceParam *params, unsigned int param_count, char *name, char descrtiption, HXAFile *(*execute_func)(HXAInterfaceParam *params, void *user), void *user))
{
	HXAInterfaceParam *params;
	params = malloc((sizeof *params) * 5);

	params[0].name = "Input Geometry"; // name of the parameter. Expected to show up in the interface
	params[0].description = "HxA structure that you want to distort"; // Longer description of the param, used for things luike tool tips.
	params[0].type = HXA_IPT_HXA; // Parameter type, in this case a HxA Structure.  
	params[0].content.hxa = NULL; // The default value. Values of type HxA is always NULL.

	params[1].name = "Distortion"; // name of the parameter. Expected to show up in the interface.
	params[1].description = "Amount of distortion"; // Longer description of the param, used for things luike tool tips.
	params[1].type = HXA_IPT_DOUBLE_UNBOUND; // Parameter type, double value. Expected to be a type-in value in an interface 
	params[1].content.double_value = 1.0; // Default value

	params[2].name = "Scale"; // name of the parameter. Expected to show up in the interface.
	params[2].description = "Scale of distortion"; // Longer description of the param, used for things luike tool tips.
	params[2].type = HXA_IPT_DOUBLE_UNBOUND; // Parameter type, double value. Expected to be a type-in value in an interface 
	params[2].content.double_value = 1.0; // Default value
	
	params[3].name = "Noise type"; // name of the parameter. Expected to show up in the interface.
	params[3].description = "The noise function used to distort the geometry"; // Longer description of the param, used for things luike tool tips.
	params[3].type = HXA_IPT_SELECT; // Selection of one of several modes. Expected to be apull down mnu or similar in a UI.
	params[3].content.select.option_names = malloc((sizeof *params[2].select.option_names) * 3);
	params[3].content.select.option_names[0] = "White noise"; // The names of the options you can select
	params[3].content.select.option_names[1] = "Perlin noise";
	params[3].content.select.option_names[2] = "Sin Wave";
	params[3].content.select.option_count = 3; // The number of options available to select 
	params[3].content.select.selected = 1; // Default mode set to Perlin

	params[4].name = "Offset"; // name of the parameter. Expected to show up in the interface.
	params[4].description = "Offset lets you move the position of the noise";
	params[4].type = HXA_IPT_POS_3D; // 3D position. 3 values and/or a 3D manipulater UI.
	params[4].content.point_3d[0] = 0.0; // Default value X
	params[4].content.point_3d[1] = 0.0; // Default value Y
	params[4].content.point_3d[2] = 0.0; // Default value Z

	register_func(params, 5,  // The five param descriptions.
				"Noise distortion", // name of the effect
				"This node uses a 3D noise function to distort a 3D model.",  // help text describing the plugin
				TRUE, // This plugin returns a HxA structure. Set to FALSE for plugins that output their processing by other means. (Like savers that save a file format)
				hxa_distort_execute_func, NULL); // the fucntion that performs the plugins work.
}
