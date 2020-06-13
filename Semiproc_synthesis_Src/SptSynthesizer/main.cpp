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
#include "SptBasicSynthesizer.h"

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

/**
 * Basic software synthesizer
 *
 * SptBasicSynthesizer is a basic synthesizer that only requires 3 files:
 * - an input exemplar name xxx_scrop.png
 * - an input segmented exemplar name xxx_seg_scrop.png
 * - a pptbf parameters file xxx_seg_scrop_pptbf_params.txt
 * where xxx is texture name.
 */
#define _USE_BASIC_SYNTHESIZER_

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
//
// To sumplify data management when loading multiple files,
// data MUST respect a strict naming convention :
//
// the SemiProcTexProject.exe software works by providing a semi-procedural texture file as parameter, such as :
//C:\PPTBF\Bin\SemiProcTexProject.exe C : \PPTBF\Data\Matching_structures\cells\TexturesCom_FloorsRounded0112_S_v2_scrop_synthesis_params.txt
//	BEWARE : required data MUST be in same directory than parameter file :
//    - xxx_scrop.png(input exemplar)
//	  - xxx_seg_scrop.png(binary structure of input exemplar)
//	  - xxx_scrop_semiproctex_pptbf_params.txt(PPTBF parameter file of input exemplar)
//	  - xxx_scrop_synthesis_params.txt(semi - procedural texture parameter file of input exemplar)

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
#ifndef _USE_BASIC_SYNTHESIZER_
	const int nbArguments = 1;
	if ( pArgc < ( 1 + nbArguments ) )
	{
		// Log info
		std::cout << "Error: waiting for " << nbArguments << " parameter(s)" << std::endl;
		std::cout << "       ex: program semiProcTex_params.txt"<< std::endl;

		// Exit
		return -1;
	}
#else
	// Example: SptBasicSynthesizer.exe cracked_asphalt_160796 0.9 0.5 2 64 100 0.0
	const int nbArguments = 7;
	if ( pArgc < ( 1 + nbArguments ) )
	{
		// Log info
		std::cout << "Error: waiting for " << nbArguments << " parameter(s)" << std::endl;
		std::cout << "       ex: program textureName GUIDE STRENGTH INITLEVEL BLOCSIZE INITERR INDEXWEIGHT" << std::endl;

		// Exit
		return -1;
	}
#endif

	// Retrieve program directory
	int indexParameter = 0;
	std::string workingDirectory = "";
	workingDirectory = pArgv[ indexParameter++ ];

	// User customizable parameters : retrieve command line parameters
#ifndef _USE_BASIC_SYNTHESIZER_
	const char* semiProcTexConfigFilename = pArgv[ indexParameter++ ];
#else
	const char* textureName = pArgv[ indexParameter++ ];
	std::string mExemplarName = std::string( textureName );
	float GUIDE = std::stof( pArgv[ indexParameter++ ] ); // default: 0.99
	float STRENGTH = std::stof( pArgv[ indexParameter++ ] ); // default: 0.5
	int INITLEVEL = std::stoi( pArgv[ indexParameter++ ] ); // default: 0
	int BLOCSIZE = std::stoi( pArgv[ indexParameter++ ] ); // default: 0
	float INITERR = std::stof( pArgv[ indexParameter++ ] ); // default: 0.5
	float INDEXWEIGHT = std::stof( pArgv[ indexParameter++ ] ); // default: 0.5
#endif

	// Initialization
#ifndef _USE_BASIC_SYNTHESIZER_
	Spt::SptSynthesizer* semiProcTexSynthesizer = new Spt::SptSynthesizer();
#else
	Spt::SptBasicSynthesizer* semiProcTexSynthesizer = new Spt::SptBasicSynthesizer();
#endif
	// - initialize resources
	semiProcTexSynthesizer->initialize();

#ifndef _USE_BASIC_SYNTHESIZER_
	// Load synthesis parameters
	int errorStatus = semiProcTexSynthesizer->loadParameters( semiProcTexConfigFilename );
	assert( errorStatus != -1 );
#else
	// Set synthesis parameters
	semiProcTexSynthesizer->setExemplarName( mExemplarName.c_str() );
	semiProcTexSynthesizer->setGUIDE( GUIDE );
	semiProcTexSynthesizer->setSTRENGTH( STRENGTH );
	semiProcTexSynthesizer->setINITLEVEL( INITLEVEL );
	semiProcTexSynthesizer->setBLOCSIZE( BLOCSIZE );
	semiProcTexSynthesizer->setINITERR( INITERR );
	semiProcTexSynthesizer->setINDEXWEIGHT( INDEXWEIGHT );
#endif

	// Launch synthesis
	semiProcTexSynthesizer->execute();

#ifndef _USE_BASIC_SYNTHESIZER_
	// Save/export results and data
	semiProcTexSynthesizer->saveResults();
#endif
	
	// Finalization
	// - clean/release resources
	semiProcTexSynthesizer->finalize();
	delete semiProcTexSynthesizer;
	semiProcTexSynthesizer = nullptr;

	// Exit
	return 0;
}
