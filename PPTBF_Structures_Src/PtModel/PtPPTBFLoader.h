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

#ifndef _PT_PPTBF_LOADER_H_
#define _PT_PPTBF_LOADER_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtConfig.h"

// Project
#include "PtPPTBF.h"

// STL
#include <vector>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

//namespace Pt
//{
//	class PtPPTBF;
//}

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace Pt
{

 /**
  * @class PtPPTBFLoader
  *
  * @brief The PtPPTBFLoader class provides interface to the "best matching tile selection" stage.
  *
  * The best matching tile selection occurs during the final texture synthesis stage:
  * - for each tile of a given composition map, a list of candidate tiles matching some features/characteristics is determined,
  * - the selector job is to select the best one among all acandidates.
  */
class PTMODEL_EXPORT PtPPTBFLoader
{
	
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	// Point process
	PtPPTBF::tilingtype pTilingType;
	float pJittering;
	float pCellSubdivisionProbability;
	int pNbRelaxationIterations;
	// Window function
	float pCellularToGaussianWindowBlend;
	float pCellularWindowNorm;
	float pRectangularToVoronoiShapeBlend;
	float pCellularWindowDecay;
	float pGaussianWindowDecay;
	float pGaussianWindowDecayJittering;
	// Feature function
	int pMinNbGaborKernels;
	int pMaxNbGaborKernels;
	float pFeatureNorm;
	int pGaborStripesFrequency;
	float pGaborStripesCurvature;
	float pGaborStripesOrientation;
	float pGaborStripesThickness;
	float pGaborDecay;
	float pGaborDecayJittering;
	float pFeaturePhaseShift;
	bool pBombingFlag;
	// Deformation
	float turbulenceA0;
	float turbulenceA1;
	float turbulenceA2;
	// Others
	float pRecursiveWindowSubdivisionProbability;
	float pRecursiveWindowSubdivisionScale;
	// Debug
	bool pShowWindow;
	bool pShowFeature;

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	PtPPTBFLoader();

	/**
	 * Destructor
	 */
	virtual ~PtPPTBFLoader();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Read file
	 */
	PtPPTBF* import( const char* pFilename );

	/**
	 * Write file
	 */
	void write( const PtPPTBF* pDataModel, const char* pFilename );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class PtPPTBFLoader

} // end of namespace Pt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#include "PtPPTBFLoader.hpp"

#endif // _PT_PPTBF_LOADER_H_
