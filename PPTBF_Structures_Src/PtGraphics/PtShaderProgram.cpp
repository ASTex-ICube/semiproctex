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

#include "PtShaderProgram.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <string>
#include <iostream>
#include <vector>

#include <fstream>
#include <sstream>

#include <fstream>
#include <cerrno>

#include <cassert>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// GigaVoxels
using namespace PtGraphics;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

const char* PtShaderProgram::ShaderTypeName[ static_cast< int >( PtShaderProgram::ShaderType::eNbShaderTypes ) ] =
{
	"Vertex",
	"Tesselation Control",
	"TesselationEvaluation",
	"Geometry",
	"Fragment",
	"Compute"
};

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Constructor
 ******************************************************************************/
PtShaderProgram::PtShaderProgram()
:	_vertexShaderFilename()
,	_tesselationControlShaderFilename()
,	_tesselationEvaluationShaderFilename()
,	_geometryShaderFilename()
,	_fragmentShaderFilename()
,	_computeShaderFilename()
,	_vertexShaderSourceCode()
,	_tesselationControlShaderSourceCode()
,	_tesselationEvaluationShaderSourceCode()
,	_geometryShaderSourceCode()
,	_fragmentShaderSourceCode()
,	_computeShaderSourceCode()
,	_program( 0 )
,	_vertexShader( 0 )
,	_tesselationControlShader( 0 )
,	_tesselationEvaluationShader( 0 )
,	_geometryShader( 0 )
,	_fragmentShader( 0 )
,	_computeShader( 0 )
,	_linked( false )
,	mName()
,	mInfo()
{
	// Initialize graphics resources
	//initialize(); // NO: to avoid crash if graphics context has not been initialized !!!!
}

/******************************************************************************
 * Desconstructor
 ******************************************************************************/
PtShaderProgram::~PtShaderProgram()
{
	// Release graphics resources
	finalize();
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
bool PtShaderProgram::initialize()
{
	// First, check if a program has already been created
	// ...
	assert( _program == 0 );

	// Create program object
	_program = glCreateProgram();
	if ( _program == 0 )
	{
		// LOG
		// ...

		return false;
	}

	return true;
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
bool PtShaderProgram::finalize()
{
	// Check all data to release

	if ( _vertexShader )
	{
		glDetachShader( _program, _vertexShader );
		glDeleteShader( _vertexShader );
	}
	if ( _tesselationControlShader )
	{
		glDetachShader( _program, _tesselationControlShader );
		glDeleteShader( _tesselationControlShader );
	}
	if ( _tesselationEvaluationShader )
	{
		glDetachShader( _program, _tesselationEvaluationShader );
		glDeleteShader( _tesselationEvaluationShader );
	}
	if ( _geometryShader )
	{
		glDetachShader( _program, _geometryShader );
		glDeleteShader( _geometryShader );
	}
	if ( _fragmentShader )
	{
		glDetachShader( _program, _fragmentShader );
		glDeleteShader( _fragmentShader );
	}
	if ( _computeShader )
	{
		glDetachShader( _program, _computeShader );
		glDeleteShader( _computeShader );
	}
		
	// Delete program object
	if ( _program )
	{
		glDeleteProgram( _program );

		// Reset value ?
		_program = 0;
	}

	_linked = false;

	return true;
}

/******************************************************************************
 * Compile shader
 ******************************************************************************/
bool PtShaderProgram::addShader( PtShaderProgram::ShaderType pShaderType, const std::string& pShaderFileName )
{
	assert( _program != 0 );

	// Retrieve file content
	std::string shaderSourceCode;
	bool isReadFileOK = getFileContent( pShaderFileName, shaderSourceCode );
	if ( ! isReadFileOK )
	{
		std::cerr<<"Error: can't read file " << pShaderFileName << std::endl;
		// LOG
		// ...

		return false;
	}

	// Create shader object
	GLuint shader = 0;
	switch ( pShaderType )
	{
		case ShaderType::eVertexShader:
			shader = glCreateShader( GL_VERTEX_SHADER );
			break;

		case ShaderType::eTesselationControlShader:
			shader = glCreateShader( GL_TESS_CONTROL_SHADER );
			break;

		case ShaderType::eTesselationEvaluationShader:
			shader = glCreateShader( GL_TESS_EVALUATION_SHADER );
			break;

		case ShaderType::eGeometryShader:
			shader = glCreateShader( GL_GEOMETRY_SHADER );
			break;

		case ShaderType::eFragmentShader:
			shader = glCreateShader( GL_FRAGMENT_SHADER );
			break;

//TODO
			//- protect code if not defined
		case ShaderType::eComputeShader:
			shader = glCreateShader( GL_COMPUTE_SHADER );
			break;

		default:

			// LOG
			// ...

			return false;
	}

	// Check shader creation error
	if ( shader == 0 )
	{
		std::cerr << "Error creating shader " << pShaderFileName << std::endl;
		// LOG
		// ...

		return false;
	}

	switch ( pShaderType )
	{
		case ShaderType::eVertexShader:
			_vertexShader = shader;
			_vertexShaderFilename = pShaderFileName;
			_vertexShaderSourceCode = shaderSourceCode;
			break;

		case ShaderType::eTesselationControlShader:
			_tesselationControlShader = shader;
			_tesselationControlShaderFilename = pShaderFileName;
			_tesselationControlShaderSourceCode = shaderSourceCode;
			break;

		case ShaderType::eTesselationEvaluationShader:
			_tesselationEvaluationShader = shader;
			_tesselationEvaluationShaderFilename = pShaderFileName;
			_tesselationEvaluationShaderSourceCode = shaderSourceCode;
			break;

		case ShaderType::eGeometryShader:
			_geometryShader = shader;
			_geometryShaderFilename = pShaderFileName;
			_geometryShaderSourceCode = shaderSourceCode;
			break;

		case ShaderType::eFragmentShader:
			_fragmentShader = shader;
			_fragmentShaderFilename = pShaderFileName;
			_fragmentShaderSourceCode = shaderSourceCode;
			break;

		case ShaderType::eComputeShader:
			_computeShader = shader;
			_computeShaderFilename = pShaderFileName;
			_computeShaderSourceCode = shaderSourceCode;
			break;

		default:
			break;
	}

	// Replace source code in shader object
	const char* source = shaderSourceCode.c_str();
	glShaderSource( shader, 1, &source, NULL );
	/*int length = shaderSourceCode.size();
	glShaderSource( shader, 1, &source, &length );*/
	
	// Compile shader object
	glCompileShader( shader );

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );
	if ( compileStatus == GL_FALSE )
	{
		// LOG
		// ...

		GLint logInfoLength = 0;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logInfoLength );
		if ( logInfoLength > 0 )
		{
			// Return information log for shader object
			GLchar* infoLog = new GLchar[ logInfoLength ];
			GLsizei length = 0;
			glGetShaderInfoLog( shader, logInfoLength, &length, infoLog );

			// LOG
			std::cout << "\nPtShaderProgram::addShader() - compilation ERROR" << std::endl;
			std::cout << "File : " << pShaderFileName << std::endl;
			std::cout << infoLog << std::endl;			

			delete[] infoLog;
		}

		return false;
	}
	else
	{
		// Attach shader object to program object
		glAttachShader( _program, shader );
	}

	return true;
}

