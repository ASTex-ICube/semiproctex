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

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// System
#include <cassert>

/******************************************************************************
 ****************************** INLINE DEFINITION *****************************
 ******************************************************************************/

namespace PtGraphics
{

/******************************************************************************
 * Use program
 ******************************************************************************/
inline void PtShaderProgram::use() const
{
	assert( _linked );

	if ( _linked )
	{
		glUseProgram( _program );
	}
}

/******************************************************************************
 * Unuse program
 ******************************************************************************/
inline void PtShaderProgram::unuse()
{
	glUseProgram( 0 );
}

///******************************************************************************
// * ...
// ******************************************************************************/
//template< typename TType >
//inline void PtShaderProgram::set< TType >( const TType& pValue, const char* pName )
//{
//}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::mat4 >( const glm::mat4& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniformMatrix4fv( mUniformLocations[ pName ], 1, GL_FALSE, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::mat3 >( const glm::mat3& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniformMatrix3fv( mUniformLocations[ pName ], 1, GL_FALSE, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::mat2 >( const glm::mat2& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniformMatrix2fv( mUniformLocations[ pName ], 1, GL_FALSE, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::vec4 >( const glm::vec4& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform4fv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::ivec4 >( const glm::ivec4& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform4iv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::uvec4 >( const glm::uvec4& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform4uiv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::vec3 >( const glm::vec3& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform3fv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::ivec3 >( const glm::ivec3& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform3iv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::uvec3 >( const glm::uvec3& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform3uiv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::vec2 >( const glm::vec2& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform2fv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::ivec2 >( const glm::ivec2& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform2iv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< glm::uvec2 >( const glm::uvec2& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform2uiv( mUniformLocations[ pName ], 1, glm::value_ptr( pValue ) );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< float >( const float& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform1f( mUniformLocations[ pName ], pValue );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< int >( const int& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform1i( mUniformLocations[ pName ], pValue );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< unsigned int >( const unsigned int& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform1ui( mUniformLocations[ pName ], pValue );
	}
}

/******************************************************************************
 * ...
 ******************************************************************************/
template<>
inline void PtShaderProgram::set< bool >( const bool& pValue, const char* pName )
{
	if ( mUniformLocations[ pName ] >= 0 )
	{
		glUniform1i( mUniformLocations[ pName ], pValue );
	}
}

/******************************************************************************
 * Shader  (name, etc...)
 ******************************************************************************/
inline const char* PtShaderProgram::getName() const
{
	return mName.c_str();
}
	
/******************************************************************************
 * Shader info (name, etc...)
 ******************************************************************************/
inline void PtShaderProgram::setName( const char* pText )
{
	mName = pText;
}

/******************************************************************************
 * Shader info (name, etc...)
 ******************************************************************************/
inline const char* PtShaderProgram::getInfo() const
{
	return mInfo.c_str();
}
	
/******************************************************************************
 * Shader info (name, etc...)
 ******************************************************************************/
inline void PtShaderProgram::setInfo( const char* pText )
{
	mInfo = pText;
}

} // namespace PtGraphics
