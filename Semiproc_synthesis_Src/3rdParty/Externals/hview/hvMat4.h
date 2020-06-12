/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hvMat4.h: interface for the mat4 class.
//
// Defines a 4x4 matrix in homogenous coordinates
// main operations are: multiplication, addition, multiplication with 4D vector
//
// By JMD 9/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAT4_H__D8969113_4A06_4FBC_AADF_632871632A1E__INCLUDED_)
#define AFX_MAT4_H__D8969113_4A06_4FBC_AADF_632871632A1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvMat3.h"
#include "hvVec4.h"

namespace hview {

template <class T> class hvMat4 
{
protected:
	hvVec4<T> i,j,k,l;

public:
	// Constructor: defines an identity matrix
	hvMat4<T>():i(hvVec4<T>(1,0,0,0)),j(hvVec4<T>(0,1,0,0)),k(hvVec4<T>(0,0,1,0)),l(hvVec4<T>(0,0,0,1)) { }
	
	// defines a matrix by four 4D vectors each representing a column 	
	hvMat4<T>(const hvVec4<T> &a, const hvVec4<T> &b, const hvVec4<T> &c, const hvVec4<T> &d):i(a),j(b),k(c),l(d) { }

	// defines a 4D matrix from a 3D one, the 3D one is upper left corner
	// rest of matrix is filled as identity
	hvMat4<T>(const hvMat3<T> &a):i(hvVec4<T>(a.I(),0)), j(hvVec4<T>(a.J(),0)), k(hvVec4<T>(a.K(),0)), l(hvVec4<T>(0,0,0,1)) { }

	// defines a 4D matrix from a 3D one + a 3D vector, the 3D one is upper left corner
	// the rest is filled as translation by vector
	hvMat4<T>(const hvMat3<T> &a, const hvVec3<T> &v) :i(hvVec4<T>(a.I(),0)), j(hvVec4<T>(a.J(),0)), k(hvVec4<T>(a.K(),0)), l(v,1) { }

	// defines a 4D matrix from a 3D vector
	// matrix is diagonal as scale
	hvMat4<T>(const hvVec3<T> &v): i(hvVec4<T>(v.X(),0,0,0)),j(hvVec4<T>(0,v.Y(),0,0)),k(hvVec4<T>(0,0,v.Z(),0)), l(hvVec4<T>(0,0,0,1)) { }

	// Selectors : vectors corresponding to columns
	hvVec4<T> I() const { return i; }
	hvVec4<T> J() const { return j; }
	hvVec4<T> K() const { return k; }
	hvVec4<T> L() const { return l; }

	template <class X> hvMat4<T>(const hvMat4<X> &v) { i. template cast<X>(v.I()); j. template cast<X>(v.J()); k. template cast<X>(v.K()); l. template cast<X>(v.L()); }

	// multiply matrix by 4D vector, result is vector 4D
	hvVec4<T> mult(const hvVec4<T> &v) const
	{
		return hvVec4<T> (	i.X()*v.X()+j.X()*v.Y()+k.X()*v.Z()+l.X()*v.W() ,
							i.Y()*v.X()+j.Y()*v.Y()+k.Y()*v.Z()+l.Y()*v.W() ,
							i.Z()*v.X()+j.Z()*v.Y()+k.Z()*v.Z()+l.Z()*v.W() ,
							i.W()*v.X()+j.W()*v.Y()+k.W()*v.Z()+l.W()*v.W()  );
	}
	hvVec4<T> operator*(const hvVec4<T> &v) const 
	{
		return hvVec4<T> (	i.X()*v.X()+j.X()*v.Y()+k.X()*v.Z()+l.X()*v.W() ,
							i.Y()*v.X()+j.Y()*v.Y()+k.Y()*v.Z()+l.Y()*v.W() ,
							i.Z()*v.X()+j.Z()*v.Y()+k.Z()*v.Z()+l.Z()*v.W() ,
							i.W()*v.X()+j.W()*v.Y()+k.W()*v.Z()+l.W()*v.W()  );
	}

	// multiply by a scalar value x all components
	void scale(T x)
	{
		i.scale(x);
		j.scale(x);
		k.scale(x);
		l.scale(x);
	}
	void operator*=(T x)
	{
		i.scale(x);
		j.scale(x);
		k.scale(x);
		l.scale(x);
	}
	void operator/=(T x)
	{
		i/=x;
		j/=x;
		k/=x;
		l/=x;
	}
	void scale(const hvMat4<T> &m, T x)
	{
		i.scale(m.i, x);
		j.scale(m.j, x);
		k.scale(m.k, x);
		l.scale(m.l, x);
	}

	// multiply two matrices
	void mult(const hvMat4<T> &a, const hvMat4<T> &b)
	{
		hvMat4<T> r;
		r.i=hvVec4<T>(	a.i.X()*b.i.X()+a.j.X()*b.i.Y()+a.k.X()*b.i.Z()+a.l.X()*b.i.W(),
						a.i.Y()*b.i.X()+a.j.Y()*b.i.Y()+a.k.Y()*b.i.Z()+a.l.Y()*b.i.W(),
						a.i.Z()*b.i.X()+a.j.Z()*b.i.Y()+a.k.Z()*b.i.Z()+a.l.Z()*b.i.W(),
						a.i.W()*b.i.X()+a.j.W()*b.i.Y()+a.k.W()*b.i.Z()+a.l.W()*b.i.W() );

		r.j=hvVec4<T>(	a.i.X()*b.j.X()+a.j.X()*b.j.Y()+a.k.X()*b.j.Z()+a.l.X()*b.j.W(),
						a.i.Y()*b.j.X()+a.j.Y()*b.j.Y()+a.k.Y()*b.j.Z()+a.l.Y()*b.j.W(),
						a.i.Z()*b.j.X()+a.j.Z()*b.j.Y()+a.k.Z()*b.j.Z()+a.l.Z()*b.j.W(),
						a.i.W()*b.j.X()+a.j.W()*b.j.Y()+a.k.W()*b.j.Z()+a.l.W()*b.j.W() );

		r.k=hvVec4<T>(	a.i.X()*b.k.X()+a.j.X()*b.k.Y()+a.k.X()*b.k.Z()+a.l.X()*b.k.W(),
						a.i.Y()*b.k.X()+a.j.Y()*b.k.Y()+a.k.Y()*b.k.Z()+a.l.Y()*b.k.W(),
						a.i.Z()*b.k.X()+a.j.Z()*b.k.Y()+a.k.Z()*b.k.Z()+a.l.Z()*b.k.W(),
						a.i.W()*b.k.X()+a.j.W()*b.k.Y()+a.k.W()*b.k.Z()+a.l.W()*b.k.W() );

		r.l=hvVec4<T>(	a.i.X()*b.l.X()+a.j.X()*b.l.Y()+a.k.X()*b.l.Z()+a.l.X()*b.l.W(),
						a.i.Y()*b.l.X()+a.j.Y()*b.l.Y()+a.k.Y()*b.l.Z()+a.l.Y()*b.l.W(),
						a.i.Z()*b.l.X()+a.j.Z()*b.l.Y()+a.k.Z()*b.l.Z()+a.l.Z()*b.l.W(),
						a.i.W()*b.l.X()+a.j.W()*b.l.Y()+a.k.W()*b.l.Z()+a.l.W()*b.l.W() );
		*this = r;
	
	}
	// multiply two matrices
	hvMat4<T> operator*(const hvMat4<T> &b) const
	{
		hvMat4<T> r;
		r.i=hvVec4<T>(	i.X()*b.i.X()+j.X()*b.i.Y()+k.X()*b.i.Z()+l.X()*b.i.W(),
						i.Y()*b.i.X()+j.Y()*b.i.Y()+k.Y()*b.i.Z()+l.Y()*b.i.W(),
						i.Z()*b.i.X()+j.Z()*b.i.Y()+k.Z()*b.i.Z()+l.Z()*b.i.W(),
						i.W()*b.i.X()+j.W()*b.i.Y()+k.W()*b.i.Z()+l.W()*b.i.W() );

		r.j=hvVec4<T>(	i.X()*b.j.X()+j.X()*b.j.Y()+k.X()*b.j.Z()+l.X()*b.j.W(),
						i.Y()*b.j.X()+j.Y()*b.j.Y()+k.Y()*b.j.Z()+l.Y()*b.j.W(),
						i.Z()*b.j.X()+j.Z()*b.j.Y()+k.Z()*b.j.Z()+l.Z()*b.j.W(),
						i.W()*b.j.X()+j.W()*b.j.Y()+k.W()*b.j.Z()+l.W()*b.j.W() );

		r.k=hvVec4<T>(	i.X()*b.k.X()+j.X()*b.k.Y()+k.X()*b.k.Z()+l.X()*b.k.W(),
						i.Y()*b.k.X()+j.Y()*b.k.Y()+k.Y()*b.k.Z()+l.Y()*b.k.W(),
						i.Z()*b.k.X()+j.Z()*b.k.Y()+k.Z()*b.k.Z()+l.Z()*b.k.W(),
						i.W()*b.k.X()+j.W()*b.k.Y()+k.W()*b.k.Z()+l.W()*b.k.W() );

		r.l=hvVec4<T>(	i.X()*b.l.X()+j.X()*b.l.Y()+k.X()*b.l.Z()+l.X()*b.l.W(),
						i.Y()*b.l.X()+j.Y()*b.l.Y()+k.Y()*b.l.Z()+l.Y()*b.l.W(),
						i.Z()*b.l.X()+j.Z()*b.l.Y()+k.Z()*b.l.Z()+l.Z()*b.l.W(),
						i.W()*b.l.X()+j.W()*b.l.Y()+k.W()*b.l.Z()+l.W()*b.l.W() );
		return r;
	}

	void operator*=(const hvMat4<T> &b)
	{
		hvMat4<T> r;
		r.i=hvVec4<T>(	i.X()*b.i.X()+j.X()*b.i.Y()+k.X()*b.i.Z()+l.X()*b.i.W(),
						i.Y()*b.i.X()+j.Y()*b.i.Y()+k.Y()*b.i.Z()+l.Y()*b.i.W(),
						i.Z()*b.i.X()+j.Z()*b.i.Y()+k.Z()*b.i.Z()+l.Z()*b.i.W(),
						i.W()*b.i.X()+j.W()*b.i.Y()+k.W()*b.i.Z()+l.W()*b.i.W() );

		r.j=hvVec4<T>(	i.X()*b.j.X()+j.X()*b.j.Y()+k.X()*b.j.Z()+l.X()*b.j.W(),
						i.Y()*b.j.X()+j.Y()*b.j.Y()+k.Y()*b.j.Z()+l.Y()*b.j.W(),
						i.Z()*b.j.X()+j.Z()*b.j.Y()+k.Z()*b.j.Z()+l.Z()*b.j.W(),
						i.W()*b.j.X()+j.W()*b.j.Y()+k.W()*b.j.Z()+l.W()*b.j.W() );

		r.k=hvVec4<T>(	i.X()*b.k.X()+j.X()*b.k.Y()+k.X()*b.k.Z()+l.X()*b.k.W(),
						i.Y()*b.k.X()+j.Y()*b.k.Y()+k.Y()*b.k.Z()+l.Y()*b.k.W(),
						i.Z()*b.k.X()+j.Z()*b.k.Y()+k.Z()*b.k.Z()+l.Z()*b.k.W(),
						i.W()*b.k.X()+j.W()*b.k.Y()+k.W()*b.k.Z()+l.W()*b.k.W() );

		r.l=hvVec4<T>(	i.X()*b.l.X()+j.X()*b.l.Y()+k.X()*b.l.Z()+l.X()*b.l.W(),
						i.Y()*b.l.X()+j.Y()*b.l.Y()+k.Y()*b.l.Z()+l.Y()*b.l.W(),
						i.Z()*b.l.X()+j.Z()*b.l.Y()+k.Z()*b.l.Z()+l.Z()*b.l.W(),
						i.W()*b.l.X()+j.W()*b.l.Y()+k.W()*b.l.Z()+l.W()*b.l.W() );
		*this = r;
	}

	// add two matrices
	void add(const hvMat4<T> &a, const hvMat4<T> &b)
	{
		i.add(a.i, b.i);
		j.add(a.j, b.j);
		k.add(a.k, b.k);
		l.add(a.l, b.l);
	}

	// add two matrices
	hvMat4<T> operator+(hvMat4<T> &b) const
	{
		hvMat4<T> r;

		r.i=i+b.i;
		r.j=j+b.j;
		r.k=k+b.k;
		r.l=l+b.l;
		return r;
	}

	// add two matrices
	void operator+=(hvMat4<T> &b)
	{
		i+=b.i;
		j+=b.j;
		k+=b.k;
		l+=b.l;
	}

	// cast mat4 as mat3 by keeping only upper left 3x3 components
	operator hvMat3<T>()
	{
		hvMat3<T> m (	hvVec3<T>(i.X(), i.Y(), i.Z()),
					hvVec3<T>(j.X(), j.Y(), j.Z()),
					hvVec3<T>(k.X(), k.Y(), k.Z())  );
		return m;
	}

	// sub two matrices
	void sub(const hvMat4<T> &a, const hvMat4<T> &b)
	{
		i.sub(a.i, b.i);
		j.sub(a.j, b.j);
		k.sub(a.k, b.k);
		l.sub(a.l, b.l);
	}
	hvMat4<T> operator-(const hvMat4<T> &b) const
	{
		hvMat4<T> r;

		r.i=i-b.i;
		r.j=j-b.j;
		r.k=k-b.k;
		r.l=l-b.l;
		return r;
	}
	void operator-=(const hvMat4<T> &b)
	{
		i-=b.i;
		j-=b.j;
		k-=b.k;
		l-=b.l;
	}


};


}

#endif // !defined(AFX_MAT4_H__D8969113_4A06_4FBC_AADF_632871632A1E__INCLUDED_)
