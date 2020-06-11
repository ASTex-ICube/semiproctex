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

#include "SptImageHelper.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// stb
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

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

// Project
using namespace Spt;

/******************************************************************************
 * Load image
 ******************************************************************************/
void SptImageHelper::loadImage( const char* pFilename, int& pWidth, int& pHeight, int& pNrChannels, unsigned char*& pData, int desired_channels )
{
	stbi_set_flip_vertically_on_load( 1 );
	pData = stbi_load( pFilename, &pWidth, &pHeight, &pNrChannels, desired_channels );
}

/******************************************************************************
 * Free image data
 ******************************************************************************/
void SptImageHelper::freeImage( unsigned char* pData )
{
	// Free memory
	stbi_image_free( pData );
}

/******************************************************************************
 * Save image
 ******************************************************************************/
int SptImageHelper::saveImage( const char* pFilename, const int pWidth, const int pHeight, const int pNrChannels, const void* pData )
{
	const void* data = pData;
	const int stride_in_bytes = pNrChannels * pWidth;
	stbi_flip_vertically_on_write( 1 );
	int status = stbi_write_png( pFilename, pWidth, pHeight, pNrChannels, data, stride_in_bytes );

	return status;
}
