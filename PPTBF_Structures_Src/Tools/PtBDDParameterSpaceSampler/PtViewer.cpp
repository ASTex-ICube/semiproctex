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

#include "PtViewer.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// GL
#include <glad/glad.h>

// STL
#include <string>
#include <iostream>
#include <fstream>

// glm
//#include <glm/gtc/type_ptr.hpp>

// Project
#include "PtApplication.h"
#include "PtShaderProgram.h"
#include "PtPPTBFLoader.h"
#include "PtGraphicsPPTBF.h"
#include "PtGraphicsHistogram.h"
#include <PtEnvironment.h>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

 // Project
using namespace Pt;
using namespace PtGraphics;
using namespace PtGUI;

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
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Default constructor
 ******************************************************************************/
PtViewer::PtViewer()
:	mWindow( nullptr )
,	mCamera( nullptr )
,	mLaunchRequest( false )
,	mShowUI( true )
,	mGraphicsPPTBF( nullptr )
,	mGraphicsHistogram( nullptr )
,	mPPTBFUpdateRequested( true )
,	mHistogramUpdateRequested( false )
,	mUIShowHistogram( false )
,	mBinaryStructureMapThreshold( 0.f )
,	mUseRealTime( false )
,	mGraphicsLBP( nullptr )
,	mUseGLCoreProfile( false )
,	mGLCoreProfileVAO( 0 )
{
	uiModelFilename = std::string();

	uiTextureSynthesis_launch = false;

	// PPTBF
	mGraphicsPPTBF = new PtGraphicsPPTBF();
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtViewer::~PtViewer()
{
	mGraphicsPPTBF->finalize();
	delete mGraphicsPPTBF;
	mGraphicsPPTBF = nullptr;
}

/******************************************************************************
 * Initialize Window
 ******************************************************************************/
void PtViewer::initializeWindow()
{
	// Offscreen contexts
	// GLFW doesn't support creating contexts without an associated window.
	// However, contexts with hidden windows can be created with the GLFW_VISIBLE window hint.
	glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );

	// uncomment these lines if on Apple OS X
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	// Ask for GL profile
#if 1
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	mUseGLCoreProfile = true;
#else
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );
	mUseGLCoreProfile = false;