/******************************************************************************
 * Link program
 ******************************************************************************/
bool PtShaderProgram::link()
{
	assert( _program != 0 );

	if ( _linked )
	{
		return true;
	}

	if ( _program == 0 )
	{
		return false;
	}

	// Indicate to the implementation the intention of the application to retrieve the program's binary representation with glGetProgramBinary
	// NOTE: set this before calling glLinkProgram()
	glProgramParameteri( _program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
	
	// Link program object
	glLinkProgram( _program );

	// Check linking status
	GLint linkStatus = 0;
	glGetProgramiv( _program, GL_LINK_STATUS, &linkStatus );
	if ( linkStatus == GL_FALSE )
	{
		// LOG
		// ...

		GLint logInfoLength = 0;
		glGetProgramiv( _program, GL_INFO_LOG_LENGTH, &logInfoLength );
		if ( logInfoLength > 0 )
		{
			// Return information log for program object
			GLchar* infoLog = new GLchar[ logInfoLength ];
			GLsizei length = 0;
			glGetProgramInfoLog( _program, logInfoLength, &length, infoLog );

			// LOG
			std::cout << "\nPtShaderProgram::link() - compilation ERROR" << std::endl;
			std::cout << infoLog << std::endl;

			delete[] infoLog;
		}

		return false;
	}
	
	// Update internal state
	_linked = true;
	
	//--------------------
#if 0
	// Get a binary representation of a program object's compiled and linked executable source
	GLint formats = 0;
	glGetIntegerv( GL_NUM_PROGRAM_BINARY_FORMATS, &formats );
	GLint* binaryFormats = new GLint[ formats ];
	glGetIntegerv( GL_PROGRAM_BINARY_FORMATS, binaryFormats );
	GLint len = 0;
	glGetProgramiv( _program, GL_PROGRAM_BINARY_LENGTH, &len );
	//u8* binary = new u8[len];
	//GLenum *binaryFormats = 0;
	//glGetProgramBinary(progId, len, NULL, (GLenum*)binaryFormats, binary);
	//FILE* fp = fopen(shader.bin, "wb");
	//fwrite(binary, len, 1, fp);
	//fclose(fp);
	//delete [] binary;
	//const size_t MAX_SIZE = 1 << 16;
	//char binary[ MAX_SIZE ];
	char* binary = new char[ len ];
	GLenum format;
	GLint length;
	//glGetProgramBinary( _program, MAX_SIZE, &length, &format, &binary[ 0 ] );
	glGetProgramBinary( _program, len, &length, &format, &binary[ 0 ] );
	static int counter = 0;
	std::string shaderName;
	if ( ! mName.empty() )
	{
		shaderName = mName;
	}
	else
	{
		shaderName = std::to_string( counter );
	}
	const std::string assemblyFilename = std::string( "ASM_" ) + shaderName + std::string( ".txt" );
	std::ofstream binaryfile( assemblyFilename );
	binaryfile.write( binary, length );
	binaryfile.close();
	delete[] binary;
	++counter;
#endif
	//--------------------

	return true;
}

/******************************************************************************
 * Initialize program with shaders as (shadertype, filename)
 ******************************************************************************/
bool PtShaderProgram::initializeProgram( const TShaderList& pShaders )
{
	bool statusOK = false;

	// LOG
	std::cout << "\t" << mInfo << std::endl;

	// Add shaders
	std::cout << "\t\tCompiling: ";
	for ( const auto& data : pShaders )
	{
		// LOG: shader type
		std::cout << std::string( ShaderTypeName[ static_cast< int >( data.first ) ] ) << " ";
				
		// Create, compile and attach shader
		statusOK = addShader( data.first, data.second );
		assert( statusOK );
		if ( ! statusOK )
		{
			// TODO: clean resource
			//...
			return false;
		}
	}

	// Link program
	std::cout << "\n\t\tLinking..." << std::endl;
	statusOK = link();
	assert( statusOK );

	return statusOK;
}

/******************************************************************************
 * ...
 *
 * @param pFilename ...
 *
 * @return ...
 ******************************************************************************/
bool PtShaderProgram::getFileContent( const std::string& /*pFilename*/Filename, std::string& pFileContent )
{
	//std::ifstream file( pFilename.c_str(), std::ios::in );
	//if ( file )
	//{
	//	// Initialize a string to store file content
	//	file.seekg( 0, std::ios::end );
	//	pFileContent.resize( file.tellg() );
	//	file.seekg( 0, std::ios::beg );

	//	// Read file content
	//	file.read( &pFileContent[ 0 ], pFileContent.size() );

	//	// Close file
	//	file.close();

	//	return true;
	//}
	//else
	//{
	//	// LOG
	//	// ...
	//}

	std::string Result;
	
	std::ifstream Stream(Filename.c_str());
	if ( !Stream.is_open() )
	{
		//return Result;
		pFileContent = Result;
		return false;
	}
	
	Stream.seekg(0, std::ios::end);
	Result.reserve(Stream.tellg());
	Stream.seekg(0, std::ios::beg);
	
	Result.assign(
		(std::istreambuf_iterator<char>(Stream)),
		std::istreambuf_iterator<char>());
	   
	//return Result;
	pFileContent = Result;
	return true;

	//return false;
}

/******************************************************************************
 * Tell whether or not pipeline has a given type of shader
 *
 * @param pShaderType the type of shader to test
 *
 * @return a flag telling whether or not pipeline has a given type of shader
 ******************************************************************************/
bool PtShaderProgram::hasShaderType( ShaderType pShaderType ) const
{
	bool result = false;

	GLuint shader = 0;
	switch ( pShaderType )
	{
		case ShaderType::eVertexShader:
			shader = _vertexShader;
			break;

		case ShaderType::eTesselationControlShader:
			shader = _tesselationControlShader;
			break;

		case ShaderType::eTesselationEvaluationShader:
			shader = _tesselationEvaluationShader;
			break;

		case ShaderType::eGeometryShader:
			shader = _geometryShader;
			break;

		case ShaderType::eFragmentShader:
			shader = _fragmentShader;
			break;

		case ShaderType::eComputeShader:
			shader = _computeShader;
			break;

		default:

			assert( false );

			break;
	}

	return ( shader != 0 );
}

/******************************************************************************
 * Get the source code associated to a given type of shader
 *
 * @param pShaderType the type of shader
 *
 * @return the associated shader source code
 ******************************************************************************/
std::string PtShaderProgram::getShaderSourceCode( ShaderType pShaderType ) const
{
	std::string shaderSourceCode( "" );
	switch ( pShaderType )
	{
		case ShaderType::eVertexShader:
			shaderSourceCode = _vertexShaderSourceCode;
			break;
			
		case ShaderType::eTesselationControlShader:
			shaderSourceCode = _tesselationControlShaderSourceCode;
			break;

		case ShaderType::eTesselationEvaluationShader:
			shaderSourceCode = _tesselationEvaluationShaderSourceCode;
			break;
			
		case ShaderType::eGeometryShader:
			shaderSourceCode = _geometryShaderSourceCode;
			break;

		case ShaderType::eFragmentShader:
			shaderSourceCode = _fragmentShaderSourceCode;
			break;

		case ShaderType::eComputeShader:
			shaderSourceCode = _computeShaderSourceCode;
			break;

		default:

			assert( false );

			break;
	}

	return shaderSourceCode;
}

/******************************************************************************
 * Get the filename associated to a given type of shader
 *
 * @param pShaderType the type of shader
 *
 * @return the associated shader filename
 ******************************************************************************/
std::string PtShaderProgram::getShaderFilename( ShaderType pShaderType ) const
{
	std::string shaderFilename( "" );
	switch ( pShaderType )
	{
		case ShaderType::eVertexShader:
			shaderFilename = _vertexShaderFilename;
			break;
			
		case ShaderType::eTesselationControlShader:
			shaderFilename = _tesselationControlShaderFilename;
			break;

		case ShaderType::eTesselationEvaluationShader:
			shaderFilename = _tesselationEvaluationShaderFilename;
			break;
			
		case ShaderType::eGeometryShader:
			shaderFilename = _geometryShaderFilename;
			break;

		case ShaderType::eFragmentShader:
			shaderFilename = _fragmentShaderFilename;
			break;

		case ShaderType::eComputeShader:
			shaderFilename = _computeShaderFilename;
			break;

		default:

			assert( false );

			break;
	}

	return shaderFilename;
}

/******************************************************************************
 * ...
 *
 * @param pShaderType the type of shader
 *
 * @return ...
 ******************************************************************************/
bool PtShaderProgram::reloadShader( ShaderType pShaderType )
{
	if ( ! hasShaderType( pShaderType ) )
	{
		// LOG
		// ...

		return false;
	}

	// Retrieve file content
	std::string shaderSourceCode;
	std::string shaderFilename = getShaderFilename( pShaderType );
	bool isReadFileOK = getFileContent( shaderFilename, shaderSourceCode );
	if ( ! isReadFileOK )
	{
		// LOG
		// ...

		return false;
	}
	
	GLuint shader = 0;
	switch ( pShaderType )
	{
		case ShaderType::eVertexShader:
			shader = _vertexShader;
			break;

		case ShaderType::eTesselationControlShader:
			shader = _tesselationControlShader;
			break;

		case ShaderType::eTesselationEvaluationShader:
			shader = _tesselationEvaluationShader;
			break;

		case ShaderType::eGeometryShader:
			shader = _geometryShader;
			break;

		case ShaderType::eFragmentShader:
			shader = _fragmentShader;
			break;

		case ShaderType::eComputeShader:
			shader = _computeShader;
			break;

		default:
			break;
	}

	// Check shader creation error
	if ( shader == 0 )
	{
		// LOG
		// ...

		return false;
	}

	// Replace source code in shader object
	const char* source = shaderSourceCode.c_str();
	glShaderSource( shader, 1, &source, NULL );
	//int length = shaderSourceCode.size();
	//glShaderSource(shader, 1, &source, &length );

	// Compile shader object
	glCompileShader( shader );

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );
	if ( compileStatus == GL_FALSE )
	{
		// LOG
		// ...

		GLint logInfoLength = 0;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logInfoLength );
		if ( logInfoLength > 0 )
		{
			// Return information log for shader object
			GLchar* infoLog = new GLchar[ logInfoLength ];
			GLsizei length = 0;
			glGetShaderInfoLog( shader, logInfoLength, &length, infoLog );

			// LOG
			std::cout << "\nPtShaderProgram::reloadShader() - compilation ERROR" << std::endl;
			std::cout << infoLog << std::endl;

			delete[] infoLog;
		}

		return false;
	}

	// Link program
	//
	// - first, unliked the program
	_linked = false;
	if ( ! link() )
	{
		return false;
	}
	
	return true;
}

/******************************************************************************
 * Register uniform names
 ******************************************************************************/
void PtShaderProgram::registerUniforms( const std::vector< std::string >& pUniformNames )
{
	// Store uniforms info (after shader program link() step)
	for ( const auto& uniformName : pUniformNames )
	{
		GLint uniformLocation = glGetUniformLocation( _program, uniformName.c_str() );
		if ( uniformLocation >= 0 )
		{
			mUniformLocations[ uniformName ] = uniformLocation;
		}
	}
}
