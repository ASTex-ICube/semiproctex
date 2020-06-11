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

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "SptSynthesizer.h"

// STL
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

// System
#include <cstdlib>
#include <ctime>
#include <cassert>

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

//-----------------------------------------------------------------------------
// SOFTWARE INPUT
//-----------------------------------------------------------------------------
//
//1) 2 images of PPTBF (pptbf in grey levels + random pptbf labels at feature points (in grey level also?)
//
//2) 3 images of exemplar: color + binary structure + les labels
//
//3) 1 file containg numperical valeus of parameters :
//- threshold
//- synthesis parameters (smart init, neighborhoods, errors, etc.)
//
//The batch software load these files (5 images + 1 file) et generates the synthesis.

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
	// Log info
	std::cout << "---------------------------------" << std::endl;
	std::cout << "- SemiProcTex Synthesizer Tool --" << std::endl;
	std::cout << "---------------------------------" << std::endl;

	// Check command line arguments
	const int nbArguments = 1;
	if ( pArgc < ( 1 + nbArguments ) )
	{
		// Log info
		std::cout << "Error: waiting for " << nbArguments << "parameter(s)" << std::endl;
		std::cout << "       ex: program semiProcTex_params.txt"<< std::endl;

		// Exit
		return -1;
	}

	// Retrieve program directory
	int indexParameter = 0;
	std::string workingDirectory = "";
	workingDirectory = pArgv[ indexParameter++ ];

	// User customizable parameters : retrieve command line parameters
	const char* semiProcTexConfigFilename = pArgv[ indexParameter++ ];

	// Initialization
	Spt::SptSynthesizer* semiProcTexSynthesizer = new Spt::SptSynthesizer();
	// - initialize resources
	semiProcTexSynthesizer->initialize();

	// Load synthesis parameters
	int errorStatus = semiProcTexSynthesizer->loadParameters( semiProcTexConfigFilename );
	assert( errorStatus != -1 );

	// Launch synthesis
	semiProcTexSynthesizer->execute();
	
	// Finalization
	// - clean/release resources
	semiProcTexSynthesizer->finalize();
	delete semiProcTexSynthesizer;
	semiProcTexSynthesizer = nullptr;

	// Exit
	return 0;
}
