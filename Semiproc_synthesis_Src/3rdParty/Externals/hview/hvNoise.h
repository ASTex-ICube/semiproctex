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











#define MAX_NEIGH_CELLS 9

typedef enum { REGULAR, IRREGULAR, CROSS, BISQUARE, IRREGULARX, IRREGULARY, APAVEMENT } tilingtype;





static int pave(float xp, float yp,
	// pavement parameters
	int Nx, float randx, float randy,
	float cx[MAX_NEIGH_CELLS], float cy[MAX_NEIGH_CELLS], float dx[MAX_NEIGH_CELLS], float dy[MAX_NEIGH_CELLS])
{
	unsigned int seed;

	int i, j;
	int nc = 0;
	float x = xp;
	float y = yp;
	int ix = (int)floor(x); float xx = x - (float)ix;
	int iy = (int)floor(y); float yy = y - (float)iy;
	for (j = -1; j <= 1; j++)
		for (i = -1; i <= 1; i++)
		{
			float rxi, rxs, ryi, rys;
			float ivpx = (float)ix + (float)i;
			float ivpy = (float)iy + (float)j;
			float decalx = (float)((int)ivpy%Nx) / (float)Nx;
			seeding((unsigned int)(ivpx + 5), (unsigned int)(ivpy + 10), 0, seed);
			rxi = next(seed)*randx*0.5f; //printf("rx %d,%d=%g, ", (unsigned int)(ivpx + 5), (unsigned int)(ivpy + 10), rx);
			seeding(3, (unsigned int)(ivpy + 10), 0, seed);
			ryi = next(seed)*randy*0.5f;
			seeding((unsigned int)(ivpx + 1 + 5), (unsigned int)(ivpy + 10), 0, seed);
			rxs = next(seed)*randx*0.5f; //printf("rxs %d,%d=%g\n", (unsigned int)(ivpx +1 + 5), (unsigned int)(ivpy + 10), rxs);
			seeding(3, (unsigned int)(ivpy + 1 + 10), 0, seed);
			rys = next(seed)*randy*0.5f;

			dx[nc] = (0.5f*(rxs + 1.0f - rxi));
			dy[nc] = 0.5f*(rys + 1.0f - ryi);
			cx[nc] = (ivpx + decalx + rxi + dx[nc]);
			cy[nc] = ivpy + ryi + dy[nc];
			nc++;
		}
	return nc;
}

static int paveb(float x, float y,
	// pavement parameters
	float cx[MAX_NEIGH_CELLS], float cy[MAX_NEIGH_CELLS], float dx[MAX_NEIGH_CELLS], float dy[MAX_NEIGH_CELLS])
{
	int i, j;
	int nc = 0;
	int ii, jj;
	int ix = (int)floor(x); float xx = x - (float)ix;
	int iy = (int)floor(y); float yy = y - (float)iy;
	int qx = (int)(xx*(int)5);
	int qy = (int)(yy*(int)5);
	for (i = 0; i < 3; i++)  for (j = 0; j < 3; j++)
	{
		if (qx >= -2 + i * 2 + j && qx <= -2 + i * 2 + 1 + j
			&& qy >= 1 - i + 2 * j && qy <= 1 - i + 2 * j + 1)
		{
			for (ii = 0; ii <= 2; ii++) for (jj = 0; jj <= 2; jj++)
			{
				if (ii == 1 || jj == 1)
				{
					int rx = -2 + i * 2 + j - 3 + ii * 2 + jj;
					int ry = 1 - i + 2 * j - 1 + jj * 2 - ii;
					dx[nc] = 1.0 / 5.0; dy[nc] = 1.0 / 5.0;
					cx[nc] = (float)ix + (float)rx / 5.0 + 1.0 / 5.0;
					cy[nc] = (float)iy + (float)ry / 5.0 + 1.0 / 5.0;
					nc++;
				}
			}

			int rx = -2 + i * 2 + j;
			int ry = 1 - i + 2 * j;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(rx - 1) / 5.0 + 0.5 / 5.0;
			cy[nc] = (float)iy + (float)ry / 5.0 + 0.5 / 5.0;
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)rx / 5.0 + 0.5 / 5.0;
			cy[nc] = (float)iy + (float)(ry + 2) / 5.0 + 0.5 / 5.0;
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(rx + 2) / 5.0 + 0.5 / 5.0;
			cy[nc] = (float)iy + (float)(ry + 1) / 5.0 + 0.5 / 5.0;
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(rx + 1) / 5.0 + 0.5 / 5.0;
			cy[nc] = (float)iy + (float)(ry - 1) / 5.0 + 0.5 / 5.0;
			nc++;
			//printf("anc=%d\n", nc);
			return nc;
		}
	}

	for (i = 0; i < 3; i++)  for (j = 0; j < 2; j++)
	{
		if (qx == i * 2 + j && qy == 2 + 2 * j - i)
		{
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)qx / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)qy / 5.0 + dy[nc];
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(qx - 2) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy + 1) / 5.0 + dy[nc];
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(qx + 1) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy + 2) / 5.0 + dy[nc];
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(qx - 1) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 2) / 5.0 + dy[nc];
			nc++;
			dx[nc] = 0.5 / 5.0; dy[nc] = 0.5 / 5.0;
			cx[nc] = (float)ix + (float)(qx + 2) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 1) / 5.0 + dy[nc];
			nc++;

			dx[nc] = 1.0 / 5.0; dy[nc] = 1.0 / 5.0;
			cx[nc] = (float)ix + (float)(qx - 2) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 1) / 5.0 + dy[nc];
			nc++;
			dx[nc] = 1.0 / 5.0; dy[nc] = 1.0 / 5.0;
			cx[nc] = (float)ix + (float)(qx - 1) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy + 1) / 5.0 + dy[nc];
			nc++;
			dx[nc] = 1.0 / 5.0; dy[nc] = 1.0 / 5.0;
			cx[nc] = (float)ix + (float)(qx + 1) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy) / 5.0 + dy[nc];
			nc++;
			dx[nc] = 1.0 / 5.0; dy[nc] = 1.0 / 5.0;
			cx[nc] = (float)ix + (float)(qx) / 5.0 + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 2) / 5.0 + dy[nc];
			nc++;
			//printf("bnc=%d\n", nc);
			return nc;
		}
	}

	printf("error in paveb!!!\n");

	return 0;
}


