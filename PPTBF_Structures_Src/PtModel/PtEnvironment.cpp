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

#include "PtEnvironment.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Pt;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

std::string PtEnvironment::mProgramPath = PT_PROJECT_PATH + std::string( "/" );
std::string PtEnvironment::mWorkingDirectory = PT_PROJECT_PATH + std::string( "/Dev/Install/PPTBFProject/bin/" );
std::string PtEnvironment::mSettingDirectory = PT_PROJECT_PATH + std::string( "/Dev/Install/PPTBFProject/bin/Settings/" );
std::string PtEnvironment::mDataPath = PT_DATA_PATH + std::string( "/" );
std::string PtEnvironment::mShaderPath = PT_SHADER_PATH + std::string( "/" );
std::string PtEnvironment::mImagePath = PT_IMAGE_PATH + std::string( "/" );

/******************************************************************************
***************************** METHOD DEFINITION ******************************
******************************************************************************/
