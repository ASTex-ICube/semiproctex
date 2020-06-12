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

#include "PtApplication.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <iostream>

// Project
#include "PtGraphicsPPTBF.h"
#include "PtViewer.h"
#include <PtModelLibrary.h>
#include <PtGraphicsLibrary.h>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

 // Project
using namespace Pt;
using namespace PtGraphics;
using namespace PtGUI;

// STL
using namespace std;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/**
 * The unique instance of the singleton.
 */
PtApplication* PtApplication::msInstance = nullptr;

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Get the unique instance.
 *
 * @return the unique instance
 ******************************************************************************/
PtApplication& PtApplication::get()
{
    if ( msInstance == nullptr )
    {
        msInstance = new PtApplication();
    }

    return *msInstance;
}

/******************************************************************************
 * Default constructor
 ******************************************************************************/
PtApplication::PtApplication()
:	mMainWindow( nullptr )
,	mPPTBFParameterFilename()
{
	// User interface
	mMainWindow = new PtViewer();
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtApplication::~PtApplication()
{
	// User interface
	delete mMainWindow;
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
bool PtApplication::initialize( const char* const pWorkingDirectory )
{
	bool statusOK = false;

	statusOK = Pt::PtDataModelLibrary::initialize( pWorkingDirectory );
	if ( ! statusOK )
	{
		return false;
	}
	statusOK = PtGraphics::PtGraphicsLibrary::initialize( pWorkingDirectory );
	if ( ! statusOK )
	{
		return false;
	}

	// Initialize the GLFW library
	initializeGLFW();

	// Initialize windows
	// - at least, create one graphics context
	initializeWindows();

	// Initialize GL library
	initializeGL();
	
	// Initialize user interface
	//initializeImGuiUserInterface(); // already done in MainWindow...

	return true;
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void PtApplication::finalize()
{
	// Finalize the GLFW library
	finalizeGLFW();
}

/******************************************************************************
 * Initialize GLFW
 ******************************************************************************/
void PtApplication::initializeGLFW()
{
	// Initialize GLFW library
	if ( ! glfwInit() )
	{
		// Initialization failed
		exit( EXIT_FAILURE );
	}

	// Set error callback
	glfwSetErrorCallback( error_callback );
}

/******************************************************************************
 * Finalize GLFW
 ******************************************************************************/
void PtApplication::finalizeGLFW()
{
	glfwTerminate();
}

/******************************************************************************
 * GLFW error callback
 ******************************************************************************/
void PtApplication::error_callback( int error, const char* description )
{
	fprintf( stderr, "Error: %s\n", description );
}

/******************************************************************************
 * Initialize windows
 ******************************************************************************/
void PtApplication::initializeWindows()
{
	// Initialize windows
	mMainWindow->initializeWindow();
}

/******************************************************************************
 * Initialize GL library
 ******************************************************************************/
void PtApplication::initializeGL()
{
	// NOTE: the following GLFW functions require a context to be current
	glfwMakeContextCurrent( mMainWindow->getWindow() );

	// Load OpenGL API/extensions
	gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
	
	// Managing swap interval
	//glfwSwapInterval( 1 );
	glfwSwapInterval( 0 );

	// Initialize
	mMainWindow->initializeGL();
}

/******************************************************************************
* Initialize ImGui user interface
******************************************************************************/
void PtApplication::initializeImGuiUserInterface()
{
	// Need a GL context?
	glfwMakeContextCurrent( mMainWindow->getWindow() );

	// Initialize ImGui
	// - GL binding
	//ImGui_ImplGlfwGL3_Init( mMainWindow->getWindow(), true ); // TODO: check callback: erased/overlapped by ImGui: use custom and call ImGui one !!! () => use FALSE !!!!
	// - style
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsDark();
}

/******************************************************************************
 * Execute
 * - main event loop
 ******************************************************************************/
void PtApplication::execute()
{
	// Set context
	// NOTE:
	// By default, making a context non-current implicitly forces a pipeline flush
	// On machines that support GL_KHR_context_flush_control, you can control whether a context performs this flush by setting the GLFW_CONTEXT_RELEASE_BEHAVIOR window hint.
	glfwMakeContextCurrent( mMainWindow->getWindow() );

	// Handle requests
	// - data model loading, descriptors computation, texture synthesis, painting, etc...
	handleRequests();

	// Process events
	glfwPollEvents();
}

/******************************************************************************
 * Handle requests
 ******************************************************************************/
void PtApplication::handleRequests()
{
	PtGraphics::PtGraphicsPPTBF* pptbfModel = getPPTBF();
	pptbfModel->generateDatabase( mBDDImageWidth, mBDDImageHeight, mBDDImageDirectory.c_str(), mBDDserieID );
}

/******************************************************************************
 * Post a request
 ******************************************************************************/
void PtApplication::postRequest( PtPipelineRequest pRequest )
{
	switch ( pRequest )
	{
		case eLoadDataModel:
			mOmniScaleModelUpdateRequested = true;
			break;

		case eSynthesizeTexture:
			mTextureSynthesisRequested = true;
			break;

		default:
			break;
	}
}

/******************************************************************************
 * Get PPTBF
 ******************************************************************************/
PtGraphicsPPTBF* PtApplication::getPPTBF()
{
	return mMainWindow->getPPTBF();
}

/******************************************************************************
 * BDD image parameters
 ******************************************************************************/
void PtApplication::setBDDImageWidth( const int pValue )
{
	mBDDImageWidth = pValue;
}

/******************************************************************************
 * BDD image parameters
 ******************************************************************************/
void PtApplication::setBDDImageHeight( const int pValue )
{
	mBDDImageHeight = pValue;
}

/******************************************************************************
 * BDD image parameters
 ******************************************************************************/
void PtApplication::setBDDImageDirectory( const char* pPath )
{
	mBDDImageDirectory = pPath;
}

/******************************************************************************
 * BDD serie ID
 ******************************************************************************/
void PtApplication::setBDDSerieID( const int pValue )
{
	mBDDserieID = pValue;
}

/******************************************************************************
 * PPTBF parameters
 ******************************************************************************/
void PtApplication::setPPTBFParameterFilename( const char* pPath )
{
	mPPTBFParameterFilename = pPath;
}
