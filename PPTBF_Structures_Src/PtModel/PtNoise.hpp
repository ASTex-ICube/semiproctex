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

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

namespace Pt
{

inline const std::vector< int >& PtNoise::getP()
{
	return mP;
}

inline const std::vector< float >& PtNoise::getRndTab()
{
	return mRndTab;
}

inline const std::vector< float >& PtNoise::getG()
{
	return mG;
}

} // namespace Pt
