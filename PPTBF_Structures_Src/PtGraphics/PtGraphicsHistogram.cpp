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

#include "PtGraphicsHistogram.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// System
#include <cassert>

// STL
#include <iostream>
#include <numeric>
#include <algorithm>

// Project
#include "PtPPTBF.h"
#include "PtNoise.h"
#include <PtEnvironment.h>

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Pt;
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
PtGraphicsHistogram::PtGraphicsHistogram()
:	mShaderProgram( nullptr )
,	mNbBins( 0 )
,	mPPTBFFrameBuffer( 0 )
,	mPPTBFTexture( 0 )
{
	mShaderProgram = new PtShaderProgram();
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtGraphicsHistogram::~PtGraphicsHistogram()
{
	// Reset graphics resources
	finalize();

	delete mShaderProgram;
	mShaderProgram = nullptr;
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void PtGraphicsHistogram::initialize( const int pNbBins )
{
	mNbBins = pNbBins;

	// Initialize framebuffer
	initializeFramebuffer();

	// Initialize shader program
	initializeShaderProgram();
}

/******************************************************************************
 * Initialize framebuffer
 ******************************************************************************/
void PtGraphicsHistogram::initializeFramebuffer()
{
	// Initialize texture "rnd_tab"
	glCreateTextures( GL_TEXTURE_2D, 1, &mPPTBFTexture );
	// - set the texture wrapping/filtering options (on the currently bound texture object)
	glTextureParameteri( mPPTBFTexture, GL_TEXTURE_WRAP_S, /*GL_REPEAT*/GL_CLAMP_TO_EDGE );
	glTextureParameteri( mPPTBFTexture, GL_TEXTURE_WRAP_T, /*GL_REPEAT*/GL_CLAMP_TO_EDGE );
	glTextureParameteri( mPPTBFTexture, GL_TEXTURE_MIN_FILTER, /*GL_LINEAR*/GL_NEAREST );
	glTextureParameteri( mPPTBFTexture, GL_TEXTURE_MAG_FILTER, /*GL_LINEAR*/GL_NEAREST );
	// - set min/max level for completeness
	glTextureParameteri( mPPTBFTexture, GL_TEXTURE_BASE_LEVEL, 0 );
	glTextureParameteri( mPPTBFTexture, GL_TEXTURE_MAX_LEVEL, 0 );
	// - generate the texture
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, mPPTBFTexture );
	glTexImage2D( GL_TEXTURE_2D, 0/*level*/, GL_R32F, mNbBins/*width*/, 1/*height*/, 0/*border*/, GL_RED, GL_FLOAT, nullptr );
	//glTextureStorage2D( mPPTBFTexture, 1/*levels*/, GL_R32F, mNbBins/*width*/, 1/*height*/ );
	//glTextureSubImage2D( mPPTBFTexture, 0/*level*/, 0/*xoffset*/, 0/*yoffset*/, mNbBins/*width*/, 1/*height*/, GL_RED, GL_FLOAT, nullptr );
	// - reset device state
	//glBindTexture( GL_TEXTURE_2D, 0 );

	// Configure framebuffer
	//glCreateFramebuffers( 1, &mPPTBFFrameBuffer );
	//glNamedFramebufferTexture( mPPTBFFrameBuffer, GL_COLOR_ATTACHMENT0, mPPTBFTexture, 0/*level*/ );
	//glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	//glNamedFramebufferDrawBuffer( mPPTBFFrameBuffer, GL_COLOR_ATTACHMENT0 );

	glGenFramebuffers( 1, &mPPTBFFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, mPPTBFFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPPTBFTexture, 0 );
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	// - check FBO status
	//GLenum fboStatus = glCheckNamedFramebufferStatus( mPPTBFFrameBuffer, GL_FRAMEBUFFER );
	if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		std::cout << "Framebuffer error - status: " << fboStatus << std::endl;
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

		// - clean device resources
		finalize();
	//	// - reset device state
	//	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		//return -1;
	}
	// - reset device state
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glBindTexture( GL_TEXTURE_2D, 0 );
}

/******************************************************************************
 * Initialize shader program
 ******************************************************************************/
