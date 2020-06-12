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

#ifndef _OS_TEXTURE_SYNTHESIZER_VIEWER_H_
#define _OS_TEXTURE_SYNTHESIZER_VIEWER_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

 // GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Project
#include "PtViewerConfig.h"
#include "PtCamera.h"

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

// Project
namespace PtGraphics
{
	class PtGraphicsPPTBF;
	class PtGraphicsHistogram;
	class PtGraphicsLBP;
}

/******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace PtGUI
{

/** 
 * @class PtViewer
 *
 * @brief The PtViewer class provides ...
 *
 * ...
 */
class PtViewer
{

	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Request: texture synthesis
	 */
	bool mLaunchRequest;

	/**
	 * Request: texture synthesis
	 */
	bool mRequestLabelMapViewer;

	/**
	 * ...
	 */
	void synthesize();

	// Main variables

	// Exemplar
	std::string uiModelFilename;

	bool uiTextureSynthesis_launch = false;

	/**
	 * Flag to tell whether or not to display the user interface
	 */
	bool mShowUI;

	/******************************** METHODS *********************************/

	/**
	 * Constructor
	 *
	 * @param pTextureSynthesizer texture synthesizer manager
	 */
	PtViewer();
	
	/**
	 * Destructor
	 */
	virtual ~PtViewer();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Graphics window
	 */
	GLFWwindow* getWindow();

	/**
	 * Initialize Window
	 */
	void initializeWindow();

	/**
	 * Initialize GL
	 */
	void initializeGL();

	/**
	 * Handle requests
	 */
	void handleRequests();

	/**
	 * Refresh
	 */
	void refresh();

	/**
	 * Get PPTBF
	 */
	PtGraphics::PtGraphicsPPTBF* getPPTBF();

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:
	
	/******************************* ATTRIBUTES *******************************/

	/**
	 * Graphics window
	 */
	GLFWwindow* mWindow;

	/**
	 * Camera
	 */
	PtGraphics::PtCamera* mCamera;

	/**
	 * OpenGL and GLSL versions
	 */
	const GLubyte* mVendor;
	const GLubyte* mRenderer;
	const GLubyte* mVersion;
	const GLubyte* mShadingLanguageVersion;
	GLint mMajorVersion;
	GLint mMinorVersion;

	/**
	 * User interface parameters
	 */
	// - Point process parameters
	int uiPointProcess_tilingType;
	float uiPointProcess_cellSubdivisionProbability;
	int uiPointProcess_nbRelaxationIterations;
	float uiPointProcess_jittering;
	// - Window function parameters
	float uiWindowFunction_cellularVSGaussianWindowBlend;
	float uiWindowFunction_rectangularToVoronoiShapeBlend;
	float uiWindowFunction_cellularWindowDecay;
	float uiWindowFunction_cellularWindowDecayVariation;
	float uiWindowFunction_cellularWindowNorm;
	int uiWindowFunction_windowShape;
	float uiWindowFunction_windowArity;
	float uiWindowFunction_windowLarp;
	float uiWindowFunction_windowNorm;
	float uiWindowFunction_windowSmooth;
	float uiWindowFunction_windowBlend;
	float uiWindowFunction_windowSigwcell;
	float uiWindowFunction_gaussianWindowDecay;
	float uiWindowFunction_gaussianWindowDecayVariation;
	float uiWindowFunction_gaussianWindowNorm;
	// - Feature function parameters
	float uiFeatureFunction_featureNorm;
	float uiFeatureFunction_winfeatcorrel; // new
	float uiFeatureFunction_anisotropy; // new
	int uiFeatureFunction_gaborMinNbKernels;
	int uiFeatureFunction_gaborMaxNbKernels;
	float uiFeatureFunction_gaborDecay;
	float uiFeatureFunction_gaborDecayVariation;
	float uiFeatureFunction_gaborStripesCurvature;
	float uiFeatureFunction_gaborStripesOrientation;
	float uiFeatureFunction_gaborStripesFrequency;
	float uiFeatureFunction_gaborStripesThickness;
	float uiFeatureFunction_featurePhaseShift;
	// - Deformations, Non-Stationarity and_Mixtures parameters
	glm::vec3 uiDefNonStaAndMix_spatialDeformation;
	float uiDefNonStaAndMix_recursiveWindowSplittingProbability;
	float uiDefNonStaAndMix_recursiveWindowSplittingScaleRatio;
	// Global settings
	int uiGlobalSettings_RESOL;
	float uiGlobalSettings_alpha;
	float uiGlobalSettings_rescalex;
	// PPTBF
	int uiPPTBF_bombingFlag;

	bool uiPPTBF_animation;
	bool uiPPTBF_megaKernel;
	
	bool uiPPTBF_timer;

	// Debug
	bool uiGlobalSettings_useWindowFunction;
	bool uiGlobalSettings_useFeatureFunction;

	// Histograms
	int uiBinaryStructureMap_nbBins;
	int uiBinaryStructureMap_threshold;

	/**
	 * Graphics objects
	 */
	PtGraphics::PtGraphicsPPTBF* mGraphicsPPTBF;

	/**
	 * Histogram
	 */
	PtGraphics::PtGraphicsHistogram* mGraphicsHistogram;

	/**
	 * UI edition flag
	 */
	bool mPPTBFUpdateRequested;

	/**
	 * UI edition flag
	 */
	bool mHistogramUpdateRequested;

	/**
	 * UI edition flag
	 */
	bool mUIShowHistogram;

	float mBinaryStructureMapThreshold;
		
	bool mUseRealTime;

	// GPU timer
	GLuint mQueryTimeElapsed;
	GLuint64 mPPTBFGenerationTime;

	/**
	 * LBP graphics manager
	 */
	PtGraphics::PtGraphicsLBP* mGraphicsLBP;

	/**
	 * GL Core profile parameters
	 */
	bool mUseGLCoreProfile;
	GLuint mGLCoreProfileVAO;

	/******************************** METHODS *********************************/

	/**
	 * ...
	 */
	static void window_close_callback( GLFWwindow* window );

	/**
	 * ...
	 */
	static void window_size_callback( GLFWwindow* window, int width, int height );

	/**
	 * ...
	 */
	static void framebuffer_size_callback( GLFWwindow* window, int width, int height );

	/**
	 * Slot called when the data model has been modified
	 */
	virtual void onDataModelModified();

	/**
	 * Callback called when PPTBF size has been modified
	 */
	void onSizeModified( const int pWidth, const int pHeight );

	/**
	 * Initialize timer(s)
	 */
	void graphicsInitialization_Timer();

	/**
	 * Finalize timer(s)
	 */
	void graphicsFinalization_Timer();

	/**
 	 * Initialize PPTBF
	 */
	void initializePPTBF( const int pWidth, const int pHeight );

	/**
 	 * Finalize PPTBF
	 */
	void finalizePPTBF();

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:
	
	/******************************* ATTRIBUTES *******************************/
	
	/******************************** METHODS *********************************/

};

}

#endif
