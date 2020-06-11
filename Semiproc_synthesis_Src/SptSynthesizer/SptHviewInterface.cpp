/*
 * Publication: Semi-Procedural Textures Using Point Process Texture Basis Functions
 *              Computer Graphics Forum (EGSR 2020 special issue)
 * Authors: P. Guehl , R. AllEgre , J.-M. Dischler, B. Benes , and E. Galin
 *
 * Code author: Pascal Guehl
 */

/** 
 * @version 1.0
 */

#include "SptHviewInterface.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "SptImageHelper.h" 

// System
#include <cassert>

// STL
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>

// HVIEW
#include "hvPicture.h"

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Spt;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Constructor
 ******************************************************************************/
SptHviewInterface::SptHviewInterface()
{
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
SptHviewInterface::~SptHviewInterface()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void SptHviewInterface::initialize()
{
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void SptHviewInterface::finalize()
{
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptHviewInterface::execute()
{
	// Call hview texture synthesis api
	mRes.pctswdistguidanceV2(
		mName.c_str(),
		mSTOPATLEVEL,
		mPosx, mPosy,
		mExample, mExdist,
		mWeight, // weight color vs distance
		mPowr, mIndweight, mNeighbor,mAtlevel, mBsize, mERR,
		mMask, mGuidance,
		mIndex
	);
}

/******************************************************************************
 * Load structure map (binary)
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadStructureMap( const char* pFilename )
{
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );
}

/******************************************************************************
 * Load distance map
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadDistanceMap( const char* pFilename )
{
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );
}

/******************************************************************************
 * Load label map
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadLabelMap( const char* pFilename )
{
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );
}
