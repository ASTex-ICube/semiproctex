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

// STL
#include <iostream>
#include <string>

// System
#include <cstdlib>
#include <ctime>

// Project
#include "PtApplication.h"
#include <PtEnvironment.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

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
 * Main entry program
 *
 * @param pArgc Number of arguments
 * @param pArgv List of arguments
 *
 * @return flag telling whether or not it succeeds
 ******************************************************************************/
int main( int pArgc, char** pArgv )
{
	std::cout << "------------------------" << std::endl;
	std::cout << "- PPTBF Designer Tool --" << std::endl;
	std::cout << "------------------------" << std::endl;

	// Retrieve program directory
	int indexParameter = 0;
	std::string workingDirectory = "";
	workingDirectory = pArgv[ indexParameter++ ];

	PtGUI::PtApplication::get();
	PtGUI::PtApplication::get().initialize( workingDirectory.c_str() );
	
	// Special settings
	// - check command line arguments
	const int nbArguments = 1;
	if ( pArgc >= ( 1 + nbArguments ) )
	{
		Pt::PtEnvironment::mImagePath = std::string( pArgv[ indexParameter++ ] );
	}

	PtGUI::PtApplication::get().execute();

	PtGUI::PtApplication::get().finalize();
	// todo: add a GLOBAL static finalize();

	return 0;
}
