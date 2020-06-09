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

#include "PtModelLibrary.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtEnvironment.h"

// STL
#include <iostream>

// System
#include <cassert>

// GsGraphics
//#include "GsGraphics/GsAssetResourceManager.h"

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

bool PtDataModelLibrary::mIsInitialized = false;

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Main GsGraphics module initialization method
 ******************************************************************************/
bool PtDataModelLibrary::initialize( const char* const pWorkingDirectory )
{
	if ( mIsInitialized )
	{
		return true;
	}

	assert( pWorkingDirectory != nullptr );
	if ( pWorkingDirectory == nullptr )
	{
		return false;
	}

	std::string workingDirectory = std::string( pWorkingDirectory );
	const size_t directoryIndex = workingDirectory.find_last_of( "/\\" );
	workingDirectory = workingDirectory.substr( 0, directoryIndex );

	PtEnvironment::mWorkingDirectory = workingDirectory;
	PtEnvironment::mSettingDirectory = PtEnvironment::mWorkingDirectory + std::string( "/" ) + std::string( "Settings" );
	PtEnvironment::mDataPath = PtEnvironment::mWorkingDirectory + std::string( "/" ) + std::string( "Data" );
	PtEnvironment::mShaderPath = PtEnvironment::mDataPath + std::string( "/" ) + std::string( "Shaders/PPTBF" );
	PtEnvironment::mImagePath = PtEnvironment::mDataPath + std::string( "/" ) + std::string( "Images" );
	
	// Load plugins
	//GsAssetResourceManager::get().loadPlugins("");

	// Update flag
	mIsInitialized = true;

	return true;
}

/******************************************************************************
 * Main GsGraphics module finalization method
 ******************************************************************************/
void PtDataModelLibrary::finalize()
{
	// Unload plugins
	//GsAssetResourceManager::get().unloadAll();

	// Update flag
	mIsInitialized = false;
}
