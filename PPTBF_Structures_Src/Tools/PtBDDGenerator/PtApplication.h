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

#ifndef _PT_APPLICATION_H_
#define _PT_APPLICATION_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtViewerConfig.h"
//#include "PtPipelineManager.h"

// STL
#include <string>

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
}
namespace PtGUI
{
	class PtViewer;
}

/******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace PtGUI
{

/** 
 * @class PtApplication
 *
 * @brief The PtApplication class provides ...
 *
 * ...
 */
class PtApplication
{
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/**
	 * Type of pipeline request
	 */
	enum PtPipelineRequest
	{
		eLoadDataModel,
		eSynthesizeTexture,
		eUpdateTexture,
		eRequestNbTypes
	};

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**
	 * Get the unique instance.
	 *
	 * @return the unique instance
	 */
	static PtApplication& get();

	/**
	 * Destructor.
	 */
	virtual ~PtApplication();

	/**
	 * Initialize
	 */
	virtual bool initialize( const char* const pWorkingDirectory );

	/**
	 * Finalize
	 */
	virtual void finalize();

	/**
	 * Execute
	 * - main event loop
	 */
	virtual void execute();

	/**
	 * Post a request
	 */
	void postRequest( PtPipelineRequest pRequest );

	/**
	 * Get PPTBF
	 */
	PtGraphics::PtGraphicsPPTBF* getPPTBF();

	/**
	 * BDD image parameters
	 */
	void setBDDImageWidth( const int pValue );
	void setBDDImageHeight( const int pValue );
	void setBDDImageDirectory( const char* pPath );
	void setBDDSerieID( const int pValue );

	void setPPTBFParameterFilename( const char* pPath );
		
	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:
	
	/******************************* ATTRIBUTES *******************************/

	/**
	 * Main window
	 */
	PtViewer* mMainWindow;

	/**
	 * Requests
	 */
	bool mOmniScaleModelUpdateRequested;
	bool mTextureSynthesisRequested;

	/**
	 * BDD image parameters
	 */
	int mBDDImageWidth;
	int mBDDImageHeight;
	std::string mBDDImageDirectory;
	int mBDDserieID;

	std::string mPPTBFParameterFilename;

	/******************************** METHODS *********************************/

	/**
	 * Initialize GLFW
	 */
	virtual void initializeGLFW();

	/**
	 * Finalize GLFW
	 */
	virtual void finalizeGLFW();

	/**
	 * Initialize windows
	 */
	virtual void initializeWindows();

	/**
	 * Initialize GL library
	 */
	virtual void initializeGL();

	/**
	 * Initialize ImGui user interface
	 */
	virtual void initializeImGuiUserInterface();

	///**
	// * Process Events
	// */
	//virtual void processEvents();

	/**
	 * Handle requests
	 */
	virtual void handleRequests();

	/**
	 * GLFW error callback
	 */
	static void error_callback( int error, const char* description );

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:
	
	/******************************* ATTRIBUTES *******************************/
	
	/**
	 * The unique instance
	 */
	static PtApplication* msInstance;

	/******************************** METHODS *********************************/

	/**
	 * Constructor
	 */
	PtApplication();
};

}

#endif
