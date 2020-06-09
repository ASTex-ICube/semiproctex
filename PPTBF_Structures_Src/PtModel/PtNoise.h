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

#ifndef _PT_NOISE_H_
#define _PT_NOISE_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtConfig.h"

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

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace Pt
{

 /**
  * @class PtNoise
  *
  * @brief The PtNoise class provides interface to the "best matching tile selection" stage.
  *
  * The best matching tile selection occurs during the final texture synthesis stage:
  * - for each tile of a given composition map, a list of candidate tiles matching some features/characteristics is determined,
  * - the selector job is to select the best one among all acandidates.
  */
class PTMODEL_EXPORT PtNoise
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
	PtNoise();

	/**
	 * Destructor
	 */
	virtual ~PtNoise();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Evaluate PPTBF
	 */
	float eval();

	inline static const std::vector< int >& getP();
	inline static const std::vector< float >& getRndTab();
	inline static const std::vector< float >& getG();

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Permutation table
	 */
	static const std::vector< int > mP;

	/**
	 * rnd_tab
	 */
	static const std::vector< float > mRndTab;

	/**
	 * G
	 */
	static const std::vector< float > mG;

	/******************************** METHODS *********************************/


}; // end of class PtNoise

} // end of namespace Pt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#include "PtNoise.hpp"

#endif // _PT_NOISE_H_
