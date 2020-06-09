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

#include "PtGraphicsLibrary.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include <PtModelLibrary.h>

// GsGraphics
//#include "GsGraphics/GsAssetResourceManager.h"

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace PtGraphics;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

bool PtGraphicsLibrary::mIsInitialized = false;

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Main GsGraphics module initialization method
 ******************************************************************************/
bool PtGraphicsLibrary::initialize( const char* const pWorkingDirectory )
{
	if ( mIsInitialized )
	{
		return true;
	}

	bool statusOK = Pt::PtDataModelLibrary::initialize( pWorkingDirectory );
	if ( ! statusOK )
	{
		return false;
	}

	// Load plugins
	//GsAssetResourceManager::get().loadPlugins("");

	// Update flag
	mIsInitialized = true;

	return true;
}

/******************************************************************************
 * Main GsGraphics module finalization method
 ******************************************************************************/
void PtGraphicsLibrary::finalize()
{
	// Unload plugins
	//GsAssetResourceManager::get().unloadAll();

	// Update flag
	mIsInitialized = false;
}
