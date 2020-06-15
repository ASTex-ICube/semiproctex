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
// hview synthesis parameters
:	mName()
,	mSTOPATLEVEL( 0 )
,	mPosx( 0 )
,	mPosy( 0 )
//,	mExample()
//,	mExdist()
,	mWeight( 0. )
,	mPowr( 0. )
,	mIndweight( 0.f )
,	mNeighbor( 0 )
,	mAtlevel( 0 )
,	mBsize( 0 )
,	mERR( 0.f )
//,	mMask()
//,	mGuidance()
//,	mIndex()
//,	mRes()
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
	// User customizable parameter(s)
	const int nbCorrectionPasses = 2;
	const bool useSmartInitialization = false;

	// Parameter conversion:
	//
	// GUIDE 0.9       <=> guidanceWeight
	// STRENGTH 0.5
	// INITLEVEL 2     <=> pyramidMaxLevel
	// BLOCSIZE 64     <=> mSmartInitNbPasses (pb: can be any value!)
	// INITERR 100     <=> initializationError (per-pixel threshold error in a block to keep it or not, % in [0;1])
	// INDEXWEIGHT 0   <=> correctionLabelErrorAmount (3x²)
	
	// Call hview texture synthesis api
	mRes.execute_semiProceduralTextureSynthesis(
		mName.c_str(),
		mSTOPATLEVEL,  // should be 0 (for debug purpose only: can stop synthesis a given coarse level)
		mPosx, mPosy,  // translation used to be able to tie tiles for large textrues
		mExample, mExdist, // exemplar
		mWeight, // weight color vs distance when searching for neighborhood candidates (i.e. new pixels)
		mPowr, mIndweight, mNeighbor,
		mAtlevel, mBsize, mERR,
		mMask, mGuidance,
		mIndex,
		// user customizable parameter(s)
		nbCorrectionPasses,
		useSmartInitialization
	);
}

/******************************************************************************
 *  * Load exemplar (color texture)
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadExemplar( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container: mExample
	// TODO
	// ...
}

/******************************************************************************
 * Load structure map (binary)
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadExemplarStructureMap( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container
	// TODO
	// ...
}

/******************************************************************************
 * Load distance map
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadExemplarDistanceMap( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container: mExdist
	// TODO
	// ...
}

/******************************************************************************
 * Load label map
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadExemplarLabelMap( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container
	// TODO
	// ...
}

/******************************************************************************
 * Load guidance PPTBF
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadGuidancePPTBF( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container
	// TODO
	// ...
}

/******************************************************************************
 * Load guidance mask
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadGuidanceMask( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container
	// TODO
	// ...
}

/******************************************************************************
 * Load guidance distance map
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadGuidanceDistanceMap( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container
	// TODO
	// ...
}

/******************************************************************************
 * Load guidance label map
 *
 * @param pFilename
 ******************************************************************************/
void SptHviewInterface::loadGuidanceLabelMap( const char* pFilename )
{
	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* pData = nullptr;
	SptImageHelper::loadImage( pFilename, width, height, nrChannels, pData, desired_channels );

	// Store data in hview container
	// TODO
	// ...
}
