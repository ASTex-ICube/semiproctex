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

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

namespace Pt
{

inline const PtWindow* const PtPPTBF::getWindow() const
{
	return mWindow;
}

inline const PtFeature* const PtPPTBF::getFeature() const
{
	return mFeature;
}

inline const PtNoise* const PtPPTBF::getNoise() const
{
	return mNoise;
}

/******************************************************************************
 * PPTBF size
 ******************************************************************************/
inline int PtPPTBF::getWidth() const
{
	return mWidth;
}
inline void PtPPTBF::setWidth( const int pValue )
{
	mWidth = pValue;
}
inline int PtPPTBF::getHeight() const
{
	return mHeight;
}
inline void PtPPTBF::setHeight( const int pValue )
{
	mHeight = pValue;
}

/******************************************************************************
 * Model Transform
 ******************************************************************************/
inline int PtPPTBF::getResolution() const
{
	return mResolution;
}
inline void PtPPTBF::setResolution( const int pValue )
{
	mResolution = pValue;
}
inline float PtPPTBF::getShiftX() const
{
	return mShiftX;
}
inline void PtPPTBF::setShiftX( const float pValue )
{
	mShiftX = pValue;
}
inline float PtPPTBF::getShiftY() const
{
	return mShiftY;
}
inline void PtPPTBF::setShiftY( const float pValue )
{
	mShiftY = pValue;
}
inline float PtPPTBF::getAlpha() const
{
	return mAlpha;
}
inline void PtPPTBF::setAlpha( const float pValue )
{
	mAlpha = pValue;
}
inline float PtPPTBF::getRescalex() const
{
	return mRescalex;
}
inline void PtPPTBF::setRescalex( const float pValue )
{
	mRescalex = pValue;
}

/******************************************************************************
 * Point process
 ******************************************************************************/
inline int PtPPTBF::getTilingType() const
{
	return static_cast< int >( mTilingType );
}
inline float PtPPTBF::getJittering() const
{
	return mJittering;
}
inline float PtPPTBF::getCellSubdivisionProbability() const
{
	return mCellSubdivisionProbability;
}
inline int PtPPTBF::getNbRelaxationIterations() const
{
	return mNbRelaxationIterations;
}
inline void PtPPTBF::setTilingType( const int pValue )
{
	mTilingType = static_cast< tilingtype >( pValue );
}
inline void PtPPTBF::setJittering( const float pValue )
{
	mJittering = pValue;
}
inline void PtPPTBF::setCellSubdivisionProbability( const float pValue )
{
	mCellSubdivisionProbability = pValue;
}
inline void PtPPTBF::setNbRelaxationIterations( const int pValue )
{
	mNbRelaxationIterations = pValue;
}

/******************************************************************************
 * Window function
 ******************************************************************************/
inline void PtPPTBF::setCellularToGaussianWindowBlend( const float pValue )
{
	mCellularToGaussianWindowBlend = pValue;
}
// - cellular window
inline float PtPPTBF::getRectangularToVoronoiShapeBlend() const
{
	return mRectangularToVoronoiShapeBlend;
}
inline void PtPPTBF::setRectangularToVoronoiShapeBlend( const float pValue )
{
	mRectangularToVoronoiShapeBlend = pValue;
}
inline float PtPPTBF::getCellularWindowDecay() const
{
	return mCellularWindowDecay;
}
inline void PtPPTBF::setCellularWindowDecay( const float pValue )
{
	mCellularWindowDecay = pValue;
}
inline float PtPPTBF::getCellularWindowNorm() const
{
	return mCellularWindowNorm;
}
inline void PtPPTBF::setCellularWindowNorm( const float pValue )
{
	mCellularWindowNorm = pValue;
}
// - Gaussian window
inline int PtPPTBF::getWindowShape() const
{
	return mWindowShape;
}
inline void PtPPTBF::setWindowShape( const int pValue )
{
	mWindowShape = pValue;
}
inline float PtPPTBF::getWindowArity() const
{
	return mWindowArity;
}
inline void PtPPTBF::setWindowArity( const float pValue )
{
	mWindowArity = pValue;
}
inline float PtPPTBF::getWindowLarp() const
{
	return mWindowLarp;
}
inline void PtPPTBF::setWindowLarp( const float pValue )
{
	mWindowLarp = pValue;
}
inline float PtPPTBF::getWindowNorm() const
{
	return mWindowNorm;
}
inline void PtPPTBF::setWindowNorm( const float pValue )
{
	mWindowNorm = pValue;
}
inline float PtPPTBF::getWindowSmooth() const
{
	return mWindowSmooth;
}
inline void PtPPTBF::setWindowSmooth( const float pValue )
{
	mWindowSmooth = pValue;
}
inline float PtPPTBF::getWindowBlend() const
{
	return mWindowBlend;
}
inline void PtPPTBF::setWindowBlend( const float pValue )
{
	mWindowBlend = pValue;
}
inline float PtPPTBF::getWindowSigwcell() const
{
	return mWindowSigwcell;
}
inline void PtPPTBF::setWindowSigwcell( const float pValue )
{
	mWindowSigwcell = pValue;
}
inline void PtPPTBF::setGaussianWindowDecay( const float pValue )
{
	mGaussianWindowDecay = pValue;
}
inline void PtPPTBF::setGaussianWindowDecayJittering( const float pValue )
{
	mGaussianWindowDecayJittering = pValue;
}

/******************************************************************************
 * Feature function
 ******************************************************************************/
inline int PtPPTBF::getMinNbGaborKernels() const
{
	return mMinNbGaborKernels;
}
inline void PtPPTBF::setMinNbGaborKernels( const int pValue )
{
	mMinNbGaborKernels = pValue;
}
inline int PtPPTBF::getMaxNbGaborKernels() const
{
	return mMaxNbGaborKernels;
}
inline void PtPPTBF::setMaxNbGaborKernels( const int pValue )
{
	mMaxNbGaborKernels = pValue;
}
inline float PtPPTBF::getFeatureNorm() const
{
	return mFeatureNorm;
}
inline void PtPPTBF::setFeatureNorm( const float pValue )
{
	mFeatureNorm = pValue;
}
inline int PtPPTBF::getGaborStripesFrequency() const
{
	return mGaborStripesFrequency;
}
inline void PtPPTBF::setGaborStripesFrequency( const int pValue )
{
	mGaborStripesFrequency = pValue;
}
inline float PtPPTBF::getGaborStripesCurvature() const
{
	return mGaborStripesCurvature;
}
inline void PtPPTBF::setGaborStripesCurvature( const float pValue )
{
	mGaborStripesCurvature = pValue;
}
inline float PtPPTBF::getGaborStripesOrientation() const
{
	return mGaborStripesOrientation;
}
inline void PtPPTBF::setGaborStripesOrientation( const float pValue )
{
	mGaborStripesOrientation = pValue;
}
inline float PtPPTBF::getGaborStripesThickness() const
{
	return mGaborStripesThickness;
}
inline void PtPPTBF::setGaborStripesThickness( const float pValue )
{
	mGaborStripesThickness = pValue;
}
inline float PtPPTBF::getGaborDecay() const
{
	return mGaborDecay;
}
inline void PtPPTBF::setGaborDecay( const float pValue )
{
	mGaborDecay = pValue;
}
inline float PtPPTBF::getGaborDecayJittering() const
{
	return mGaborDecayJittering;
}
inline void PtPPTBF::setGaborDecayJittering( const float pValue )
{
	mGaborDecayJittering = pValue;
}
inline float PtPPTBF::getFeaturePhaseShift() const
{
	return mFeaturePhaseShift;
}
inline void PtPPTBF::setFeaturePhaseShift( const float pValue )
{
	mFeaturePhaseShift = pValue;
}
inline int PtPPTBF::getBombingFlag() const
{
	return mBombingFlag;
}
inline void PtPPTBF::setBombingFlag( const int pValue )
{
	mBombingFlag = pValue;
}
inline float PtPPTBF::getFeatureWinfeatcorrel() const
{
	return mFeatureWinfeatcorrel;
}
inline void PtPPTBF::setFeatureWinfeatcorrel( const float pValue )
{
	mFeatureWinfeatcorrel = pValue;
}
inline float PtPPTBF::getFeatureAnisotropy() const
{
	return mFeatureAnisotropy;
}
inline void PtPPTBF::setFeatureAnisotropy( const float pValue )
{
	mFeatureAnisotropy = pValue;
}

/******************************************************************************
 * Deformation
 ******************************************************************************/
inline float PtPPTBF::getTurbulenceAmplitude0() const
{
	return mTurbulenceAmplitude0;
}
inline void PtPPTBF::setTurbulenceAmplitude0( const float pValue )
{
	mTurbulenceAmplitude0 = pValue;
}
inline float PtPPTBF::getTurbulenceAmplitude1() const
{
	return mTurbulenceAmplitude1;
}
inline void PtPPTBF::setTurbulenceAmplitude1( const float pValue )
{
	mTurbulenceAmplitude1 = pValue;
}
inline float PtPPTBF::getTurbulenceAmplitude2() const
{
	return mTurbulenceAmplitude2;
}
inline void PtPPTBF::setTurbulenceAmplitude2( const float pValue )
{
	mTurbulenceAmplitude2 = pValue;
}

/******************************************************************************
 * Recursivity
 ******************************************************************************/
inline void PtPPTBF::setRecursiveWindowSubdivisionProbability( const float pValue )
{
	mRecursiveWindowSubdivisionProbability = pValue;
}
inline void PtPPTBF::setRecursiveWindowSubdivisionScale( const float pValue )
{
	mRecursiveWindowSubdivisionScale = pValue;
}

} // namespace Pt
