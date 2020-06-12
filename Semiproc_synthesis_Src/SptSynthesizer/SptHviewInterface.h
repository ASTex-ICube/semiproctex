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

#ifndef _SPT_HVIEW_INTERFACE_H_
#define _SPT_HVIEW_INTERFACE_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

// hview
#include <hvVec2.h>
#include <hvArray2.h>
#include <hvBitmap.h>
#include <hvPictRGB.h>

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

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace Spt
{

 /**
  * @class SptHviewInterface
  *
  * @brief The SptHviewInterface class provides interface to the hview software texture synthesis api.
  *
  * SptHviewInterface is an wrapper interaface the hview software texture synthesis api.
  */
class SptHviewInterface
{
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	SptHviewInterface();

	/**
	 * Destructor
	 */
	virtual ~SptHviewInterface();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Launch the synthesis pipeline
	 */
	void execute();

	/**
	 * Load exemplar (color texture)
	 *
	 * @param pFilename
	 */
	void loadExemplar( const char* pFilename );

	/**
	 * Load structure map (binary)
	 *
	 * @param pFilename
	 */
	void loadExemplarStructureMap( const char* pFilename );

	/**
	 * Load distance map
	 *
	 * @param pFilename
	 */
	void loadExemplarDistanceMap( const char* pFilename );

	/**
	 * Load label map
	 *
	 * @param pFilename
	 */
	void loadExemplarLabelMap( const char* pFilename );

	/**
	 * Load guidance PPTBF
	 *
	 * @param pFilename
	 */
	void loadGuidancePPTBF( const char* pFilename );

	/**
	 * Load guidance mask
	 *
	 * @param pFilename
	 */
	void loadGuidanceMask( const char* pFilename );

	/**
	 * Load guidance distance map
	 *
	 * @param pFilename
	 */
	void loadGuidanceDistanceMap( const char* pFilename );

	/**
	 * Load guidance label map
	 *
	 * @param pFilename
	 */
	void loadGuidanceLabelMap( const char* pFilename );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * hview synthesis parameters
	 */
	// - input exemplar name
	std::string mName;
	// - synthesis algorithm
	int mSTOPATLEVEL;
	// - global translation (used for large maps generated in smaller tiles)
	int mPosx;
	int mPosy;
	// - exemplar
	hview::hvPictRGB< unsigned char > mExample;
	// - exemplar distance map
	hview::hvPictRGB<unsigned char> mExdist;
	double mWeight; // weight color vs distance
	// - synthesis algorithm
	double mPowr; // ...
	float mIndweight; // amount of error added when error on labels while searching for better pixel candidates
	int mNeighbor; // neighborhood half-patch size while searching for better pixel candidates
	int mAtlevel; // starting level of the pyramid LOD hierarchy
	int mBsize; // block size at starting level
	float mERR; // error at smart initialization
	// - guidance mask (thresholded PPTBF + potential constraints relaxation on structure borders)
	hview::hvBitmap mMask;
	// - guidance distance and labels
	hview::hvPictRGB<unsigned char> mGuidance;
	// - main synthesized UV map (indirection map: texture coordinates)
	hview::hvArray2< hview::hvVec2< int > > mIndex;
	// - synthesized output texture
	hview::hvPictRGB< unsigned char > mRes;

	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class SptHviewInterface

} // end of namespace Spt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#endif // _SPT_HVIEW_INTERFACE_H_
