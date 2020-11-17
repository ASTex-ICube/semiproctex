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

#ifndef _PT_PPTBF_H_
#define _PT_PPTBF_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtConfig.h"

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

namespace Pt
{
	class PtWindow;
	class PtFeature;
	class PtNoise;
}

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace Pt
{

 /**
  * @class PtPPTBF
  *
  * @brief The PtPPTBF class provides interface to the "best matching tile selection" stage.
  *
  * The best matching tile selection occurs during the final texture synthesis stage:
  * - for each tile of a given composition map, a list of candidate tiles matching some features/characteristics is determined,
  * - the selector job is to select the best one among all acandidates.
  */
class PTMODEL_EXPORT PtPPTBF
{
	
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/**
	 * Tiling type
	 */
	typedef enum
	{
		REGULAR, IRREGULAR, CROSS, BISQUARE, IRREGULARX, IRREGULARY
	}
	tilingtype;

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	PtPPTBF();

	/**
	 * Destructor
	 */
	virtual ~PtPPTBF();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	inline const PtWindow* const getWindow() const;
	inline const PtFeature* const getFeature() const;
	inline const PtNoise* const getNoise() const;

	/**
	 * Evaluate PPTBF
	 */
	float eval( const float x, const float y,
		// Point process
		const PtPPTBF::tilingtype pTilingType, const float pJittering, const float pCellSubdivisionProbability, const int pNbRelaxationIterations,
		// Window function
		const float pCellularToGaussianWindowBlend, /*const*/ float pCellularWindowNorm, const float pRectangularToVoronoiShapeBlend, const float pCellularWindowDecay, const float pGaussianWindowDecay, const float pGaussianWindowDecayJittering,
		// Feature function
		const int pMinNbGaborKernels, const int pMaxNbGaborKernels, /*const*/ float pFeatureNorm, const int pGaborStripesFrequency, const float pGaborStripesCurvature, const float pGaborStripesOrientation, const float pGaborStripesThickness, const float pGaborDecay, const float pGaborDecayJittering, const float pFeaturePhaseShift, const bool pBombingFlag,
		// Others
		const float pRecursiveWindowSubdivisionProbability, const float pRecursiveWindowSubdivisionScale,
		// Debug
		const bool pShowWindow = true, const bool pShowFeature = true );

	/**
	 * ...
	 */
	/*inline*/ static float getNorm( float inorm );

	/**
	 * PPTBF size
	 */
	inline int getWidth() const;
	inline void setWidth( const int pValue );
	inline int getHeight() const;
	inline void setHeight( const int pValue );

	// Model Transform
	inline int getResolution() const;
	inline void setResolution( const int pValue );
	inline float getShiftX() const;
	inline void setShiftX( const float pValue );
	inline float getShiftY() const;
	inline void setShiftY( const float pValue );
	inline float getAlpha() const;
	inline void setAlpha( const float pValue );
	inline float getRescalex() const;
	inline void setRescalex( const float pValue );

	// Deformation
	inline float getTurbulenceAmplitude0() const;
	inline void setTurbulenceAmplitude0( const float pValue );
	inline float getTurbulenceAmplitude1() const;
	inline void setTurbulenceAmplitude1( const float pValue );
	inline float getTurbulenceAmplitude2() const;
	inline void setTurbulenceAmplitude2( const float pValue );

	// Point process
	inline int getTilingType() const;
	inline float getJittering() const;
	inline float getCellSubdivisionProbability() const;
	inline int getNbRelaxationIterations() const;
	inline void setTilingType( const int pValue );
	inline void setJittering( const float pValue );
	inline void setCellSubdivisionProbability( const float pValue );
	inline void setNbRelaxationIterations( const int pValue );

	// Window function
	inline void setCellularToGaussianWindowBlend( const float pValue );
	// - cellular window
	inline float getRectangularToVoronoiShapeBlend() const;
	inline void setRectangularToVoronoiShapeBlend( const float pValue );
	inline float getCellularWindowDecay() const;
	inline void setCellularWindowDecay( const float pValue );
	inline float getCellularWindowNorm() const;
	inline void setCellularWindowNorm( const float pValue );
	// - Gaussian window
	inline int getWindowShape() const;
	inline void setWindowShape( const int pValue );
	inline float getWindowArity() const;
	inline void setWindowArity( const float pValue );
	inline float getWindowLarp() const;
	inline void setWindowLarp( const float pValue );
	inline float getWindowNorm() const;
	inline void setWindowNorm( const float pValue );
	inline float getWindowSmooth() const;
	inline void setWindowSmooth( const float pValue );
	inline float getWindowBlend() const;
	inline void setWindowBlend( const float pValue );
	inline float getWindowSigwcell() const;
	inline void setWindowSigwcell( const float pValue );
	inline void setGaussianWindowDecay( const float pValue );
	inline void setGaussianWindowDecayJittering( const float pValue );

	// Feature function
	inline int getBombingFlag() const;
	inline void setBombingFlag( const int pValue );
	inline float getFeatureNorm() const;
	inline void setFeatureNorm( const float pValue );
	inline float getFeatureWinfeatcorrel() const;
	inline void setFeatureWinfeatcorrel( const float pValue );
	inline float getFeatureAnisotropy() const;
	inline void setFeatureAnisotropy( const float pValue );
	inline int getMinNbGaborKernels() const;
	inline void setMinNbGaborKernels( const int pValue );
	inline int getMaxNbGaborKernels() const;
	inline void setMaxNbGaborKernels( const int pValue );
	inline float getGaborDecay() const;
	inline void setGaborDecay( const float pValue );
	inline float getGaborDecayJittering() const;
	inline void setGaborDecayJittering( const float pValue );
	inline int getGaborStripesFrequency() const;
	inline void setGaborStripesFrequency( const int pValue );
	inline float getFeaturePhaseShift() const;
	inline void setFeaturePhaseShift( const float pValue );
	inline float getGaborStripesThickness() const;
	inline void setGaborStripesThickness( const float pValue );
	inline float getGaborStripesCurvature() const;
	inline void setGaborStripesCurvature( const float pValue );
	inline float getGaborStripesOrientation() const;
	inline void setGaborStripesOrientation( const float pValue );
	
	// Recursivity
	inline void setRecursiveWindowSubdivisionProbability( const float pValue );
	inline void setRecursiveWindowSubdivisionScale( const float pValue );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Window function
	 */
	PtWindow* mWindow;

	/**
	 * Feature function
	 */
	PtFeature* mFeature;

	/**
	 * Noise
	 */
	PtNoise* mNoise;

	/**
	 * PPTBF size
	 */
	int mWidth;
	int mHeight;

	// Model Transform
	int mResolution;
	float mShiftX;
	float mShiftY;
	float mAlpha;
	float mRescalex;

	// Deformation
	float mTurbulenceAmplitude0;
	float mTurbulenceAmplitude1;
	float mTurbulenceAmplitude2;

	// Point process
	PtPPTBF::tilingtype mTilingType;
	float mJittering;
	float mCellSubdivisionProbability;
	int mNbRelaxationIterations;

	// Window function
	float mCellularToGaussianWindowBlend;
	float mCellularWindowNorm;
	float mRectangularToVoronoiShapeBlend;
	float mCellularWindowDecay;
	int mWindowShape; // new: type of window function [0,3] (blended tapered cos, Gauss, triang, tapered cos)
	float mWindowArity; // >=2.0 && <=5.0, arity of polygonal cell window
	float mWindowLarp; // anisotropy of cell for cellular window
	float mWindowNorm; // norm 1=L1, 2=L2, 3=Linf of cellular window
	float mWindowSmooth; // smoothness of cellular 0=linear, 1 or 2=Bezier
	float mWindowBlend; // blend between cellular=1 and classical window=0
	float mWindowSigwcell; // power of cellular window
	float mGaussianWindowDecay;
	float mGaussianWindowDecayJittering;

	// Feature function
	int mBombingFlag;
	float mFeatureNorm;
	float mFeatureWinfeatcorrel;
	float mFeatureAnisotropy;
	int mMinNbGaborKernels;
	int mMaxNbGaborKernels;
	float mGaborDecay; // uFeatureSigcos
	float mGaborDecayJittering; // uFeatureSigcosvar
	int mGaborStripesFrequency;
	float mFeaturePhaseShift;
	float mGaborStripesThickness;
	float mGaborStripesCurvature;
	float mGaborStripesOrientation;
		
	// Recursivity
	float mRecursiveWindowSubdivisionProbability;
	float mRecursiveWindowSubdivisionScale;

	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class PtPPTBF

} // end of namespace Pt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#include "PtPPTBF.hpp"

#endif // _PT_PPTBF_H_
