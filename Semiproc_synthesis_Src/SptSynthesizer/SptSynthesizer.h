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

#ifndef _SPT_SYNTHESIZER_H_
#define _SPT_SYNTHESIZER_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "SptHviewInterface.h"

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
  * @class SptSynthesizer
  *
  * @brief The SptSynthesizer class provides interface to semi-procedural texture synthesis model.
  *
  * Semi-procedural texture synthesis separate process in two:
  * procedural stochastic structures are synthesized procedurally,
  * then color details are transfer through data-driven by-example synthesis.
  */
class SptSynthesizer
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
	SptSynthesizer();

	/**
	 * Destructor
	 */
	virtual ~SptSynthesizer();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Load synthesis parameters
	 *
	 * @param pFilename
	 */
	void loadParameters( const char* pFilename );

	/**
	 * Launch the synthesis pipeline
	 */
	virtual void execute();

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Interface to hview api
	 */
	SptHviewInterface mHviewInterface;

	/******************************** METHODS *********************************/

	/**
	 * Initialization stage
	 * - strategies/policies to choose blocks at initialization
	 */
	virtual void smartInitialization();

	/**
	 * Correction pass
	 */
	virtual void correction();

	/**
	 * Upsampling pass
	 */
	virtual void upsampling();

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class SptSynthesizer

} // end of namespace Spt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#endif // _SPT_SYNTHESIZER_H_