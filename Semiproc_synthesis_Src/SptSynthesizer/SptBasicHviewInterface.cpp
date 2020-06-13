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

#include "SptBasicHviewInterface.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "SptImageHelper.h" 

// System
#include <cassert>

// STL
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>

// hview
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#include <hvVec2.h>
#include <hvArray2.h>
#include <hvBitmap.h>
#include <hvPictRGB.h>
#include "hvPicture.h"

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
SptBasicHviewInterface::SptBasicHviewInterface()
:	GUIDE( 0.f )
,	STRENGTH( 0.f )
,	INITLEVEL( 0 )
,	BLOCSIZE( 0 )
,	INITERR( 0.f )
,	INDEXWEIGHT( 0.f )
{
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
SptBasicHviewInterface::~SptBasicHviewInterface()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void SptBasicHviewInterface::initialize()
{
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void SptBasicHviewInterface::finalize()
{
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptBasicHviewInterface::execute()
{
	// TODO:
	// Put here, code of the basic software algorithm
	// ...

	const int SPOSX = 0;
	const int SPOSY = 0;

	const int NPPTBFS = 1;

	//float ZZ = 1.0;
	/*const */int PSIZE = 512;
	/*const*/ int STARTX = 10 * PSIZE;
	/*const*/ int STARTY = 10 * PSIZE;
	int shiftx = STARTX + SPOSX * PSIZE + 0 * PSIZE, shifty = STARTY + SPOSY * PSIZE;

	const int npptbf = 1;

	/*const*/ int padding = (1 << INITLEVEL)*BLOCSIZE;

	char* name[ npptbf ] = {
		"cracked_asphalt_160796"
	};

	char pname[ 500 ];

	hview::hvPictRGB<unsigned char> example[NPPTBFS];
	hview::hvBitmap exseg[NPPTBFS];

	int i, j, k, ii, jj;
	
	k = 0;

	sprintf( pname, "%s_scrop.png", name[ k ] );

	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* data = nullptr;
	SptImageHelper::loadImage( pname, width, height, nrChannels, data, desired_channels );
	if ( data == nullptr )
	{
		std::cout << "Failed to load image: " << pname << std::endl;
		assert( false );
		//return 1; // TODO: handle this!
	}

	///////////////////////////////////////////////////////////////////////
	assert( width == height );
	//PSIZE = 2 * width; // we want 2 times the original size for our tests !!

	//---------------------------------------------------------------------------------
	// PSIZE must be a multiple of BLOCSIZE
	//int deltaPSIZE = 0;
	//PSIZE += deltaPSIZE;
	{
		// mimic makepyr() method
		int s = 0;
		int f = 1;
		int w = width;
		int h = height;
		while ( s < INITLEVEL && w > 16 && h > 16 )
		{
			// shrink size
			w /= 2;
			h /= 2;

			// update pyramid info
			s++;
			f *= 2;
		}

		// Handle error
		if ( w < 2 * BLOCSIZE )
		{
			printf( "\nERROR: block size %d must be less or equal than %d", w / 2 );
			assert( false );
		}

		int outputSize = 2 * width; // we want 2 times the original size for our tests !!
		int exemplarBlockSize = f * BLOCSIZE;
		if ( width < exemplarBlockSize || height < exemplarBlockSize )
		{
			int maxBlockSize = std::min( width, height ) / f;
			printf( "\ERROR: block size %d must be less or equal to %d", BLOCSIZE, maxBlockSize );
			assert( false );
		}
		int nbBlocks = outputSize / ( f * BLOCSIZE ) + ( ( outputSize % ( f * BLOCSIZE ) ) == 0 ? 0 : 1 );
		PSIZE = nbBlocks * ( f * BLOCSIZE );
	}
	//---------------------------------------------------------------------------------

	STARTX = 10 * PSIZE;
	STARTY = 10 * PSIZE;
	shiftx = STARTX + SPOSX * PSIZE + 0 * PSIZE;
	shifty = STARTY + SPOSY * PSIZE;
	///////////////////////////////////////////////////////////////////////

	//exalbedo[k].reset(width, height, hvColRGB<unsigned char>(0));
	//for (int ii = 0; ii < width; ii++) for (int jj = 0; jj < height; jj++)
	//{
	//	exalbedo[k].update(ii, jj, hvColRGB<unsigned char>(data[nrChannels * (ii + jj*width)], data[nrChannels * (ii + jj*width) + 1], data[nrChannels * (ii + jj*width) + 2]));
	//}
	//stbi_image_free(data);

	//sprintf(pname, "%s_seg_scrop.png", name[k]);
	//data = nullptr;
	//stbi_set_flip_vertically_on_load(1);
	//data = stbi_load(pname, &width, &height, &nrChannels, 0);
	//if (data == nullptr)
	//{
	//	std::cout << "Failed to load image: " << pname << std::endl;
	//	assert(false);
	//	//return 1;
	//}
	//exseg[k].reset(width, height, false);
	//for (int ii = 0; ii < width; ii++) for (int jj = 0; jj < height; jj++)
	//{
	//	exseg[k].set(ii, jj, (int)data[nrChannels * (ii + jj*width)] > 128 ? true : false);
	//}
	//stbi_image_free(data);

	// TODO:
	// - continue moving original code here!
	// ...
}
