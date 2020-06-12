/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hvVec.h: interface for the vector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__9621FBEC_AB7C_4F9F_B7E3_56541A01CC32__INCLUDED_)
#define AFX_VECTOR_H__9621FBEC_AB7C_4F9F_B7E3_56541A01CC32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "hvError.h"

namespace hview {

template <class T, int size> class hvVec 
{
protected:
	T		v[size];

public:
	hvVec<T,size>(T nil) { for (int i=0; i<size; i++) v[i]=nil; } 
	hvVec<T,size>() { for (int i=0; i<size; i++) v[i]=(T)0; } 
	T &operator[](int i)
	{
		if (i<0 || i>=size) { hvFatal("access out of range in vector!"); }
		return v[i];
	}
	// copy 
	hvVec<T,size>(const hvVec<T,size> &a) 
	{ 
		for (int i=0; i<size; i++) v[i]=a.v[i];
		//printf("warning: temporary creation of vec<T,int>!\n"); 
	}
	T *data() { return v; }
	hvVec<T,size> &operator=(const hvVec<T,size> &a)
	{
		if (this != &a)
		{
			for (int i=0; i<size; i++) v[i]=a.v[i];
		}
		return *this;
	}

//	int size() { return size; }
	void clear(T x=T(0)) { for (int i=0; i<size; i++) v[i]=x; }

	///////  operations
	void add(const hvVec<T,size> &a, const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i]=a.v[i]+b.v[i]; }
	}

	hvVec<T,size> operator+(const hvVec<T,size> &a) const
	{ 
		hvVec<T,size> r;
		for (int i=0; i<size; i++) { r.v[i]= v[i]+a.v[i]; }
		return r;
	}
	void operator+=(const hvVec<T,size> &a) 
	{ 
		for (int i=0; i<size; i++) { v[i]+= a.v[i]; }
	}

	void sub(const hvVec<T,size> &a, const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i]=a.v[i]-b.v[i]; }
	}
	hvVec<T,size> operator-(const hvVec<T,size> &b) const
	{ 
		hvVec<T,size> r;
		for (int i=0; i<size; i++) { r.v[i]= v[i]-b.v[i]; }
		return r;
	}
	void operator-=(const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i]-= b.v[i]; }
	}

	void mult(const hvVec<T,size> &a, const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i]=a.v[i]*b.v[i]; }
	}
	hvVec<T,size> operator*(const hvVec<T,size> &b) const
	{ 
		hvVec<T,size> r;
		for (int i=0; i<size; i++) { r.v[i]= v[i]*b.v[i]; }
		return r;
	}
	void operator*=(const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i]*= b.v[i]; }
	}
	void operator*=(T x)
	{
		for (int i=0; i<size; i++) { v[i]*= x; }
	}
	template <class X> void operator*=(X x)
	{
		for (int i=0; i<size; i++) { v[i]= (T)((X)v[i]*x); }
	}


	void div(const hvVec<T,size> &a, const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i]=a.v[i]/b.v[i]; }
	}
	hvVec<T,size> operator/(const hvVec<T,size> &b) const
	{ 
		hvVec<T,size> r;
		for (int i=0; i<size; i++) { r.v[i]= v[i]/b.v[i]; }
		return r;
	}
	void operator/=(const hvVec<T,size> &b) 
	{ 
		for (int i=0; i<size; i++) { v[i] /= b.v[i]; }
	}
	void operator/=(T x) 
	{ 
		for (int i=0; i<size; i++) { v[i]/= x; }
	}
	template <class X> void operator/=(X x) 
	{ 
		for (int i=0; i<size; i++) { v[i]= (T)((X)v[i]/x); }
	}

	// scale by scalar value
	void scale(const hvVec<T,size> &a, T k) { for (int i=0; i<size; i++) v[i]=a.v[i]*k; }
	void scale(T k) { for (int i=0; i<size; i++) v[i]*=k; }
	template <class X> void scale(X k) { for (int i=0; i<size; i++) v[i]= (T)((X)v[i]*k); }
	// divide by scalar, this scalar cannot be null
	void normalize(T norme) { for (int i=0; i<size; i++) v[i]/=norme; }
	void divScale(T k) { for (int i=0; i<size; i++) v[i]/=k; }
	template <class X> void divScale(X k) { for (int i=0; i<size; i++) v[i]= (T)((X)v[i]/k); }

	// absolute value component by component
	void abs()
		{
		for (int i=0; i<size; i++) if (v[i]<(T)0) v[i] = -v[i];
		}

	// dot product (produit scalaire)
	T dot(const hvVec<T,size> &a) const { T r=0; for (int i=0; i<size; i++) r+=(v[i]*a.v[i]); return r; }

	// opposite vector
	void reverse() { for (int i=0; i<size; i++) v[i]=-v[i]; }
	hvVec<T,size> operator-() { hvVec<T,size> r=*this; r.reverse(); return r; }


	/* linear interpolation between v1 and v2 
	according to t (between 0 and 1),
	if t=0 result is v1, if t=1 result is v2
	result is not normalized */
	template <class X> void interpolate(const hvVec<T,size> &v1, const hvVec<T,size> &v2, X t)
		{
		for (int i=0; i<size; i++) { v[i] = (T)(((X)1.0-t)*(X)v1.v[i] + t*(X)v2.v[i]); }
		}

	/* linear interpolation between v1 and v2 
	according to vector t; each component of t is between 0 and 1 and gives coefficient,
	result is not normalized */
	template <class X> void interpolate(const hvVec<T,size> &v1, const hvVec<T,size> &v2, const hvVec<X,size> &t)
		{
		for (int i=0; i<size; i++) { v[i] = (T)(((X)1.0-t.v[i])*(X)v1.v[i] + t.v[i]*(X)v2.v[i]); }
		}


	/* compares component by component and keeps the min one 
	result is not normalized */
	void keepMin(const hvVec<T,size> &v1, const hvVec<T,size> &v2)
		{
		for (int i=0; i<size; i++) if (v1.v[i]<v2.v[i]) v[i] = v1.v[i]; else v[i] = v2.v[i];
		}

	/* compares component by component and keeps the max one 
	result is not normalized */
	void keepMax(const hvVec<T,size> &v1, const hvVec<T,size> &v2)
		{
		for (int i=0; i<size; i++) if (v1.v[i]>v2.v[i]) v[i] = v1.v[i]; else v[i] = v2.v[i];
		}

	
	/* returns the largest / smallest component */
	T maxCoord() const
		{
		T m=v[0];

		for (int i=0; i<size; i++) if (v[i]>m) m=v[i];
		return m;
		}
	T minCoord() const
		{
		T m=v[0];

		for (int i=0; i<size; i++) if (v[i]<m) m=v[i];
		return m;
		}

	/* computes the norm (vector length) */
	T normSquared() const { T m=0; for (int i=0; i<size; i++) m+=v[i]*v[i]; return m; }
	double normSquaredDouble() const { double m=0; for (int i=0; i<size; i++) m+=(double)v[i]*(double)v[i]; return m; }
	T norm() const { return (T)sqrt(normSquaredDouble()); }

	bool equals(const hvVec<T,size> &tt)
	{
		for (int i=0; i<size; i++) if (v[i]!=tt.v[i]) return false;
		return true;
	}
	bool operator==(const hvVec<T,size> &tt)
	{
		for (int i=0; i<size; i++) if (v[i]!=tt.v[i]) return false;
		return true;
	}
};



}
#endif // !defined(AFX_VECTOR_H__9621FBEC_AB7C_4F9F_B7E3_56541A01CC32__INCLUDED_)
