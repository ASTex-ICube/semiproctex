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

// STL
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

// System
#include <cstdlib>
#include <ctime>

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
	std::cout << "---------------------------------" << std::endl;
	std::cout << "- SemiProcTex Synthesizer Tool --" << std::endl;
	std::cout << "---------------------------------" << std::endl;

	// Retrieve program directory
	int indexParameter = 0;
	std::string workingDirectory = "";
	workingDirectory = pArgv[ indexParameter++ ];

	// TODO
	// ...

	return 0;
}