static int pavec(float x, float y,
	// pavement parameters
	float cx[9], float cy[9], float dx[9], float dy[9])
{
	float sx[9][9] = { {1.0, 1.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 1.0 },
						{1.5,0.5,0.5,1.0,0.5,1.0,0.5,1.0,1.0},
						{0.5,1.5,1.0,1.0,0.5,0.5,1.5,1.0,1.0},
						{1.0,1.0,1.5,0.5,1.5,1.0,0.5,0.5,0.5},
						{0.5,1.0,1.0,0.5,0.5,1.0,1.5,1.5,1.5},

						{0.5,1.0,0.5,0.5,1.5,1.0,1.0,1.5,1.0},
						{1.0,0.5,1.0,0.5,0.5,1.5,1.0,1.0,1.0},
						{0.5,1.0,1.0,1.0,1.5,0.5,0.5,0.5,1.0},
						{1.0,1.0,1.5,0.5,1.5,1.0,0.5,0.5,0.5}
	};
	float sy[9][9] = { {1.0, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 1.0, 0.5 },
						{0.5,1.0,1.0,0.5,1.0,0.5,0.5,1.0,0.5},
						{1.0,0.5,0.5,1.0,0.5,1.0,0.5,0.5,1.0},
						{0.5,1.0,0.5,1.0,0.5,1.0,1.0,0.5,1.0},
						{0.5,1.0,0.5,1.0,1.0,0.5,0.5,0.5,0.5},

						{1.0,1.0,0.5,1.0,0.5,0.5,1.0,0.5,0.5},
						{0.5,1.0,1.0,0.5,1.0,0.5,0.5,0.5,0.5},
						{1.0,0.5,1.0,0.5,0.5,1.0,0.5,1.0,0.5},
						{0.5,1.0,0.5,1.0,0.5,1.0,1.0,0.5,1.0}
	};
	float ddx[9][9] = { { 1.0, 0.0, 3.0, 3.0, 3.0, 3.0, 1.0, 0.0, -1.0 },
						{0.0,-1.0,0.0,1.0,3.0,3.0,3.0,1.0,-1.0},
						{3.0, 0.0,1.0,1.0,3.0,4.0,4.0,3.0,1.0},
						{3.0,1.0,0.0,3.0,4.0,5.0,4.0,3.0,4.0},
						{3.0,1.0,3.0,4.0,3.0,1.0,0.0,4.0,0.0},

						{3.0,1.0,3.0,4.0,4.0,3.0,1.0,0.0,1.0},
						{1.0,0.0,1.0,3.0,3.0,0.0,-1.0,3.0,3.0},
						{0.0,-1.0,1.0,1.0,0.0,-1.0,-1.0,-1.0,-1.0},
						{-1.0,-3.0,-4.0,-1.0,0.0,1.0,0.0,-1.0,-1.0}
	};
	float ddy[9][9] = { { 1.0, 3.0, 3.0, 2.0, 1.0, -1.0, 0.0, 0.0, 2.0 },
						{3.0, 3.0,4.0,4.0,3.0,2.0,1.0,1.0,2.0},
						{3.0, 3.0,4.0,5.0,5.0,4.0,3.0,2.0,1.0},
						{2.0,1.0,3.0,3.0,3.0,1.0,0.0,1.0,4.0},
						{1.0,1.0,2.0,1.0,-1.0,0.0,3.0,3.0,-1.0},

						{-1.0,1.0,1.0,0.0,-1.0,-2.0,-3.0,-1.0,0.0},
						{0.0,0.0,1.0,1.0,-1.0,-1.0,-2.0,-2.0,2.0},
						{0.0,2.0,1.0,0.0,-1.0,-1.0,1.0,3.0,-2.0},
						{2.0,1.0,3.0,3.0,3.0,1.0,0.0,1.0,-1.0}
	};

	int i, j;
	int nc = 0;
	int ii, jj;
	int ix = (int)floor(x); float xx = x - (float)ix;
	int iy = (int)floor(y); float yy = y - (float)iy;
	int qx = (int)(xx*4.0);
	int qy = (int)(yy*4.0);
	int qq = 0;
	if (qx >= 1 && qx <= 2 && qy >= 1 && qy <= 2) qq = 0;
	else if (qx >= 0 && qx <= 2 && qy == 3) qq = 1;
	else if (qx == 3 && qy == 3) qq = 2;
	else if (qx == 3 && qy == 2) qq = 3;
	else if (qx == 3 && qy == 1) qq = 4;
	else if (qx == 3 && qy == 0) qq = 5;
	else if (qx >= 1 && qx <= 2 && qy == 0) qq = 6;
	else if (qx == 0 && qy >= 0 && qy <= 1) qq = 7;
	else if (qx == 0 && qy == 2) qq = 8;
	for (ii = 0; ii < 9; ii++)
	{
		dx[nc] = sx[qq][ii] / 4.0; dy[nc] = sy[qq][ii] / 4.0;
		cx[nc] = (float)ix + ddx[qq][ii] / 4.0 + dx[nc];
		cy[nc] = (float)iy + ddy[qq][ii] / 4.0 + dy[nc];
		nc++;
	}
	return nc;
}


static int paved(float x, float y,
	// pavement parameters
	int Nx,
	float cx[MAX_NEIGH_CELLS], float cy[MAX_NEIGH_CELLS], float dx[MAX_NEIGH_CELLS], float dy[MAX_NEIGH_CELLS])
{
	int i, j;
	int ix = (int)floor(x); float xx = x - (float)ix;
	int iy = (int)floor(y); float yy = y - (float)iy;
	int qx = (int)(xx*(int)(2 * Nx));
	int qy = (int)(yy*(int)(2 * Nx));
	int nc = 0;
	// horizontal
	if ((qx >= qy && qx <= qy + Nx - 1) || (qx >= qy - 2 * Nx && qx <= qy + Nx - 1 - 2 * Nx))
	{
		int rx, ry;
		if (qx >= qy && qx <= qy + Nx - 1) { rx = qy; ry = qy; }
		else { rx = qy - 2 * Nx; ry = qy; }
		for (i = 0; i < 3; i++)
		{
			cx[nc] = (float)ix + ((float)rx + (float)(i - 1) + (float)(Nx) *0.5) / (float)(2 * Nx);
			cy[nc] = (float)iy + ((float)ry + (float)(i - 1) + 0.5) / (float)(2 * Nx);
			dx[nc] = ((float)Nx*0.5) / (float)(2 * Nx);
			dy[nc] = 0.5 / (float)(2 * Nx);

			cx[nc + 1] = (float)ix + ((float)rx + (float)(i - 2) + 0.5) / (float)(2 * Nx);
			cy[nc + 1] = (float)iy + ((float)ry + (float)(i - 1) + (float)(Nx) *0.5) / (float)(2 * Nx);
			dx[nc + 1] = 0.5 / (float)(2 * Nx);
			dy[nc + 1] = ((float)Nx*0.5) / (float)(2 * Nx);

			cx[nc + 2] = (float)ix + ((float)rx + (float)(i - 1) + (float)Nx + 0.5) / (float)(2 * Nx);
			cy[nc + 2] = (float)iy + ((float)ry + (float)(i)-(float)(Nx)*0.5) / (float)(2 * Nx);
			dx[nc + 2] = 0.5 / (float)(2 * Nx);
			dy[nc + 2] = ((float)Nx*0.5) / (float)(2 * Nx);

			nc += 3;
		}
	}
	// vertical

	else
	{
		int rx, ry;
		if (qy >= qx + 1 && qy <= qx + 1 + Nx - 1)
		{
			rx = qx;
			ry = qx + 1;
		}
		else
		{
			rx = qx;
			ry = qx + 1 - 2 * Nx;
		}
		for (i = 0; i < 3; i++)
		{
			cx[nc] = (float)ix + ((float)rx + (float)(i - 1) + 0.5) / (float)(2 * Nx);
			cy[nc] = (float)iy + ((float)ry + (float)(i - 1) + (float)(Nx) *0.5) / (float)(2 * Nx);
			dx[nc] = 0.5 / (float)(2 * Nx);
			dy[nc] = ((float)Nx*0.5) / (float)(2 * Nx);

			cx[nc + 1] = (float)ix + ((float)rx + (float)(i - 1) + (float)(Nx)*0.5) / (float)(2 * Nx);
			cy[nc + 1] = (float)iy + ((float)ry + (float)(i - 2) + 0.5) / (float)(2 * Nx);
			dx[nc + 1] = ((float)Nx*0.5) / (float)(2 * Nx);
			dy[nc + 1] = 0.5 / (float)(2 * Nx);

			cx[nc + 2] = (float)ix + ((float)rx + (float)(i - 1) - (float)(Nx)*0.5) / (float)(2 * Nx);
			cy[nc + 2] = (float)iy + ((float)ry + (float)(i - 1) + (float)(Nx - 1) + 0.5) / (float)(2 * Nx);
			dx[nc + 2] = ((float)Nx*0.5) / (float)(2 * Nx);
			dy[nc + 2] = 0.5 / (float)(2 * Nx);

			nc += 3;
		}
	}
	return nc;
}





