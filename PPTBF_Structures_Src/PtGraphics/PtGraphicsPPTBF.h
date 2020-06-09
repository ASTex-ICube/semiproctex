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

#ifndef _PT_GRAPHICS_PPTBF_H_
#define _PT_GRAPHICS_PPTBF_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtGraphicsConfig.h"
#include "PtPPTBF.h"

// GL
#include <glad/glad.h>

// STL
#include <vector>
#include <sstream>

// glm
#include <glm/glm.hpp>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

namespace PtGraphics
{
	class PtShaderProgram;
	class PtGraphicsDeformation;
	class PtGraphicsPointProcess;
	class PtGraphicsWindowCellular;
	class PtGraphicsFeature;
}

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace PtGraphics
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
class PTGRAPHICS_EXPORT PtGraphicsPPTBF : public Pt::PtPPTBF
{
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/**
	 * PPTBF rendering mode
	 */
	enum class ERenderingMode
	{
		ePPTBF = 0,
		eBinaryMap,
		ePalette,
		eNbRenderingModes
	};

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Textures
	 ** TODO: move this to "protected"
	 */
	GLuint mTexture_P;
	GLuint mTexture_rnd_tab;
	GLuint mTexture_G;
	GLuint mSampler_PRNG_Noise;

	GLuint mPPTBFFrameBuffer;
	GLuint mPPTBFTexture;

	GLuint mThresholdTexture;

	/**
	 * Palette exemplar
	 */
	PtShaderProgram* mShaderProgram;

	// Multiple outputs
	std::vector< GLuint > mPPTBFTextureLists;

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	PtGraphicsPPTBF();

	/**
	 * Destructor
	 */
	virtual ~PtGraphicsPPTBF();

	/**
	 * Initialize
	 */
	void initialize( const int pWidth, const int pHeight );

	/**
	 * Initialize
	 */
	void initialize( const int pWidth, const int pHeight, const int pNbOutputs );

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Get the shader program
	 *
	 * @return the shader program
	 */
	inline PtShaderProgram* editPPTBFGeneratorShaderProgram();

	/**
	 * Get the shader program
	 *
	 * @return the shader program
	 */
	inline PtShaderProgram* editPPTBFViewerShaderProgram();
	inline PtShaderProgram* editBinaryStructureMapShaderProgram();
	
	/**
	 * Compute PPTBF
	 */
	void compute();

	void compute( const int pOutputID );

	/**
	 * Render
	 */
	void render();

	// General parameters
	inline void setResolution( const int pValue );
	inline void setAlpha( const float pValue );
	inline void setRescalex( const float pValue );
	inline void setImageWidth( const int pValue );
	inline void setImageHeight( const int pValue );
	
	// Point process
	inline void setTilingType( const int pValue );
	inline void setJittering( const float pValue );
	inline void setCellSubdivisionProbability( const float pValue );
	inline void setNbRelaxationIterations( const int pValue );

	// Window function
	inline void setCellularToGaussianWindowBlend( const float pValue );
	// - cellular window
	inline void setRectangularToVoronoiShapeBlend( const float pValue );
	inline void setCellularWindowDecay( const float pValue );
	inline void setCellularWindowNorm( const float pValue );
	// - Gaussian window
	/*inline int getWindowShape() const;
	inline void setWindowShape( const int pValue );*/
	/*inline float getWindowArity() const;
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
	inline void setWindowSigwcell( const float pValue );*/
	inline void setGaussianWindowDecay( const float pValue );
	inline void setGaussianWindowDecayJittering( const float pValue );

	// Feature function
	inline void setMinNbGaborKernels( const int pValue );
	inline void setMaxNbGaborKernels( const int pValue );
	inline void setFeatureNorm( const float pValue );
	inline void setGaborStripesFrequency( const int pValue );
	inline void setGaborStripesCurvature( const float pValue );
	inline void setGaborStripesOrientation( const float pValue );
	inline void setGaborStripesThickness( const float pValue );
	inline void setGaborDecay( const float pValue );
	inline void setGaborDecayJittering( const float pValue );
	inline void setFeaturePhaseShift( const float pValue );
	inline void setBombingFlag( const int pValue );