bool PtGraphicsHistogram::initializeShaderProgram()
{
	bool statusOK = false;

	// Global variables
	const std::string shaderPath = PtEnvironment::mShaderPath + std::string( "/" );
	PtShaderProgram* shaderProgram = nullptr;
	PtShaderProgram::TShaderList shaders;
	std::string shaderFilename;
	std::vector< std::string > uniforms;

	// [1] - Initialize shader program
	shaderProgram = mShaderProgram;
	shaderProgram->finalize();
	shaderProgram->initialize();
	shaderProgram->setName( "histogram" );
	shaderProgram->setInfo( "Histogram" );
	// - path
	shaders.clear();
	shaderFilename = shaderPath + "histogram_vert.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eVertexShader, shaderFilename ) );
	shaderFilename = shaderPath + "histogram_frag.glsl";
	shaders.push_back( std::make_pair( PtShaderProgram::ShaderType::eFragmentShader, shaderFilename ) );
	statusOK = shaderProgram->initializeProgram( shaders );
	// Store uniforms info (after shader program link() step)
	uniforms.clear();
	uniforms.push_back( "uTexture" );
	uniforms.push_back( "uTextureWidth" );
//	uniforms.push_back( "uNbBins" );
	shaderProgram->registerUniforms( uniforms );
	
	// Set uniform(s) that will never be modified (if no other shader link!)
	shaderProgram->use();
	shaderProgram->set( 0, "uTexture" );
	PtShaderProgram::unuse();
	
	return statusOK;
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void PtGraphicsHistogram::finalize()
{
	if ( mPPTBFFrameBuffer )
	{
		glDeleteFramebuffers( 1, &mPPTBFFrameBuffer );
		mPPTBFFrameBuffer = 0;
	}

	if ( mPPTBFTexture )
	{
		glDeleteTextures( 1, &mPPTBFTexture );
		mPPTBFTexture = 0;
	}

	mShaderProgram->finalize();
}

/******************************************************************************
 * Get number of bins
 ******************************************************************************/
int PtGraphicsHistogram::getNbBins() const
{
	return mNbBins;
}

/******************************************************************************
 * Set number of bins
 ******************************************************************************/
void PtGraphicsHistogram::setNbBins( const int pValue )
{
	mNbBins = pValue;

	// Initialize framebuffer
	if ( mPPTBFFrameBuffer )
	{
		glDeleteFramebuffers( 1, &mPPTBFFrameBuffer );
		mPPTBFFrameBuffer = 0;
	}

	if ( mPPTBFTexture )
	{
		glDeleteTextures( 1, &mPPTBFTexture );
		mPPTBFTexture = 0;
	}

	initializeFramebuffer();
}

/******************************************************************************
 * Compute histogram on device (i.e. GPU)
 ******************************************************************************/
void PtGraphicsHistogram::compute( const GLuint pTexture, const int pTextureWidth, const int pTextureHeight )
{

	printf( "\nhistogram: %d x %d", pTextureWidth, pTextureHeight );


	// Backup GL state
	GLint last_viewport[ 4 ];
	glGetIntegerv( GL_VIEWPORT, last_viewport );

	
	
	// Set and clear dedicated framebuffer
	//glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mPPTBFFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, mPPTBFFrameBuffer ); // read-write

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPPTBFTexture);

	//glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	//glDrawBuffers(GL_COLOR_ATTACHMENT0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	//glReadBuffer( GL_COLOR_ATTACHMENT0 );
	
	glClearColor( 0.f, 0.f, 0.f, 0.f );
	glClear( GL_COLOR_BUFFER_BIT );
	
	// Set viewport according to histogram size (1D + 1 fragment per bin)
	glViewport( 0, 0, mNbBins, 1 );
	
	// Set GL state to manage histograms
	// - sum fragments value per bin
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );
	glBlendEquation( GL_FUNC_ADD );
	glDisable( GL_DEPTH_TEST );
	
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPPTBFTexture);*/
	
	// Set texture(s)
	glBindTextureUnit( 0/*unit*/, pTexture );
	
	// Set shader
	mShaderProgram->use();

	// Set uniform(s)
	//
	mShaderProgram->set( 0, "uTexture" );
	//
	mShaderProgram->set( pTextureWidth, "uTextureWidth" );
	//mShaderProgram->set(mNbBins, "uNbBins");
	

	//////------------------------------------------------------------------------------------
	//std::vector< float > pHistogram;
	//pHistogram.resize(mNbBins, 0.f);
	//glReadPixels(0, 0, mNbBins/*width*/, 1/*height*/, GL_RED, GL_FLOAT, pHistogram.data());
	//std::cout << "----------------------------------" << std::endl;
	//std::cout << "- HISTOGRAM" << std::endl;
	//for (const auto v : pHistogram)
	//{
	//	//std::cout << ( v / static_cast< float >( nbPixels ) ) << " ";
	//	std::cout << v << " ";
	//}
	//std::cout << std::endl;
	//////------------------------------------------------------------------------------------

	

		// Draw command(s)
	// - send points (as much image pixels)
	const int nbPixels = pTextureWidth * pTextureHeight;
	printf("\nnb pixels: %d", nbPixels );
	glDrawArrays( GL_POINTS, 0, nbPixels );
	
	// Reset shader
	PtShaderProgram::unuse();

	// Restore modified GL state
	//glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glViewport( last_viewport[ 0 ], last_viewport[ 1 ], static_cast< GLsizei >( last_viewport[ 2 ] ), static_cast< GLsizei >( last_viewport[ 3 ] ) );
	glClearColor( 0.45f, 0.55f, 0.60f, 1.00f );



	//////------------------------------------------------------------------------------------
	//std::vector< float > shaderData(mNbBins);
	//glGetTextureImage(mPPTBFTexture, 0/*level*/, GL_RED, GL_FLOAT, sizeof(float) * mNbBins, shaderData.data());
	//auto result = std::minmax_element(shaderData.begin(), shaderData.end());
	//std::cout << "\nhistogram: " << "(" << *(result.first) << "," << *(result.second) << ")" << std::endl;
	////getchar();
	////------------------------------------------------------------------------------------

	////------------------------------------------------------------------------------------
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, mPPTBFFrameBuffer);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//// Retrieve histogram
	//pHistogram.resize(mNbBins, 0.f);
	//glReadPixels(0, 0, mNbBins/*width*/, 1/*height*/, GL_RED, GL_FLOAT, pHistogram.data());
	//// Reset GL state(s)
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	////------------------------------------------------------------------------------------
}

