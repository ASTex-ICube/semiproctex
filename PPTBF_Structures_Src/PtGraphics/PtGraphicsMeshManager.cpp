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

#include "PtGraphicsMeshManager.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "PtShaderProgram.h"
#include "PtCamera.h"
#include <PtEnvironment.h>

// glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// System
#include <cassert>
#include <cstddef>

// STL
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iomanip>
#include <vector>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace PtGraphics;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/
	
/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Constructor
 ******************************************************************************/
PtGraphicsMeshManager::PtGraphicsMeshManager()
:	mVAO( 0 )
,	mPositionVBO( 0 )
,	mTextureCoordinateVBO( 0 )
,	mNormalVBO( 0 )
,	mElementArrayBuffer( 0 )
,	mNbMeshIndices( 0 )
,	mModelMatrix( glm::mat4( 1.f ) )
,	mUseMaterial( false )
,	mBump( 1.f )
,	mMeshType( MeshType::eGridMesh )
,	mColormapIndex( -1 )
{
	mShaderProgram = new PtShaderProgram();
	mShaderProgram2 = new PtShaderProgram();
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtGraphicsMeshManager::~PtGraphicsMeshManager()
{
	delete mShaderProgram;
	mShaderProgram = nullptr;

	delete mShaderProgram2;
	mShaderProgram2 = nullptr;
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void PtGraphicsMeshManager::initialize()
{
	initializeGraphicsResources();
	initializeShaderPrograms();
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void PtGraphicsMeshManager::finalize()
{
}

/******************************************************************************
 * Initialize per-pixel parameters graphics resources
 *
 * @return a flag telling whether or not it succeeds
 ******************************************************************************/
bool PtGraphicsMeshManager::initializeGraphicsResources()
{
	return true;
}

/******************************************************************************
 * Initialize per-pixel parameters graphics resources
 *
 * @return a flag telling whether or not it succeeds
 ******************************************************************************/
bool PtGraphicsMeshManager::finalizeGraphicsResources()
{
	// Release graphics resources
	if ( mVAO )
	{
		glDeleteVertexArrays( 1, &mVAO );
	}
	if ( mPositionVBO )
	{
		glDeleteBuffers( 1, &mPositionVBO );
	}
	if ( mTextureCoordinateVBO )
	{
		glDeleteBuffers( 1, &mTextureCoordinateVBO );
	}
	if ( mNormalVBO )
	{
		glDeleteBuffers( 1, &mNormalVBO );
	}
	if ( mElementArrayBuffer )
	{
		glDeleteBuffers( 1, &mElementArrayBuffer );
	}

	return true;
}

/******************************************************************************
 * ...
 ******************************************************************************/
void PtGraphicsMeshManager::setPPTBFTexture( const GLuint pTexture )
{
	mPPTBFTexture = pTexture;
}

/******************************************************************************
 * Set the mesh type
 ******************************************************************************/
void PtGraphicsMeshManager::setMeshType( const PtGraphicsMeshManager::MeshType pMeshType )
{
	switch ( pMeshType )
	{
		case MeshType::eGridMesh:
			generateGrid( mUseMaterial ? 1024 : 4 );
			break;

		case MeshType::eWaveMesh:
			generateWave( mUseMaterial ? 1024 : 64 );
			break;

		case MeshType::eCylinderMesh:
			generateCylinder( mUseMaterial ? 1024 : 64 );
			break;

		case MeshType::eTorusMesh:
			generateTorus( mUseMaterial ? 1024 : 64 );
			break;

		case MeshType::eSphereMesh:
			generateSphere( mUseMaterial ? 1024 : 64 );
			break;

		default:
			generateGrid( mUseMaterial ? 1024 : 4 );
			break;
	}

	mMeshType = pMeshType;
}

/******************************************************************************
 * Initialize shader program
 ******************************************************************************/
bool PtGraphicsMeshManager::initializeShaderPrograms()
{
	bool statusOK = false;

	/*delete mShaderProgram;
	mShaderProgram = nullptr;
	mShaderProgram = new PtShaderProgram();*/

	// Global variables
	const std::string shaderPath = Pt::PtEnvironment::mShaderPath + std::string( "/" );
	PtShaderProgram* shaderProgram = nullptr;
	PtShaderProgram::TShaderList shaders;
	std::string shaderFilename;
	std::vector< std::string > uniforms;

	// Rendering: fullscreen triangle
	// Initialize shader program
	shaderProgram = mShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "meshRenderer" );
	shaderProgram->setInfo( "Mesh Renderer" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "meshRendering_vert.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eVertexShader, shaderFilename ) );
	shaderFilename = shaderPath + "meshRendering_frag.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eFragmentShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	uniforms.push_back( "uProjectionMatrix" );
	uniforms.push_back( "uViewMatrix" );
	uniforms.push_back( "uModelMatrix" );
	uniforms.push_back( "uNormalMatrix" );
	uniforms.push_back( "uPPTBFTexture" );
	uniforms.push_back( "uColormapIndex" );
	shaderProgram->registerUniforms( uniforms );

	// Rendering: fullscreen triangle
	// Initialize shader program
	shaderProgram = mShaderProgram2;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "meshRenderer" );
	shaderProgram->setInfo( "Mesh Renderer" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "meshRendering_bump_vert.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eVertexShader, shaderFilename ) );
	shaderFilename = shaderPath + "meshRendering_bump_frag.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eFragmentShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	uniforms.push_back( "uProjectionMatrix" );
	uniforms.push_back( "uViewMatrix" );
	uniforms.push_back( "uModelMatrix" );
	uniforms.push_back( "uNormalMatrix" );
	uniforms.push_back( "uPPTBFTexture" );
	uniforms.push_back( "hmax" );
	uniforms.push_back( "uColormapIndex" );
	shaderProgram->registerUniforms( uniforms );

	return statusOK;
}

/******************************************************************************
 * Render mesh
 ******************************************************************************/
void PtGraphicsMeshManager::render( const PtCamera* const pCamera )
{
	// Set shader program
	PtShaderProgram* shaderProgram;
	if ( ! useMaterial() )
	{
		shaderProgram = mShaderProgram;
	}
	else
	{
		shaderProgram = mShaderProgram2;
	}
	shaderProgram->use();
	{
		// Set texture(s)
		// - PPTBF data
		glBindTextureUnit( 0/*unit*/, mPPTBFTexture );
		
		// Set uniform(s)
		// - camera
		const glm::mat4 viewMatrix = pCamera->getViewMatrix();
		const glm::mat4 projectionMatrix = pCamera->getProjectionMatrix();
		shaderProgram->set( viewMatrix, "uViewMatrix" );
		shaderProgram->set( projectionMatrix, "uProjectionMatrix" );
		// - model transform
		shaderProgram->set( mModelMatrix, "uModelMatrix" );
		// - appearance
		if ( useMaterial() )
		{
			shaderProgram->set( glm::mat3( glm::inverseTranspose( viewMatrix ) ), "uNormalMatrix" );
			shaderProgram->set( mBump, "hmax" );
		}
		shaderProgram->set( getColormapIndex(), "uColormapIndex" );
		// - PPTBF data
		shaderProgram->set( 0, "uPPTBFTexture" ); // Note: this uniform never changes => can be set after shaderprogram::link()

		// Set vertex array
		glBindVertexArray( mVAO );
						
		// Draw command(s)
		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDrawElements( GL_TRIANGLES, mNbMeshIndices, GL_UNSIGNED_INT, 0 );
		//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	}
	PtShaderProgram::unuse();
}

/******************************************************************************
 * Render baisc (i.e. no shader)
 ******************************************************************************/
void PtGraphicsMeshManager::renderBasic()
{
	// Set vertex array
	glBindVertexArray( mVAO );
						
	// Draw command(s)
	glDrawElements( GL_TRIANGLES, mNbMeshIndices, GL_UNSIGNED_INT, 0 );
}

/******************************************************************************
 * Generate a grid mesh
 ******************************************************************************/
void PtGraphicsMeshManager::generateGrid( const unsigned int n )
{
	assert( n > 0 );

	// Release graphics resources
	finalizeGraphicsResources();

	// Build geometry
	const unsigned int n1 = n - 1;
	std::vector< glm::vec3 > pos;
	std::vector< glm::vec2 > tc;
	std::vector< glm::vec3 > norm;
	pos.reserve( n * n );
	tc.reserve( n * n );
	norm.reserve( n * n );
	for ( unsigned int j = 0; j < n; ++j )
	{
		for ( unsigned int i = 0; i < n; ++i )
		{
			const float u = ( 1.f / n1 ) * i;
			const float v = ( 1.f / n1 ) * j;
			tc.push_back( glm::vec2( u, v ) );
			pos.push_back( glm::vec3( ( u - 0.5f ) * 2.f, ( v - 0.5f ) * 2.f, 0.f ) );
			norm.push_back( glm::vec3( 0.f, 0.f, 1.f ) );
		}
	}

	// Build topology
	std::vector< unsigned int > indices;
	indices.reserve( 6 * (n - 1)*(n - 1) );
	unsigned int last = 0;
	for ( unsigned int j = 1; j < n; ++j )
	{
		for ( unsigned int i = 1; i < n; ++i )
		{
			unsigned int k = j*n + i;

			// push quad
			indices.push_back(k);
			indices.push_back(k - n - 1);
			indices.push_back(k - n);

			indices.push_back(k - n - 1);
			indices.push_back(k);
			indices.push_back(k - 1);
		}
	}
	// Store number of indices (for rendering)
	mNbMeshIndices = indices.size();
	
	// Set graphics resources
	setGraphicsResources( pos, tc, norm, indices );
}

/******************************************************************************
 * Generate a wave mesh
 ******************************************************************************/
void PtGraphicsMeshManager::generateWave( const unsigned int n )
{
	assert( n > 0 );

	// Release graphics resources
	finalizeGraphicsResources();

	// Build geometry
	const unsigned int n1 = n - 1;
	std::vector< glm::vec3 > pos;
	std::vector< glm::vec2 > tc;
	std::vector< glm::vec3 > norm;
	pos.reserve( n * n );
	tc.reserve( n * n );
	norm.reserve( n * n );
	for ( unsigned int j = 0; j < n; ++j )
	{
		const float v = (1.0/n1)*j;
		for( unsigned int i = 0; i < n; ++i )
		{
			const float u = (1.0/n1)*i;
			tc.push_back( glm::vec2( u, v ) );

			const float x = ( u - 0.5 ) * 2;
			const float y = ( v - 0.5 ) * 2;
			const float r = glm::sqrt( x * x + y * y );
			const float h = 0.2f * ( 1.f - r / 2.f ) * glm::sin( glm::half_pi< float >() + r * 8.f );
			pos.push_back( glm::vec3( x, y, h ) );

			const float dh = -0.2/2 * glm::sin( glm::half_pi< float >() + r * 8.f ) +
							0.2 * (1-r/2) * 8.f * glm::cos( glm::half_pi< float >() + r * 8.f );
			const glm::vec3 n = glm::vec3( -x/r*dh, -y/r*dh, 1.f );
			norm.push_back( glm::normalize( n ) );
		}
	}
	
	// Build topology
	std::vector< unsigned int > indices;
	indices.reserve( 6 * (n - 1)*(n - 1) );
	unsigned int last = 0;
	for ( unsigned int j = 1; j < n; ++j )
	{
		for ( unsigned int i = 1; i < n; ++i )
		{
			unsigned int k = j*n + i;

			// push quad
			indices.push_back(k);
			indices.push_back(k - n - 1);
			indices.push_back(k - n);

			indices.push_back(k - n - 1);
			indices.push_back(k);
			indices.push_back(k - 1);
		}
	}
	// Store number of indices (for rendering)
	mNbMeshIndices = indices.size();
	
	// Set graphics resources
	setGraphicsResources( pos, tc, norm, indices );
}

/******************************************************************************
 * Generate a cylinder mesh
 ******************************************************************************/
void PtGraphicsMeshManager::generateCylinder( const unsigned int n )
{
	assert( n > 0 );

	// Release graphics resources
	finalizeGraphicsResources();

	// Build geometry
	const unsigned int n1 = n - 1;
	std::vector< glm::vec3 > pos;
	std::vector< glm::vec2 > tc;
	std::vector< glm::vec3 > norm;
	pos.reserve( n * n );
	tc.reserve( n * n );
	norm.reserve( n * n );
	std::vector< glm::vec3 > cpos;
	std::vector< glm::vec3 > cnorm;
	cpos.reserve( n );
	cnorm.reserve( n );
	for( unsigned int i = 0; i < n; ++i )
	{
		const float alpha = ( (1.0/n1) * i ) * 2 * glm::pi< float >();
		glm::vec3 p = glm::vec3( 0.f, glm::cos(alpha), glm::sin(alpha) );
		cnorm.push_back( p );
		cpos.push_back( p * 0.8f );
	}
	for ( unsigned int j = 0; j < n; ++j )
	{
		//let tr = translate( -1+2/n1*j, 0, 0 );
		const float t = -1 + ( 2.f / n1 ) * j;
		glm::mat4 tr = glm::translate( glm::mat4( 1.0f ), glm::vec3( t, 0.f, 0.f ) );
		//let ntr = tr.inverse3transpose();
		glm::mat4 ntr = glm::transpose( glm::inverse( tr ) );
		const float v = (1.0/n1) * j;
		for ( unsigned int i = 0; i < n; ++i )
		{
			const float u = (1.0/n1) * i;
			tc.push_back( glm::vec2( u, v ) );
			//pos.push_back( tr.transform( cpos.get_vec3( i ) ) );
			pos.push_back( glm::vec3( tr * glm::vec4( cpos[ i ], 1.f ) ) );
			//norm.push_back( ntr.mult( cnorm.get_vec3( i ) ) );
			norm.push_back( glm::vec3( ntr * glm::vec4( cnorm[ i ], 0.f ) ) );
		}
	}

	// Build topology
	std::vector< unsigned int > indices;
	indices.reserve( 6 * (n - 1)*(n - 1) );
	unsigned int last = 0;
	for ( unsigned int j = 1; j < n; ++j )
	{
		for ( unsigned int i = 1; i < n; ++i )
		{
			unsigned int k = j*n + i;

			// push quad
			indices.push_back(k);
			indices.push_back(k - n - 1);
			indices.push_back(k - n);

			indices.push_back(k - n - 1);
			indices.push_back(k);
			indices.push_back(k - 1);
		}
	}
	// Store number of indices (for rendering)
	mNbMeshIndices = indices.size();
	
	// Set graphics resources
	setGraphicsResources( pos, tc, norm, indices );
}

/******************************************************************************
 * Generate a torus mesh
 ******************************************************************************/
void PtGraphicsMeshManager::generateTorus( const unsigned int n )
{
	assert( n > 0 );

	// Release graphics resources
	finalizeGraphicsResources();

	// Build geometry
	const unsigned int n1 = n - 1;
	std::vector< glm::vec3 > pos;
	std::vector< glm::vec2 > tc;
	std::vector< glm::vec3 > norm;
	pos.reserve( n * n );
	tc.reserve( n * n );
	norm.reserve( n * n );
	std::vector< glm::vec3 > cpos;
	std::vector< glm::vec3 > cnorm;
	cpos.reserve( n );
	cnorm.reserve( n );
	for( unsigned int i = 0; i <n; ++i )
	{
		const float alpha = ( (1.0/n1) * i ) * 2.f * glm::pi< float >();
		glm::vec3 p = glm::vec3( 0, glm::sin( alpha ), glm::cos( alpha ) );
		cnorm.push_back( p );
		cpos.push_back( p * 0.4f );
	}

	for( unsigned int j = 0; j < n; ++j )
	{
		//let tr = rotate( (360/n1)*j, Vec3(0,0,1) ).mult(translate(0,0.6,0));
		//glm::mat4 tr = glm::translate( glm::rotate( glm::mat4( 1.f ), (360.f / n1) * j, glm::vec3( 0.f, 0.f, 1.f ) ), glm::vec3( 0.f, 0.6f, 0.f ) );
		const glm::mat4 t = glm::translate( glm::mat4( 1.f ), glm::vec3( 0.f, 0.6f, 0.f ) );
		const glm::mat4 r = glm::rotate( ( 1.f / n1 ) * j * 2.f * glm::pi< float >(), glm::vec3( 0.f, 0.f, 1.f ) );
		const glm::mat4 tr = r * t;
		//let ntr = tr.inverse3transpose();
		glm::mat4 ntr = glm::transpose( glm::inverse( tr ) );
		const float v = (1.0/n1) * j;
		for ( unsigned int i = 0; i < n; ++i )
		{
			const float u = (1.0/n1) * i;
			tc.push_back( glm::vec2(u,v) );
			//pos.push_back( tr.transform(cpos.get_vec3(i)) );
			pos.push_back( glm::vec3( tr * glm::vec4( cpos[ i ], 1.f ) ) );
			//norm.push_back( ntr.mult(cnorm.get_vec3(i)) );
			norm.push_back( glm::vec3( ntr * glm::vec4( cnorm[ i ], 0.f ) ) );
		}
	}

	// Build topology
	std::vector< unsigned int > indices;
	indices.reserve( 6 * (n - 1)*(n - 1) );
	unsigned int last = 0;
	for ( unsigned int j = 1; j < n; ++j )
	{
		for ( unsigned int i = 1; i < n; ++i )
		{
			unsigned int k = j*n + i;

			// push quad
			indices.push_back(k);
			indices.push_back(k - n - 1);
			indices.push_back(k - n);

			indices.push_back(k - n - 1);
			indices.push_back(k);
			indices.push_back(k - 1);
		}
	}
	// Store number of indices (for rendering)
	mNbMeshIndices = indices.size();
	
	// Set graphics resources
	setGraphicsResources( pos, tc, norm, indices );
}

/******************************************************************************
 * Generate a sphere mesh
 ******************************************************************************/
void PtGraphicsMeshManager::generateSphere( const unsigned int n )
{
	assert( n > 0 );

	// Release graphics resources
	finalizeGraphicsResources();

	// Build geometry
	const unsigned int n1 = n - 1;
	std::vector< glm::vec3 > pos;
	std::vector< glm::vec2 > tc;
	std::vector< glm::vec3 > norm;
	pos.reserve( n * n );
	tc.reserve( n * n );
	norm.reserve( n * n );
	const float a1 = glm::pi< float >() / n1;
	const float a2 = 2.f * glm::pi< float >() / n1;

	for ( unsigned int j = 0; j < n; ++j )
	{
		const float angle = -glm::half_pi< float >() + a1*j;
		const float z = glm::sin( angle );
		const float radius = glm::cos( angle );
		const float v = (1.0/n1) * j;
		for ( unsigned int i = 0; i < n; ++i )
		{
			const float u = (1.0/n1) * i;
			tc.push_back( glm::vec2(u,v) );
			const float beta = a2*i;
			glm::vec3 p = glm::vec3( radius * glm::cos(beta), radius * glm::sin(beta), z );
			pos.push_back( p );
			norm.push_back( p );
		}
	}

	// Build topology
	std::vector< unsigned int > indices;
	indices.reserve( 6 * (n - 1)*(n - 1) );
	unsigned int last = 0;
	for ( unsigned int j = 1; j < n; ++j )
	{
		for ( unsigned int i = 1; i < n; ++i )
		{
			unsigned int k = j*n + i;

			// push quad
			indices.push_back(k);
			indices.push_back(k - n - 1);
			indices.push_back(k - n);

			indices.push_back(k - n - 1);
			indices.push_back(k);
			indices.push_back(k - 1);
		}
	}
	// Store number of indices (for rendering)
	mNbMeshIndices = indices.size();
	
	// Set graphics resources
	setGraphicsResources( pos, tc, norm, indices );
}

/******************************************************************************
 * Set graphics resources
 ******************************************************************************/
void PtGraphicsMeshManager::setGraphicsResources( const std::vector< glm::vec3 >& pPositions,
							const std::vector< glm::vec2 >& pTextureCoordinates,
							const std::vector< glm::vec3 >& pNormals,
							const std::vector< unsigned int >& pIndices )
{
	const bool usePositions = ! pPositions.empty();
	const bool useTextureCoordinates = ! pTextureCoordinates.empty();
	const bool useNormals = ! pNormals.empty();
	const bool useIndices = ! pIndices.empty();

	// Create buffer objects
	if ( usePositions )
	{
		glCreateBuffers( 1, &mPositionVBO );
	}
	if ( useTextureCoordinates )
	{
		glCreateBuffers( 1, &mTextureCoordinateVBO );
	}
	if ( useNormals )
	{
		glCreateBuffers( 1, &mNormalVBO );
	}
	if ( useIndices )
	{
		glCreateBuffers( 1, &mElementArrayBuffer );
	}

	// Creates and initializes buffer objects's immutable data store
	if ( usePositions )
	{
		glNamedBufferStorage( mPositionVBO, static_cast< GLsizeiptr >( sizeof( glm::vec3 ) * pPositions.size() ), pPositions.data(), 0/*static data buffer*/ );
	}
	if ( useTextureCoordinates )
	{
		glNamedBufferStorage( mTextureCoordinateVBO, static_cast< GLsizeiptr >( sizeof( glm::vec2 ) * pTextureCoordinates.size() ), pTextureCoordinates.data(), 0/*static data buffer*/ );
	}
	if ( useNormals )
	{
		glNamedBufferStorage( mNormalVBO, static_cast< GLsizeiptr >( sizeof( glm::vec3 ) * pNormals.size() ), pNormals.data(), 0/*static data buffer*/ );
	}
	if ( useIndices )
	{
		glNamedBufferStorage( mElementArrayBuffer, static_cast< GLsizeiptr >( sizeof( unsigned int ) * pIndices.size() ), pIndices.data(), 0/*static data buffer*/ );
	}

	// Create vertex array object
	glCreateVertexArrays( 1, &mVAO );

	// Bind buffers to vertex buffer bind points
	if ( usePositions )
	{
		glVertexArrayVertexBuffer( mVAO, 0/*binding index*/, mPositionVBO, 0/*offset*/, sizeof( glm::vec3 )/*stride*/ );
	}
	if ( useTextureCoordinates )
	{
		glVertexArrayVertexBuffer( mVAO, 1/*binding index*/, mTextureCoordinateVBO, 0/*offset*/, sizeof( glm::vec2 )/*stride*/ );
	}
	if ( useNormals )
	{
		glVertexArrayVertexBuffer( mVAO, 2/*binding index*/, mNormalVBO, 0/*offset*/, sizeof( glm::vec3 )/*stride*/ );
	}
	// Configures element array buffer binding of a vertex array object
	if ( useIndices )
	{
		glVertexArrayElementBuffer( mVAO, mElementArrayBuffer );
	}

	// Enable generic vertex attribute arrays
	if ( usePositions )
	{
		glEnableVertexArrayAttrib( mVAO, 0/*index*/ );
	}
	if ( useTextureCoordinates )
	{
		glEnableVertexArrayAttrib( mVAO, 1/*index*/ );
	}
	if ( useNormals )
	{
		glEnableVertexArrayAttrib( mVAO, 2/*index*/ );
	}

	// Specify the organization of vertex arrays
	if ( usePositions )
	{
		glVertexArrayAttribFormat( mVAO, 0/*attribindex*/, 3/*size*/, GL_FLOAT, GL_FALSE/*normalized*/, 0/*relativeoffset*/ );
	}
	if ( useTextureCoordinates )
	{
		glVertexArrayAttribFormat( mVAO, 1/*attribindex*/, 2/*size*/, GL_FLOAT, GL_FALSE/*normalized*/, 0/*relativeoffset*/ );
	}
	if ( useNormals )
	{
		glVertexArrayAttribFormat( mVAO, 2/*attribindex*/, 3/*size*/, GL_FLOAT, GL_FALSE/*normalized*/, 0/*relativeoffset*/ );
	}

	// Associate a vertex attribute and a vertex buffer binding for a vertex array object
	if ( usePositions )
	{
		glVertexArrayAttribBinding( mVAO, 0/*attribindex*/, 0/*bindingindex*/ );
	}
	if ( useTextureCoordinates )
	{
		glVertexArrayAttribBinding( mVAO, 1/*attribindex*/, 1/*bindingindex*/ );
	}
	if ( useNormals )
	{
		glVertexArrayAttribBinding( mVAO, 2/*attribindex*/, 2/*bindingindex*/ );
	}
}