static int pavement(float x, float y,
	tilingtype tt, int decalx, int Nx,
	float ccx[MAX_NEIGH_CELLS], float ccy[MAX_NEIGH_CELLS], float cdx[MAX_NEIGH_CELLS], float cdy[MAX_NEIGH_CELLS])
{
	switch (tt) {
	case REGULAR: return hvNoise::pave(x, y, decalx, 0.0, 0.0, ccx, ccy, cdx, cdy); break;
	case IRREGULAR: return hvNoise::pave(x, y, decalx, 0.8, 0.8, ccx, ccy, cdx, cdy); break;
	case CROSS: return hvNoise::paved(x, y, Nx, ccx, ccy, cdx, cdy); break;
	case BISQUARE: return hvNoise::paveb(x, y, ccx, ccy, cdx, cdy); break;
	case IRREGULARX: return hvNoise::pave(x, y, decalx, 0.8, 0.0, ccx, ccy, cdx, cdy); break;
	case IRREGULARY: return hvNoise::pave(x, y, decalx, 0.0, 0.8, ccx, ccy, cdx, cdy); break;
	case APAVEMENT: return hvNoise::pavec(x, y, ccx, ccy, cdx, cdy); break;
	default: return hvNoise::pave(x, y, decalx, 0.0, 0.0, ccx, ccy, cdx, cdy); break;
	}
}


