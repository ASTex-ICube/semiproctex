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

namespace PtGraphics
{

inline bool PtGraphicsMeshManager::useMaterial() const
{
	return mUseMaterial;
}

inline void PtGraphicsMeshManager::setUseMaterial( const bool pFlag )
{
	mUseMaterial = pFlag;
}

inline PtGraphicsMeshManager::MeshType PtGraphicsMeshManager::getMeshType() const
{
	return mMeshType;
}

/******************************************************************************
 * Color map
 ******************************************************************************/
inline int PtGraphicsMeshManager::getColormapIndex() const
{
	return mColormapIndex;
}
inline void PtGraphicsMeshManager::setColormapIndex( const int pValue )
{
	mColormapIndex = pValue;
}

} // namespace PtGraphics
