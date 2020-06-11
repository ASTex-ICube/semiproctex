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

#include "SptHviewInterface.h"
 
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

// hview
#include <hvPictRGB.h>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Spt;

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
 * Constructor
 ******************************************************************************/
SptHviewInterface::SptHviewInterface()
{
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
SptHviewInterface::~SptHviewInterface()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void SptHviewInterface::initialize()
{
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void SptHviewInterface::finalize()
{
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptHviewInterface::execute()
{
	// Call hview texture synthesis api
	// TODO
	//...
//	hview::semiProcTexwdistguidance(
		//char *name,
		//int STOPATLEVEL,
		//int posx, int posy,
		//const hvPictRGB<T> &example, const hvPictRGB<T> &exdist,
		//double weight, // weight color vs distance
		//double powr, float indweight, int neighbor, int atlevel, int bsize, float ERR,
		//const hvBitmap &mask, const hvPictRGB<T> &guidance,
		//hvArray2<hvVec2<int> > &index
//	);
}
