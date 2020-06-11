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

#ifndef _SPT_SYNTHESIZER_H_
#define _SPT_SYNTHESIZER_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "SptHviewInterface.h"

// STL
#include <string>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace Spt
{

 /**
  * @class SptSynthesizer
  *
  * @brief The SptSynthesizer class provides interface to semi-procedural texture synthesis model.
  *
  * Semi-procedural texture synthesis separate process in two:
  * procedural stochastic structures are synthesized procedurally,
  * then color details are transfer through data-driven by-example synthesis.
  *
  * Example of parameter file:
  *
  *  [EXEMPLAR]
  *  name TexturesCom_Crackles0011_S
  *  exemplarSize 512 512
  *  [SYNTHESIS]
  *  outputSize 1024 1024
  *  [PYRAMID]
  *  nbMipmapLevels 10
  *  pyramidMaxLevel 9
  *  pyramidMinSize 32
  *  nbPyramidLevels 5
  *  [BLOCK INITIALIZATION]
  *  blockGrid 8 8
  *  blockSize 128 128
  *  useSmartInitialization 0
  *  smartInitNbPasses 10
  *  [CORRECTION PASS]
  *  correctionNbPasses 3
  *  correctionSubPassBlockSize 3
  *  correctionNeighborhoodSize 2
  *  correctionNeighborSearchRadius 4
  *  correctionNeighborSearchNbSamples 11
  *  correctionNeighborSearchDDepth 3
  *  [MATERIAL]
  *  correctionWeightAlbedo 1
  *  correctionWeightHeight 1
  *  correctionWeightNormal 1
  *  correctionWeightRoughness 1
  *  [LABEL MAP]
  *  useLabelMap 1
  *  labelmapType 0
  *  useLabelSampler 0
  *  labelSamplerAreaThreshold 2500
  *  [GUIDANCE]
  *  correctionGuidanceWeight 0.85
  *  correctionExemplarWeightDistance 0
  *  correctionGuidanceWeightDistance 0
  *  correctionLabelErrorAmount 0.25
  *  [SEMI PROCEDURAL]
  *  PPTBFThreshold 0.109375
  *  relaxContraints 0.09375 0.210938
  *  guidanceWeight 0.763 0
  *  distancePower 0.1
  *  initializationError 0.196078
  *  nbLabels 6
  *  [PPTBF]
  *  shift 10 10
  *
  */
class SptSynthesizer
{
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	SptSynthesizer();

	/**
	 * Destructor
	 */
	virtual ~SptSynthesizer();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Load synthesis parameters
	 *
	 * @param pFilename
	 *
	 * @return error status
	 */
	int loadParameters( const char* pFilename );

	/**
	 * Launch the synthesis pipeline
	 */
	virtual void execute();

	/**
	 * Save/export synthesis results
	 */
	virtual void saveResults();

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Interface to hview api
	 */
	SptHviewInterface mHviewInterface;

	/**
	 * Semi-procedural texture synthesis parameters
	 */
	// - exemplar
	std::string mExemplarName;
	int mExemplarWidth;
	int mExemplarHeight;
	// - output
	int mOutputWidth;
	int mOutputHeight;
	// - pyramid
	int mPyramidNbMipmapLevels;
	int mPyramidMaxLevel;
	int mPyramidMinSize;
	int mPyramidNbLevels;
	// - block initialization
	int mblockInitGridWidth;
	int mblockInitGridHeight;
	int mblockInitBlockWidth;
	int mblockInitBlockHeight;
	bool mblockInitUseSmartInitialization;
	int mblockInitSmartInitNbPasses;
	// - correction pass
	int mCorrectionNbPasses;
	int mCorrectionSubPassBlockSize;
	int mCorrectionNeighborhoodSize;
	int mCorrectionNeighborSearchRadius;
	int mCorrectionNeighborSearchNbSamples;
	int mCorrectionNeighborSearchDepth;
	// - material
	float mCorrectionWeightAlbedo;
	float mCorrectionWeightHeight;
	float mCorrectionWeightNormal;
	float mCorrectionWeightRoughness;
	// - label map
	bool mUseLabelMap;
	int mLabelmapType;
	bool mUseLabelSampler;
	float mLabelSamplerAreaThreshold;
	// - guidance
	float mCorrectionGuidanceWeight;
	float mCorrectionExemplarWeightDistance;
	float mCorrectionGuidanceWeightDistance;
	float mCorrectionLabelErrorAmount;
	// - semi-procedural
	float mSemiProcTexPPTBFThreshold;
	float mSemiProcTexRelaxContraints;
	float mSemiProcTexGuidanceWeight;
	float mSemiProcTexDistancePower;
	float mSemiProcTexInitializationError;
	int mSemiProcTexNbLabels;
	//	- PPTBF
	int mPtbfShiftX;
	int mPptbfShiftY;

	/******************************** METHODS *********************************/

	/**
	 * Initialization stage
	 * - strategies/policies to choose blocks at initialization
	 */
	virtual void smartInitialization();

	/**
	 * Correction pass
	 */
	virtual void correction();

	/**
	 * Upsampling pass
	 */
	virtual void upsampling();

	/**
	 * Load structure map (binary)
	 *
	 * @param pFilename
	 */
	void loadStructureMap( const char* pFilename );

	/**
	 * Load distance map
	 *
	 * @param pFilename
	 */
	void loadDistanceMap( const char* pFilename );

	/**
	 * Load label map
	 *
	 * @param pFilename
	 */
	void loadLabelMap( const char* pFilename );

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class SptSynthesizer

} // end of namespace Spt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#endif // _SPT_SYNTHESIZER_H_
