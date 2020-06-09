/*
* Publication: Semi-Procedural Textures Using Point Process Texture Basis Functions
* Authors: anonymous
*
* Code author: Pascal Guehl
*
* anonymous
* anonymous
*/

/**
* @version 1.0
*/

#ifndef _PT_GRAPHICS_CONFIG_H_
#define _PT_GRAPHICS_CONFIG_H_

/******************************************************************************
******************************* INCLUDE SECTION ******************************
******************************************************************************/

/******************************************************************************
************************* DEFINE AND CONSTANT SECTION ************************
******************************************************************************/

//*** GsGraphics Library 

// Static or dynamic link configuration
#ifdef WIN32
#	ifdef PTGRAPHICS_MAKELIB	// Create a static library.
#		define PTGRAPHICS_EXPORT
#		define PTGRAPHICS_TEMPLATE_EXPORT
#	elif defined PTGRAPHICS_USELIB	// Use a static library.
#		define PTGRAPHICS_EXPORT
#		define PTGRAPHICS_TEMPLATE_EXPORT
#	elif defined PTGRAPHICS_MAKEDLL	// Create a DLL library.
#		define PTGRAPHICS_EXPORT	__declspec(dllexport)
#		define PTGRAPHICS_TEMPLATE_EXPORT
#	else	// Use DLL library
#		define PTGRAPHICS_EXPORT	__declspec(dllimport)
#		define PTGRAPHICS_TEMPLATE_EXPORT	extern
#	endif
#else
#	 if defined(PTGRAPHICS_MAKEDLL) || defined(PTGRAPHICS_MAKELIB)
#		define PTGRAPHICS_EXPORT
#		define PTGRAPHICS_TEMPLATE_EXPORT
#	else
#		define PTGRAPHICS_EXPORT
#		define PTGRAPHICS_TEMPLATE_EXPORT	extern
#	endif
#endif

// ---------------- GLM library Management ----------------

///**
// * To remove warnings at compilation with GLM deprecated functions
// */
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS

//#define _OS_USE_HOME_PATH_

#endif // !_PT_GRAPHICS_CONFIG_H_
