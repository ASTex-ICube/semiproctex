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

#include "PtViewer.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

#if _WIN32
#define NOMINMAX
#include <windows.h>
#endif

// GL
#include <glad/glad.h>

// STL
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iomanip>
#include <algorithm>

// glm
//#include <glm/gtc/type_ptr.hpp>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS // => Anonymous
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <imgui_internal.h>

// Project
#include "PtApplication.h"
#include "PtShaderProgram.h"
#include "PtPPTBFLoader.h"
#include "PtGraphicsPPTBF.h"
#include "PtGraphicsHistogram.h"
#include "PtGraphicsMeshManager.h" 
#include "PtCamera.h"
#include "PtImageHelper.h"
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
,	mUseGLCoreProfile( false )
,	mGLCoreProfileVAO( 0 )
,	mGraphicsMeshManager( nullptr )
{
	uiModelFilename = std::string();

	uiTextureSynthesis_launch = false;

	// PPTBF
	mGraphicsPPTBF = new PtGraphicsPPTBF();

	mGraphicsMeshManager = new PtGraphicsMeshManager();

	// Initialize camera
	mCamera = new PtCamera();
	mCamera->setCameraType( PtCamera::ePerspective );
	mCamera->setEye( glm::vec3( 0.f, 0.f, 5.f ) );
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtViewer::~PtViewer()
{
	delete mCamera;
	mCamera = nullptr;

	mGraphicsMeshManager->finalize();
	delete mGraphicsMeshManager;
	mGraphicsMeshManager = nullptr;

	mGraphicsPPTBF->finalize();
	delete mGraphicsPPTBF;
	mGraphicsPPTBF = nullptr;
}

/******************************************************************************
 * Initialize Window
 ******************************************************************************/
void PtViewer::initializeWindow()
{
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
#if 0
	mWindow = glfwCreateWindow( 1280, 720, "PPTBF Designer", nullptr, nullptr );
#else
	mWindow = glfwCreateWindow( 512, 512, "PPTBF Designer", nullptr, nullptr );
#endif
	if ( ! mWindow )
	{
		// Window or OpenGL context creation failed
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	// Initialize ImGui
	// Need a GL context?
	glfwMakeContextCurrent( mWindow );
	// - GL binding
	const bool install_callbacks = false; // customize call: call ImGui ones then custom ones
	ImGui_ImplGlfwGL3_Init( mWindow, install_callbacks );
	// - style
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsDark();
	// User interface parameters
	// - Point process parameters
	uiPointProcess_tilingType = static_cast< int >( PtPPTBF::IRREGULAR );
	uiPointProcess_jittering = 0.f;
	// - Window function parameters
	uiWindowFunction_windowShape = 2;
	uiWindowFunction_windowArity = 2.f;
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
	uiFeatureFunction_anisotropy = 0.f;
	uiFeatureFunction_gaborMinNbKernels = 1;
	uiFeatureFunction_gaborMaxNbKernels = 2;
	uiFeatureFunction_gaborDecay = 1.f;
	uiFeatureFunction_gaborDecayVariation = 0.1f;
	uiFeatureFunction_gaborStripesFrequency = 0.f;
	uiFeatureFunction_featurePhaseShift = 0.f;
	uiFeatureFunction_gaborStripesThickness = 0.01f;
	uiFeatureFunction_gaborStripesCurvature = 0.f;
	uiFeatureFunction_gaborStripesOrientation = static_cast< float >( M_PI ) / 2.f;
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
	uiBinaryStructureMap_nbBins = 128;
	uiBinaryStructureMap_threshold = 75;

	// Initialize window event handling
	// - custom user data
	glfwSetWindowUserPointer( mWindow, this );
	// - window
	glfwSetWindowCloseCallback( mWindow, window_close_callback );
	glfwSetWindowSizeCallback( mWindow, window_size_callback );
	glfwSetWindowPosCallback( mWindow, window_pos_callback );
	// - frame buffer
	glfwSetFramebufferSizeCallback( mWindow, framebuffer_size_callback );
	// - mouse
	glfwSetCursorPosCallback( mWindow, cursor_position_callback );
	glfwSetMouseButtonCallback( mWindow, mouse_button_callback );
	glfwSetScrollCallback( mWindow, scroll_callback );
	// - keyboard
	glfwSetKeyCallback( mWindow, key_callback );
	// - text input
	glfwSetCharCallback( mWindow, character_callback );
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
	uiBinaryStructureMap_threshold = 75;
	// - resize containers on Host
	const int nbBins = mGraphicsHistogram->getNbBins();
	histogram.resize( nbBins, 0.f );
	CDF.resize( nbBins, 0.f );

	// Initialize timer(s)
	graphicsInitialization_Timer();

	// Cannot draw with a bounded VAO in GL Core profile...?
	glGenVertexArrays( 1, &mGLCoreProfileVAO );

	// Initialize mesh manager
	mGraphicsMeshManager->initialize();
	mGraphicsMeshManager->setPPTBFTexture( mGraphicsPPTBF->mPPTBFTexture );
	// Generate a grid mesh
	mGraphicsMeshManager->setMeshType( PtGraphicsMeshManager::MeshType::eTorusMesh );
	mGraphicsMeshManager->setUseMaterial( true );

	//----------------------------------------------------
	GLuint& texture = mTransferFunctionTex;
	glCreateTextures( GL_TEXTURE_2D, 1, &texture );
	// - set texture wrapping/filtering options
	// Common GL parameters
	const GLint param_TEXTURE_WRAP_S = GL_CLAMP_TO_EDGE; /*GL_REPEAT*/
	const GLint param_TEXTURE_WRAP_T = GL_CLAMP_TO_EDGE; /*GL_REPEAT*/
	const GLint param_TEXTURE_MIN_FILTER = GL_NEAREST; /*GL_LINEAR*/
	const GLint param_TEXTURE_MAG_FILTER = GL_NEAREST; /*GL_LINEAR*/
	glTextureParameteri( texture, GL_TEXTURE_WRAP_S, param_TEXTURE_WRAP_S );
	glTextureParameteri( texture, GL_TEXTURE_WRAP_T, param_TEXTURE_WRAP_T );
	glTextureParameteri( texture, GL_TEXTURE_MIN_FILTER, param_TEXTURE_MIN_FILTER );
	glTextureParameteri( texture, GL_TEXTURE_MAG_FILTER, param_TEXTURE_MAG_FILTER );
	// - set min/max level for completeness
	glTextureParameteri( texture, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( texture, GL_TEXTURE_MAX_LEVEL, 0 );
	// - allocate memory
	glTextureStorage2D( texture, 1/*levels*/, GL_RGBA32F, 256/*width*/, 1/*height*/ );
	// - clear texture
	glm::vec4 nullData = glm::vec4( 0.f );
	glClearTexImage( texture, 0/*level*/, GL_RGBA, GL_FLOAT, glm::value_ptr( nullData ) );
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

	// Finalize ImGui
	ImGui_ImplGlfwGL3_Shutdown();

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

	bool statusOK = false;

	// Global variables
	const std::string shaderPath = PtEnvironment::mShaderPath + std::string( "/" );
	PtShaderProgram* shaderProgram = nullptr;
	PtShaderProgram::TShaderList shaders;
	std::string shaderFilename;
	std::vector< std::string > uniforms;

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
void PtViewer::key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	// Call ImGui callback
	ImGui_ImplGlfwGL3_KeyCallback( window, key, scancode, action, mods );

	//printf( "\nPtTextureSynthesizerViewer::key_callback" );

	/*if ( static_cast< PtViewer* >(glfwGetWindowUserPointer(window)) != this )
	{
		return;
	}*/

    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{
        glfwSetWindowShouldClose( window, GLFW_TRUE );
	}

	if ( key == GLFW_KEY_SPACE && action == GLFW_RELEASE )
	{
		const bool currentFlag = static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->mShowUI;
		static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->mShowUI = ( ! currentFlag );
	}

//	printf("key_callback = (%d,%d,%d,%d)", key, scancode, action, mods);
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::character_callback( GLFWwindow* window, unsigned int codepoint )
{
	// Call ImGui callback
	ImGui_ImplGlfwGL3_CharCallback( window, codepoint );

	// Call Pt callback
	static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->processCharacterEvent( codepoint );
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::processCharacterEvent( unsigned int codepoint )
{
	const int keyStatus = glfwGetKey( mWindow, GLFW_KEY_R );
	if ( keyStatus == GLFW_PRESS )
	{
		mGraphicsMeshManager->mModelMatrix = glm::mat4( 1.f );
		mCamera->setEye( glm::vec3( 0.f, 0.f, 5.f ) );
	}
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

	static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->onSizeModified( width, height );
}

/******************************************************************************
 * Callback called when PPTBF size has been modified
 ******************************************************************************/
void PtViewer::onSizeModified( const int pWidth, const int pHeight )
{
	const float aspect = static_cast< float >( pWidth ) / static_cast< float >( pHeight );
	mCamera->setAspect( aspect );
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::window_pos_callback( GLFWwindow* window, int xpos, int ypos )
{
//	printf( "window_pos_callback = (%d,%d)", xpos, ypos );
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtViewer::cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{
	//printf( "\ncursor_position_callback = (%f,%f)", xpos, ypos );

	static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->processMouseMoveEvent( xpos, ypos );
}

/******************************************************************************
* ...
******************************************************************************/
void PtViewer::processMouseMoveEvent( double xpos, double ypos )
{
	static double lastPosX = xpos;
	static double lastPosY = ypos;

	const int mouseButtonLeftStatus = glfwGetMouseButton( mWindow, GLFW_MOUSE_BUTTON_LEFT );
	const int mouseButtonRightStatus = glfwGetMouseButton( mWindow, GLFW_MOUSE_BUTTON_RIGHT );
	const int keyStatus = glfwGetKey( mWindow, GLFW_KEY_LEFT_CONTROL );
	const int keyAltStatus = glfwGetKey( mWindow, GLFW_KEY_LEFT_ALT );
	const int keyShiftStatus = glfwGetKey( mWindow, GLFW_KEY_LEFT_SHIFT );
	
	// Handle rotation movements
	if ( mouseButtonLeftStatus == GLFW_PRESS && keyStatus == GLFW_PRESS )
	{
		//mCamera->truck( 0.01f * ( xpos - lastPosX ) );
		
		const float xoffset = static_cast< float >( xpos - lastPosX );
		const float yoffset = static_cast< float >( ypos - lastPosY );
		glm::vec3 axis = glm::vec3( yoffset, xoffset, 0.f );
		axis = glm::normalize( axis );
		const float angle = glm::length( glm::vec2( xoffset, yoffset ) ) * 0.005f;
		glm::quat quaternion = glm::angleAxis( angle, axis );
		glm::mat4 transform = glm::mat4_cast( quaternion );
		mGraphicsMeshManager->mModelMatrix = transform * mGraphicsMeshManager->mModelMatrix;
	}

	// Handle translation movements
	if ( mouseButtonRightStatus == GLFW_PRESS && keyStatus == GLFW_PRESS )
	{
		mCamera->truck( -0.05f * ( xpos - lastPosX ) );
		mCamera->pedestal( 0.05f * ( ypos - lastPosY ) );
	}

	// Handle rotation movements
	if ( mouseButtonRightStatus == GLFW_PRESS && keyShiftStatus == GLFW_PRESS )
	{
		const float xoffset = static_cast< float >( xpos - lastPosX );
		const float yoffset = static_cast< float >( ypos - lastPosY );
		mGraphicsPPTBF->setShiftX( mGraphicsPPTBF->getShiftX() + xoffset * 0.025f );
		mGraphicsPPTBF->setShiftY( mGraphicsPPTBF->getShiftY() + yoffset * 0.025f );
		mPPTBFUpdateRequested = true;
	}
	if ( mouseButtonLeftStatus == GLFW_PRESS && keyShiftStatus == GLFW_PRESS )
	{
		const float angleOffset = static_cast< float >( xpos - lastPosX );
		float alpha = mGraphicsPPTBF->getAlpha();
		alpha /= static_cast< float >( M_PI );
		alpha += angleOffset * 0.025f;
		alpha = glm::mod( alpha, 2.f );
		mGraphicsPPTBF->setAlpha( alpha * static_cast< float >( M_PI ) );
		mPPTBFUpdateRequested = true;
	}

	lastPosX = xpos;
	lastPosY = ypos;

	if ( mPPTBFUpdateRequested )
	{
		handleRequests();
		refresh();
	}
}

/******************************************************************************
 * If you wish to be notified when a mouse button is pressed or released, set a mouse button callback.
 * The callback function receives the mouse button, button action and modifier bits.
 * The action is one of GLFW_PRESS or GLFW_RELEASE.
 * Mouse button states for named buttons are also saved in per-window state arrays that can be polled with glfwGetMouseButton.
 * The returned state is one of GLFW_PRESS or GLFW_RELEASE.
 ******************************************************************************/
void PtViewer::mouse_button_callback( GLFWwindow* window, int button, int action, int mods )
{
	//printf( "\nmouse_button_callback = (%d,%d;%d)", button, action, mods );

	// Call ImGui callback
	ImGui_ImplGlfwGL3_MouseButtonCallback( window, button, action, mods );

	// Call Pt callback
	static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->processMouseButtonEvent( button, action, mods );
}

/******************************************************************************
 * If you wish to be notified when a mouse button is pressed or released, set a mouse button callback.
 * The callback function receives the mouse button, button action and modifier bits.
 * The action is one of GLFW_PRESS or GLFW_RELEASE.
 * Mouse button states for named buttons are also saved in per-window state arrays that can be polled with glfwGetMouseButton.
 * The returned state is one of GLFW_PRESS or GLFW_RELEASE.
 ******************************************************************************/
void PtViewer::processMouseButtonEvent( int button, int action, int mods )
{
}

/******************************************************************************
 * If you wish to be notified when the user scrolls, whether with a mouse wheel or touchpad gesture, set a scroll callback.
 * The callback function receives two-dimensional scroll offsets.
 * A simple mouse wheel, being vertical, provides offsets along the Y-axis.
 ******************************************************************************/
void PtViewer::scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
	//printf( "\nscroll_callback = (%f,%f)", xoffset, yoffset );

	// Call ImGui callback
	ImGui_ImplGlfwGL3_ScrollCallback( window, xoffset, yoffset );

	// Call Pt callback
	static_cast< PtViewer* >( glfwGetWindowUserPointer( window ) )->processScrollEvent( xoffset, yoffset );
}

/******************************************************************************
 * If you wish to be notified when the user scrolls, whether with a mouse wheel or touchpad gesture, set a scroll callback.
 * The callback function receives two-dimensional scroll offsets.
 * A simple mouse wheel, being vertical, provides offsets along the Y-axis.
 ******************************************************************************/
void PtViewer::processScrollEvent( double xoffset, double yoffset )
{
	const int keyStatus = glfwGetKey( mWindow, GLFW_KEY_LEFT_CONTROL );
	if ( keyStatus == GLFW_PRESS )
	{
		mCamera->dolly( - static_cast< float >( yoffset ) * 2.5f );
	}

	const int keyShiftStatus = glfwGetKey( mWindow, GLFW_KEY_LEFT_SHIFT );
	if ( keyShiftStatus == GLFW_PRESS )
	{
		int resolution = mGraphicsPPTBF->getResolution();
		resolution += static_cast< int >( - yoffset * 10. );
		resolution = glm::clamp( resolution, 1, 1000 );
		mGraphicsPPTBF->setResolution( resolution );
		mPPTBFUpdateRequested = true;
	}
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
	render();

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
 * Render
 ******************************************************************************/
void PtViewer::render()
{
	// Rendering stage

	// Clear framebuffer
	ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
	glClearColor( clear_color.x, clear_color.y, clear_color.z, clear_color.w );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Render scene

	// Cannot draw with a bounded VAO in GL Core profile...?
	if ( mUseGLCoreProfile )
	{
		glBindVertexArray( mGLCoreProfileVAO );
	}

	glClearColor( clear_color.x, clear_color.y, clear_color.z, clear_color.w );

	// Render PPTBF
	//mGraphicsPPTBF->render();
	
	// Cannot draw with a bounded VAO in GL Core profile...?
	if ( mUseGLCoreProfile )
	{
		glBindVertexArray( mGLCoreProfileVAO );
	}
	
	// Render 3D mesh
	glEnable( GL_DEPTH_TEST );
	//glBindSampler( 0, 0 );
	mGraphicsMeshManager->render( mCamera );
	glDisable( GL_DEPTH_TEST );

	// PPTBF
	{
		GLint last_viewport[ 4 ];
		glGetIntegerv( GL_VIEWPORT, last_viewport );
		int width, height;
		glfwGetWindowSize( mWindow, &width, &height );

		const int mapSize = 300;
		glViewport( width - mapSize, mapSize, mapSize, mapSize );
		glDisable( GL_DEPTH_TEST );

		// Set shader program
		PtShaderProgram* shaderProgram = mGraphicsPPTBF->editPPTBFViewerShaderProgram();
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
			glBindTextureUnit( 0/*unit*/, mGraphicsPPTBF->mPPTBFTexture );

			// Set uniform(s)
			shaderProgram->set( 0, "uTexture" ); // Note: this uniform never changes => can be set after shaderprogram::link()
	
			 // Draw command(s)
			glDrawArrays( GL_TRIANGLES, 0, 3 );
		}
		
		// Reset GL state(s)
		//glBindTexture( GL_TEXTURE_2D, 0 );
		PtShaderProgram::unuse();
		glViewport( last_viewport[ 0 ], last_viewport[ 1 ], static_cast< GLsizei >( last_viewport[ 2 ] ), static_cast< GLsizei >( last_viewport[ 3 ] ) );
		glEnable( GL_DEPTH_TEST );
	}

	// Binary structure map
	if ( mUIShowHistogram )
	{
		// Binary structure map
		//glBlitFramebuffer(,  );
		//glBlitNamedFramebuffer( mGraphicsPPTBF->mPPTBFFrameBuffer, 0 );
		GLint last_viewport[ 4 ];
		glGetIntegerv( GL_VIEWPORT, last_viewport );
		int width, height;
		glfwGetWindowSize( mWindow, &width, &height );
		//glfwGetFramebufferSize( mWindow, &width, &height );
		const int mapSize = 300;
		//glViewport( 250, 250, static_cast< GLsizei >( static_cast< float >( mapSize ) * /*screen ratio*/( static_cast< float >( width ) / static_cast< float >( height ) ) ), mapSize );
		glViewport( width - mapSize, 0, mapSize, mapSize );
		glDisable( GL_DEPTH_TEST );
		// Render PPTBF
		// - render PPTBF texture
		PtShaderProgram* binaryStructureMapShaderProgram = mGraphicsPPTBF->editBinaryStructureMapShaderProgram();
		binaryStructureMapShaderProgram->use();
		glBindTextureUnit( 0/*unit*/, mGraphicsPPTBF->mPPTBFTexture );
		binaryStructureMapShaderProgram->set( 0, "uTexture" ); // Note: this uniform never changes => can be set after shaderprogram::link()
		binaryStructureMapShaderProgram->set( mBinaryStructureMapThreshold, "uThreshold" );
		// Draw command(s)
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		// Reset GL state(s)
		//glBindTexture( GL_TEXTURE_2D, 0 );
		PtShaderProgram::unuse();
		glViewport( last_viewport[ 0 ], last_viewport[ 1 ], static_cast< GLsizei >( last_viewport[ 2 ] ), static_cast< GLsizei >( last_viewport[ 3 ] ) );
		glEnable( GL_DEPTH_TEST );
	}
		
	// Cannot draw with a bounded VAO in GL Core profile...?
	if ( mUseGLCoreProfile )
	{
		glBindVertexArray( 0 );
	}

	// Render GUI
	renderGUI();
}

/******************************************************************************
 * Render HUD
 ******************************************************************************/
void PtViewer::renderHUD()
{
}

/******************************************************************************
 * Render GUI
 ******************************************************************************/
void PtViewer::renderGUI()
{
	// Check whether or not to display the user interface
	if ( mShowUI )
	{
		bool uiFrame_ModelTransform_show = true;
		bool uiFrame_Deformations_NonStationarity_and_Mixtures_show = true;
		bool uiFrame_PointProcess_show = true;
		bool uiFrame_WindowFunction_show = true;
		bool uiFrame_FeatureFunction_show = true;
		bool uiFrame_TextureSynthesis_show = true;
		bool uiFrame_GlobalSettings_show = true;
		bool uiFrame_FPS_show = true;
		bool uiFrame_Performance_show = true;
		bool uiFrame_BinaryStructureMap_show = true;
		bool uiFrame_Rendering_show = true;
				
		// ImGui new frame
		ImGui_ImplGlfwGL3_NewFrame();

		// for UI enhancing...
		//bool show_demo_window = true;
		//ImGui::SetNextWindowPos( ImVec2( 20, 20 ), ImGuiCond_FirstUseEver ); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		//ImGui::ShowTestWindow( &show_demo_window );

		// Rendering Parameters
		if ( uiFrame_Rendering_show )
		{
			ImGui::Begin( "Rendering Parameters", &uiFrame_Rendering_show );
			{
				// Force the window size to be adjusted to its content
				ImGui::SetWindowSize( ImVec2( 0.f,0.f ) );
				
				// Start widget
				//ImGui::Begin( "Global Settings", &uiFrame_GlobalSettings_show );
				if ( ImGui::CollapsingHeader( "Mesh Settings" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

					// Output size
					static int meshType = 0;
					bool meshWidgetPressed = false;
					if ( ImGui::RadioButton( "grid##Mesh", &meshType, 0 ) )
					{
						meshWidgetPressed = true;
					}
					if ( ImGui::RadioButton( "wave##Mesh", &meshType, 1 ) )
					{
						meshWidgetPressed = true;
					}
					if ( ImGui::RadioButton( "cylinder##Mesh", &meshType, 2 ) )
					{
						meshWidgetPressed = true;
					}
					if ( ImGui::RadioButton( "torus##Mesh", &meshType, 3 ) )
					{
						meshWidgetPressed = true;
					}
					if ( ImGui::RadioButton( "sphere##Mesh", &meshType, 4 ) )
					{
						meshWidgetPressed = true;
					}
					// Apply modification if any
					if ( meshWidgetPressed )
					{
						mGraphicsMeshManager->setMeshType( static_cast< PtGraphicsMeshManager::MeshType >( meshType ) );
					}
				}

				if ( ImGui::CollapsingHeader( "Rendering Mode" ) )
				{
					// Output size
					static int renderingMode = 0;
					bool renderingModeWidgetPressed = false;
					if ( ImGui::RadioButton( "pptfb##RenderingMode", &renderingMode, 0 ) )
					{
						renderingModeWidgetPressed = true;
					}
					if ( ImGui::RadioButton( "binaryMap##RenderingMode", &renderingMode, 1 ) )
					{
						renderingModeWidgetPressed = true;
					}
					// Apply modification if any
					if ( renderingModeWidgetPressed )
					{
						mGraphicsPPTBF->setRenderingMode( static_cast< PtGraphicsPPTBF::ERenderingMode >( renderingMode ) );
						switch ( static_cast< PtGraphicsPPTBF::ERenderingMode >( renderingMode ) )
						{
							case PtGraphicsPPTBF::ERenderingMode::ePPTBF:
								mGraphicsMeshManager->setPPTBFTexture( mGraphicsPPTBF->mPPTBFTexture );
								break;

							case PtGraphicsPPTBF::ERenderingMode::eBinaryMap:
								mGraphicsMeshManager->setPPTBFTexture( mGraphicsPPTBF->mThresholdTexture);
								break;

							default:
								mGraphicsMeshManager->setPPTBFTexture( mGraphicsPPTBF->mPPTBFTexture );
								break;
						}
					}
				}
			}
			ImGui::End();
		}

		// Binary structure map
			if ( uiFrame_BinaryStructureMap_show )
			{
				ImGui::Begin( "Binary Structure Map", &uiFrame_BinaryStructureMap_show );
				{
					// Force the window size to be adjusted to its content
					ImGui::SetWindowSize( ImVec2( 0.f,0.f ) );

					ImGui::Checkbox( "BinaryHistograms", &mUIShowHistogram );
					if ( mUIShowHistogram )
					{
						// Nb bins
						if ( ImGui::SliderInt( "Nb bins", &uiBinaryStructureMap_nbBins, 1, 1000 ) )
						{
							mHistogramUpdateRequested = true;
						}

						// Histograms
						const int width = mGraphicsPPTBF->getWidth();
						const int height = mGraphicsPPTBF->getHeight();
#if 0
						/*std::vector< float > histogram;
						std::vector< float > CDF;*/
						mGraphicsHistogram->getHistogram( histogram, CDF, width, height ); //, todo: move that in hendleRequest() [outside rendering stage]
#endif

						ImGui::PlotHistogram( "Histogram", histogram.data(), static_cast< int >( histogram.size() ), 0, NULL, 0, FLT_MAX, ImVec2( 100, 100 ) );
						const int nbPixels = width * height;
						/*for ( auto& v : CDF )
						{
							v /= static_cast< float >( nbPixels );
						}*/
						ImGui::PlotHistogram( "CDF", CDF.data(), static_cast< int >( CDF.size() ), 0, NULL, 0.0, /*1.0*/FLT_MAX, ImVec2( 100, 100 ) );

						// Threshold
						if ( ImGui::SliderInt( "Threshold", &uiBinaryStructureMap_threshold, 0, 100 ) )
						{
							mGraphicsPPTBF->setThreshold( uiBinaryStructureMap_threshold * 0.01f );

							mHistogramUpdateRequested = true;
						}
						ImGui::Text( "Binary structure map: %.6f", mBinaryStructureMapThreshold );
					}
				}
				ImGui::End();
			}

		bool uiFrame_PPTBFDesigner_show = true;
		ImGui::Begin( "PPTBF Designer", &uiFrame_PPTBFDesigner_show );
		{
			// Force the window size to be adjusted to its content
			//ImGui::SetWindowSize( ImVec2( 0.f,0.f ) );

			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			if ( uiFrame_GlobalSettings_show )
			{
				// Start widget
				//ImGui::Begin( "Global Settings", &uiFrame_GlobalSettings_show );
				if ( ImGui::CollapsingHeader( "Global Settings" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

					// Output size
					int synthesisSize[ 2 ] = { mGraphicsPPTBF->getWidth(), mGraphicsPPTBF->getHeight() };
					if ( ImGui::InputInt2( "size", synthesisSize ) )
					{
						mGraphicsPPTBF->setWidth( synthesisSize[ 0 ] );
						mGraphicsPPTBF->setHeight( synthesisSize[ 1 ] );
						mGraphicsPPTBF->setImageWidth( synthesisSize[ 0 ] );
						mGraphicsPPTBF->setImageHeight( synthesisSize[ 1 ] );
						mGraphicsPPTBF->onSizeModified( synthesisSize[ 0 ], synthesisSize[ 1 ] );

						mPPTBFUpdateRequested = true;
					}

					//// Debug
					//if ( ImGui::TreeNode( "Debug" ) )
					//{
					//	ImGui::Checkbox( "Use Window Function", &uiGlobalSettings_useWindowFunction );
					//	ImGui::Checkbox( "Use Feature Function", &uiGlobalSettings_useFeatureFunction );
					//
					//	ImGui::TreePop();
					//}

					// End widget
					//ImGui::End();
				}
			}

		if ( ImGui::CollapsingHeader( "PPTBF Parameters" ) )
		{
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			if ( uiFrame_ModelTransform_show )
			{
				// Start widget
				//ImGui::Begin( "Deformations, Non-Stationarity and Mixtures Parameters", &uiFrame_Deformations_NonStationarity_and_Mixtures_show );
				//if ( ImGui::CollapsingHeader( "\tDeformations, Non-Stationarity and Mixtures" ) )
				if ( ImGui::CollapsingHeader( "\tModel Transform" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

					//if ( ImGui::TreeNode( "Spatial Deformation" ) )
					//{
						// RESOL
						int modelTransformResolution = mGraphicsPPTBF->getResolution();
						if ( ImGui::SliderInt( "resolution", &modelTransformResolution, 1, 1000 ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setResolution( modelTransformResolution );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
			
						// alpha
						float modelTransformRotation = mGraphicsPPTBF->getAlpha();
						modelTransformRotation /= static_cast< float >( M_PI );
						if ( ImGui::SliderFloat( "rotation", &modelTransformRotation, 0.f, 2.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setAlpha( modelTransformRotation * static_cast< float >( M_PI ) );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// rescalex
						float modelTransformAspectRatio = mGraphicsPPTBF->getRescalex();
						if ( ImGui::SliderFloat( "aspect ratio", &modelTransformAspectRatio, 0.01f, 10.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setRescalex( modelTransformAspectRatio );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
				
						//ImGui::TreePop();
					//}
				}
			}

			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			if ( uiFrame_Deformations_NonStationarity_and_Mixtures_show )
			{
				// Start widget
				//ImGui::Begin( "Deformations, Non-Stationarity and Mixtures Parameters", &uiFrame_Deformations_NonStationarity_and_Mixtures_show );
				//if ( ImGui::CollapsingHeader( "\tDeformations, Non-Stationarity and Mixtures" ) )
				if ( ImGui::CollapsingHeader( "\tDeformations" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

					//if ( ImGui::TreeNode( "Spatial Deformation" ) )
					//{
						// Spatial deformation
						//float spatialDeformation[ 3 ] = { uiDefNonStaAndMix_spatialDeformation.x, uiDefNonStaAndMix_spatialDeformation.y, uiDefNonStaAndMix_spatialDeformation.z };
						//ImGui::SliderFloat3( "Multi-band amplitudes", spatialDeformation, 0.f, 1.f );
						float deformationTurbulenceAmplitude0 = mGraphicsPPTBF->getTurbulenceAmplitude0();
						if ( ImGui::SliderFloat( "base amplitude##deformation", &deformationTurbulenceAmplitude0, 0.f, 0.25f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setTurbulenceAmplitude0( deformationTurbulenceAmplitude0 );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
						float deformationTurbulenceAmplitude1 = mGraphicsPPTBF->getTurbulenceAmplitude1();
						if ( ImGui::SliderFloat( "gain##deformation", &deformationTurbulenceAmplitude1, 0.f, 4.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setTurbulenceAmplitude1( deformationTurbulenceAmplitude1 );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
						float deformationTurbulenceAmplitude2 = mGraphicsPPTBF->getTurbulenceAmplitude2();
						if ( ImGui::SliderFloat( "A2##deformation", &deformationTurbulenceAmplitude2, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setTurbulenceAmplitude2( deformationTurbulenceAmplitude2 );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
						/*uiDefNonStaAndMix_spatialDeformation.x = spatialDeformation[ 0 ];
						uiDefNonStaAndMix_spatialDeformation.y = spatialDeformation[ 1 ];
						uiDefNonStaAndMix_spatialDeformation.z = spatialDeformation[ 2 ];*/
				
					//	ImGui::TreePop();
					//}
				}
			}

			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			if ( uiFrame_PointProcess_show )
			{
				// Start widget
				//ImGui::Begin( "Point Process Parameters", &uiFrame_PointProcess_show );
				if ( ImGui::CollapsingHeader( "\tPoint Process" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

				//	if ( ImGui::TreeNode( "Pavement" ) )
				//	{
						// Tiling type
						//if ( ImGui::Combo( "tiling type", &uiPointProcess_tilingType, "regular\0irregular\0cross\0bisquare\0irregular_x\0irregular_y\0\0" ) )   // Combo using values packed in a single constant string (for really quick combo)
						int pointProcessTilingType = mGraphicsPPTBF->getTilingType();
						if ( ImGui::SliderInt( "tiling type", &pointProcessTilingType, 0, 17 ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setTilingType( pointProcessTilingType );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Jittering
						float pointProcessJittering = mGraphicsPPTBF->getJittering();
						if ( ImGui::SliderFloat( "jittering", &pointProcessJittering, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setJittering( pointProcessJittering );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

					//	ImGui::TreePop();
					//}

					//if ( ImGui::TreeNode( "Repulsive Forces" ) )
					//{
					//	// Number of relaxation iterations (repulsive force)
					//	if ( ImGui::SliderInt( "nb relaxation iterations", &uiPointProcess_nbRelaxationIterations, 0, 5 ) )
					//	{
					//		if ( uiPointProcess_nbRelaxationIterations != mGraphicsPPTBF->getNbRelaxationIterations() )
					//		{
					//			mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
					//			mGraphicsPPTBF->setNbRelaxationIterations( uiPointProcess_nbRelaxationIterations );
					//			PtShaderProgram::unuse();
					//			mPPTBFUpdateRequested = true;
					//		}
					//	}
			
					//	ImGui::TreePop();
					//}
			
					// End widget
					//ImGui::End();
				}
			}

			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			if ( uiFrame_WindowFunction_show )
			{
				// Start widget
				//ImGui::Begin( "Window Function Parameters", &uiFrame_WindowFunction_show );
				if ( ImGui::CollapsingHeader( "\tWindow Function" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

					// Window blend
					float windowBlend = mGraphicsPPTBF->getWindowBlend();
					if ( ImGui::SliderFloat( "blend##Window", &windowBlend, 0.f, 1.f ) )
					{
						mGraphicsPPTBF->setWindowBlend( windowBlend );
						mPPTBFUpdateRequested = true;
					}
					// Display type
					{
						ImGui::SameLine();
						std::string windowTypename = "";
						if ( windowBlend == 0.f ) windowTypename = "[regular]";
						else if ( windowBlend == 1.f ) windowTypename = "[cellular]";
						else windowTypename = "[mix regular/cellular]";
						ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), windowTypename.c_str() );
					}

					// Window norm
					float windowNorm = mGraphicsPPTBF->getWindowNorm();
					if ( ImGui::SliderFloat( "norm##Window", &windowNorm, 1.f, 3.f ) )
					{
						mGraphicsPPTBF->setWindowNorm( windowNorm );
						mPPTBFUpdateRequested = true;
					}

					// Gaussian Window
					if ( ImGui::TreeNode( "Cellular" ) )
					{
						// Window larp
						float windowLarp = mGraphicsPPTBF->getWindowLarp();
						if ( ImGui::SliderFloat( "larp##Window", &windowLarp, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->setWindowLarp( windowLarp );
							mPPTBFUpdateRequested = true;
						}

						// Window arity
						float windowArity = mGraphicsPPTBF->getWindowArity();
						if ( ImGui::SliderFloat( "arity##Window", &windowArity, 2.f, 10.f ) )
						{
							mGraphicsPPTBF->setWindowArity( windowArity );
							mPPTBFUpdateRequested = true;
						}

						// Window smooth
						float windowSmooth = mGraphicsPPTBF->getWindowSmooth();
						if ( ImGui::SliderFloat( "smooth##Window", &windowSmooth, 0.f, 2.f ) )
						{
							mGraphicsPPTBF->setWindowSmooth( windowSmooth );
							mPPTBFUpdateRequested = true;
						}

						// Window sigwcell
						float windowSigwcell = mGraphicsPPTBF->getWindowSigwcell();
						if ( ImGui::SliderFloat( "sigwcell##Window", &windowSigwcell, 0.01f, 4.f ) )
						{
							mGraphicsPPTBF->setWindowSigwcell( windowSigwcell );
							mPPTBFUpdateRequested = true;
						}

						ImGui::TreePop();
					}

					// Gaussian Window
					if ( ImGui::TreeNode( "Regular" ) )
					{
						// Window shape
						int windowShape = mGraphicsPPTBF->getWindowShape();
						if ( ImGui::SliderInt( "shape##Window", &windowShape, 0, 3 ) )
						{
							mGraphicsPPTBF->setWindowShape( windowShape );
							mPPTBFUpdateRequested = true;
						}
						// Display type
						{
							ImGui::SameLine();
							std::string windowTypename = "";
							if ( windowShape == 0 ) windowTypename = "[normalized tapered cosine]";
							else if ( windowShape == 1 ) windowTypename = "[clamped gaussian]";
							else if ( windowShape == 2 ) windowTypename = "[triangular]";
							else if ( windowShape == 3 ) windowTypename = "[tapered cosine]";
							else windowTypename = "[triangular]";
							ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), windowTypename.c_str() );
						}

						ImGui::TreePop();
					}
			
					// End widget
					//ImGui::End();
				}
			}

			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			if ( uiFrame_FeatureFunction_show )
			{
				// Start widget
				//ImGui::Begin( "Feature Function Parameters", &uiFrame_FeatureFunction_show );
				if ( ImGui::CollapsingHeader( "\tFeature Function" ) )
				{
					// slider will be 65% of the window width (this is the default)
					ImGui::PushItemWidth( ImGui::GetWindowWidth() * 0.4f );

					// Bombing flag
					int featureType = mGraphicsPPTBF->getBombingFlag();
					if ( ImGui::SliderInt( "type##FeatureFunction", &featureType, 0, 8 ) ) // max ??
					{
						mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
						mGraphicsPPTBF->setBombingFlag( featureType );
						PtShaderProgram::unuse();
						mPPTBFUpdateRequested = true;
					}
					// Display type
					{
						ImGui::SameLine();
						std::string featureTypename = "";
						if ( featureType == 0 ) featureTypename = "[constant]";
						else if ( featureType == 1 ) featureTypename = "[Gabor random]";
						else if ( featureType == 2 ) featureTypename = "[Gabor]";
						else if ( featureType == 3 ) featureTypename = "[bombing]";
						else if ( featureType == 4 ) featureTypename = "[Voronoise]";
						else if ( featureType == 5 ) featureTypename = "[USER Markov Chain network]";
						else featureTypename = "[not yet...]";
						ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), featureTypename.c_str() );
					}

					// Feature stripes
					if ( ImGui::TreeNode( "Appearance" ) )
					{
						// Gaussian window norm
						float featureWinfeatcorrel = mGraphicsPPTBF->getFeatureWinfeatcorrel();
						if ( ImGui::SliderFloat( "Winfeatcorrel##FeatureFunction", &featureWinfeatcorrel, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setFeatureWinfeatcorrel( featureWinfeatcorrel );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Feature phase shift
						float featurePhaseShift = mGraphicsPPTBF->getFeaturePhaseShift();
						featurePhaseShift /= ( static_cast< float >( M_PI ) * 0.5f );
						if ( ImGui::SliderFloat( "phase shift", &featurePhaseShift, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setFeaturePhaseShift( featurePhaseShift * ( static_cast< float >( M_PI ) * 0.5f ) );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Gaussian window norm
						float featureNorm = mGraphicsPPTBF->getFeatureNorm();
						if ( ImGui::SliderFloat( "norm##FeatureFunction", &featureNorm, 1.f, 3.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setFeatureNorm( featureNorm );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Gaussian window norm
						float featureAnisotropy = mGraphicsPPTBF->getFeatureAnisotropy();
						if ( ImGui::SliderFloat( "anisotropy##FeatureFunction", &featureAnisotropy, 0.f, 5.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setFeatureAnisotropy( featureAnisotropy );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
					
						// Gabor decay
						float featureSigcos = mGraphicsPPTBF->getGaborDecay();
						if ( ImGui::SliderFloat( "sigcos##featureGaussian", &featureSigcos, 0.f, 10.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setGaborDecay( featureSigcos );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Gabor decay variation
						float featureSigcosvar = mGraphicsPPTBF->getGaborDecayJittering();
						if ( ImGui::SliderFloat( "sigcosvar##featureGaussian", &featureSigcosvar, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setGaborDecayJittering( featureSigcosvar );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						ImGui::TreePop();
					}

					// Feature stripes
					if ( ImGui::TreeNode( "Kernels" ) )
					{
						// Min number of Gabor kernels
						int featureMinNbKernels = mGraphicsPPTBF->getMinNbGaborKernels();
						int featureMaxNbKernels = mGraphicsPPTBF->getMaxNbGaborKernels();
						if ( ImGui::SliderInt( "nb min", &featureMinNbKernels, 0, 8 ) )
						{
							featureMinNbKernels = glm::clamp( featureMinNbKernels, 0, featureMaxNbKernels );

							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setMinNbGaborKernels( featureMinNbKernels );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}
						// Max number of Gabor kernels
						if ( ImGui::SliderInt( "nb max", &featureMaxNbKernels, 0, 8 ) )
						{
							featureMaxNbKernels = glm::clamp( featureMaxNbKernels, featureMinNbKernels, 8 );
							
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setMaxNbGaborKernels( featureMaxNbKernels );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						ImGui::TreePop();
					}

					// Feature stripes
					if ( ImGui::TreeNode( "Stripes" ) )
					{
						// Gabor stripes frequency
						int featureStripesFrequency = mGraphicsPPTBF->getGaborStripesFrequency();
						if ( ImGui::SliderInt( "frequency##featureStripes", &featureStripesFrequency, 0, 16 ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setGaborStripesFrequency( featureStripesFrequency );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Gabor stripes thickness
						float featureStripesThickness = mGraphicsPPTBF->getGaborStripesThickness();
						if ( ImGui::SliderFloat( "thickness##featureStripes", &featureStripesThickness, 0.001f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setGaborStripesThickness( featureStripesThickness );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Gabor stripes curvature
						float featureStripesCurvature = mGraphicsPPTBF->getGaborStripesCurvature();
						if ( ImGui::SliderFloat( "curvature##featureStripes", &featureStripesCurvature, 0.f, 1.f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setGaborStripesCurvature( featureStripesCurvature );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						// Gabor stripes orientation
						float featureStripesOrientation = mGraphicsPPTBF->getGaborStripesOrientation();
						featureStripesOrientation /= static_cast< float >( M_PI );
						if ( ImGui::SliderFloat( "orientation##featureStripes", &featureStripesOrientation, 0.f, 0.5f ) )
						{
							mGraphicsPPTBF->editPPTBFGeneratorShaderProgram()->use();
							mGraphicsPPTBF->setGaborStripesOrientation( featureStripesOrientation * static_cast< float >( M_PI ) );
							PtShaderProgram::unuse();
							mPPTBFUpdateRequested = true;
						}

						ImGui::TreePop();
					}
				}
			}
		}

			//// Binary structure map
			//if ( uiFrame_BinaryStructureMap_show )
			//{
			//	//ImGui::Begin( "Binary Structure Map", &uiFrame_BinaryStructureMap_show );
			//	if ( ImGui::CollapsingHeader( "Binary Structure Map" ) )
			//	{
			//		ImGui::Checkbox( "BinaryHistograms", &mUIShowHistogram );
			//		if ( mUIShowHistogram )
			//		{
			//			// Nb bins
			//			if ( ImGui::SliderInt( "Nb bins", &uiBinaryStructureMap_nbBins, 1, 1000 ) )
			//			{
			//				mHistogramUpdateRequested = true;
			//			}

			//			// Histograms
			//			int width, height;
			//			glfwGetWindowSize( mWindow, &width, &height );
			//			std::vector< float > histogram;
			//			std::vector< float > CDF;
			//			mGraphicsHistogram->getHistogram( histogram, CDF, width, height );
			//			ImGui::PlotHistogram( "Histogram", histogram.data(), static_cast< int >( histogram.size() ), 0, NULL, 0, FLT_MAX, ImVec2( 100, 100 ) );
			//			const int nbPixels = width * height;
			//			for ( auto& v : CDF )
			//			{
			//				v /= static_cast< float >( nbPixels );
			//			}
			//			ImGui::PlotHistogram( "CDF", CDF.data(), static_cast< int >( CDF.size() ), 0, NULL, 0.0, 1.0, ImVec2( 100, 100 ) );

			//			// Threshold
			//			if ( ImGui::SliderInt( "Threshold", &uiBinaryStructureMap_threshold, 0, 100 ) )
			//			{
			//				float pptbfThreshold = 0.f;
			//				float cdfThreshold = static_cast< float >( uiBinaryStructureMap_threshold ) * 0.01f;
			//				for ( const auto v : CDF )
			//				{
			//					if ( v < cdfThreshold )
			//					{
			//						pptbfThreshold += ( 1.f / static_cast< float >( CDF.size() ) );
			//					}
			//					else
			//					{
			//						break;
			//					}
			//				}
			//				mBinaryStructureMapThreshold = pptbfThreshold;
			//				//std::cout << "pptbfThreshold:" << pptbfThreshold << std::endl;
			//			}
			//			ImGui::Text( "Binary structure map: %.6f", mBinaryStructureMapThreshold );
			//		}
			//			
			//		//ImGui::End();
			//	}
			//}

		}
		ImGui::End();

		bool show_app_fixed_overlay = true;
		if (show_app_fixed_overlay) showExampleAppFixedOverlay( &show_app_fixed_overlay );

		// Show IMGui metrics
		//ImGui::ShowMetricsWindow();

		//ImGui::Image((void*)(mGraphicsPPTBF->mPPTBFTexture), ImVec2(512, 512));

		// Render HUD overlay (user interface)
		ImGui::Render();
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
// Demonstrate creating a simple static window with no decoration + a context-menu to choose which corner of the screen to use.
void PtViewer::showExampleAppFixedOverlay( bool* p_open )
{
	const float DISTANCE = 10.0f;
	
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f)); // Transparent background
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.45f, 0.55f, 0.60f, 0.75f)); // Transparent background

	static int corner = 2;
	ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
	ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	if (ImGui::Begin("Mouse Info", p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
	{
		//ImGui::Text("Simple overlay\nin the corner of the screen.\n(right-click to change position)");
		//ImGui::Separator();
		//ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		ImGui::Text("Mouse Position: (%d,%d)", static_cast< int >( ImGui::GetIO().MousePos.x ), static_cast< int >( ImGui::GetIO().MousePos.y ) );
		/*if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			ImGui::EndPopup();
		}*/
		ImGui::End();
	}

	static int corner2 = 1;
	window_pos = ImVec2((corner2 & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner2 & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
	window_pos_pivot = ImVec2((corner2 & 1) ? 1.0f : 0.0f, (corner2 & 2) ? 1.0f : 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	if (ImGui::Begin("Synthesis Detailed Info", p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Text( "Profiler" );
		ImGui::Separator();
		ImGui::Text(const_cast< char* >( mGraphicsPPTBF->getSynthesisInfo().str().c_str() ), static_cast< int >(ImGui::GetIO().MousePos.x), static_cast< int >(ImGui::GetIO().MousePos.y));
		
		ImGui::End();
	}

	int width, height;
	glfwGetWindowSize( mWindow, &width, &height );
	//glfwGetFramebufferSize( mWindow, &width, &height );
	static int corner3 = 0;
	window_pos = ImVec2((corner3 & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner3 & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
	window_pos_pivot = ImVec2((corner3 & 1) ? 1.0f : 0.0f, (corner3 & 2) ? 1.0f : 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	if (ImGui::Begin("Example: Fixed Overlay", p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Text( "Performance" );
		ImGui::Separator();
		ImGui::Text( "FPS          %.1f", ImGui::GetIO().Framerate );
		ImGui::Text( "Frame        %.3f ms", 1000.0f / ImGui::GetIO().Framerate );
		ImGui::Text( "Window       %dx%d", width, height );
		ImGui::Separator();
		//ImGui::Text( "Exemplar     %dx%d", mGraphicsSynthesizer->getExemplarWidth(), mGraphicsSynthesizer->getExemplarHeight() );
		//ImGui::Text( "Output       %dx%d", mGraphicsSynthesizer->getWidth(), mGraphicsSynthesizer->getHeight() );
		//ImGui::Text( "Synthesis    %5.3f ms", mGraphicsSynthesizer->getSynthesisTime() );
		// Timer
		ImGui::Checkbox( "Timer", &uiPPTBF_timer );
		if ( uiPPTBF_timer )
		{
		//	ImGui::Text( "PPTBF: [%5.3f ms]", mPPTBFGenerationTime * 1.e-6 );
		//	ImGui::Separator();
			ImGui::Text( "Output       %dx%d", mGraphicsPPTBF->getWidth(), mGraphicsPPTBF->getHeight() );
			ImGui::Text( "Synthesis    %5.3f ms", mGraphicsPPTBF->getSynthesisTime() );
		}

		ImGui::End();
	}

	ImGui::PopStyleColor();
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
	// Generate PPTBF
	// - check if PPTBF computation is requested (Update UI edition flag)
	if ( mPPTBFUpdateRequested )
	{
		mPPTBFUpdateRequested = false; // TEST

		// Compute PPTBF
		//mGraphicsPPTBF->setTime( static_cast< float >( glfwGetTime() ) ); // TODO: no active program !!!!!!!!!!
		mGraphicsPPTBF->compute();

		// Update UI edition flag
		//mPPTBFUpdateRequested = false;

		// Update UI edition flag
		mHistogramUpdateRequested = true;
	}

	// Binary structure map
	if ( mHistogramUpdateRequested )
	{
		if ( uiBinaryStructureMap_nbBins != mGraphicsHistogram->getNbBins() )
		{
			mGraphicsHistogram->setNbBins( uiBinaryStructureMap_nbBins );

			// Resize containers
			const int nbBins = uiBinaryStructureMap_nbBins;
			histogram.clear(); // to be able to assign 0 in previous data...
			CDF.clear(); // to be able to assign 0 in previous data...
			histogram.resize( nbBins );
			CDF.resize( nbBins );
		}
#if 0 // commented while debugging GPU bug...
		// GPU-based implementation
		if ( mUseGLCoreProfile )
		{
			glBindVertexArray( mGLCoreProfileVAO );
		}

		// Generate PPTBF histogram
		//int width, height;
		//glfwGetWindowSize( mWindow, &width, &height );
		//glfwGetFramebufferSize( mWindow, &width, &height );
		//mGraphicsHistogram->compute( mGraphicsPPTBF->mPPTBFTexture, width, height );
		mGraphicsHistogram->compute( mGraphicsPPTBF->mPPTBFTexture, mGraphicsPPTBF->getWidth(), mGraphicsPPTBF->getHeight() );

		//// Cannot draw with a bounded VAO in GL Core profile...?
		if ( mUseGLCoreProfile )
		{
			glBindVertexArray( 0 );
		}
#else
		// Software implementation...
		// Retrieve data on host
		const int width = mGraphicsPPTBF->getWidth();
		const int height = mGraphicsPPTBF->getHeight();
		std::vector< float > f_pptbf( width * height );
		glGetTextureImage( mGraphicsPPTBF->mPPTBFTexture, 0/*level*/, GL_RED, GL_FLOAT, sizeof( float ) * width * height, f_pptbf.data() );
		// - histogram
		const int nbBins = mGraphicsHistogram->getNbBins();
		std::fill( histogram.begin(), histogram.end(), 0.f );
		std::fill( CDF.begin(), CDF.end(), 0.f );
		for ( auto v : f_pptbf )
		{
			int binID = static_cast< int >( glm::floor( v * nbBins ) );
			binID = glm::clamp( binID, 0, nbBins - 1 );
			histogram[ binID ]++;
		}
		// - cumulative distribution function
		std::partial_sum( histogram.begin(), histogram.begin() + nbBins, CDF.begin() );
		// - update threshold
		float pptbfThreshold = 0.f;
		float cdfThreshold = static_cast< float >( uiBinaryStructureMap_threshold ) * 0.01f;
		const int nbPixels = width * height;
		const float scaleFactor = static_cast< float >( nbPixels ); // map CDF data to [0;1]
		const float cdfThresholdScaled = cdfThreshold * scaleFactor;
		for ( const auto v : CDF )
		{
			//if ( ( v * scaleFactor ) <= cdfThreshold )
			if ( v <= cdfThresholdScaled )
			{
				pptbfThreshold += ( 1.f / static_cast< float >( CDF.size() ) );
			}
			else
			{
				break;
			}
		}
		mBinaryStructureMapThreshold = pptbfThreshold;

		// Compute binary structure map
		mGraphicsPPTBF->computeBinaryStructureMap();

#endif
		
		// Update histogram edition flag
		mHistogramUpdateRequested = false;
	}
}
