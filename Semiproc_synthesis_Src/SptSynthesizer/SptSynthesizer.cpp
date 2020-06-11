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
	//synthesisParameterFile << synthesizer->useLabelMap() ? 1 : 0;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	bool useLabelMap = false;
	{
		std::stringstream ss( lineData );
		ss >> text;
		int value;
		ss >> value;
		useLabelMap = value > 0 ? true : false;
	}

	// - labelmapType
	//int labelMapType = 0;
	////if ( synthesizer->useUniqueLabelMap() )
	////if ( synthesizer->getLabelMapType() == PcGraphicsSynthesizer::LabelMapType::eUnique )
	////{
	////	labelMapType = static_cast< int >( PcGraphicsSynthesizer::LabelMapType::eUnique );
	////}
	//////else if ( synthesizer->useRandomLabelMap() )
	////else if ( synthesizer->getLabelMapType() == PcGraphicsSynthesizer::LabelMapType::eRandom )
	////{
	////	labelMapType = static_cast< int >( PcGraphicsSynthesizer::LabelMapType::eRandom );
	////}
	////else
	////{
	////	labelMapType = static_cast< int >( PcGraphicsSynthesizer::LabelMapType::eClassification );
	////}
	//labelMapType = static_cast< int >( synthesizer->getLabelMapType() );
	//synthesisParameterFile << labelMapType;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	int labelmapType = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> labelmapType;
	}

	// - useLabelSampler
	//synthesisParameterFile << synthesizer->mUseLabelSampler ? 1 : 0;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	bool useLabelSampler = false;
	{
		std::stringstream ss( lineData );
		ss >> text;
		int value;
		ss >> value;
		useLabelSampler = value > 0 ? true : false;
	}

	// - labelSamplerAreaThreshold
	//synthesisParameterFile << synthesizer->mLabelSamplerThreshold;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float labelSamplerAreaThreshold = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> labelSamplerAreaThreshold;
	}

	//----------------------------------------------

	//synthesisParameterFile << "[GUIDANCE]" << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	
	//synthesisParameterFile << "correctionGuidanceWeight ";
	//synthesisParameterFile << synthesizer->getLabelWeight();
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float correctionGuidanceWeight = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> correctionGuidanceWeight;
	}

	//synthesisParameterFile << "correctionExemplarWeightDistance ";
	//synthesisParameterFile << synthesizer->getDistanceWeight();
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float correctionExemplarWeightDistance = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> correctionExemplarWeightDistance;
	}

	//synthesisParameterFile << "correctionGuidanceWeightDistance ";
	//synthesisParameterFile << synthesizer->getGuidanceDistanceWeight();
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float correctionGuidanceWeightDistance = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> correctionGuidanceWeightDistance;
	}

	//synthesisParameterFile << "correctionLabelErrorAmount ";
	//synthesisParameterFile << synthesizer->getLabelErrorAmount();
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float correctionLabelErrorAmount = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> correctionLabelErrorAmount;
	}

	//// ----------------------------------------------------------------------------------------
	//
	////synthesisParameterFile << "GUIDE ";
	////synthesisParameterFile << GUIDE;
	////synthesisParameterFile << std::endl;

	////synthesisParameterFile << "STRENGTH ";
	////synthesisParameterFile << STRENGTH;
	////synthesisParameterFile << std::endl;
	////synthesisParameterFile << "INITLEVEL ";
	////synthesisParameterFile << INITLEVEL;
	////synthesisParameterFile << std::endl;
	////synthesisParameterFile << "BLOCSIZE ";
	////synthesisParameterFile << BLOCSIZE;
	////synthesisParameterFile << std::endl;
	////synthesisParameterFile << "INITERR ";
	////synthesisParameterFile << INITERR;
	////synthesisParameterFile << std::endl;

	////synthesisParameterFile << "INDEXWEIGHT ";
	////synthesisParameterFile << synthesizer->getLabelWeight(); // is that the right one?
	////synthesisParameterFile << std::endl;

	//----------------------------------------------

	//synthesisParameterFile << "[SEMI PROCEDURAL]" << std::endl;
	std::getline( semiProcTexConfigFile, lineData );

	//synthesisParameterFile << "PPTBFThreshold ";
	//synthesisParameterFile << synthesizer->getTextureExtrapolator().mThreshold;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float PPTBFThreshold = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> PPTBFThreshold;
	}

	//synthesisParameterFile << "relaxContraints ";
	//synthesisParameterFile << synthesizer->getTextureExtrapolator().mGuidanceRelaxationMin << " " << synthesizer->getTextureExtrapolator().mGuidanceRelaxationMax;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float relaxContraintsMin = 0;
	float relaxContraintsMax = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> relaxContraintsMin;
		ss >> relaxContraintsMax;
	}

	//synthesisParameterFile << "guidanceWeight ";
	//synthesisParameterFile << synthesizer->getTextureExtrapolator().mGuidanceWeight;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float guidanceWeight = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> guidanceWeight;
	}

	//synthesisParameterFile << "distancePower ";
	//synthesisParameterFile << synthesizer->getTextureExtrapolator().mFeaturePower << " " << synthesizer->getTextureExtrapolator().mGuidanceRelaxationMax;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float distancePower = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> distancePower;
	}

	//synthesisParameterFile << "initializationError ";
	//synthesisParameterFile << synthesizer->getTextureExtrapolator().mInitializationError;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float initializationError = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> initializationError;
	}

	//synthesisParameterFile << "nbLabels ";
	//synthesisParameterFile << synthesizer->getTextureExtrapolator().mNbLabels;
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	int nbLabels = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> nbLabels;
	}

	//----------------------------------------------

	//synthesisParameterFile << "[PPTBF]" << std::endl;
	std::getline( semiProcTexConfigFile, lineData );

	//synthesisParameterFile << "shift ";
	//synthesisParameterFile << synthesizer->getPPTBF()->getShiftX() << " " << synthesizer->getPPTBF()->getShiftY();
	//synthesisParameterFile << std::endl;
	std::getline( semiProcTexConfigFile, lineData );
	float pptbfShiftX = 0;
	float pptbfShiftY = 0;
	{
		std::stringstream ss( lineData );
		ss >> text;
		ss >> pptbfShiftX;
		ss >> pptbfShiftY;
	}

	//--------------------------------------------------------------------------------------------------------


	// TODO
	// - continue reading parameter file
	// ...

	// Close file
	semiProcTexConfigFile.close();

	// Exit code
	return 0;
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
