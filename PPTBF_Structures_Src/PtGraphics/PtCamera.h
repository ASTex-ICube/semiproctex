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

#ifndef _PT_CAMERA_H_
#define _PT_CAMERA_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

#include "PtGraphicsConfig.h"

// glm
#include <glm/glm.hpp>

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

namespace PtGraphics
{

/** 
 * @class PtCamera
 *
 * @brief The PtCamera class provides ...
 *
 * ...
 */
class PTGRAPHICS_EXPORT PtCamera
{
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Camera type
	 */
	enum ECameraType
	{
		ePerspective,
		eOrthographic
	};

	/******************************** METHODS *********************************/

	/**
	 * Default constructor.
	 */
	PtCamera();
	
	/**
	 * Destructor.
	 */
	virtual ~PtCamera();

	void tilt( float pValue );
	void pan( float pValue );
	void dolly( float pValue );
	void truck( float pValue );
	void pedestal( float pValue );
	void zoom( float pValue );

	void setEye( const glm::vec3& pPosition );
	void setFront( const glm::vec3& pPosition );
	void setUp( const glm::vec3& pDirection );
	
	float getFovY() const;
	void setFovY( float pValue );
	float getAspect() const;
	void setAspect( float pValue );

	float getLeft() const;
	void setLeft( float pValue );
	float getRight() const;
	void setRight( float pValue );
	float getBottom() const;
	void setBottom( float pValue );
	float getTop() const;
	void setTop( float pValue );
	
	float getZNear() const;
	void setZNear( float pValue );
	float getZFar() const;
	void setZFar( float pValue );
	
	float getSensitivity() const;
	void setSensitivity( float pValue );

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	void setCameraType( ECameraType pType );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:
	
	/******************************* ATTRIBUTES *******************************/

	glm::vec3 mEye;
	glm::vec3 mFront;
	glm::vec3 mUp;
	
	/**
	 * Perspective
	 */
	float mFovY;
	float mAspect;
	
	/**
	 * Orthographic
	 */
	float mLeft;
	float mRight;
	float mBottom;
	float mTop;

	float mZNear;
	float mZFar;
	
	float mSensitivity;

	/**
	 * Camera type
	 */
	ECameraType mCameraType;
	
	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:
	
	/******************************* ATTRIBUTES *******************************/
	
	/******************************** METHODS *********************************/

};

}

#endif
