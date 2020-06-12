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

#include "PtGraphicsPPTBF.h"
 
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
#include <iomanip>
#include <vector>
#include <fstream>

// Project
#include "PtNoise.h"
#include "PtImageHelper.h"
#include <PtEnvironment.h>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Pt;
using namespace PtGraphics;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

//#define PROBE

#define PRODUCT

//#define WTRANSF

//#define REFINEMENT

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Constructor
 ******************************************************************************/
PtGraphicsPPTBF::PtGraphicsPPTBF()
:	PtPPTBF()
,	mTexture_P( 0 )
,	mTexture_rnd_tab( 0 )
,	mTexture_G( 0 )
,	mUseLowResolutionPPTBF( false )
,	mWidthLowQuality( 512 )
,	mHeightLowQuality( 512 )
,	mTime( 0.f )
,	mUpdateDeformation( true )
,	mUpdatePointProcess( true )
,	mUpdateWindowCellular( true )
,	mUpdateFeature( true )
,	mQueryTimeElapsed( 0 )
,	mPPTBFGenerationTime( 0 )
,	mSynthesisTime( 0.f )
,	mSynthesisInfo()
,	mThreshold( 0.0f )
,	mRenderingMode( ERenderingMode::ePPTBF )
,	mThresholdTexture( 0 )
,	mNbLabels( 0 )
,	mPPTBFLabelMap( 0 )
,	mPPTBFRandomValueMap( 0 )
{
	mShaderProgram = new PtShaderProgram();

	mPPTBFViewerShaderProgram = new PtShaderProgram();
	mBinaryStructureMapShaderProgram = new PtShaderProgram();

	// PPTBF Megakernel approach
	mMegakernelShaderProgram = new PtShaderProgram();
	// - with threshold
	mMegakernelThresholdShaderProgram = new PtShaderProgram();
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtGraphicsPPTBF::~PtGraphicsPPTBF()
{
	delete mShaderProgram;
	mShaderProgram = nullptr;

	delete mMegakernelShaderProgram;
	mMegakernelShaderProgram = nullptr;

	delete mMegakernelThresholdShaderProgram;
	mMegakernelThresholdShaderProgram = nullptr;

	delete mPPTBFViewerShaderProgram;
	mPPTBFViewerShaderProgram = nullptr;

	delete mBinaryStructureMapShaderProgram;
	mBinaryStructureMapShaderProgram = nullptr;
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void PtGraphicsPPTBF::initialize( const int pWidth, const int pHeight )
{
	mWidth = pWidth;
	mHeight = pHeight;

	int width = pWidth;
	int height = pHeight;

	if ( mUseLowResolutionPPTBF )
	{
		width = mWidthLowQuality;
		height = mHeightLowQuality;
	}

	// Initialize PRNG
	graphicsInitialization_PRNG();

	// Initialize noise
	graphicsInitialization_Noise();

	// Initialize timer(s)
	graphicsInitialization_Timer();

	// Initialize shader programs
	bool statusOK = false;
	statusOK = initializeShaderPrograms();
	assert( statusOK );

	// Create device resources
	glGenFramebuffers( 1, &mPPTBFFrameBuffer );
	glGenTextures( 1, &mPPTBFTexture );
	// Initialize texture "rnd_tab"
	glBindTexture( GL_TEXTURE_2D, mPPTBFTexture );
	// - set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// - set min/max level for completeness
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	// - generate the texture
	glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, width, height, 0/*border*/, GL_RED, GL_FLOAT, nullptr );
	// - reset device state
	glBindTexture( GL_TEXTURE_2D, 0 );
	// Configure framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, mPPTBFFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPPTBFTexture, 0 );
	glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	// - check FBO status
	GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		std::cout << "Framebuffer error - status: " << fboStatus << std::endl;
		// - clean device resources
		finalize();
		// - reset device state
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		//return -1;
	}
	// - reset device state
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	//----------------------------------------------------
	// Initialize label map
	glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFLabelMap );
	glTextureParameteri( mPPTBFLabelMap, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( mPPTBFLabelMap, GL_TEXTURE_MAX_LEVEL, 0 );
	// - allocate memory
	const GLsizei levels = 1;
	const GLenum internalFormat = GL_R8UI;
	glTextureStorage2D( mPPTBFLabelMap, levels, internalFormat, width, height );
	// - fill data
	const GLenum format = GL_RED_INTEGER;
	const GLenum type = GL_UNSIGNED_BYTE; // 256 labels max
	//const void* data = getNoise()->getP().data();
	//glTextureSubImage2D( mPPTBFLabelMap, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
	// - clear texture
	std::vector< char > nullData = { 0 };
	glClearTexImage( mPPTBFLabelMap, /*level*/0, format, type, nullData.data() );
	//----------------------------------------------------

	//----------------------------------------------------
	// Initialize label map
	{
		glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFRandomValueMap );
		glTextureParameteri( mPPTBFRandomValueMap, GL_TEXTURE_BASE_LEVEL, 0 );
		glTextureParameteri( mPPTBFRandomValueMap, GL_TEXTURE_MAX_LEVEL, 0 );
		// - allocate memory
		const GLsizei levels = 1;
		const GLenum internalFormat = GL_R32F;
		glTextureStorage2D( mPPTBFRandomValueMap, levels, internalFormat, width, height );
		// - fill data
		const GLenum format = GL_RED;
		const GLenum type = GL_FLOAT;
		//const void* data = getNoise()->getP().data();
		//glTextureSubImage2D( mPPTBFRandomValueMap, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
		// - clear texture
		std::vector< float > nullData = { 0.f };
		glClearTexImage( mPPTBFRandomValueMap, /*level*/0, format, type, nullData.data() );
	}
	//----------------------------------------------------

	// Binary structure map
	if ( mThresholdTexture )
	{
		glDeleteTextures( 1, &mPPTBFTexture );
	}
	graphicsInitialization_BinaryStructureMap();
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void PtGraphicsPPTBF::initialize( const int pWidth, const int pHeight, const int pNbOutputs )
{
	mWidth = pWidth;
	mHeight = pHeight;

	int width = pWidth;
	int height = pHeight;

	if ( mUseLowResolutionPPTBF )
	{
		width = mWidthLowQuality;
		height = mHeightLowQuality;
	}

	// Initialize PRNG
	graphicsInitialization_PRNG();

	// Initialize noise
	graphicsInitialization_Noise();

	// Initialize timer(s)
	graphicsInitialization_Timer();

	// Initialize shader programs
	bool statusOK = false;
	statusOK = initializeShaderPrograms();
	assert( statusOK );

	// Multiple outputs
	mPPTBFTextureLists.resize( pNbOutputs );
	for ( auto& texture : mPPTBFTextureLists )
	{
		// Create device resources
		glGenTextures( 1, &texture);
		// Initialize texture "rnd_tab"
		glBindTexture( GL_TEXTURE_2D, texture );
		// - set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		// - set min/max level for completeness
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
		// - generate the texture
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, width, height, 0/*border*/, GL_RED, GL_FLOAT, nullptr );
	}
	// - reset device state
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	//----------------------------------------------------
	// Initialize label map
	glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFLabelMap );
	glTextureParameteri( mPPTBFLabelMap, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( mPPTBFLabelMap, GL_TEXTURE_MAX_LEVEL, 0 );
	// - allocate memory
	const GLsizei levels = 1;
	const GLenum internalFormat = GL_R8UI;
	glTextureStorage2D( mPPTBFLabelMap, levels, internalFormat, width, height );
	// - fill data
	const GLenum format = GL_RED_INTEGER;
	const GLenum type = GL_UNSIGNED_BYTE; // 256 labels max
	//const void* data = getNoise()->getP().data();
	//glTextureSubImage2D( mPPTBFLabelMap, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
	// - clear texture
	std::vector< char > nullData = { 0 };
	glClearTexImage( mPPTBFLabelMap, /*level*/0, format, type, nullData.data() );
	//----------------------------------------------------

	//----------------------------------------------------
	// Initialize label map
	{
		glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFRandomValueMap );
		glTextureParameteri( mPPTBFRandomValueMap, GL_TEXTURE_BASE_LEVEL, 0 );
		glTextureParameteri( mPPTBFRandomValueMap, GL_TEXTURE_MAX_LEVEL, 0 );
		// - allocate memory
		const GLsizei levels = 1;
		const GLenum internalFormat = GL_R32F;
		glTextureStorage2D( mPPTBFRandomValueMap, levels, internalFormat, width, height );
		// - fill data
		const GLenum format = GL_RED;
		const GLenum type = GL_FLOAT;
		//const void* data = getNoise()->getP().data();
		//glTextureSubImage2D( mPPTBFRandomValueMap, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
		// - clear texture
		std::vector< float > nullData = { 0.f };
		glClearTexImage( mPPTBFRandomValueMap, /*level*/0, format, type, nullData.data() );
	}
	//----------------------------------------------------

	// Binary structure map
	if ( mThresholdTexture )
	{
		glDeleteTextures( 1, &mPPTBFTexture );
	}
	graphicsInitialization_BinaryStructureMap();
}

/******************************************************************************
 * Initialize shader program
 ******************************************************************************/
bool PtGraphicsPPTBF::initializeShaderPrograms()
{
	bool statusOK = false;

	// Global variables
	const std::string shaderPath = PtEnvironment::mShaderPath + std::string( "/" );
	PtShaderProgram* shaderProgram = nullptr;
	PtShaderProgram::TShaderList shaders;
	std::string shaderFilename;
	std::vector< std::string > uniforms;

	// PPTBF
	// Initialize shader program
	// Initialize shader program
	shaderProgram = mShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "exemplarPaletteViewer" );
	shaderProgram->setInfo( "Exemplar Palette Viewer" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "fullscreenTexturedTriangle_vert.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eVertexShader, shaderFilename ) );
	shaderFilename = shaderPath + "fullscreenTexturedTriangle_color_frag.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eFragmentShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	uniforms.push_back( "uTexture" );
	shaderProgram->registerUniforms( uniforms );

	// Rendering: fullscreen triangle
	// Initialize shader program
	shaderProgram = mPPTBFViewerShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "pptbfViewer" );
	shaderProgram->setInfo( "PPTBF Viewer" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "fullscreenTexturedTriangle_vert.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eVertexShader, shaderFilename ) );
	shaderFilename = shaderPath + "fullscreenTexturedTriangle_frag.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eFragmentShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	uniforms.push_back( "uTexture" );
	uniforms.push_back( "uSelected" );
	shaderProgram->registerUniforms( uniforms );

	// Binary structure map
	// Initialize shader program
	shaderProgram = mBinaryStructureMapShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "binaryStructureMap" );
	shaderProgram->setInfo( "Binary Structure Map" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "fullscreenTexturedTriangle_vert.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eVertexShader, shaderFilename ) );
	shaderFilename = shaderPath + "binaryStructureMap_frag.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eFragmentShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	uniforms.push_back( "uTexture" );
	uniforms.push_back( "uThreshold" );
	uniforms.push_back( "uMinValue" );
	uniforms.push_back( "uMaxValue" );
	uniforms.push_back( "uMeanValue" );
	uniforms.push_back( "uSelected" );
	shaderProgram->registerUniforms( uniforms );

	// Megakernel approach
	// Initialize shader program
	shaderProgram = mMegakernelShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "pptbfMegakernel" );
	shaderProgram->setInfo( "PPTBF Megakernel" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "pptbf_megakernel_v2_comp.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eComputeShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	// PPTBF parameters
	// - PRNG + Noise
	uniforms.push_back( "uPermutationTex" );
	uniforms.push_back( "uNoiseTex" );
	// - deformation
	uniforms.push_back( "uTurbulenceAmplitude_0" );
	uniforms.push_back( "uTurbulenceAmplitude_1" );
	uniforms.push_back( "uTurbulenceAmplitude_2" );
	// - model transform
	uniforms.push_back( "uResolution" );
	uniforms.push_back( "uShiftX" );
	uniforms.push_back( "uShiftY" );
	uniforms.push_back( "uRotation" );
	uniforms.push_back( "uRescaleX" );
	// - point process
	uniforms.push_back( "uPointProcessTilingType" );
	uniforms.push_back( "uPointProcessJitter" );
	// - window function
	uniforms.push_back( "uWindowShape" );
	uniforms.push_back( "uWindowArity" );
	uniforms.push_back( "uWindowLarp" );
	uniforms.push_back( "uWindowNorm" );
	uniforms.push_back( "uWindowSmooth" );
	uniforms.push_back( "uWindowBlend" );
	uniforms.push_back( "uWindowSigwcell" );
	// - window function
	uniforms.push_back( "uFeatureBomb" );
	uniforms.push_back( "uFeatureNorm" );
	uniforms.push_back( "uFeatureWinfeatcorrel" );
	uniforms.push_back( "uFeatureAniso" );
	uniforms.push_back( "uFeatureNpmin" );
	uniforms.push_back( "uFeatureNpmax" );
	uniforms.push_back( "uFeatureSigcos" );
	uniforms.push_back( "uFeatureSigcosvar" );
	uniforms.push_back( "uFeatureFreq" );
	uniforms.push_back( "uFeaturePhase" );
	uniforms.push_back( "uFeatureThickness" );
	uniforms.push_back( "uFeatureCourbure" );
	uniforms.push_back( "uFeatureDeltaorient" );
	// - window function
	uniforms.push_back( "uThreshold" );
	// - labeling
	uniforms.push_back( "uNbLabels" );
	// - register uniforms
	shaderProgram->registerUniforms( uniforms );

	// Megakernel approach with threshold
	// Initialize shader program
	shaderProgram = mMegakernelThresholdShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "pptbfMegakernelThreshold" );
	shaderProgram->setInfo( "PPTBF Megakernel Threshold" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "pptbf_megakernel_threshold_v2_comp.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eComputeShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	// PPTBF parameters
	// - binary structure map
	uniforms.push_back( "uThreshold" );
	// - register uniforms
	shaderProgram->registerUniforms( uniforms );


	return statusOK;
}

/******************************************************************************
 * Callback called when PPTBF size has been modified
 ******************************************************************************/
void PtGraphicsPPTBF::onSizeModified( const int pWidth, const int pHeight )
{
	// Release graphics resources
	if ( mPPTBFFrameBuffer )
	{
		glDeleteFramebuffers( 1, &mPPTBFFrameBuffer );
	}
	if ( mPPTBFTexture )
	{
		glDeleteTextures( 1, &mPPTBFTexture );
	}
	// PPTBF label map
	if ( mPPTBFLabelMap )
	{
		glDeleteTextures( 1, &mPPTBFLabelMap );
	}
	// Random value map
	if ( mPPTBFRandomValueMap )
	{
		glDeleteTextures( 1, &mPPTBFRandomValueMap );
	}

	mWidth = pWidth;
	mHeight = pHeight;

	int width = pWidth;
	int height = pHeight;

	if ( mUseLowResolutionPPTBF )
	{
		width = mWidthLowQuality;
		height = mHeightLowQuality;
	}

	// Create device resources
	glGenFramebuffers( 1, &mPPTBFFrameBuffer );
	glGenTextures( 1, &mPPTBFTexture );
	// Initialize texture "rnd_tab"
	glBindTexture( GL_TEXTURE_2D, mPPTBFTexture );
	// - set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// - set min/max level for completeness
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	// - generate the texture
	glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, width, height, 0/*border*/, GL_RED, GL_FLOAT, nullptr );
	// - reset device state
	glBindTexture( GL_TEXTURE_2D, 0 );
	// Configure framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, mPPTBFFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPPTBFTexture, 0 );
	glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	// - check FBO status
	GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		std::cout << "Framebuffer error - status: " << fboStatus << std::endl;
		// - clean device resources
		finalize();
		// - reset device state
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		//return -1;
	}
	// - reset device state
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	//----------------------------------------------------
	// Initialize label map
	glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFLabelMap );
	glTextureParameteri( mPPTBFLabelMap, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( mPPTBFLabelMap, GL_TEXTURE_MAX_LEVEL, 0 );
	// - allocate memory
	const GLsizei levels = 1;
	const GLenum internalFormat = GL_R8UI;
	glTextureStorage2D( mPPTBFLabelMap, levels, internalFormat, width, height );
	// - fill data
	const GLenum format = GL_RED_INTEGER;
	const GLenum type = GL_UNSIGNED_BYTE; // 256 labels max
	//const void* data = getNoise()->getP().data();
	//glTextureSubImage2D( mPPTBFLabelMap, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
	// - clear texture
	std::vector< char > nullData = { 0 };
	glClearTexImage( mPPTBFLabelMap, /*level*/0, format, type, nullData.data() );
	//----------------------------------------------------

	//----------------------------------------------------
	// Initialize random value map
	{
		glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFRandomValueMap );
		glTextureParameteri( mPPTBFRandomValueMap, GL_TEXTURE_BASE_LEVEL, 0 );
		glTextureParameteri( mPPTBFRandomValueMap, GL_TEXTURE_MAX_LEVEL, 0 );
		// - allocate memory
		const GLsizei levels = 1;
		const GLenum internalFormat = GL_R32F;
		glTextureStorage2D( mPPTBFRandomValueMap, levels, internalFormat, width, height );
		// - fill data
		const GLenum format = GL_RED;
		const GLenum type = GL_FLOAT;
		//const void* data = getNoise()->getP().data();
		//glTextureSubImage2D( mPPTBFRandomValueMap, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
		// - clear texture
		std::vector< float > nullData = { 0.f };
		glClearTexImage( mPPTBFRandomValueMap, /*level*/0, format, type, nullData.data() );
	}
	//----------------------------------------------------

	// Binary structure map
	if ( mThresholdTexture )
	{
		glDeleteTextures( 1, &mThresholdTexture );
	}
	graphicsInitialization_BinaryStructureMap();
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void PtGraphicsPPTBF::finalize()
{
	if ( mTexture_P )
	{
		glDeleteTextures( 1, &mTexture_P );
	}
	if ( mTexture_rnd_tab )
	{
		glDeleteTextures( 1, &mTexture_rnd_tab );
	}
	if ( mTexture_G )
	{
		glDeleteTextures( 1, &mTexture_G );
	}
	
	// Finalize timer(s)
	graphicsFinalization_Timer();
}

/******************************************************************************
 * Initialize binary structure map
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsInitialization_BinaryStructureMap()
{
	// Retrieve PPTBF info
	const int width = getWidth();
	const int height = getHeight();
	assert( width != 0 && height != 0 );

	// Common GL parameters
	const GLint param_TEXTURE_WRAP_S = GL_CLAMP_TO_EDGE; /*GL_REPEAT*/
	const GLint param_TEXTURE_WRAP_T = GL_CLAMP_TO_EDGE; /*GL_REPEAT*/
	const GLint param_TEXTURE_MIN_FILTER = GL_NEAREST; /*GL_LINEAR*/
	const GLint param_TEXTURE_MAG_FILTER = GL_NEAREST; /*GL_LINEAR*/

	GLuint& texture = mThresholdTexture;
	glCreateTextures( GL_TEXTURE_2D, 1, &texture );
	// - set texture wrapping/filtering options
	glTextureParameteri( texture, GL_TEXTURE_WRAP_S, param_TEXTURE_WRAP_S );
	glTextureParameteri( texture, GL_TEXTURE_WRAP_T, param_TEXTURE_WRAP_T );
	glTextureParameteri( texture, GL_TEXTURE_MIN_FILTER, param_TEXTURE_MIN_FILTER );
	glTextureParameteri( texture, GL_TEXTURE_MAG_FILTER, param_TEXTURE_MAG_FILTER );
	// - set min/max level for completeness
	glTextureParameteri( texture, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( texture, GL_TEXTURE_MAX_LEVEL, 0 );
	// - allocate memory
	glTextureStorage2D( texture, 1/*levels*/, GL_R32F, width, height );
	// - clear texture
	float nullData = 0.f;
	glClearTexImage( texture, 0, GL_RED, GL_FLOAT, &nullData );
}

/******************************************************************************
 * Load a texture
 *
 * @param pFilename image
 ******************************************************************************/
