// hvArray1.h: interface for the array1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARRAY1_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
#define AFX_ARRAY1_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "hvError.h"
#include "hvPair.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

namespace  hview {

template <class T> class hvArray1  
{
protected:
	T			*t;
	int			sx;

public:
	hvArray1() { t=0; sx=0; }
	hvArray1(int x, T nil)
	{
		t = new T[x];
		if (t==0) { sx=-1; return; }
		for (int i=0; i<x; i++) t[i]=nil;
		sx = x;
	}
	T *data() const { return t; }

	// copy
	hvArray1(const hvArray1 &a)
	{
		hvFatal("No temporary creation of hvArray1!");
	}

	// affectation
	hvArray1 &operator=(const hvArray1 &a)
	{
		if (this != &a)
		{
			if (t!=0) delete [] t;
			if (a.isInvalid()) { t=0; sx=-1; return *this; }
			sx = a.sx;
			t = new T [sx];
			if (t==0) { sx=-1; return *this; }
			for (int i=0; i<sx; i++) t[i]=a.t[i];
		}
		return *this;
	}

	void reset(int x, T nil)
	{
		if (t!=0) delete [] t;
		t = new T[x];
		if (t==0) { sx=-1; return; }
		for (int i=0; i<x; i++) t[i]=nil;
		sx = x;
	}
	void reset(int x)
	{
		if (t!=0) delete [] t;
		t = new T[x];
		if (t==0) { sx=-1; return; }
		sx = x;
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
	void clear(T nil)
	{
		for (int i=0; i<sx; i++) t[i]=nil;
	}

	// selectors
	int size() const { return sx; }

	T &operator[](int x)
	{
		if(x<0 || x>=sx) { hvFatal("out of range!"); }
		if (t==0) { hvFatal("hvArray1 is void!"); }
		return t[x];
	}
	T get(int x) const
	{
		assert(!(x < 0 || x >= sx));
		assert(t != NULL);
		return t[x];
	}
	T *getPointer(int x)
	{
		if(x<0 || x>=sx) { hvFatal("out of range!"); }
		if (t==0) { hvFatal("hvArray1 is void!"); }
		return t+x;
	}
	void update(int x, T val)
	{
		assert(!(x < 0 || x >= sx));
		assert(t != NULL);
		t[x]=val;
	}
	template <class U> void updateRange(int start, int end, U *val)
	{
		assert(!(start < 0 || start >= sx));
		assert(!(end < 0 || end >= sx));
		assert(t != NULL);
		for (int i=start; i<=end; i++) t[i]=T(val[i-start]);
	}

	~hvArray1() { if (t!=0) delete [] t;}

	// FFT algorithm 
	// F is pointer to pairs (integer, imaginary) part of complex numbers of type T
	// N is such that 2^N is the size of the array F
	static void fft(hvArray1<hvPair<T,T> > &F, int N, int s, int id, bool divn)
	{
	int n,i,j,k,l,nv2,ip;
	hvPair<T,T> u,w,tmp;

	n = 1<<N; /* n=2^N */ nv2=n>>1;	/* nv2=2^(N-1) */
	j=1;
	for (i=1; i<=n-1; i++)
		{
		if (i<j)
			{
			tmp=F[(j-1)*s+id];
			F[(j-1)*s+id]=F[(i-1)*s+id];
			F[(i-1)*s+id]=tmp;
			}
		k=nv2; while(k<j) { j=j-k; k=k>>1; }
		j=j+k;
		}
	for (l=1; l<=N; l++)
		{
		int le = 1<<l;
		int le1 = le>>1;
		u=hvPair<T,T>(T(1.0),T(0.0));
		double a = M_PI/(double)le1;
		w=hvPair<T,T>(T(cos(a)),T(-sin(a)));
		for (j=1; j<=le1; j++)
			{
			for (i=j; i<=n; i+=le)
				{
				ip=i+le1;
				tmp = hvPair<T,T>(F[((ip-1)*s+id)].getLeft()*u.getLeft()-F[((ip-1)*s+id)].getRight()*u.getRight(),F[((ip-1)*s+id)].getLeft()*u.getRight()+F[((ip-1)*s+id)].getRight()*u.getLeft());
				F[((ip-1)*s+id)]= hvPair<T,T>(F[((i-1)*s+id)].getLeft()-tmp.getLeft(), F[((i-1)*s+id)].getRight()-tmp.getRight());
				F[((i-1)*s+id)]= hvPair<T,T>(F[((i-1)*s+id)].getLeft()+tmp.getLeft(), F[((i-1)*s+id)].getRight()+tmp.getRight());
				}
			u = hvPair<T,T>(u.getLeft()*w.getLeft()-u.getRight()*w.getRight(), u.getLeft()*w.getRight()+u.getRight()*w.getLeft());
			}
		}
	if (divn)
		{
		double ff = 1.0/(double)n;
		for (i=1;i<=n; i++) F[((i-1)*s+id)]= hvPair<T,T>(F[((i-1)*s+id)].getLeft()*ff, F[((i-1)*s+id)].getRight()*ff);
		}	
	}
};

}
#endif // !defined(AFX_ARRAY1_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
