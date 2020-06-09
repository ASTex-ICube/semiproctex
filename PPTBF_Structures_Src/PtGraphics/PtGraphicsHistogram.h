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

#ifndef _PT_GRAPHICS_HISTOGRAM_H_
#define _PT_GRAPHICS_HISTOGRAM_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtGraphicsConfig.h"

// GL
#include <glad/glad.h>

// STL
#include <vector>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

namespace Pt
{
	class PtPPTBF;
}

namespace PtGraphics
{
	class PtShaderProgram;
}

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace PtGraphics
{

 /**
  * @class PtPPTBF
  *
  * @brief The PtPPTBF class provides interface to the "best matching tile selection" stage.
  *
  * The best matching tile selection occurs during the final texture synthesis stage:
  * - for each tile of a given composition map, a list of candidate tiles matching some features/characteristics is determined,
  * - the selector job is to select the best one among all acandidates.
  */
class PTGRAPHICS_EXPORT PtGraphicsHistogram
{
	
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Textures
	 ** TODO: move this to "protected"
	 */
	GLuint mPPTBFFrameBuffer;
	GLuint mPPTBFTexture;

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	PtGraphicsHistogram();

	/**
	 * Destructor
	 */
	virtual ~PtGraphicsHistogram();

	/**
	 * Initialize
	 */
	void initialize( const int pNbBins );

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Compute
	 */
	void compute( const GLuint pTexture, const int pTextureWidth, const int pTextureHeight );

	/**
	 * Get histogram
	 */
	void getHistogram( std::vector< float >& pHistogram, std::vector< float >& pCDF, const int pTextureWidth, const int pTextureHeight );

	/**
	 * Render
	 */
	void render();

	/**
	 * Number of bins
	 */
	int getNbBins() const;
	void setNbBins( const int pValue );
			
	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * PPTBF generator shader program
	 */
	PtShaderProgram* mShaderProgram;

	/**
	 * Number of bins
	 */
	int mNbBins;
	
	/******************************** METHODS *********************************/

	/**
	 * Initialize
	 */
	void initializeFramebuffer();
	bool initializeShaderProgram();

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class PtGraphicsHistogram

} // end of namespace PtGraphics

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#include "PtGraphicsHistogram.hpp"

#endif // _PT_GRAPHICS_PPTBF_H_