bool PtGraphicsPPTBF::loadTexture( const char* pFilename,
	const int pNbRequestedChannels, const GLenum pInternalFormat, const GLenum pFormat, const GLenum pType,
	const bool pGenerateMipmap,
	GLuint& pTexture,
	int& pWidth, int& pHeight )
{
	// Open file
	std::ifstream file;
	file.open( pFilename );
	if ( ! file.is_open() )
	{
		std::cout << "ERROR: file cannot be opened: " << std::string( pFilename ) << std::endl;
		assert( false );
		return false;
	}

	// Load image
	int width, height, nrChannels;
	const std::string imageFilename = pFilename;
	unsigned char* data = nullptr;
	const int desired_channels = pNbRequestedChannels; // force that many components per pixel
	PtImageHelper::loadImage( imageFilename.c_str(), width, height, nrChannels, data, desired_channels );
	if ( data == nullptr )
	{
		std::cout << "Failed to load image: " << imageFilename << std::endl;
		assert( false );
		return false;
	}

	// Store txeture info
	pWidth = width;
	pHeight = height;
	
	// Compute the number of mipmap levels corresponding to given width and height of an image
	const int nbMipmapLevels = getNbMipmapLevels( width, height );

	// Release graphics resources
	glDeleteTextures( 1, &pTexture );
		
	// Initialize texture
	glCreateTextures( GL_TEXTURE_2D, 1, &pTexture );
	// - set the texture wrapping/filtering options (on the currently bound texture object)
	glTextureParameteri( pTexture, GL_TEXTURE_WRAP_S, /*GL_REPEAT*/GL_CLAMP_TO_EDGE );
	glTextureParameteri( pTexture, GL_TEXTURE_WRAP_T, /*GL_REPEAT*/GL_CLAMP_TO_EDGE );
	glTextureParameteri( pTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTextureParameteri( pTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// - set min/max level for completeness
	glTextureParameteri( pTexture, GL_TEXTURE_BASE_LEVEL, 0 );
	GLint textureMaxLevel = 0;
	if ( pGenerateMipmap )
	{
		textureMaxLevel = nbMipmapLevels - 1;
	}
	glTextureParameteri( pTexture, GL_TEXTURE_MAX_LEVEL, textureMaxLevel );
#if 1
	// - deal with odd texture dimensions
	glBindTexture( GL_TEXTURE_2D, pTexture );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
#endif
	// - allocate memory
	GLsizei levels = 1;
	if ( pGenerateMipmap )
	{
		levels = nbMipmapLevels;
	}
	glTextureStorage2D( pTexture, levels, pInternalFormat, width, height );
	// - fill data
	glTextureSubImage2D( pTexture, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, pFormat, pType, data );
	
	// Mipmap generation
	if ( pGenerateMipmap )
	{
		glGenerateTextureMipmap( pTexture );
	}

	// - object name
	const std::string objectLabel = "PtGraphicsSynthesizer::exemplar";
	glObjectLabel( GL_TEXTURE, pTexture, sizeof( objectLabel ), objectLabel.data() );

	// Free memory
	PtImageHelper::freeImage( data );

	return true;
}

/******************************************************************************
 * Initialize PRNG (permutation texture)
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsInitialization_PRNG()
{
	// Initialize texture
	GLuint& texture = mTexture_P;
	glCreateTextures( GL_TEXTURE_2D, 1, &texture );
	// - set the texture wrapping/filtering options (on the currently bound texture object)
	glTextureParameteri( texture, GL_TEXTURE_WRAP_S, GL_REPEAT/*GL_CLAMP_TO_EDGE*/ );
	glTextureParameteri( texture, GL_TEXTURE_WRAP_T, GL_REPEAT/*GL_CLAMP_TO_EDGE*/ );
	glTextureParameteri( texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTextureParameteri( texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	// - set min/max level for completeness
	glTextureParameteri( texture, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( texture, GL_TEXTURE_MAX_LEVEL, 0 );
#if 0
	// - deal with odd texture dimensions
	glBindTexture( GL_TEXTURE_2D, texture );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
#endif
	// - allocate memory
	const GLsizei levels = 1;
	const GLenum internalFormat = GL_R32I;
	const GLsizei width = static_cast< GLsizei >( getNoise()->getP().size() );
	const GLsizei height = 1;
	glTextureStorage2D( texture, levels, internalFormat, width, height );
	// - fill data
	const GLenum format = GL_RED_INTEGER;
	const GLenum type = GL_INT;
	const void* data = getNoise()->getP().data();
	glTextureSubImage2D( texture, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
}

/******************************************************************************
 * Initialize noise
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsInitialization_Noise()
{
	// Initialize sampler
	GLuint& sampler = mSampler_PRNG_Noise;
	glCreateSamplers( 1, &sampler );
	// - set the sampler wrapping/filtering options
	glSamplerParameteri( sampler, GL_TEXTURE_WRAP_S, GL_REPEAT/*GL_CLAMP_TO_EDGE*/ );
	glSamplerParameteri( sampler, GL_TEXTURE_WRAP_T, GL_REPEAT/*GL_CLAMP_TO_EDGE*/ );
	glSamplerParameteri( sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	// Initialize texture "rnd_tab"
	graphicsInitialization_Noise_rnd_tab();

	// Initialize texture "G"
	graphicsInitialization_Noise_G();
}

/******************************************************************************
 * Initialize noise
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsInitialization_Noise_rnd_tab()
{
	// Initialize texture "rnd_tab"
	GLuint& texture = mTexture_rnd_tab;
	glCreateTextures( GL_TEXTURE_2D, 1, &texture );
	// - set min/max level for completeness
	glTextureParameteri( texture, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( texture, GL_TEXTURE_MAX_LEVEL, 0 );
#if 0
	// - deal with odd texture dimensions
	glBindTexture( GL_TEXTURE_2D, texture );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
#endif
	// - allocate memory
	const GLsizei levels = 1;
	const GLenum internalFormat = GL_R32F;
	const GLsizei width = static_cast< GLsizei >( getNoise()->getRndTab().size() );
	const GLsizei height = 1;
	glTextureStorage2D( texture, levels, internalFormat, width, height );
	// - fill data
	const GLenum format = GL_RED;
	const GLenum type = GL_FLOAT;
	const void* data = getNoise()->getRndTab().data();
	glTextureSubImage2D( texture, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
}

/******************************************************************************
 * Initialize noise
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsInitialization_Noise_G()
{
	// Initialize texture "G"
	GLuint& texture = mTexture_G;
	glCreateTextures( GL_TEXTURE_2D, 1, &texture );
	// - set min/max level for completeness
	glTextureParameteri( texture, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( texture, GL_TEXTURE_MAX_LEVEL, 0 );
#if 0
	// - deal with odd texture dimensions
	glBindTexture( GL_TEXTURE_2D, texture );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
#endif
	// - allocate memory
	const GLsizei levels = 1;
	const GLenum internalFormat = GL_RGB32F;
	const GLsizei width = static_cast< GLsizei >( getNoise()->getG().size() );
	const GLsizei height = 1;
	glTextureStorage2D( texture, levels, internalFormat, width, height );
	// - fill data
	const GLenum format = GL_RGB;
	const GLenum type = GL_FLOAT;
	const void* data = getNoise()->getG().data();
	glTextureSubImage2D( texture, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, width, height, format, type, data );
}

/******************************************************************************
 * Initialize timer(s)
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsInitialization_Timer()
{
	// Device timer
	glCreateQueries( GL_TIME_ELAPSED, 1, &mQueryTimeElapsed );
}

/******************************************************************************
 * Finalize timer(s)
 ******************************************************************************/
void PtGraphicsPPTBF::graphicsFinalization_Timer()
{
	// Device timer
	glDeleteQueries( 1, &mQueryTimeElapsed );
}

/******************************************************************************
 * Launch PPTBF computation
 ******************************************************************************/
void PtGraphicsPPTBF::compute()
{
	static int counter = 0;
	std::cout << "synthesis: " << counter << std::endl;
	counter++;

	// GPU timer
	GLuint64 result = 0;
	GLuint64 totalTime = 0;
	mSynthesisInfo = std::stringstream(); // NOTE: don't use clear() => it is not what you may think...

	// Timer
	glBeginQuery( GL_TIME_ELAPSED, mQueryTimeElapsed );

	// Execute the mega-kernel approach
	executeMegakernelApproach();
		
	// Timer
	glEndQuery( GL_TIME_ELAPSED );
	glGetQueryObjectui64v( mQueryTimeElapsed, GL_QUERY_RESULT, &result );
	totalTime += result;
	// LOG info
	mSynthesisInfo << "    Mega-Kernel          ";
	mSynthesisInfo << "\t" << std::fixed << std::setw( 9 ) << std::setprecision( 3 ) << std::setfill( ' ' ) << ( ( totalTime ) * 1.e-6 ) << " ms\n";
	// Update internal performance counter
	mSynthesisTime = static_cast< float >( totalTime * 1.e-6 );
	// LOG info
	std::cout << "\tTOTAL: ";
	std::cout << "\t" << std::fixed << std::setw( 9 ) << std::setprecision( 3 ) << std::setfill( ' ' ) << ( totalTime * 1.e-6 ) << " ms\n";
}

/******************************************************************************
 * Launch PPTBF computation
 ******************************************************************************/
void PtGraphicsPPTBF::compute( const int pOutputID )
{
	static int counter = 0;
	std::cout << "synthesis: " << counter << std::endl;
	counter++;

	// GPU timer
	GLuint64 result = 0;
	GLuint64 totalTime = 0;
	mSynthesisInfo = std::stringstream(); // NOTE: don't use clear() => it is not what you may think...

	// Timer
	glBeginQuery( GL_TIME_ELAPSED, mQueryTimeElapsed );

	// Execute the mega-kernel approach
	executeMegakernelApproach( pOutputID );
		
	// Timer
	glEndQuery( GL_TIME_ELAPSED );
	glGetQueryObjectui64v( mQueryTimeElapsed, GL_QUERY_RESULT, &result );
	totalTime += result;
	// LOG info
	mSynthesisInfo << "    Mega-Kernel          ";
	mSynthesisInfo << "\t" << std::fixed << std::setw( 9 ) << std::setprecision( 3 ) << std::setfill( ' ' ) << ( ( totalTime ) * 1.e-6 ) << " ms\n";
	// Update internal performance counter
	mSynthesisTime = static_cast< float >( totalTime * 1.e-6 );
	// LOG info
	std::cout << "\tTOTAL: ";
	std::cout << "\t" << std::fixed << std::setw( 9 ) << std::setprecision( 3 ) << std::setfill( ' ' ) << ( totalTime * 1.e-6 ) << " ms\n";
}

/******************************************************************************
 * Execute the mega-kernel approach
 ******************************************************************************/
void PtGraphicsPPTBF::executeMegakernelApproach()
{
	// Compute PPTBF
	// Megakernel approach
	// Set shader program
	//PtShaderProgram* shaderProgram = editPPTBFGeneratorShaderProgram();

	PtShaderProgram* shaderProgram = nullptr;

	/*switch ( mRenderingMode )
	{
		case ERenderingMode::ePPTBF:
			shaderProgram = mMegakernelShaderProgram;
			break;

		case ERenderingMode::eBinaryMap:
			shaderProgram = mMegakernelThresholdShaderProgram;
			break;

		default:
			shaderProgram = mMegakernelShaderProgram;
			break;
	}*/
	shaderProgram = mMegakernelShaderProgram;

	shaderProgram->use();
	{
		// Set texture(s)
#if 1
		glBindTextureUnit( 0/*unit*/, mTexture_P );
		glBindTextureUnit( 1/*unit*/, mTexture_rnd_tab );
#else
		GLuint textures[ 3 ] = { mTexture_P , mTexture_G, mTexture_rnd_tab };
		glBindTextures( 0/*first*/, 3/*count*/, textures );
#endif

		// Set sampler(s)
#if 1
		glBindSampler( 0/*unit*/, mSampler_PRNG_Noise );
		glBindSampler( 1/*unit*/, mSampler_PRNG_Noise );
#else
		std::vector< GLuint > samplers( 3, mSampler_PRNG_Noise );
		glBindSamplers( 0/*first*/, 3/*count*/, samplers.data() );
#endif

		// Set image(s)
		// - PPTBF (output)
		glBindImageTexture( 0/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
		// - labeling
		glBindImageTexture( 1/*unit*/, mPPTBFLabelMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R8UI );
		// - labeling
		glBindImageTexture( 2/*unit*/, mPPTBFRandomValueMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );

		// Set uniform(s)
		// - PRNG
		shaderProgram->set( 0, "uPermutationTex" );
		// - noise
		shaderProgram->set( 1, "uNoiseTex" );
		// - deformation
		shaderProgram->set( getTurbulenceAmplitude0(), "uTurbulenceAmplitude_0" );
		shaderProgram->set( getTurbulenceAmplitude1(), "uTurbulenceAmplitude_1" );
		shaderProgram->set( getTurbulenceAmplitude2(), "uTurbulenceAmplitude_2" );
		// - model transform
		shaderProgram->set( getResolution(), "uResolution" );
		shaderProgram->set( getAlpha(), "uRotation" );
		shaderProgram->set( getRescalex(), "uRescaleX" );
		shaderProgram->set( getShiftX(), "uShiftX" );
		shaderProgram->set( getShiftY(), "uShiftY" );
		// - point process
		shaderProgram->set( getTilingType(), "uPointProcessTilingType" );
		shaderProgram->set( getJittering(), "uPointProcessJitter" );
		// - window function
		shaderProgram->set( getWindowShape(), "uWindowShape" );
		shaderProgram->set( getWindowArity(), "uWindowArity" );
		shaderProgram->set( getWindowLarp(), "uWindowLarp" );
		shaderProgram->set( getWindowNorm(), "uWindowNorm" );
		shaderProgram->set( getWindowSmooth(), "uWindowSmooth" );
		shaderProgram->set( getWindowBlend(), "uWindowBlend" );
		shaderProgram->set( getWindowSigwcell(), "uWindowSigwcell" );
		// - feature function
		shaderProgram->set( getBombingFlag(), "uFeatureBomb" );
		shaderProgram->set( getFeatureNorm(), "uFeatureNorm" );
		shaderProgram->set( getFeatureWinfeatcorrel(), "uFeatureWinfeatcorrel" );
		shaderProgram->set( getFeatureAnisotropy(), "uFeatureAniso" );
		shaderProgram->set( getMinNbGaborKernels(), "uFeatureNpmin" );
		shaderProgram->set( getMaxNbGaborKernels(), "uFeatureNpmax" );
		shaderProgram->set( getGaborDecay(), "uFeatureSigcos" );
		shaderProgram->set( getGaborDecayJittering(), "uFeatureSigcosvar" );
		shaderProgram->set( getGaborStripesFrequency(), "uFeatureFreq" );
		shaderProgram->set( getFeaturePhaseShift(), "uFeaturePhase" );
		shaderProgram->set( getGaborStripesThickness(), "uFeatureThickness" );
		shaderProgram->set( getGaborStripesCurvature(), "uFeatureCourbure" );
		shaderProgram->set( getGaborStripesOrientation(), "uFeatureDeltaorient" );
		// - labeling
		shaderProgram->set( getNbLabels(), "uNbLabels" );
		
		// Launch kernel
		// - block
		const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
		const int blockSizeY = 8;
		// - grid
		const int width = getWidth();
		const int height = getHeight();
		assert(width != 0 && height != 0);
		const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
		const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );
		// - kernel
		glDispatchCompute( gridSizeX, gridSizeY, 1 );

		// Reset GL state(s)
		//glBindTexture( GL_TEXTURE_2D, 0 );
	}
	PtShaderProgram::unuse();

	// Synchronization
	// - make sure writing to image has finished before read
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
	glMemoryBarrier( GL_ALL_BARRIER_BITS );
}

/******************************************************************************
 * Execute the mega-kernel approach
 ******************************************************************************/
void PtGraphicsPPTBF::executeMegakernelApproach( const int pOutputID )
{
	// Compute PPTBF
	// Megakernel approach
	// Set shader program
	//PtShaderProgram* shaderProgram = editPPTBFGeneratorShaderProgram();

	PtShaderProgram* shaderProgram = nullptr;

	/*switch ( mRenderingMode )
	{
		case ERenderingMode::ePPTBF:
			shaderProgram = mMegakernelShaderProgram;
			break;

		case ERenderingMode::eBinaryMap:
			shaderProgram = mMegakernelThresholdShaderProgram;
			break;

		default:
			shaderProgram = mMegakernelShaderProgram;
			break;
	}*/
	shaderProgram = mMegakernelShaderProgram;

	shaderProgram->use();
	{
		// Set texture(s)
#if 1
		glBindTextureUnit( 0/*unit*/, mTexture_P );
		glBindTextureUnit( 1/*unit*/, mTexture_rnd_tab );
#else
		GLuint textures[ 3 ] = { mTexture_P , mTexture_G, mTexture_rnd_tab };
		glBindTextures( 0/*first*/, 3/*count*/, textures );
#endif

		// Set sampler(s)
#if 1
		glBindSampler( 0/*unit*/, mSampler_PRNG_Noise );
		glBindSampler( 1/*unit*/, mSampler_PRNG_Noise );
#else
		std::vector< GLuint > samplers( 3, mSampler_PRNG_Noise );
		glBindSamplers( 0/*first*/, 3/*count*/, samplers.data() );
#endif

		// Set image(s)
		// - PPTBF (output)
		//glBindImageTexture( 0/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
		glBindImageTexture( 0/*unit*/, mPPTBFTextureLists[ pOutputID ], 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
		// - labeling
		glBindImageTexture( 1/*unit*/, mPPTBFLabelMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R8UI );
		// - labeling
		glBindImageTexture( 2/*unit*/, mPPTBFRandomValueMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );

		// Set uniform(s)
		// - PRNG
		shaderProgram->set( 0, "uPermutationTex" );
		// - noise
		shaderProgram->set( 1, "uNoiseTex" );
		// - deformation
		shaderProgram->set( getTurbulenceAmplitude0(), "uTurbulenceAmplitude_0" );
		shaderProgram->set( getTurbulenceAmplitude1(), "uTurbulenceAmplitude_1" );
		shaderProgram->set( getTurbulenceAmplitude2(), "uTurbulenceAmplitude_2" );
		// - model transform
		shaderProgram->set( getResolution(), "uResolution" );
		shaderProgram->set( getAlpha(), "uRotation" );
		shaderProgram->set( getRescalex(), "uRescaleX" );
		shaderProgram->set( getShiftX(), "uShiftX" );
		shaderProgram->set( getShiftY(), "uShiftY" );
		// - point process
		shaderProgram->set( getTilingType(), "uPointProcessTilingType" );
		shaderProgram->set( getJittering(), "uPointProcessJitter" );
		// - window function
		shaderProgram->set( getWindowShape(), "uWindowShape" );
		shaderProgram->set( getWindowArity(), "uWindowArity" );
		shaderProgram->set( getWindowLarp(), "uWindowLarp" );
		shaderProgram->set( getWindowNorm(), "uWindowNorm" );
		shaderProgram->set( getWindowSmooth(), "uWindowSmooth" );
		shaderProgram->set( getWindowBlend(), "uWindowBlend" );
		shaderProgram->set( getWindowSigwcell(), "uWindowSigwcell" );
		// - feature function
		shaderProgram->set( getBombingFlag(), "uFeatureBomb" );
		shaderProgram->set( getFeatureNorm(), "uFeatureNorm" );
		shaderProgram->set( getFeatureWinfeatcorrel(), "uFeatureWinfeatcorrel" );
		shaderProgram->set( getFeatureAnisotropy(), "uFeatureAniso" );
		shaderProgram->set( getMinNbGaborKernels(), "uFeatureNpmin" );
		shaderProgram->set( getMaxNbGaborKernels(), "uFeatureNpmax" );
		shaderProgram->set( getGaborDecay(), "uFeatureSigcos" );
		shaderProgram->set( getGaborDecayJittering(), "uFeatureSigcosvar" );
		shaderProgram->set( getGaborStripesFrequency(), "uFeatureFreq" );
		shaderProgram->set( getFeaturePhaseShift(), "uFeaturePhase" );
		shaderProgram->set( getGaborStripesThickness(), "uFeatureThickness" );
		shaderProgram->set( getGaborStripesCurvature(), "uFeatureCourbure" );
		shaderProgram->set( getGaborStripesOrientation(), "uFeatureDeltaorient" );
		// - labeling
		shaderProgram->set( getNbLabels(), "uNbLabels" );
		
		// Launch kernel
		// - block
		const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
		const int blockSizeY = 8;
		// - grid
		const int width = getWidth();
		const int height = getHeight();
		assert(width != 0 && height != 0);
		const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
		const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );
		// - kernel
		glDispatchCompute( gridSizeX, gridSizeY, 1 );

		// Reset GL state(s)
		//glBindTexture( GL_TEXTURE_2D, 0 );
	}
	PtShaderProgram::unuse();

	// Synchronization
	// - make sure writing to image has finished before read
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
	glMemoryBarrier( GL_ALL_BARRIER_BITS );
}

/******************************************************************************
 * Compute binary structure map
 ******************************************************************************/
void PtGraphicsPPTBF::computeBinaryStructureMap()
{
	PtShaderProgram* shaderProgram = mMegakernelThresholdShaderProgram;

	shaderProgram->use();
	{
		// Set image(s)
		// - PPTBF (output)
		glBindImageTexture( 0/*unit*/, mThresholdTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
		// - binary structure map
		glBindImageTexture( 1/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_READ_ONLY, GL_R32F );

		// Set uniform(s)
		// - threshold
		shaderProgram->set( getThreshold(), "uThreshold" );
			
		// Launch kernel
		// - block
		const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
		const int blockSizeY = 8;
		// - grid
		const int width = getWidth();
		const int height = getHeight();
		assert(width != 0 && height != 0);
		const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
		const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );
		// - kernel
		glDispatchCompute( gridSizeX, gridSizeY, 1 );

		// Reset GL state(s)
		//glBindTexture( GL_TEXTURE_2D, 0 );
	}
	PtShaderProgram::unuse();

	// Synchronization
	// - make sure writing to image has finished before read
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
	glMemoryBarrier( GL_ALL_BARRIER_BITS );
}

/******************************************************************************
 * Render PPTBF
 ******************************************************************************/
void PtGraphicsPPTBF::render()
{
	if ( mRenderingMode == ERenderingMode::ePPTBF )
	{
	}

#if 0
	// Set shader program
	PtShaderProgram* shaderProgram = mPPTBFViewerShaderProgram;
	shaderProgram->use();
	{
		// Set texture(s)
		//glBindTextureUnit( 0/*unit*/, mPPTBFTexture );
		// TEST: display separable pass
		// - window function
		//const GLuint* const windowCellularTextureID = mWindowCellular->getTextureID();
		//glBindTextureUnit( 0/*unit*/, *windowCellularTextureID );
		//// - feature function
		//const GLuint* const featureTextureID = mFeatureManager->getTextureID();
		//glBindTextureUnit( 0/*unit*/, *featureTextureID );
		// - PPTBF (compositing)
		glBindTextureUnit( 0/*unit*/, mPPTBFTexture );

		// Set uniform(s)
		shaderProgram->set( 0, "uTexture" ); // Note: this uniform never changes => can be set after shaderprogram::link()
	
		 // Draw command(s)
		glDrawArrays( GL_TRIANGLES, 0, 3 );
	}
	PtShaderProgram::unuse();

	// TEST: render HUD
	//mPointProcess->render();
#else
	//---------------------------------------------------
	// TEST: color map
	//---------------------------------------------------

	// Set shader program
	PtShaderProgram* shaderProgram = mColormapShaderProgram;
	shaderProgram->use();
	{
		// Set texture(s)
		// - PPTBF (compositing)
		glBindTextureUnit( 0/*unit*/, mPPTBFTexture );
		// - color map
		//glBindTextureUnit( 1/*unit*/, mColormapTexture );

		// Set uniform(s)
		shaderProgram->set( 0, "uPPTBFTex" ); // Note: this uniform never changes => can be set after shaderprogram::link()
		shaderProgram->set( 1, "uColormapTex" );
		//shaderProgram->set( getColormapIndex(), "uColormapIndex" );
				
		 // Draw command(s)
		glDrawArrays( GL_TRIANGLES, 0, 3 );
	}
	PtShaderProgram::unuse();
#endif
}

/******************************************************************************
 * Flag to tell whether or not to use full FBO resolution (slow "screen size" or faster "640x480" for editing)
 ******************************************************************************/
bool PtGraphicsPPTBF::useLowResolutionPPTBF() const
{
	return mUseLowResolutionPPTBF;
}

/******************************************************************************
 * Flag to tell whether or not to use full FBO resolution (slow "screen size" or faster "640x480" for editing)
 ******************************************************************************/
void PtGraphicsPPTBF::setUseLowResolutionPPTBF( const bool pFlag )
{
	mUseLowResolutionPPTBF = pFlag;

	mUpdateDeformation = true;
	mUpdatePointProcess = true;
	mUpdateWindowCellular = true;
	mUpdateFeature = true;

	onSizeModified( mWidth, mHeight );
}

/******************************************************************************
 * Generate database
 ******************************************************************************/
void PtGraphicsPPTBF::generateDatabase_test( const unsigned int pWidth, const unsigned int pHeight, const char* pPath )
{
	assert( pWidth != 0 && pHeight != 0 );

	// LOG info
	std::cout << "\nPPTBF Database generation..." << std::endl;
	std::cout << "START..." << std::endl;

	const int width = pWidth;
	const int height = pHeight;

	// Resize graphics resources
	onSizeModified( width, height );

	//const std::string databasePath = PtEnvironment::mDataPath + std::string( "/BDDStructure/Test/" );
	const std::string databasePath = pPath;

	// Deformation
	std::vector< float > turbulenceAmplitude0Array;
	std::vector< float > turbulenceAmplitude1Array;
	std::vector< float > turbulenceAmplitude2Array;

	// Model Transform
	std::vector< int > modelResolutionArray;
	std::vector< float > modelAlphaArray;
	std::vector< float > modelRescalexArray;
	
	// Point process
	std::vector< int > tilingTypeArray;
	std::vector< float > jitteringArray;
	
	// Window function
	std::vector< int > windowShapeArray;
	std::vector< float > windowArityArray;
	std::vector< float > windowLarpArray;
	std::vector< float > windowNormArray;
	std::vector< float > windowSmoothArray;
	std::vector< float > windowBlendArray;
	std::vector< float > windowSigwcellArray;
	
	// Feature function
	std::vector< int > featureBombingArray;
	std::vector< float > featureNormArray;
	std::vector< float > featureWinfeatcorrelArray;
	std::vector< float > featureAnisoArray;
	std::vector< int > featureMinNbKernelsArray;
	std::vector< int > featureMaxNbKernelsArray;
	std::vector< float > featureSigcosArray;
	std::vector< float > featureSigcosvarArray;
	std::vector< int > featureFrequencyArray;
	std::vector< float > featurePhaseShiftArray;
	std::vector< float > featureThicknessArray;
	std::vector< float > featureCurvatureArray;
	std::vector< float > featureOrientationArray;
	
	///////////////////////////////////////////
	// USER Experiment
	// - sampling the space of PPTBF structures
	///////////////////////////////////////////

#if 0

	// Deformation
	turbulenceAmplitude0Array.push_back( 0.f );
	turbulenceAmplitude1Array.push_back( 0.f );
	turbulenceAmplitude2Array.push_back( 0.f );

	// Model Transform
	modelResolutionArray.push_back( 100 );
	modelAlphaArray.push_back( 0.f * M_PI );
	modelRescalexArray.push_back( 1.f );

	// Point process
	tilingTypeArray.push_back( 4 );
	jitteringArray.push_back( 0.f );

	// Window function
	windowShapeArray.push_back( 2 );
	windowArityArray.push_back( 6.f );
	windowLarpArray.push_back( 0.f );
	windowNormArray.push_back( 2.f );
	windowSmoothArray.push_back( 0.f );
	windowBlendArray.push_back( 1.f );
	windowSigwcellArray.push_back( 0.1f );

	// Feature function
	featureBombingArray.push_back( 0 );
	featureNormArray.push_back( 2.f );
	featureWinfeatcorrelArray.push_back( 0.f );
	featureAnisoArray.push_back( 10.f );
	featureMinNbKernelsArray.push_back( 1 );
	featureMaxNbKernelsArray.push_back( 2 );
	featureSigcosArray.push_back( 1.f );
	featureSigcosvarArray.push_back( 0.1f );
	featureFrequencyArray.push_back( 0 );
	featurePhaseShiftArray.push_back( 0.f );
	featureThicknessArray.push_back( 0.01f );
	featureCurvatureArray.push_back( 0.f );
	featureOrientationArray.push_back( M_PI / 2.f );

#else

	//////////////
	// Deformation
	//////////////

	// Parameter 0
	for ( float param_0 = 0.0f; param_0 <= 1.0f; param_0 += 0.25f )
	{
		turbulenceAmplitude0Array.push_back( param_0 );
	}

	// Parameter 1
	for ( float param_1 = 0.0f; param_1 <= 1.0f; param_1 += 0.25f )
	{
		turbulenceAmplitude1Array.push_back( param_1 );
	}

	// Parameter 2
	for ( float param_2 = 0.0f; param_2 <= 1.0f; param_2 += 0.25f )
	{
		turbulenceAmplitude2Array.push_back( param_2 );
	}

	//////////////////
	// Model Transform
	//////////////////

	// Resolution
	for ( int resolution = 100; resolution <= 1000; resolution += 100 )
	{
		modelResolutionArray.push_back( resolution );
	}

	// Alpha
	for ( float alpha = 0.f; alpha <= 2.f; alpha += 0.25f )
	{
		modelAlphaArray.push_back( alpha * M_PI );
	}

	// Rescale X
	for ( float rescalex = 1.0f; rescalex <= 4.f; rescalex += 0.5f )
	{
		modelRescalexArray.push_back( rescalex );
	}

	////////////////
	// Point Process
	////////////////

	// Tiling type
	//tilingTypeArray.resize( 18 );
	//std::iota( tilingTypeArray.begin(), tilingTypeArray.end(), 0 );
	//for ( int tileid = 0; tileid <= 17; tileid++ )
	for ( int tileid = 4; tileid <= 17; tileid++ )
	{
		tilingTypeArray.push_back( tileid );
	}

	// Jittering
	for ( float jitter = 0.0f; jitter <= 1.0f; jitter += 0.8f )
	{
		jitteringArray.push_back( jitter );
	}

	//////////////////
	// WINDOW FUNCTION
	//////////////////

	// Shape
	//for ( int shape = 0; shape <= 3; shape += 1 )
	for ( int shape = 2; shape <= 3; shape += 1 )
	{
		windowShapeArray.push_back( shape );
	}

	// Arity
	//for ( float windowArity = 2.f; windowArity <= 6.f; windowArity += 1.f )
	for ( float windowArity = 6.f; windowArity <= 6.f; windowArity += 1.f )
	{
		windowArityArray.push_back( windowArity );
	}

	// Larp
	for ( float windowLarp = 0.f; windowLarp <= 1.f; windowLarp += 0.5f )
	{
		windowLarpArray.push_back( windowLarp );
	}

	// Norm
	//for ( float windowNorm = 1.f; windowNorm <= 3.f; windowNorm += 1.f )
	for ( float windowNorm = 2.f; windowNorm <= 3.f; windowNorm += 1.f )
	{
		windowNormArray.push_back( windowNorm );
	}

	// Smooth
	for ( float windowSmooth = 0.f; windowSmooth <= 2.f; windowSmooth += 1.f )
	{
		windowSmoothArray.push_back( windowSmooth );
	}

	// Blend
	//for ( float windowBlend = 0.f; windowBlend <= 1.f; windowBlend += 0.25f )
	for ( float windowBlend = 1.f; windowBlend <= 1.f; windowBlend += 0.25f )
	{
		windowBlendArray.push_back( windowBlend );
	}

	// Sigwcell
	//for ( float windowSigwcell = 0.f; windowSigwcell <= 1.f; windowSigwcell += 0.25f )
	for ( float windowSigwcell = 0.1f; windowSigwcell <= 1.f; windowSigwcell += 0.25f )
	{
		windowSigwcellArray.push_back( windowSigwcell );
	}

	///////////////////
	// Feature Function
	///////////////////

	// Bombing
	for ( int bombing = 0; bombing <= 3; bombing += 1 )
	{
		featureBombingArray.push_back( bombing );
	}

	// Norm
	//for ( float norm = 1.0; norm <= 3.0; norm += 1.0 )
	for ( float norm = 2.0; norm <= 3.0; norm += 1.0 )
	{
		featureNormArray.push_back( norm );
	}

	// Winfeatcorrel
	for ( float winfeatcorrel = 0.0; winfeatcorrel <= 10.0; winfeatcorrel += 1.0 )
	{
		featureWinfeatcorrelArray.push_back( winfeatcorrel );
	}

	// Anisotropy
	//for ( float anisotropy = 1.0; anisotropy <= 10.0; anisotropy += 1.0 )
	for ( float anisotropy = 10.f; anisotropy <= 10.0; anisotropy += 1.0 )
	{
		featureAnisoArray.push_back( anisotropy );
	}

	// Min/Max number of kernels
	for ( int Npmin = 1; Npmin <= 8; Npmin += 4 )
	{
		featureMinNbKernelsArray.push_back( Npmin );
		for ( int Npmax = Npmin; Npmin <= 8; Npmin += 4 )
		{
			featureMaxNbKernelsArray.push_back( Npmax );
		}
	}

	// Sigcos
	//for ( float featureSigcos = 0.1f; featureSigcos <= 1.f; featureSigcos += 0.5f )
	for ( float featureSigcos = 1.f; featureSigcos <= 1.f; featureSigcos += 0.5f )
	{
		featureSigcosArray.push_back( featureSigcos );
	}

	// Sigcosvar
	for ( float featureSigcosvar = 0.1f; featureSigcosvar <= 1.f; featureSigcosvar += 0.5f )
	{
		featureSigcosvarArray.push_back( featureSigcosvar );
	}

	// Frequency
	for ( int ifreq = 0; ifreq <= 3; ifreq += 1 )
	{
		featureFrequencyArray.push_back( ifreq );
	}

	// Phase
	for ( float phase = 0.0; phase <= 2.0; phase += 2.0 )
	{
		featurePhaseShiftArray.push_back( phase * M_PI );
	}

	// Thickness
	//for ( float thickness = 0.1; thickness <= 1.0; thickness += 0.4 )
	for ( float thickness = 0.01f; thickness <= 1.0; thickness += 0.4 )
	{
		featureThicknessArray.push_back( thickness );
	}

	// Curvature
	for ( float curvature = 0.0; curvature <= 0.4; curvature += 0.2 )
	{
		featureCurvatureArray.push_back( curvature );
	}

	// Orientation
	for ( float deltaorient = 0.0; deltaorient <= M_PI / 2.0; deltaorient += M_PI / 4.0 )
	{
		featureOrientationArray.push_back( deltaorient );
	}

#endif
	
	///////////////////////
	// Set common GL states
	///////////////////////

	//setImageWidth( width );
	//setImageHeight( height );
	setWidth( width );
	setHeight( height );

	// Set shader program
	PtShaderProgram* shaderProgram = mMegakernelShaderProgram;
	shaderProgram->use();

	// Set texture(s)
	// - PRNG
	glBindTextureUnit( 0/*unit*/, mTexture_P );
	// - noise
	glBindTextureUnit( 1/*unit*/, mTexture_rnd_tab );

	// Set sampler(s)
	// - PRNG
	glBindSampler( 0/*unit*/, mSampler_PRNG_Noise );
	// - noise
	glBindSampler( 1/*unit*/, mSampler_PRNG_Noise );

	// Set image(s)
	// - PPTBF (output)
	glBindImageTexture( 0/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );

	// Set uniform(s)
	// - PRNG
	shaderProgram->set( 0, "uPermutationTex" );
	// - noise
	shaderProgram->set( 1, "uNoiseTex" );

	// Kernel configuration
	// - block
	const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
	const int blockSizeY = 8;
	// - grid
	const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
	const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );

	/////////////////////////////////////////
	// Sampling the space of PPTBF structures
	/////////////////////////////////////////
	
	// Global PPTBF counter
	int i = 0;

	// Deformation
	int d_0 = 0;
	int d_1 = 0;
	int d_2 = 0;
	// Model Transform
	int m_0 = 0;
	int m_1 = 0;
	int m_2 = 0;
	// Point process
	int p_0 = 0;
	int p_1 = 0;
	// Window function
	int w_0 = 0;
	int w_1 = 0;
	int w_2 = 0;
	int w_3 = 0;
	int w_4 = 0;
	int w_5 = 0;
	int w_6 = 0;
	// Feature function
	int f_0 = 0;
	int f_1 = 0;
	int f_2 = 0;
	int f_3 = 0;
	int f_4 = 0;
	int f_5 = 0;
	int f_6 = 0;
	int f_7 = 0;
	int f_8 = 0;
	int f_9 = 0;
	int f_10 = 0;
	int f_11 = 0;
	int f_12 = 0;

	std::size_t nbSamples = 1;
	// Deformation
	nbSamples *= turbulenceAmplitude0Array.size();
	nbSamples *= turbulenceAmplitude1Array.size();
	nbSamples *= turbulenceAmplitude2Array.size();
	// Model Transform
	nbSamples *= modelResolutionArray.size();
	nbSamples *= modelAlphaArray.size();
	nbSamples *= modelRescalexArray.size();
	// Point process
	nbSamples *= tilingTypeArray.size();
	nbSamples *= jitteringArray.size();
	// Window function
	nbSamples *= windowShapeArray.size();
	nbSamples *= windowArityArray.size();
	nbSamples *= windowLarpArray.size();
	nbSamples *= windowNormArray.size();
	nbSamples *= windowSmoothArray.size();
	nbSamples *= windowBlendArray.size();
	nbSamples *= windowSigwcellArray.size();
	// Feature function
	nbSamples *= featureBombingArray.size();
	nbSamples *= featureNormArray.size();
	nbSamples *= featureWinfeatcorrelArray.size();
	nbSamples *= featureAnisoArray.size();
	nbSamples *= featureMinNbKernelsArray.size();
	nbSamples *= featureMaxNbKernelsArray.size();
	nbSamples *= featureSigcosArray.size();
	nbSamples *= featureSigcosvarArray.size();
	nbSamples *= featureFrequencyArray.size();
	nbSamples *= featurePhaseShiftArray.size();
	nbSamples *= featureThicknessArray.size();
	nbSamples *= featureCurvatureArray.size();
	nbSamples *= featureOrientationArray.size();

	//////////////
	// Deformation
	//////////////

	d_0 = 0;
	for ( const auto deformation_0 : turbulenceAmplitude0Array )
	{
		shaderProgram->set( deformation_0, "uTurbulenceAmplitude_0" );

	d_1 = 0;
	for ( const auto deformation_1 : turbulenceAmplitude1Array )
	{
		shaderProgram->set( deformation_1, "uTurbulenceAmplitude_1" );

	d_2 = 0;
	for ( const auto deformation_2 : turbulenceAmplitude2Array )
	{
		shaderProgram->set( deformation_2, "uTurbulenceAmplitude_2" );

	//////////////////
	// Model Transform
	//////////////////

	// Default translation
	shaderProgram->set( 0.f, "uShiftX" );
	shaderProgram->set( 0.f, "uShiftY" );

	m_0 = 0;
	for ( const auto modelResolution : modelResolutionArray )
	{
		shaderProgram->set( modelResolution, "uResolution" );

	m_1 = 0;
	for ( const auto modelAlpha : modelAlphaArray )
	{
		shaderProgram->set( modelAlpha, "uRotation" );

	m_2 = 0;
	for ( const auto modelRescalex : modelRescalexArray )
	{
		shaderProgram->set( modelRescalex, "uRescaleX" );

	////////////////
	// Point Process
	////////////////
	
	p_0 = 0;
	for ( const auto tilingType : tilingTypeArray )
	{
		shaderProgram->set( tilingType, "uPointProcessTilingType" );

	p_1 = 0;
	for ( const auto jittering : jitteringArray )
	{
		shaderProgram->set( jittering, "uPointProcessJitter" );
	
	//////////////////
	// Window Function
	//////////////////

	w_0 = 0;
	for ( const auto windowShape : windowShapeArray )
	{
		shaderProgram->set( windowShape, "uWindowShape" );

	w_1 = 0;
	for ( const auto windowArity : windowArityArray )
	{
		shaderProgram->set( windowArity, "uWindowArity" );

	w_2 = 0;
	for ( const auto windowLarp : windowLarpArray )
	{
		shaderProgram->set( windowLarp, "uWindowLarp" );

	w_3 = 0;
	for ( const auto windowNorm : windowNormArray )
	{
		shaderProgram->set( windowNorm, "uWindowNorm" );

	w_4 = 0;
	for ( const auto windowSmooth : windowSmoothArray )
	{
		shaderProgram->set( windowSmooth, "uWindowSmooth" );

	w_5 = 0;
	for ( const auto windowBlend : windowBlendArray )
	{
		shaderProgram->set( windowBlend, "uWindowBlend" );

	w_6 = 0;
	for ( const auto windowSigwcell : windowSigwcellArray )
	{
		shaderProgram->set( windowSigwcell, "uWindowSigwcell" );

	///////////////////
	// Feature Function
	///////////////////

	/*
	f_0 = 0;
	for ( const auto featureBombing : featureBombingArray )
	{
		shaderProgram->set( featureBombing, "uFeatureBomb" );

	f_1 = 0;
	for ( const auto featureNorm : featureNormArray )
	{
		shaderProgram->set( featureNorm, "uFeatureNorm" );

	f_2 = 0;
	for ( const auto featureWinfeatcorrel : featureWinfeatcorrelArray )
	{
		shaderProgram->set( featureWinfeatcorrel, "uFeatureWinfeatcorrel" );

	f_3 = 0;
	for ( const auto featureAniso : featureAnisoArray )
	{
		shaderProgram->set( featureAniso, "uFeatureAniso" );

	f_4 = 0;
	for ( const auto minNbGaborKernels : featureMinNbKernelsArray )
	{
		shaderProgram->set( minNbGaborKernels, "uFeatureNpmin" );

	f_5 = 0;
	for ( const auto maxNbGaborKernels : featureMaxNbKernelsArray )
	{
		shaderProgram->set( maxNbGaborKernels, "uFeatureNpmax" );

	f_6 = 0;
	for ( const auto featureSigcos : featureSigcosArray )
	{
		shaderProgram->set( featureSigcos, "uFeatureSigcos" );

	f_7 = 0;
	for ( const auto featureSigcosvar : featureSigcosvarArray )
	{
		shaderProgram->set( featureSigcosvar, "uFeatureSigcosvar" );

	f_8 = 0;
	for ( const auto gaborStripesFrequency : featureFrequencyArray )
	{
		shaderProgram->set( gaborStripesFrequency, "uFeatureFreq" );

	f_9 = 0;
	for ( const auto featurePhase : featurePhaseShiftArray )
	{
		shaderProgram->set( featurePhase, "uFeaturePhase" );

	f_10 = 0;
	for ( const auto gaborStripesThickness : featureThicknessArray )
	{
		shaderProgram->set( gaborStripesThickness, "uFeatureThickness" );

	f_11 = 0;
	for ( const auto gaborStripesCurvature : featureCurvatureArray )
	{
		shaderProgram->set( gaborStripesCurvature, "uFeatureCourbure" );

	f_12 = 0;
	for ( const auto gaborStripesOrientation : featureOrientationArray )
	{
		shaderProgram->set( gaborStripesOrientation, "uFeatureDeltaorient" );*/
		
		//---------------------
		// Generate PPTBF image
		//---------------------

		std::cout << "generating image: " << ( i + 1 ) << "/" << nbSamples << std::endl;
		
		// Launch mega-kernel
		glDispatchCompute( gridSizeX, gridSizeY, 1 );
		
		// Synchronization
		// - make sure writing to image has finished before read
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
		glMemoryBarrier( GL_ALL_BARRIER_BITS );

		//-------------------
		// Export PPTBF image
		//-------------------
								
		// Retrieve data on host
		std::vector< float > f_pptbf( width * height );
#if 1
		// - deal with odd texture dimensions
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
#endif
		glGetTextureImage( mPPTBFTexture, 0/*level*/, GL_RED, GL_FLOAT, sizeof( float ) * width * height, f_pptbf.data() );
		// Convert data (float to unsigned char)
		// TODO: maybe use for_each with a lambda doing the "cast" directly instead?
		std::transform( f_pptbf.begin(), f_pptbf.end(), f_pptbf.begin(), std::bind( std::multiplies< float >(), std::placeholders::_1, 255.f ) );
		std::vector< unsigned char > u_pptbf( f_pptbf.begin(), f_pptbf.end() );
		// Save data in image
		//const std::string pptbfName = std::string( "pptbf_" )/*name*/ + std::to_string( i );
		const std::string pptbfName = std::string( "pptbf" )/*name*/
									+ std::string( "_D_" ) + std::to_string( d_0 ) + std::string( "_" ) + std::to_string( d_1 ) + std::string( "_" ) + std::to_string( d_2 )
									+ std::string( "_M_" ) + std::to_string( m_0 ) + std::string( "_" ) + std::to_string( m_1 ) + std::string( "_" ) + std::to_string( m_2 )
									+ std::string( "_P_" ) + std::to_string( p_0 ) + std::string( "_" ) + std::to_string( p_1 )
									+ std::string( "_W_" ) + std::to_string( w_0 ) + std::string( "_" ) + std::to_string( w_1 ) + std::string( "_" ) + std::to_string( w_2 )
														   + std::string( "_" ) + std::to_string( w_3 ) + std::string( "_" ) + std::to_string( w_4 ) + std::string( "_" ) + std::to_string( w_5 )
														   + std::string( "_" ) + std::to_string( w_6 )
									+ std::string( "_F_" ) + std::to_string( f_0 ) + std::string( "_" ) + std::to_string( f_1 ) + std::string( "_" ) + std::to_string( f_2 )
														   + std::string( "_" ) + std::to_string( f_3 ) + std::string( "_" ) + std::to_string( f_4 ) + std::string( "_" ) + std::to_string( f_5 )
														   + std::string( "_" ) + std::to_string( f_6 ) + std::string( "_" ) + std::to_string( f_7 ) + std::string( "_" ) + std::to_string( f_8 )
														   + std::string( "_" ) + std::to_string( f_9 ) + std::string( "_" ) + std::to_string( f_10 ) + std::string( "_" ) + std::to_string( f_11 )
														   + std::string( "_" ) + std::to_string( f_12 );
		const std::string pptbfFilename = databasePath + pptbfName + std::string( ".png" );
		PtImageHelper::saveImage( pptbfFilename.c_str(), width, height, 1/*nb channels*/, u_pptbf.data() );
		
		// Update counter
		i++;
	/*}
	}
	}
	}
	}
	}
	}
	}
	}
	}
	}
	}
	}*/
		w_6++;
	}
		w_5++;
	}
		w_4++;
	}
		w_3++;
	}
		w_2++;
	}
		w_1++;
	}
		w_0++;
	}
		p_1++;
	}
		p_0++;
	}
		m_2++;
	}
		m_1++;
	}
		m_0++;
	}
		d_2++;
	}
		d_1++;
	}
		d_0++;
	}

	// Reset GL state(s)
	// - Unset shader program
	PtShaderProgram::unuse();

	// LOG info
	std::cout << "Finished..." << nbSamples << std::endl;
}

/******************************************************************************
 * Generate database
 ******************************************************************************/
void PtGraphicsPPTBF::generateDatabase( const unsigned int pWidth, const unsigned int pHeight, const char* pPath, const unsigned int pSerieID )
{
	assert(pWidth != 0 && pHeight != 0);

	// LOG info
	std::cout << "\nPPTBF Database generation..." << std::endl;
	std::cout << "START..." << std::endl;

	const int width = pWidth;
	const int height = pHeight;

	// Resize graphics resources
	onSizeModified(width, height);

	//const std::string databasePath = PtEnvironment::mDataPath + std::string( "/BDDStructure/Test/" );
	const std::string databasePath = pPath;

	// Deformation
	std::vector< float > turbulenceAmplitude0Array;
	std::vector< float > turbulenceAmplitude1Array;
	std::vector< float > turbulenceAmplitude2Array;

	// Model Transform
	std::vector< int > modelResolutionArray;
	std::vector< float > modelAlphaArray;
	std::vector< float > modelRescalexArray;

	// Point process
	std::vector< int > tilingTypeArray;
	std::vector< float > jitteringArray;

	// Window function
	std::vector< int > windowShapeArray;
	std::vector< float > windowArityArray;
	std::vector< float > windowLarpArray;
	std::vector< float > windowNormArray;
	std::vector< float > windowSmoothArray;
	std::vector< float > windowBlendArray;
	std::vector< float > windowSigwcellArray;

	// Feature function
	std::vector< int > featureBombingArray;
	std::vector< float > featureNormArray;
	std::vector< float > featureWinfeatcorrelArray;
	std::vector< float > featureAnisoArray;
	std::vector< int > featureMinNbKernelsArray;
	std::vector< int > featureMaxNbKernelsArray;
	std::vector< float > featureSigcosArray;
	std::vector< float > featureSigcosvarArray;
	std::vector< int > featureFrequencyArray;
	std::vector< float > featurePhaseShiftArray;
	std::vector< float > featureThicknessArray;
	std::vector< float > featureCurvatureArray;
	std::vector< float > featureOrientationArray;

	///////////////////////////////////////////
	// USER Experiment
	// - sampling the space of PPTBF structures
	///////////////////////////////////////////

#if 0

	// Deformation
	turbulenceAmplitude0Array.push_back(0.f);
	turbulenceAmplitude1Array.push_back(0.f);
	turbulenceAmplitude2Array.push_back(0.f);

	// Model Transform
	modelResolutionArray.push_back(100);
	modelAlphaArray.push_back(0.f * M_PI);
	modelRescalexArray.push_back(1.f);

	// Point process
	tilingTypeArray.push_back(4);
	jitteringArray.push_back(0.f);

	// Window function
	windowShapeArray.push_back(2);
	windowArityArray.push_back(6.f);
	windowLarpArray.push_back(0.f);
	windowNormArray.push_back(2.f);
	windowSmoothArray.push_back(0.f);
	windowBlendArray.push_back(1.f);
	windowSigwcellArray.push_back(0.1f);

	// Feature function
	featureBombingArray.push_back(0);
	featureNormArray.push_back(2.f);
	featureWinfeatcorrelArray.push_back(0.f);
	featureAnisoArray.push_back(10.f);
	featureMinNbKernelsArray.push_back(1);
	featureMaxNbKernelsArray.push_back(2);
	featureSigcosArray.push_back(1.f);
	featureSigcosvarArray.push_back(0.1f);
	featureFrequencyArray.push_back(0);
	featurePhaseShiftArray.push_back(0.f);
	featureThicknessArray.push_back(0.01f);
	featureCurvatureArray.push_back(0.f);
	featureOrientationArray.push_back(M_PI / 2.f);

#else

	//int descriptor = 2;
	int descriptor = 1;
	//const int KEEPNBEST = 1;
	const int KEEPNBEST = 5;
	//const int NR = 100;
	const int NR = 10;
	float deltaalpha = 0.0; // 0.5 = pi/2
	float factresol = 1.0;
	float factrescalex = 1.0;
	float ecart = 0.5;
	//char *exname = "TexturesCom_Asphalt11_2x2_1K_seg_scrop_80";
	//char *exname = "TexturesCom_Pavement_CobblestoneForest01_4x4_1K_albedo_seg_scrop";
	char *exname = "20181215_153153_seg_scrop_450";
	
	const float KEEP_ERROR = 0.0;
	const int SS = 400;
	char pname[128];
	int pptbf_count = 0;
	bool withwindowfile = true;
	int qthresh = 0;
	float percent = 0.2;

	int series_val[17 + 5 * 5 + 3 * 17][4] = {
		{0, 1, 0, 0},{ 1, 1, 0, 0 },{ 2, 1, 0, 0 },{ 3, 1, 0, 0 },{ 4, 1, 0, 0 },{ 5, 1, 0, 0 },{ 6, 1, 0, 0 },{ 7, 1, 0, 0 },{ 8, 1, 0, 0 },{ 9, 1, 0, 0 },{ 10, 1, 0, 0 },{ 11, 1, 0, 0 },{ 12, 1, 0, 0 },{ 13, 1, 0, 0 },{ 14, 1, 0, 0 },{ 15, 1, 0, 0 },{ 16, 1, 0, 0 },
		{ 0, 0, 1, 0 },{ 1, 0, 1, 0 },{ 7, 0, 1, 0 },{ 10, 0, 1, 0 },{ 14, 0, 1, 0 },
		{ 0, 0, 2, 0 },{ 1, 0, 2, 0 },{ 7, 0, 2, 0 },{ 10, 0, 2, 0 },{ 14, 0, 2, 0 },
		{ 0, 0, 3, 0 },{ 1, 0, 3, 0 },{ 7, 0, 3, 0 },{ 10, 0, 3, 0 },{ 14, 0, 3, 0 },
		{ 0, 0, 4, 0 },{ 1, 0, 4, 0 },{ 7, 0, 4, 0 },{ 10, 0, 4, 0 },{ 14, 0, 4, 0 },
		{ 0, 0, 2, 1 },{ 1, 0, 2, 1 },{ 7, 0, 2, 1 },{ 10, 0, 2, 1 },{ 14, 0, 2, 1 },
		//{ 0, 0, 4, 1 },{ 1, 0, 4, 1 },{ 7, 0, 4, 1 },{ 10, 0, 4, 1 },{ 14, 0, 4, 1 },
		{ 0, 1, 1, 0 },{ 1, 1, 1, 0 },{ 2, 1, 1, 0 },{ 3, 1, 1, 0 },{ 4, 1, 1, 0 },{ 5, 1, 1, 0 },{ 6, 1, 1, 0 },{ 7, 1, 1, 0 },{ 8, 1, 1, 0 },{ 9, 1, 1, 0 },{ 10, 1, 1, 0 },{ 11, 1, 1, 0 },{ 12, 1, 1, 0 },{ 13, 1, 1, 0 },{ 14, 1, 1, 0 },{ 15, 1, 1, 0 },{ 16, 1, 1, 0 },
		{ 0, 1, 2, 0 },{ 1, 1, 2, 0 },{ 2, 1, 2, 0 },{ 3, 1, 2, 0 },{ 4, 1, 2, 0 },{ 5, 1, 2, 0 },{ 6, 1, 2, 0 },{ 7, 1, 2, 0 },{ 8, 1, 2, 0 },{ 9, 1, 2, 0 },{ 10, 1, 2, 0 },{ 11, 1, 2, 0 },{ 12, 1, 2, 0 },{ 13, 1, 2, 0 },{ 14, 1, 2, 0 },{ 15, 1, 2, 0 },{ 16, 1, 2, 0 },
		{ 0, 1, 2, 1 },{ 1, 1, 2, 1 },{ 2, 1, 2, 1 },{ 3, 1, 2, 1 },{ 4, 1, 2, 1 },{ 5, 1, 2, 1 },{ 6, 1, 2, 1 },{ 7, 1, 2, 1 },{ 8, 1, 2, 1 },{ 9, 1, 2, 1 },{ 10, 1, 2, 1 },{ 11, 1, 2, 1 },{ 12, 1, 2, 1 },{ 13, 1, 2, 1 },{ 14, 1, 2, 1 },{ 15, 1, 2, 1 },{ 16, 1, 2, 1 }
	};

	//for (int do_serie = 0; do_serie <= 0; do_serie++)
	//{
	//int do_serie = 17 + 5 * 5 + 2*17 + 16; // 17 + 5 * 5 + 0 * 17;
	int do_serie = pSerieID; // TODO: check bounds

	int do_tiling = series_val[do_serie][0]; // 0;  // 0 - 17
	int do_window = series_val[do_serie][1]; //1;  // 0 or 1
	int do_feature = series_val[do_serie][2]; // 0; // 0 - 4
	int do_phase = series_val[do_serie][3]; //0;  // 0 or 1


	float jittering, wjittering, fjittering;
	int resolution, wresolution, fresolution;
	float rotation, wrotation, frotation;
	float rescalex, wrescalex, frescalex;
	float turbAmplitude0, turbAmplitude1, turbAmplitude2;
	float wturbAmplitude0, wturbAmplitude1, wturbAmplitude2;
	float fturbAmplitude0, fturbAmplitude1, fturbAmplitude2;

	int windowShape = do_window, wwindowShape, fwindowShape;
	float windowArity = 10.0, wwindowArity, fwindowArity;
	float windowLarp = 0.0, wwindowLarp, fwindowLarp;
	float windowNorm = 2.0, wwindowNorm, fwindowNorm;
	float windowSmoothness = 0.0, wwindowSmoothness, fwindowSmoothness;
	float windowBlend = 1.0, wwindowBlend, fwindowBlend;
	float windowSigwcell = 0.7, wwindowSigwcell, fwindowSigwcell;

	int featureBombing = 0, wfeatureBombing, ffeatureBombing;
	float featureNorm = 2.0, wfeatureNorm, ffeatureNorm;
	float featureWinfeatcorrel = 0.0, wfeatureWinfeatcorrel, ffeatureWinfeatcorrel;
	float featureAniso = 0.0, wfeatureAniso, ffeatureAniso;
	int featureMinNbKernels = 0, featureMaxNbKernels = 0;
	int wfeatureMinNbKernels = 0, wfeatureMaxNbKernels = 0;
	int ffeatureMinNbKernels = 0, ffeatureMaxNbKernels = 0;
	float featureSigcos = 1.0, wfeatureSigcos, ffeatureSigcos;
	float featureSigcosvar = 0.2, wfeatureSigcosvar, ffeatureSigcosvar;
	int featureFrequency = 0, wfeatureFrequency, ffeatureFrequency;
	float featurePhaseShift = (float)do_phase, wfeaturePhaseShift, ffeaturePhaseShift;
	float featureThickness = 1.0, wfeatureThickness, ffeatureThickness;
	float featureCurvature = 0.0, wfeatureCurvature, ffeatureCurvature;
	float featureOrientation = 0.0, wfeatureOrientation, ffeatureOrientation;
#ifdef REFINEMENT
	sprintf(pname, "%s/pptbf_matching_rnd_%d.txt", pPath, descriptor);
#else
	sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
#endif
	FILE *fdn = fopen(pname, "w");
#ifndef REFINEMENT
	fprintf(fdn, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");
#endif

#ifdef PROBE
	int tresolution;
	float tturbAmplitude0;

	sprintf(pname, "%s/gen_pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	FILE *fdg = fopen(pname, "w");

	sprintf(pname, "%s/pptbf_probe_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	FILE *fd = fopen(pname, "w");
	fprintf(fd, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");

	// only window function, featurefunct=cst, windowBlend=1
	int resolutionArray[] = { 50 / 2, 150 / 2 };
	//int resolutionArray[] = { 50, 150 };
	if (!withwindowfile)
	{
		if (do_tiling == 2 || do_tiling >= 10) for (int i = 0; i < 2; i++) resolutionArray[i] *= 2;
		for (int iresolution = 0; iresolution <= 0; iresolution++)
			//for (int iresolution = 0; iresolution <= 1; iresolution++)
		{
			resolution = resolutionArray[iresolution];
			tresolution = resolution * 2;
			float zoom = (float)SS / (float)resolutionArray[iresolution];
			float rescalexArray[] = { 1.0, 3.0, 8.0 };
			//for (int irescalex = 0; irescalex <= 2; irescalex++)
			for (int irescalex = 0; irescalex <= 0; irescalex++)
			{
				rescalex = rescalexArray[irescalex];
				if (rescalex >= 5.0) resolution /= 2;
				else if (rescalex >= 2.5) resolution = resolution * 2 / 3;
				float rotstep = 1.0 / 8.0;
				//for (rotation = 0.0; rotation <= 1.0 / 8.0; rotation += rotstep)
				for (rotation = 0.0; rotation <= 0.0 / 8.0; rotation += rotstep)
				{

					float turbAmplitude0A[] = { 0.0, 0.06,  0.05, 0.03 };
					float turbAmplitude1A[] = { 0.0, 0.05, 0.5, 0.8 };
					float turbAmplitude2A[] = { 1.0, 1.0, 1.0, 1.0 };
					for (int iturbAmplitude = 0; iturbAmplitude <= 0; iturbAmplitude++)
					{
						turbAmplitude0 = turbAmplitude0A[iturbAmplitude] * (float)resolution / (50.0 / 3.0);
						tturbAmplitude0 = turbAmplitude0A[iturbAmplitude] * pow((float)tresolution / 50.0, 1.6);
						turbAmplitude1 = turbAmplitude1A[iturbAmplitude];
						turbAmplitude2 = turbAmplitude2A[iturbAmplitude];

						float mjittering = 1.0;
						//if (do_window == 0) mjittering = 0.01;
						for (jittering = (do_serie < 42 ? 0.01 : 0.4); jittering <= mjittering; jittering += 0.45)
						{
							fprintf(fdg, "resol=%d, rescal=%g, rot=%g, iturbAmplitude=%d, jit=%g\n\n", resolution, rescalex, rotation, iturbAmplitude, jittering);

							///////////////////////////////////////
							// WINDOW FUNCTION ONLY, FEATURE is constant
							windowShape = do_window;
							featureBombing = do_feature;
							featureNorm = 2.0;
							featureWinfeatcorrel = 0.0;
							featureAniso = 0.0;
							featureMinNbKernels = 0, featureMaxNbKernels = 0;
							featureSigcos = 3.0;
							featureSigcosvar = 0.2;
							featureFrequency = 0;
							featurePhaseShift = (float)do_phase;
							featureThickness = 1.0;
							featureCurvature = 0.0;
							featureOrientation = 0.0;

							//float windowBlendA[] = { 1.0, 0.5, 0.1 };
							float windowBlendA[] = { 1.0, 0.5, 0.1 };

							if (do_feature == 0 && do_window == 1)
								for (int iwindowBlend = 0; iwindowBlend <= 2; iwindowBlend += 1)
								{
									windowBlend = windowBlendA[iwindowBlend];
									fprintf(fdg, "windowBlend=%g\n", windowBlend);
									int featureBombingA[] = { 0, 2, 5 };
									for (int ifeatureBombing = 0; ifeatureBombing <= 0; ifeatureBombing++)
										//for (int ifeatureBombing = 0; ifeatureBombing <= (windowBlend>0.5?2:1); ifeatureBombing++)
									{
										featureBombing = featureBombingA[ifeatureBombing];
										//if (featureBombing == 2) featurePhaseShift = 1.0;
										//else featurePhaseShift = 0.0;

										int lfeatureMinNbKernels = 0;
										if (featureBombing == 5) lfeatureMinNbKernels = 2;
										int efeatureMinNbKernels = 0;
										int sfeatureMinNbKernels = 1;
										if (featureBombing == 2) efeatureMinNbKernels = 2;
										else if (featureBombing == 5) efeatureMinNbKernels = 4;
										if (featureBombing == 5) sfeatureMinNbKernels = 3;

										for (featureMinNbKernels = lfeatureMinNbKernels; featureMinNbKernels <= efeatureMinNbKernels; featureMinNbKernels += 2)
											for (featureMaxNbKernels = featureMinNbKernels; featureMaxNbKernels <= featureMinNbKernels; featureMaxNbKernels += 4)
												//for (featureMaxNbKernels = featureMinNbKernels + sfeatureMinNbKernels; featureMaxNbKernels <= featureMinNbKernels + sfeatureMinNbKernels; featureMaxNbKernels += 4)
											{
												fprintf(fdg, "featureMinNbKernels=%g\n", featureMinNbKernels);
												for (featureWinfeatcorrel = 0.0; featureWinfeatcorrel <= 0.0; featureWinfeatcorrel += 0.6)
													//for (featureWinfeatcorrel = (featureBombing == 5 ? 0.5 : 0.0); featureWinfeatcorrel <= (featureBombing == 5 ? 0.5 : 0.0); featureWinfeatcorrel += 0.3)
												{
													fprintf(fdg, "featureWinfeatcorrel=%g\n", featureWinfeatcorrel);
													float lfeatureSigcos = 4.0;
													float efeatureSigcos = 4.0;
													if (featureBombing == 2) efeatureSigcos = 8.0;
													else if (featureBombing == 5) { lfeatureSigcos = 0.5; efeatureSigcos = 0.5; }
													for (featureSigcos = lfeatureSigcos; featureSigcos <= efeatureSigcos; featureSigcos += 4.0)
													{
														fprintf(fdg, "featureSigcos=%g\n", featureSigcos);
														float windowArityA[] = { 2.0, 3.0, 4.0, 8.0 };
														for (int iwindowArity = (windowBlend <= 0.5 ? 3 : 0); iwindowArity <= 3; iwindowArity++)
														{
															windowArity = windowArityA[iwindowArity];
															fprintf(fdg, "windowArity=%g\n", windowArity);
															float lwindowSmoothness = 2.0;
															if (windowArity >= 6.0) lwindowSmoothness = 0.0;
															else if (windowArity >= 5.0) lwindowSmoothness = 1.0;
															for (windowSmoothness = 0.0; windowSmoothness <= lwindowSmoothness; windowSmoothness += 1.0)
															{
																fprintf(fdg, "windowSmoothness=%g\n", windowSmoothness);
																float lwindowLarp = 1.0;
																float windowstepLarp = 0.25;
																if (windowArity <= 3.0) lwindowLarp = 0.0;
																else if (windowArity <= 5.0) lwindowLarp = 0.25;
																else windowstepLarp = 0.3;
																for (windowLarp = 0.0; windowLarp <= lwindowLarp; windowLarp += windowstepLarp)
																{
																	fprintf(fdg, "windowLarp=%g\n", windowLarp);
																	for (windowNorm = (windowBlend < 0.5 ? 3.0 : 2.0); windowNorm >= (windowLarp == 0.0 && windowArity > 5.0 ? 1.0 : 2.0); windowNorm -= 1.0)
																	{
																		fprintf(fdg, "windowNorm=%g\n", windowNorm);
																		for (windowSigwcell = 1.0; windowSigwcell <= 1.7; windowSigwcell += 1.0)
																		{
																			fprintf(fdg, "windowSigwcell=%g\n", windowSigwcell);
																			fprintf(fdg, "------------> do image %06d\n", pptbf_count);
																			fprintf(fd, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, tresolution, rotation, rescalex, tturbAmplitude0, turbAmplitude1, turbAmplitude2,
																				windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
																				featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
																			pptbf_count++;

																			// Deformation
																			turbulenceAmplitude0Array.push_back(turbAmplitude0);
																			turbulenceAmplitude1Array.push_back(turbAmplitude1);
																			turbulenceAmplitude2Array.push_back(turbAmplitude2);

																			// Model Transform
																			modelResolutionArray.push_back(resolution);
																			modelAlphaArray.push_back(rotation * M_PI);
																			modelRescalexArray.push_back(rescalex);

																			// Point process
																			tilingTypeArray.push_back(do_tiling);
																			jitteringArray.push_back(jittering);

																			// Window function
																			windowShapeArray.push_back(windowShape);
																			windowArityArray.push_back(windowArity);
																			windowLarpArray.push_back(windowLarp);
																			windowNormArray.push_back(windowNorm);
																			windowSmoothArray.push_back(windowSmoothness);
																			windowBlendArray.push_back(windowBlend);
																			windowSigwcellArray.push_back(windowSigwcell);

																			// Feature function
																			featureBombingArray.push_back(featureBombing);
																			featureNormArray.push_back(featureNorm);
																			featureWinfeatcorrelArray.push_back(featureWinfeatcorrel);
																			featureAnisoArray.push_back(featureAniso);
																			featureMinNbKernelsArray.push_back(featureMinNbKernels);
																			featureMaxNbKernelsArray.push_back(featureMaxNbKernels);
																			featureSigcosArray.push_back(featureSigcos);
																			featureSigcosvarArray.push_back(featureSigcosvar);
																			featureFrequencyArray.push_back(featureFrequency);
																			featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
																			featureThicknessArray.push_back(featureThickness);
																			featureCurvatureArray.push_back(featureCurvature);
																			featureOrientationArray.push_back(M_PI * featureOrientation);
																		}//windowSigwcell
																	}//windowNorm
																}//windowLarp
															}//windowSmoothness
														}//windowArity

													}//featureSigcos
												}//featureWinfeatcorrel
											}//featureMinNbKernels
									}//featureBombing
								}//windowBlend

							///////////////////////////////////////
							// FEATURE FUNCTION only, Window is constant, resulting in blending (windowShape=0)
							windowShape = do_window;
							windowArity = 10.0;
							windowLarp = 0.0;
							windowNorm = 2.0;
							windowSmoothness = 0.0;
							windowBlend = 0.001;
							windowSigwcell = 0.5;
							featurePhaseShift = (float)do_phase;
							featureWinfeatcorrel = 0.0;
							//for (featureBombing = 2; featureBombing <= 2; featureBombing++)
							featureBombing = do_feature;

							if (do_feature > 0 && do_window == 0) //for (featureBombing = 1; featureBombing <= 4; featureBombing++)
							{
								float mfeatureAniso = 5.0;
								if (featureBombing == 5) mfeatureAniso = 0.0;
								else if (featureBombing == 4) mfeatureAniso = 2.5;
								for (featureAniso = 0.0; featureAniso <= mfeatureAniso; featureAniso += 2.5)
								{
									fprintf(fdg, "featureAniso=%g\n", featureAniso);
									int featureFrequencyA[] = { 0, 1, 2, 4, 8, 16, 32 };
									//for (int ifeatureFrequency = 2; ifeatureFrequency <= (featureBombing == 1 || featureBombing == 2 ? 6 : 0); ifeatureFrequency++)
									//for (int ifeatureFrequency = 0; ifeatureFrequency <= (featureBombing == 1 || featureBombing == 2 ? 4 : 0); ifeatureFrequency++)
									for (int ifeatureFrequency = 0; ifeatureFrequency <= 4; ifeatureFrequency++)
									{
										featureFrequency = featureFrequencyA[ifeatureFrequency];
										fprintf(fdg, "featureFrequency=%d\n", featureFrequency);
										for (featureNorm = (featureAniso >= 2.5 || featureBombing == 5 || featureBombing == 1 || featureBombing == 2 ? 2.0 : 1.0); featureNorm <= (featureAniso != 0.0 || featureBombing == 5 || featureBombing == 1 || featureBombing == 2 ? 2.0 : 3.0); featureNorm += 1.0)
										{
											fprintf(fdg, "featureNorm=%g\n", featureNorm);
											float lfeatureSigcos = (featureFrequency > 1 ? 1.0 : 2.0);
											float mfeatureSigcos = 1.0;
											float sfeatureSigcos = 1.0;
											if (featureBombing == 3)
											{
												lfeatureSigcos = 10.0;
												if (featureAniso > 4.0) { mfeatureSigcos = 10.0; }
												else if (featureAniso > 3.0) { mfeatureSigcos = 5.0; sfeatureSigcos = 4.0; }
												else if (featureAniso > 2.0) { mfeatureSigcos = 3.0; sfeatureSigcos = 3.0; }
												else if (featureAniso > 1.0) { mfeatureSigcos = 1.0; sfeatureSigcos = 3.0; }
												else { mfeatureSigcos = 0.75; sfeatureSigcos = 0.25; lfeatureSigcos = 1.25; }
											}
											else if (featureBombing == 4)
											{
												mfeatureSigcos = 0.25;
												lfeatureSigcos = 1.0;
												sfeatureSigcos = 0.75;
											}
											else if (featureBombing == 5)
											{
												mfeatureSigcos = 0.6;
												lfeatureSigcos = 1.0;
												sfeatureSigcos = 0.4;
											}
											for (featureSigcos = mfeatureSigcos; featureSigcos <= lfeatureSigcos; featureSigcos += sfeatureSigcos)
											{
												fprintf(fdg, "featureSigcos=%g\n", featureSigcos);
												for (featureMinNbKernels = (featureBombing == 5 ? 2 : 1); featureMinNbKernels <= (featureBombing == 5 ? 10 : 9); featureMinNbKernels += 4)
													for (featureMaxNbKernels = featureMinNbKernels; featureMaxNbKernels <= featureMinNbKernels; featureMaxNbKernels += 4)
													{
														fprintf(fdg, "featureMinNbKernels=%d\n", featureMinNbKernels);
														float featureCurvatureA[] = { 0.0, 0.2, 1.0 };
														//for (int ifeatureCurvature = 2; ifeatureCurvature <= (ifeatureFrequency == 0 ? 0 : 2); ifeatureCurvature++)
														for (int ifeatureCurvature = 0; ifeatureCurvature <= (ifeatureFrequency == 0 ? 0 : 2); ifeatureCurvature++)
														{
															featureCurvature = featureCurvatureA[ifeatureCurvature];
															fprintf(fdg, "featureCurvature=%g\n", featureCurvature);
															float ffeatureThickness = 0.1;
															if (ifeatureFrequency == 0) ffeatureThickness = 1.0;
															else if (featureFrequency > 10) ffeatureThickness = 0.5;
															if (featureBombing == 5) ffeatureThickness = 0.1;
															// 	for (featureThickness = (featureBombing == 5 ? 0.15 : 1.0); featureThickness >= ffeatureThickness; featureThickness -= (featureBombing == 5 ? 0.05 : 0.45))
															for (featureThickness = (featureBombing == 5 ? 0.15 : 1.0); featureThickness >= ffeatureThickness; featureThickness -= (featureBombing == 5 ? 0.05 : 0.9))
															{
																fprintf(fdg, "featureThickness=%g\n", featureThickness);
																float ffeatureOrientation = 0.5;
																if (ifeatureFrequency == 0 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																if (featureCurvature > 0.9 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																if (featureBombing >= 3) ffeatureOrientation = 0.5;
																if ((featureBombing == 3 || featureBombing == 4) && featureNorm == 2.0 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																if ((featureBombing == 4) && featureNorm == 2.0 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																if (featureBombing == 5) ffeatureOrientation = 0.0;
																for (featureOrientation = 0.0; featureOrientation <= ffeatureOrientation; featureOrientation += 0.15)
																{
																	fprintf(fdg, "featureOrientation=%g\n", featureOrientation);
																	for (featureWinfeatcorrel = 0.0; featureWinfeatcorrel <= 0.1; featureWinfeatcorrel += 0.5)
																	{
																		fprintf(fdg, "featureWinfeatcorrel=%g\n", featureWinfeatcorrel);
																		fprintf(fdg, "------------> do image %06d\n", pptbf_count);
																		//if (featureBombing == 5) printf("at %d\n", pptbf_count);
																		fprintf(fd, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, tresolution, rotation, rescalex, tturbAmplitude0, turbAmplitude1, turbAmplitude2,
																			windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
																			featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
																		pptbf_count++;
																		// Deformation
																		turbulenceAmplitude0Array.push_back(turbAmplitude0);
																		turbulenceAmplitude1Array.push_back(turbAmplitude1);
																		turbulenceAmplitude2Array.push_back(turbAmplitude2);

																		// Model Transform
																		modelResolutionArray.push_back(resolution);
																		modelAlphaArray.push_back(rotation * M_PI);
																		modelRescalexArray.push_back(rescalex);

																		// Point process
																		tilingTypeArray.push_back(do_tiling);
																		jitteringArray.push_back(jittering);

																		// Window function
																		windowShapeArray.push_back(windowShape);
																		windowArityArray.push_back(windowArity);
																		windowLarpArray.push_back(windowLarp);
																		windowNormArray.push_back(windowNorm);
																		windowSmoothArray.push_back(windowSmoothness);
																		windowBlendArray.push_back(windowBlend);
																		windowSigwcellArray.push_back(windowSigwcell);

																		// Feature function
																		featureBombingArray.push_back(featureBombing);
																		featureNormArray.push_back(featureNorm);
																		featureWinfeatcorrelArray.push_back(featureWinfeatcorrel);
																		featureAnisoArray.push_back(featureAniso);
																		featureMinNbKernelsArray.push_back(featureMinNbKernels);
																		featureMaxNbKernelsArray.push_back(featureMaxNbKernels);
																		featureSigcosArray.push_back(featureSigcos);
																		featureSigcosvarArray.push_back(featureSigcosvar);
																		featureFrequencyArray.push_back(featureFrequency);
																		featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
																		featureThicknessArray.push_back(featureThickness);
																		featureCurvatureArray.push_back(featureCurvature);
																		featureOrientationArray.push_back(M_PI * featureOrientation);
																	}//featureWinfeatcorrel
																}//featureOrientation
															}//featureThickness
														}//featureCurvature
													}//featureMinNbKernels and featureMaxNbKernels
											}//featureSigcos
										}//featureNorm
									}//featureFrequency
								}//featureAniso
							}//featureBombing

							if (do_feature > 0 && do_feature != 3 && do_window == 1)
							{
								windowBlendA[1] = 0.6;
								for (int iwindowBlend = 0; iwindowBlend <= 1; iwindowBlend += 1)
								{
									windowBlend = windowBlendA[iwindowBlend];
									fprintf(fdg, "windowBlend=%g\n", windowBlend);

									float mfeatureAniso = 5.0;
									if (featureBombing == 4) mfeatureAniso = 2.5;
									for (featureAniso = 0.0; featureAniso <= mfeatureAniso; featureAniso += 2.5)
									{
										fprintf(fdg, "featureAniso=%g\n", featureAniso);
										int featureFrequencyA[] = { 0, 1, 2 };
										//for (int ifeatureFrequency = 2; ifeatureFrequency <= (featureBombing == 1 || featureBombing == 2 ? 6 : 0); ifeatureFrequency++)
										//for (int ifeatureFrequency = 0; ifeatureFrequency <= (featureBombing == 1 || featureBombing == 2 ? 4 : 0); ifeatureFrequency++)
										for (int ifeatureFrequency = 0; ifeatureFrequency <= 2; ifeatureFrequency += 2)
										{
											featureFrequency = featureFrequencyA[ifeatureFrequency];
											fprintf(fdg, "featureFrequency=%d\n", featureFrequency);
											for (featureNorm = 2.0; featureNorm <= 2.0; featureNorm += 1.0)
											{
												fprintf(fdg, "featureNorm=%g\n", featureNorm);
												float lfeatureSigcos = (featureFrequency > 1 ? 1.0 : 2.0);
												float mfeatureSigcos = 1.0;
												float sfeatureSigcos = 1.0;
												if (featureBombing == 3)
												{
													lfeatureSigcos = 10.0;
													if (featureAniso > 4.0) { mfeatureSigcos = 10.0; }
													else if (featureAniso > 3.0) { mfeatureSigcos = 5.0; sfeatureSigcos = 4.0; }
													else if (featureAniso > 2.0) { mfeatureSigcos = 3.0; sfeatureSigcos = 3.0; }
													else if (featureAniso > 1.0) { mfeatureSigcos = 1.0; sfeatureSigcos = 3.0; }
													else { mfeatureSigcos = 0.75; sfeatureSigcos = 0.25; lfeatureSigcos = 1.25; }
												}
												for (featureSigcos = mfeatureSigcos; featureSigcos <= lfeatureSigcos; featureSigcos += sfeatureSigcos)
												{
													fprintf(fdg, "featureSigcos=%g\n", featureSigcos);
													for (featureMinNbKernels = 3; featureMinNbKernels <= 9; featureMinNbKernels += 6)
														for (featureMaxNbKernels = featureMinNbKernels; featureMaxNbKernels <= featureMinNbKernels; featureMaxNbKernels += 6)
														{
															fprintf(fdg, "featureMinNbKernels=%d\n", featureMinNbKernels);
															float featureCurvatureA[] = { 0.0, 0.2, 1.0 };
															//for (int ifeatureCurvature = 2; ifeatureCurvature <= (ifeatureFrequency == 0 ? 0 : 2); ifeatureCurvature++)
															for (int ifeatureCurvature = 0; ifeatureCurvature <= (ifeatureFrequency == 0 ? 0 : 2); ifeatureCurvature += 2)
															{
																featureCurvature = featureCurvatureA[ifeatureCurvature];
																fprintf(fdg, "featureCurvature=%g\n", featureCurvature);
																float ffeatureThickness = 0.1;
																if (ifeatureFrequency == 0) ffeatureThickness = 1.0;
																else if (featureFrequency > 10) ffeatureThickness = 0.5;
																if (featureBombing == 5) ffeatureThickness = 0.1;
																// 	for (featureThickness = (featureBombing == 5 ? 0.15 : 1.0); featureThickness >= ffeatureThickness; featureThickness -= (featureBombing == 5 ? 0.05 : 0.45))
																for (featureThickness = (featureBombing == 5 ? 0.15 : 1.0); featureThickness >= ffeatureThickness; featureThickness -= (featureBombing == 5 ? 0.05 : 0.9))
																{
																	fprintf(fdg, "featureThickness=%g\n", featureThickness);
																	float ffeatureOrientation = 0.5;
																	if (ifeatureFrequency == 0 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																	if (featureCurvature > 0.9 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																	if (featureBombing >= 3) ffeatureOrientation = 0.5;
																	if ((featureBombing == 3 || featureBombing == 4) && featureNorm == 2.0 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																	if ((featureBombing == 4) && featureNorm == 2.0 && featureAniso == 0.0) ffeatureOrientation = 0.0;
																	//if (featureBombing == 5) ffeatureOrientation = 0.0;
																	for (featureOrientation = 0.0; featureOrientation <= ffeatureOrientation; featureOrientation += 0.25)
																	{
																		fprintf(fdg, "featureOrientation=%g\n", featureOrientation);
																		float windowArityA[] = { 2.0, 3.0, 4.0, 8.0 };
																		for (int iwindowArity = 1; iwindowArity <= 3; iwindowArity += 2)
																		{
																			windowArity = windowArityA[iwindowArity];
																			fprintf(fdg, "windowArity=%g\n", windowArity);
																			float lwindowSmoothness = 2.0;
																			if (windowArity >= 6.0) lwindowSmoothness = 0.0;
																			else if (windowArity >= 5.0) lwindowSmoothness = 1.0;
																			for (windowSmoothness = 0.0; windowSmoothness <= lwindowSmoothness; windowSmoothness += 2.0)
																			{
																				fprintf(fdg, "windowSmoothness=%g\n", windowSmoothness);
																				float lwindowLarp = 0.5;
																				if (windowArity <= 3.0) lwindowLarp = 0.0;
																				else if (windowArity <= 5.0) lwindowLarp = 0.25;
																				//if (jittering<0.1) lwindowLarp = 0.0;
																				for (windowLarp = 0.0; windowLarp <= lwindowLarp; windowLarp += 0.25)
																					//float lwindowLarp = 1.0;
																					//float windowstepLarp = 0.25;
																					//if (windowArity <= 3.0) lwindowLarp = 0.0;
																					//else if (windowArity <= 5.0) lwindowLarp = 0.25;
																					//else windowstepLarp = 0.3;
																					//for (windowLarp = 0.0; windowLarp <= lwindowLarp; windowLarp += windowstepLarp)
																				{
																					fprintf(fdg, "windowLarp=%g\n", windowLarp);
																					for (windowNorm = 2.0; windowNorm >= 2.0; windowNorm -= 1.0)
																					{
																						fprintf(fdg, "windowNorm=%g\n", windowNorm);
																						float windowSigwcells = 1.0;
																						//if (windowBlend <= 0.7)  windowSigwcells = 0.7;
																						for (windowSigwcell = windowSigwcells; windowSigwcell <= 1.5; windowSigwcell += 0.8)
																						{
																							fprintf(fdg, "windowSigwcell=%g\n", windowSigwcell);
																							for (featureWinfeatcorrel = 0.0; featureWinfeatcorrel <= 0.6; featureWinfeatcorrel += 0.5)
																							{
																								fprintf(fdg, "featureWinfeatcorrel=%g\n", featureWinfeatcorrel);
																								fprintf(fdg, "------------> do image %06d\n", pptbf_count);
																								//if (featureBombing == 5) printf("at %d\n", pptbf_count);
																								fprintf(fd, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, tresolution, rotation, rescalex, tturbAmplitude0, turbAmplitude1, turbAmplitude2,
																									windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
																									featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
																								pptbf_count++;
																								// Deformation
																								turbulenceAmplitude0Array.push_back(turbAmplitude0);
																								turbulenceAmplitude1Array.push_back(turbAmplitude1);
																								turbulenceAmplitude2Array.push_back(turbAmplitude2);

																								// Model Transform
																								modelResolutionArray.push_back(resolution);
																								modelAlphaArray.push_back(rotation * M_PI);
																								modelRescalexArray.push_back(rescalex);

																								// Point process
																								tilingTypeArray.push_back(do_tiling);
																								jitteringArray.push_back(jittering);

																								// Window function
																								windowShapeArray.push_back(windowShape);
																								windowArityArray.push_back(windowArity);
																								windowLarpArray.push_back(windowLarp);
																								windowNormArray.push_back(windowNorm);
																								windowSmoothArray.push_back(windowSmoothness);
																								windowBlendArray.push_back(windowBlend);
																								windowSigwcellArray.push_back(windowSigwcell);

																								// Feature function
																								featureBombingArray.push_back(featureBombing);
																								featureNormArray.push_back(featureNorm);
																								featureWinfeatcorrelArray.push_back(featureWinfeatcorrel);
																								featureAnisoArray.push_back(featureAniso);
																								featureMinNbKernelsArray.push_back(featureMinNbKernels);
																								featureMaxNbKernelsArray.push_back(featureMaxNbKernels);
																								featureSigcosArray.push_back(featureSigcos);
																								featureSigcosvarArray.push_back(featureSigcosvar);
																								featureFrequencyArray.push_back(featureFrequency);
																								featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
																								featureThicknessArray.push_back(featureThickness);
																								featureCurvatureArray.push_back(featureCurvature);
																								featureOrientationArray.push_back(M_PI * featureOrientation);
																							}//featureWinfeatcorrel
																						}//windowSigwcell
																					}//windowNorm
																				}//windowLarp
																			}//windowSmoothness
																		}//windowArity
																	}//featureOrientation
																}//featureThickness
															}//featureCurvature
														}//featureMinNbKernels and featureMaxNbKernels
												}//featureSigcos
											}//featureNorm
										}//featureFrequency
									}//featureAniso
								}//featureBombing
							} //windowBlend

							fflush(fdg);
						}//jittering
					}//turbulence
				}//rotation
			} // rescalex
		}//resolution
	}
	else
	{
		sprintf(pname, "%s/pptbf_window.txt", pPath);
		FILE *wfd = fopen(pname, "r");
		char buff[2048], rdbuff[10];
		int num;
		fgets(buff, 1000, wfd); // skip first line
		fgets(buff, 1000, wfd);
		fprintf(fdg, "%s <window>\n", buff); fflush(fdg);
		bool contw = true;
		int pptbf_countw = 0;
		do {
			sscanf(buff, "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", &num, &wjittering, &wresolution, &wrotation, &wrescalex, &wturbAmplitude0, &wturbAmplitude1, &wturbAmplitude2,
				&wwindowShape, &wwindowArity, &wwindowLarp, &wwindowNorm, &wwindowSmoothness, &wwindowBlend, &wwindowSigwcell,
				&wfeatureBombing, &wfeatureNorm, &wfeatureWinfeatcorrel, &wfeatureAniso, &wfeatureMinNbKernels, &wfeatureMaxNbKernels, &wfeatureSigcos, &wfeatureSigcosvar, &wfeatureFrequency, &wfeaturePhaseShift, &wfeatureThickness, &wfeatureCurvature, &wfeatureOrientation);
			if (num == pptbf_countw)
			{
				contw = true;
				sprintf(pname, "%s/pptbf_feature.txt", pPath);
				FILE *ffd = fopen(pname, "r");
				fgets(buff, 1000, ffd); // skip first line
				fgets(buff, 1000, ffd);
				fprintf(fdg, "%s <feature>\n", buff); fflush(fdg);
				bool contf = true;
				int pptbf_countf = 0;
				do {
					contf = true;
					sscanf(buff, "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", &num, &fjittering, &fresolution, &frotation, &frescalex, &fturbAmplitude0, &fturbAmplitude1, &fturbAmplitude2,
						&fwindowShape, &fwindowArity, &fwindowLarp, &fwindowNorm, &fwindowSmoothness, &fwindowBlend, &fwindowSigwcell,
						&ffeatureBombing, &ffeatureNorm, &ffeatureWinfeatcorrel, &ffeatureAniso, &ffeatureMinNbKernels, &ffeatureMaxNbKernels, &ffeatureSigcos, &ffeatureSigcosvar, &ffeatureFrequency, &ffeaturePhaseShift, &ffeatureThickness, &ffeatureCurvature, &ffeatureOrientation);
					if (num == pptbf_countf)
					{
						turbAmplitude0 = wturbAmplitude0;
						turbAmplitude1 = wturbAmplitude1;
						turbAmplitude2 = wturbAmplitude2;
						resolution = wresolution;
						rotation = wrotation;
						rescalex = wrescalex;
						jittering = wjittering;
						windowShape = wwindowShape;
						windowArity = wwindowArity;
						windowNorm = wwindowNorm;
						windowSmoothness = wwindowSmoothness;
						windowBlend = wwindowBlend;
						windowLarp = wwindowLarp;
						windowSigwcell = wwindowSigwcell;

						featureBombing = ffeatureBombing;
						featureNorm = ffeatureNorm;
						featureWinfeatcorrel = ffeatureWinfeatcorrel;
						featureAniso = ffeatureAniso;
						featureMinNbKernels = ffeatureMinNbKernels;
						featureMaxNbKernels = ffeatureMaxNbKernels;
						featureSigcos = ffeatureSigcos;
						featureSigcosvar = ffeatureSigcosvar;
						featureFrequency = ffeatureFrequency;
						featurePhaseShift = ffeaturePhaseShift;
						featureThickness = ffeatureThickness;
						featureCurvature = ffeatureCurvature;
						featureOrientation = ffeatureOrientation;

						for (windowBlend = 0.6; windowBlend <= 1.1; windowBlend += 0.4)
						{
							for (featureWinfeatcorrel = 0.0; featureWinfeatcorrel <= 0.8; featureWinfeatcorrel += 0.7)
							{
								fprintf(fd, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
									windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
									featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);

								// Deformation
								turbulenceAmplitude0Array.push_back(turbAmplitude0);
								turbulenceAmplitude1Array.push_back(turbAmplitude1);
								turbulenceAmplitude2Array.push_back(turbAmplitude2);

								// Model Transform
								modelResolutionArray.push_back(resolution);
								modelAlphaArray.push_back(rotation * M_PI);
								modelRescalexArray.push_back(rescalex);

								// Point process
								tilingTypeArray.push_back(do_tiling);
								jitteringArray.push_back(jittering);

								// Window function
								windowShapeArray.push_back(windowShape);
								windowArityArray.push_back(windowArity);
								windowLarpArray.push_back(windowLarp);
								windowNormArray.push_back(windowNorm);
								windowSmoothArray.push_back(windowSmoothness);
								windowBlendArray.push_back(windowBlend);
								windowSigwcellArray.push_back(windowSigwcell);

								// Feature function
								featureBombingArray.push_back(featureBombing);
								featureNormArray.push_back(featureNorm);
								featureWinfeatcorrelArray.push_back(featureWinfeatcorrel);
								featureAnisoArray.push_back(featureAniso);
								featureMinNbKernelsArray.push_back(featureMinNbKernels);
								featureMaxNbKernelsArray.push_back(featureMaxNbKernels);
								featureSigcosArray.push_back(featureSigcos);
								featureSigcosvarArray.push_back(featureSigcosvar);
								featureFrequencyArray.push_back(featureFrequency);
								featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
								featureThicknessArray.push_back(featureThickness);
								featureCurvatureArray.push_back(featureCurvature);
								featureOrientationArray.push_back(M_PI * featureOrientation);

								pptbf_count++;
							}
						}
						pptbf_countf++;
						fgets(buff, 1000, ffd);
						fprintf(fdg, "%s <feature>\n", buff); fflush(fdg);
					}
					else contf = false;
				} while (contf);
				fclose(ffd);

				pptbf_countw++;
				fgets(buff, 1000, wfd);
				fprintf(fdg, "%s <window>\n", buff); fflush(fdg);
			}
			else contw = false;
		} while (contw);
		fclose(wfd);
	}
	fclose(fdg);
	fclose(fd);
#endif
#ifdef PRODUCT
	sprintf(pname, "%s/pptbf_probe_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	FILE *fd = fopen(pname, "r");
	//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	//FILE *fdn = fopen(pname, "w");
	//fprintf(fdn, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");
	char buff[2048];
	int num;
	fgets(buff, 1000, fd); // skip first line
	fgets(buff, 1000, fd);
	bool cont = true;
	int pptbf_countf = 0;
	do {
		sscanf(buff, "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", &num, &jittering, &resolution, &rotation, &rescalex, &turbAmplitude0, &turbAmplitude1, &turbAmplitude2,
			&windowShape, &windowArity, &windowLarp, &windowNorm, &windowSmoothness, &windowBlend, &windowSigwcell,
			&featureBombing, &featureNorm, &featureWinfeatcorrel, &featureAniso, &featureMinNbKernels, &featureMaxNbKernels, &featureSigcos, &featureSigcosvar, &featureFrequency, &featurePhaseShift, &featureThickness, &featureCurvature, &featureOrientation);
		if (num == pptbf_countf)
		{
			cont = true;

			int resolutionArray[] = { 40 / 2, 100/2, 150 / 2 };

			if (do_tiling == 2 || do_tiling >= 10) for (int i = 0; i < 4; i++) resolutionArray[i] *= 2;
#ifdef WTRANSF
			for (int iresolution = 0; iresolution <= 2; iresolution++)
#else
			for (int iresolution = 1; iresolution <= 1; iresolution++)
#endif
			{
				resolution = resolutionArray[iresolution];
				float zoom = (float)SS / (float)resolutionArray[iresolution];
				float rescalexArray[] = { 1.0, 3.0, 6.0 };
#ifdef WTRANSF
				for (int irescalex = 0; irescalex <= 1; irescalex++)
#else
				for (int irescalex = 0; irescalex <= 0; irescalex++)
#endif
				{
					rescalex = rescalexArray[irescalex];
					//if (rescalex >= 5.0) resolution *= 2;
					if (rescalex >= 5.0) resolution = (resolution * 4) / 3;
					float rotstep = 1.0 / 8.0;
#ifdef WTRANSF
					for (rotation = 0.0; rotation <= 1.0 / 8.0; rotation += rotstep)
#else
					for (rotation = 0.0; rotation <= 0.0 / 8.0; rotation += rotstep)
#endif
					{

						//float turbAmplitude0A[] = { 0.0, 0.06,  0.05, 0.03, 0.06, 0.05, 0.02 };
						//float turbAmplitude1A[] = { 0.0, 0.05, 0.5, 0.8, 0.05, 0.5, 0.5 };
						//float turbAmplitude2A[] = { 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 4.0  };
						float turbAmplitude0A[] = { 0.0, 0.06,  0.05, 0.06, 0.05};
						float turbAmplitude1A[] = { 0.0, 0.05, 0.5, 0.05, 0.5};
						float turbAmplitude2A[] = { 1.0, 1.0, 1.0, 2.0, 2.0};
#ifdef WTRANSF
						for (int iturbAmplitude = 0; iturbAmplitude <= 3; iturbAmplitude++)
#else
						for (int iturbAmplitude = 0; iturbAmplitude <= 0; iturbAmplitude++)
#endif
						{
							turbAmplitude0 = turbAmplitude0A[iturbAmplitude] * (float)resolution / (50.0 / 1.0);
							turbAmplitude1 = turbAmplitude1A[iturbAmplitude];
							turbAmplitude2 = turbAmplitude2A[iturbAmplitude];

							// Deformation
							turbulenceAmplitude0Array.push_back(turbAmplitude0);
							turbulenceAmplitude1Array.push_back(turbAmplitude1);
							turbulenceAmplitude2Array.push_back(turbAmplitude2);

							// Model Transform
							modelResolutionArray.push_back(resolution);
							modelAlphaArray.push_back(rotation * M_PI);
							modelRescalexArray.push_back(rescalex);

							// Point process
							tilingTypeArray.push_back(do_tiling);
							jitteringArray.push_back(jittering);

							// Window function
							windowShapeArray.push_back(windowShape);
							windowArityArray.push_back(windowArity);
							windowLarpArray.push_back(windowLarp);
							windowNormArray.push_back(windowNorm);
							windowSmoothArray.push_back(windowSmoothness);
							windowBlendArray.push_back(windowBlend);
							windowSigwcellArray.push_back(windowSigwcell);

							// Feature function
							featureBombingArray.push_back(featureBombing);
							featureNormArray.push_back(featureNorm);
							featureWinfeatcorrelArray.push_back(featureWinfeatcorrel);
							featureAnisoArray.push_back(featureAniso);
							featureMinNbKernelsArray.push_back(featureMinNbKernels);
							featureMaxNbKernelsArray.push_back(featureMaxNbKernels);
							featureSigcosArray.push_back(featureSigcos);
							featureSigcosvarArray.push_back(featureSigcosvar);
							featureFrequencyArray.push_back(featureFrequency);
							featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
							featureThicknessArray.push_back(featureThickness);
							featureCurvatureArray.push_back(featureCurvature);
							featureOrientationArray.push_back(M_PI * featureOrientation);
							
							//fprintf(fdn, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
							//	windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
							//	featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
							pptbf_count++;
						}
					}
				}
			}
			pptbf_countf++;

			fgets(buff, 1000, fd);
		}
		else cont = false;
	} while (cont);
//	fclose(fdn);
	// Close files
	fclose(fd);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LOG info
	//std::cout << "Finished..." << pptbf_count << std::endl;
	//return;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	

#endif
#ifdef REFINEMENT

	
	int pptbf_countf = 0;
	sprintf(pname, "%s/%s_best_pptbfparams_%d.txt", pPath, exname, descriptor);
	FILE *fd = fopen(pname, "r");
	//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	//FILE *fdn = fopen(pname, "w");
	//fprintf(fdn, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");
	char buff1[2048], buff2[2048], buff3[2048];
	int num;
	fgets(buff1, 1000, fd);
	sscanf(buff1, "%g", &percent);
	percent = 1.0 - percent;
	fgets(buff1, 1000, fd); fgets(buff2, 1000, fd); fgets(buff3, 1000, fd);
	bool cont = true;
	do { 
		sscanf(buff1, "%02d %02d %d %d %d", &do_serie, &do_tiling, &do_window, &do_feature, &do_phase);
		sscanf(buff3, "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", 
			&num, &jittering, &resolution, &rotation, &rescalex, &turbAmplitude0, &turbAmplitude1, &turbAmplitude2,
			&windowShape, &windowArity, &windowLarp, &windowNorm, &windowSmoothness, &windowBlend, &windowSigwcell,
			&featureBombing, &featureNorm, &featureWinfeatcorrel, &featureAniso, &featureMinNbKernels, &featureMaxNbKernels, &featureSigcos, &featureSigcosvar, &featureFrequency, &featurePhaseShift, &featureThickness, &featureCurvature, &featureOrientation);
		if (pptbf_countf < KEEPNBEST)
		{
			cont = true;
			bool initial = true;
			for (int iresolution = 0; iresolution < NR; iresolution++)
			//for (int iresolution = 0; iresolution <= 0; iresolution++)
			{
				int nresolution = (int)(factresol*(double)resolution*(1.0f + (initial ? 0.0 : 0.25*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				float zoom = (float)SS / (float)nresolution;
				float njittering = jittering*(1.0f + (initial ? 0.0 : ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (iresolution >= NR / 2 && !initial) njittering = (float)rand() / (float)RAND_MAX;
				if (njittering >= 1.0) njittering = 0.99;
				float nrescalex = factrescalex*rescalex*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				
				float turbAmplitude0A[] = { 0.0, 0.06,  0.05, 0.03, 0.06, 0.05, 0.02 };
				float turbAmplitude1A[] = { 0.0, 0.05, 0.5, 0.8, 0.05, 0.5, 0.5 };
				float turbAmplitude2A[] = { 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 4.0  };

				float nturbAmplitude0 = turbAmplitude0*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				float nturbAmplitude1 = turbAmplitude1*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				float nturbAmplitude2 = turbAmplitude2*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (iresolution >= NR / 2 && !initial) nturbAmplitude0 = turbAmplitude0A[(iresolution - 1) % 6+1]* (1.0+0.2*ecart*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f))* (float)nresolution / (50.0 / 1.0);
				if (iresolution >= NR / 2 && !initial) nturbAmplitude1 = turbAmplitude1A[(iresolution - 1) % 6+1] * (1.0 + 0.2*ecart*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (iresolution >= NR / 2 && !initial) nturbAmplitude2 = turbAmplitude2A[(iresolution - 1) % 6+1] * (1.0 + 0.2*ecart*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));

				// Deformation
				turbulenceAmplitude0Array.push_back(nturbAmplitude0);
				turbulenceAmplitude1Array.push_back(nturbAmplitude1);
				turbulenceAmplitude2Array.push_back(nturbAmplitude2);

				// Model Transform
				modelResolutionArray.push_back(nresolution);
				modelAlphaArray.push_back((rotation+ deltaalpha) * M_PI);
				modelRescalexArray.push_back(nrescalex);

				// Point process
				tilingTypeArray.push_back(do_tiling);
				jitteringArray.push_back(njittering);

				// Window function
				float nwindowArity= windowArity*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (nwindowArity <= 3.0) nwindowArity = 3.0;
				nwindowArity = floor(nwindowArity);
				windowShapeArray.push_back(windowShape);
				windowArityArray.push_back(windowArity);
				windowLarpArray.push_back(windowLarp*(1.0f + (initial ? 0.0 : 0.2*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				windowNormArray.push_back(windowNorm);
				windowSmoothArray.push_back(windowSmoothness*(1.0f + (initial ? 0.0 : 0.2*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				windowBlendArray.push_back(windowBlend);
				windowSigwcellArray.push_back(windowSigwcell);

				// Feature function
				featureBombingArray.push_back(featureBombing);
				featureNormArray.push_back(featureNorm);
				featureWinfeatcorrelArray.push_back(featureWinfeatcorrel*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				featureAnisoArray.push_back(featureAniso);
				featureMinNbKernelsArray.push_back(featureMinNbKernels);
				int nfeatureMaxNbKernels = (int)((float)featureMaxNbKernels*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)) + 0.5);
				if (nfeatureMaxNbKernels < featureMinNbKernels) nfeatureMaxNbKernels = featureMinNbKernels;
				featureMaxNbKernelsArray.push_back(nfeatureMaxNbKernels);
				featureSigcosArray.push_back(featureSigcos);
				featureSigcosvarArray.push_back(featureSigcosvar);
				featureFrequencyArray.push_back(featureFrequency*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
				featureThicknessArray.push_back(featureThickness*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				featureCurvatureArray.push_back(featureCurvature);
				featureOrientationArray.push_back(M_PI * featureOrientation *(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));

				//fprintf(fdn, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
				//	windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
				//	featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
				pptbf_count++;
				initial = false;


			}
			fgets(buff1, 1000, fd); fgets(buff2, 1000, fd); fgets(buff3, 1000, fd);
			pptbf_countf++;
		}
		else cont = false;
	} while (cont);
	//	fclose(fdn);
	fclose(fd);
#endif

	////////////////
	//// Deformation
	////////////////

	//// Parameter 0
	//for ( float param_0 = 0.0f; param_0 <= 1.0f; param_0 += 0.25f )
	//{
	//	turbulenceAmplitude0Array.push_back( param_0 );
	//}

	//// Parameter 1
	//for ( float param_1 = 0.0f; param_1 <= 1.0f; param_1 += 0.25f )
	//{
	//	turbulenceAmplitude1Array.push_back( param_1 );
	//}

	//// Parameter 2
	//for ( float param_2 = 0.0f; param_2 <= 1.0f; param_2 += 0.25f )
	//{
	//	turbulenceAmplitude2Array.push_back( param_2 );
	//}



	//////////////////
	//// Point Process
	//////////////////

	//// Tiling type
	////tilingTypeArray.resize( 18 );
	////std::iota( tilingTypeArray.begin(), tilingTypeArray.end(), 0 );
	////for ( int tileid = 0; tileid <= 17; tileid++ )
	//for ( int tileid = do_tiling; tileid <= do_tiling; tileid++ )
	//{
	//	tilingTypeArray.push_back( tileid );
	//}

	//// Jittering
	//for ( float jitter = 0.5f; jitter <= 1.0f; jitter += 0.45f )
	//{
	//	jitteringArray.push_back( jitter );
	//}

	////////////////////
	//// Model Transform & Deformation
	////////////////////

	//// Resolution
	//for (int resolution = 50; resolution <= 200; resolution += 50)
	//{
	//	modelResolutionArray.push_back(resolution);
	//}

	//// Alpha
	//for (float alpha = 0.f; alpha <= 2.f; alpha += 0.25f)
	//{
	//	modelAlphaArray.push_back(alpha * M_PI);
	//}

	//// Rescale X
	//for (float rescalex = 1.0f; rescalex <= 4.f; rescalex += 0.5f)
	//{
	//	modelRescalexArray.push_back(rescalex);
	//}

	////////////////////
	//// WINDOW FUNCTION
	////////////////////

	//// Shape
	////for ( int shape = 0; shape <= 3; shape += 1 )
	//for ( int shape = 2; shape <= 3; shape += 1 )
	//{
	//	windowShapeArray.push_back( shape );
	//}

	//// Arity
	////for ( float windowArity = 2.f; windowArity <= 6.f; windowArity += 1.f )
	//for ( float windowArity = 6.f; windowArity <= 6.f; windowArity += 1.f )
	//{
	//	windowArityArray.push_back( windowArity );
	//}

	//// Larp
	//for ( float windowLarp = 0.f; windowLarp <= 1.f; windowLarp += 0.5f )
	//{
	//	windowLarpArray.push_back( windowLarp );
	//}

	//// Norm
	////for ( float windowNorm = 1.f; windowNorm <= 3.f; windowNorm += 1.f )
	//for ( float windowNorm = 2.f; windowNorm <= 3.f; windowNorm += 1.f )
	//{
	//	windowNormArray.push_back( windowNorm );
	//}

	//// Smooth
	//for ( float windowSmooth = 0.f; windowSmooth <= 2.f; windowSmooth += 1.f )
	//{
	//	windowSmoothArray.push_back( windowSmooth );
	//}

	//// Blend
	////for ( float windowBlend = 0.f; windowBlend <= 1.f; windowBlend += 0.25f )
	//for ( float windowBlend = 1.f; windowBlend <= 1.f; windowBlend += 0.25f )
	//{
	//	windowBlendArray.push_back( windowBlend );
	//}

	//// Sigwcell
	////for ( float windowSigwcell = 0.f; windowSigwcell <= 1.f; windowSigwcell += 0.25f )
	//for ( float windowSigwcell = 0.1f; windowSigwcell <= 1.f; windowSigwcell += 0.25f )
	//{
	//	windowSigwcellArray.push_back( windowSigwcell );
	//}

	/////////////////////
	//// Feature Function
	/////////////////////

	//// Bombing
	//for ( int bombing = 0; bombing <= 3; bombing += 1 )
	//{
	//	featureBombingArray.push_back( bombing );
	//}

	//// Norm
	////for ( float norm = 1.0; norm <= 3.0; norm += 1.0 )
	//for ( float norm = 2.0; norm <= 3.0; norm += 1.0 )
	//{
	//	featureNormArray.push_back( norm );
	//}

	//// Winfeatcorrel
	//for ( float winfeatcorrel = 0.0; winfeatcorrel <= 10.0; winfeatcorrel += 1.0 )
	//{
	//	featureWinfeatcorrelArray.push_back( winfeatcorrel );
	//}

	//// Anisotropy
	////for ( float anisotropy = 1.0; anisotropy <= 10.0; anisotropy += 1.0 )
	//for ( float anisotropy = 10.f; anisotropy <= 10.0; anisotropy += 1.0 )
	//{
	//	featureAnisoArray.push_back( anisotropy );
	//}

	//// Min/Max number of kernels
	//for ( int Npmin = 1; Npmin <= 8; Npmin += 4 )
	//{
	//	featureMinNbKernelsArray.push_back( Npmin );
	//	for ( int Npmax = Npmin; Npmin <= 8; Npmin += 4 )
	//	{
	//		featureMaxNbKernelsArray.push_back( Npmax );
	//	}
	//}

	//// Sigcos
	////for ( float featureSigcos = 0.1f; featureSigcos <= 1.f; featureSigcos += 0.5f )
	//for ( float featureSigcos = 1.f; featureSigcos <= 1.f; featureSigcos += 0.5f )
	//{
	//	featureSigcosArray.push_back( featureSigcos );
	//}

	//// Sigcosvar
	//for ( float featureSigcosvar = 0.1f; featureSigcosvar <= 1.f; featureSigcosvar += 0.5f )
	//{
	//	featureSigcosvarArray.push_back( featureSigcosvar );
	//}

	//// Frequency
	//for ( int ifreq = 0; ifreq <= 3; ifreq += 1 )
	//{
	//	featureFrequencyArray.push_back( ifreq );
	//}

	//// Phase
	//for ( float phase = 0.0; phase <= 2.0; phase += 2.0 )
	//{
	//	featurePhaseShiftArray.push_back( phase * M_PI );
	//}

	//// Thickness
	////for ( float thickness = 0.1; thickness <= 1.0; thickness += 0.4 )
	//for ( float thickness = 0.01f; thickness <= 1.0; thickness += 0.4 )
	//{
	//	featureThicknessArray.push_back( thickness );
	//}

	//// Curvature
	//for ( float curvature = 0.0; curvature <= 0.4; curvature += 0.2 )
	//{
	//	featureCurvatureArray.push_back( curvature );
	//}

	//// Orientation
	//for ( float deltaorient = 0.0; deltaorient <= M_PI / 2.0; deltaorient += M_PI / 4.0 )
	//{
	//	featureOrientationArray.push_back( deltaorient );
	//}

#endif


std::vector< std::vector<unsigned char> > similarity(pptbf_count);
int pptbf_keep_count = 0;
bool *keep = new bool[pptbf_count];
//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
//fd = fopen(pname, "w");
//fprintf(fd, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");


	///////////////////////
	// Set common GL states
	///////////////////////

	//setImageWidth( width );
	//setImageHeight( height );
	setWidth( width );
	setHeight( height );

	// Set shader program
	PtShaderProgram* shaderProgram = mMegakernelShaderProgram;
	shaderProgram->use();

	// Set texture(s)
	// - PRNG
	glBindTextureUnit( 0/*unit*/, mTexture_P );
	// - noise
	glBindTextureUnit( 1/*unit*/, mTexture_rnd_tab );

	// Set sampler(s)
	// - PRNG
	glBindSampler( 0/*unit*/, mSampler_PRNG_Noise );
	// - noise
	glBindSampler( 1/*unit*/, mSampler_PRNG_Noise );

	// Set image(s)
	// - PPTBF (output)
	glBindImageTexture( 0/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );

	// Set uniform(s)
	// - PRNG
	shaderProgram->set( 0, "uPermutationTex" );
	// - noise
	shaderProgram->set( 1, "uNoiseTex" );

	// Kernel configuration
	// - block
	const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
	const int blockSizeY = 8;
	// - grid
	const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
	const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );

	/////////////////////////////////////////
	// Sampling the space of PPTBF structures
	/////////////////////////////////////////
	
	// Global PPTBF counter
#ifdef REFINEMENT
	fprintf(fdn, "%g\n", 1.0 - percent);
#endif
	for (int k = 0; k<pptbf_count; k++)
	{

	//// Deformation
	//int d_0 = 0;
	//int d_1 = 0;
	//int d_2 = 0;
	//// Model Transform
	//int m_0 = 0;
	//int m_1 = 0;
	//int m_2 = 0;
	//// Point process
	//int p_0 = 0;
	//int p_1 = 0;
	//// Window function
	//int w_0 = 0;
	//int w_1 = 0;
	//int w_2 = 0;
	//int w_3 = 0;
	//int w_4 = 0;
	//int w_5 = 0;
	//int w_6 = 0;
	//// Feature function
	//int f_0 = 0;
	//int f_1 = 0;
	//int f_2 = 0;
	//int f_3 = 0;
	//int f_4 = 0;
	//int f_5 = 0;
	//int f_6 = 0;
	//int f_7 = 0;
	//int f_8 = 0;
	//int f_9 = 0;
	//int f_10 = 0;
	//int f_11 = 0;
	//int f_12 = 0;

	//std::size_t nbSamples = 1;
	//// Deformation
	//nbSamples *= turbulenceAmplitude0Array.size();
	//nbSamples *= turbulenceAmplitude1Array.size();
	//nbSamples *= turbulenceAmplitude2Array.size();
	//// Model Transform
	//nbSamples *= modelResolutionArray.size();
	//nbSamples *= modelAlphaArray.size();
	//nbSamples *= modelRescalexArray.size();
	//// Point process
	//nbSamples *= tilingTypeArray.size();
	//nbSamples *= jitteringArray.size();
	//// Window function
	//nbSamples *= windowShapeArray.size();
	//nbSamples *= windowArityArray.size();
	//nbSamples *= windowLarpArray.size();
	//nbSamples *= windowNormArray.size();
	//nbSamples *= windowSmoothArray.size();
	//nbSamples *= windowBlendArray.size();
	//nbSamples *= windowSigwcellArray.size();
	//// Feature function
	//nbSamples *= featureBombingArray.size();
	//nbSamples *= featureNormArray.size();
	//nbSamples *= featureWinfeatcorrelArray.size();
	//nbSamples *= featureAnisoArray.size();
	//nbSamples *= featureMinNbKernelsArray.size();
	//nbSamples *= featureMaxNbKernelsArray.size();
	//nbSamples *= featureSigcosArray.size();
	//nbSamples *= featureSigcosvarArray.size();
	//nbSamples *= featureFrequencyArray.size();
	//nbSamples *= featurePhaseShiftArray.size();
	//nbSamples *= featureThicknessArray.size();
	//nbSamples *= featureCurvatureArray.size();
	//nbSamples *= featureOrientationArray.size();

	////////////////
	// Point Process
	////////////////
	
	//p_0 = 0;
	//for ( const auto tilingType : tilingTypeArray )
	//{
		shaderProgram->set(tilingTypeArray[k], "uPointProcessTilingType" );

	//p_1 = 0;
	//for ( const auto jittering : jitteringArray )
	//{
		shaderProgram->set(jitteringArray[k], "uPointProcessJitter" );

	//////////////////
	// Model Transform
	//////////////////

	//m_0 = 0;
	//for ( const auto modelResolution : modelResolutionArray )
	//{
		shaderProgram->set(modelResolutionArray[k], "uResolution" );

	//m_1 = 0;
	//for ( const auto modelAlpha : modelAlphaArray )
	//{
		shaderProgram->set(modelAlphaArray[k], "uRotation" );

	//m_2 = 0;
	//for ( const auto modelRescalex : modelRescalexArray )
	//{
		shaderProgram->set( modelRescalexArray[k], "uRescaleX" );	
		
		
	//////////////
	// Deformation
	//////////////

	//d_0 = 0;
	//for ( const auto deformation_0 : turbulenceAmplitude0Array )
	//{
		shaderProgram->set(turbulenceAmplitude0Array[k], "uTurbulenceAmplitude_0" );

	//d_1 = 0;
	//for ( const auto deformation_1 : turbulenceAmplitude1Array )
	//{
		shaderProgram->set(turbulenceAmplitude1Array[k], "uTurbulenceAmplitude_1" );

	//d_2 = 0;
	//for ( const auto deformation_2 : turbulenceAmplitude2Array )
	//{
		shaderProgram->set(turbulenceAmplitude2Array[k], "uTurbulenceAmplitude_2" );



	
	//////////////////
	// Window Function
	//////////////////

	//w_0 = 0;
	//for ( const auto windowShape : windowShapeArray )
	//{
		shaderProgram->set(windowShapeArray[k], "uWindowShape" );

	//w_1 = 0;
	//for ( const auto windowArity : windowArityArray )
	//{
		shaderProgram->set(windowArityArray[k], "uWindowArity" );

	//w_2 = 0;
	//for ( const auto windowLarp : windowLarpArray )
	//{
		shaderProgram->set(windowLarpArray[k], "uWindowLarp" );

	//w_3 = 0;
	//for ( const auto windowNorm : windowNormArray )
	//{
		shaderProgram->set(windowNormArray[k], "uWindowNorm" );

	//w_4 = 0;
	//for ( const auto windowSmooth : windowSmoothArray )
	//{
		shaderProgram->set(windowSmoothArray[k], "uWindowSmooth" );

	//w_5 = 0;
	//for ( const auto windowBlend : windowBlendArray )
	//{
		shaderProgram->set(windowBlendArray[k], "uWindowBlend" );

	//w_6 = 0;
	//for ( const auto windowSigwcell : windowSigwcellArray )
	//{
		shaderProgram->set(windowSigwcellArray[k], "uWindowSigwcell" );

	///////////////////
	// Feature Function
	///////////////////

	
	//f_0 = 0;
	//for ( const auto featureBombing : featureBombingArray )
	//{
		shaderProgram->set(featureBombingArray[k], "uFeatureBomb" );

	//f_1 = 0;
	//for ( const auto featureNorm : featureNormArray )
	//{
		shaderProgram->set(featureNormArray[k], "uFeatureNorm" );

	//f_2 = 0;
	//for ( const auto featureWinfeatcorrel : featureWinfeatcorrelArray )
	//{
		shaderProgram->set(featureWinfeatcorrelArray[k], "uFeatureWinfeatcorrel" );

	//f_3 = 0;
	//for ( const auto featureAniso : featureAnisoArray )
	//{
		shaderProgram->set(featureAnisoArray[k], "uFeatureAniso" );

	//f_4 = 0;
	//for ( const auto minNbGaborKernels : featureMinNbKernelsArray )
	//{
		shaderProgram->set(featureMinNbKernelsArray[k], "uFeatureNpmin" );

	//f_5 = 0;
	//for ( const auto maxNbGaborKernels : featureMaxNbKernelsArray )
	//{
		shaderProgram->set(featureMaxNbKernelsArray[k], "uFeatureNpmax" );

	//f_6 = 0;
	//for ( const auto featureSigcos : featureSigcosArray )
	//{
		shaderProgram->set(featureSigcosArray[k], "uFeatureSigcos" );

	//f_7 = 0;
	//for ( const auto featureSigcosvar : featureSigcosvarArray )
	//{
		shaderProgram->set(featureSigcosvarArray[k], "uFeatureSigcosvar" );

	//f_8 = 0;
	//for ( const auto gaborStripesFrequency : featureFrequencyArray )
	//{
		shaderProgram->set(featureFrequencyArray[k], "uFeatureFreq" );

	//f_9 = 0;
	//for ( const auto featurePhase : featurePhaseShiftArray )
	//{
		shaderProgram->set(featurePhaseShiftArray[k], "uFeaturePhase" );

	//f_10 = 0;
	//for ( const auto gaborStripesThickness : featureThicknessArray )
	//{
		shaderProgram->set(featureThicknessArray[k], "uFeatureThickness" );

	//f_11 = 0;
	//for ( const auto gaborStripesCurvature : featureCurvatureArray )
	//{
		shaderProgram->set(featureCurvatureArray[k], "uFeatureCourbure" );

	//f_12 = 0;
	//for ( const auto gaborStripesOrientation : featureOrientationArray )
	//{
		shaderProgram->set(featureOrientationArray[k], "uFeatureDeltaorient" );
	
		//----------------------------------------------------------------------------------------------
		// FOR NEW code
		// - labeling
		shaderProgram->set( /*getNbLabels()*/0, "uNbLabels" );
		// -default translation
		shaderProgram->set( /*getShiftX()*/0.f, "uShiftX" );
		shaderProgram->set( /*getShiftY()*/0.f, "uShiftY" );
		// - labeling
		glBindImageTexture( 1/*unit*/, mPPTBFLabelMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R8UI );
		// - labeling
		glBindImageTexture( 2/*unit*/, mPPTBFRandomValueMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
		//----------------------------------------------------------------------------------------------

		//---------------------
		// Generate PPTBF image
		//---------------------

		std::cout << "generating image: " << ( k + 1 ) << "/" << pptbf_count << std::endl;
		
		// Launch mega-kernel
		glDispatchCompute( gridSizeX, gridSizeY, 1 );
		
		// Synchronization
		// - make sure writing to image has finished before read
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
		glMemoryBarrier( GL_ALL_BARRIER_BITS );

		//-------------------
		// Export PPTBF image
		//-------------------
								
		// Retrieve data on host
		std::vector< float > f_pptbf( width * height );
#if 1
		// - deal with odd texture dimensions
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
#endif
		glGetTextureImage( mPPTBFTexture, 0/*level*/, GL_RED, GL_FLOAT, sizeof( float ) * width * height, f_pptbf.data() );

		//-------------------
		// make approximate histogramm equalization
		//-------------------
		
		float f_avg = 0.0; float f_min = 0.0, f_max = 0.0;
		int count = 0;
		for (int i = 0; i < width * height; i++)
		{
			if (!isnan<float>(f_pptbf[i]))
			{
				f_avg += f_pptbf[i]; count++;
				if (i == 0) { f_min = f_avg; f_max = f_avg; }
				if (f_min > f_pptbf[i]) f_min = f_pptbf[i];
				if (f_max < f_pptbf[i]) f_max = f_pptbf[i];
			}
		}
		f_avg /= (float)(count);
		//std::cout << "avg=" << f_avg << ", min=" << f_min << ", max=" << f_max << "\n";
		//int cmin = 0, cmax = 0;
		//float f_avgmin[3] = { 0.0, 0.0, 0.0 }, f_avgmax[3] = { 0.0,0.0,0.0 };
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] < f_avg) { f_avgmin[1] += f_pptbf[i]; cmin++; }
		//		else { f_avgmax[1] += f_pptbf[i]; cmax++; }
		//	}
		//}
		//f_avgmin[1] /= (float)cmin; f_avgmax[1] /= (float)cmax;

		//cmin = 0; cmax = 0;
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] < f_avg)
		//		{
		//			if (f_pptbf[i] < f_avgmin[1]) { f_avgmin[0] += f_pptbf[i]; cmin++; }
		//			else { f_avgmin[2] += f_pptbf[i]; cmax++; }
		//		}
		//	}
		//}
		//f_avgmin[0] /= (float)cmin; f_avgmin[2] /= (float)cmax;
		//cmin = 0; cmax = 0;
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] > f_avg)
		//		{
		//			if (f_pptbf[i] < f_avgmax[1]) { f_avgmax[0] += f_pptbf[i]; cmin++; }
		//			else { f_avgmax[2] += f_pptbf[i]; cmax++; }
		//		}
		//	}
		//}
		//f_avgmax[0] /= (float)cmin; f_avgmax[2] /= (float)cmax;

		const int bins = 100;
		float histo[bins];
		for (int i = 0; i < bins; i++) histo[i] = 0.0f;
		for (int i = 0; i < width * height; i++)
		{
			if (f_pptbf[i] < f_avg) f_pptbf[i] = (f_pptbf[i] - f_min) / (f_avg - f_min)*0.5;
			else f_pptbf[i] = (f_pptbf[i] - f_avg) / (f_max - f_avg)*0.5+0.5;
			
			int iv = (int)((float)(bins)*f_pptbf[i]);
			if (iv >= bins) iv = bins - 1;
			histo[iv] += 1.0f / (float)(width * height);

			//if (f_pptbf[i] < f_avgmin[0]) f_pptbf[i] = (f_pptbf[i] - f_min) / (f_avgmin[0] - f_min)*0.125;
			//else if (f_pptbf[i] < f_avgmin[1]) f_pptbf[i] = (f_pptbf[i] - f_avgmin[0]) / (f_avgmin[1] - f_avgmin[0])*0.125+0.125;
			//else if (f_pptbf[i] < f_avgmin[2]) f_pptbf[i] = (f_pptbf[i] - f_avgmin[1]) / (f_avgmin[2] - f_avgmin[1])*0.125 + 0.25;
			//else if (f_pptbf[i] < f_avg) f_pptbf[i] = (f_pptbf[i] - f_avgmin[2]) / (f_avg - f_avgmin[2])*0.125+0.375;
			//else if (f_pptbf[i] < f_avgmax[0]) f_pptbf[i] = (f_pptbf[i] - f_avg) / (f_avgmax[0] - f_avg)*0.125 + 0.5;
			//else if (f_pptbf[i] < f_avgmax[1]) f_pptbf[i] = (f_pptbf[i] - f_avgmax[0]) / (f_avgmax[1] - f_avgmax[0])*0.125 + 0.625;
			//else if (f_pptbf[i] < f_avgmax[2]) f_pptbf[i] = (f_pptbf[i] - f_avgmax[1]) / (f_avgmax[2] - f_avgmax[1])*0.125 + 0.75;
			//else f_pptbf[i] = (f_pptbf[i] - f_avgmax[2]) / (f_max - f_avgmax[2])*0.125 + 0.875;
		}
		int i;
		float sum = 0.0;
		for (i = 0; i < bins && sum < percent; i++) sum += histo[i];
		//if (i == bins) return vmax;
		float ratio = (percent - (sum - histo[i])) / histo[i];
		//float ratio = 0.5f;
		//printf("compute Thresh bin =%d, sum=%g, percent=%g\n", i, sum, percent);
		float rangea =  ((float)(i - 1) / (float)bins + ratio / (float)bins);

		keep[k] = true;
#ifdef REFINEMENT
		for (int i = 0; i < width * height; i++)
		{
			if (f_pptbf[i] > rangea) f_pptbf[i] = 1.0;
			else f_pptbf[i] = 0.0;
		}
#else
		for (int i = 0; i < width-1; i+=4) for (int j = 0; j < height-1; j+=4) similarity[k].push_back((unsigned char)(255.0/4.0*(f_pptbf[i+j*width]+ f_pptbf[i+1 + j*width]+ f_pptbf[i + (j+1)*width]+ f_pptbf[i+1 + (j+1)*width])));
		int r;
		for (r = 0; r < k && keep[k]; r++)
		{
			if (keep[r])
			{
				float mse = 0.0;
				for (int i = 0; i < width*height / 16; i++) mse += ((float)similarity[k][i] - (float)similarity[r][i])*((float)similarity[k][i] - (float)similarity[r][i]);
				if (sqrt(mse) < (float)(width*height) / 16.0 * KEEP_ERROR) keep[k] = false;
				//printf("error %d - %d is %g\n", k, r, sqrt(mse));
			}
		}
#endif
		if (keep[k])
		{
			// Deformation
			turbAmplitude0 = turbulenceAmplitude0Array[k];
			turbAmplitude1 = turbulenceAmplitude1Array[k];
			turbAmplitude2 = turbulenceAmplitude2Array[k];

			// Model Transform
			resolution = modelResolutionArray[k];
			rotation = modelAlphaArray[k] / M_PI;
			rescalex = modelRescalexArray[k];

			// Point process
			do_tiling = tilingTypeArray[k];
			jittering = jitteringArray[k];

			// Window function
			windowShape  = windowShapeArray[k];
			windowArity = windowArityArray[k];
			windowLarp = windowLarpArray[k];
			windowNorm = windowNormArray[k];
			windowSmoothness = windowSmoothArray[k];
			windowBlend = windowBlendArray[k];
			windowSigwcell = windowSigwcellArray[k];

			// Feature function
			featureBombing = featureBombingArray[k];
			featureNorm = featureNormArray[k];
			featureWinfeatcorrel = featureWinfeatcorrelArray[k];
			featureAniso = featureAnisoArray[k];
			featureMinNbKernels = featureMinNbKernelsArray[k];
			featureMaxNbKernels = featureMaxNbKernelsArray[k];
			featureSigcos = featureSigcosArray[k];
			featureSigcosvar = featureSigcosvarArray[k];
			featureFrequency = featureFrequencyArray[k];
			featurePhaseShift = featurePhaseShiftArray[k]/M_PI/0.5;
			featureThickness = featureThicknessArray[k];
			featureCurvature = featureCurvatureArray[k];
			featureOrientation = featureOrientationArray[k]/M_PI;
#ifdef REFINEMENT
			fprintf(fdn, "%02d %02d %d %d %d\n", 0, do_tiling, 0, 0, 0);
			fprintf(fdn, "%06d %g %d %d\n", pptbf_keep_count, 0.0, 0, 0);
			fprintf(fdn, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_keep_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
				windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
				featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
#else
			fprintf(fdn, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_keep_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
				windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
				featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
#endif
			pptbf_keep_count++;
			fflush(fd);
		}
//		else printf("erased %d as it looks like %d\n", k, r-1);

		if (keep[k])
		{
			// Convert data (float to unsigned char)
		// TODO: maybe use for_each with a lambda doing the "cast" directly instead?
			std::transform(f_pptbf.begin(), f_pptbf.end(), f_pptbf.begin(), std::bind(std::multiplies< float >(), std::placeholders::_1, 255.f));
			std::vector< unsigned char > u_pptbf(f_pptbf.begin(), f_pptbf.end());
			// Save data in image
			//const std::string pptbfName = std::string( "pptbf_" )/*name*/ + std::to_string( i );
			char numbuff[100]; 
#ifdef REFINEMENT
			sprintf(numbuff, "_%d_%06d", descriptor,pptbf_keep_count - 1);
#else
			sprintf(numbuff, "_%02d_%02d_%d_%d_%d_%06d", do_serie, do_tiling, do_window, do_feature, do_phase, pptbf_keep_count-1);
#endif
			const std::string pptbfName = std::string("pptbf")/*name*/
				+ std::string(numbuff);
			//+ std::string( "_D_" ) + std::to_string( d_0 ) + std::string( "_" ) + std::to_string( d_1 ) + std::string( "_" ) + std::to_string( d_2 )
			//+ std::string( "_M_" ) + std::to_string( m_0 ) + std::string( "_" ) + std::to_string( m_1 ) + std::string( "_" ) + std::to_string( m_2 )
			//+ std::string( "_P_" ) + std::to_string( p_0 ) + std::string( "_" ) + std::to_string( p_1 )
			//+ std::string( "_W_" ) + std::to_string( w_0 ) + std::string( "_" ) + std::to_string( w_1 ) + std::string( "_" ) + std::to_string( w_2 )
			//					   + std::string( "_" ) + std::to_string( w_3 ) + std::string( "_" ) + std::to_string( w_4 ) + std::string( "_" ) + std::to_string( w_5 )
			//					   + std::string( "_" ) + std::to_string( w_6 )
			//+ std::string( "_F_" ) + std::to_string( f_0 ) + std::string( "_" ) + std::to_string( f_1 ) + std::string( "_" ) + std::to_string( f_2 )
			//					   + std::string( "_" ) + std::to_string( f_3 ) + std::string( "_" ) + std::to_string( f_4 ) + std::string( "_" ) + std::to_string( f_5 )
			//					   + std::string( "_" ) + std::to_string( f_6 ) + std::string( "_" ) + std::to_string( f_7 ) + std::string( "_" ) + std::to_string( f_8 )
			//					   + std::string( "_" ) + std::to_string( f_9 ) + std::string( "_" ) + std::to_string( f_10 ) + std::string( "_" ) + std::to_string( f_11 )
			//					   + std::string( "_" ) + std::to_string( f_12 );
			const std::string pptbfFilename = databasePath + pptbfName + std::string(".png");
			PtImageHelper::saveImage(pptbfFilename.c_str(), width, height, 1/*nb channels*/, u_pptbf.data());
		}
		
		// Update counter
		//i++;
	/*}
	}
	}
	}
	}
	}
	}
	}
	}
	}
	}
	}
	}
		w_6++;
	}
		w_5++;
	}
		w_4++;
	}
		w_3++;
	}
		w_2++;
	}
		w_1++;
	}
		w_0++;
	}
		p_1++;
	}
		p_0++;
	}
		m_2++;
	}
		m_1++;
	}
		m_0++;
	}
		d_2++;
	}
		d_1++;
	}
		d_0++;
	}
	*/
	}
	fclose(fdn);
	// Reset GL state(s)
	// - Unset shader program
	PtShaderProgram::unuse();

	// LOG info
	std::cout << "Finished..." << pptbf_count << std::endl;
}

/******************************************************************************
 * sampleParameterSpace
 ******************************************************************************/
void PtGraphicsPPTBF::sampleParameterSpace( const unsigned int pWidth, const unsigned int pHeight, const char* pPath, const char* pImageName,
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
	std::vector< int >& tilingTypeArray,
	std::vector< float >& jitteringArray,
	// - Transform
	std::vector< int >& modelResolutionArray,
	std::vector< float >& modelAlphaArray,
	std::vector< float >& modelRescalexArray,
	// - Turbulence
	std::vector< float >& turbulenceAmplitude0Array,
	std::vector< float >& turbulenceAmplitude1Array,
	std::vector< float >& turbulenceAmplitude2Array,
	std::vector< std::vector< float > >& turbAmp,
	// - Window
	std::vector< int >& windowShapeArray,
	std::vector< float >& windowArityArray,
	std::vector< float >& windowLarpArray,
	std::vector< float >& windowNormArray,
	std::vector< float >& windowSmoothArray,
	std::vector< float >& windowBlendArray,
	std::vector< float >& windowSigwcellArray,
	// - Feature
	std::vector< int >& featureBombingArray,
	std::vector< float >& featureNormArray,
	std::vector< float >& featureWinfeatcorrelArray,
	std::vector< float >& featureAnisoArray,
	std::vector< int >& featureMinNbKernelsArray,
	std::vector< int >& featureMaxNbKernelsArray,
	std::vector< float >& featureSigcosArray,
	std::vector< float >& featureSigcosvarArray,
	std::vector< int >& featureFrequencyArray,
	std::vector< float >& featurePhaseShiftArray,
	std::vector< float >& featureThicknessArray,
	std::vector< float >& featureCurvatureArray,
	std::vector< float >& featureOrientationArray,
	// save bank ID
	std::vector< int >& featureBankIDs )
{
	assert( pWidth != 0 && pHeight != 0 );

	//-----------------------------------------------------------------------------------------

	featureBankIDs.clear();

	mSelectedNNCandidates.clear();
	mPptbfCandidateThresholds.clear();
	mPptbfCandidateMinValues.clear();
	mPptbfCandidateMaxValues.clear();
	mPptbfCandidateMeanValues.clear();
	// PPTBF
	// - point process
	tilingTypeArray.clear(),
	jitteringArray.clear();
	// - transform
	modelResolutionArray.clear();
	modelAlphaArray.clear();
	modelRescalexArray.clear();
	// - turbulence
	turbulenceAmplitude0Array.clear();
	turbulenceAmplitude1Array.clear();
	turbulenceAmplitude2Array.clear();
	turbAmp.clear();
	// - window
	windowShapeArray.clear();
	windowArityArray.clear();
	windowLarpArray.clear();
	windowNormArray.clear();
	windowSmoothArray.clear();
	windowBlendArray.clear();
	windowSigwcellArray.clear();
	// - feature
	featureBombingArray.clear();
	featureNormArray.clear();
	featureWinfeatcorrelArray.clear();
	featureAnisoArray.clear();
	featureMinNbKernelsArray.clear();
	featureMaxNbKernelsArray.clear();
	featureSigcosArray.clear();
	featureSigcosvarArray.clear();
	featureFrequencyArray.clear();
	featurePhaseShiftArray.clear();
	featureThicknessArray.clear();
	featureCurvatureArray.clear();
	featureOrientationArray.clear();

	//-----------------------------------------------------------------------------------------
	
	unsigned int pSerieID;

	// LOG info
	std::cout << "\nPPTBF Database generation..." << std::endl;
	std::cout << "START..." << std::endl;

	const int width = pWidth;
	const int height = pHeight;

	// Resize graphics resources
	onSizeModified(width, height);

	//const std::string databasePath = PtEnvironment::mDataPath + std::string( "/BDDStructure/Test/" );
	const std::string databasePath = pPath;

	// Deformation
	//std::vector< float > turbulenceAmplitude0Array;
	//std::vector< float > turbulenceAmplitude1Array;
	//std::vector< float > turbulenceAmplitude2Array;

	// Model Transform
	//std::vector< int > modelResolutionArray;
	//std::vector< float > modelAlphaArray;
	//std::vector< float > modelRescalexArray;

	// Point process
	//std::vector< int > tilingTypeArray;
	//std::vector< float > jitteringArray;

	// Window function
	//std::vector< int > windowShapeArray;
	//std::vector< float > windowArityArray;
	//std::vector< float > windowLarpArray;
	//std::vector< float > windowNormArray;
	//std::vector< float > windowSmoothArray;
	//std::vector< float > windowBlendArray;
	//std::vector< float > windowSigwcellArray;

	// Feature function
	//std::vector< int > featureBombingArray;
	//std::vector< float > featureNormArray;
	//std::vector< float > featureWinfeatcorrelArray;
	//std::vector< float > featureAnisoArray;
	//std::vector< int > featureMinNbKernelsArray;
	//std::vector< int > featureMaxNbKernelsArray;
	//std::vector< float > featureSigcosArray;
	//std::vector< float > featureSigcosvarArray;
	//std::vector< int > featureFrequencyArray;
	//std::vector< float > featurePhaseShiftArray;
	//std::vector< float > featureThicknessArray;
	//std::vector< float > featureCurvatureArray;
	//std::vector< float > featureOrientationArray;

	///////////////////////////////////////////
	// USER Experiment
	// - sampling the space of PPTBF structures
	///////////////////////////////////////////

	const int descriptor = pDescriptor; // default: 1
	const int KEEPNBEST = pKEEPNBEST; // default: 5
	const int NR = pNR; // default: 10
	float deltaalpha = pDeltaAlpha; // default: 0.0    and 0.5 = pi/2
	float factresol = pFactResol; // default: 1.0
	float factrescalex = pFactRescaleX; // default: 1.0
	float ecart = pEcart; // default: 0.5
	//char *exname = "TexturesCom_Asphalt11_2x2_1K_seg_scrop_80";
	//char *exname = "TexturesCom_Pavement_CobblestoneForest01_4x4_1K_albedo_seg_scrop";
	//char *exname = "20181215_153153_seg_scrop_450";
	std::string imageName = std::string( pImageName );
	const char* exname = imageName.c_str();
	
	const int SS = 400;
	char pname[128];
	int pptbf_count = 0;
	
	float percent = pPercent; // default 0.2

	int do_serie = 0;

	int do_tiling = 0;
	int do_window = 0;
	int do_feature = 0;
	int do_phase = 0;
	
	float jittering, wjittering, fjittering;
	int resolution, wresolution, fresolution;
	float rotation, wrotation, frotation;
	float rescalex, wrescalex, frescalex;
	float turbAmplitude0, turbAmplitude1, turbAmplitude2;
	float wturbAmplitude0, wturbAmplitude1, wturbAmplitude2;
	float fturbAmplitude0, fturbAmplitude1, fturbAmplitude2;

	int windowShape = do_window, wwindowShape, fwindowShape;
	float windowArity = 10.0, wwindowArity, fwindowArity;
	float windowLarp = 0.0, wwindowLarp, fwindowLarp;
	float windowNorm = 2.0, wwindowNorm, fwindowNorm;
	float windowSmoothness = 0.0, wwindowSmoothness, fwindowSmoothness;
	float windowBlend = 1.0, wwindowBlend, fwindowBlend;
	float windowSigwcell = 0.7, wwindowSigwcell, fwindowSigwcell;

	int featureBombing = 0, wfeatureBombing, ffeatureBombing;
	float featureNorm = 2.0, wfeatureNorm, ffeatureNorm;
	float featureWinfeatcorrel = 0.0, wfeatureWinfeatcorrel, ffeatureWinfeatcorrel;
	float featureAniso = 0.0, wfeatureAniso, ffeatureAniso;
	int featureMinNbKernels = 0, featureMaxNbKernels = 0;
	int wfeatureMinNbKernels = 0, wfeatureMaxNbKernels = 0;
	int ffeatureMinNbKernels = 0, ffeatureMaxNbKernels = 0;
	float featureSigcos = 1.0, wfeatureSigcos, ffeatureSigcos;
	float featureSigcosvar = 0.2, wfeatureSigcosvar, ffeatureSigcosvar;
	int featureFrequency = 0, wfeatureFrequency, ffeatureFrequency;
	float featurePhaseShift = (float)do_phase, wfeaturePhaseShift, ffeaturePhaseShift;
	float featureThickness = 1.0, wfeatureThickness, ffeatureThickness;
	float featureCurvature = 0.0, wfeatureCurvature, ffeatureCurvature;
	float featureOrientation = 0.0, wfeatureOrientation, ffeatureOrientation;

	sprintf(pname, "%s/pptbf_matching_rnd_%d.txt", pPath, descriptor);

	FILE *fdn = fopen(pname, "w");

	int pptbf_countf = 0;
	sprintf(pname, "%s/%s_best_pptbfparams_%d.txt", pPath, exname, descriptor);
	FILE *fd = fopen(pname, "r");
	//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	//FILE *fdn = fopen(pname, "w");
	//fprintf(fdn, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");
	char buff1[2048], buff2[2048], buff3[2048];
	int num;
	fgets(buff1, 1000, fd);
	sscanf(buff1, "%g", &percent);
	percent = 1.0 - percent;
	fgets(buff1, 1000, fd); fgets(buff2, 1000, fd); fgets(buff3, 1000, fd);
	bool cont = true;
	do { 
		sscanf(buff1, "%02d %02d %d %d %d", &do_serie, &do_tiling, &do_window, &do_feature, &do_phase);
		sscanf(buff3, "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", 
			&num, &jittering, &resolution, &rotation, &rescalex, &turbAmplitude0, &turbAmplitude1, &turbAmplitude2,
			&windowShape, &windowArity, &windowLarp, &windowNorm, &windowSmoothness, &windowBlend, &windowSigwcell,
			&featureBombing, &featureNorm, &featureWinfeatcorrel, &featureAniso, &featureMinNbKernels, &featureMaxNbKernels, &featureSigcos, &featureSigcosvar, &featureFrequency, &featurePhaseShift, &featureThickness, &featureCurvature, &featureOrientation);
		if (pptbf_countf < KEEPNBEST)
		{
			cont = true;
			bool initial = true;
			for (int iresolution = 0; iresolution < NR; iresolution++)
			//for (int iresolution = 0; iresolution <= 0; iresolution++)
			{
				int nresolution = (int)(factresol*(double)resolution*(1.0f + (initial ? 0.0 : 0.25*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				float zoom = (float)SS / (float)nresolution;
				float njittering = jittering*(1.0f + (initial ? 0.0 : ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (iresolution >= NR / 2 && !initial) njittering = (float)rand() / (float)RAND_MAX;
				if (njittering >= 1.0) njittering = 0.99;
				float nrescalex = factrescalex*rescalex*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				
				float turbAmplitude0A[] = { 0.0, 0.06,  0.05, 0.03, 0.06, 0.05, 0.02 };
				float turbAmplitude1A[] = { 0.0, 0.05, 0.5, 0.8, 0.05, 0.5, 0.5 };
				float turbAmplitude2A[] = { 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 4.0  };

				float nturbAmplitude0 = turbAmplitude0*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				float nturbAmplitude1 = turbAmplitude1*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				float nturbAmplitude2 = turbAmplitude2*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (iresolution >= NR / 2 && !initial) nturbAmplitude0 = turbAmplitude0A[(iresolution - 1) % 6+1]* (1.0+0.2*ecart*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f))* (float)nresolution / (50.0 / 1.0);
				if (iresolution >= NR / 2 && !initial) nturbAmplitude1 = turbAmplitude1A[(iresolution - 1) % 6+1] * (1.0 + 0.2*ecart*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (iresolution >= NR / 2 && !initial) nturbAmplitude2 = turbAmplitude2A[(iresolution - 1) % 6+1] * (1.0 + 0.2*ecart*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));

				// Deformation
				turbulenceAmplitude0Array.push_back(nturbAmplitude0);
				turbulenceAmplitude1Array.push_back(nturbAmplitude1);
				turbulenceAmplitude2Array.push_back(nturbAmplitude2);

				// Model Transform
				modelResolutionArray.push_back(nresolution);
				modelAlphaArray.push_back((rotation+ deltaalpha) * M_PI);
				modelRescalexArray.push_back(nrescalex);

				// Point process
				tilingTypeArray.push_back(do_tiling);
				jitteringArray.push_back(njittering);

				// Window function
				float nwindowArity= windowArity*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f));
				if (nwindowArity <= 3.0) nwindowArity = 3.0;
				nwindowArity = floor(nwindowArity);
				windowShapeArray.push_back(windowShape);
				windowArityArray.push_back(windowArity);
				windowLarpArray.push_back(windowLarp*(1.0f + (initial ? 0.0 : 0.2*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				windowNormArray.push_back(windowNorm);
				windowSmoothArray.push_back(windowSmoothness*(1.0f + (initial ? 0.0 : 0.2*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				windowBlendArray.push_back(windowBlend);
				windowSigwcellArray.push_back(windowSigwcell);

				// Feature function
				featureBombingArray.push_back(featureBombing);
				featureNormArray.push_back(featureNorm);
				featureWinfeatcorrelArray.push_back(featureWinfeatcorrel*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				featureAnisoArray.push_back(featureAniso);
				featureMinNbKernelsArray.push_back(featureMinNbKernels);
				int nfeatureMaxNbKernels = (int)((float)featureMaxNbKernels*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)) + 0.5);
				if (nfeatureMaxNbKernels < featureMinNbKernels) nfeatureMaxNbKernels = featureMinNbKernels;
				featureMaxNbKernelsArray.push_back(nfeatureMaxNbKernels);
				featureSigcosArray.push_back(featureSigcos);
				featureSigcosvarArray.push_back(featureSigcosvar);
				featureFrequencyArray.push_back(featureFrequency*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				featurePhaseShiftArray.push_back(featurePhaseShift*M_PI*0.5);
				featureThicknessArray.push_back(featureThickness*(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));
				featureCurvatureArray.push_back(featureCurvature);
				featureOrientationArray.push_back(M_PI * featureOrientation *(1.0f + (initial ? 0.0 : 0.5*ecart)*(2.0f*(float)rand() / (float)RAND_MAX - 1.0f)));

				//fprintf(fdn, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
				//	windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
				//	featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);
				pptbf_count++;
				initial = false;

				// Keep track of feature bank ID
				featureBankIDs.push_back( do_serie );
			}
			
			fgets( buff1, 1000, fd );
			fgets( buff2, 1000, fd );
			fgets( buff3, 1000, fd );
			
			pptbf_countf++;
		}
		else
		{
			cont = false;
		}
	}
	while ( cont );

	//	fclose(fdn);
	fclose(fd);

	std::vector< std::vector< unsigned char > > similarity( pptbf_count );
	int pptbf_keep_count = 0;
	bool* keep = new bool[ pptbf_count ];
	//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	//sprintf(pname, "%s/pptbf_%02d_%02d_%d_%d_%d.txt", pPath, do_serie, do_tiling, do_window, do_feature, do_phase);
	//fd = fopen(pname, "w");
	//fprintf(fd, "num jittering zoom rotation rescalex turbAmplitude0 turbAmplitude1 turbAmplitude2 windowShape windowArity windowLarp windowNorm windowSmoothness windowBlend windowSigwcell featureBombing featureNorm featureWinfeatcorrel featureAniso featureMinNbKernels featureMaxNbKernels featureSigcos featureSigcosvar featureFrequency featurePhaseShift featureThickness featureCurvature featureOrientation\n");

	///////////////////////
	// Set common GL states
	///////////////////////

	//setImageWidth( width );
	//setImageHeight( height );
	setWidth( width );
	setHeight( height );

	// Set shader program
	PtShaderProgram* shaderProgram = mMegakernelShaderProgram;
	shaderProgram->use();

	// Set texture(s)
	// - PRNG
	glBindTextureUnit( 0/*unit*/, mTexture_P );
	// - noise
	glBindTextureUnit( 1/*unit*/, mTexture_rnd_tab );

	// Set sampler(s)
	// - PRNG
	glBindSampler( 0/*unit*/, mSampler_PRNG_Noise );
	// - noise
	glBindSampler( 1/*unit*/, mSampler_PRNG_Noise );

	// Set image(s)
	// - PPTBF (output)
	glBindImageTexture( 0/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );

	// Set uniform(s)
	// - PRNG
	shaderProgram->set( 0, "uPermutationTex" );
	// - noise
	shaderProgram->set( 1, "uNoiseTex" );

	// Kernel configuration
	// - block
	const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
	const int blockSizeY = 8;
	// - grid
	const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
	const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );

	/////////////////////////////////////////
	// Sampling the space of PPTBF structures
	/////////////////////////////////////////
	
	// Global PPTBF counter

	fprintf(fdn, "%g\n", 1.0 - percent);

	for (int k = 0; k<pptbf_count; k++)
	{

	////////////////
	// Point Process
	////////////////
	
	//p_0 = 0;
	//for ( const auto tilingType : tilingTypeArray )
	//{
		shaderProgram->set(tilingTypeArray[k], "uPointProcessTilingType" );

	//p_1 = 0;
	//for ( const auto jittering : jitteringArray )
	//{
		shaderProgram->set(jitteringArray[k], "uPointProcessJitter" );

	//////////////////
	// Model Transform
	//////////////////

	//m_0 = 0;
	//for ( const auto modelResolution : modelResolutionArray )
	//{
		shaderProgram->set(modelResolutionArray[k], "uResolution" );

	//m_1 = 0;
	//for ( const auto modelAlpha : modelAlphaArray )
	//{
		shaderProgram->set(modelAlphaArray[k], "uRotation" );

	//m_2 = 0;
	//for ( const auto modelRescalex : modelRescalexArray )
	//{
		shaderProgram->set( modelRescalexArray[k], "uRescaleX" );	
		
	//////////////
	// Deformation
	//////////////

	//d_0 = 0;
	//for ( const auto deformation_0 : turbulenceAmplitude0Array )
	//{
		shaderProgram->set(turbulenceAmplitude0Array[k], "uTurbulenceAmplitude_0" );

	//d_1 = 0;
	//for ( const auto deformation_1 : turbulenceAmplitude1Array )
	//{
		shaderProgram->set(turbulenceAmplitude1Array[k], "uTurbulenceAmplitude_1" );

	//d_2 = 0;
	//for ( const auto deformation_2 : turbulenceAmplitude2Array )
	//{
		shaderProgram->set(turbulenceAmplitude2Array[k], "uTurbulenceAmplitude_2" );
	
	//////////////////
	// Window Function
	//////////////////

	//w_0 = 0;
	//for ( const auto windowShape : windowShapeArray )
	//{
		shaderProgram->set(windowShapeArray[k], "uWindowShape" );

	//w_1 = 0;
	//for ( const auto windowArity : windowArityArray )
	//{
		shaderProgram->set(windowArityArray[k], "uWindowArity" );

	//w_2 = 0;
	//for ( const auto windowLarp : windowLarpArray )
	//{
		shaderProgram->set(windowLarpArray[k], "uWindowLarp" );

	//w_3 = 0;
	//for ( const auto windowNorm : windowNormArray )
	//{
		shaderProgram->set(windowNormArray[k], "uWindowNorm" );

	//w_4 = 0;
	//for ( const auto windowSmooth : windowSmoothArray )
	//{
		shaderProgram->set(windowSmoothArray[k], "uWindowSmooth" );

	//w_5 = 0;
	//for ( const auto windowBlend : windowBlendArray )
	//{
		shaderProgram->set(windowBlendArray[k], "uWindowBlend" );

	//w_6 = 0;
	//for ( const auto windowSigwcell : windowSigwcellArray )
	//{
		shaderProgram->set(windowSigwcellArray[k], "uWindowSigwcell" );

	///////////////////
	// Feature Function
	///////////////////

	
	//f_0 = 0;
	//for ( const auto featureBombing : featureBombingArray )
	//{
		shaderProgram->set(featureBombingArray[k], "uFeatureBomb" );

	//f_1 = 0;
	//for ( const auto featureNorm : featureNormArray )
	//{
		shaderProgram->set(featureNormArray[k], "uFeatureNorm" );

	//f_2 = 0;
	//for ( const auto featureWinfeatcorrel : featureWinfeatcorrelArray )
	//{
		shaderProgram->set(featureWinfeatcorrelArray[k], "uFeatureWinfeatcorrel" );

	//f_3 = 0;
	//for ( const auto featureAniso : featureAnisoArray )
	//{
		shaderProgram->set(featureAnisoArray[k], "uFeatureAniso" );

	//f_4 = 0;
	//for ( const auto minNbGaborKernels : featureMinNbKernelsArray )
	//{
		shaderProgram->set(featureMinNbKernelsArray[k], "uFeatureNpmin" );

	//f_5 = 0;
	//for ( const auto maxNbGaborKernels : featureMaxNbKernelsArray )
	//{
		shaderProgram->set(featureMaxNbKernelsArray[k], "uFeatureNpmax" );

	//f_6 = 0;
	//for ( const auto featureSigcos : featureSigcosArray )
	//{
		shaderProgram->set(featureSigcosArray[k], "uFeatureSigcos" );

	//f_7 = 0;
	//for ( const auto featureSigcosvar : featureSigcosvarArray )
	//{
		shaderProgram->set(featureSigcosvarArray[k], "uFeatureSigcosvar" );

	//f_8 = 0;
	//for ( const auto gaborStripesFrequency : featureFrequencyArray )
	//{
		shaderProgram->set(featureFrequencyArray[k], "uFeatureFreq" );

	//f_9 = 0;
	//for ( const auto featurePhase : featurePhaseShiftArray )
	//{
		shaderProgram->set(featurePhaseShiftArray[k], "uFeaturePhase" );

	//f_10 = 0;
	//for ( const auto gaborStripesThickness : featureThicknessArray )
	//{
		shaderProgram->set(featureThicknessArray[k], "uFeatureThickness" );

	//f_11 = 0;
	//for ( const auto gaborStripesCurvature : featureCurvatureArray )
	//{
		shaderProgram->set(featureCurvatureArray[k], "uFeatureCourbure" );

	//f_12 = 0;
	//for ( const auto gaborStripesOrientation : featureOrientationArray )
	//{
		shaderProgram->set(featureOrientationArray[k], "uFeatureDeltaorient" );
	
		//----------------------------------------------------------------------------------------------
		// FOR NEW code
		// - labeling
		shaderProgram->set( /*getNbLabels()*/0, "uNbLabels" );
		// -default translation
		shaderProgram->set( /*getShiftX()*/0.f, "uShiftX" );
		shaderProgram->set( /*getShiftY()*/0.f, "uShiftY" );
		// - labeling
		glBindImageTexture( 1/*unit*/, mPPTBFLabelMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R8UI );
		// - labeling
		glBindImageTexture( 2/*unit*/, mPPTBFRandomValueMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
		//----------------------------------------------------------------------------------------------

		//---------------------
		// Generate PPTBF image
		//---------------------

		std::cout << "generating image: " << ( k + 1 ) << "/" << pptbf_count << std::endl;
		
		// Launch mega-kernel
		glDispatchCompute( gridSizeX, gridSizeY, 1 );
		
		// Synchronization
		// - make sure writing to image has finished before read
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
		glMemoryBarrier( GL_ALL_BARRIER_BITS );

		//-------------------
		// Export PPTBF image
		//-------------------
								
		// Retrieve data on host
		std::vector< float > f_pptbf( width * height );
#if 1
		// - deal with odd texture dimensions
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
#endif
		glGetTextureImage( mPPTBFTexture, 0/*level*/, GL_RED, GL_FLOAT, sizeof( float ) * width * height, f_pptbf.data() );

		//-------------------
		// make approximate histogramm equalization
		//-------------------
		
		float f_avg = 0.0; float f_min = 0.0, f_max = 0.0;
		int count = 0;
		for (int i = 0; i < width * height; i++)
		{
			if (!isnan<float>(f_pptbf[i]))
			{
				f_avg += f_pptbf[i]; count++;
				if (i == 0) { f_min = f_avg; f_max = f_avg; }
				if (f_min > f_pptbf[i]) f_min = f_pptbf[i];
				if (f_max < f_pptbf[i]) f_max = f_pptbf[i];
			}
		}
		f_avg /= (float)(count);
		//std::cout << "avg=" << f_avg << ", min=" << f_min << ", max=" << f_max << "\n";
		//int cmin = 0, cmax = 0;
		//float f_avgmin[3] = { 0.0, 0.0, 0.0 }, f_avgmax[3] = { 0.0,0.0,0.0 };
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] < f_avg) { f_avgmin[1] += f_pptbf[i]; cmin++; }
		//		else { f_avgmax[1] += f_pptbf[i]; cmax++; }
		//	}
		//}
		//f_avgmin[1] /= (float)cmin; f_avgmax[1] /= (float)cmax;

		//cmin = 0; cmax = 0;
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] < f_avg)
		//		{
		//			if (f_pptbf[i] < f_avgmin[1]) { f_avgmin[0] += f_pptbf[i]; cmin++; }
		//			else { f_avgmin[2] += f_pptbf[i]; cmax++; }
		//		}
		//	}
		//}
		//f_avgmin[0] /= (float)cmin; f_avgmin[2] /= (float)cmax;
		//cmin = 0; cmax = 0;
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] > f_avg)
		//		{
		//			if (f_pptbf[i] < f_avgmax[1]) { f_avgmax[0] += f_pptbf[i]; cmin++; }
		//			else { f_avgmax[2] += f_pptbf[i]; cmax++; }
		//		}
		//	}
		//}
		//f_avgmax[0] /= (float)cmin; f_avgmax[2] /= (float)cmax;

		const int bins = 100;
		float histo[bins];
		for (int i = 0; i < bins; i++) histo[i] = 0.0f;
		for (int i = 0; i < width * height; i++)
		{
			if (f_pptbf[i] < f_avg) f_pptbf[i] = (f_pptbf[i] - f_min) / (f_avg - f_min)*0.5;
			else f_pptbf[i] = (f_pptbf[i] - f_avg) / (f_max - f_avg)*0.5+0.5;
			
			int iv = (int)((float)(bins)*f_pptbf[i]);
			if (iv >= bins) iv = bins - 1;
			histo[iv] += 1.0f / (float)(width * height);

			//if (f_pptbf[i] < f_avgmin[0]) f_pptbf[i] = (f_pptbf[i] - f_min) / (f_avgmin[0] - f_min)*0.125;
			//else if (f_pptbf[i] < f_avgmin[1]) f_pptbf[i] = (f_pptbf[i] - f_avgmin[0]) / (f_avgmin[1] - f_avgmin[0])*0.125+0.125;
			//else if (f_pptbf[i] < f_avgmin[2]) f_pptbf[i] = (f_pptbf[i] - f_avgmin[1]) / (f_avgmin[2] - f_avgmin[1])*0.125 + 0.25;
			//else if (f_pptbf[i] < f_avg) f_pptbf[i] = (f_pptbf[i] - f_avgmin[2]) / (f_avg - f_avgmin[2])*0.125+0.375;
			//else if (f_pptbf[i] < f_avgmax[0]) f_pptbf[i] = (f_pptbf[i] - f_avg) / (f_avgmax[0] - f_avg)*0.125 + 0.5;
			//else if (f_pptbf[i] < f_avgmax[1]) f_pptbf[i] = (f_pptbf[i] - f_avgmax[0]) / (f_avgmax[1] - f_avgmax[0])*0.125 + 0.625;
			//else if (f_pptbf[i] < f_avgmax[2]) f_pptbf[i] = (f_pptbf[i] - f_avgmax[1]) / (f_avgmax[2] - f_avgmax[1])*0.125 + 0.75;
			//else f_pptbf[i] = (f_pptbf[i] - f_avgmax[2]) / (f_max - f_avgmax[2])*0.125 + 0.875;
		}
		int i;
		float sum = 0.0;
		for (i = 0; i < bins && sum < percent; i++) sum += histo[i];
		//if (i == bins) return vmax;
		float ratio = (percent - (sum - histo[i])) / histo[i];
		//float ratio = 0.5f;
		//float ratio = 0.5f;
		//printf("compute Thresh bin =%d, sum=%g, percent=%g\n", i, sum, percent);
		float rangea =  ((float)(i - 1) / (float)bins + ratio / (float)bins);

		keep[k] = true;

#if 0
		for (int i = 0; i < width * height; i++)
		{
			if (f_pptbf[i] > rangea) f_pptbf[i] = 1.0;
			else f_pptbf[i] = 0.0;
		}
#endif
		if (keep[k])
		{
			// Deformation
			turbAmplitude0 = turbulenceAmplitude0Array[k];
			turbAmplitude1 = turbulenceAmplitude1Array[k];
			turbAmplitude2 = turbulenceAmplitude2Array[k];

			// Model Transform
			resolution = modelResolutionArray[k];
			rotation = modelAlphaArray[k] / M_PI;
			rescalex = modelRescalexArray[k];

			// Point process
			do_tiling = tilingTypeArray[k];
			jittering = jitteringArray[k];

			// Window function
			windowShape  = windowShapeArray[k];
			windowArity = windowArityArray[k];
			windowLarp = windowLarpArray[k];
			windowNorm = windowNormArray[k];
			windowSmoothness = windowSmoothArray[k];
			windowBlend = windowBlendArray[k];
			windowSigwcell = windowSigwcellArray[k];

			// Feature function
			featureBombing = featureBombingArray[k];
			featureNorm = featureNormArray[k];
			featureWinfeatcorrel = featureWinfeatcorrelArray[k];
			featureAniso = featureAnisoArray[k];
			featureMinNbKernels = featureMinNbKernelsArray[k];
			featureMaxNbKernels = featureMaxNbKernelsArray[k];
			featureSigcos = featureSigcosArray[k];
			featureSigcosvar = featureSigcosvarArray[k];
			featureFrequency = featureFrequencyArray[k];
			featurePhaseShift = featurePhaseShiftArray[k]/M_PI/0.5;
			featureThickness = featureThicknessArray[k];
			featureCurvature = featureCurvatureArray[k];
			featureOrientation = featureOrientationArray[k]/M_PI;

			fprintf(fdn, "%02d %02d %d %d %d\n", 0, do_tiling, 0, 0, 0);
			fprintf(fdn, "%06d %g %d %d\n", pptbf_keep_count, 0.0, 0, 0);
			fprintf(fdn, "%06d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n", pptbf_keep_count, jittering, resolution, rotation, rescalex, turbAmplitude0, turbAmplitude1, turbAmplitude2,
				windowShape, windowArity, windowLarp, windowNorm, windowSmoothness, windowBlend, windowSigwcell,
				featureBombing, featureNorm, featureWinfeatcorrel, featureAniso, featureMinNbKernels, featureMaxNbKernels, featureSigcos, featureSigcosvar, featureFrequency, featurePhaseShift, featureThickness, featureCurvature, featureOrientation);

			pptbf_keep_count++;
			fflush(fd);
		}
//		else printf("erased %d as it looks like %d\n", k, r-1);

		if (keep[k])
		{
			std::vector< float > f_pptbf_binary(width * height);
			f_pptbf_binary = f_pptbf;
			for (int i = 0; i < width * height; i++)
			{
				if (f_pptbf_binary[i] > rangea) f_pptbf_binary[i] = 1.0;
				else f_pptbf_binary[i] = 0.0;
			}

			// Convert data (float to unsigned char)
		// TODO: maybe use for_each with a lambda doing the "cast" directly instead?
			std::transform(f_pptbf.begin(), f_pptbf.end(), f_pptbf.begin(), std::bind(std::multiplies< float >(), std::placeholders::_1, 255.f));
			std::vector< unsigned char > u_pptbf(f_pptbf.begin(), f_pptbf.end());
			// Save data in image
			//const std::string pptbfName = std::string( "pptbf_" )/*name*/ + std::to_string( i );
			char numbuff[100]; 

			sprintf(numbuff, "_%d_%06d", descriptor,pptbf_keep_count - 1);

			const std::string pptbfName = std::string("pptbf")/*name*/
				+ std::string(numbuff);
			//+ std::string( "_D_" ) + std::to_string( d_0 ) + std::string( "_" ) + std::to_string( d_1 ) + std::string( "_" ) + std::to_string( d_2 )
			//+ std::string( "_M_" ) + std::to_string( m_0 ) + std::string( "_" ) + std::to_string( m_1 ) + std::string( "_" ) + std::to_string( m_2 )
			//+ std::string( "_P_" ) + std::to_string( p_0 ) + std::string( "_" ) + std::to_string( p_1 )
			//+ std::string( "_W_" ) + std::to_string( w_0 ) + std::string( "_" ) + std::to_string( w_1 ) + std::string( "_" ) + std::to_string( w_2 )
			//					   + std::string( "_" ) + std::to_string( w_3 ) + std::string( "_" ) + std::to_string( w_4 ) + std::string( "_" ) + std::to_string( w_5 )
			//					   + std::string( "_" ) + std::to_string( w_6 )
			//+ std::string( "_F_" ) + std::to_string( f_0 ) + std::string( "_" ) + std::to_string( f_1 ) + std::string( "_" ) + std::to_string( f_2 )
			//					   + std::string( "_" ) + std::to_string( f_3 ) + std::string( "_" ) + std::to_string( f_4 ) + std::string( "_" ) + std::to_string( f_5 )
			//					   + std::string( "_" ) + std::to_string( f_6 ) + std::string( "_" ) + std::to_string( f_7 ) + std::string( "_" ) + std::to_string( f_8 )
			//					   + std::string( "_" ) + std::to_string( f_9 ) + std::string( "_" ) + std::to_string( f_10 ) + std::string( "_" ) + std::to_string( f_11 )
			//					   + std::string( "_" ) + std::to_string( f_12 );
			const std::string pptbfFilename = databasePath + pptbfName + std::string(".png");
			PtImageHelper::saveImage(pptbfFilename.c_str(), width, height, 1/*nb channels*/, u_pptbf.data());

			// Binary version
			{
				// Convert data (float to unsigned char)
				// TODO: maybe use for_each with a lambda doing the "cast" directly instead?
				std::transform(f_pptbf_binary.begin(), f_pptbf_binary.end(), f_pptbf_binary.begin(), std::bind(std::multiplies< float >(), std::placeholders::_1, 255.f));
				std::vector< unsigned char > u_pptbf(f_pptbf_binary.begin(), f_pptbf_binary.end());
				// Save data in image
				//const std::string pptbfName = std::string( "pptbf_" )/*name*/ + std::to_string( i );
				char numbuff[100];

				sprintf(numbuff, "_%d_%06d", descriptor, pptbf_keep_count - 1);

				const std::string pptbfName = std::string("pptbf")/*name*/
					+ std::string(numbuff);
				//+ std::string( "_D_" ) + std::to_string( d_0 ) + std::string( "_" ) + std::to_string( d_1 ) + std::string( "_" ) + std::to_string( d_2 )
				//+ std::string( "_M_" ) + std::to_string( m_0 ) + std::string( "_" ) + std::to_string( m_1 ) + std::string( "_" ) + std::to_string( m_2 )
				//+ std::string( "_P_" ) + std::to_string( p_0 ) + std::string( "_" ) + std::to_string( p_1 )
				//+ std::string( "_W_" ) + std::to_string( w_0 ) + std::string( "_" ) + std::to_string( w_1 ) + std::string( "_" ) + std::to_string( w_2 )
				//					   + std::string( "_" ) + std::to_string( w_3 ) + std::string( "_" ) + std::to_string( w_4 ) + std::string( "_" ) + std::to_string( w_5 )
				//					   + std::string( "_" ) + std::to_string( w_6 )
				//+ std::string( "_F_" ) + std::to_string( f_0 ) + std::string( "_" ) + std::to_string( f_1 ) + std::string( "_" ) + std::to_string( f_2 )
				//					   + std::string( "_" ) + std::to_string( f_3 ) + std::string( "_" ) + std::to_string( f_4 ) + std::string( "_" ) + std::to_string( f_5 )
				//					   + std::string( "_" ) + std::to_string( f_6 ) + std::string( "_" ) + std::to_string( f_7 ) + std::string( "_" ) + std::to_string( f_8 )
				//					   + std::string( "_" ) + std::to_string( f_9 ) + std::string( "_" ) + std::to_string( f_10 ) + std::string( "_" ) + std::to_string( f_11 )
				//					   + std::string( "_" ) + std::to_string( f_12 );
				const std::string pptbfFilename = databasePath + pptbfName + std::string("_binary.png");
				PtImageHelper::saveImage(pptbfFilename.c_str(), width, height, 1/*nb channels*/, u_pptbf.data());
			}

		}
		
		// Update counter
		//i++;
	}

	fclose(fdn);

	// Reset GL state(s)
	// - Unset shader program
	PtShaderProgram::unuse();

	// LOG info
	std::cout << "Finished..." << pptbf_count << std::endl;
}

/******************************************************************************
 * Generate a PPTBF given a parameter file
 ******************************************************************************/
void PtGraphicsPPTBF::generatePPTBF( const unsigned int pWidth, const unsigned int pHeight, const char* pParameterFilename )
{
	assert( pWidth != 0 && pHeight != 0 );

	//--------------------------------------------------------------------------

	/**
	 * PPTBF data type enumeration
	 */
	enum class PPTBFDataType
	{
		// Point process
		eTilingType = 0,
		eJittering,
		// Transformation
		eResolution, eRotation, eAspectRatio,
		// Turbulence
		eDistorsionBaseAmplitude, eDistorsionAmplitudeGain, eDistorsionFrequency,
		// Window function
		eWindowShape, eWindowArity, eWindowLarp, eWindowNorm, eWindowSmoothness, eWindowBlend, eWindowSigwcell,
		// Feature function
		eFeatureBombing,
		eFeatureNorm,
		eFeatureWinfeatcorrel,
		eFeatureAniso,
		eFeatureMinNbKernels, eFeatureMaxNbKernels,
		eFeatureSigcos, eFeatureSigcosvar,
		eFeatureFrequency,
		eFeaturePhaseShift,
		eFeatureThickness, eFeatureCurvature, eFeatureOrientation,
		// counter
		ePPTBFDataTypes
	};

	// LOG info
	std::cout << "\nPPTBF generation..." << std::endl;
	std::cout << "START..." << std::endl;

	const int width = pWidth;
	const int height = pHeight;

	// Resize graphics resources
	onSizeModified( width, height );

	///////////////////////////////////////////
	// Read USER parameters
	///////////////////////////////////////////

	//-----------------------------------------
	//// extract image name
	//const std::string filename = std::string( pParameterFilename );
	//size_t lastindex = filename.find_last_of( "/\\" );
	//std::string textureDirectory = filename.substr( 0, lastindex );
	//std::string textureName = filename.substr( 0, lastindex );
	//-----------------------------------------
	
	//-----------------------------------------
	// Extract image name
	// - filename
	const std::string filename = std::string( pParameterFilename );
	// - image directory
	const size_t directoryIndex = filename.find_last_of( "/\\" );
	const std::string textureDirectory = filename.substr( 0, directoryIndex );
	// - file extension
	const size_t extensionIndex = filename.find_last_of( "." );
	const std::string imageExtension = filename.substr( extensionIndex + 1 );
	// - image name
	std::string imageName = filename.substr( directoryIndex + 1 );
	const size_t nameIndex = imageName.find_last_of( "." );
	const std::string pptbfParameterSuffix = "_pptbf_params";
	imageName = imageName.substr( 0, nameIndex - pptbfParameterSuffix.size() );
	std::string textureName = imageName;
	//-----------------------------------------

	std::ifstream estimatedParameterFile( pParameterFilename );
		
	std::string lineData;
		
	// Get threshold
	float threshold;
	std::getline( estimatedParameterFile, lineData );
	std::stringstream ssthreshold( lineData );
	ssthreshold >> threshold;

	// Get PPTBF parameters
	std::getline( estimatedParameterFile, lineData );
		
	// LOG info
	std::cout << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << "PPTBF" << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	// PPTBF parameters
	std::vector< float > pptbf;
	std::stringstream sspptbf( lineData );
	float value;
	while ( sspptbf >> value )
	{
		// Fill parameters
		pptbf.push_back( value );
	}
		
	// Point Process
	const int pointProcess_tilingType = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eTilingType ) ] );
	const float pointProcess_jittering = pptbf[ static_cast< int >( PPTBFDataType::eJittering ) ];
	std::cout << "[POINT PROCESS]" << std::endl;
	std::cout << "tiling type: " << pointProcess_tilingType << std::endl;
	std::cout << "jittering: " << pointProcess_jittering << std::endl;

	// Transformation
	std::cout << std::endl;
	const int transformation_resolution = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eResolution ) ] );
	const float transformation_rotation = pptbf[ static_cast< int >( PPTBFDataType::eRotation ) ] * M_PI;
	const float transformation_aspectRatio = pptbf[ static_cast< int >( PPTBFDataType::eAspectRatio ) ];
	std::cout << "[TRANSFORMATION]" << std::endl;
	std::cout << "resolution: " << transformation_resolution << std::endl;
	std::cout << "rotation: " << transformation_rotation << std::endl;
	std::cout << "aspectRatio: " << transformation_aspectRatio << std::endl;

	// Turbulence
	std::cout << std::endl;
	const float distorsion_baseAmplitude = pptbf[ static_cast< int >( PPTBFDataType::eDistorsionBaseAmplitude ) ];
	const float distorsion_amplitudeGain = pptbf[ static_cast< int >( PPTBFDataType::eDistorsionAmplitudeGain ) ];
	const float distorsion_frequency = pptbf[ static_cast< int >( PPTBFDataType::eDistorsionFrequency ) ];
	std::cout << "[TURBULENCE]" << std::endl;
	std::cout << "base amplitude: " << distorsion_baseAmplitude << std::endl;
	std::cout << "gain: " << distorsion_amplitudeGain << std::endl;
	std::cout << "frequency: " << distorsion_frequency << std::endl;
	
	// Window Function
	std::cout << std::endl;
	std::cout << "[WINDOW FUNCTION]" << std::endl;
	const int window_shape = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eWindowShape ) ] );
	const float window_arity = pptbf[ static_cast< int >( PPTBFDataType::eWindowArity ) ];
	const float window_larp = pptbf[ static_cast< int >( PPTBFDataType::eWindowLarp ) ];
	const float window_norm = pptbf[ static_cast< int >( PPTBFDataType::eWindowNorm ) ];
	const float window_smoothness = pptbf[ static_cast< int >( PPTBFDataType::eWindowSmoothness ) ];
	const float window_blend = pptbf[ static_cast< int >( PPTBFDataType::eWindowBlend ) ];
	const float window_decay = pptbf[ static_cast< int >( PPTBFDataType::eWindowSigwcell ) ];
	std::cout << "shape: " << window_shape << std::endl;
	std::cout << "arity: " << window_arity << std::endl;
	std::cout << "larp: " << window_larp << std::endl;
	std::cout << "norm: " << window_norm << std::endl;
	std::cout << "smoothness: " << window_smoothness << std::endl;
	std::cout << "blend: " << window_blend << std::endl;
	std::cout << "decay: " << window_decay << std::endl;

	// Feature Function
	std::cout << std::endl;
	const int feature_type = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eFeatureBombing ) ] );
	const float feature_norm = pptbf[ static_cast< int >( PPTBFDataType::eFeatureNorm ) ];
	const float feature_featureCorrelation = pptbf[ static_cast< int >( PPTBFDataType::eFeatureWinfeatcorrel ) ];
	const float feature_anisotropy = pptbf[ static_cast< int >( PPTBFDataType::eFeatureAniso ) ];
	const int feature_nbMinKernels = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eFeatureMinNbKernels ) ] );
	const int feature_nbMaxKernels = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eFeatureMaxNbKernels ) ] );
	const float feature_decay = pptbf[ static_cast< int >( PPTBFDataType::eFeatureSigcos ) ];
	const float feature_decayDelta = pptbf[ static_cast< int >( PPTBFDataType::eFeatureSigcosvar ) ];
	const int feature_frequency = static_cast< int >( pptbf[ static_cast< int >( PPTBFDataType::eFeatureFrequency ) ] );
	const float feature_phaseShift = pptbf[ static_cast< int >( PPTBFDataType::eFeaturePhaseShift ) ] * M_PI * 0.5;
	const float feature_thickness = pptbf[ static_cast< int >( PPTBFDataType::eFeatureThickness ) ];
	const float feature_curvature = pptbf[ static_cast< int >( PPTBFDataType::eFeatureCurvature ) ];
	const float feature_orientation = pptbf[ static_cast< int >( PPTBFDataType::eFeatureOrientation ) ] * M_PI;
	std::cout << "[FEATURE FUNCTION]" << std::endl;
	std::cout << "type: " << feature_type << std::endl;
	std::cout << "norm: " << feature_norm << std::endl;
	std::cout << "window feature correlation: " << feature_featureCorrelation << std::endl;
	std::cout << "anisotropy: " << feature_anisotropy << std::endl;
	std::cout << "nb min kernels: " << feature_nbMinKernels << std::endl;
	std::cout << "nb max kernels: " << feature_nbMaxKernels << std::endl;
	std::cout << "decay: " << feature_decay << std::endl;
	std::cout << "decay delta: " << feature_decayDelta << std::endl;
	std::cout << "frequency: " << feature_frequency << std::endl;
	std::cout << "phase shift: " << feature_phaseShift << std::endl;
	std::cout << "thickness: " << feature_thickness << std::endl;
	std::cout << "curvature: " << feature_curvature << std::endl;
	std::cout << "orientation: " << feature_orientation << std::endl;

	///////////////////////
	// Set common GL states
	///////////////////////

	//setImageWidth( width );
	//setImageHeight( height );
	setWidth( width );
	setHeight( height );

	// Set shader program
	PtShaderProgram* shaderProgram = mMegakernelShaderProgram;
	shaderProgram->use();

	// Set texture(s)
	// - PRNG
	glBindTextureUnit( 0/*unit*/, mTexture_P );
	// - noise
	glBindTextureUnit( 1/*unit*/, mTexture_rnd_tab );

	// Set sampler(s)
	// - PRNG
	glBindSampler( 0/*unit*/, mSampler_PRNG_Noise );
	// - noise
	glBindSampler( 1/*unit*/, mSampler_PRNG_Noise );

	// Set image(s)
	// - PPTBF (output)
	glBindImageTexture( 0/*unit*/, mPPTBFTexture, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );

	// Set uniform(s)
	// - PRNG
	shaderProgram->set( 0, "uPermutationTex" );
	// - noise
	shaderProgram->set( 1, "uNoiseTex" );

	// Kernel configuration
	// - block
	const int blockSizeX = 8; // TODO: benchmark with 8x8, 16x16 and 32x32
	const int blockSizeY = 8;
	// - grid
	const GLuint gridSizeX = glm::max( ( width + blockSizeX - 1 ) / blockSizeX, 1 );
	const GLuint gridSizeY = glm::max( ( height + blockSizeY - 1 ) / blockSizeY, 1 );

	/////////////////////////////////////////
	// Sampling the space of PPTBF structures
	/////////////////////////////////////////
	
	////////////////
	// Point Process
	////////////////
	
	shaderProgram->set( pointProcess_tilingType, "uPointProcessTilingType" );
	shaderProgram->set( pointProcess_jittering, "uPointProcessJitter" );

	//////////////////
	// Model Transform
	//////////////////

	shaderProgram->set( transformation_resolution, "uResolution" );
	shaderProgram->set( transformation_rotation, "uRotation" );
	shaderProgram->set( transformation_aspectRatio, "uRescaleX" );
	
	//////////////
	// Deformation
	//////////////

	shaderProgram->set( distorsion_baseAmplitude, "uTurbulenceAmplitude_0" );
	shaderProgram->set( distorsion_amplitudeGain, "uTurbulenceAmplitude_1" );
	shaderProgram->set( distorsion_frequency, "uTurbulenceAmplitude_2" );
	
	//////////////////
	// Window Function
	//////////////////

	shaderProgram->set( window_shape, "uWindowShape" );
	shaderProgram->set( window_arity, "uWindowArity" );
	shaderProgram->set( window_larp, "uWindowLarp" );
	shaderProgram->set( window_norm, "uWindowNorm" );
	shaderProgram->set( window_smoothness, "uWindowSmooth" );
	shaderProgram->set( window_blend, "uWindowBlend" );
	shaderProgram->set( window_decay, "uWindowSigwcell" );

	///////////////////
	// Feature Function
	///////////////////

	shaderProgram->set( feature_type, "uFeatureBomb" );
	shaderProgram->set( feature_norm, "uFeatureNorm" );
	shaderProgram->set( feature_featureCorrelation, "uFeatureWinfeatcorrel" );
	shaderProgram->set( feature_anisotropy, "uFeatureAniso" );
	shaderProgram->set( feature_nbMinKernels, "uFeatureNpmin" );
	shaderProgram->set( feature_nbMaxKernels, "uFeatureNpmax" );
	shaderProgram->set( feature_decay, "uFeatureSigcos" );
	shaderProgram->set( feature_decayDelta, "uFeatureSigcosvar" );
	shaderProgram->set( feature_frequency, "uFeatureFreq" );
	shaderProgram->set( feature_phaseShift, "uFeaturePhase" );
	shaderProgram->set( feature_thickness, "uFeatureThickness" );
	shaderProgram->set( feature_curvature, "uFeatureCourbure" );
	shaderProgram->set( feature_orientation, "uFeatureDeltaorient" );
	
	//----------------------------------------------------------------------------------------------
	// - labeling
	shaderProgram->set( /*getNbLabels()*/0, "uNbLabels" ); // TODO: check this !!!! 										   
														   
	//// - default translation
	//shaderProgram->set( /*getShiftX()*/0.f, "uShiftX" ); // TODO: check this !!!!
	//shaderProgram->set( /*getShiftY()*/0.f, "uShiftY" ); // TODO: check this !!!!
	// - "parameter estimation" tool
	shaderProgram->set( /*getShiftX()*/10.f, "uShiftX" ); // TODO: check this !!!!
	shaderProgram->set( /*getShiftY()*/10.f, "uShiftY" ); // TODO: check this !!!!

	// - labeling
	glBindImageTexture( 1/*unit*/, mPPTBFLabelMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R8UI );
	// - labeling
	glBindImageTexture( 2/*unit*/, mPPTBFRandomValueMap, 0/*level*/, GL_FALSE/*layered*/, 0/*layer*/, GL_WRITE_ONLY, GL_R32F );
	//----------------------------------------------------------------------------------------------

	//---------------------
	// Generate PPTBF image
	//---------------------

	std::cout << "generating image..." << std::endl;
		
	// Launch mega-kernel
	glDispatchCompute( gridSizeX, gridSizeY, 1 );
		
	// Synchronization
	// - make sure writing to image has finished before read
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT ); // pb: wait for nothing? use a boolean if someone need to sample texture
	glMemoryBarrier( GL_ALL_BARRIER_BITS );

	//-------------------
	// Export PPTBF image
	//-------------------
								
	// Retrieve data on host
	std::vector< float > f_pptbf( width * height );
#if 1
	// - deal with odd texture dimensions
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
#endif
	glGetTextureImage( mPPTBFTexture, 0/*level*/, GL_RED, GL_FLOAT, sizeof( float ) * width * height, f_pptbf.data() );

	//-----------------------------------------------------------------------------------------------------------------------------------
		   	  
	// TESTing for "nan" and "inf"
	for (int i = 0; i < width * height; i++)
	{
		if (isnan<float>(f_pptbf[i]) || isinf<float>(f_pptbf[i])) // TODO: add isinf also?
		{
			f_pptbf[i] = 0.f;
		}
	}
	 
	//-------------------
	// Export data
	//-------------------
	{
		// Convert data (float to unsigned char)
		// TODO: maybe use for_each with a lambda doing the "cast" directly instead?
		std::transform( f_pptbf.begin(), f_pptbf.end(), f_pptbf.begin(), std::bind( std::multiplies< float >(), std::placeholders::_1, 255.f ) );
		std::vector< unsigned char > u_pptbf(f_pptbf.begin(), f_pptbf.end());
		// Save data in image
		//const std::string pptbfName = std::string( "pptbf_" )/*name*/ + std::to_string( i );
		char numbuff[100]; 
		//const std::string pptbfName = std::string( "pptbf_TEST" )/*name*/;
		const std::string pptbfName = textureName + std::string( "_pptbf" );
		const std::string pptbfFilename = textureDirectory + std::string( "/" ) + pptbfName + std::string( ".png" );
		std::cout << "write PPTBF: " << pptbfFilename << std::endl;
		PtImageHelper::saveImage( pptbfFilename.c_str(), width, height, 1/*nb channels*/, u_pptbf.data() );
	}
	//-----------------------------------------------------------------------------------------------------------------------------------

	//-------------------
	// make approximate histogramm equalization
	//-------------------

	//std::cout << "----------- 0" << std::endl;

	//-------------------
	// TEST
	//const float percent = threshold;
	const float percent = 1.f - threshold;
	//-------------------
		
		float f_avg = 0.0; float f_min = 0.0, f_max = 0.0;
		int count = 0;
		for (int i = 0; i < width * height; i++)
		{
			if ( !isnan<float>(f_pptbf[i]) && !isinf<float>(f_pptbf[i]) ) // TODO: add isinf also?
			{
				f_avg += f_pptbf[i]; count++;

				if (i == 0) { f_min = f_avg; f_max = f_avg; }

				if (f_min > f_pptbf[i]) f_min = f_pptbf[i];
				if (f_max < f_pptbf[i]) f_max = f_pptbf[i];
			}
		}
		if ( count > 0 )
		{
			f_avg /= (float)(count);
		}
		//std::cout << "avg=" << f_avg << ", min=" << f_min << ", max=" << f_max << "\n";
		//int cmin = 0, cmax = 0;
		//float f_avgmin[3] = { 0.0, 0.0, 0.0 }, f_avgmax[3] = { 0.0,0.0,0.0 };
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] < f_avg) { f_avgmin[1] += f_pptbf[i]; cmin++; }
		//		else { f_avgmax[1] += f_pptbf[i]; cmax++; }
		//	}
		//}
		//f_avgmin[1] /= (float)cmin; f_avgmax[1] /= (float)cmax;

		//cmin = 0; cmax = 0;
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] < f_avg)
		//		{
		//			if (f_pptbf[i] < f_avgmin[1]) { f_avgmin[0] += f_pptbf[i]; cmin++; }
		//			else { f_avgmin[2] += f_pptbf[i]; cmax++; }
		//		}
		//	}
		//}
		//f_avgmin[0] /= (float)cmin; f_avgmin[2] /= (float)cmax;
		//cmin = 0; cmax = 0;
		//for (int i = 0; i < width * height; i++)
		//{
		//	if (!isnan<float>(f_pptbf[i]))
		//	{
		//		if (f_pptbf[i] > f_avg)
		//		{
		//			if (f_pptbf[i] < f_avgmax[1]) { f_avgmax[0] += f_pptbf[i]; cmin++; }
		//			else { f_avgmax[2] += f_pptbf[i]; cmax++; }
		//		}
		//	}
		//}
		//f_avgmax[0] /= (float)cmin; f_avgmax[2] /= (float)cmax;

		//std::cout << "----------- 1" << std::endl;

		const int bins = 100;
		float histo[bins];
		for (int i = 0; i < bins; i++) histo[i] = 0.0f;
		for (int i = 0; i < width * height; i++)
		{
			if ( !isnan<float>(f_pptbf[i]) && !isinf<float>(f_pptbf[i]) ) // TODO: add isinf also?
			{
				if (f_pptbf[i] < f_avg)
				{
					f_pptbf[i] = (f_pptbf[i] - f_min) / (f_avg - f_min)*0.5;
				}
				else
				{
					f_pptbf[i] = (f_pptbf[i] - f_avg) / (f_max - f_avg)*0.5 + 0.5;
				}
			
				int iv = (int)((float)(bins)*f_pptbf[i]);
				if (iv >= bins) iv = bins - 1;
				
				histo[iv] += 1.0f / (float)(width * height);
			}

			//if (f_pptbf[i] < f_avgmin[0]) f_pptbf[i] = (f_pptbf[i] - f_min) / (f_avgmin[0] - f_min)*0.125;
			//else if (f_pptbf[i] < f_avgmin[1]) f_pptbf[i] = (f_pptbf[i] - f_avgmin[0]) / (f_avgmin[1] - f_avgmin[0])*0.125+0.125;
			//else if (f_pptbf[i] < f_avgmin[2]) f_pptbf[i] = (f_pptbf[i] - f_avgmin[1]) / (f_avgmin[2] - f_avgmin[1])*0.125 + 0.25;
			//else if (f_pptbf[i] < f_avg) f_pptbf[i] = (f_pptbf[i] - f_avgmin[2]) / (f_avg - f_avgmin[2])*0.125+0.375;
			//else if (f_pptbf[i] < f_avgmax[0]) f_pptbf[i] = (f_pptbf[i] - f_avg) / (f_avgmax[0] - f_avg)*0.125 + 0.5;
			//else if (f_pptbf[i] < f_avgmax[1]) f_pptbf[i] = (f_pptbf[i] - f_avgmax[0]) / (f_avgmax[1] - f_avgmax[0])*0.125 + 0.625;
			//else if (f_pptbf[i] < f_avgmax[2]) f_pptbf[i] = (f_pptbf[i] - f_avgmax[1]) / (f_avgmax[2] - f_avgmax[1])*0.125 + 0.75;
			//else f_pptbf[i] = (f_pptbf[i] - f_avgmax[2]) / (f_max - f_avgmax[2])*0.125 + 0.875;
		}

		//std::cout << "----------- 2" << std::endl;

		int i;
		float sum = 0.0;
		for (i = 0; i < bins && sum < percent; i++) sum += histo[i];
		//if (i == bins) return vmax;
		float ratio = 0.f;
		if ( histo[i] > 0.f )
		{
			ratio = ( percent - (sum - histo[i]) ) / histo[i];
		}
		//float ratio = 0.5f;
		//printf("compute Thresh bin =%d, sum=%g, percent=%g\n", i, sum, percent);
		float rangea =  ((float)(i - 1) / (float)bins + ratio / (float)bins);

		for (int i = 0; i < width * height; i++)
		{
			//if (isnan<float>(f_pptbf[i]) ) // TODO: add isinf also?
			//{
			//	int test = 0;
			//	test++;
			//}

			//if (isinf<float>(f_pptbf[i])) // TODO: add isinf also?
			//{
			//	int test = 0;
			//	test++;
			//}

			if (f_pptbf[i] > rangea) f_pptbf[i] = 1.0;
			else f_pptbf[i] = 0.0;
		}

	//-----------------------------------------------------------------------------------------------------------------------------------
	
	//std::cout << "----------- 3" << std::endl;

	//-------------------
	// Export data
	//-------------------

	// Convert data (float to unsigned char)
	// TODO: maybe use for_each with a lambda doing the "cast" directly instead?
	std::transform( f_pptbf.begin(), f_pptbf.end(), f_pptbf.begin(), std::bind( std::multiplies< float >(), std::placeholders::_1, 255.f ) );
	std::vector< unsigned char > u_pptbf(f_pptbf.begin(), f_pptbf.end());
	// Save data in image
	//const std::string pptbfName = std::string( "pptbf_" )/*name*/ + std::to_string( i );
	char numbuff[100]; 
	//const std::string pptbfName = std::string( "pptbf_binary_TEST" )/*name*/;
	const std::string pptbfName = textureName + std::string( "_pptbf_binary" );
	const std::string pptbfFilename = textureDirectory + std::string( "/" ) + pptbfName + std::string( ".png" );
	std::cout << "write PPTBF binary: " << pptbfFilename << std::endl;
	PtImageHelper::saveImage( pptbfFilename.c_str(), width, height, 1/*nb channels*/, u_pptbf.data() );
	
	//std::cout << "----------- 4" << std::endl;

	// Reset GL state(s)
	// - Unset shader program
	PtShaderProgram::unuse();

	// LOG info
	std::cout << "Finished..." << std::endl;
}
