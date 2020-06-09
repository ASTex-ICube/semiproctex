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

#ifndef _PT_SHADER_PROGRAM_H_
#define _PT_SHADER_PROGRAM_H_

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

#include "PtGraphicsConfig.h"

// OpenGL
#include <glad/glad.h>

// STL
#include <string>
#include <vector>
#include <map>
#include <utility>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
 * @class PtShaderProgram
 *
 * @brief The PtShaderProgram class provides interface to handle a ray map.
 *
 * Ray map is a container of ray initialized for the rendering phase.
 */
class PTGRAPHICS_EXPORT PtShaderProgram
{

	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/****************************** INNER TYPES *******************************/

	/**
	 * Shader type enumeration
	 */
	enum class ShaderType
	{
		eVertexShader = 0,
		eTesselationControlShader,
		eTesselationEvaluationShader,
		eGeometryShader,
		eFragmentShader,
		eComputeShader,
		eNbShaderTypes
	};

	/**
	 * Shader type names
	 */
	static const char* ShaderTypeName[ static_cast< int >( ShaderType::eNbShaderTypes ) ];

	/**
	 * Type definition to manipulate shaders as (shadertype, filename)
	 */
	typedef std::vector< std::pair< ShaderType, std::string > > TShaderList;

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Main shader program
	 */
	GLuint _program;

	/******************************** METHODS *********************************/

	/**
	 * Constructor
	 */
	PtShaderProgram();

	/**
	 * Destructor
	 */
	virtual ~PtShaderProgram();

	/**
	 * Initialize
	 *
	 * @return a flag to tell whether or not it succeeds.
	 */
	bool initialize();

	/**
	 * Finalize
	 *
	 * @return a flag to tell whether or not it succeeds.
	 */
	bool finalize();

	/**
	 * Compile shader
	 */
	bool addShader( ShaderType pShaderType, const std::string& pShaderFileName );

	/**
	 * Link program
	 */
	bool link();

	/**
	 * Initialize program with shaders as (shadertype, filename)
	 */
	bool initializeProgram( const TShaderList& pShaders );

	/**
	 * Use program
	 */
	inline void use() const;

	/**
	 * Unuse program
	 */
	static inline void unuse();

	/**
	 * Tell whether or not pipeline has a given type of shader
	 *
	 * @param pShaderType the type of shader to test
	 *
	 * @return a flag telling whether or not pipeline has a given type of shader
	 */
	bool hasShaderType( ShaderType pShaderType ) const;

	/**
	 * Get the source code associated to a given type of shader
	 *
	 * @param pShaderType the type of shader
	 *
	 * @return the associated shader source code
	 */
	std::string getShaderSourceCode( ShaderType pShaderType ) const;

	/**
	 * Get the filename associated to a given type of shader
	 *
	 * @param pShaderType the type of shader
	 *
	 * @return the associated shader filename
	 */
	std::string getShaderFilename( ShaderType pShaderType ) const;

	/**
	 * ...
	 *
	 * @param pShaderType the type of shader
	 *
	 * @return ...
	 */
	bool reloadShader( ShaderType pShaderType );

	/**
	 * Register uniform names
	 */
	void registerUniforms( const std::vector< std::string >& pUniformNames );

	/**
	 * Set uniform
	 */
	template< typename TType >
	void set( const TType& pValue, const char* pName );

	/**
	 * Shader name (short, no space)
	 */
	inline const char* getName() const;
	
	/**
	 * Shader name (short, no space)
	 */
	inline void setName( const char* pText );

	/**
	 * Shader info (name, etc...)
	 */
	inline const char* getInfo() const;
	
	/**
	 * Shader info (name, etc...)
	 */
	inline void setInfo( const char* pText );

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Vertex shader file name
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _vertexShaderFilename;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Tesselation Control shader file name
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _tesselationControlShaderFilename;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Tesselation Evaluation shader file name
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _tesselationEvaluationShaderFilename;
#if defined _MSC_VER
#pragma warning( pop )
#endif
	/**
	 * Geometry shader file name
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _geometryShaderFilename;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Fragment shader file name
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _fragmentShaderFilename;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Compute shader file name
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _computeShaderFilename;
#if defined _MSC_VER
#pragma warning( pop )
#endif
	
	/**
	 * Vertex shader source code
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _vertexShaderSourceCode;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Tesselation Control shader source code
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _tesselationControlShaderSourceCode;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Tesselation Evaluation shader source code
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _tesselationEvaluationShaderSourceCode;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Geometry shader source code
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _geometryShaderSourceCode;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Fragment shader source code
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _fragmentShaderSourceCode;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	/**
	 * Compute shader source code
	 */
#if defined _MSC_VER
#pragma warning( push )
#pragma warning( disable:4251 )
#endif
	std::string _computeShaderSourceCode;
#if defined _MSC_VER
#pragma warning( pop )
#endif

	///**
	// * Main shader program
	// */
	//GLuint _program;

	/**
	 * Vertex shader
	 */
	GLuint _vertexShader;

	/**
	 * Tesselation Control shader
	 */
	GLuint _tesselationControlShader;

	/**
	 * Tesselation Evaluation shader
	 */
	GLuint _tesselationEvaluationShader;

	/**
	 * Geometry shader
	 */
	GLuint _geometryShader;

	/**
	 * Fragment shader
	 */
	GLuint _fragmentShader;

	/**
	 * Compute shader
	 */
	GLuint _computeShader;

	/**
	 * ...
	 */
	bool _linked;

	/**
	 * List of uniform locations
	 */
	std::map< std::string, GLint > mUniformLocations;

	/**
	 * Shader info (name, etc...)
	 */
	std::string mName;

	/**
	 * Shader info (name, etc...)
	 */
	std::string mInfo;

	/******************************** METHODS *********************************/

	/**
	 * ...
	 *
	 * @param pFilename ...
	 * @param pFileContent ...
	 *
	 * @return ...
	 */
	static bool getFileContent( const std::string& pFilename, std::string& pFileContent );

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**
	 * Copy constructor forbidden.
	 */
	PtShaderProgram( const PtShaderProgram& );

	/**
	 * Copy operator forbidden.
	 */
	PtShaderProgram& operator=( const PtShaderProgram& );

};

} // namespace PtGraphics

/**************************************************************************
 ***************************** INLINE SECTION *****************************
 **************************************************************************/

#include "PtShaderProgram.hpp"

#endif
