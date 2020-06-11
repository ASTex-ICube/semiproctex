// hvField.h: interface for the field class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELD_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
#define AFX_FIELD_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////
/*
Field<X>: is an array of dimension N=1,2,3 containing elements of type X.
X : is a scalable (implements hviAlgebra operators)
*/ 
////////////////////////////////////////////
////////////////////////////////////////////

#include <math.h>
#include "hvError.h"

namespace hview {

const unsigned int MAX_FIELD_DIMENSION=3;

template <class X> class hvField 
{
public:
	enum bordertype { CLAMP, LOOP, MIRROR };
protected:
	bordertype	loop[MAX_FIELD_DIMENSION];
	X			clampvalue;
public:
	hvField<X>() { clampvalue=X(0); }
	hvField<X>(const X &cl) { clampvalue=cl; }
	// copy
	hvField<X>(const hvField<X> &a)
	{
		hvFatal("No temporary creation of hvField!");
	}
	hvField<X> &operator=(hvField<X> &a)
	{
		hvFatal("No operator= for hvField!");
		return a;
	}

	static int index(int i, bordertype b, int s)
	{
		int id=i;
		if (b==CLAMP) return i;
		while (id<0) { if (b==MIRROR) id=-id; else id += s; }
		if (b==MIRROR && (id/s)%2==1) id = s-(id%s)-1;
		else id = id%s;
		return id;
	}

	virtual void update(int ind[], X val) =0;
	virtual void clear(X val) =0;
	virtual int	size(int dim) const =0;
	X getClampValue() const { return clampvalue; }
	bordertype getLoop(int dim) 
	{ 
		if (dim>=dimension()) hvFatal("too high dimension in hvField<X>::getLoop");
		return loop[dim]; 
	}

	virtual void setLoop(int dim, bordertype v)
	{
		if (dim>=dimension()) hvFatal("too high dimension in hvField<X>::setLoop");
		loop[dim]=v; 
	}
	virtual void setClampValue(X v) { clampvalue=v; }
	
	virtual int dimension() const = 0;
	virtual X operator()(int ind[]) const = 0;
	virtual X get(int ind[]) const = 0;

	virtual void scale(int k) =0;
	virtual void scale(float k) =0;
	virtual void scale(double k) =0;

	virtual void add(hvField<X> *f) =0;

	virtual void clone(hvField<X> *f) =0;
	virtual void shrink(hvField<X> *f) =0;

	virtual void derivative(hvField<X> *f, int dim) =0;

};



}

#endif // !defined(AFX_SCAL_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
