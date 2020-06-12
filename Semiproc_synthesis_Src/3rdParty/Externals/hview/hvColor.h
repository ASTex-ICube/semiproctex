/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

#if !defined(AFX_COLOR_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
#define AFX_COLOR_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "hvVec3.h"

namespace hview {

// D65 reference white
static const double Xn = 0.950456 ;
static const double Yn = 1.0 ;
static const double Zn = 1.088754 ;

static const double un = 0.197832 ;
static const double vn = 0.468340 ;

template <class T> class hvColRGB : public hvVec3<T>  
{
public:
	hvColRGB<T>() : hvVec3<T>() { }
	hvColRGB<T>(T r, T g, T b) : hvVec3<T>(r,g,b) { }
	hvColRGB<T>(T l) : hvVec3<T>(l) { }
	hvColRGB<T>(const hvVec3<T> &a): hvVec3<T>(a)  {  }
	template <class X> hvColRGB<T>(const hvColRGB<X> &c) { hvVec3<T> v; v.cast(hvVec3<X>(c)); *this = hvColRGB<T>(v); }
	operator hvVec3<T>() { return (hvVec3<T>)*this; }
	T RED() const { return hvVec3<T>::X(); }
	T GREEN() const { return hvVec3<T>::Y(); }
	T BLUE() const { return hvVec3<T>::Z(); }
	T luminance() const { return (T)(((double)hvVec3<T>::X()+(double)hvVec3<T>::Y()+(double)hvVec3<T>::Z())/3.0); }


	void torgbe(T scal, unsigned char rgbe[4])
	{
	  T v;
	  int e;

	  v = RED();
	  if (GREEN() > v) v = GREEN();
	  if (BLUE() > v) v = BLUE();
	  if ((float)v/(float)scal < 1e-32) {
		rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
	  }
	  else {
		float vr = frexp((float)v/(float)scal,&e) * 256.0/((float)v/(float)scal);
		rgbe[0] = (unsigned char) ((float)RED()/(float)scal * vr);
		rgbe[1] = (unsigned char) ((float)GREEN()/(float)scal * vr);
		rgbe[2] = (unsigned char) ((float)BLUE()/(float)scal * vr);
		rgbe[3] = (unsigned char) (e + 128);
	  }
	}
	void fromrgbe(T scal, unsigned char rgbe[4])
	{
	  float f;

	  if (rgbe[3]!=0) {   /*nonzero pixel*/
		f = ldexp(1.0,rgbe[3]-(int)(128+8));
		*this = hvColRGB( (T)((float)scal*f*(float)rgbe[0]), (T)((float)scal*f*(float)rgbe[1]),(T)((float)scal*f*(float)rgbe[2]) );
	  }
	  else
		*this = hvColRGB();
	}
	void tohsv(const hvColRGB<T> &rgb, T scal)
	{
		double min = (double)rgb.minCoord()/(double)scal;
		double max = (double)rgb.maxCoord()/(double)scal;
		double rr = (double)rgb.RED()/(double)scal;
		double gg = (double)rgb.GREEN()/(double)scal;
		double bb = (double)rgb.BLUE()/(double)scal;
		int t = 0;
		if (max!=min)
		{
			if (rgb.RED()>=rgb.GREEN() && rgb.RED()>=rgb.BLUE()) t = (int)(60.0*(gg-bb)/(max-min)+360.0) % 360;
			else if (rgb.GREEN()>=rgb.RED() && rgb.GREEN()>=rgb.BLUE()) t = (int)(60.0*(bb-rr)/(max-min)+120.0);
			else t = (int)(60.0*(rr-gg)/(max-min)+240.0);
		}
		double s = (max==0.0?0.0:1.0-min/max);
		double v = max;
		*this = hvColRGB((T)((double)t/360.0*(double)scal), (T)(s*(double)scal), (T)(v*(double)scal));
	}
	void toxyz(const hvColRGB<T> &rgb, T scal)
	{
	double cx = 0.412453 * (double)rgb.RED()/(double)scal ;
	cx += 0.357580 * (double)rgb.GREEN()/(double)scal ;
	cx += 0.180423 * (double)rgb.BLUE()/(double)scal ;
	if (cx>1.0) cx=1.0;

	double cy = 0.212671 * (double)rgb.RED()/(double)scal ;
	cy += 0.715160 * (double)rgb.GREEN()/(double)scal ;
	cy += 0.072169 * (double)rgb.BLUE()/(double)scal ;
	if (cy>1.0) cy=1.0;

	double cz = 0.019334 * (double)rgb.RED()/(double)scal ;
	cz += 0.119193 * (double)rgb.GREEN()/(double)scal ;
	cz += 0.950227 * (double)rgb.BLUE()/(double)scal ;
	if (cz>1.0) cz=1.0;

	*this = hvColRGB((T)(cx*(double)scal), (T)(cy*(double)scal), (T)(cz*(double)scal)); 
	}
	
	void fromxyz(const hvColRGB<T> &rgb, T scal)
	{
	double cx = 3.240479 * (double)rgb.RED()/(double)scal ;
	cx += -1.537150 * (double)rgb.GREEN()/(double)scal ;
	cx += -0.498535 * (double)rgb.BLUE()/(double)scal ;
	if (cx>1.0) cx=1.0; if (cx<0.0) cx=0.0;

	double cy = -0.969256 * (double)rgb.RED()/(double)scal ;
	cy += 1.875992 * (double)rgb.GREEN()/(double)scal ;
	cy += 0.041556 * (double)rgb.BLUE()/(double)scal ;
	if (cy>1.0) cy=1.0; if (cy<0.0) cy=0.0;

	double cz = 0.055648 * (double)rgb.RED()/(double)scal ;
	cz += -0.204043 * (double)rgb.GREEN()/(double)scal ;
	cz += 1.057311 * (double)rgb.BLUE()/(double)scal ;
	if (cz>1.0) cz=1.0; if (cz<0.0) cz=0.0;

	*this = hvColRGB((T)(cx*(double)scal), (T)(cy*(double)scal), (T)(cz*(double)scal)); 
	}

	void toLuv(const hvColRGB<T> &rgb, T scal)
	{
		hvColRGB<T> xyz; xyz.toxyz(rgb,scal);
		double X=(double)xyz.RED()/(double)scal;
		double Y=(double)xyz.GREEN()/(double)scal;
		double Z=(double)xyz.BLUE()/(double)scal;
		double L,u,v;  /*!< L in [0,100], u in [-83,175], v in [-134,108] */

		double Ydiv = Y/Yn ;
		if (Ydiv > 0.008856)
			L = 116.0 * pow(Ydiv,1.0/3.0) - 16.0 ;
		else // near black
			L = 903.3 * Ydiv ;
		double den = X + 15.0 * Y + 3.0 * Z ;
		double u1 = (4.0 * X) / den ;
		double v1 = (9.0 * Y) / den ;
		u = 13.0*L * (u1 - un) ;
		v = 13.0*L * (v1 - vn) ;

		if (L<0.0) L=0.0; if (L>100.0) L=100.0;
		if (u<-83.0) u=-83.0; if (u>175.0) u=175.0;
		if (v<-134.0) u=-134.0; if (u>108.0) u=108.0;

		*this = hvColRGB((T)((L/100.0)*(double)scal), (T)((u+83.0)/(175.0+83.0)*(double)scal), (T)((v+134.0)/(108.0+134.0)*(double)scal)); 
	}

	void fromLuv(const hvColRGB<T> &Luv, T scal)
	{
		double L = (double)Luv.RED()/(double)scal*100.0 ;
		double u = (double)Luv.GREEN()/(double)scal*(175.0+83.0)-83.0 ;
		double v = (double)Luv.BLUE()/(double)scal*(108.0+134.0)-134.0 ;

		double X,Y,Z;

		if (L > 8.0)
			Y = pow(((L+16.0) / 116.0),3.0) ;
		else // near black
			Y = Yn * L / 903.3 ;

		double den = 13.0 * L ;
		double u1 = u/den + un ;
		double v1 = v/den + vn ;
		den = 4.0*v1 ;
		X = Y * 9.0 * u1 / den ;
		Z = Y * (12.0 - 3.0*u1 - 20.0*v1) / den ;

		if (X<0.0) X=0.0; if (X>1.0) X=1.0;
		if (Y<0.0) Y=0.0; if (Y>1.0) Y=1.0;
		if (Z<0.0) Z=0.0; if (Z>1.0) Z=1.0;

		hvColRGB<T> xyz = hvColRGB((T)(X*(double)scal), (T)(Y*(double)scal), (T)(Z*(double)scal)); 
		fromxyz(xyz,scal);
	}

	void clamp(T min, T max) 
	{
		T r = hvVec3<T>::X(); if (r<min) r=min; else if (r>max) r=max;
		T g = hvVec3<T>::Y(); if (g<min) g=min; else if (g>max) g=max;
		T b = hvVec3<T>::Z(); if (b<min) b=min; else if (b>max) b=max;
		*this = hvColRGB(r, g, b);
	}
	void gammaNormalized(const hvColRGB<T> &max, T scal, double power) 
	{
		double r = (double)hvVec3<T>::X()/(double)max.X(); if (r<0.0) r=0.0; else if (r>1.0) r=1.0;
		double g = (double)hvVec3<T>::Y()/(double)max.Y(); if (g<0.0) g=0.0; else if (g>1.0) g=1.0;
		double b = (double)hvVec3<T>::Z()/(double)max.Z(); if (b<0.0) b=0.0; else if (b>1.0) b=1.0;
		r = (double)scal*pow(r,power);
		g = (double)scal*pow(g,power);
		b = (double)scal*pow(b,power);
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void gammaNormalizedMax(const hvColRGB<T> &max, T scal, double power) 
	{
		double r = (double)hvVec3<T>::X()/(double)max.X(); if (r<0.0) r=0.0; else if (r>1.0) r=1.0;
		double g = (double)hvVec3<T>::Y()/(double)max.Y(); if (g<0.0) g=0.0; else if (g>1.0) g=1.0;
		double b = (double)hvVec3<T>::Z()/(double)max.Z(); if (b<0.0) b=0.0; else if (b>1.0) b=1.0;
		r = (double)scal*pow(r,power);
		g = (double)scal*pow(g,power);
		b = (double)scal*pow(b,power);
		//if (r>=g && r>=b) *this = hvColRGB((T)r, (T)r, (T)r);
		//else if (g>=r && g>=b) *this = hvColRGB((T)g, (T)g, (T)g);
		//else *this = hvColRGB((T)b, (T)b, (T)b);
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void gammaClampedMax(const hvColRGB<T> &max, T scal, double thresh)
	{
		double r = (double)hvVec3<T>::X() / (double)max.X(); if (r<0.0) r = 0.0; else if (r>1.0) r = 1.0;
		double g = (double)hvVec3<T>::Y() / (double)max.Y(); if (g<0.0) g = 0.0; else if (g>1.0) g = 1.0;
		double b = (double)hvVec3<T>::Z() / (double)max.Z(); if (b<0.0) b = 0.0; else if (b>1.0) b = 1.0;
		if (r < thresh) r = 0.0; else r = (double)scal*r;
		if (g < thresh) g = 0.0; else g = (double)scal*g;
		if (b < thresh) b = 0.0; else b = (double)scal*b;
		//if (r>=g && r>=b) *this = hvColRGB((T)r, (T)r, (T)r);
		//else if (g>=r && g>=b) *this = hvColRGB((T)g, (T)g, (T)g);
		//else *this = hvColRGB((T)b, (T)b, (T)b);
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void gamma(T scal, double power)
	{
		double r = (double)hvVec3<T>::X()/(double)scal; if (r<0.0) r=0.0; else if (r>1.0) r=1.0;
		double g = (double)hvVec3<T>::Y()/(double)scal; if (g<0.0) g=0.0; else if (g>1.0) g=1.0;
		double b = (double)hvVec3<T>::Z()/(double)scal; if (b<0.0) b=0.0; else if (b>1.0) b=1.0;
		r = (double)scal*pow(r,power);
		g = (double)scal*pow(g,power);
		b = (double)scal*pow(b,power);
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void blend(const hvColRGB<T> &c1, const hvColRGB<T> &c2, T scal, double alpha) 
	{
		double r = (double)scal*((double)c1.RED()/(double)scal*alpha+(double)c2.RED()/(double)scal*(1.0-alpha));
		double g = (double)scal*((double)c1.GREEN()/(double)scal*alpha+(double)c2.GREEN()/(double)scal*(1.0-alpha));
		double b = (double)scal*((double)c1.BLUE()/(double)scal*alpha+(double)c2.BLUE()/(double)scal*(1.0-alpha));
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void blend(const hvColRGB<T> &c1, const hvColRGB<T> &c2, double alpha) 
	{
		double r = (double)c1.RED()*alpha+(double)c2.RED()*(1.0-alpha);
		double g = (double)c1.GREEN()*alpha+(double)c2.GREEN()*(1.0-alpha);
		double b = (double)c1.BLUE()*alpha+(double)c2.BLUE()*(1.0-alpha);
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void add(const hvColRGB<T> &a, const hvColRGB<T> &b) { hvVec3<T>::add((hvVec3<T>)a,(hvVec3<T>)b); }
	hvColRGB<T> operator+(const hvColRGB<T> &b) const { hvVec3<T>::operator+((hvVec3<T>)b); return *this; }
	void operator+=(const hvColRGB<T> &b) { hvVec3<T>::operator+=((hvVec3<T>)b); }
	void sub(const hvColRGB<T> &a, const hvColRGB<T> &b) { hvVec3<T>::sub((hvVec3<T>)a,(hvVec3<T>)b); }
	hvColRGB<T> operator-(const hvColRGB<T> &b) const { hvVec3<T>::operator-((hvVec3<T>)b); return *this; }
	void operator-=(const hvColRGB<T> &b) { hvVec3<T>::operator-=((hvVec3<T>)b); }
	void subabs(const hvColRGB<T> &a, const hvColRGB<T> &b)
	{
		*this = hvColRGB( a.RED()<b.RED()?b.RED()-a.RED():a.RED()-b.RED(), 
			a.GREEN()<b.GREEN()?b.GREEN()-a.GREEN():a.GREEN()-b.GREEN(),
			a.BLUE()<b.BLUE()?b.BLUE()-a.BLUE():a.BLUE()-b.BLUE()
			);
	}
	void mult(const hvColRGB<T> &a, const hvColRGB<T> &b) { hvVec3<T>::mult((hvVec3<T>)a,(hvVec3<T>)b); }
	hvColRGB<T> operator*(const hvColRGB<T> &b) const { hvVec3<T>::operator*((hvVec3<T>)b); return *this; }
	void operator*=(const hvColRGB<T> &b) { hvVec3<T>::operator*=((hvVec3<T>)b); }
	template <class X> void operator*=(X v) { hvVec3<T>::x= (T)((X)hvVec3<T>::x*v); hvVec3<T>::y=(T)((X)hvVec3<T>::y*v); hvVec3<T>::z=(T)((X)hvVec3<T>::z*v); }
	void div(const hvColRGB<T> &a, const hvColRGB<T> &b) { hvVec3<T>::div((hvVec3<T>)a,(hvVec3<T>)b); }
	hvColRGB<T> operator/(const hvColRGB<T> &b) const { hvVec3<T>::operator/((hvVec3<T>)b); return *this; }
	void operator/=(const hvColRGB<T> &b) { hvVec3<T>::operator/=((hvVec3<T>)b); }
	template <class X> void operator/=(X v) { hvVec3<T>::x= (T)((X)hvVec3<T>::x/v); hvVec3<T>::y=(T)((X)hvVec3<T>::y/v); hvVec3<T>::z=(T)((X)hvVec3<T>::z/v); }
	//void operator/=(T v) { x= (T)(x/v); y=(T)(y/v); z=(T)(z/v); }
	void scale(const hvColRGB<T> &a, T k) { hvVec3<T>::scale((hvVec3<T>)a,k); }
	template <class X> void scale(X v) { hvVec3<T>::x= (T)((X)hvVec3<T>::x*v); hvVec3<T>::y=(T)((X)hvVec3<T>::y*v); hvVec3<T>::z=(T)((X)hvVec3<T>::z*v); }
	void square() { hvVec3<T>::square(); }
	bool equals(const hvColRGB<T> &a) const { return hvVec3<T>::equals((hvVec3<T>)a); }
	bool operator==(const hvColRGB<T> &a) const { return hvVec3<T>::equals((hvVec3<T>)a); }
	void difference(const hvColRGB<T> &c1, const hvColRGB<T> &c2) 
	{
		double r = (double)c1.RED()-(double)c2.RED(); if (r<0.0) r=-r;
		double g = (double)c1.GREEN()-(double)c2.GREEN(); if (g<0.0) g=-g;
		double b = (double)c1.BLUE()-(double)c2.BLUE(); if (b<0.0) b=-b;
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void difference(const hvColRGB<T> &c1, const hvColRGB<T> &c2, double scale, double offset) 
	{
		double r = (double)c1.RED()-(double)c2.RED()+offset; 
		if (r<0.0) r=0.0; else if (r>=2.0*scale) r = 2.0*scale-1.0;
		double g = (double)c1.GREEN()-(double)c2.GREEN()+offset; 
		if (g<0.0) g=0.0; else if (g>=2.0*scale) g = 2.0*scale-1.0;
		double b = (double)c1.BLUE()-(double)c2.BLUE()+offset;
		if (b<0.0) b=0.0; else if (b>=2.0*scale) b = 2.0*scale-1.0;
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	double squaredDifferenceChroma(const hvColRGB<T> &c2) 
	{
		hvColRGB<unsigned char> cl1; cl1.toLuv(*this, 255.0);
		hvColRGB<unsigned char> cl2; cl2.toLuv(c2, 255.0);

		double g = (double)cl1.GREEN()-(double)cl2.GREEN(); 
		double b = (double)cl1.BLUE()-(double)cl2.BLUE(); 
		return g*g+b*b;
	}
	double squaredDifference(const hvColRGB<T> &c2) 
	{
		double r = (double)RED()-(double)c2.RED(); 
		double g = (double)GREEN()-(double)c2.GREEN(); 
		double b = (double)BLUE()-(double)c2.BLUE(); 
		return r*r+g*g+b*b;
	}
	double squaredDifferenceNorm(double scal, const hvColRGB<T> &c2) 
	{
		double r = (double)RED()/scal-(double)c2.RED()/scal; 
		double g = (double)GREEN()/scal-(double)c2.GREEN()/scal; 
		double b = (double)BLUE()/scal-(double)c2.BLUE()/scal; 
		return r*r+g*g+b*b;
	}
	void keepMin(const hvColRGB<T> &v1, const hvColRGB<T> &v2) { hvVec3<T>::keepMin((hvVec3<T>)v1, (hvVec3<T>)v2); }
	void keepMax(const hvColRGB<T> &v1, const hvColRGB<T> &v2) { hvVec3<T>::keepMax((hvVec3<T>)v1, (hvVec3<T>)v2); }
	void normalize(const hvColRGB<T> &min, const hvColRGB<T> &max, double scal) 
	{ 
		double r = (double)RED(); r = scal*(r-(double)min.RED())/((double)max.RED()-(double)min.RED());
		double g = (double)GREEN(); g = scal*(g-(double)min.GREEN())/((double)max.GREEN()-(double)min.GREEN());
		double b = (double)BLUE(); b = scal*(b-(double)min.BLUE())/((double)max.BLUE()-(double)min.BLUE());
		*this = hvColRGB((T)r, (T)g, (T)b);
	}
	void mean(int n, hvColRGB<T> cc[])
	{
		double r=0.0, g=0.0, b=0.0;
		for (int i=0; i<n; i++)
		{
			r += (double)cc[i].RED();
			g += (double)cc[i].GREEN();
			b += (double)cc[i].BLUE();
		}
		*this = hvColRGB((T)(r/(double)n), (T)(g/(double)n), (T)(b/(double)n));
	}

	//T maxCoord() const { return hvVec3<T>::maxCoord(); }
};

template <class T> class hvColRGBA : public hvColRGB<T>  
{
protected:
	T	a;
public:
	hvColRGBA<T>() : hvColRGB<T>(),a(T(0)) { }
	hvColRGBA<T>(T r, T g, T b) : hvColRGB<T>(r,g,b),a(T(0)) {   }
	hvColRGBA<T>(T r, T g, T b, T alpha) : hvColRGB<T>(r,g,b), a(alpha) {  }
	hvColRGBA<T>(T l) : hvColRGB<T>(l),a(T(0)) {  }
	hvColRGBA<T>(const hvColRGB<T> &c, T alpha) : hvColRGB<T>(c), a(alpha) {  }

	//template <class X> hvColRGBA<T>(const hvColRGBA<X> &c) { *this = hvColRGB<T>(hvColRGB<X>(c)); a=T(c.a); }
	T ALPHA() const { return a; }
	void setAlpha(T aa) { a = aa; }

	void operator*=(const hvColRGBA<T> &b) { hvVec3<T>::operator*=((hvVec3<T>)b); }
	void operator/=(const hvColRGBA<T> &b) { hvVec3<T>::operator/=((hvVec3<T>)b); }

	void interpolate(const hvColRGBA<T> &c1, const hvColRGBA<T> &c2, double alpha) 
	{
		double r = (double)c1.RED()*(1.0-alpha)+(double)c2.RED()*alpha;
		double g = (double)c1.GREEN()*(1.0-alpha)+(double)c2.GREEN()*alpha;
		double b = (double)c1.BLUE()*(1.0-alpha)+(double)c2.BLUE()*alpha;
		double a = (double)c1.ALPHA()*(1.0-alpha)+(double)c2.ALPHA()*alpha;
		*this = hvColRGBA((T)r, (T)g, (T)b, (T)a);
	}
	void mix(const hvColRGBA<T> &c1, const hvColRGBA<T> &c2, T scal, double alpha) 
	{
		double r = (double)scal*((double)c1.RED()/(double)scal*alpha+(double)c2.RED()/(double)scal*(1.0-alpha));
		double g = (double)scal*((double)c1.GREEN()/(double)scal*alpha+(double)c2.GREEN()/(double)scal*(1.0-alpha));
		double b = (double)scal*((double)c1.BLUE()/(double)scal*alpha+(double)c2.BLUE()/(double)scal*(1.0-alpha));
		double aa = (double)scal*((double)c1.ALPHA()/(double)scal*alpha+(double)c2.ALPHA()/(double)scal*(1.0-alpha));
		*this = hvColRGBA((T)r, (T)g, (T)b, (T)aa);
	}
	void mix(const hvColRGBA<T> &c1, const hvColRGBA<T> &c2, double alpha) 
	{
		double r = (double)c1.RED()*alpha+(double)c2.RED()*(1.0-alpha);
		double g = (double)c1.GREEN()*alpha+(double)c2.GREEN()*(1.0-alpha);
		double b = (double)c1.BLUE()*alpha+(double)c2.BLUE()*(1.0-alpha);
		double aa = (double)c1.ALPHA()*alpha+(double)c2.ALPHA()*(1.0-alpha);
		*this = hvColRGBA((T)r, (T)g, (T)b, (T)aa);
	}

};

}

#endif // !defined(AFX_COLOR_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
