/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hviTransform.h: interface (abstract object) for a Transformation.
//
// JMD 1/01/2007
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFORM_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
#define AFX_TRANSFORM_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////
/*
A Transformation T consists of a conversion of an object U to another object of the same data type U
The interface implements following issues: 
the application of the transform to an object U
and the composition of two transforms
Neutral transform is called Identity.
*/ 
////////////////////////////////////////////
////////////////////////////////////////////

namespace hview {

template <class T, class U> class hviTransform  
{
public:
	// apply a transform to an object of type T, returns another object of type T
	virtual U apply(const U  &a) const =0;

	// compose two transforms
	virtual void compose(const T &a, const T &b) =0;

	// set identity
	virtual void setIdentity() =0;
};

}
#endif // !defined(AFX_ALGEBRA_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
