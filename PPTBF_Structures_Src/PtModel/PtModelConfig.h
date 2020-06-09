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
 
#ifndef _PT_MODEL_CONFIG_H_
#define _PT_MODEL_CONFIG_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

//*** GsGraphics Library 

// Static or dynamic link configuration
#ifdef WIN32
#	ifdef PTMODEL_MAKELIB	// Create a static library.
#		define PTMODEL_EXPORT
#		define PTMODEL_TEMPLATE_EXPORT
#	elif defined PTMODEL_USELIB	// Use a static library.
#		define PTMODEL_EXPORT
#		define PTMODEL_TEMPLATE_EXPORT
#	elif defined PTMODEL_MAKEDLL	// Create a DLL library.
#		define PTMODEL_EXPORT	__declspec(dllexport)
#		define PTMODEL_TEMPLATE_EXPORT
#	else	// Use DLL library
#		define PTMODEL_EXPORT	__declspec(dllimport)
#		define PTMODEL_TEMPLATE_EXPORT	extern
#	endif
#else
#	 if defined(PTMODEL_MAKEDLL) || defined(PTMODEL_MAKELIB)
#		define PTMODEL_EXPORT
#		define PTMODEL_TEMPLATE_EXPORT
#	else
#		define PTMODEL_EXPORT
#		define PTMODEL_TEMPLATE_EXPORT	extern
#	endif
#endif

// ---------------- GLM library Management ----------------

///**
// * To remove warnings at compilation with GLM deprecated functions
// */
//#define GLM_FORCE_RADIANS

#endif // !_PT_MODEL_CONFIG_H_
