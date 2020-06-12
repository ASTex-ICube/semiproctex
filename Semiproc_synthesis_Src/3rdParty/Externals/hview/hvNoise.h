/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

#ifndef _HV_NOISE_H_
#define _HV_NOISE_H_



//#define SINC


////////////////////////////////////////////
/*
Noise: is a function returning a pseudo-random value between -1 and 1.
*/ 
////////////////////////////////////////////
////////////////////////////////////////////

#include "hvLinearTransform3.h"
#include "hvPair.h"

#define INTERPOL(s,v0,v1) 	((v0)+(s)*((v1)-(v0)))
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )
#define F2 0.366025403 // F2 = 0.5*(sqrt(3.0)-1.0)
#define G2 0.211324865 // G2 = (3.0-Math.sqrt(3.0))/6.0
// Simple skewing factors for the simplex 3D case
#define F3 0.333333333
#define G3 0.166666667
// The skewing and unskewing factors are hairy again for the 4D case
#define F4 0.309016994 // F4 = (Math.sqrt(5.0)-1.0)/4.0
#define G4 0.138196601 // G4 = (5.0-Math.sqrt(5.0))/20.0

namespace hview {

const int MAX_NOISE_RAND=1024;
const int WAVELETNOISE_SIZE=512;

typedef struct { double x, y, z; } hvnoise_vec;

class hvNoise
{
public:

static double rnd_tab[MAX_NOISE_RAND];
static unsigned int P[MAX_NOISE_RAND];
static hvnoise_vec G[MAX_NOISE_RAND];
static unsigned char perm[512];

static int phi(int x)
{
x = x%MAX_NOISE_RAND; if (x<0) x=x+MAX_NOISE_RAND;
return(P[x]);
}

static void gamma(int i,int j,int k,hvVec3<double> *v,double *val)
{
int index;

index=phi(i+phi(j+phi(k)));
*v = hvVec3<double>(G[index].x,G[index].y,G[index].z) ;
*val = rnd_tab[index];
}

static double omega(int i,int j,int k,double x,double y,double z)
{
register double u,v,w;
double val;
hvVec3<double> vec,vec2;

u = (x>0.0?x:-x); v=(y>0.0?y:-y); w=(z>0.0?z:-z);
u= (u<1.0)? u*u*(2.0*u-3.0)+1.0 : 0.0;
v= (v<1.0)? v*v*(2.0*v-3.0)+1.0 : 0.0;
w= (w<1.0)? w*w*(2.0*w-3.0)+1.0 : 0.0;
vec = hvVec3<double>(x,y,z);
gamma(i,j,k,&vec2,&val);
return(u*v*w*(vec.dot(vec2)+val));
}

static double  grad1( int hash, double x ) 
{
    int h = hash & 15;
    double grad = 1.0f + (h & 7);   // Gradient value 1.0, 2.0, ..., 8.0
    if (h&8) grad = -grad;         // Set a random sign for the gradient
    return ( grad * x );           // Multiply the gradient with the distance
}

static double  grad2( int hash, double x, double y ) 
{
    int h = hash & 7;      // Convert low 3 bits of hash code
    double u = h<4 ? x : y;  // into 8 simple gradient directions,
    double v = h<4 ? y : x;  // and compute the dot product with (x,y).
    return ((h&1)? -u : u) + ((h&2)? -2.0f*v : 2.0f*v); 
}

static double  grad3( int hash, double x, double y , double z ) 
{
    int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
    double u = h<8 ? x : y; // gradient directions, and compute dot product.
    double v = h<4 ? y : h==12||h==14 ? x : z; // Fix repeats at h = 12 to 15
    return ((h&1)? -u : u) + ((h&2)? -v : v);
}

static double  grad4( int hash, double x, double y, double z, double t ) 
{
    int h = hash & 31;      // Convert low 5 bits of hash code into 32 simple
    double u = h<24 ? x : y; // gradient directions, and compute dot product.
    double v = h<16 ? y : z;
    double w = h<8 ? z : t;
    return ((h&1)? -u : u) + ((h&2)? -v : v) + ((h&4)? -w : w);
}

public:

	static double gnoise(double x,double y,double z)
	{
	int i,j,k;
	double ix,iy,iz;

	ix=floor(x); iy=floor(y); iz=floor(z);
	i=(int)ix; j=(int)iy; k=(int)iz;
	double vv= 
	omega(i,j,k,x-ix,y-iy,z-iz)+
	omega(i,j,k+1,x-ix,y-iy,z-(iz+1.0))+
	omega(i,j+1,k,x-ix,y-(iy+1.0),z-iz)+
	omega(i,j+1,k+1,x-ix,y-(iy+1.0),z-(iz+1.0))+
	omega(i+1,j,k,x-(ix+1.0),y-iy,z-iz)+
	omega(i+1,j,k+1,x-(ix+1.0),y-iy,z-(iz+1.0))+
	omega(i+1,j+1,k,x-(ix+1.0),y-(iy+1.0),z-iz)+
	omega(i+1,j+1,k+1,x-(ix+1.0),y-(iy+1.0),z-(iz+1.0));
	return vv/4.0;

	}

