/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hvMat2.h: interface for the mat2 class.
//
// Defines a 2x2 matrix
// main operations are: multiplication, addition, multiplication with vector
//
// By JMD 9/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAT2_H__1544EA11_662A_41FD_8BD3_D7311F73A131__INCLUDED_)
#define AFX_MAT2_H__1544EA11_662A_41FD_8BD3_D7311F73A131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#include "hvVec2.h"

namespace hview {

template <class T> class hvMat2 // : public hviAlgebra<hvMat3<T>,T>  
{
protected:
	hvVec2<T> i,j;

public:
	// Constructor: defines an identity matrix
	hvMat2<T>():i(hvVec2<T>(1,0)),j(hvVec2<T>(0,1)) {   }
	// defines a matrix by three 3D vectors each representing a column 
	hvMat2<T>(const hvVec2<T> &a, const hvVec2<T> &b):i(a),j(b) { }

	// defines a matrix with a single vector u as : M=uu^t (vector u times its transposed u^t) 
	hvMat2<T>(const hvVec2<T> &u) 
	{
		hvVec2<T> v(u);
		i.scale(u, v.X());
		j.scale(u, v.Y());
	}

	// Selectors : vectors corresponding to columns
	hvVec2<T> I() const { return i; }
	hvVec2<T> J() const { return j; }

	template <class X> hvMat2<T>(const hvMat2<X> &v) { i. template cast<X>(v.I()); j. template cast<X>(v.J()); }

	// multiply matrix by vector, result is vector
	hvVec2<T> mult(const hvVec2<T> &v) const
	{
		return hvVec2<T> (	i.X()*v.X()+j.X()*v.Y() ,
							i.Y()*v.X()+j.Y()*v.Y() );
	}
	hvVec2<T> operator*(const hvVec2<T> &v) const
	{
		return hvVec2<T> (	i.X()*v.X()+j.X()*v.Y() ,
							i.Y()*v.X()+j.Y()*v.Y() );
	}

	// multiply by a scalar value all components
	void scale(T x)
	{
		i.scale(x);
		j.scale(x);
	}
	void operator*=(T x)
	{
		i.scale(x);
		j.scale(x);
	}
	void operator/=(T x)
	{
		i/=x;
		j/=x;
	}
	void scale(const hvMat2<T> &m, T x)
	{
		i.scale(m.i, x);
		j.scale(m.j, x);
	}

	// multiply two matrices
	void mult(const hvMat2<T> &a, const hvMat2<T> &b)
	{
		hvMat2<T> r;
		r.i=hvVec2<T>(	a.i.X()*b.i.X()+a.j.X()*b.i.Y(),
						a.i.Y()*b.i.X()+a.j.Y()*b.i.Y() );

		r.j=hvVec2<T>(	a.i.X()*b.j.X()+a.j.X()*b.j.Y(),
						a.i.Y()*b.j.X()+a.j.Y()*b.j.Y() );

		*this = r;
	}

	// multiply two matrices
	hvMat2<T> operator*(const hvMat2<T> &b) const
	{
		hvMat2<T> r;

		r.i=hvVec2<T>(	i.X()*b.i.X()+j.X()*b.i.Y(),
						i.Y()*b.i.X()+j.Y()*b.i.Y() );

		r.j=hvVec2<T>(	i.X()*b.j.X()+j.X()*b.j.Y(),
						i.Y()*b.j.X()+j.Y()*b.j.Y() );

		return r;
	}

	// multiply two matrices
	void operator*=(const hvMat2<T> &b)
	{
		hvMat2<T> r;

		r.i=hvVec2<T>(	i.X()*b.i.X()+j.X()*b.i.Y(),
						i.Y()*b.i.X()+j.Y()*b.i.Y() );

		r.j=hvVec2<T>(	i.X()*b.j.X()+j.X()*b.j.Y(),
						i.Y()*b.j.X()+j.Y()*b.j.Y() );

		*this= r;
	}

	// divide two matrices (multiply with inverse)
	void div(const hvMat2<T> &a, const hvMat2<T> &b)
	{
		hvMat2<T> r(b);
		T d = r.det();
		if (d==T(0)) { hvFatal("cannot divide by matrice!"); }
		r.inverse(r, d);
		mult(a, r);
	}
	void operator/=(const hvMat2<T> &b)
	{
		hvMat2<T> r(b);
		T d = r.det();
		if (d==T(0)) { hvFatal("cannot divide by matrice!"); }
		r.inverse(r, d);
		mult(*this, r);
	}
	hvMat2<T> operator/(const hvMat2<T> &b) const
	{
		hvMat2<T> r(b);
		T d = r.det();
		if (d==T(0)) { hvFatal("cannot divide by matrice!"); }
		r.inverse(r, d);
		r.mult(*this, r);
		return r;
	}

	// add two matrices
	void add(const hvMat2<T> &a, const hvMat2<T> &b)
	{
		i.add(a.i, b.i);
		j.add(a.j, b.j);
	}
	hvMat2<T> operator+(const hvMat2<T> &b) const
	{
		hvMat2<T> r;

		r.i=i+b.i;
		r.j=j+b.j;
		return r;
	}
	void operator+=(const hvMat2<T> &b)
	{
		i+=b.i;
		j+=b.j;
	}

	// sub two matrices
	void sub(const hvMat2<T> &a, const hvMat2<T> &b)
	{
		i.sub(a.i, b.i);
		j.sub(a.j, b.j);
	}
	hvMat2<T> operator-(const hvMat2<T> &b) const
	{
		hvMat2<T> r;

		r.i=i-b.i;
		r.j=j-b.j;
		return r;
	}
	void operator-=(const hvMat2<T> &b)
	{
		i-=b.i;
		j-=b.j;
	}


	// compute determinant (a scalar value)
	T det() const
	{
		return	  i.X()*j.Y() - i.Y()*j.X();
	}

	// Inverse the matrix a, 
	// works only if det/=0, det must be the determinant of a
	void inverse(const hvMat2<T> &a, T det)
	{
		if (det==T(0)) { hvFatal("cannot inverse with nul det!"); }
		hvVec2<T> ai(a.i), aj(a.j);
		i = hvVec3<T>(	aj.Y(), -ai.Y()   );
		j = hvVec3<T>(	-aj.X(), ai.X()   );
		scale(1.0/det);
	}
	void inverse(const hvMat2<T> &a)
	{
		hvMat2<T> r(a);
		T dd = r.det();
		if (dd==T(0)) { hvFatal("cannot inverse with nul det!"); }
		inverse(r, dd);
	}
	void inverse(T dd)
	{
		hvVec2<T> ai(i), aj(j);
		if (dd==T(0)) { hvFatal("cannot inverse with nul det!"); }
		scale(1.0/dd);
	}
	void inverse()
	{
	T dd = det();
	if (dd==T(0)) { hvFatal("cannot inverse with nul det!"); }
	inverse(dd);
	}
	// transpose matrix (symetry along the diagonal)
	void transpose()
	{
		hvMat2<T> r;

		r.i=hvVec2<T>(i.X(), j.X());
		r.j=hvVec2<T>(i.Y(), j.Y());
		*this =  r;
	}

};

}

#endif // !defined(AFX_MAT3_H__1544EA11_662A_41FD_8BD3_D7311F73A131__INCLUDED_)