#endif

	// Create window
	mWindow = glfwCreateWindow( 512, 512, "", nullptr, nullptr );
	if ( ! mWindow )
	{
		// Window or OpenGL context creation failed
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	// Need a GL context?
	glfwMakeContextCurrent( mWindow );
			
	// User interface parameters
	// - Point process parameters
	uiPointProcess_tilingType = static_cast< int >( PtPPTBF::IRREGULAR );
	uiPointProcess_jittering = 0.f;
	// - Window function parameters
	uiWindowFunction_windowShape = 2;
	uiWindowFunction_windowArity = 6.f;
	uiWindowFunction_windowLarp = 0.f;
	uiWindowFunction_windowNorm = 2.f;
	uiWindowFunction_windowSmooth = 0.f;
	uiWindowFunction_windowBlend = 1.0f;
	uiWindowFunction_windowSigwcell = 0.1f;
	uiWindowFunction_gaussianWindowDecay = 1.f;
	uiWindowFunction_gaussianWindowDecayVariation = 0.f;
	uiWindowFunction_gaussianWindowNorm = 2.f;
	// - Feature function parameters
	uiPPTBF_bombingFlag = 0;
	uiFeatureFunction_featureNorm = 2.f;
	uiFeatureFunction_winfeatcorrel = 0.f;
	uiFeatureFunction_anisotropy = 10.f;
	uiFeatureFunction_gaborMinNbKernels = 1;
	uiFeatureFunction_gaborMaxNbKernels = 2;
	uiFeatureFunction_gaborDecay = 1.f;
	uiFeatureFunction_gaborDecayVariation = 0.1f;
	uiFeatureFunction_gaborStripesFrequency = 0.f;
	uiFeatureFunction_featurePhaseShift = 0.f;
	uiFeatureFunction_gaborStripesThickness = 0.01f;
	uiFeatureFunction_gaborStripesCurvature = 0.f;
	uiFeatureFunction_gaborStripesOrientation = M_PI / 2.f;
	// - Deformations, Non-Stationarity and_Mixtures parameters
	uiDefNonStaAndMix_spatialDeformation = glm::vec3( 0.f, 0.f, 0.f );
	uiDefNonStaAndMix_recursiveWindowSplittingProbability = 0.f;
	uiDefNonStaAndMix_recursiveWindowSplittingScaleRatio = 0.5f;
	// Global settings
	uiGlobalSettings_RESOL = 100;
	uiGlobalSettings_alpha = 0.f * M_PI;
	uiGlobalSettings_rescalex = 1.f;
	// PPTBF
	uiPPTBF_animation = false;
	uiPPTBF_megaKernel = true;
	uiPPTBF_timer = true;

	// Display binary visual structure
	mUIShowHistogram = true;
	
	// Debug
	uiGlobalSettings_useWindowFunction = true;
	uiGlobalSettings_useFeatureFunction = true;
	// Binary structure map
	uiBinaryStructureMap_nbBins = 10;
	uiBinaryStructureMap_threshold = 100;

	// Initialize window event handling
	// - custom user data
	glfwSetWindowUserPointer( mWindow, this );
	// - window
	glfwSetWindowCloseCallback( mWindow, window_close_callback );
	glfwSetWindowSizeCallback( mWindow, window_size_callback );
	// - frame buffer
	glfwSetFramebufferSizeCallback( mWindow, framebuffer_size_callback );
}

/******************************************************************************
 * ...
 ******************************************************************************/
static void APIENTRY openglCallbackFunction(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
)
{
  /*(void)source; (void)type; (void)id; 
  (void)severity; (void)length; (void)userParam;
  
  fprintf( stderr, "%s\n", message );
  
  if ( severity == GL_DEBUG_SEVERITY_HIGH )
  {
    fprintf( stderr, "Aborting...\n" );
    abort();
  }*/

	// TEST
	if ( severity != GL_DEBUG_SEVERITY_HIGH )
	//if ( severity != GL_DEBUG_SEVERITY_MEDIUM )
	//if ( severity != GL_DEBUG_SEVERITY_LOW )
	{
		return;
	}

	std::cout << "------------opengl-debug-callback------------" << std::endl;
	std::cout << "message: "<< message << std::endl;
	std::cout << "type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
        break;
    }
	std::cout << std::endl;
 
	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
    switch (severity){
    case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
        break;
    }
	std::cout << std::endl;
}

/******************************************************************************
 * Initialize GL
 ******************************************************************************/
void PtViewer::initializeGL()
{
	// Determine the OpenGL and GLSL versions
	mVendor = glGetString( GL_VENDOR );
	mRenderer = glGetString( GL_RENDERER );
	mVersion = glGetString( GL_VERSION );
	mShadingLanguageVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );
	glGetIntegerv( GL_MAJOR_VERSION, &mMajorVersion );
	glGetIntegerv( GL_MINOR_VERSION, &mMinorVersion );
	
	// Determine the OpenGL and GLSL versions
	printf( "\n[Renderer Info]\n" );
	printf( "\tGL Vendor: %s\n", mVendor );
	printf( "\tGL Renderer: %s\n", mRenderer );
	printf( "\tGL Version (string): %s\n", mVersion );
	printf( "\tGL Version (integer): %d.%d\n", mMajorVersion, mMinorVersion );
	printf( "\tGLSL Version: %s\n", mShadingLanguageVersion );

	// Check for Bindless Texture extension
	if ( glfwExtensionSupported( "GL_ARB_bindless_texture" ) == GLFW_FALSE )
	{
		std::cout << "\nERROR: GL_ARB_bindless_texture extension is not supported by the current context" << std::endl;
		exit( -1 );
	}

	std::cout << "\n[Path configuration]" << std::endl;
	//std::string projectBinPath = "";
	std::string projectPath = "";
	std::string projectDataPath = "";
	std::string projectImagePath = "";
	std::string projectShaderPath = "";