	inline void setUseAnimation( const bool pValue );
	inline void setUseMegaKernel( const bool pValue );

	// Deformation
	inline void setTurbulenceAmplitude0( const float pValue );
	inline void setTurbulenceAmplitude1( const float pValue );
	inline void setTurbulenceAmplitude2( const float pValue );
	
	// Recursivity
	inline void setRecursiveWindowSubdivisionProbability( const float pValue );
	inline void setRecursiveWindowSubdivisionScale( const float pValue );

	// Animation
	inline void setTime( const float pValue );

	/**
	 * Labeling
	 */
	inline int getNbLabels() const;
	inline void setNbLabels( const int pValue );
	inline GLuint getPPTBFLabelMapGLHandle() const;
	inline GLuint getPPTBFRandomValueMapGLHandle() const;

	/**
	 * Callback called when PPTBF size has been modified
	 */
	void onSizeModified( const int pWidth, const int pHeight );

	/**
	 * Flag to tell whether or not to use full FBO resolution (slow "screen size" or faster "640x480" for editing)
	 */
	bool useLowResolutionPPTBF() const;
	void setUseLowResolutionPPTBF( const bool pFlag );

	/**
	 * Generate database
	 */
	// test : uniform sampling
	void generateDatabase_test( const unsigned int pWidth, const unsigned int pHeight, const char* pPath );
	// advanced sampling
	void generateDatabase( const unsigned int pWidth, const unsigned int pHeight, const char* pPath, const unsigned int pSerieID );

	/**
	 * Generate a PPTBF given a parameter file
	 */
	void generatePPTBF( const unsigned int pWidth, const unsigned int pHeight, const char* pParameterFilename );

	/**
	 * Timer info
	 */
	inline float getSynthesisTime() const;
	inline const std::stringstream& getSynthesisInfo() const;

	/**
	 * Get threshold used to binarize PPTBF
	 */
	inline float getThreshold() const;

	/**
	 * Set threshold used to binarize PPTBF
	 */
	inline void setThreshold( const float pValue );

	/**
	 * Set the rendering mode
	 */
	inline ERenderingMode getRenderingMode() const;

	/**
	 * Set the rendering mode
	 */
	inline void setRenderingMode( const ERenderingMode pValue );

	/**
	 * Compute binary structure map
	 */
	void computeBinaryStructureMap();

	/**
	 * Helper function to load a texture
	 */
	static bool loadTexture( const char* pFilename,
		const int pNbRequestedChannels, const GLenum pInternalFormat, const GLenum pFormat, const GLenum pType,
		const bool pGenerateMipmap,
		GLuint& pTexture,
		int& pWidth, int& pHeight );

	/**
	 * Compute the number of mipmap levels corresponding to given width and height of an image
	 *
	 * @param pImageWidth width of an image
	 * @param pImageHeight height of an image
	 *
	 * @return the number of mipmap levels
	 */
	static inline int getNbMipmapLevels( const int pImageWidth, const int pImageHeight );

