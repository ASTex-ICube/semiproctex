// hvMat3.h: interface for the mat3 class.
//
// Defines a 3x3 matrix
// main operations are: multiplication, addition, multiplication with vector
//
// By JMD 9/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAT3_H__1544EA11_662A_41FD_8BD3_D7311F73A131__INCLUDED_)
#define AFX_MAT3_H__1544EA11_662A_41FD_8BD3_D7311F73A131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#include "hvVec3.h"
#include "hvMat2.h"

namespace hview {

template <class T> class hvMat3 // : public hviAlgebra<hvMat3<T>,T>  
{
protected:
	hvVec3<T> i,j,k;

public:
	// Constructor: defines an identity matrix
	hvMat3<T>():i(hvVec3<T>(1,0,0)),j(hvVec3<T>(0,1,0)),k(hvVec3<T>(0,0,1)) {   }
	// defines a matrix by three 3D vectors each representing a column 
	hvMat3<T>(const hvVec3<T> &a, const hvVec3<T> &b, const hvVec3<T> &c):i(a),j(b),k(c) { }

	// defines a matrix with a single vector u as : M=uu^t (vector u times its transposed u^t) 
	hvMat3<T>(const hvVec3<T> &u) 
	{
		hvVec3<T> v(u);
		i.scale(u, v.X());
		j.scale(u, v.Y());
		k.scale(u, v.Z());
	}

	// Selectors : vectors corresponding to columns
	hvVec3<T> I() const { return i; }
	hvVec3<T> J() const { return j; }
	hvVec3<T> K() const { return k; }

	template <class X> hvMat3<T>(const hvMat3<X> &v) { i. template cast<X>(v.I()); j. template cast<X>(v.J()); k. template cast<X>(v.K()); }
	bool operator==(const hvMat3<T> &mm) const
	{
		return i==mm.i && j==mm.j && k==mm.k;
	}

	// multiply matrix by vector, result is vector
	hvVec3<T> mult(const hvVec3<T> &v) const
	{
		return hvVec3<T> (	i.X()*v.X()+j.X()*v.Y()+k.X()*v.Z() ,
							i.Y()*v.X()+j.Y()*v.Y()+k.Y()*v.Z() ,
							i.Z()*v.X()+j.Z()*v.Y()+k.Z()*v.Z() );
	}
	hvVec3<T> operator*(const hvVec3<T> &v) const
	{
		return hvVec3<T> (	i.X()*v.X()+j.X()*v.Y()+k.X()*v.Z() ,
							i.Y()*v.X()+j.Y()*v.Y()+k.Y()*v.Z() ,
							i.Z()*v.X()+j.Z()*v.Y()+k.Z()*v.Z() );
	}

	// multiply by a scalar value all components
	void scale(T x)
	{
		i.scale(x);
		j.scale(x);
		k.scale(x);
	}
	void operator*=(T x)
	{
		i.scale(x);
		j.scale(x);
		k.scale(x);
	}
	void operator/=(T x)
	{
		i/=x;
		j/=x;
		k/=x;
	}
	void scale(const hvMat3<T> &m, T x)
	{
		i.scale(m.i, x);
		j.scale(m.j, x);
		k.scale(m.k, x);
	}

	// multiply two matrices
	void mult(const hvMat3<T> &a, const hvMat3<T> &b)
	{
		hvMat3<T> r;
		r.i=hvVec3<T>(	a.i.X()*b.i.X()+a.j.X()*b.i.Y()+a.k.X()*b.i.Z(),
						a.i.Y()*b.i.X()+a.j.Y()*b.i.Y()+a.k.Y()*b.i.Z(),
						a.i.Z()*b.i.X()+a.j.Z()*b.i.Y()+a.k.Z()*b.i.Z() );

		r.j=hvVec3<T>(	a.i.X()*b.j.X()+a.j.X()*b.j.Y()+a.k.X()*b.j.Z(),
						a.i.Y()*b.j.X()+a.j.Y()*b.j.Y()+a.k.Y()*b.j.Z(),
						a.i.Z()*b.j.X()+a.j.Z()*b.j.Y()+a.k.Z()*b.j.Z() );

		r.k=hvVec3<T>(	a.i.X()*b.k.X()+a.j.X()*b.k.Y()+a.k.X()*b.k.Z(),
						a.i.Y()*b.k.X()+a.j.Y()*b.k.Y()+a.k.Y()*b.k.Z(),
						a.i.Z()*b.k.X()+a.j.Z()*b.k.Y()+a.k.Z()*b.k.Z() );
		*this = r;
	}

	// multiply two matrices
	hvMat3<T> operator*(const hvMat3<T> &b) const
	{
		hvMat3<T> r;

		r.i=hvVec3<T>(	i.X()*b.i.X()+j.X()*b.i.Y()+k.X()*b.i.Z(),
						i.Y()*b.i.X()+j.Y()*b.i.Y()+k.Y()*b.i.Z(),
						i.Z()*b.i.X()+j.Z()*b.i.Y()+k.Z()*b.i.Z() );

		r.j=hvVec3<T>(	i.X()*b.j.X()+j.X()*b.j.Y()+k.X()*b.j.Z(),
						i.Y()*b.j.X()+j.Y()*b.j.Y()+k.Y()*b.j.Z(),
						i.Z()*b.j.X()+j.Z()*b.j.Y()+k.Z()*b.j.Z() );

		r.k=hvVec3<T>(	i.X()*b.k.X()+j.X()*b.k.Y()+k.X()*b.k.Z(),
						i.Y()*b.k.X()+j.Y()*b.k.Y()+k.Y()*b.k.Z(),
						i.Z()*b.k.X()+j.Z()*b.k.Y()+k.Z()*b.k.Z() );
		return r;
	}

	// multiply two matrices
	void operator*=(const hvMat3<T> &b)
	{
		hvMat3<T> r;

		r.i=hvVec3<T>(	i.X()*b.i.X()+j.X()*b.i.Y()+k.X()*b.i.Z(),
						i.Y()*b.i.X()+j.Y()*b.i.Y()+k.Y()*b.i.Z(),
						i.Z()*b.i.X()+j.Z()*b.i.Y()+k.Z()*b.i.Z() );

		r.j=hvVec3<T>(	i.X()*b.j.X()+j.X()*b.j.Y()+k.X()*b.j.Z(),
						i.Y()*b.j.X()+j.Y()*b.j.Y()+k.Y()*b.j.Z(),
						i.Z()*b.j.X()+j.Z()*b.j.Y()+k.Z()*b.j.Z() );

		r.k=hvVec3<T>(	i.X()*b.k.X()+j.X()*b.k.Y()+k.X()*b.k.Z(),
						i.Y()*b.k.X()+j.Y()*b.k.Y()+k.Y()*b.k.Z(),
						i.Z()*b.k.X()+j.Z()*b.k.Y()+k.Z()*b.k.Z() );
		*this= r;
	}

	// divide two matrices (multiply with inverse)
	void div(const hvMat3<T> &a, const hvMat3<T> &b)
	{
		hvMat3<T> r(b);
		T d = r.det();
		if (d==T(0)) { hvFatal("cannot divide by matrice!"); }
		r.inverse(r, d);
		mult(a, r);
	}
	void operator/=(const hvMat3<T> &b)
	{
		hvMat3<T> r(b);
		T d = r.det();
		if (d==T(0)) { hvFatal("cannot divide by matrice!"); }
		r.inverse(r, d);
		mult(*this, r);
	}
	hvMat3<T> operator/(const hvMat3<T> &b) const
	{
		hvMat3<T> r(b);
		T d = r.det();
		if (d==T(0)) { hvFatal("cannot divide by matrice!"); }
		r.inverse(r, d);
		r.mult(*this, r);
		return r;
	}

	// add two matrices
	void add(const hvMat3<T> &a, const hvMat3<T> &b)
	{
		i.add(a.i, b.i);
		j.add(a.j, b.j);
		k.add(a.k, b.k);
	}
	hvMat3<T> operator+(const hvMat3<T> &b) const
	{
		hvMat3<T> r;

		r.i=i+b.i;
		r.j=j+b.j;
		r.k=k+b.k;
		return r;
	}
	void operator+=(const hvMat3<T> &b)
	{
		i+=b.i;
		j+=b.j;
		k+=b.k;
	}

	// sub two matrices
	void sub(const hvMat3<T> &a, const hvMat3<T> &b)
	{
		i.sub(a.i, b.i);
		j.sub(a.j, b.j);
		k.sub(a.k, b.k);
	}
	hvMat3<T> operator-(const hvMat3<T> &b) const
	{
		hvMat3<T> r;

		r.i=i-b.i;
		r.j=j-b.j;
		r.k=k-b.k;
		return r;
	}
	void operator-=(const hvMat3<T> &b)
	{
		i-=b.i;
		j-=b.j;
		k-=b.k;
	}

	// cast mat3 as mat2 by keeping only upper left 2x2 components
	operator hvMat2<T>()
	{
		hvMat2<T> m (	hvVec2<T>(i.X(), i.Y()),
					hvVec2<T>(j.X(), j.Y())  );
		return m;
	}


	// compute determinant (a scalar value)
	T det() const
	{
		return	  i.X()*(j.Y()*k.Z()-k.Y()*j.Z()) 
				- i.Y()*(j.X()*k.Z()-k.X()*j.Z())
				+ i.Z()*(j.X()*k.Y()-k.X()*j.Y());
	}

	// Inverse the matrix a, 
	// works only if det/=0, det must be the determinant of a
	void inverse(const hvMat3<T> &a, T det)
	{
		if (det==T(0)) { hvFatal("cannot inverse with nul det!"); }
		hvVec3<T> ai(a.i), aj(a.j), ak(a.k);
		i = hvVec3<T>(	  aj.Y()*ak.Z()-ak.Y()*aj.Z(),
						-(aj.X()*ak.Z()-ak.X()*aj.Z()),
						  aj.X()*ak.Y()-ak.X()*aj.Y()  );

		j = hvVec3<T>(	-(ai.Y()*ak.Z()-ak.Y()*ai.Z()),
						  ai.X()*ak.Z()-ak.X()*ai.Z(),
						-(ai.X()*ak.Y()-ak.X()*ai.Y())  );

		k = hvVec3<T>(	  ai.Y()*aj.Z()-aj.Y()*ai.Z(),
						-(ai.X()*aj.Z()-aj.X()*ai.Z()),
						  ai.X()*aj.Y()-aj.X()*ai.Y()  );

		scale(1.0/det);
		transpose();
	}
	void inverse(const hvMat3<T> &a)
	{
		hvMat3<T> r(a);
		T dd = r.det();
		if (dd==T(0)) { hvFatal("cannot inverse with nul det!"); }
		inverse(r, dd);
	}
	void inverse(T dd)
	{
		hvVec3<T> ai(i), aj(j), ak(k);
		if (dd==T(0)) { hvFatal("cannot inverse with nul det!"); }
		i = hvVec3<T>(	  aj.Y()*ak.Z()-ak.Y()*aj.Z(),
						-(aj.X()*ak.Z()-ak.X()*aj.Z()),
						  aj.X()*ak.Y()-ak.X()*aj.Y()  );

		j = hvVec3<T>(	-(ai.Y()*ak.Z()-ak.Y()*ai.Z()),
						  ai.X()*ak.Z()-ak.X()*ai.Z(),
						-(ai.X()*ak.Y()-ak.X()*ai.Y())  );

		k = hvVec3<T>(	  ai.Y()*aj.Z()-aj.Y()*ai.Z(),
						-(ai.X()*aj.Z()-aj.X()*ai.Z()),
						  ai.X()*aj.Y()-aj.X()*ai.Y()  );

		scale(1.0/dd);
		transpose();
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
		hvMat3<T> r;

		r.i=hvVec3<T>(i.X(), j.X(), k.X());
		r.j=hvVec3<T>(i.Y(), j.Y(), k.Y());
		r.k=hvVec3<T>(i.Z(), j.Z(), k.Z());
		*this =  r;
	}

	// computes eigen vectors I,J,K from matrix "this"
	hvMat3<T> eigen()
	{
		/*** computing first EigenVector v1 = (a1 = theta, a2= phi in polar) ***/
		int i;
		hvVec3<T> v1,v2,v3,v,w;
		T a1=T(0); T a2=T(0);
		for(i=2;i<20;i++)
			{
			v1=hvVec3<T>(cos(a1),sin(a1)*cos(a2),sin(a1)*sin(a2));
			v=this->mult(v1);
			w=hvVec3<T>(-sin(a1),cos(a1)*cos(a2),cos(a1)*sin(a2));
			T d1 = v.dot(w);
			w=hvVec3<T>(T(0),-sin(a1)*sin(a2),sin(a1)*cos(a2));
			T d2= v.dot(w);
			a1+=M_PI*(d1<T(0) ? T(-1.0) : T(1.0))/(T)pow(2.0,(double)i);				/* adjust ai */
			a2+=M_PI*(d2<T(0) ? T(-1.0) : T(1.0))/(T)pow(2.0,(double)i);
			}
		v1=hvVec3<T>(cos(a1),sin(a1)*cos(a2),sin(a1)*sin(a2));

		/*** computing 2nd & 3rd EigenVectors (a1 = theta in polar in plane (v2,v3) ) ***/
		v2=hvVec3<T>(-v1.Y(),v1.X(),T(0));											
		v2.normalize(v2.norm());
		v3.cross(v1,v2);
		v=this->mult(v2);
		T k1=-v.dot(v2);
		T k2=2.0*v.dot(v3);
		v=this->mult(v3);
		k1 += v.dot(v3); /*   k1= v3.C.v3-v2.C.v2 ; k2= 2 * v3.C.v2   */
		a1=T(0);
		for(i=2;i<20;i++)
			{
			T d1=sin(2.0*a1)*k1+cos(2.0*a1)*k2;
			a1 += M_PI*(d1<T(0) ? T(-1.0) : T(1.0))/(T)pow(2.0,(double)i);
		}
		v=v2*cos(a1);
		w=v3*sin(a1);
		v2=v+w;
		v3.cross(v1,v2);
		hvMat3<T> res(v1,v2,v3);
		res.transpose();
		return res;
	}

};

}

#endif // !defined(AFX_MAT3_H__1544EA11_662A_41FD_8BD3_D7311F73A131__INCLUDED_)