//#ifdef PT_PROJECT_BIN_PATH
//	projectBinPath = "PT_PROJECT_BIN_PATH";
//#endif
#ifdef PT_PROJECT_PATH
	projectPath = PT_PROJECT_PATH;
#endif
#ifdef PT_DATA_PATH
	projectDataPath = PtEnvironment::mDataPath;
#endif
#ifdef PT_IMAGE_PATH
	projectImagePath = PtEnvironment::mImagePath;
#endif
#ifdef PT_SHADER_PATH
	projectShaderPath = PtEnvironment::mShaderPath;
#endif
	//printf( "PT_PROJECT_BIN_PATH: %s\n", projectBinPath.c_str() );
	printf( "\tPROJECT\t%s\n", projectPath.c_str() );
	printf( "\tDATA\t%s\n", projectDataPath.c_str() );
	printf( "\tIMAGE\t%s\n", projectImagePath.c_str() );
	printf( "\tSHADER\t%s\n", projectShaderPath.c_str() );

#if _DEBUG // Use a boolean in User Interface instead !!!!!!!!!!!!!!!!!!!!!
	// Enable graphics callback
	glEnable( GL_DEBUG_OUTPUT );
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
	glDebugMessageCallback( openglCallbackFunction, nullptr );
	glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true );
#endif

	// Initialize Graphics data

	int width, height;
	glfwGetWindowSize( mWindow, &width, &height );
	//glfwGetFramebufferSize( mWindow, &width, &height );
	glViewport( 0, 0, width, height );

	// Initialize graphics PPTBF
	initializePPTBF( width, height );
	
	// Initialize graphics PPTBF
	mGraphicsHistogram = new PtGraphicsHistogram();
	mGraphicsHistogram->initialize( uiBinaryStructureMap_nbBins );

	// Initialize timer(s)
	graphicsInitialization_Timer();

	// Cannot draw with a bounded VAO in GL Core profile...?
	glGenVertexArrays( 1, &mGLCoreProfileVAO );
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::finalize()
{
	// Cannot draw with a bounded VAO in GL Core profile...?
	glDeleteVertexArrays( 1, &mGLCoreProfileVAO );

	// Finalize timer(s)
	graphicsFinalization_Timer();

	glfwDestroyWindow( mWindow );
}

/******************************************************************************
 * Initialize PPTBF
 ******************************************************************************/