	static double inoise(int ix,int iy, int iz)
	{
	return(rnd_tab[(phi(ix)+3*phi(iy)+5*phi(iz))%MAX_NOISE_RAND]);
	}

	static double cnoise2D(double x, double y)
	{
		double vx0, vx1, vy0, vy1;
		int ix, iy;
		double sx, sy;
		double rt;

		ix = (int)floor(x); x -= ix;
		iy = (int)floor(y); y -= iy;

		sx = x*x*(3.0 - 2.0*x);
		sy = y*y*(3.0 - 2.0*y);

		vy0 = inoise(ix, iy, 0);
		vy1 = inoise(ix, iy + 1, 0);
		vx0 = INTERPOL(sy, vy0, vy1);
		vy0 = inoise(ix + 1, iy, 0);
		vy1 = inoise(ix + 1, iy + 1, 0);
		vx1 = INTERPOL(sy, vy0, vy1);
		rt = INTERPOL(sx, vx0, vx1);
		return(rt);
	}

	static double cnoise(double x,double y,double z)
	{
	double vx0,vx1,vy0,vy1,vz0,vz1;
	int ix,iy,iz;
	double sx,sy,sz;
	double rt;

	ix = (int)floor(x); x -= ix;
	iy = (int)floor(y); y -= iy;
	iz = (int)floor(z); z -= iz;

	sx = x*x*(3.0-2.0*x);
	sy = y*y*(3.0-2.0*y);
	sz = z*z*(3.0-2.0*z);

	vz0 = inoise(ix,iy,iz);
	vz1 = inoise(ix,iy,iz+1);
	vy0 = INTERPOL(sz,vz0,vz1);
	vz0 = inoise(ix,iy+1,iz);
	vz1 = inoise(ix,iy+1,iz+1);
	vy1 = INTERPOL(sz,vz0,vz1);
	vx0 = INTERPOL(sy,vy0,vy1);
	vz0 = inoise(ix+1,iy,iz);
	vz1 = inoise(ix+1,iy,iz+1);
	vy0 = INTERPOL(sz,vz0,vz1);
	vz0 = inoise(ix+1,iy+1,iz);
	vz1 = inoise(ix+1,iy+1,iz+1);
	vy1 = INTERPOL(sz,vz0,vz1);
	vx1 = INTERPOL(sy,vy0,vy1);
	rt = INTERPOL(sx,vx0,vx1);
	return(rt);
	}

	static double turbulence(double x,double y,double z,double pix)
	{ 
	double t,scale,n;

	t=0.0; scale=1.0;
	while(scale>pix) { n=scale*gnoise(x/scale,y/scale,z/scale); t+=(n>0.0?n:-n); scale/=2.0; }
	return(0.8*t);
	}
	static double turbulence(double x, double y, double z, float ampli[], int nn)
	{
		double t, scale, n;

		t = 0.0; scale = 1.0;
		for (int i = 0; i<nn; i++) {
			n = ampli[i]*gnoise(x / scale, y / scale, z / scale);
			t += (n>0.0 ? n : -n); scale /= 3.0;
		}
		return(t);
	}
	static double turbulence(double x, double y, double z, double power, int nn)
	{
		double t, scale, n;

		t = 0.0; scale = 1.0;
		for (int i=0; i<nn; i++) { n = scale*gnoise(x / scale, y / scale, z / scale); 
		t += (n>0.0 ? n : -n); scale /= power; }
		return(0.8*t);
	}
	hvNoise() { }
	virtual int dimension() const { return 2; }
	virtual double operator()(double ind[]) const { return hvNoise::turbulence(ind[0], ind[1], 5.78, 0.000001); }
	virtual double get(double ind[]) const { return hvNoise::turbulence(ind[0], ind[1], 5.78, 0.000001); }




//static unsigned int seed;
static void seeding(unsigned int x, unsigned int y, unsigned int z, unsigned int &seed )
//{ seed=x%unsigned int(1024)+(y%unsigned int(1024))*unsigned int(1024)+(z%unsigned int(1024))*unsigned int(1024*1024); }
{ seed=phi(x+phi(y+phi(z)))%(unsigned int)(1<<15)+(phi(3*x+phi(4*y+phi(z)))%(unsigned int)(1<<15))*(unsigned int)(1<<15); }
static float next(unsigned int &seed) { seed *= (unsigned int)(3039177861); float res=(float)((double(seed)/4294967296.0)*2.0-1.0); return res; }





  //////////////////////////////////////////
  // SINCNOISE

static float sinc(float x)
{
	if (x==0.0f) return 1.0f; else if (x<-8.0 || x>8.0) return 0.0;
	return (float)sin(M_PI*x)/M_PI/x;
}


static int Mod(int x, int n) {int m=x%n; return (m<0) ? m+n : m;}
#define ARAD 16





};

//unsigned int hvNoise::seed=0;

}

#endif // _HV_NOISE_H_
