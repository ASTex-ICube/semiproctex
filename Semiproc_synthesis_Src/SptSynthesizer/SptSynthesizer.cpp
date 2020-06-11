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

#include "SptSynthesizer.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// System
#include <cassert>

// STL
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>

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
SptSynthesizer::SptSynthesizer()
:	mHviewInterface()
// - exemplar
,	mExemplarName()
,	mExemplarWidth( 0 )
,	mExemplarHeight( 0 )
// - output
,	mOutputWidth( 0 )
,	mOutputHeight( 0 )
// - pyramid
,	mPyramidNbMipmapLevels( 0 )
,	mPyramidMaxLevel( 0 )
,	mPyramidMinSize( 0 )
,	mPyramidNbLevels( 0 )
// - block initialization
,	mblockInitGridWidth( 0 )
,	mblockInitGridHeight( 0 )
,	mblockInitBlockWidth( 0 )
,	mblockInitBlockHeight( 0 )
,	mblockInitUseSmartInitialization( false )
,	mblockInitSmartInitNbPasses( 0 )
// - correction pass
,	mCorrectionNbPasses( 0 )
,	mCorrectionSubPassBlockSize( 0 )
,	mCorrectionNeighborhoodSize( 0 )
,	mCorrectionNeighborSearchRadius( 0 )
,	mCorrectionNeighborSearchNbSamples( 0 )
,	mCorrectionNeighborSearchDepth( 0 )
// TODO: continue...
{
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
SptSynthesizer::~SptSynthesizer()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void SptSynthesizer::initialize()
{
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void SptSynthesizer::finalize()
{
}

/******************************************************************************
 * Load synthesis parameters
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadParameters( const char* pFilename )
{
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptSynthesizer::execute()
{
	mHviewInterface.execute();
}

/******************************************************************************
 * Save/export synthesis results
 ******************************************************************************/
void SptSynthesizer::saveResults()
{
}

/******************************************************************************
 * Initialization stage
 *  - strategies/policies to choose blocks at initialization
 ******************************************************************************/
void SptSynthesizer::smartInitialization()
{
}

/******************************************************************************
 * Correction pass
 ******************************************************************************/
void SptSynthesizer::correction()
{
}

/******************************************************************************
 * Upsampling pass
 ******************************************************************************/
void SptSynthesizer::upsampling()
{
}

/******************************************************************************
 * Load structure map (binary)
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadStructureMap( const char* pFilename )
{
}

/******************************************************************************
 * Load distance map
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadDistanceMap( const char* pFilename )
{
}

/******************************************************************************
 * Load label map
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadLabelMap( const char* pFilename )
{
}