void PtViewer::initializePPTBF( const int pWidth, const int pHeight )
{
	// Update PPTBF
	mGraphicsPPTBF->setWidth( pWidth );
	mGraphicsPPTBF->setHeight( pHeight );

	// Initialize graphics PPTBF
	mGraphicsPPTBF->initialize( pWidth, pHeight );

	// Upload PPTBF parameters to device (i.e. GPU)
	mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
	{
		// - general parameters
		mGraphicsPPTBF->setResolution( uiGlobalSettings_RESOL );
		mGraphicsPPTBF->setAlpha( uiGlobalSettings_alpha * static_cast< float >( M_PI ) );
		mGraphicsPPTBF->setRescalex( uiGlobalSettings_rescalex );
		//mGraphicsPPTBF->setImageWidth( pWidth );
		//mGraphicsPPTBF->setImageHeight( pHeight );
		// - PPTBF
		mGraphicsPPTBF->setBombingFlag( uiPPTBF_bombingFlag );
		// - point process
		mGraphicsPPTBF->setTilingType( uiPointProcess_tilingType );
		mGraphicsPPTBF->setJittering( uiPointProcess_jittering );
		mGraphicsPPTBF->setCellSubdivisionProbability( uiPointProcess_cellSubdivisionProbability );
		mGraphicsPPTBF->setNbRelaxationIterations( uiPointProcess_nbRelaxationIterations );
		// - window function
		mGraphicsPPTBF->setCellularToGaussianWindowBlend( uiWindowFunction_cellularVSGaussianWindowBlend );
		mGraphicsPPTBF->setRectangularToVoronoiShapeBlend( uiWindowFunction_rectangularToVoronoiShapeBlend );
		mGraphicsPPTBF->setCellularWindowNorm( uiWindowFunction_cellularWindowNorm );
		mGraphicsPPTBF->setWindowShape( uiWindowFunction_windowShape );
		mGraphicsPPTBF->setWindowArity( uiWindowFunction_windowArity );
		mGraphicsPPTBF->setWindowLarp( uiWindowFunction_windowLarp );
		mGraphicsPPTBF->setWindowNorm( uiWindowFunction_windowNorm );
		mGraphicsPPTBF->setWindowSmooth( uiWindowFunction_windowSmooth );
		mGraphicsPPTBF->setWindowBlend( uiWindowFunction_windowBlend );
		mGraphicsPPTBF->setWindowSigwcell( uiWindowFunction_windowSigwcell );
		mGraphicsPPTBF->setCellularWindowDecay( uiWindowFunction_cellularWindowDecay );
		mGraphicsPPTBF->setGaussianWindowDecay( uiWindowFunction_gaussianWindowDecay );
		mGraphicsPPTBF->setGaussianWindowDecayJittering( uiWindowFunction_gaussianWindowDecayVariation );
		// - feature function
		mGraphicsPPTBF->setMinNbGaborKernels( uiFeatureFunction_gaborMinNbKernels );
		mGraphicsPPTBF->setMaxNbGaborKernels( uiFeatureFunction_gaborMaxNbKernels );
		mGraphicsPPTBF->setFeatureNorm( uiFeatureFunction_featureNorm );
		mGraphicsPPTBF->setFeatureWinfeatcorrel( uiFeatureFunction_winfeatcorrel );
		mGraphicsPPTBF->setFeatureAnisotropy( uiFeatureFunction_anisotropy );
		mGraphicsPPTBF->setGaborStripesFrequency( static_cast< int >( uiFeatureFunction_gaborStripesFrequency ) );
		mGraphicsPPTBF->setGaborStripesCurvature( uiFeatureFunction_gaborStripesCurvature );
		mGraphicsPPTBF->setGaborStripesOrientation( uiFeatureFunction_gaborStripesOrientation );
		mGraphicsPPTBF->setGaborStripesThickness( uiFeatureFunction_gaborStripesThickness );
		mGraphicsPPTBF->setGaborDecay( uiFeatureFunction_gaborDecay );
		mGraphicsPPTBF->setGaborDecayJittering( uiFeatureFunction_gaborDecayVariation );
		mGraphicsPPTBF->setFeaturePhaseShift( uiFeatureFunction_featurePhaseShift * ( static_cast< float >( M_PI ) * 0.5f ) );
		// - deformation
		mGraphicsPPTBF->setTurbulenceAmplitude0( uiDefNonStaAndMix_spatialDeformation.x );
		mGraphicsPPTBF->setTurbulenceAmplitude1( uiDefNonStaAndMix_spatialDeformation.y );
		mGraphicsPPTBF->setTurbulenceAmplitude2( uiDefNonStaAndMix_spatialDeformation.z );
		// - recursivity
		mGraphicsPPTBF->setRecursiveWindowSubdivisionProbability( uiDefNonStaAndMix_recursiveWindowSplittingProbability );
		mGraphicsPPTBF->setRecursiveWindowSubdivisionScale( uiDefNonStaAndMix_recursiveWindowSplittingScaleRatio );
		// - animation
		mGraphicsPPTBF->setUseAnimation( uiPPTBF_animation );
	}
	PtShaderProgram::unuse();
}

