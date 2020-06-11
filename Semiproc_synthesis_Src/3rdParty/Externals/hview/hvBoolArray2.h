// hvBoolArray2.h: interface for the array2 of boolean class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOOLARRAY2_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
#define AFX_BOOLARRAY2_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvError.h"
#include <assert.h>

namespace  hview {

class hvBoolArray2  
{
protected:
	unsigned char	*t;
	int				sx, sy;

public:
	hvBoolArray2() { t=0; sx=0; sy=0; }
	hvBoolArray2(int x, int y, bool nil)
	{
		t = new unsigned char [(x/8+1)*y];
		if (t==0) { sx=-1; sy=-1; return; }
		for (int i=0; i<(x/8+1)*y; i++) t[i]=(nil==true?255:0);
		sx = x; sy = y;
	}

	// copy
	hvBoolArray2(const hvBoolArray2 &a)
	{
		hvFatal("No temporary creation of hvBoolArray2!");
	}

	// affectation
	hvBoolArray2 &operator=(const hvBoolArray2 &a)
	{
		if (this != &a)
		{
			if (t!=0) delete [] t;
			sx = a.sx;
			sy = a.sy;
			t = new unsigned char [(sx/8+1)*sy];
			if (t==0) { sx=-1; sy=-1; return *this; }
			for (int i=0; i<(sx/8+1)*sy; i++) t[i]=a.t[i];
		}
		return *this;
	}

	void reset(int x, int y, bool nil)
	{
		if (t!=0) delete [] t;
		t = new unsigned char [(x/8+1)*y];
		if (t==0) { sx=-1; sy=-1; return; }
		for (int i=0; i<(x/8+1)*y; i++) t[i]=(nil==true?255:0);
		sx = x; sy = y;
	}
	void reset(bool nil)
	{
		if (t == 0)
		{
			sx = -1;
			return;
		}
		for (int i = 0; i<(sx / 8 + 1)*sy; i++)
			t[i] = (nil == true ? 255 : 0);
	}
	void reset()
	{
		if (t!=0) delete [] t;
		t=0;
		sx=0;
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
	void clear(bool nil)
	{
		for (int i=0; i<(sx/8+1)*sy; i++) t[i]=(nil==true?255:0);
	}

	// selectors
	int sizeX() const { return sx; }
	int sizeY() const { return sy; }

	bool get(int x, int y) const
	{
		if(x<0 || x>=sx) { hvFatal("out of sx range!"); }
		if(y<0 || y>=sy) { hvFatal("out of sy range!"); }
		if (t==0) { hvFatal("hvArray1 is void!"); }
		unsigned char p=(unsigned char)(((unsigned int)x)&7);
		unsigned char mask = ((unsigned char)1)<<p;
		if (t[(((unsigned int)x)>>3)+y*(sx/8+1)] & mask ) return true; 
		return false;
	}

	void set(int x, int y, bool v)
	{
		if(x<0 || x>=sx) { hvFatal("out of sx range!"); }
		if(y<0 || y>=sy) { hvFatal("out of sy range!"); }
		if (t==0) { hvFatal("hvArray1 is void!"); }
		unsigned char p=(unsigned char)(((unsigned int)x)&7);
		unsigned char mask = ((unsigned char)1)<<p;
		if (v) t[(((unsigned int)x)>>3)+y*(sx/8+1)] = t[(((unsigned int)x)>>3)+y*(sx/8+1)] | mask; 
		else t[(((unsigned int)x)>>3)+y*(sx/8+1)] = t[(((unsigned int)x)>>3)+y*(sx/8+1)] & (~mask);
	}

	// standard boolean orperators
	void operator|=(const hvBoolArray2 &x)
	{
		if (x.sizeX()!=sizeX()) hvFatal("not same sizeX in hvBoolArray1::operator|");
		if (x.sizeY()!=sizeY()) hvFatal("not same sizeY in hvBoolArray1::operator|");
		for (int i=0; i<(sx/8+1)*sy; i++) t[i] |= x.t[i];
	}
	void operator&=(const hvBoolArray2 &x)
	{
		assert(x.sizeX()==sizeX());
		assert(x.sizeY()==sizeY());
		for (int i=0; i<(sx/8+1)*sy; i++) t[i] &= x.t[i];
	}
	void operator~()
	{
		for (int i=0; i<(sx/8+1)*sy; i++) t[i] = ~t[i];
	}
	void operator^=(const hvBoolArray2 &x)
	{
		if (x.sizeX()!=sizeX()) hvFatal("not same sizeX in hvBoolArray1::operator|");
		if (x.sizeY()!=sizeY()) hvFatal("not same sizeY in hvBoolArray1::operator|");
		for (int i=0; i<(sx/8+1)*sy; i++) t[i] ^= x.t[i];
	}

	~hvBoolArray2() { if (t!=0) delete [] t;}
};

}
#endif // !defined(AFX_ARRAY1_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