static int pointset(
	// point set parameters
	float psubx, float psuby, float jitx, float jity, int nn,
	float ccx[MAX_NEIGH_CELLS], float ccy[MAX_NEIGH_CELLS], float cdx[MAX_NEIGH_CELLS], float cdy[MAX_NEIGH_CELLS],
	float cx[MAX_NEIGH_CELLS * 4], float cy[MAX_NEIGH_CELLS * 4], float ncx[MAX_NEIGH_CELLS * 4], float ncy[MAX_NEIGH_CELLS * 4], float ndx[MAX_NEIGH_CELLS * 4], float ndy[MAX_NEIGH_CELLS * 4])
{
	unsigned int seed;

	int i, j, k;
	int nc = 0;
	for (k = 0; k < nn; k++)
	{
		int ix = (int)floor(ccx[k]); float xx = ccx[k] - (float)ix;
		int iy = (int)floor(ccy[k]); float yy = ccy[k] - (float)iy;
		seeding((unsigned int)((int)floor(ccx[k] * 15.0) + 10), (unsigned int)((int)floor(ccy[k] * 10.0) + 3), 0, seed);
		float subx = next(seed)*0.5 + 0.5;
		//float suby = next()*0.5 + 0.5;
		float dif = cdx[k] - cdy[k]; if (dif < 0.0) dif = -dif;
		//printf("dif=%g, psubx=%g(%g), psuby=%g(%g)\n", dif,psubx,subx,psuby,suby);
		if (dif < 0.1 && (subx < psubx)) // || suby<psuby))
		{
			float cutx = 0.5 + 0.2*next(seed)*jitx;
			float cuty = 0.5 + 0.2*next(seed)*jity;
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = (cutx*2.0*cdx[k])*0.5;
			ncdy = (cuty*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;

			ncdx = ((1.0 - cutx)*2.0*cdx[k])*0.5;
			ncdy = (cuty*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + (cutx*2.0*cdx[k]) + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;

			ncdx = (cutx*2.0*cdx[k])*0.5;
			ncdy = ((1.0 - cuty)*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + (cuty*2.0*cdy[k]) + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;

			ncdx = ((1.0 - cutx)*2.0*cdx[k])*0.5;
			ncdy = ((1.0 - cuty)*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + (cutx*2.0*cdx[k]) + ncdx;
			nccy = ccy[k] - cdy[k] + (cuty*2.0*cdy[k]) + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;
		}
		else if (cdx[k] > cdy[k] + 0.1 && subx < psubx)
		{
			float cutx = 0.4 + 0.2*(next(seed)*0.5 + 0.5);
			float cuty = 1.0;
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = (cutx*2.0*cdx[k])*0.5;
			ncdy = (cuty*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;

			ncdx = ((1.0 - cutx)*2.0*cdx[k])*0.5;
			ncdy = (cuty*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + (cutx*2.0*cdx[k]) + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;
		}
		else if (cdy[k] > cdx[k] + 0.1 && subx < psuby)
		{
			float cutx = 1.0;
			float cuty = 0.4 + 0.2*(next(seed)*0.5 + 0.5);
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = (cutx*2.0*cdx[k])*0.5;
			ncdy = (cuty*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;

			ncdx = (cutx*2.0*cdx[k])*0.5;
			ncdy = ((1.0 - cuty)*2.0*cdy[k])*0.5;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + (cuty*2.0*cdy[k]) + ncdy;
			rx = ncdx * next(seed)*jitx;
			ry = ncdy * next(seed)*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			ncx[nc] = nccx; ncy[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			nc++;
		}
		else
		{
			float rx = cdx[k] * next(seed)*jitx;
			float ry = cdy[k] * next(seed)*jity;
			cx[nc] = ccx[k] + rx;
			cy[nc] = ccy[k] + ry;
			ncx[nc] = ccx[k]; ncy[nc] = ccy[k]; ndx[nc] = cdx[k]; ndy[nc] = cdy[k];
			nc++;
		}
	}
	return nc;
}


static void relax(int nrelax, int n, float bx, float by, float dx, float dy,
	float cx[MAX_NEIGH_CELLS * 4], float cy[MAX_NEIGH_CELLS * 4], float bcx[MAX_NEIGH_CELLS * 4], float bcy[MAX_NEIGH_CELLS * 4], float dcx[MAX_NEIGH_CELLS * 4], float dcy[MAX_NEIGH_CELLS * 4])
{
	int i, j, k;
	float mcx[MAX_NEIGH_CELLS * 4], mcy[MAX_NEIGH_CELLS * 4];
	for (i = 0; i < nrelax; i++)
	{
		for (k = 0; k < n; k++)
			if (cx[k] >= bx - dx && cx[k] <= bx + dx && cy[k] >= by - dy && cy[k] <= by + dy)
			{
				float distmin1 = 100000.0; int ind1 = 0;
				float distmin2 = 100000.0; int ind2 = 0;
				float distmin3 = 100000.0; int ind3 = 0;
				for (j = 0; j < n; j++) if (j != k)
				{
					float dd = sqrt((cx[k] - cx[j])*(cx[k] - cx[j]) + (cy[k] - cy[j])*(cy[k] - cy[j]));
					if (dd < distmin1) { distmin3 = distmin2; ind3 = ind2; distmin2 = distmin1; ind2 = ind1; distmin1 = dd; ind1 = j; }
					else if (dd < distmin2) { distmin3 = distmin2; ind3 = ind2; distmin2 = dd; ind2 = j; }
					else if (dd < distmin3) { distmin3 = dd; ind3 = j; }
				}
				float dx1 = cx[ind1] - cx[k];
				float dy1 = cy[ind1] - cy[k];
				float no1 = sqrt((double)(dx1*dx1 + dy1 * dy1)); if (no1 == 0.0) no1 = 1.0;
				float dx2 = cx[ind2] - cx[k];
				float dy2 = cy[ind2] - cy[k];
				float no2 = sqrt((double)(dx2*dx2 + dy2 * dy2)); if (no2 == 0.0) no2 = 1.0;
				float dx3 = cx[ind3] - cx[k];
				float dy3 = cy[ind3] - cy[k];
				float no3 = sqrt((double)(dx3*dx3 + dy3 * dy3)); if (no3 == 0.0) no3 = 1.0;
				float dirx = dx1 / no1 / no1 + dx2 / no2 / no2 + dx3 / no3 / no3;
				float diry = dy1 / no1 / no1 + dy2 / no2 / no2 + dy3 / no3 / no3;
				float no = sqrt(dirx*dirx + diry * diry); if (no == 0.0) no = 1.0;
				mcx[k] = cx[k] - (dirx / no * 0.05);
				mcy[k] = cy[k] - (diry / no * 0.05);
				if (mcx[k] < bcx[k] - dcx[k] + 0.05) mcx[k] = bcx[k] - dcx[k] + 0.05;
				if (mcx[k] > bcx[k] + dcx[k] - 0.05) mcx[k] = bcx[k] + dcx[k] - 0.05;
				if (mcy[k] < bcy[k] - dcy[k] + 0.05) mcy[k] = bcy[k] - dcy[k] + 0.05;
				if (mcy[k] > by + dy) mcy[k] = bcy[k] + dcy[k] - 0.05;
			}
			else { mcx[k] = cx[k]; mcy[k] = cy[k]; }
		for (k = 0; k < n; k++) { cx[k] = mcx[k]; cy[k] = mcy[k]; }
	}
}



static int distribute(float px, float py,
	// point set parameters
	tilingtype tt, float psubx, float psuby,
	int decalx, int Nx, int nrelax, float jitter,
	float cx[MAX_NEIGH_CELLS * 4], float cy[MAX_NEIGH_CELLS * 4], float ncx[MAX_NEIGH_CELLS * 4], float ncy[MAX_NEIGH_CELLS * 4], float ndx[MAX_NEIGH_CELLS * 4], float ndy[MAX_NEIGH_CELLS * 4])
{
	int i, k;
	float ccx[MAX_NEIGH_CELLS]; float ccy[MAX_NEIGH_CELLS]; float cdx[MAX_NEIGH_CELLS]; float cdy[MAX_NEIGH_CELLS];
	int nn = hvNoise::pavement(px, py, tt, decalx, Nx, ccx, ccy, cdx, cdy);
	int np = 0;
	if (nrelax == 0) np = hvNoise::pointset(psubx, psuby, 0.9, 0.9, nn, ccx, ccy, cdx, cdy, cx, cy, ncx, ncy, ndx, ndy);
	else for (k = 0; k < nn; k++)
	{
		float gccx[MAX_NEIGH_CELLS]; float gccy[MAX_NEIGH_CELLS]; float gcdx[MAX_NEIGH_CELLS]; float gcdy[MAX_NEIGH_CELLS];
		float gcx[MAX_NEIGH_CELLS * 4]; float gcy[MAX_NEIGH_CELLS * 4]; float gncx[MAX_NEIGH_CELLS * 4]; float gncy[MAX_NEIGH_CELLS * 4]; float gndx[MAX_NEIGH_CELLS * 4]; float gndy[MAX_NEIGH_CELLS * 4];
		int nsub = hvNoise::pavement(ccx[k], ccy[k], tt, decalx, Nx, gccx, gccy, gcdx, gcdy);
		int npk = hvNoise::pointset(psubx, psuby, 0.9, 0.9, nsub, gccx, gccy, gcdx, gcdy, gcx, gcy, gncx, gncy, gndx, gndy);
		hvNoise::relax(nrelax, npk, ccx[k], ccy[k], cdx[k], cdy[k], gcx, gcy, gncx, gncy, gndx, gndy);
		for (i = 0; i < npk; i++)
			if (gcx[i] >= ccx[k] - cdx[k] && gcx[i] <= ccx[k] + cdx[k] &&
				gcy[i] >= ccy[k] - cdy[k] && gcy[i] <= ccy[k] + cdy[k])
			{
				cx[np] = gcx[i]; cy[np] = gcy[i];
				ncx[np] = gncx[i]; ncy[np] = gncy[i];
				ndx[np] = gndx[i]; ndy[np] = gndy[i];
				np++;
			}
	}
	for (i = 0; i < np; i++) {
		cx[i] = cx[i] * jitter + ncx[i] * (1.0 - jitter);
		cy[i] = cy[i] * jitter + ncy[i] * (1.0 - jitter);
	}
	return np;
}




static int genPointSet(float x, float y,
	// point set parameters
	int pointsettype,
	float jitter,
	float px[MAX_NEIGH_CELLS * 4], float py[MAX_NEIGH_CELLS * 4], float ncx[MAX_NEIGH_CELLS * 4], float ncy[MAX_NEIGH_CELLS * 4], float ndx[MAX_NEIGH_CELLS * 4], float ndy[MAX_NEIGH_CELLS * 4])
{
	tilingtype tt; float ppointsub;
	int decalx; int Nx; int nrelax;
	//printf("pointsett=%d\n", pointsettype);
	switch (pointsettype)
	{
	case 0: tt = hvNoise::REGULAR; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
	case 1: tt = hvNoise::REGULAR; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 0; break;
	case 2: tt = hvNoise::REGULAR; ppointsub = 1.0;  decalx = 1; Nx = 0; nrelax = 5; break;
	case 3: tt = hvNoise::REGULAR; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 5; break;
	case 4: tt = hvNoise::REGULAR; ppointsub = 0.0;  decalx = 2; Nx = 0; nrelax = 0; break;
	case 5: tt = hvNoise::REGULAR; ppointsub = 0.0;  decalx = 3; Nx = 0; nrelax = 0; break;
	case 6: tt = hvNoise::IRREGULAR; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
	case 7: tt = hvNoise::IRREGULAR; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 0; break;
	case 8: tt = hvNoise::IRREGULARX; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
	case 9: tt = hvNoise::IRREGULARX; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 0; break;
	case 10: tt = hvNoise::CROSS; ppointsub = 0.0;  decalx = 0; Nx = 2; nrelax = 0; break;
	case 11: tt = hvNoise::CROSS; ppointsub = 0.5;  decalx = 0; Nx = 2; nrelax = 0; break;
	case 12: tt = hvNoise::CROSS; ppointsub = 0.0;  decalx = 0; Nx = 3; nrelax = 0; break;
	case 13: tt = hvNoise::CROSS; ppointsub = 0.5;  decalx = 0; Nx = 3; nrelax = 0; break;
	case 14: tt = hvNoise::BISQUARE; ppointsub = 0.0;  decalx = 0; Nx = 1; nrelax = 0; break;
	case 15: tt = hvNoise::BISQUARE; ppointsub = 0.5;  decalx = 0; Nx = 1; nrelax = 0; break;
	case 16: tt = hvNoise::APAVEMENT; ppointsub = 0.0;  decalx = 0; Nx = 1; nrelax = 0; break;
	case 17: tt = hvNoise::APAVEMENT; ppointsub = 0.5;  decalx = 0; Nx = 1; nrelax = 0; break;
	default: tt = hvNoise::REGULAR; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
	}
	// compute points
	return hvNoise::distribute(x, y, tt, ppointsub, ppointsub, decalx, Nx, nrelax, jitter, px, py, ncx, ncy, ndx, ndy);
}





static float cdistance(float x1, float y1, float x2, float y2, float norm,
	float cx, float cy, float dx, float dy, float larp)
{
	//seeding((unsigned int)(cx*12.0 + 5.0), (unsigned int)(cy*12.0 + 11.0), 0);
	//float ss = size*next();
	float ddx = (x1 - x2);
	float ddy = (y1 - y2);
	float ex = ddx < 0.0 ? -ddx / (x2 - cx + dx) : ddx / (cx + dx - x2);
	float ey = ddy < 0.0 ? -ddy / (y2 - cy + dy) : ddy / (cy + dy - y2);
	//float lx = (1.0 - larp)*abs(ddx) + larp*ex;
	//float ly = (1.0 - larp)*abs(ddy) + larp*ey;
	//float no = (float)pow(pow(lx, norm) + pow(ly, norm), 1.0 / norm);
	//if (norm <= 2.0) return(no);
	//else return (norm - 2.0)*(lx > ly ? lx : ly) + (1.0 - (norm - 2.0))*no;
	//printf("cdist: ddx=%g,ddy=%g, ex=%g, ey=%g, dx=%g,dy=%g\n", ddx, ddy, ex, ey, dx, dy);
	//return (larp*(float)pow(pow(abs(ddx), norm) + pow(abs(ddy), norm), 1.0 / norm)
		//+(1.0-larp)*(ex > ey ? ex : ey));
	return ((1.0 - larp)*pow(pow(abs(ddx), norm) + pow(abs(ddy), norm), 1.0 / norm) + larp * (ex > ey ? ex : ey));

}


static int cclosest(float xx, float yy, float cx[], float cy[], int nc,
	float norm, float cnx[], float cny[], float dx[], float dy[], float larp)
{
	int mink = 0;
	float mind = 0.0;
	int k;
	for (k = 0; k < nc; k++)
	{
		float dd = cdistance(xx, yy, cx[k], cy[k], norm, cnx[k], cny[k], dx[k], dy[k], larp);
		//float dx = xx - cx[k];
		//float dy = yy - cy[k];
		//float dd = (float)pow(pow(abs(dx), norm) + pow(abs(dy), norm), 1.0 / norm);
		if (k == 0) { mind = dd; }
		else if (mind > dd) { mink = k; mind = dd; }
	}
	return mink;
}




static void nthclosest(int mink[], int nn, float xx, float yy, float cx[], float cy[], int nc,
	float norm, float ncx[], float ncy[], float dx[], float dy[], float larp)
{
	int i, k;
	float dist[200];
	for (k = 0; k < nc; k++)
	{
		float dd = cdistance(xx, yy, cx[k], cy[k], norm, ncx[k], ncy[k], dx[k], dy[k], larp);
		//float dx = xx - cx[k];
		//float dy = yy - cy[k];
		//float dd = (float)pow(pow(abs(dx), norm) + pow(abs(dy), norm), 1.0 / norm);
		dist[k] = dd;
	}
	for (i = 0; i < nn; i++)
	{
		int mk = 0;
		for (k = 1; k < nc; k++)
		{
			if (dist[mk] > dist[k]) mk = k;
		}
		mink[i] = mk;
		dist[mk] = 100000.0;
	}
}


static float celldist(float ixx, float iyy, int k, int mink, float cx[], float cy[], int nc,
	float norm, float cnx[], float cny[], float dx[], float dy[], float larp)
{
	float delta = 0.2;
	int count, nk;
	float xx, yy, ddx, ddy, dd;
	do {
		xx = ixx; yy = iyy;
		ddx = cx[k] - xx; ddy = cy[k] - yy;
		dd = (float)sqrt(ddx*ddx + ddy * ddy);
		if (dd < 0.001) return 0.0;
		ddx *= delta / dd; ddy *= delta / dd;
		if (k == mink) { ddx = -ddx; ddy = -ddy; }
		//printf("start  with cell %d, %d, %g,%g, %g,%g,  %g,%g ->%g,%g\n", k, mink, ddx, ddy, ixx, iyy, cx[k], cy[k], xx, yy);
		//printf("cell is: %g,%g, (%g,%g)\n", cnx[k], cny[k], dx[k], dy[k]);
		//printf("mincell is: %g,%g, (%g,%g)\n", cnx[mink], cny[mink], dx[mink], dy[mink]);
		count = 0;
		//nk = cclosest(xx+ddx, yy+ddy, cx, cy, nc, norm, cnx, cny, dx, dy, larp);
		//if (!((k == mink && nk == k) || (k != mink&&nk != k))) {
		//	printf("start problem with cell, %d, %d, %g,%g, %g,%g,  %g,%g ->%g,%g\n", k, mink, dx, dy, ixx, iyy, cx[k], cy[k], xx, yy); 
		//}
		do {
			xx += ddx; yy += ddy;
			nk = cclosest(xx, yy, cx, cy, nc, norm, cnx, cny, dx, dy, larp);
			//if (count>97) printf("%d, nk=%d, xx=%g,yy=%g, delta=%g\n", count, nk, xx, yy, delta);
			count++;
		} while (((k == mink && nk == k) || (k != mink && nk != k)) && count < 100);
		if (count == 100 && delta <= 0.009) {
			printf("problem with cell, %d, %d, displ:%g,%g, \nfrom: %g,%g,  cell:%g,%g ->at: %g,%g\n", k, mink, ddx, ddy, ixx, iyy, cx[k], cy[k], xx, yy);
			printf("in cell: %g,%g, (%g,%g)\n", cnx[k], cny[k], dx[k], dy[k]);
			for (int u = 0; u < nc; u++) if (u != k)
				printf("neighbor cell %d: %g,%g, (%g,%g)\n", u, cnx[u], cny[u], dx[u], dy[u]);
			hvFatal("error");
		}
		delta /= 2.0f;
	} while (count == 100 && delta >= 0.01);
	float xa = xx - ddx, ya = yy - ddy;
	float midx = (xa + xx) / 2.0, midy = (ya + yy) / 2.0;
	//printf("refine ddx=%g, ddy=%g, midx=%g,midy=%g, cx=%g,cy=%g,cnx=%g,cny=%g,dx=%g,dy=%g\n", ddx, ddy, midx, midy, cx[k], cy[k], cnx[k], cny[k], dx[k], dy[k]);
	for (int i = 0; i < 5; i++)
	{
		nk = cclosest(midx, midy, cx, cy, nc, norm, cnx, cny, dx, dy, larp);
		if (((k == mink && nk == k) || (k != mink && nk != k))) { xa = midx; ya = midy; }
		else { xx = midx; yy = midy; }
		midx = (xa + xx) / 2.0; midy = (ya + yy) / 2.0;
	}
	//float cdi=cdistance(midx, midy, cx[k], cy[k], norm, cnx[k], cny[k], dx[k], dy[k], larp);
	//printf("%g : k=%d, mink=%d, ddx=%g, ddy=%g, midx=%g,midy=%g, cx=%g,cy=%g,cnx=%g,cny=%g,dx=%g,dy=%g\n", cdi, k, mink, ddx, ddy, midx, midy, cx[k], cy[k], cnx[k], cny[k], dx[k], dy[k]);
	//return cdi;
	float vdx = cx[k] - midx, vdy = cy[k] - midy;
	//return (float)pow(pow(abs(dx), norm) + pow(abs(dy), norm), 1.0 / norm);
	return sqrt(vdx*vdx + vdy * vdy);
}



static float interTriangle(float origx, float origy, float ddx, float ddy, float startx, float starty, float endx, float endy)
{
	float dirx = (endx - startx);
	float diry = (endy - starty);
	float dirno = sqrt(dirx*dirx + diry * diry);
	dirx /= dirno; diry /= dirno;
	float val = ddx * diry - ddy * dirx;
	float segx = -(startx - origx);
	float segy = -(starty - origy);
	float lambda = (dirx*segy - diry * segx) / val;
	return lambda;
}

static void bezier2(float ts, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float &splinex, float &spliney)
{
	float p01x = ts * p1x + (1.0 - ts)*p0x;
	float p01y = ts * p1y + (1.0 - ts)*p0y;
	float p11x = ts * p2x + (1.0 - ts)*p1x;
	float p11y = ts * p2y + (1.0 - ts)*p1y;
	splinex = ts * p11x + (1.0 - ts)*p01x;
	spliney = ts * p11y + (1.0 - ts)*p01y;
}




///////////////////////////////////////////////////// POINT PROCESS NOISE with cell center
static float cpptbf_gen_v2c(float xx, float yy,
	// deformation and normalization
	float zoom, float alpha, float rescalex,
	float ampli[3],

	// point set parameters
	int tt,
	float jitter,

	// window function parameters
	int winshape,
	float arity, // voroi cell polygon arity
	float larp,  // anisotropy of cell norm
	float wsmooth,
	float norm, float normblend, float normsig,

	// feature function parameters
	int bomb,
	float normfeat, float winfeatcorrel, float feataniso,
	int Npmin, int Npmax,
	float sigcos, float sigcosvar,
	float freq, float phase, float thickness, float courbure, float deltaorient,
	float &pointvalue, float &cellpointx, float &cellpointy
)
{
	unsigned int seed;

	int i, k;
	float cx[MAX_NEIGH_CELLS]; float cy[MAX_NEIGH_CELLS]; float dx[MAX_NEIGH_CELLS]; float dy[MAX_NEIGH_CELLS];
	float px[MAX_NEIGH_CELLS * 4], py[MAX_NEIGH_CELLS * 4], ncx[MAX_NEIGH_CELLS * 4], ncy[MAX_NEIGH_CELLS * 4], ndx[MAX_NEIGH_CELLS * 4], ndy[MAX_NEIGH_CELLS * 4];

	//----------------
	// [1] Deformation
	//----------------

	// Apply turbulence (i.e. noise-based spatial distorsion)
	float distortamp = ampli[0];
	float distortfact = ampli[1]; // \in ]0,1]
	float distortfreq = ampli[2]; // \in [0,1]
								  // - x position
	float ppx = xx + distortamp * cnoise2D(distortfreq*xx*zoom*0.5 + 2.0, distortfreq*yy*zoom*0.5)
		+ distortamp * distortfact * cnoise2D(distortfreq*xx*zoom + 2.0, distortfreq*yy*zoom)
		+ distortamp * distortfact * distortfact * cnoise2D(distortfreq*xx*zoom*2.0 + 2.0, distortfreq*yy*zoom*2.0)
		+ distortamp * distortfact * distortfact * distortfact * cnoise2D(distortfreq*xx*zoom*4.0 + 2.0, distortfreq*yy*zoom*4.0);
	// - y position
	float ppy = yy + distortamp * cnoise2D(distortfreq*xx*zoom*0.5, distortfreq*yy*zoom*0.5 + 5.0)
		+ distortamp * distortfact * cnoise2D(distortfreq*xx*zoom, distortfreq*yy*zoom + 5.0)
		+ distortamp * distortfact * distortfact * cnoise2D(distortfreq*xx*zoom*2.0, distortfreq*yy*zoom*2.0 + 5.0)
		+ distortamp * distortfact * distortfact * distortfact * cnoise2D(distortfreq*xx*zoom*4.0, distortfreq*yy*zoom*4.0 + 5.0);
	//--------------------
	// [2] Model Transform
	//--------------------

	// Apply other transforms: rescale, rotation and zoom
	// Apply also a shift +100.0 to avoid negative coordinates
	float x = 100.0 + (ppx * cos(-alpha) + ppy * sin(-alpha)) / rescalex * zoom;
	float y = 100.0 + (-ppx * sin(-alpha) + ppy * cos(-alpha)) * zoom;

	//------------------
	// [3] Point Process
	//------------------
	// compute points
	int nc = hvNoise::genPointSet(x, y, tt, jitter, px, py, ncx, ncy, ndx, ndy);
	int mink[MAX_NEIGH_CELLS * 4];
	int npp = (nc < 18 ? nc : 18);
	hvNoise::nthclosest(mink, npp, x, y, px, py, nc, norm, ncx, ncy, ndx, ndy, larp);

	//-------------------------
	// [4] PPTBF = PP x ( W F )
	//-------------------------

	float vv = 0.0f; // final value, to be computed and returned
	float winsum = 0.0f; // for normalization
	float pmax = -1.0f; // initial lowest priority for bombing
	float closestdist = -100.0f; // initial value for min distance
	for (k = 0; k < npp; k++) // for each neighbor cell / point
	{
		// generate all random values of the cell / point
		seeding((unsigned int)(px[mink[k]] * 12.0 + 7.0), (unsigned int)(py[mink[k]] * 12.0 + 1.0), 0, seed );
		float dalpha = 2.0 * M_PI / pow(2.0, (float)((unsigned int)(arity + 0.5)));
		float rotalpha = dalpha * (next(seed) * 0.5 + 0.5);
		float npointvalue = next(seed);
		if (k == 0) {
			pointvalue = npointvalue; cellpointx = px[mink[k]]; cellpointy = py[mink[k]];
		}
		//float npointvalue = mval + vval*next();
		//if (k == 0) pointvalue = npointvalue;
		int nn = Npmin + (int)((float)(Npmax - Npmin)*(0.5*next(seed) + 0.5));
		if (nn > 20) {
			printf("Npmin=%d, Npmax=%d\n", Npmin, Npmax); hvFatal("stop");
		}
		//printf("k=%d,mink=%d, px,py=%g,%g, nn=%d\n", k, mink[k], px[mink[k]], py[mink[k]], nn);
		//-----------------------
		// [5] Window Function: W
		//-----------------------
		// compute Window function: it is a blend between cellular and classical window functions
		////////////////////////////////////////////////////////////////////////
		float ddx = (x - px[mink[k]]); // / ndx[mink[k]];
		float ddy = (y - py[mink[k]]); // / ndy[mink[k]];
		float sdd = sqrt(ddx * ddx + ddy * ddy);
		float sddno = pow(pow(abs(ddx), norm) + pow(abs(ddy), norm), 1.0 / norm);
		if (norm > 2.0)  sddno = (norm - 2.0) * (abs(ddx) > abs(ddy) ? abs(ddx) : abs(ddy)) + (1.0 - (norm - 2.0)) * sddno;
		float gauss = 1.0;
		float foot[4] = { 1.2, 1.0, 1.0, 1.0 };
		float footprint = foot[winshape]; // * ( 1.0 + 0.2 * jitter * next() );
		if (tt >= 10) footprint *= 0.4;
		switch (winshape)
		{
			//case 0: if (sddno < 1.0 * footprint / 4.0) gauss = 1.0; // tapered cosine window
			//		else if (sddno < footprint) gauss = 0.5 * (1.0 + cos(M_PI * 4.0 / footprint / 3.0 * (sddno - footprint / 4.0)));
			//		else gauss = 0.0;
			//		break;
		case 0:
		case 1: gauss = (exp(-2.0 * sddno) - exp(-2.0 * footprint)) / (1.0 - exp(-2.0 * footprint)); break; // clamped gaussian
		case 2: gauss = 1.0 - sddno / footprint; break; // triangular window
		case 3: if (sddno > footprint) gauss = 0.0; break; // rectangular window
		default: gauss = 1.0 - sddno / footprint; break; // triangular window
		}
		if (gauss < 0.0) gauss = 0.0; else if (gauss > 1.0) gauss = 1.0;

		//--------------------
		// [6] Cellular Window
		//--------------------
		// compute cellular value
		float cv = 0.0;
		if (k == 0) // if inside closest cell then compute cellular value, outside it is 0
		{
			//////////////////////////////////////////////////////////
			// compute position inside cell in  polar coordinates
			ddx /= sdd; ddy /= sdd;
			float dd = sdd;
			float alpha = acos(ddx);
			if (ddy < 0.0) alpha = 2.0*M_PI - alpha;
			float palpha = alpha - rotalpha;
			if (palpha < 0.0) palpha += 2.0*M_PI;
			int ka = (int)(palpha / dalpha);
			float rka = palpha / dalpha - (float)ka;

			float ptx = px[mink[0]] + 0.1*cos(dalpha*(float)ka + rotalpha);
			float pty = py[mink[0]] + 0.1*sin(dalpha*(float)ka + rotalpha);
			float celldd1 = hvNoise::celldist(ptx, pty, mink[k], mink[0], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp);
			float startx = px[mink[0]] + celldd1*cos(dalpha*(float)ka + rotalpha);
			float starty = py[mink[0]] + celldd1*sin(dalpha*(float)ka + rotalpha);
			ptx = px[mink[0]] + 0.1*cos(dalpha*(float)ka + dalpha + rotalpha);
			pty = py[mink[0]] + 0.1*sin(dalpha*(float)ka + dalpha + rotalpha);
			float celldd2 = hvNoise::celldist(ptx, pty, mink[k], mink[0], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp);
			float endx = px[mink[0]] + celldd2*cos(dalpha*(float)ka + dalpha + rotalpha);
			float endy = py[mink[0]] + celldd2*sin(dalpha*(float)ka + dalpha + rotalpha);

			// for smoothing the cell using Bezier
			float midx = (startx + endx) / 2.0;
			float midy = (starty + endy) / 2.0;
			float middx = (midx - px[mink[0]]);
			float middy = (midy - py[mink[0]]);
			float midno = sqrt(middx*middx + middy*middy);
			middx /= midno; middy /= midno;
			//printf("acos=%g, dalpha=%g\n", acos(middx*ddx + middy*ddy) / M_PI, dalpha/M_PI);
			float midalpha = acos(middx);
			if (middy < 0.0) midalpha = 2.0*M_PI - midalpha;
			float diff = alpha - midalpha; if (diff < 0.0) diff = -diff;
			if (diff > 2.0*dalpha && alpha < 2.0*dalpha) midalpha -= 2.0*M_PI;
			else if (diff > 2.0*dalpha && alpha > 2.0*M_PI - 2.0*dalpha) midalpha += 2.0*M_PI;
			float splinex, spliney;
			float smoothx, smoothy;
			if (alpha > midalpha)
			{
				ptx = px[mink[0]] + 0.1*cos(dalpha*(float)ka + 2.0*dalpha + rotalpha);
				pty = py[mink[0]] + 0.1*sin(dalpha*(float)ka + 2.0*dalpha + rotalpha);
				float celldd = hvNoise::celldist(ptx, pty, mink[k], mink[0], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp);
				float nendx = px[mink[0]] + celldd*cos(dalpha*(float)ka + 2.0*dalpha + rotalpha);
				float nendy = py[mink[0]] + celldd*sin(dalpha*(float)ka + 2.0*dalpha + rotalpha);
				float vvx = (endx - startx), vvy = (endy - starty);
				float nn = sqrt(vvx*vvx + vvy*vvy); vvx /= nn; vvy /= nn;
				float wwx = (nendx - endx), wwy = (nendy - endy);
				nn = sqrt(wwx*wwx + wwy*wwy); wwx /= nn; wwy /= nn;
				nendx = (nendx + endx) / 2.0; nendy = (nendy + endy) / 2.0;

				float lambda = hvNoise::interTriangle(px[mink[0]], py[mink[0]], ddx, ddy, midx, midy, nendx, nendy);
				float bordx = ddx*lambda + px[mink[0]];
				float bordy = ddy*lambda + py[mink[0]];
				float dirno = sqrt((nendx - midx)*(nendx - midx) + (nendy - midy)*(nendy - midy));
				float ts = sqrt((bordx - midx)*(bordx - midx) + (bordy - midy)*(bordy - midy));
				ts /= dirno;
				hvNoise::bezier2(ts, midx, midy, endx, endy, nendx, nendy, splinex, spliney);
				smoothx = bordx; smoothy = bordy;
			}
			else
			{
				ptx = px[mink[0]] + 0.1*cos(dalpha*(float)ka - dalpha + rotalpha);
				pty = py[mink[0]] + 0.1*sin(dalpha*(float)ka - dalpha + rotalpha);
				float celldd = hvNoise::celldist(ptx, pty, mink[k], mink[0], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp);
				float nstartx = px[mink[0]] + celldd*cos(dalpha*(float)ka - dalpha + rotalpha);
				float nstarty = py[mink[0]] + celldd*sin(dalpha*(float)ka - dalpha + rotalpha);
				float vvx = (startx - nstartx), vvy = (starty - nstarty);
				float nn = sqrt(vvx*vvx + vvy*vvy); vvx /= nn; vvy /= nn;
				float wwx = (endx - startx), wwy = (endy - starty);
				nn = sqrt(wwx*wwx + wwy*wwy); wwx /= nn; wwy /= nn;
				nstartx = (nstartx + startx) / 2.0; nstarty = (nstarty + starty) / 2.0;
				float lambda = hvNoise::interTriangle(px[mink[0]], py[mink[0]], ddx, ddy, nstartx, nstarty, midx, midy);
				float bordx = ddx*lambda + px[mink[0]];
				float bordy = ddy*lambda + py[mink[0]];
				float dirno = sqrt((midx - nstartx)*(midx - nstartx) + (midy - nstarty)*(midy - nstarty));
				float ts = sqrt((bordx - nstartx)*(bordx - nstartx) + (bordy - nstarty)*(bordy - nstarty));
				ts /= dirno;
				hvNoise::bezier2(ts, nstartx, nstarty, startx, starty, midx, midy, splinex, spliney);
				smoothx = bordx; smoothy = bordy;
			}

			float lambda = hvNoise::interTriangle(px[mink[0]], py[mink[0]], ddx, ddy, startx, starty, endx, endy);
			float bordx = ddx*lambda + px[mink[0]];
			float bordy = ddy*lambda + py[mink[0]];

			float smoothdist = sqrt((smoothx - px[mink[0]])*(smoothx - px[mink[0]]) + (smoothy - py[mink[0]])*(smoothy - py[mink[0]]));
			float splinedist = sqrt((splinex - px[mink[0]])*(splinex - px[mink[0]]) + (spliney - py[mink[0]])*(spliney - py[mink[0]]));

			if (wsmooth < 1.0) cv = (1.0 - wsmooth)*(1.0 - dd / lambda) + wsmooth*(1.0 - dd / smoothdist);
			else if (wsmooth < 2.0) cv = (2.0 - wsmooth)*(1.0 - dd / smoothdist) + (wsmooth - 1.0)*(1.0 - dd / splinedist);
			else cv = 1.0 - dd / splinedist;
			if (cv < 0.0) cv = 0.0; else if (cv > 1.0) cv = 1.0;
		}
		// blend the cellular window with the classical window
		float coeff1 = normblend * (exp((cv - 1.0)*normsig) - exp(-1.0*normsig)) / (1.0 - exp(-1.0*normsig));
		float coeff2 = (1.0 - normblend) * gauss;
		//winsum += coeff;
		winsum += coeff1 + coeff2;
		//if (k==0) printf("k=%d,cv=%g,gauss=%g,coeff=%g,winsum=%g\n", k, cv,gauss, coeff,winsum);

		//---------------------
		// [7] Feature Function
		//---------------------
		// Feature function, by default is constant=1.0
		float feat = 0.0;
		float lx[20], ly[20], deltalx[20], deltaly[20], angle[20], prior[20], sigb[20], valb[20]; // , sigma[20];

		seeding((unsigned int)(px[mink[k]] * 15.0 + 2.0), (unsigned int)(py[mink[k]] * 15.0 + 5.0), 0, seed);
		//valb[ 0 ] = next();
		//prior[ 0 ] = next() * 0.5 + 0.5;
		//lx[ 0 ] = px[ mink[ k ] ];
		//ly[ 0 ] = py[ mink[ k ] ];
		//deltalx[ 0 ] = ( x - lx[ 0 ] ) / ndx[ mink[ k ] ];
		//deltaly[ 0 ] = ( y - ly[ 0 ] ) / ndy[ mink[ k ] ];
		//angle[ 0 ] = deltaorient * next();
		//sigb[ 0 ] = sigcos * ( 1.0 + ( next() + 1.0 ) / 2.0 * sigcosvar );
		for (int i = 0; i < nn; i++)
		{
			valb[i] = next(seed);
			prior[i] = next(seed) * 0.5 + 0.5;
			lx[i] = ncx[mink[k]] + next(seed) * 0.98 * ndx[mink[k]];
			ly[i] = ncy[mink[k]] + next(seed) * 0.98 * ndy[mink[k]];
			lx[i] = winfeatcorrel*px[mink[k]] + (1.0 - winfeatcorrel)*lx[i];
			ly[i] = winfeatcorrel*py[mink[k]] + (1.0 - winfeatcorrel)*ly[i];
			deltalx[i] = (x - lx[i]) / ndx[mink[k]];
			deltaly[i] = (y - ly[i]) / ndy[mink[k]];
			angle[i] = deltaorient * next(seed);
			sigb[i] = sigcos * (1.0 + (next(seed) + 1.0) / 2.0 * sigcosvar);
		}
		feat = ((bomb == 0 || bomb == 5) ? 1.0 : 0.0);
		for (i = 0; i < nn; i++)
		{
			float ddx = (deltalx[i] * cos(-angle[i]) + deltaly[i] * sin(-angle[i]));
			float iddy = (-deltalx[i] * sin(-angle[i]) + deltaly[i] * cos(-angle[i]));
			float ddy = iddy / pow(2.0, feataniso);
			float dd2 = pow(pow(abs(ddx), normfeat) + pow(abs(ddy), normfeat), 1.0 / normfeat);
			if (normfeat > 2.0) dd2 = (normfeat - 2.0) * (abs(ddx) > abs(ddy) ? abs(ddx) : abs(ddy)) + (1.0 - (normfeat - 2.0)) * dd2;
			float ff = 0.5 + 0.5 * cos(M_PI * float(freq) * sqrt(ddx * ddx * courbure * courbure + ddy * ddy));
			ff = pow(ff, 1.0f / (0.0001 + thickness));
			float ddist = dd2 / (footprint / sigb[i]);
			float dwin = 1.0 - ddist;
			if (dwin < 0.0) dwin = 0.0;
			if (bomb == 1) {
				float val = (valb[i] < 0.0 ? -0.25 + 0.75 * valb[i] : 0.25 + 0.75 * valb[i]);
				feat += ff * val * exp(-ddist);
				//if (k == 0) printf("i=%d, dd2=%g, feat=%g\n", i, dd2, feat);
			}
			else if (bomb == 2) feat += ff * exp(-ddist);
			else if (bomb == 3)
			{
				//printf("cell %d: at %g,%g, coeff=%g, feat=%g, prior=%g, max=%g\n", k, px[mink[k]], py[mink[k]], coeff, feat, prior, pmax);
				// add contributions, except if bombing
				if (pmax < prior[i] && dwin > 0.0 && ff>0.5)
				{
					pmax = prior[i]; vv = 2.0*(ff - 0.5) * exp(-ddist);
					pointvalue = npointvalue;
					cellpointx = lx[i]; cellpointy = ly[i];
				}
			}
			else if (bomb == 4)
			{
				float ww = ff* exp(-ddist);
				if (closestdist < ww)
				{
					vv = ww; closestdist = ww; //( 1.0 - dd2 / ( footprint ) );
					pointvalue = npointvalue;
					cellpointx = lx[i]; cellpointy = ly[i];
				}
			}
			else feat = 1.0f;
		}
		if (bomb == 1) feat = 0.5 * feat + 0.5; // normalization
		if (bomb == 2) feat /= float(Npmax / 2 > 0 ? Npmax / 2 : 1); // normalization
																	 // apply phase which should be 0.0 or PI/2
		if (bomb != 0) feat = feat * cos(phase) * cos(phase) + (1.0 - feat) * sin(phase) * sin(phase);
		//if ( feat < 0.0 ) feat = 0.0;

		//float correl = 1.0 - sddno / (footprint); if (correl<0.0) correl = 0.0;
		//if ((1.0 - winfeatcorrel) >= correl) correl = 1.0;
		//else if ((1.0 - winfeatcorrel) < correl*0.8) correl = 0.0;
		//else correl = ((1.0 - winfeatcorrel) - correl*0.8) / (correl*0.2);
		//if ((bomb != 3 && bomb != 4) || winshape != 0) vv += coeff * (1.0 * (1.0 - correl) + correl * feat);
		//if ( (bomb !=3 && bomb!=4) || winshape != 0 ) vv += coeff * feat ;

		if (bomb != 3 && bomb != 4) { vv += (coeff1 + coeff2) * feat; }
		//if (k==0) printf("k=%d, coeff=%g, feat=%g, vv=%g\n", k, coeff, feat, vv);
	}

	if ((bomb != 3 && bomb != 4) && winshape == 0 && winsum > 0.0) vv /= winsum;
	//if ((bomb != 3 && bomb != 4) && winsum > 0.0) vv /= winsum;
	if (vv < 0.0) vv = 0.0;
	return vv;
}


};

//unsigned int hvNoise::seed=0;

}

#endif // _HV_NOISE_H_