/******************************************************************************
 * ...
 ******************************************************************************/
GLFWwindow* PtViewer::getWindow()
{
	return mWindow;
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::window_close_callback( GLFWwindow* window )
{
	//printf("\nPtTextureSynthesizerViewer::window_close_callback");

    //if ( ! time_to_close )
	{
        glfwSetWindowShouldClose( window, GLFW_TRUE );
	}

	//printf("window_close_callback");
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::window_size_callback( GLFWwindow* window, int width, int height )
{
	//printf( "window_size_callback = (%d,%d)", width, height );
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::framebuffer_size_callback( GLFWwindow* window, int width, int height )
{
	glViewport( 0, 0, width, height );
	
	//printf( "framebuffer_size_callback = (%d,%d)", width, height );

	static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->onSizeModified( width, height );
}

/******************************************************************************
 * Callback called when PPTBF size has been modified
 ******************************************************************************/
void PtViewer::onSizeModified( const int pWidth, const int pHeight )
{
	// Update PPTBF
	//mGraphicsPPTBF->setWidth( pWidth );
	//mGraphicsPPTBF->setHeight( pHeight );
	mGraphicsPPTBF->setImageWidth( pWidth );
	mGraphicsPPTBF->setImageHeight( pHeight );

	//mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
	//mGraphicsPPTBF->setImageWidth( pWidth );
	//mGraphicsPPTBF->setImageHeight( pHeight );
	//PtShaderProgram::unuse();
	mPPTBFUpdateRequested = true;

	// Update PPTBF framebuffer
	mGraphicsPPTBF->onSizeModified( pWidth, pHeight );
}

/******************************************************************************
 * Refresh
 ******************************************************************************/
void PtViewer::refresh()
{
	// Set context
	// NOTE:
	// By default, making a context non-current implicitly forces a pipeline flush
	// On machines that support GL_KHR_context_flush_control, you can control whether a context performs this flush by setting the GLFW_CONTEXT_RELEASE_BEHAVIOR window hint.
	glfwMakeContextCurrent( getWindow() );
				
	// Render scene
	//render();

	// Swap buffers
	glfwSwapBuffers( getWindow() );
}

/******************************************************************************
 * Initialize timer(s)
 ******************************************************************************/
void PtViewer::graphicsInitialization_Timer()
{
	// Device timer
	glCreateQueries( GL_TIME_ELAPSED, 1, &mQueryTimeElapsed );
}

/******************************************************************************
 * Finalize timer(s)
 ******************************************************************************/
void PtViewer::graphicsFinalization_Timer()
{
	// Device timer
	glDeleteQueries( 1, &mQueryTimeElapsed );
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::synthesize()
{
}

/******************************************************************************
 * Slot called when the data model has been modified
 ******************************************************************************/
void PtViewer::onDataModelModified()
{
}

/******************************************************************************
 * Handle requests
 ******************************************************************************/
void PtViewer::handleRequests()
{
	// handle Requests stage

	//std::cout << "\nHandle requests..." << std::endl;
	
	// Generate PPTBF
	// - check if PPTBF computation is requested (Update UI edition flag)
	if ( mPPTBFUpdateRequested )
	{
		mPPTBFUpdateRequested = false; // TEST

		// Compute PPTBF
		//mGraphicsPPTBF->setTime( static_cast< float >( glfwGetTime() ) ); // TODO: no active program !!!!!!!!!!
		//mGraphicsPPTBF->compute();

		// Update UI edition flag
		//mPPTBFUpdateRequested = false;
	}
}

/******************************************************************************
 * Get synthesizer
 ******************************************************************************/
PtGraphicsPPTBF* PtViewer::getPPTBF()
{
	return mGraphicsPPTBF;
}
