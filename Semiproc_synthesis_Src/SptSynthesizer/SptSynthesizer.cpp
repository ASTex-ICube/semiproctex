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

// Project
#include "SptHviewInterface.h"

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
:	mHviewInterface( nullptr )
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
,	mCorrectionGuidanceWeight( 0.f )
,	mCorrectionExemplarWeightDistance( 0.f )
,	mCorrectionGuidanceWeightDistance( 0.f )
,	mCorrectionLabelErrorAmount( 0.f )
// - semi-procedural
,	mSemiProcTexPPTBFThreshold( 0.f )
,	mSemiProcTexRelaxContraintMin( 0.f )
,	mSemiProcTexRelaxContraintMax( 0.f )
,	mSemiProcTexGuidanceWeight( 0.f )
,	mSemiProcTexDistancePower( 0.f )
,	mSemiProcTexInitializationError( 0.f )
,	mSemiProcTexNbLabels( 1 )
//	[PPTBF]
,	mPptbfShiftX( 0 )
,	mPptbfShiftY( 0 )
{
	// Hview interface
	mHviewInterface = new SptHviewInterface;
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
SptSynthesizer::~SptSynthesizer()
{
	// Hview interface
	delete mHviewInterface;
	mHviewInterface = nullptr;
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void SptSynthesizer::initialize()
{
	// Hview interface
	mHviewInterface->initialize();
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void SptSynthesizer::finalize()
{
	// Hview interface
	mHviewInterface->finalize();
}

/******************************************************************************
 * Load synthesis parameters
 *
 * @param pFilename
 *
 * @return error status
 ******************************************************************************/
int SptSynthesizer::loadParameters( const char* pFilename )
{
	// Open file
	std::ifstream semiProcTexConfigFile;
	std::string semiProcTexConfigFilename = std::string( pFilename );
	semiProcTexConfigFile.open( semiProcTexConfigFilename );
	if ( ! semiProcTexConfigFile.is_open() )
	{
		// Log info
		std::cout << "ERROR: file cannot be opened: " << std::string( semiProcTexConfigFilename ) << std::endl;
		
		// Handle error
		assert( false );
		return -1;
	}

	// Temp variables
	std::string lineData;
	std::string text;

	//----------------------------------------------
	// [EXEMPLAR]
	//----------------------------------------------

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

	//----------------------------------------------
	// [SYNTHESIS]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );
	
	// - outputSize
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mOutputWidth;
		ss >> mOutputHeight;
	}

	//----------------------------------------------
	// [PYRAMID]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );

	// - nbMipmapLevels
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mPyramidNbMipmapLevels;
	}

	// - pyramidMaxLevel
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mPyramidMaxLevel;
	}

	// - pyramidMinSize
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mPyramidMinSize;
	}

	// - nbPyramidLevels
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mPyramidNbLevels;
	}

	//----------------------------------------------
	// [BLOCK INITIALIZATION]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );

	// - blockGrid
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mblockInitGridWidth;
		ss >> mblockInitGridHeight;
	}

	// - blockSize
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mblockInitBlockWidth;
		ss >> mblockInitBlockHeight;
	}

	// - useSmartInitialization
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		int value;
		ss >> value;
		mblockInitUseSmartInitialization = value > 0 ? true : false;
	}

	// - smartInitNbPasses
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mblockInitSmartInitNbPasses;
	}

	//----------------------------------------------
	// [CORRECTION PASS]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );

	// - correctionNbPasses
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionNbPasses;
	}

	// - correctionSubPassBlockSize
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionSubPassBlockSize;
	}

	// - correctionNeighborhoodSize
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionNeighborhoodSize;
	}

	// - correctionNeighborSearchRadius
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionNeighborSearchRadius;
	}

	// - correctionNeighborSearchNbSamples
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionNeighborSearchNbSamples;
	}

	// - correctionNeighborSearchDepth
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionNeighborSearchDepth;
	}

	//----------------------------------------------
	// [MATERIAL]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );

	// - correctionWeightAlbedo
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionWeightAlbedo;
	}

	// - correctionWeightHeight
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionWeightHeight;
	}

	// - correctionWeightNormal
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionWeightNormal;
	}

	// - correctionWeightRoughness
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionWeightRoughness;
	}

	//----------------------------------------------
	// [LABEL MAP]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );
	
	// - useLabelMap
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		int value;
		ss >> value;
		mUseLabelMap = value > 0 ? true : false;
	}

	// - labelmapType
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mLabelmapType;
	}

	// - useLabelSampler
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		int value;
		ss >> value;
		mUseLabelSampler = value > 0 ? true : false;
	}

	// - labelSamplerAreaThreshold
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mLabelSamplerAreaThreshold;
	}

	//----------------------------------------------
	// [GUIDANCE]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );
	
	// - correctionGuidanceWeight
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionGuidanceWeight;
	}

	// - correctionExemplarWeightDistance
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionExemplarWeightDistance;
	}

	// - correctionGuidanceWeightDistance
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionGuidanceWeightDistance;
	}

	// - correctionLabelErrorAmount
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mCorrectionLabelErrorAmount;
	}

	//----------------------------------------------
	// [SEMI PROCEDURAL]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );

	// - PPTBFThreshold
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mSemiProcTexPPTBFThreshold;
	}

	// - relaxContraints
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mSemiProcTexRelaxContraintMin;
		ss >> mSemiProcTexRelaxContraintMax;
	}

	// - guidanceWeight
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mSemiProcTexGuidanceWeight;
	}

	// - distancePower
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mSemiProcTexDistancePower;
	}

	// - initializationError
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mSemiProcTexInitializationError;
	}

	// - nbLabels
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mSemiProcTexNbLabels;
	}

	//----------------------------------------------
	// [PPTBF]
	//----------------------------------------------

	std::getline( semiProcTexConfigFile, lineData );

	// - shift
	std::getline( semiProcTexConfigFile, lineData );
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> mPptbfShiftX;
		ss >> mPptbfShiftY;
	}

	// Close file
	semiProcTexConfigFile.close();

	//--------------------------------------
	// TODO: adapt semi-procedural texture synthesis parameters to hview api ones
	//--------------------------------------

	// TODO
	// ...

	//--------------------------------------
	// TODO: load exemplar data
	//--------------------------------------

	// Load exemplar data
	// - color
	const std::string exemplarFilename = "";
	loadExemplar( exemplarFilename.c_str() );
	// - structure
	const std::string exemplarStructureFilename = "";
	loadExemplarStructureMap( exemplarStructureFilename.c_str() );
	// - distance
	const std::string exemplarDistanceFilename = "";
	loadExemplarDistanceMap( exemplarDistanceFilename.c_str() );
	// - labels
	const std::string exemplarLabelFilename = "";
	loadExemplarLabelMap( exemplarLabelFilename.c_str() );

	//--------------------------------------
	// TODO: load guidance data
	//--------------------------------------

	// Load guidance data
	// - PPTBF
	const std::string guidancePPTBFFilename = "";
	loadGuidancePPTBF( guidancePPTBFFilename.c_str() );
	// - mask
	const std::string guidanceMaskFilename = "";
	loadGuidanceMask( guidanceMaskFilename.c_str() );
	// - distance
	const std::string guidanceDistanceFilename = "";
	loadGuidanceDistanceMap( guidanceDistanceFilename.c_str() );
	// - labels
	const std::string guidanceLabelFilename = "";
	loadGuidanceLabelMap( guidanceLabelFilename.c_str() );

	// Exit code
	return 0;
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptSynthesizer::execute()
{
	// Delegate synthesis to hview interface
	mHviewInterface->execute();
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
 * Load exemplar (color texture)
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadExemplar(const char* pFilename)
{
	// Delegate to hview interface
	mHviewInterface->loadExemplar( pFilename );
}

/******************************************************************************
 * Load structure map (binary)
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadExemplarStructureMap( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadExemplarStructureMap( pFilename );
}

/******************************************************************************
 * Load distance map
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadExemplarDistanceMap( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadExemplarDistanceMap( pFilename );
}

/******************************************************************************
 * Load label map
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadExemplarLabelMap( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadExemplarLabelMap( pFilename );
}

/******************************************************************************
 * Load guidance PPTBF
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadGuidancePPTBF( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadGuidancePPTBF( pFilename );
}

/******************************************************************************
 * Load guidance mask
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadGuidanceMask( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadGuidanceMask( pFilename) ;
}

/******************************************************************************
 * Load guidance distance map
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadGuidanceDistanceMap( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadGuidanceDistanceMap( pFilename );
}

/******************************************************************************
 * Load guidance label map
 *
 * @param pFilename
 ******************************************************************************/
void SptSynthesizer::loadGuidanceLabelMap( const char* pFilename )
{
	// Delegate to hview interface
	mHviewInterface->loadGuidanceLabelMap( pFilename );
}
