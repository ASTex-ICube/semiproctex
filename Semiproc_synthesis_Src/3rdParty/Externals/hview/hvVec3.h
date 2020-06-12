/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// vec3.h: interface for the generic vec3 class.
//
// defines a vector in 3D
// By JMD 8/8/04
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VEC3_H__A6721A04_8090_4DA7_A7BB_8AAB841F9C11__INCLUDED_)
#define AFX_VEC3_H__A6721A04_8090_4DA7_A7BB_8AAB841F9C11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "hvError.h"

namespace hview {


template <class T> class hvVec3 
{
protected:
	T x, y, z;

public:
	//////// constructors
	hvVec3<T>():x(T(0)),y(T(0)),z(T(0)) {  }
	hvVec3<T>(T a):x(a),y(a),z(a) {  }
	hvVec3<T>(T a, T b, T c):x(a),y(b),z(c) { }
	//template <class X> hvVec3<T>(const hvVec3<X> &v) { x=T(v.X()); y=T(v.Y()); z=T(v.Z()); }
	template <class X> void cast(const hvVec3<X> &v) { x=T(v.X()); y=T(v.Y()); z=T(v.Z()); }

	// define vector from two points (a,b): v=ab
	void PVec(const hvVec3<T> &a, const hvVec3<T> &b) { x=b.x-a.x; y=b.y-a.y; z=b.z-a.z; }

	//////// selectors of coordinates
	T X() const { return x; }
	T Y() const { return y; }
	T Z() const { return z; }
	T &operator[](int i)
	{
		if (i<0 || i>2) { hvFatal("out of vec3 range!"); }
		if (i==0) return x;
		if (i==1) return y;
		return z;
	}


	///////  operations
	void add(const hvVec3<T> &a, const hvVec3<T> &b) { x=a.x+b.x; y=a.y+b.y; z=a.z+b.z; }
	hvVec3<T> operator+(const hvVec3<T> &b) const { hvVec3<T> vv; vv.x=x+b.x; vv.y=y+b.y; vv.z=z+b.z; return vv;}
	void operator+=(const hvVec3<T> &b) { x+=b.x; y+=b.y; z+=b.z; }

	void sub(const hvVec3<T> &a, const hvVec3<T> &b) { x=a.x-b.x; y=a.y-b.y; z=a.z-b.z; }
	hvVec3<T> operator-(const hvVec3<T> &b) const { hvVec3<T> vv; vv.x=x-b.x; vv.y=y-b.y; vv.z=z-b.z; return vv;}
	void operator-=(const hvVec3<T> &b) { x-=b.x; y-=b.y; z-=b.z; }

	void mult(const hvVec3<T> &a, const hvVec3<T> &b) { x=b.x*a.x; y=b.y*a.y; z=b.z*a.z; }
	hvVec3<T> operator*(const hvVec3<T> &b) const { hvVec3<T> vv; vv.x=x*b.x; vv.y=y*b.y; vv.z=z*b.z; return vv;}
	void operator*=(const hvVec3<T> &b) { x*=b.x; y*=b.y; z*=b.z; }
	void operator*=(T v) { x*=v; y*=v; z*=v; }
	template <class X> void operator*=(X v) { x= (T)((X)x*v); y=(T)((X)y*v); z=(T)((X)z*v); }

	void div(const hvVec3<T> &a, const hvVec3<T> &b) { x=a.x/b.x; y=a.y/b.y; z=a.z/b.z; }
	hvVec3<T> operator/(const hvVec3<T> &b) const { hvVec3<T> vv; vv.x=x/b.x; vv.y=y/b.y; vv.z=z/b.z; return vv;}
	void operator/=(const hvVec3<T> &b) { x/=b.x; y/=b.y; z/=b.z; }
	void operator/=(T v) { x/=v; y/=v; z/=v;}
	template <class X> void operator/=(X v) { x= (T)((X)x/v); y=(T)((X)y/v); z=(T)((X)z/v); }

	// scale by scalar value
	void scale(const hvVec3<T> &a, T k) { x=a.x*k; y=a.y*k; z=a.z*k; }
	void scale(T k) { x*=k; y*=k; z*=k; }
	template <class X> void scale(X v) { x= (T)((X)x*v); y=(T)((X)y*v); z=(T)((X)z*v); }
	// divide by scalar, this scalar cannot be null
	void normalize(T norme) { x /=  norme; y /=  norme; z /=  norme; }
	void normalize(const hvVec3<T> &min, const hvVec3<T> &max)
	{
		x = (x-min.x)/(max.x-min.x);
		y = (y-min.y)/(max.y-min.y);
		z = (z-min.z)/(max.z-min.z);
	}
	void divScale(T k) { x/=k; y/=k; z/=k; }
	template <class X> void divScale(X v) { x= (T)((X)x/v); y=(T)((X)y/v); z=(T)((X)z/v); }

	// absolute value component by component
	void abs()
		{
		if (x<T(0)) x = -x;
		if (y<T(0)) y = -y;
		if (z<T(0)) z = -z;
		}
	// square component by component
	void square()
		{
		x = x*x;
		y = y*y;
		z = z*z;
		}

	// Trunc to max value
	void trunc(T max)
		{
		if (x>max) x = max;
		if (y>max) y = max;
		if (z>max) z = max;
		}

	// random vector
	void random()
	{
		x = ((T)((double)rand()/(double)RAND_MAX));
		y = ((T)((double)rand()/(double)RAND_MAX));
		z = ((T)((double)rand()/(double)RAND_MAX));
	}

	// points
	hvVec3<T> shift(const hvVec3<T> &p, const hvVec3<T> &dir, T length)
	{
		x = p.x+length*dir.x;
		y = p.y+length*dir.y;
		z = p.z+length*dir.z;
	}
	void boxPoints(const hvVec3<T> &dir, T length, hvVec3<T> points[]) const
	{
		hvVec3<T> min = *this;
		hvVec3<T> max = dir;
		max.scale(length);
		max.add(max, min);
		points[0]=min;
		points[1]=hvVec3<T>(min.X(),min.Y(),max.Z());
		points[2]=hvVec3<T>(min.X(),max.Y(),min.Z());
		points[3]=hvVec3<T>(min.X(),max.Y(),max.Z());
		points[4]=hvVec3<T>(max.X(),min.Y(),min.Z());
		points[5]=hvVec3<T>(max.X(),min.Y(),max.Z());
		points[6]=hvVec3<T>(max.X(),max.Y(),min.Z());
		points[7]=max;
	}


	// dot product (produit scalaire)
	T dot(const hvVec3<T> &v) const { return x*v.x+y*v.y+z*v.z; }

	// opposite vector
	void reverse() { x=-x; y=-y; z=-z; }
	hvVec3<T> operator-() { hvVec3<T> v(-x,-y,-z); return v; }

	// cross product (produit vectoriel)
	void cross( const hvVec3<T> &v1, const hvVec3<T> &v2)
		{
		  hvVec3<T> v;

		  v.x =   (v1.y)*(v2.z)  - ((v1.z)*(v2.y));
		  v.y = -((v1.x)*(v2.z)) + ((v1.z)*(v2.x));
		  v.z =   (v1.x)*(v2.y)  - ((v1.y)*(v2.x));
		  *this = v;
		 } 

	// Makes a rotation of vector v according to x, y and z axis sequentially
	//vector a indicates the respective angles in radian (0-2PI)
	void rotateCoord( const hvVec3<T> &v, const hvVec3<T> &a)
		{
		hvVec3<T> p,p2;

		/* Rotation along X axis */
		p.z = v.z*cos(a.x)-v.y*sin(a.x); 
		p.y = v.z*sin(a.x)+v.y*sin(a.x);
		p.x = v.x;

		/* Rotation along Y axis */
		p2.x = p.x*cos(a.y)-p.z*sin(a.y);                                
		p2.z = p.x*sin(a.y)+p.z*cos(a.y);
		p2.y = p.y;

		/* Rotation along Z axis */
		x = p2.x*cos(a.z)-p2.y*sin(a.z);                                 
		y = p2.x*sin(a.z)+p2.y*cos(a.z);
		z = p2.z;
		}

	// Makes a rotation of vector v according to x, y and z axis sequentially
	// operates like rotateCoord, but this time cosine and sine are directly given by vectors
	void rotateCosSinCoord( const hvVec3<T> &v, const hvVec3<T> &co, const hvVec3<T> &si)
		{
		hvVec3<T> p,p2;

		/* Rotation along X axis */
		p.z = v.z*co.x-v.y*si.x;
		p.y = v.z*si.x+v.y*co.x;
		p.x = v.x;

		/* Rotation along Y axis */
		p2.x = p.x*co.y-p.z*si.y;
		p2.z = p.x*si.y+p.z*co.y;
		p2.y = p.y;

		/* Rotation along Z axis */
		x = p2.x*co.z-p2.y*si.z;
		y = p2.x*si.z+p2.y*co.z;
		z = p2.z;
		}

	// chooses an orthogonal vector to r, parallel to the z=0 plane 
	//the resulting vector is normalized 
	void orthogonal(const hvVec3<T> &r)
		{ 
		T no;

		if (r.x==0 && r.y==0) { x=0.0; y=1.0; }
		else { x = -r.y; y = r.x; }
		z = 0.0;
		if (y<0.0) reverse();
		else if (x<0.0) reverse();
		no=norm(); if (no==T(0)) { x=T(0); y=T(0); z=T(0); }
		normalize(no);
		}
	// chooses an orthogonal vector to r, parallel to plane defined by the couple (r,v) 
	//the resulting vector is normalized 
	void orthogonalInPlan(const hvVec3<T> &r, const hvVec3<T> &v)
		{ 
		*this = r; scale(-r.dot(v)/r.dot(r));
		this->operator+=(v);
		T no=norm(); if (no==T(0)) { x=T(0); y=T(0); z=T(0); }
		normalize(no);
		if (dot(v)<T(0)) reverse();
		}

	// compute median vector between a and b, result is not normalized
	void bissec( const hvVec3<T> &a, const hvVec3<T> &b)
		{
		add(a,b); scale(0.5);
		}

	// compute reflection vector from normal n and viewing direction v, 
	//dot product between n and v must be positive
	//Note: result is not normalized 
	void reflection( const hvVec3<T> &v, const hvVec3<T> &n)
		{
		T scal;
		hvVec3<T> r;

		scal= 2.0*n.dot(v);
		r.scale(n,scal); 
		r.sub(r,v);
		*this=r;
		}

	// compute refraction vector from normal n and viewing direction v
	//according to the refraction indice ir (ratio between the two media) 
	//and Snell's law,
	//dot product between n and v must be positive
	//result is normalized,
	//returns false if computation was not possible (result is complex number), in this
	//case resulting vector is set null
	bool refraction( const hvVec3<T> &v, const hvVec3<T>  &n, double ir)
		{
		hvVec3<T>  vp, xx, r;
		double	kf;

		if (ir==1.0) { *this=v; reverse(); kf=norm(); normalize(kf); return true; }
		vp = v;
		kf = -1.0 / v.dot(n);  
		vp.scale(kf);
		xx.add(n, vp);
		kf = ir*ir*vp.dot(vp) - xx.dot(xx);
		if (kf <= T(0)) {  x=T(0); y=T(0); z=T(0); return false; }
		kf = 1.0 / sqrt(kf);
		r = xx; 
		r.scale(T(kf));
		r.sub(r,n); 
		kf=r.norm();
		r.normalize(kf);
		*this = r;
		return true;
		}



	// linear interpolation between v1 and v2 
	//according to t (between 0 and 1),
	//if t=0 result is v1, if t=1 result is v2
	//result is not normalized 
	template <class X> void interpolate(const hvVec3<T> &v1, const hvVec3<T> &v2, X t)
		{
		x = (T)(((X)1.0-t)*(X)v1.x + t*(X)v2.x); 
		y = (T)(((X)1.0-t)*(X)v1.y + t*(X)v2.y); 
		z = (T)(((X)1.0-t)*(X)v1.z + t*(X)v2.z); 
		}

	// linear interpolation between v1 and v2 
	//according to vector t; each component of t is between 0 and 1 and gives coefficient,
	//result is not normalized 
	template <class X> void interpolate(const hvVec3<T> &v1, const hvVec3<T> &v2, const hvVec3<X> &t)
		{
		x = (T)(((X)1.0-t.x)*(X)v1.x + t.x*(X)v2.x); 
		y = (T)(((X)1.0-t.y)*(X)v1.y + t.y*(X)v2.y); 
		z = (T)(((X)1.0-t.z)*(X)v1.z + t.z*(X)v2.z);
		}


	// compares component by component and keeps the min one 
	//result is not normalized
	void keepMin(const hvVec3<T> &v1, const hvVec3<T> &v2)
		{
		if (v1.x<v2.x) x = v1.x; else x = v2.x;
		if (v1.y<v2.y) y = v1.y; else y = v2.y;
		if (v1.z<v2.z) z = v1.z; else z = v2.z;
		}

	// compares component by component and keeps the max one 
	//result is not normalized 
	void keepMax(const hvVec3<T> &v1, const hvVec3<T> &v2)
		{
		if (v1.x>v2.x) x = v1.x; else x = v2.x;
		if (v1.y>v2.y) y = v1.y; else y = v2.y;
		if (v1.z>v2.z) z = v1.z; else z = v2.z;
		}

	
	// returns the largest / smallest component
	T maxCoord() const
		{
		if (x>y && x>z) return x;
		if (y>z) return y;
		return z;
		}
	T minCoord() const
		{
		if (x<y && x<z) return x;
		if (y<z) return y;
		return z;
		}


	// returns the euclidian distance between two points
	T distance( const hvVec3<T> &a) const
		{
		hvVec3<T> r;

		r.PVec(a, *this);
		return r.norm(); 
		}

	// compares two vectors
	bool equals(const hvVec3<T> &p1) const
		{
		return (p1.x==x) && (p1.y==y) && (p1.z==z);
		}
	bool operator==(const hvVec3<T> &p1) const
		{
		return (p1.x==x) && (p1.y==y) && (p1.z==z);
		}
	// true if the points are closer than dist
	bool areClose(const hvVec3<T> &p1, T dist) const
		{
		hvVec3<T> v;
		v.PVec(p1, *this);
		v.abs();
		if (v.x<dist && v.y<dist && v.z<dist) return true;
		return false;
		}

	// computes the norm (vector length) 
	T normSquared() const { return x*x+y*y+z*z; } 
	double normSquaredDouble() const { return (double)x*(double)x+(double)y*(double)y+(double)z*(double)z; }
	T norm() const { return (T) sqrt(normSquaredDouble()); }

	// compares to null vector
	bool isNull() const
		{
		return x==T(0) && y==T(0) && z==T(0);
		}

	bool isQNull(T dist) const
		{
		hvVec3<T> v=*this;
		v.abs();
		return v.x<=dist && v.y<=dist && v.z<=dist;
		}

	hvVec3<T> cartToPol() const
	{
		double zz = sqrt((double)x*(double)x+(double)y*(double)y+(double)z*(double)z);
		double d = sqrt((double)x*(double)x+(double)y*(double)y);
		if (d==0.0) 
			{ 
			return hvVec3<T>(T(0),z>T(0) ? T(M_PI/2.0): T(-M_PI/2.0),T(zz) ); 
			}
		double xx = acos((double)x/d);
		if (y<T(0)) xx = 2.0*M_PI-xx;
		return hvVec3<T>(T(xx), (T)atan((double)z/d), T(zz));
	}
	hvVec3<T> polToCart() const
	{
		return hvVec3<T>(T((double)z*cos((double)x)*cos((double)y)), T((double)z*sin((double)x)*cos((double)y)), T((double)z*sin((double)y)) );
	}

	// choosing an equiprobable random direction in unit Hemisphere defined by frame (no, uu, vv)
	// using a partitionning according to alpha only
	// A portion is considered only, the portion is given from angle betamin (from 0) to angle betamax (up to PI/2)
	// if nbu==0, no partitionning is used for alpha
	// otherwise a partitionning into nbu partitions is used, parameter ku gives the partition index in [0, nbu-1]
	void randomPolDir(int ku, int nbu, hvVec3<T> no, hvVec3<T> uu, hvVec3<T> vv, T betamin=T(0), T betamax=T(M_PI/2.0), bool holesphere=false)
	{
		/*
		double alpha,beta;
		if (nbu<=1)alpha= 2.0*M_PI*(double)rand()/(double)RAND_MAX;
		else alpha= 2.0*M_PI*( (double)ku+(double)rand()/(double)RAND_MAX)/(double)nbu;
		double rr = (double)rand()/(double)RAND_MAX;
		beta = acos( (1.0-rr)*cos((double)betamin)+rr*cos((double)betamax) );
		hvVec3<T> dir=no;
		if (holesphere) dir.scale((double)rand()/(double)RAND_MAX<0.5?(T)cos(beta):-(T)cos(beta));
		else dir.scale((T)cos(beta));
		uu.scale((T)(cos(alpha)*sin(beta))); dir+=uu;
		vv.scale((T)(sin(alpha)*sin(beta))); dir+=vv;
		dir.normalize(dir.norm());
		*this = dir;
		*/
		randomPolDir(ku,nbu,0,0,no,uu,vv,betamin,betamax,holesphere);
	}
	// choosing an equiprobable random direction in unit Hemisphere defined by frame (no, uu, vv)
	// using a partitionning according to alpha and beta
	// A portion is considered only, the portion is given from angle betamin (from 0) to angle betamax (up to PI/2)
	// if nbu==0, no partitionning is used for alpha, if nbv==0 no partitionning is used for beta
	// otherwise a partitionning into nbu*nbv partitions is used, parameters ku,kv give the partition index resp. in [0, nbu-1] and [0, nbv-1]
	void randomPolDir(int ku, int nbu, int kv, int nbv, hvVec3<T> no, hvVec3<T> uu, hvVec3<T> vv, T betamin=T(0), T betamax=T(M_PI/2.0), bool holesphere=false)
	{
		double alpha,beta;
		if (nbu<=1)alpha= 2.0*M_PI*(double)rand()/(double)RAND_MAX;
		else alpha= 2.0*M_PI*( (double)ku+(double)rand()/(double)RAND_MAX)/(double)nbu;
		double rr;
		if (nbv<=1) rr = (double)rand()/(double)RAND_MAX;
		else rr = ((double)kv+(double)rand()/(double)RAND_MAX)/(double)nbv;
		beta = acos( (1.0-rr)*cos((double)betamin)+rr*cos((double)betamax) );
		hvVec3<T> dir=no;
		if (holesphere) dir.scale((double)rand()/(double)RAND_MAX<0.5?(T)cos(beta):-(T)cos(beta));
		else dir.scale((T)cos(beta));
		uu.scale((T)(cos(alpha)*sin(beta))); dir+=uu;
		vv.scale((T)(sin(alpha)*sin(beta))); dir+=vv;
		dir.normalize(dir.norm());
		*this = dir;
	}
};

}

#endif // !defined(AFX_VEC3_H__A6721A04_8090_4DA7_A7BB_8AAB841F9C11__INCLUDED_)
