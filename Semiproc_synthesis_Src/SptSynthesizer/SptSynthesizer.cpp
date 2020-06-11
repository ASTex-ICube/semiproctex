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
#include <fstream>
#include <sstream>

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
// - material
,	mCorrectionWeightAlbedo( 1.f )
,	mCorrectionWeightHeight( 1.f )
,	mCorrectionWeightNormal( 1.f )
,	mCorrectionWeightRoughness( 1.f )
// - label map
,	mUseLabelMap( false )
,	mLabelmapType( 0 )
,	mUseLabelSampler( false )
,	mLabelSamplerAreaThreshold( 0.f )
// - guidance
,	mCorrectionGuidanceWeight( 1.f )
,	mCorrectionExemplarWeightDistance( 1.f )
,	mCorrectionGuidanceWeightDistance( 1.f )
,	mCorrectionLabelErrorAmount( 1.f )
// - semi-procedural
,	mSemiProcTexPPTBFThreshold( 1.f )
,	mSemiProcTexRelaxContraints( 1.f )
,	mSemiProcTexGuidanceWeight( 1.f )
,	mSemiProcTexDistancePower( 1.f )
,	mSemiProcTexInitializationError( 1.f )
,	mSemiProcTexNbLabels( 1 )
//	[PPTBF]
,	mPtbfShiftX( 0 )
,	mPptbfShiftY( 0 )
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
	// Open file
	std::ifstream semiProcTexConfigFile;
	std::string semiProcTexConfigFilename = std::string( pFilename );
	semiProcTexConfigFile.open( semiProcTexConfigFilename );
	if ( ! semiProcTexConfigFile.is_open() )
	{
		// Log info
		std::cout << "ERROR: file cannot be opened: " << std::string( semiProcTexConfigFilename ) << std::endl;
		
		// Handle error (TODO)
		assert( false );
		//return -1;
	}

	// Temp variables
	std::string lineData;
	std::string text;

	// [EXEMPLAR]
	std::getline( semiProcTexConfigFile, lineData );

	// - name
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mExemplarName;
	}

	// - exemplarSize
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mExemplarWidth;
		ss >> mExemplarHeight;
	}

	// TODO
	// - continue reading parameter file
	// ...

	// Close file
	semiProcTexConfigFile.close();
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptSynthesizer::execute()
{
	// Delegate synthesis to hview interface
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
