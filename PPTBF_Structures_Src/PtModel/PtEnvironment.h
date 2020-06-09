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

#ifndef _PT_ENVIRONMENT_H_
#define _PT_ENVIRONMENT_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtConfig.h"

// STL
#include <string>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

#define _OS_USE_VIEWER_REFRESH_
//#define _OS_USE_DEBUG_GL_
//#define _OS_USE_HOME_PATH_

#define _OS_USE_SHADER_DEBUG_

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

namespace Pt
{

class PTMODEL_EXPORT PtEnvironment
{
public:
	static std::string mProgramPath;
	static std::string mWorkingDirectory;
	static std::string mSettingDirectory;
	static std::string mDataPath;
	static std::string mShaderPath;
	static std::string mImagePath;
};

}

/******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

#endif // !_PT_ENVIRONMENT_H_