	/**
	 * sampleParameterSpace
	 */
	void sampleParameterSpace( const unsigned int pWidth, const unsigned int pHeight, const char* pPath, const char* pImageName,
		// method parameters
		const int pDescriptor, const int pKEEPNBEST, const int pNR, const float pDeltaAlpha, const float pFactResol, const float pFactRescaleX, const float pEcart, const float pPercent,
		// external data
		std::vector< float >& mPptbfCandidateThresholds,
		std::vector< float >& mPptbfCandidateMinValues,
		std::vector< float >& mPptbfCandidateMaxValues,
		std::vector< float >& mPptbfCandidateMeanValues,
		std::vector< bool >& mSelectedNNCandidates,
		// PPTBF
		// - Point Process
		std::vector< int >& tilingType,
		std::vector< float >& jittering,
		// - Transform
		std::vector< int >& resolution,
		std::vector< float >& rotation,
		std::vector< float >& rescalex,
		// - Turbulence
		std::vector< float >& turbAmplitude0,
		std::vector< float >& turbAmplitude1,
		std::vector< float >& turbAmplitude2,
		std::vector< std::vector< float > >& turbAmp,
		// - Window
		std::vector< int >& windowShape,
		std::vector< float >& windowArity,
		std::vector< float >& windowLarp,
		std::vector< float >& windowNorm,
		std::vector< float >& windowSmoothness,
		std::vector< float >& windowBlend,
		std::vector< float >& windowSigwcell,
		// - Feature
		std::vector< int >& featureBombing,
		std::vector< float >& featureNorm,
		std::vector< float >& featureWinfeatcorrel,
		std::vector< float >& featureAniso,
		std::vector< int >& featureMinNbKernels,
		std::vector< int >& featureMaxNbKernels,
		std::vector< float >& featureSigcos,
		std::vector< float >& featureSigcosvar,
		std::vector< int >& featureFrequency,
		std::vector< float >& featurePhaseShift,
		std::vector< float >& featureThickness,
		std::vector< float >& featureCurvature,
		std::vector< float >& featureOrientation,
		// save bank ID
		std::vector< int >& featureBankIDs );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * PPTBF viewer shader program
	 */
	PtShaderProgram* mPPTBFViewerShaderProgram;
	PtShaderProgram* mBinaryStructureMapShaderProgram;

	/**
	 * Flag to tell whether or not to use full FBO resolution (slow "screen size" or faster "640x480" for editing)
	 */
	bool mUseLowResolutionPPTBF;
	int mWidthLowQuality;
	int mHeightLowQuality;

	/**
	 * Animation
	 */
	float mTime;

	/**
	 * PPTBF componenets status flag
	 */
	bool mUpdateDeformation;
	bool mUpdatePointProcess;
	bool mUpdateWindowCellular;
	bool mUpdateFeature;

	// GPU timer
	GLuint mQueryTimeElapsed;
	GLuint64 mPPTBFGenerationTime;
	float mSynthesisTime;
	std::stringstream mSynthesisInfo;
	
	/**
	 * PPTBF megakernel shader program
	 */
	PtShaderProgram* mMegakernelShaderProgram;

	/**
	 * PPTBF megakernel shader program with threshold
	 */
	PtShaderProgram* mMegakernelThresholdShaderProgram;

	/**
	 * Colormap shader program
	 */
	PtShaderProgram* mColormapShaderProgram;
	int mColormapIndex;

	/**
	 * Threshold to binarize PPTBF
	 */
	float mThreshold;

	/**
	 * Rendering mode
	 */
	ERenderingMode mRenderingMode;

	// Labeling: on-the-fly generation
	// - labeling
	int mNbLabels;
	// - texture of labels
	GLuint mPPTBFLabelMap;
	GLuint mPPTBFRandomValueMap;
			
	/******************************** METHODS *********************************/

	/**
	 * Initialize shader programs
	 *
	 * @return a flag telling whether or not it succeeds
	 */
	bool initializeShaderPrograms();

	void graphicsInitialization_BinaryStructureMap();

	/**
	 * Initialize PRNG
	 */
	void graphicsInitialization_PRNG();

	/**
	 * Initialize noise
	 */
	void graphicsInitialization_Noise();
	void graphicsInitialization_Noise_rnd_tab();
	void graphicsInitialization_Noise_G();

	/**
	 * Initialize timer(s)
	 */
	void graphicsInitialization_Timer();

	/**
	 * Finalize timer(s)
	 */
	void graphicsFinalization_Timer();

	/**
	 * Execute the mega-kernel approach
	 */
	void executeMegakernelApproach();
	void executeMegakernelApproach( const int pOutputID );

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class PtGraphicsPPTBF

} // end of namespace PtGraphics

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#include "PtGraphicsPPTBF.hpp"

#endif // _PT_GRAPHICS_PPTBF_H_