/******************************************************************************
* Render
******************************************************************************/
void PtGraphicsHistogram::render()
{
}

/******************************************************************************
 * Retrieve histogram on host (i.e. CPU)
 ******************************************************************************/
void PtGraphicsHistogram::getHistogram( std::vector< float >& pHistogram, std::vector< float >& pCDF, const int pTextureWidth, const int pTextureHeight )
{
	// Configure GL state(s)
	//glNamedFramebufferReadBuffer( mPPTBFFrameBuffer, GL_COLOR_ATTACHMENT0 );
	//glBindFramebuffer( GL_READ_FRAMEBUFFER, mPPTBFFrameBuffer );
	glBindFramebuffer(GL_FRAMEBUFFER, mPPTBFFrameBuffer);
	glReadBuffer( GL_COLOR_ATTACHMENT0 );
	
	//// - deal with odd texture dimensions
	//glBindTexture( GL_TEXTURE_2D, mPPTBFTexture );
	//glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	//glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	//glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );
	//glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	
	// Retrieve histogram
	pHistogram.resize( mNbBins, 0.f );
	//pHistogram.resize( 4 * mNbBins, 0.f );
	glReadPixels( 0, 0, mNbBins/*width*/, 1/*height*/, GL_RED, GL_FLOAT, pHistogram.data() );
	//glReadPixels( 0, 0, mNbBins/*width*/, 1/*height*/, GL_RGBA, GL_FLOAT, pHistogram.data() );
	
	// Reset GL state(s)

	//glReadBuffer( GL_BACK ); // GL_FRONT

	//glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// Build CDF (cumulative distribution function)
	pCDF.resize( mNbBins, 0.f );
	//pCDF.resize( 4 * mNbBins, 0.f );
	std::partial_sum( pHistogram.begin(), pHistogram.begin() + mNbBins, pCDF.begin() );

	// DEBUG

#if _DEBUG
	const int nbPixels = pTextureWidth * pTextureHeight;
	printf("\nnb pixels: %d", nbPixels);

	std::cout << "----------------------------------"<< std::endl;
	std::cout << "- HISTOGRAM" << std::endl;
	for ( const auto v : pHistogram )
	{
		//std::cout << ( v / static_cast< float >( nbPixels ) ) << " ";
		std::cout << v << " ";
	}
	std::cout << std::endl;

	std::cout << "- CDF" << std::endl;
	for ( const auto v : pCDF )
	{
		std::cout << ( v / static_cast< float >( nbPixels ) ) << " ";
		//std::cout << v << " ";
	}
	std::cout << std::endl;
#endif
}
