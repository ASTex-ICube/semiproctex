/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hvVec2.h: interface for the vec2 class.
//
// defines a vector in 2D
// By JMD 8/8/04
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VEC2_H__954C4849_5D4B_4173_9842_797C84C6FB1B__INCLUDED_)
#define AFX_VEC2_H__954C4849_5D4B_4173_9842_797C84C6FB1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "hvError.h"

namespace hview {


template <class T> class hvVec2 
{
protected:
	T		x, y;
public:
	//////// constructors
	hvVec2<T>():x(T(0)),y(T(0)) {  }
	hvVec2<T>(T a):x(a),y(a) {  }
	hvVec2<T>(T a, T b):x(a),y(b) {  }

	template <class X> void cast(const hvVec2<X> &v) { x=T(v.X()); y=T(v.Y());  }

	// define vector from two points (a,b):  v=ab
	void PVec(const hvVec2<T> &a, const hvVec2<T> &b) { x=b.x-a.x; y=b.y-a.y; }

	//////// selectors for coordinates
	T X() const { return x; }
	T Y() const { return y; }
	
	T &operator[](int i)
	{
		if (i!=0 && i!=1) { hvFatal("out of vec2 range!"); }
		if (i==0) return x;
		return y;
	}

	///////  operations: addition, substraction, multiplication and division
	void add(const hvVec2<T> &a, const hvVec2<T> &b) { x=a.x+b.x; y=a.y+b.y; }
	hvVec2 operator+(const hvVec2<T> &b) const { hvVec2<T> vv; vv.x=x+b.x; vv.y=y+b.y; return vv;}
	void operator+=(const hvVec2<T> &b) { x+=b.x; y+=b.y; }

	void sub(const hvVec2<T> &a, const hvVec2<T> &b) { x=a.x-b.x; y=a.y-b.y; }
	hvVec2 operator-(const hvVec2<T> &b) const { hvVec2<T> vv; vv.x=x-b.x; vv.y=y-b.y; return vv;}
	void operator-=(const hvVec2<T> &b) { x-=b.x; y-=b.y; }

	void mult(const hvVec2<T> &a, const hvVec2<T> &b) { x=b.x*a.x; y=b.y*a.y; }
	hvVec2<T> operator*(const hvVec2<T> &b) const { hvVec2<T> vv; vv.x=x*b.x; vv.y=y*b.y; return vv;}
	void operator*=(const hvVec2<T> &b) { x*=b.x; y*=b.y; }
	void operator*=(T v) { x*=v; y*=v; }
	template <class X> void operator*=(X v) { x= (T)((X)x*v); y=(T)((X)y*v); }

	void div(const hvVec2<T> &a, const hvVec2<T> &b) { x=a.x/b.x; y=a.y/b.y; }
	hvVec2<T> operator/(const hvVec2<T> &b) const { hvVec2<T> vv; vv.x=x/b.x; vv.y=y/b.y; return vv;}
	void operator/=(const hvVec2<T> &b) { x/=b.x; y/=b.y; }
	void operator/=(T v) { x/=v; y/=v; }
	template <class X> void operator/=(X v) { x= (T)((X)x/v); y=(T)((X)y/v); }

	// scale by scalar value
	void scale(const hvVec2<T> &a, T k) { x=a.x*k; y=a.y*k; }
	void scale(T k) { x*=k; y*=k; }
	template <class X> void scale(X v) { x= (T)((X)x*v); y=(T)((X)y*v); }

	// divide by scalar, this scalar cannot be null
	void normalize(T norme) { x /=  norme; y /=  norme; }
	void divScale(T k) { x/=k; y/=k; }
	template <class X> void divScale(X v) { x= (T)((X)x/v); y=(T)((X)y/v); }

	// absolute value component by component
	void abs()
		{
		if (x<T(0)) x = -x;
		if (y<T(0)) y = -y;
		}

	// Trunc to max value
	void trunc(T max)
		{
		if (x>max) x = max;
		if (y>max) y = max;
		}

	// random vector
	void random()
	{
		x = ((T)((double)rand()/(double)RAND_MAX));
		y = ((T)((double)rand()/(double)RAND_MAX));
	}

	// points
	hvVec2<T> shift(const hvVec2<T> &p, const hvVec2<T> &dir, T length)
	{
		x = p.x+length*dir.x;
		y = p.y+length*dir.y;
	}
	void boxPoints(const hvVec2<T> &dir, T length, hvVec2<T> points[]) const
	{
		points[0]=*this;
		points[1]=hvVec2<T>(x, y+dir.y*length);
		points[2]=hvVec2<T>(x+dir.x*length, y);
		points[3]=hvVec2<T>(x+dir.x*length,y+dir.y*length);
	}

	// dot product (produit scalaire)
	T dot(const hvVec2<T> v) const { return x*v.x+y*v.y; }

	// opposite vector
	void reverse() { x=-x; y=-y; }
	hvVec2<T> operator-() { hvVec2<T> v(-x,-y); return v; }

	// cross product (determinant)
	T det( const hvVec2<T> &v2) const
		{
		return x*v2.y-y*v2.x;  
		} 
	// cross product (determinant)
	T cross( const hvVec2<T> &v2) const
		{
		return x*v2.y-y*v2.x;  
		} 

	// compute median vector between a and b, result is not normalized
	void bissec( const hvVec2<T> &a, const hvVec2<T> &b)
		{
		add(a,b); scale(0.5);
		}


	// linear interpolation between v1 and v2 
	//according to t (between 0 and 1), t must be a float or double
	//if t=0 result is v1, if t=1 result is v2
	//result is not normalized 
	template <class X>  void interpolate(const hvVec2<T> &v1, const hvVec2<T> &v2, X t)
		{
		x = (T)(((X)1.0-t)*(X)v1.x + t*(X)v2.x); 
		y = (T)(((X)1.0-t)*(X)v1.y + t*(X)v2.y); 
		}

	// linear interpolation between v1 and v2 
	//according to vector t; each component of t is between 0 and 1 and gives coefficient,
	//result is not normalized 
	template <class X> void interpolate(const hvVec2<T> &v1, const hvVec2<T> &v2, const hvVec2<X> &t)
		{
		x = (T)(((X)1.0-t.x)*(X)v1.x + t.x*(X)v2.x); 
		y = (T)(((X)1.0-t.y)*(X)v1.y + t.y*(X)v2.y); 
		}


	// compares component by component and keeps the min one 
	// result is not normalized
	void keepMin(const hvVec2<T> &v1, const hvVec2<T> &v2)
		{
		if (v1.x<v2.x) x = v1.x; else x = v2.x;
		if (v1.y<v2.y) y = v1.y; else y = v2.y;
		}

	// compares component by component and keeps the max one 
	// result is not normalized
	void keepMax(const hvVec2<T> &v1, const hvVec2<T> &v2)
		{
		if (v1.x>v2.x) x = v1.x; else x = v2.x;
		if (v1.y>v2.y) y = v1.y; else y = v2.y;
		}

	
	// returns the largest / smallest component 
	T maxCoord() const
		{
		if (x>y) return x;
		return y;
		}
	T minCoord() const
		{
		if (x<y) return x;
		return y;
		}


	// returns the euclidian distance between two points
	T distance( const hvVec2<T> &a) const
		{
		hvVec2<T> r;

		r.PVec(a, *this);
		return r.norm(); 
		}

	// compares two vectors
	bool equals(const hvVec2<T> &p1) const
		{
		return (p1.x==x) && (p1.y==y) ;
		}
	bool operator==(const hvVec2<T> &p1) const
		{
		return (p1.x==x) && (p1.y==y) ;
		}
	bool areClose(const hvVec2<T> &p1, T dist) const
		{
		hvVec2<T> v;
		v.PVec(p1, *this);
		v.abs();
		if (v.x<dist && v.y<dist ) return true;
		return false;
		}

	// computes the norm (vector length)
	T normSquared() const { return x*x+y*y; }
	double normSquaredDouble() const { return (double)x*(double)x+(double)y*(double)y; }
	T norm() const { return (T) sqrt(normSquaredDouble()); }

	// compare to zero
	bool isNull() const
		{
		return x==T(0) && y==T(0) ;
		}

	// test if all components are small (lower than dist)
	bool isQNull(T dist) const
		{
		hvVec2<T> v=*this;
		v.abs();
		return v.x<=dist && v.y<=dist ;
		}

};

}

#endif // !defined(AFX_VEC2_H__954C4849_5D4B_4173_9842_797C84C6FB1B__INCLUDED_)
