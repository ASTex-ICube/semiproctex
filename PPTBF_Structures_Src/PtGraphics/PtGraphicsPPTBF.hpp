/*
 * Publication: Semi-Procedural Textures Using Point Process Texture Basis Functions
 * Authors: anonymous
 *
 * Code author: Pascal Guehl
 *
 * anonymousmShaderProgram
 * anonymous
 */

/** 
 * @version 1.0
 */

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtShaderProgram.h"

// STL
#include <algorithm>
//#include <cmath>
#include <math.h>

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

namespace PtGraphics
{

/******************************************************************************
 * Get the shader program
 *
 * @return the shader program
 ******************************************************************************/
inline PtShaderProgram* PtGraphicsPPTBF::editPPTBFGeneratorShaderProgram()
{
	//return mShaderProgram;
	
	//return nullptr;

	/*if ( mRenderingMode == ERenderingMode::eBinaryMap )
	{
		return mMegakernelThresholdShaderProgram;
	}*/

	return mMegakernelShaderProgram;
}

/******************************************************************************
 * Get the shader program
 *
 * @return the shader program
 ******************************************************************************/
inline PtShaderProgram* PtGraphicsPPTBF::editPPTBFViewerShaderProgram()
{
	return mPPTBFViewerShaderProgram;
}

/******************************************************************************
 * Get the shader program
 *
 * @return the shader program
 ******************************************************************************/
inline PtShaderProgram* PtGraphicsPPTBF::editBinaryStructureMapShaderProgram()
{
	return mBinaryStructureMapShaderProgram;
}

/******************************************************************************
 * General parameters
 ******************************************************************************/
inline void PtGraphicsPPTBF::setResolution( const int pValue )
{
	//mShaderProgram->set( pValue, "uResolution" );
	mResolution = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setAlpha( const float pValue )
{
	//mShaderProgram->set( pValue, "alpha" );
	mAlpha = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setRescalex( const float pValue )
{
	//mShaderProgram->set( pValue, "rescalex" );
	mRescalex = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setImageWidth( const int pValue )
{
	//mShaderProgram->set( pValue, "imageWidth" );
	mWidth = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setImageHeight( const int pValue )
{
	//mShaderProgram->set( pValue, "imageHeight" );
	mHeight = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
	
/******************************************************************************
 * Point process
 ******************************************************************************/
inline void PtGraphicsPPTBF::setTilingType( const int pValue )
{
	//mShaderProgram->set( pValue, "uTilingType" );
	mTilingType =  static_cast< tilingtype >( pValue );

	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setJittering( const float pValue )
{
	//mShaderProgram->set( pValue, "uJittering" );
	mJittering = pValue;

	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;

	mUpdateFeature = true; // correlated to point process ?
}
inline void PtGraphicsPPTBF::setCellSubdivisionProbability( const float pValue )
{
	//mShaderProgram->set( pValue, "uCellSubdivisionProbability" );
	mCellSubdivisionProbability = pValue;

	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setNbRelaxationIterations( const int pValue )
{
	//mShaderProgram->set( pValue, "uNbRelaxationIterations" );
	mNbRelaxationIterations = pValue;

	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;

	mUpdateFeature = true;
}

/******************************************************************************
 * Window function
 ******************************************************************************/
inline void PtGraphicsPPTBF::setCellularToGaussianWindowBlend( const float pValue )
{
	//mShaderProgram->set( pValue, "uCellularToGaussianWindowBlend" );
	mCellularToGaussianWindowBlend = pValue;
}
// - cellular window
inline void PtGraphicsPPTBF::setRectangularToVoronoiShapeBlend( const float pValue )
{
	//mShaderProgram->set( pValue, "uRectangularToVoronoiShapeBlend" );
	mRectangularToVoronoiShapeBlend = pValue;

	mUpdatePointProcess = true; // modify this...
	mUpdateWindowCellular = true;
}
inline void PtGraphicsPPTBF::setCellularWindowDecay( const float pValue )
{
	//mShaderProgram->set( pValue, "uCellularWindowDecay" );
	mCellularWindowDecay = pValue;

	mUpdateWindowCellular = true;
}
inline void PtGraphicsPPTBF::setCellularWindowNorm( const float pValue )
{
	//mShaderProgram->set( pValue, "uCellularWindowNorm" );
	mCellularWindowNorm = pValue;

	mUpdatePointProcess = true; // modify this...
	mUpdateWindowCellular = true;
}
// - Gaussian window
//inline int PtGraphicsPPTBF::getWindowShape() const
//{
//	return mWindowShape;
//}
//inline void PtGraphicsPPTBF::setWindowShape( const int pValue )
//{
//	mWindowShape = pValue;
//}
inline void PtGraphicsPPTBF::setGaussianWindowDecay( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaussianWindowDecay" );
	mGaussianWindowDecay = pValue;
}
inline void PtGraphicsPPTBF::setGaussianWindowDecayJittering( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaussianWindowDecayJittering" );
	mGaussianWindowDecayJittering = pValue;
}

/******************************************************************************
 * Feature function
 ******************************************************************************/
inline void PtGraphicsPPTBF::setMinNbGaborKernels( const int pValue )
{
	//mShaderProgram->set( pValue, "uMinNbGaborKernels" );
	mMinNbGaborKernels = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setMaxNbGaborKernels( const int pValue )
{
	//mShaderProgram->set( pValue, "uMaxNbGaborKernels" );
	mMaxNbGaborKernels = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setFeatureNorm( const float pValue )
{
	//mShaderProgram->set( pValue, "uFeatureNorm" );
	mFeatureNorm = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setGaborStripesFrequency( const int pValue )
{
	//mShaderProgram->set( pValue, "uGaborStripesFrequency" );
	mGaborStripesFrequency = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setGaborStripesCurvature( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaborStripesCurvature" );
	mGaborStripesCurvature = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setGaborStripesOrientation( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaborStripesOrientation" );
	mGaborStripesOrientation = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setGaborStripesThickness( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaborStripesThickness" );
	mGaborStripesThickness = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setGaborDecay( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaborDecay" );
	mGaborDecay = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setGaborDecayJittering( const float pValue )
{
	//mShaderProgram->set( pValue, "uGaborDecayJittering" );
	mGaborDecayJittering = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setFeaturePhaseShift( const float pValue )
{
	//mShaderProgram->set( pValue, "uFeaturePhaseShift" );
	mFeaturePhaseShift = pValue;

	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setBombingFlag( const int pValue )
{
	//mShaderProgram->set( pValue, "uBombingFlag" );
	mBombingFlag = pValue;

	mUpdateFeature = true; // feature or window ?
}

inline void PtGraphicsPPTBF::setUseAnimation( const bool pFlag )
{
	//mShaderProgram->set(static_cast< unsigned int >( pFlag ), "uUseAnimation");
}

/******************************************************************************
 * Deformation
 ******************************************************************************/
inline void PtGraphicsPPTBF::setTurbulenceAmplitude0( const float pValue )
{
	//mShaderProgram->set( pValue, "uTurbulenceAmplitude_0" );
	mTurbulenceAmplitude0 = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setTurbulenceAmplitude1( const float pValue )
{
	//mShaderProgram->set( pValue, "uTurbulenceAmplitude_1" );
	mTurbulenceAmplitude1 = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}
inline void PtGraphicsPPTBF::setTurbulenceAmplitude2( const float pValue )
{
	//mShaderProgram->set( pValue, "uTurbulenceAmplitude_2" );
	mTurbulenceAmplitude2 = pValue;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;
}

/******************************************************************************
 * Recursivity
 ******************************************************************************/
inline void PtGraphicsPPTBF::setRecursiveWindowSubdivisionProbability( const float pValue )
{
	//mShaderProgram->set( pValue, "uRecursiveWindowSubdivisionProbability" );
}
inline void PtGraphicsPPTBF::setRecursiveWindowSubdivisionScale( const float pValue )
{
	//mShaderProgram->set( pValue, "uRecursiveWindowSubdivisionScale" );
}

/******************************************************************************
 * Labeling
 ******************************************************************************/
inline int PtGraphicsPPTBF::getNbLabels() const
{
	return mNbLabels;
}
inline void PtGraphicsPPTBF::setNbLabels( const int pValue )
{
	mNbLabels = pValue;
}
inline GLuint PtGraphicsPPTBF::getPPTBFLabelMapGLHandle() const
{
	return mPPTBFLabelMap;
}
inline GLuint PtGraphicsPPTBF::getPPTBFRandomValueMapGLHandle() const
{
	return mPPTBFRandomValueMap;
}

/******************************************************************************
 * Animation
 ******************************************************************************/
inline void PtGraphicsPPTBF::setTime( const float pValue )
{
	mTime = pValue;

	//mShaderProgram->set( pValue, "uTime" );
}

/******************************************************************************
 * ...
 ******************************************************************************/
inline float PtGraphicsPPTBF::getSynthesisTime() const
{
	return mSynthesisTime;
}

/******************************************************************************
 * ...
 ******************************************************************************/
inline const std::stringstream& PtGraphicsPPTBF::getSynthesisInfo() const
{
	return mSynthesisInfo;
}

/******************************************************************************
 * Get threshold used to binarize PPTBF
 ******************************************************************************/
inline float PtGraphicsPPTBF::getThreshold() const
{
	return mThreshold;
}

/******************************************************************************
 * Set threshold used to binarize PPTBF
 ******************************************************************************/
inline void PtGraphicsPPTBF::setThreshold( const float pValue )
{
	mThreshold = pValue;
}

/******************************************************************************
 * Get the rendering mode
 ******************************************************************************/
inline PtGraphicsPPTBF::ERenderingMode PtGraphicsPPTBF::getRenderingMode() const
{
	return mRenderingMode;
}

/******************************************************************************
 * Set the rendering mode
 ******************************************************************************/
inline void PtGraphicsPPTBF::setRenderingMode( const PtGraphicsPPTBF::ERenderingMode pValue )
{
	mRenderingMode = pValue;
}

/******************************************************************************
 * Compute the number of mipmap levels corresponding to given width and height of an image
 *
 * @param pImageWidth width of an image
 * @param pImageHeight height of an image
 *
 * @return the number of mipmap levels
 ******************************************************************************/
inline int PtGraphicsPPTBF::getNbMipmapLevels( const int pImageWidth, const int pImageHeight )
{
	return static_cast< int >( std::floor( std::log2( static_cast< float >( std::max( pImageWidth, pImageHeight ) ) ) ) ) + 1;
}

} // namespace PtGraphics
