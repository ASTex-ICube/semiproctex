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

#ifndef _PT_GRAPHICS_MESH_MANAGER_H_
#define _PT_GRAPHICS_MESH_MANAGER_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtGraphicsConfig.h"

// GL
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>

// STL
#include <vector>
#include <sstream>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

namespace PtGraphics
{
	class PtShaderProgram;
	class PtCamera;
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
class PTGRAPHICS_EXPORT PtGraphicsMeshManager
{
	
	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/******************************* INNER TYPES *******************************/

	/**
	 * Shader type enumeration
	 */
	enum class MeshType
	{
		eGridMesh = 0,
		eWaveMesh,
		eCylinderMesh,
		eTorusMesh,
		eSphereMesh,
		eNbMeshTypes
	};

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Model matrix
	 */
	glm::mat4 mModelMatrix; // TODO: for test...
	float mBump;

	/******************************** METHODS *********************************/
	
	/**
	 * Constructor
	 */
	PtGraphicsMeshManager();

	/**
	 * Destructor
	 */
	virtual ~PtGraphicsMeshManager();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Finalize
	 */
	void finalize();

	/**
	 * Render
	 */
	void render( const PtCamera* const pCamera );

	/**
	 * Render
	 */
	void renderBasic();
	
	/**
	 *
	 */
	void setPPTBFTexture( const GLuint pTexture );

	/**
	 * Set the mesh type
	 */
	void setMeshType( const MeshType pMeshType );
	inline MeshType getMeshType() const;

	// Material
	inline bool useMaterial() const;
	inline void setUseMaterial( const bool pFlag );
	// Color map
	inline int getColormapIndex() const;
	inline void setColormapIndex( const int pValue );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Mesh graphics resources
	 */
	GLuint mVAO;
	GLuint mPositionVBO;
	GLuint mTextureCoordinateVBO;
	GLuint mNormalVBO;
	GLuint mElementArrayBuffer;
	unsigned int mNbMeshIndices;

	bool mUseMaterial;
	MeshType mMeshType;

	/**
	 * PPTBF generator shader program
	 */
	PtShaderProgram* mShaderProgram;
	PtShaderProgram* mShaderProgram2;

	/**
	 * PPTBF texture
	 */
	GLuint mPPTBFTexture;

	// Appearance
	int mColormapIndex;

	/******************************** METHODS *********************************/

	/**
	 * Initialize shader programs
	 *
	 * @return a flag telling whether or not it succeeds
	 */
	bool initializeGraphicsResources();
	
	/**
	 * Initialize shader programs
	 *
	 * @return a flag telling whether or not it succeeds
	 */

	bool finalizeGraphicsResources();

	/**
	 * Initialize shader programs
	 *
	 * @return a flag telling whether or not it succeeds
	 */
	bool initializeShaderPrograms();

	/**
	 * Release graphics resources
	 */
	void setGraphicsResources( const std::vector< glm::vec3 >& pPositions,
							const std::vector< glm::vec2 >& pTextureCoordinates,
							const std::vector< glm::vec3 >& pNormals,
							const std::vector< unsigned int >& pIndices );

	/**
	 * Generate a grid mesh
	 */
	void generateGrid( const unsigned int pResolution );

	/**
	 * Generate a wave mesh
	 */
	void generateWave( const unsigned int pResolution );

	/**
	 * Generate a cylinder mesh
	 */
	void generateCylinder( const unsigned int pResolution );

	/**
	 * Generate a torus mesh
	 */
	void generateTorus( const unsigned int pResolution );

	/**
	 * Generate a sphere mesh
	 */
	void generateSphere( const unsigned int pResolution );

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/******************************* INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/


}; // end of class PtGraphicsMeshManager

} // end of namespace PtGraphics

/******************************************************************************
 ******************************* INLINE SECTION ******************************
 ******************************************************************************/

#include "PtGraphicsMeshManager.hpp"

#endif // _PT_GRAPHICS_MESH_MANAGER_H_
