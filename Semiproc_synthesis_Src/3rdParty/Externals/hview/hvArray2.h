// array2.h: interface for the array2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARRAY2_H__164A3508_C961_4D87_AD7A_9D43051600EA__INCLUDED_)
#define AFX_ARRAY2_H__164A3508_C961_4D87_AD7A_9D43051600EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvError.h"

#include <cassert>

namespace hview {

template <class T> class hvArray2  
{
protected:
	T			*t;
	int			sx, sy;

public:
	hvArray2() { t=0; sx=0; sy=0; }
	hvArray2(int x, int y, T nil)
	{
		t = new T[x*y];
		if (t==0) { sx=-1; sy=1; return; }
		for (int i=0; i<x*y; i++) t[i]=nil;
		sx = x;
		sy = y;
	}
	T *data() const { return t; }

	// copy
	hvArray2(const hvArray2 &a)
	{
		hvFatal("no temporary creation of hvArray2!");
	}

	// affectation
	hvArray2 &operator=(hvArray2 &a)
	{
		if (this != &a)
		{
			if (t!=0) delete [] t;
			if (a.isInvalid()) { sx=-1; sy=1; t=0; return *this; }
			sx = a.sx;
			sy = a.sy;
			t = new T [sx*sy];
			if (t==0) { sx=-1; sy=1; return *this; }
			for (int i=0; i<sx*sy; i++) t[i]=a.t[i];
		}
		return *this;
	}
	// affectation
	void clone(const hvArray2 &a)
	{
		if (t!=0) delete [] t;
		if (a.isInvalid()) { sx=-1; sy=1; t=0; return; }
		sx = a.sx;
		sy = a.sy;
		t = new T [sx*sy];
		if (t==0) { sx=-1; sy=1; return; }
		for (int i=0; i<sx*sy; i++) t[i]=a.t[i];
	}

	// isInvalid
	bool isInvalid() const
	{
		return sx==-1;
	}
	// isVoid
	bool isVoid() const
	{
		return t==0;
	}

	// operations
	void clear(T nil)
	{
		for (int i=0; i<sx*sy; i++) t[i]=nil;
	}

	// selectors
	int sizeX()const { return sx; }
	int sizeY()const { return sy; }

	int maxLevels()
	{
		int nn=sx<sy?sy:sx;
		int ll=0;
		while(nn!=1) { nn/=2; ll++; }
		return ll;
	}

	void reset(int x, int y, T nil)
	{
		//printf("in reset %d\n", t);
		if (t!=0) { delete [] t; }
		t = new T[x*y];
		//printf("allocation %d\n", t);
		if (t==0) { sx=-1; return; }
		for (int i=0; i<x*y; i++) t[i]=nil;
		sx = x;
		sy = y;
	}
	void reset(int x, int y)
	{
		if (t!=0) delete [] t;
		t = new T[x*y];
		if (t==0) { sx=-1; return; }
		sx = x;
		sy = y;
	}
	void reset()
	{
		if (t!=0) delete [] t;
		t=0; sx=0; sy=0;
	}



	T get(int x, int y) const
	{
		assert(!(x < 0 || x >= sx || y < 0 || y >= sy));
		assert(t != NULL);
		return t[x+y*sx];
	}
	void update(int x, int y, T val)
	{
		assert(!(x < 0 || x >= sx || y < 0 || y >= sy));
		assert(t != NULL);
		t[x+y*sx]=val;
	}

	~hvArray2() { if (t!=0) delete [] t; }

};


}
#endif // !defined(AFX_ARRAY2_H__164A3508_C961_4D87_AD7A_9D43051600EA__INCLUDED_)
