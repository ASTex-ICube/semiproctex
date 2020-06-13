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

#ifndef _SPT_BASIC_SYNTHESIZER_H_
#define _SPT_BASIC_SYNTHESIZER_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

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

// Project
namespace Spt
{
	class SptBasicHviewInterface;
}

 /******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

namespace Spt
{

 /**
  * @class SptBasicSynthesizer
  *
  * @brief The SptBasicSynthesizer class provides interface to semi-procedural texture synthesis model.
  *
  * Semi-procedural texture synthesis separate process in two:
  * procedural stochastic structures are synthesized procedurally,
  * then color details are transfer through data-driven by-example synthesis.
  *
  * SptBasicSynthesizer is a basic synthesizer that only requires 3 files:
  * - an input exemplar name xxx_scrop.png
  * - an input segmented exemplar name xxx_seg_scrop.png
  * - a pptbf parameters file xxx_seg_scrop_pptbf_params.txt
  * where xxx is texture name.
  *
  * Launch command: SptBasicSynthesizer.exe cracked_asphalt_160796 0.9 0.5 2 64 100 0.0
  *
  * Detailed parameters : TODO => detail parameters list!
  * Example :
  * GUIDE 0.9
  * STRENGTH 0.5
  * INITLEVEL 2
  * BLOCSIZE 64
  * INITERR 100
  * INDEXWEIGHT 0
  */
class SptBasicSynthesizer
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
	SptBasicSynthesizer();

	/**
	 * Destructor
	 */
	virtual ~SptBasicSynthesizer();

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
	virtual void execute();

	/**
	 * Save/export synthesis results
	 */
	virtual void saveResults();

	/**
	 * Semi-procedural texture synthesis parameters
	 */
	void setExemplarName( const char* pText );
	void setGUIDE( const float pValue );
	void setSTRENGTH( const float pValue );
	void setINITLEVEL( const int pValue );
	void setBLOCSIZE( const int pValue );
	void setINITERR( const float pValue );
	void setINDEXWEIGHT( const float pValue );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Interface to hview api
	 */
	SptBasicHviewInterface* mHviewInterface;

	///**
	// * Semi-procedural texture synthesis parameters
	// */
	//float GUIDE;
	//float STRENGTH;
	//int INITLEVEL;
	//int BLOCSIZE;
	//float INITERR;
	//float INDEXWEIGHT;

	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class SptBasicSynthesizer

} // end of namespace Spt

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#endif // _SPT_BASIC_SYNTHESIZER_H_
