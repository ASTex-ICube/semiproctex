////////////////////////////////////////////////////////////////////////////////
//
// COMPUTE SHADER
//
// Author(s): Anonymous
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Publication: By-Example Point Process Texture Basis Functions
// Authors: Anonymous
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// VERSION
////////////////////////////////////////////////////////////////////////////////

// OpenGL
#version 460

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////

// PRNG (Pseudo-Random Number Generator) + Noise
#define MAX_NOISE_RAND 1024

// Maths
#define M_PI 3.1415926535897932384626433832795

// Point Process
// - tiling type
#define PP_tilingType_REGULAR 0
#define PP_tilingType_IRREGULAR 1
#define PP_tilingType_CROSS 2
#define PP_tilingType_BISQUARE 3
#define PP_tilingType_IRREGULARX 4
#define PP_tilingType_IRREGULARY 5
#define PP_tilingType_APAVEMENT 6
// - max number of neighbors
#define PP_nbMaxNeighbors 18

// Window Function
// - shape
#define W_windowShape_NORMALIZED_TAPERED_COSINE 0
#define W_windowShape_CLAMPED_GAUSSIAN 1
#define W_windowShape_TRIANGULAR 2
#define W_windowShape_TAPERED_COSINE 3

// Feature Function
// - type
#define F_featureType_CONSTANT 0 // feature == 1.0
#define F_featureType_RANDOM_GABOR 1
#define F_featureType_GABOR 2
#define F_featureType_BOMBING 3
#define F_featureType_VORONOISE 4
#define F_featureType_USER_MARKOV_CHAIN_NETWORK 5
// - max number of neighbors
//#define F_nbMaxKernels 8
#define F_nbMaxKernels 16 // value is superior than 8 due to feature type "Markov chain network"

////////////////////////////////////////////////////////////////////////////////
// INPUTS
////////////////////////////////////////////////////////////////////////////////

// Block size
// - 8x8 threads
layout( local_size_x = 8, local_size_y = 8 ) in;

////////////////////////////////////////////////////////////////////////////////
// OUTPUTS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// UNIFORMS
////////////////////////////////////////////////////////////////////////////////

// PPTBF map
layout( r32f, binding = 0 ) uniform writeonly image2D uPPTBFImage;
 
// PRNG (Pseudo-Random Number Generator)
uniform isampler2D uPermutationTex;

// Noise
uniform sampler2D uNoiseTex;

// Deformation
uniform float uTurbulenceAmplitude_0;
uniform float uTurbulenceAmplitude_1;
uniform float uTurbulenceAmplitude_2;

// Model Transform
uniform int uResolution;
uniform float uShiftX;
uniform float uShiftY;
uniform float uRotation;
uniform float uRescaleX;

// Point Process
uniform int uPointProcessTilingType; // type of tiling for Point Process
uniform float uPointProcessJitter;

// Window Function
uniform int uWindowShape; // new: type of window function [0,3] (blended tapered cos, Gauss, triang, tapered cos)
uniform float uWindowArity; // >=2.0 && <=5.0, arity of polygonal cell window
uniform float uWindowLarp; // anisotropy of cell for cellular window
uniform float uWindowNorm; // norm 1=L1, 2=L2, 3=Linf of cellular window
uniform float uWindowSmooth; // smoothness of cellular 0=linear, 1 or 2=Bezier
uniform float uWindowBlend; // blend between cellular=1 and classical window=0
uniform float uWindowSigwcell; // power of cellular window

// Feature Function
uniform int uFeatureBomb; // type of feature function: constant=0, sum of Gaussian=1, Fourier=2, bombing=3, distance=4
uniform float uFeatureNorm; // norm 1=L1, 2=L2, 3=Linf of feature elements
uniform float uFeatureWinfeatcorrel; // correlation with window function 0 - 10 (very strong)
uniform float uFeatureAniso; // >=1.0, anisotropy of elements, 1.0=> isotropic
uniform int uFeatureNpmin; // Npmin>=1, Npmax<=Npmin, amount of elements
uniform int uFeatureNpmax; // Npmin>=1, Npmax<=Npmin, amount of elements
uniform float uFeatureSigcos; // size of elements + variations
uniform float uFeatureSigcosvar; // size of elements + variations
uniform int uFeatureFreq;
uniform float uFeaturePhase; // 0.0 or PI/2
uniform float uFeatureThickness; 
uniform float uFeatureCourbure;  // for Fourier
uniform float uFeatureDeltaorient; // orientation when anisotropy

// Labeling: on-the-fly generation
// - labeling
uniform int uNbLabels;
// - PPTBF label map
layout( r8ui, binding = 1 ) uniform writeonly uimage2D uLabelImage;
// - PPTBF random value map
layout( r32f, binding = 2 ) uniform writeonly image2D uRandomValueImage;

////////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------
// PRNG (Pseudo-Random Number Generator) + Noise
//----------------------------------------------

uint g_PRNG_seed;

////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//------
// PPTBF
//------

float procedural_pptbf(
	// position
	float xx, float yy,
	// model transform
	float zoom, float alpha, float rescalex,
	// deformation
	float distortamp, float distortfact, float distortnorm,
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
	float normfeat,  float winfeatcorrel, float feataniso,
	int Npmin, int Npmax, float sigcos, float sigcosvar,
	int freq, float phase, float thickness, float courbure, float deltaorient,
	inout float pLabelProbability
);

//----------------------------------------------
// PRNG (Pseudo-Random Number Generator) + Noise
//----------------------------------------------

int phi( int x );

float inoise( int ix, int iy, int iz );

float cnoise2D( float x, float y);

void seeding( uint x, uint y, uint z );

float next();

//--------------
// Point Process
//--------------

int PP_genPointSet( float x, float y,
	// point set parameters
	int pointsettype,
	float jitter,
	out float px[9 * 4], out float py[9 * 4], out float ncx[9 * 4], out float ncy[9 * 4], out float ndx[9 * 4], out float ndy[9 * 4] );

int PP_distribute( float px, float py,
	// point set parameters
	int tt, float psubx, float psuby, 
	int decalx, int Nx, int nrelax, float jitter,
	out float cx[9 * 4], out float cy[9 * 4], out float ncx[9 * 4], out float ncy[9 * 4], out float ndx[9 * 4], out float ndy[9 * 4] );
	
void PP_pavement( float x, float y,
	int tt, int decalx, int Nx,
	out float ccx[9], out float ccy[9], out float cdx[9], out float cdy[9] );
	
void PP_pave( float xp, float yp,
	// pavement parameters
	int Nx, float randx, float randy,
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] );

void PP_paved( float x, float y,
	// pavement parameters
	int Nx,
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] );
	
void PP_paveb(
	// position
	float x, float y,
	// pavement parameters
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] );
	
void PP_pavec(
	// position
	float x, float y,
	// pavement parameters
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] );
	
int PP_pointset(
	// point set parameters
	float psubx, float psuby, float jitx, float jity,
	float ccx[9], float ccy[9], float cdx[9], float cdy[9],
	out float cx[9 * 4], out float cy[9 * 4], out float ncx[9 * 4], out float ncy[9 * 4], out float ndx[9 * 4], out float ndy[9 * 4] );
	
void PP_nthclosest(
	out int mink[9 * 4], int nn, float xx, float yy, float cx[9 * 4], float cy[9 * 4], int nc, 
	float norm, float ncx[9 * 4], float ncy[9 * 4], float dx[9 * 4], float dy[9 * 4], float larp );

float PP_cdistance( float x1, float y1, float x2, float y2, float norm, 
	float cx, float cy, float dx, float dy, float larp );

void PP_relax(
	int nrelax, int n, float bx, float by, float dx, float dy,
	inout float cx[9 * 4], inout float cy[9 * 4],
	float bcx[9 * 4], float bcy[9 * 4], float dcx[9 * 4], float dcy[9 * 4] );
	
//-------------------

float celldist( float ixx, float iyy, int k, int mink, float cx[9 * 4], float cy[9 * 4], int nc,
	float norm, float cnx[9 * 4], float cny[9 * 4], float dx[9 * 4], float dy[9 * 4], float larp );
	
int cclosest( float xx, float yy, float cx[9 * 4], float cy[9 * 4], int nc, 
	float norm, float cnx[9 * 4], float cny[9 * 4], float dx[9 * 4], float dy[9 * 4], float larp );
	
float interTriangle( float origx, float origy, float ddx, float ddy, float startx, float starty, float endx, float endy );

void bezier2( float ts, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, out float splinex, out float spliney );
	
////////////////////////////////////////////////////////////////////////////////
// PRNG (Pseudo-Random Number Generator)
////////////////////////////////////////////////////////////////////////////////
	
//------------------------------------------------------------------------------
// phi
//------------------------------------------------------------------------------
int phi( int x )
{
	if ( x < 0 )
	{
		x = x + 10 * MAX_NOISE_RAND;
	}
	x = x % MAX_NOISE_RAND;
	
	return texelFetch( uPermutationTex, ivec2( x, 0 ), 0 ).r;
}

//------------------------------------------------------------------------------
// seeding
//------------------------------------------------------------------------------
void seeding( uint x, uint y, uint z )
{
	g_PRNG_seed = uint( phi( int( x ) + phi( int( y ) + phi( int( z ) ) ) ) % int( 1 << 15 ) + ( phi( 3 * int( x ) + phi( 4 * int( y ) + phi( int( z ) ) ) ) % int( 1 << 15 ) ) * int( 1 << 15 ) );
}

//------------------------------------------------------------------------------
// next
//------------------------------------------------------------------------------
float next()
{
	g_PRNG_seed *= uint( 3039177861 );
	float res = ( float( g_PRNG_seed ) / 4294967296.0 ) * 2.0 - 1.0;
	return res;
}

////////////////////////////////////////////////////////////////////////////////
// NOISE
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// inoise
//------------------------------------------------------------------------------
float inoise( int ix, int iy, int iz )
{
	int index = ( phi( ix ) + 3 * phi( iy ) + 5 * phi( iz ) ) % MAX_NOISE_RAND;
	
	return texelFetch( uNoiseTex, ivec2( index, 0 ), 0 ).r;
}

//------------------------------------------------------------------------------
// cnoise2D
//------------------------------------------------------------------------------
float cnoise2D( float x, float y)
{
	float rt;
	
	float vx0, vx1, vy0, vy1;
	int ix, iy;
	float sx, sy;
	
	ix = int( floor( x ) ); x -= ix;
	iy = int( floor( y ) ); y -= iy;

	sx = ( x * x * ( 3.0 - 2.0 * x ) ) ;
	sy = ( y * y * ( 3.0 - 2.0 * y ) ) ;

	vy0 = inoise( ix, iy, 0 );
	vy1 = inoise( ix, iy + 1, 0 );
	vx0 = mix( vy0, vy1, sy );
	
	vy0 = inoise( ix + 1, iy, 0 );
	vy1 = inoise( ix + 1, iy + 1, 0 );
	vx1 = mix( vy0, vy1, sy );
	
	rt = mix( vx0, vx1, sx );
	
	return rt;
}

////////////////////////////////////////////////////////////////////////////////
// POINT PROCESS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// PP_genPointSet
//------------------------------------------------------------------------------
int PP_genPointSet(
	// position
	float x, float y,
	// point set parameters
	int pointsettype,
	float jitter,
	out float px[9 * 4], out float py[9 * 4], out float ncx[9 * 4], out float ncy[9 * 4], out float ndx[9 * 4], out float ndy[9 * 4] )
{
	int tt; float ppointsub;
	int decalx; int Nx; int nrelax;
	
	switch ( pointsettype )
	{
		case 0: tt = PP_tilingType_REGULAR; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
		
		case 1: tt = PP_tilingType_REGULAR; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 0; break;
		
		case 2: tt = PP_tilingType_REGULAR; ppointsub = 1.0;  decalx = 1; Nx = 0; nrelax = 5; break;
		
		case 3: tt = PP_tilingType_REGULAR; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 5; break;
		
		case 4: tt = PP_tilingType_REGULAR; ppointsub = 0.0;  decalx = 2; Nx = 0; nrelax = 0; break;
		
		case 5: tt = PP_tilingType_REGULAR; ppointsub = 0.0;  decalx = 3; Nx = 0; nrelax = 0; break;
		
		case 6: tt = PP_tilingType_IRREGULAR; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
		
		case 7: tt = PP_tilingType_IRREGULAR; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 0; break;
		
		case 8: tt = PP_tilingType_IRREGULARX; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
		
		case 9: tt = PP_tilingType_IRREGULARX; ppointsub = 0.5;  decalx = 1; Nx = 0; nrelax = 0; break;
		
		case 10: tt = PP_tilingType_CROSS; ppointsub = 0.0;  decalx = 0; Nx = 2; nrelax = 0; break;
		
		case 11: tt = PP_tilingType_CROSS; ppointsub = 0.5;  decalx = 0; Nx = 2; nrelax = 0; break;
		
		case 12: tt = PP_tilingType_CROSS; ppointsub = 0.0;  decalx = 0; Nx = 3; nrelax = 0; break;
		
		case 13: tt = PP_tilingType_CROSS; ppointsub = 0.5;  decalx = 0; Nx = 3; nrelax = 0; break;
		
		case 14: tt = PP_tilingType_BISQUARE; ppointsub = 0.0;  decalx = 0; Nx = 1; nrelax = 0; break;
		
		case 15: tt = PP_tilingType_BISQUARE; ppointsub = 0.5;  decalx = 0; Nx = 1; nrelax = 0; break;
		
		case 16: tt = PP_tilingType_APAVEMENT; ppointsub = 0.0;  decalx = 0; Nx = 1; nrelax = 0; break;
		
		case 17: tt = PP_tilingType_APAVEMENT; ppointsub = 0.5;  decalx = 0; Nx = 1; nrelax = 0; break;
		
		default: tt = PP_tilingType_REGULAR; ppointsub = 0.0;  decalx = 1; Nx = 0; nrelax = 0; break;
	}
	
	// Compute points
	return PP_distribute( x, y, tt, ppointsub, ppointsub, decalx, Nx, nrelax, jitter, px, py, ncx, ncy, ndx, ndy );
}

//------------------------------------------------------------------------------
// PP_distribute
//------------------------------------------------------------------------------
int PP_distribute(
	// position
	float px, float py,
	// point set parameters
	int tt, float psubx, float psuby, 
	int decalx, int Nx, int nrelax, float jitter,
	out float cx[9 * 4], out float cy[9 * 4], out float ncx[9 * 4], out float ncy[9 * 4], out float ndx[9 * 4], out float ndy[9 * 4] )
{
	int i, k;
	float ccx[9]; float ccy[9]; float cdx[9]; float cdy[9];
	
	PP_pavement( px, py, tt, decalx, Nx, ccx, ccy, cdx, cdy );
	
	int np = 0;
	
	if ( nrelax == 0 )
	{
		np = PP_pointset( psubx, psuby, 0.9, 0.9, ccx, ccy, cdx, cdy, cx, cy, ncx, ncy, ndx, ndy );
	}
	else
	{
		for ( k = 0; k < 9; k++ )
		{
			float gccx[9]; float gccy[9]; float gcdx[9]; float gcdy[9];
			float gcx[9 * 4]; float gcy[9 * 4]; float gncx[9 * 4]; float gncy[9 * 4]; float gndx[9 * 4]; float gndy[9 * 4];
			PP_pavement( ccx[k], ccy[k], tt, decalx, Nx, gccx, gccy, gcdx, gcdy );
			int npk = PP_pointset( psubx, psuby, 0.9, 0.9, gccx, gccy, gcdx, gcdy, gcx, gcy, gncx, gncy, gndx, gndy );
			PP_relax( nrelax, npk, ccx[k], ccy[k], cdx[k], cdy[k], gcx, gcy, gncx, gncy, gndx, gndy );
			for ( i = 0; i < npk; i++ )
			{
				if ( gcx[ i ] >= ccx[ k ] - cdx[ k ] && gcx[ i ] <= ccx[ k ] + cdx[ k ] &&
					gcy[ i ] >= ccy[ k ] - cdy[ k ] && gcy[ i ] <= ccy[ k ] + cdy[ k ] )
				{
					cx[ np ] = gcx[ i ]; cy[ np ] = gcy[i];
					ncx[np] = gncx[i]; ncy[np] = gncy[i];
					ndx[np] = gndx[i]; ndy[np] = gndy[i];
					np++;
				}
			}
		}
	}
	
	/*
	float jitter_eps = jitter == 0.0 ? 0.015 : jitter;

	for ( i = 0; i < np; i++ )
	{
		cx[ i ] = cx[ i ] * jitter_eps + ncx[ i ] * ( 1.0 - jitter_eps );
		cy[ i ] = cy[ i ] * jitter_eps + ncy[ i ] * ( 1.0 - jitter_eps );
	}
	*/

	
	for ( i = 0; i < np; i++ )
	{
		cx[ i ] = cx[ i ] * jitter + ncx[ i ] * ( 1.0 - jitter );
		cy[ i ] = cy[ i ] * jitter + ncy[ i ] * ( 1.0 - jitter );
	}
	
	
	return np;
}

//------------------------------------------------------------------------------
// PP_pavement
//------------------------------------------------------------------------------
void PP_pavement( float x, float y,
	int tt, int decalx, int Nx,
	out float ccx[9], out float ccy[9], out float cdx[9], out float cdy[9] )
{
	switch ( tt )
	{
		case PP_tilingType_REGULAR: PP_pave( x, y, decalx, 0.0, 0.0, ccx, ccy, cdx, cdy ); break;
		
		case PP_tilingType_IRREGULAR: PP_pave( x, y, decalx, 0.8, 0.8, ccx, ccy, cdx, cdy ); break;
		
		case PP_tilingType_CROSS: PP_paved( x, y, Nx, ccx, ccy, cdx, cdy ); break;
		
		case PP_tilingType_BISQUARE: PP_paveb( x, y, ccx, ccy, cdx, cdy ); break;
		
		case PP_tilingType_IRREGULARX: PP_pave( x, y, decalx, 0.8, 0.0, ccx, ccy, cdx, cdy ); break;
		
		case PP_tilingType_IRREGULARY: PP_pave( x, y, decalx, 0.0, 0.8, ccx, ccy, cdx, cdy ); break;
		
		case PP_tilingType_APAVEMENT: PP_pavec( x, y, ccx, ccy, cdx, cdy ); break;
		
		default: PP_pave( x, y, decalx, 0.0, 0.0, ccx, ccy, cdx, cdy ); break;
	}
}

//------------------------------------------------------------------------------
// PP_pave
//------------------------------------------------------------------------------
void PP_pave(
	// position
	float xp, float yp,
	// pavement parameters
	int Nx, float randx, float randy,
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] )
{
	int i, j;
	int nc = 0;
	float x = xp;
	float y = yp;
	
	int ix = int( floor( x ) ); float xx = x - float( ix );
	int iy = int( floor( y ) ); float yy = y - float( iy );
	
	for ( j = -1; j <= +1; j++ )
	{
		for ( i = -1; i <= +1; i++ )
		{
			float rxi, rxs, ryi, rys;
			float ivpx = float( ix ) + float( i );
			float ivpy = float( iy ) + float( j );
			float decalx = float( int( ivpy )%Nx ) / float( Nx );
			seeding( uint( ivpx + 5.0 ), uint( ivpy + 10.0 ), 0u );
			rxi = next() * randx * 0.5;
			seeding( 3u, uint( ivpy + 10.0 ), 0u );
			ryi = next() * randy * 0.5;
			seeding( uint( ivpx + 1.0 + 5.0 ), uint( ivpy + 10.0 ), 0u );
			rxs = next() * randx * 0.5;
			seeding( 3u, uint( ivpy + 1.0 + 10.0 ), 0u );
			rys = next() * randy * 0.5;

			dx[ nc ] = 0.5 * ( rxs + 1.0 - rxi );
			dy[ nc ] = 0.5 * ( rys + 1.0 - ryi );
			cx[ nc ] = ivpx + decalx + rxi + dx[ nc ];
			cy[ nc ] = ivpy + ryi + dy[ nc ];
			nc++;
		}
	}
}

//------------------------------------------------------------------------------
// PP_paved
//------------------------------------------------------------------------------
void PP_paved( 
	float x, float y,
	// pavement parameters
	int Nx,
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] )
{
	int i, j;
	int ix = int( floor( x ) ); float xx = x - float( ix );
	int iy = int( floor( y ) ); float yy = y - float( iy );
	int qx = int( xx * float( 2 * Nx ) );
	int qy = int( yy * float( 2 * Nx ) );
	
	// horizontal
	if ( ( qx >= qy && qx <= qy + Nx - 1 ) || ( qx >= qy - 2 * Nx && qx <= qy + Nx - 1 - 2 * Nx ) )
	{
		int rx, ry;
		
		if ( qx >= qy && qx <= qy + Nx - 1 )
		{
			rx = qy; ry = qy;
		}
		else
		{
			rx = qy - 2 * Nx; ry = qy;
		}
		
		for ( i = 0; i < 3; i++ )
		{
			cx[ 3 * i ] = float( ix ) + ( float( rx ) + float( i - 1 ) + float( Nx ) * 0.5 ) / float( 2 * Nx );
			cy[ 3 * i ] = float( iy ) + ( float( ry ) + float( i - 1 ) + 0.5 ) / float( 2 * Nx );
			dx[ 3 * i ] = ( float( Nx ) * 0.5 ) / float( 2 * Nx );
			dy[ 3 * i ] = 0.5 / float( 2 * Nx );

			cx[ 3 * i + 1 ] = float( ix ) + ( float( rx ) + float( i - 2 ) + 0.5 ) / float( 2 * Nx );
			cy[ 3 * i + 1 ] = float( iy ) + ( float( ry ) + float( i - 1 ) + float( Nx ) * 0.5 ) / float( 2 * Nx );
			dx[ 3 * i + 1 ] = 0.5 / float( 2 * Nx );
			dy[ 3 * i + 1 ] = ( float( Nx ) * 0.5 ) / float( 2 * Nx );

			cx[ 3 * i + 2 ] = float( ix ) + ( float( rx ) + float( i - 1 ) + float( Nx ) + 0.5 ) / float( 2 * Nx );
			cy[ 3 * i + 2 ] = float( iy ) + ( float( ry ) + float( i ) - float( Nx ) * 0.5 ) / float( 2 * Nx );
			dx[ 3 * i + 2 ] = 0.5 / float( 2 * Nx );
			dy[ 3 * i + 2 ] = ( float( Nx ) * 0.5 ) / float( 2 * Nx );
		}
	}
	// vertical
	else
	{
		int rx, ry;
		if ( qy >= qx + 1 && qy <= qx + 1 + Nx - 1 )
		{
			rx = qx;
			ry = qx + 1;
		}
		else
		{
			rx = qx;
			ry = qx + 1 - 2 * Nx;
		}
		for ( i = 0; i < 3; i++ )
		{
			cx[ 3 * i ] = float( ix ) + ( float( rx ) + float( i - 1 ) + 0.5 ) / float( 2 * Nx );
			cy[ 3 * i ] = float( iy ) + ( float( ry ) + float( i - 1 ) + float( Nx ) * 0.5 ) / float( 2 * Nx );
			dx[ 3 * i ] = 0.5 / float( 2 * Nx );
			dy[ 3 * i ] = ( float( Nx ) * 0.5 ) / float( 2 * Nx );

			cx[ 3 * i + 1 ] = float( ix ) + ( float( rx ) + float( i - 1 ) + float( Nx ) * 0.5 ) / float( 2 * Nx );
			cy[ 3 * i + 1 ] = float( iy ) + ( float( ry ) + float( i - 2 ) + 0.5 ) / float( 2 * Nx );
			dx[ 3 * i + 1 ] = ( float( Nx)  * 0.5 ) / float( 2 * Nx );
			dy[ 3 * i + 1 ] = 0.5 / float( 2 * Nx );

			cx[ 3 * i + 2 ] = float( ix ) + ( float( rx ) + float( i - 1 ) - float( Nx ) * 0.5 ) / float( 2 * Nx );
			cy[ 3 * i + 2 ] = float( iy ) + ( float( ry ) + float( i - 1 ) + float( Nx - 1 ) + 0.5 ) / float( 2 * Nx );
			dx[ 3 * i + 2 ] = ( float( Nx ) * 0.5 ) / float( 2 * Nx );
			dy[ 3 * i + 2 ] = 0.5 / float( 2 * Nx );
		}
	}
}

//------------------------------------------------------------------------------
// PP_paveb
//------------------------------------------------------------------------------
void PP_paveb(
	// position
	float x, float y,
	// pavement parameters
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] )
{
	int i, j;
	int nc = 0;
	int ii, jj;
	
	int ix = int( floor( x ) ); float xx = x - float( ix );
	int iy = int( floor( y ) ); float yy = y - float( iy );
	int qx = int( xx * 5.0 );
	int qy = int( yy * 5.0 );
	
	for ( i = 0; i < 3; i++ ) for ( j=0; j<3; j++ )
	{
		if ( qx >= -2 + i * 2 + j && qx <= -2 + i * 2 + 1 + j
			&& qy >= 1 - i + 2 * j && qy <= 1 - i + 2 * j + 1 )
		{
			for ( ii = 0; ii <= 2; ii++ ) for ( jj = 0; jj <= 2; jj++ )
			{
				if ( ii == 1 || jj == 1 )
				{
					int rx = -2 + i * 2 + j -3 + ii * 2 + jj;
					int ry = 1 - i + 2 * j - 1 + jj * 2 - ii;
					dx[ nc ] = 1.0 / 5.0; dy[ nc ] = 1.0 / 5.0;
					cx[ nc ] = float( ix ) + float( rx ) / 5.0 + 1.0 / 5.0;
					cy[ nc ] = float( iy ) + float( ry ) / 5.0 + 1.0 / 5.0;
					nc++;
				}
			}
			
			int rx = -2 + i * 2 + j;
			int ry = 1 - i + 2 * j;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( rx - 1 ) / 5.0 + 0.5 / 5.0;
			cy[ nc ] = float( iy ) + float( ry ) / 5.0 + 0.5 / 5.0;
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( rx ) / 5.0 + 0.5 / 5.0;
			cy[ nc ] = float( iy ) + float( ry + 2 ) / 5.0 + 0.5 / 5.0;
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( rx + 2 ) / 5.0 + 0.5 / 5.0;
			cy[ nc ] = float( iy ) + float( ry + 1 ) / 5.0 + 0.5 / 5.0;
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( rx + 1 ) / 5.0 + 0.5 / 5.0;
			cy[ nc ] = float( iy ) + float( ry - 1 ) / 5.0 + 0.5 / 5.0;
			nc++;
			
			return;
		}
	}
	
	for ( i = 0; i < 3; i++ ) for ( j = 0; j < 2; j++ )
	{
		if ( qx == i * 2 + j && qy == 2 + 2 * j - i )
		{
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( qx )/ 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( qx - 2 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy + 1 ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( qx + 1 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy + 2 ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( qx - 1 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy - 2 ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 0.5 / 5.0; dy[ nc ] = 0.5 / 5.0;
			cx[ nc ] = float( ix ) + float( qx + 2 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy - 1 ) / 5.0 + dy[ nc ];
			nc++;

			dx[ nc ] = 1.0 / 5.0; dy[nc] = 1.0 / 5.0;
			cx[ nc ] = float( ix ) + float( qx - 2 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy - 1 ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 1.0 / 5.0; dy[ nc ] = 1.0 / 5.0;
			cx[ nc ] = float( ix ) + float( qx - 1 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy + 1 ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 1.0 / 5.0; dy[ nc ] = 1.0 / 5.0;
			cx[ nc ] = float( ix ) + float( qx + 1 ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy ) / 5.0 + dy[ nc ];
			nc++;
			dx[ nc ] = 1.0 / 5.0; dy[ nc ] = 1.0 / 5.0;
			cx[ nc ] = float( ix ) + float( qx ) / 5.0 + dx[ nc ];
			cy[ nc ] = float( iy ) + float( qy - 2 ) / 5.0 + dy[ nc ];
			nc++;
			
			return;
		}
	}
	
	// if here, error in paveb
	
	return;
}

//------------------------------------------------------------------------------
// PP_pavec
//------------------------------------------------------------------------------
void PP_pavec(
	// position
	float x, float y,
	// pavement parameters
	out float cx[9], out float cy[9], out float dx[9], out float dy[9] )
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
	float sy[9][9] = {	{1.0, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 1.0, 0.5 },
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
	
	int ix = int( floor( x ) ); float xx = x - float( ix );
	int iy = int( floor( y ) ); float yy = y - float( iy );
	int qx = int( xx * 4.0 );
	int qy = int( yy * 4.0 );
	int qq = 0;
	
	if ( qx >= 1 && qx <= 2 && qy >= 1 && qy <= 2 ) qq = 0;
	else if ( qx >= 0 && qx <= 2 && qy == 3 ) qq = 1;
	else if ( qx == 3 && qy == 3 ) qq = 2;
	else if ( qx == 3 && qy == 2 ) qq = 3;
	else if ( qx == 3 && qy == 1 ) qq = 4;
	else if ( qx == 3 && qy == 0 ) qq = 5;
	else if ( qx >= 1 && qx <= 2 && qy == 0 ) qq = 6;
	else if ( qx == 0 && qy >= 0 && qy <= 1 ) qq = 7;
	else if ( qx == 0 && qy == 2 ) qq = 8;
	
	for ( ii = 0; ii < 9; ii++ )
	{
		dx[ nc ] = sx[ qq ][ ii ] / 4.0; dy[ nc ] = sy[ qq ][ ii ] / 4.0;
		cx[ nc ] = float( ix ) + ddx[ qq ][ ii ] / 4.0 + dx[ nc ];
		cy[ nc ] = float( iy ) + ddy[ qq ][ ii ] / 4.0 + dy[ nc ];
		nc++;
	}
}

//------------------------------------------------------------------------------
// PP_pointset
//------------------------------------------------------------------------------
int PP_pointset(
	// point set parameters
	float psubx, float psuby, float jitx, float jity,
	float ccx[9], float ccy[9], float cdx[9], float cdy[9],
	out float cx[9 * 4], out float cy[9 * 4], out float ncx[9 * 4], out float ncy[9 * 4], out float ndx[9 * 4], out float ndy[9 * 4] )
{
	int i, j,k;
	
	int nc = 0;
	
	for ( k = 0; k < 9; k++ )
	{
		int ix = int( floor( ccx[ k ] ) ); float xx = ccx[ k ] - float( ix );
		int iy = int( floor( ccy[ k ] ) ); float yy = ccy[ k ] - float( iy );
		seeding( uint( int( floor( ccx[ k ] * 15.0 ) ) + 10 ), uint( int( floor( ccy[ k ] * 10.0 ) ) + 3 ), 0u );
		float subx = next() * 0.5 + 0.5;
		//float suby = next() * 0.5 + 0.5;
		float dif = cdx[ k ] - cdy[ k ]; if ( dif < 0.0 ) dif = -dif;
		if ( dif < 0.1 && ( subx < psubx ) ) // || suby < psuby ) )
		{
			float cutx = 0.5 + 0.2 * next() * jitx;
			float cuty = 0.5 + 0.2 * next() * jity;
			float ncdx, ncdy, nccx, nccy, rx, ry;
			
			ncdx = ( cutx * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( cuty * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[k] + ncdx;
			nccy = ccy[ k ] - cdy[k] + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;

			ncdx = ( ( 1.0 - cutx ) * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( cuty * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ( cutx * 2.0 * cdx[ k ] ) + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;

			ncdx = ( cutx * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( ( 1.0 - cuty ) * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ( cuty * 2.0 * cdy[ k ] ) + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;

			ncdx = ( ( 1.0 - cutx ) * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( ( 1.0 - cuty ) * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ( cutx * 2.0 * cdx[ k ] ) + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ( cuty * 2.0 * cdy[ k ] ) + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;
		}
		else if ( cdx[ k ] > cdy[ k ] + 0.1 && subx < psubx )
		{
			float cutx = 0.4 + 0.2 * ( next() * 0.5 + 0.5 );
			float cuty = 1.0;
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = ( cutx * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( cuty * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc]  = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;

			ncdx = ( ( 1.0 - cutx ) * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( cuty * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ( cutx * 2.0 * cdx[ k ] ) + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;
		}
		else if ( cdy[ k ] > cdx[ k ] + 0.1 && subx < psuby )
		{
			float cutx = 1.0;
			float cuty = 0.4 + 0.2 * ( next() * 0.5 + 0.5 );
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = ( cutx * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( cuty * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;

			ncdx = ( cutx * 2.0 * cdx[ k ] ) * 0.5;
			ncdy = ( ( 1.0 - cuty ) * 2.0 * cdy[ k ] ) * 0.5;
			nccx = ccx[ k ] - cdx[ k ] + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ( cuty * 2.0 * cdy[ k ] ) + ncdy;
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			ncx[ nc ] = nccx; ncy[ nc ] = nccy; ndx[ nc ] = ncdx; ndy[ nc ] = ncdy;
			nc++;
		}
		else
		{
			float rx = cdx[ k ] * next() * jitx;
			float ry = cdy[ k ] * next() * jity;
			cx[ nc ] = ccx[ k ] + rx;
			cy[ nc ] = ccy[ k ] + ry;
			ncx[ nc ] = ccx[ k ]; ncy[ nc ] = ccy[ k ]; ndx[ nc ] = cdx[ k ]; ndy[ nc ] = cdy[ k ];
			nc++;
		}
	}
	
	return nc;
}

//------------------------------------------------------------------------------
// PP_relax
//------------------------------------------------------------------------------
void PP_relax(
	int nrelax, int n, float bx, float by, float dx, float dy,
	inout float cx[9 * 4], inout float cy[9 * 4],
	float bcx[9 * 4], float bcy[9 * 4], float dcx[9 * 4], float dcy[9 * 4] )
{
	int i, j, k;
	float mcx[9 * 4], mcy[9 * 4];
	for ( i = 0; i < nrelax; i++ )
	{
		for ( k = 0; k < n; k++ ) 
			if ( cx[k] >= bx - dx && cx[k] <= bx + dx && cy[k] >= by - dy && cy[k] <= by + dy )
			{
				float distmin1 = 100000.0; int ind1 = 0;
				float distmin2 = 100000.0; int ind2 = 0;
				float distmin3 = 100000.0; int ind3 = 0;
				for ( j = 0; j < n; j++ ) if ( j != k )
				{
					float dd = sqrt((cx[k] - cx[j])*(cx[k] - cx[j]) + (cy[k] - cy[j])*(cy[k] - cy[j]));
					if (dd < distmin1) { distmin3 = distmin2; ind3 = ind2; distmin2 = distmin1; ind2 = ind1; distmin1 = dd; ind1 = j; }
					else if (dd < distmin2) { distmin3 = distmin2; ind3 = ind2; distmin2 = dd; ind2 = j; }
					else if (dd < distmin3) { distmin3 = dd; ind3 = j; }
				}
				float dx1 = cx[ ind1 ] - cx[ k ];
				float dy1 = cy[ ind1 ] - cy[ k ];
				float no1 = sqrt( dx1 * dx1 + dy1 * dy1 ); if ( no1 == 0.0 ) no1 = 1.0;
				float dx2 = cx[ ind2 ] - cx[ k ];
				float dy2 = cy[ ind2 ] - cy[ k ];
				float no2 = sqrt( dx2 * dx2 + dy2 * dy2 ); if ( no2 == 0.0 ) no2 = 1.0;
				float dx3 = cx[ ind3 ] - cx[ k ];
				float dy3 = cy[ ind3 ] - cy[ k ];
				float no3 = sqrt( dx3 * dx3 + dy3 * dy3 ); if ( no3 == 0.0 ) no3 = 1.0;
				float dirx = dx1 / no1 / no1 + dx2 / no2 / no2 + dx3 / no3 / no3;
				float diry = dy1 / no1 / no1 + dy2 / no2 / no2 + dy3 / no3 / no3;
				float no = sqrt( dirx * dirx + diry * diry ); if ( no == 0.0 ) no = 1.0;
				mcx[ k ] = cx[ k ] - ( dirx / no * 0.05 );
				mcy[ k ] = cy[ k ] - ( diry / no * 0.05 );
				if (mcx[ k ] < bcx[ k ] - dcx[ k ] + 0.05) mcx[ k ] = bcx[ k ] - dcx[ k ] + 0.05;
				if (mcx[ k ] > bcx[ k ] + dcx[ k ] - 0.05) mcx[ k ] = bcx[ k ] + dcx[ k ] - 0.05;
				if (mcy[ k ] < bcy[ k ] - dcy[ k ] + 0.05) mcy[ k ] = bcy[ k ] - dcy[ k ] + 0.05;
				if (mcy[ k ] > by + dy ) mcy[ k ] = bcy[ k ] + dcy[ k ] - 0.05;
			}
			else { mcx[ k ] = cx[ k ]; mcy[ k ] = cy[ k ]; }
		for ( k = 0; k < n; k++ ) { cx[ k ] = mcx[ k ]; cy[ k ] = mcy[ k ]; }
	}
}

//------------------------------------------------------------------------------
// PP_nthclosest
//------------------------------------------------------------------------------
void PP_nthclosest(
	out int mink[9 * 4], int nn, float xx, float yy, float cx[9 * 4], float cy[9 * 4], int nc, 
	float norm, float ncx[9 * 4], float ncy[9 * 4], float dx[9 * 4], float dy[9 * 4], float larp )
{
	int i,k;
	
	float dist[9 * 4];
	
	for ( k = 0; k < nc; k++ )
	{
		float dd = PP_cdistance( xx, yy, cx[ k ], cy[ k ], norm, ncx[ k ], ncy[ k ], dx[ k ], dy[ k ], larp );
		//float dx = xx - cx[k];
		//float dy = yy - cy[k];
		//float dd = (float)pow(pow(abs(dx), norm) + pow(abs(dy), norm), 1.0 / norm);
		dist[ k ] = dd;
	}
	
	for ( i = 0; i < nn; i++ )
	{
		int mk = 0;
		for ( k = 1; k < nc; k++ )
		{
			if ( dist[ mk ] > dist[ k ] ) mk = k;
		}
		mink[ i ] = mk; 
		dist[ mk ] = 100000.0;
	}
}

//------------------------------------------------------------------------------
// PP_cdistance
//------------------------------------------------------------------------------
float PP_cdistance( float x1, float y1, float x2, float y2, float norm, 
	float cx, float cy, float dx, float dy, float larp )
{
	//seeding((unsigned int)(cx*12.0 + 5.0), (unsigned int)(cy*12.0 + 11.0), 0);
	//float ss = size*next();
	float ddx = ( x1 - x2 );
	float ddy = ( y1 - y2 );
	float ex = ddx < 0.0 ? (-ddx) / ( x2 - cx + dx ): ddx / ( cx + dx - x2 );
	float ey = ddy < 0.0 ? (-ddy) / ( y2 - cy + dy ) : ddy / ( cy + dy - y2 );
	//float lx = ( 1.0 - larp ) * abs( ddx ) + larp * ex;
	//float ly = ( 1.0 - larp ) * abs( ddy ) + larp * ey;
	//float no = pow( pow( lx, norm ) + pow( ly, norm ), 1.0 / norm );
	//if ( norm <= 2.0 )
	//{
	//	return no;
	//}
	//else
	//{
	//	return ( norm - 2.0 ) * ( lx > ly ? lx : ly ) + ( 1.0 - ( norm - 2.0 ) ) * no;
	//}
	
	//printf("cdist: ddx=%g,ddy=%g, ex=%g, ey=%g, dx=%g,dy=%g\n", ddx, ddy, ex, ey, dx, dy);
	return ((1.0-larp)*pow(pow(abs(ddx), norm) + pow(abs(ddy), norm), 1.0 / norm)+larp*(ex > ey ? ex : ey));
}

//------------------------------------------------------------------------------
// interTriangle
//------------------------------------------------------------------------------
float interTriangle( float origx, float origy, float ddx, float ddy, float startx, float starty, float endx, float endy )
{
	float dirx = ( endx - startx );
	float diry = ( endy - starty );
	float dirno = sqrt( dirx * dirx + diry * diry );
	dirx /= dirno; diry /= dirno;
	float val = ddx * diry - ddy * dirx;
	float segx = -( startx - origx );
	float segy = -( starty - origy );
	float lambda = ( dirx * segy - diry * segx ) / val;
	
	return lambda;
}

//------------------------------------------------------------------------------
// bezier2
//------------------------------------------------------------------------------
void bezier2( float ts, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, out float splinex, out float spliney )
{
	float p01x = ts * p1x + ( 1.0 - ts ) * p0x;
	float p01y = ts * p1y + ( 1.0 - ts ) * p0y;
	float p11x = ts * p2x + ( 1.0 - ts ) * p1x;
	float p11y = ts * p2y + ( 1.0 - ts ) * p1y;
	splinex = ts * p11x + ( 1.0 - ts ) * p01x;
	spliney = ts * p11y + ( 1.0 - ts ) * p01y;
}

//------------------------------------------------------------------------------
// celldist
//------------------------------------------------------------------------------
float celldist( float ixx, float iyy, int k, int mink, float cx[9 * 4], float cy[9 * 4], int nc,
	float norm, float cnx[9 * 4], float cny[9 * 4], float dx[9 * 4], float dy[9 * 4], float larp )
{
	float delta = 0.2;
	int count, nk;
	float xx, yy, ddx, ddy, dd;
	
	do
	{
		xx = ixx; yy = iyy;
		ddx = cx[ k ] - xx; ddy = cy[ k ] - yy;
		dd = sqrt( ddx * ddx + ddy * ddy );
		if ( dd < 0.001 ) return 0.0;
		ddx *= delta / dd; ddy *= delta / dd;
		if ( k == mink ) { ddx = -ddx; ddy = -ddy; }
		//printf("start  with cell %d, %d, %g,%g, %g,%g,  %g,%g ->%g,%g\n", k, mink, ddx, ddy, ixx, iyy, cx[k], cy[k], xx, yy);
		//printf("cell is: %g,%g, (%g,%g)\n", cnx[k], cny[k], dx[k], dy[k]);
		//printf("mincell is: %g,%g, (%g,%g)\n", cnx[mink], cny[mink], dx[mink], dy[mink]);
		count = 0;
		//nk = cclosest(xx+ddx, yy+ddy, cx, cy, nc, norm, cnx, cny, dx, dy, larp);
		//if (!((k == mink && nk == k) || (k != mink&&nk != k))) {
		//	printf("start problem with cell, %d, %d, %g,%g, %g,%g,  %g,%g ->%g,%g\n", k, mink, dx, dy, ixx, iyy, cx[k], cy[k], xx, yy); 
		//}
		do
		{
			xx += ddx; yy += ddy;
			nk = cclosest( xx, yy, cx, cy, nc, norm, cnx, cny, dx, dy, larp );
			//if (count>97) printf("%d, nk=%d, xx=%g,yy=%g, delta=%g\n", count, nk, xx, yy, delta);
			count++;
		} while ( ( ( k == mink && nk == k ) || ( k != mink && nk != k ) ) && count < 100 );
		if ( count == 100 && delta <= 0.009 )
		{
			//printf("problem with cell, %d, %d, displ:%g,%g, \nfrom: %g,%g,  cell:%g,%g ->at: %g,%g\n", k, mink, ddx, ddy, ixx, iyy, cx[k], cy[k], xx, yy);
			//printf("in cell: %g,%g, (%g,%g)\n", cnx[k], cny[k], dx[k], dy[k]);
			// for ( int u = 0; u < nc; u++ ) if ( u != k )
				// printf("neighbor cell %d: %g,%g, (%g,%g)\n", u, cnx[u], cny[u], dx[u], dy[u]);
			// hvFatal("error");
		}
		delta /= 2.0;
	} while ( count == 100 && delta >= 0.01 );
	float xa = xx - ddx, ya = yy - ddy;
	float midx = ( xa + xx ) / 2.0, midy = ( ya + yy ) / 2.0;
	//printf("refine ddx=%g, ddy=%g, midx=%g,midy=%g, cx=%g,cy=%g,cnx=%g,cny=%g,dx=%g,dy=%g\n", ddx, ddy, midx, midy, cx[k], cy[k], cnx[k], cny[k], dx[k], dy[k]);
	for ( int i = 0; i < 5; i++ )
	{
		nk = cclosest( midx, midy, cx, cy, nc, norm, cnx, cny,dx, dy, larp );
		if ( ( ( k == mink && nk == k ) || ( k != mink && nk != k ) ) ) { xa = midx; ya = midy; }
		else { xx = midx; yy = midy; }
		midx = ( xa + xx ) / 2.0; midy = ( ya + yy ) / 2.0;
	}
	//float cdi = PP_cdistance( midx, midy, cx[ k ], cy[ k ], norm, cnx[ k ], cny[ k ], dx[ k ], dy[ k ], larp );
	//printf("%g : k=%d, mink=%d, ddx=%g, ddy=%g, midx=%g,midy=%g, cx=%g,cy=%g,cnx=%g,cny=%g,dx=%g,dy=%g\n", cdi, k, mink, ddx, ddy, midx, midy, cx[k], cy[k], cnx[k], cny[k], dx[k], dy[k]);
	//return cdi;
	float vdx = cx[k] - midx, vdy = cy[k] - midy;
	//return (float)pow(pow(abs(dx), norm) + pow(abs(dy), norm), 1.0 / norm);
	return sqrt(vdx*vdx + vdy*vdy);
}

//------------------------------------------------------------------------------
// cclosest
//------------------------------------------------------------------------------
int cclosest( float xx, float yy, float cx[9 * 4], float cy[9 * 4], int nc, 
	float norm, float cnx[9 * 4], float cny[9 * 4], float dx[9 * 4], float dy[9 * 4], float larp )
{
	int mink = 0;
	float mind = 0.0;
	int k;
	for ( k = 0; k < nc; k++ )
	{
		float dd = PP_cdistance( xx, yy, cx[ k ], cy[ k ], norm, cnx[ k ], cny[ k ], dx[ k ], dy[ k ], larp );
		//float dx = xx - cx[k];
		//float dy = yy - cy[k];
		//float dd = (float)pow(pow(abs(dx), norm) + pow(abs(dy), norm), 1.0 / norm);
		if ( k == 0 ) { mind = dd; }
		else if ( mind > dd ) { mink = k; mind = dd; }
	}
	
	return mink;
}

////////////////////////////////////////////////////////////////////////////////
// PPTBF
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// procedural_pptbf
//------------------------------------------------------------------------------
float procedural_pptbf(
	// position
	float xx, float yy,
	// model transform
	float zoom, float alpha, float rescalex,
	// deformation
	float distortamp, float distortfact, float distortfreq,
	// point set parameters
	int tt,
	float jitter,
	// window function parameters
	int winshape,
	float arity, // voronoi cell polygon arity
	float larp,  // anisotropy of cell norm
	float wsmooth,
	float norm, float normblend, float normsig,
	// feature function parameters
	int bomb,
	float normfeat,  float winfeatcorrel, float feataniso,
	int Npmin, int Npmax, float sigcos, float sigcosvar,
	int freq, float phase, float thickness, float courbure, float deltaorient,
	inout float pLabelProbability
)
{
	float pptbf = 0.0;
	
	int i, k;

	// Translation
	xx = xx + uShiftX;
	yy = yy + uShiftY;
	
	//----------------
	// [1] Deformation
	//----------------
	
	// Apply turbulence (i.e. noise-based spatial distorsion)
	// - x position
	float ppx = xx + distortamp * cnoise2D( distortfreq*xx*zoom*0.5 +2.0, distortfreq*yy*zoom*0.5 )
			+ distortamp * distortfact * cnoise2D( distortfreq*xx*zoom+2.0, distortfreq*yy*zoom)
			+ distortamp * distortfact * distortfact * cnoise2D( distortfreq*xx*zoom*2.0+2.0, distortfreq*yy*zoom*2.0 )
			+ distortamp * distortfact * distortfact * distortfact * cnoise2D( distortfreq*xx*zoom*4.0 + 2.0, distortfreq*yy*zoom*4.0 );
	// - y position
	float ppy = yy + distortamp * cnoise2D( distortfreq*xx*zoom*0.5 , distortfreq*yy*zoom*0.5+5.0  )
			+ distortamp * distortfact * cnoise2D( distortfreq*xx*zoom, distortfreq*yy*zoom+5.0 )
			+ distortamp * distortfact * distortfact * cnoise2D( distortfreq*xx*zoom*2.0, distortfreq*yy*zoom*2.0+5.0 )
			+ distortamp * distortfact * distortfact * distortfact * cnoise2D( distortfreq*xx*zoom*4.0, distortfreq*yy*zoom*4.0 + 5.0 );
			
	//--------------------
	// [2] Model Transform
	//--------------------
	
	// Apply other transforms: rescale, rotation and zoom
	// Apply also a shift +100.0 to avoid negative coordinates
	float x = 100.0+( ppx * cos( -alpha ) + ppy * sin( -alpha ) ) / rescalex * zoom;
	float y = 100.0+( -ppx * sin( -alpha ) + ppy * cos( -alpha ) ) * zoom;
	
	//------------------
	// [3] Point Process
	//------------------
	
	// Distribute points
	// - feature points (in tiles)
	float px[9 * 4], py[9 * 4];
	// - tile centers
	float ncx[9 * 4], ncy[9 * 4];
	// - tile sizes
	float ndx[9 * 4], ndy[9 * 4];
	int nc = PP_genPointSet( x, y, tt, jitter, px, py, ncx, ncy, ndx, ndy );
	
	// Sort points
	int npp = ( nc < PP_nbMaxNeighbors ? nc : PP_nbMaxNeighbors ); // TODO: check if some windows can overlap after "PP_nbMaxNeighbors"
	int mink[ 9 * 4 ]; // for GLSL compilation...
	PP_nthclosest( mink, npp, x, y, px, py, nc, norm, ncx, ncy, ndx, ndy, larp );
	
	// Labeling
	seeding( uint( px[ mink[ 0 ] ] * 12.0 + 7.0 ), uint( py[ mink[ 0 ] ] * 12.0 + 1.0 ), 0u );
	pLabelProbability = 0.5 * next() + 0.5;
		
	//-------------------------------------------------------------------------------------
	// [4] PPTBF = PP x ( W F )
	//
	// Convolution of a point process and the product of a window W and feature F functions
	//-------------------------------------------------------------------------------------
	
	float vv = 0.0f; // final value, to be computed and returned
	float vv2 = 0.0f; // for second window
	float winsum = 0.0f; // for normalization
	float pmax = -1.0f; // initial lowest priority for bombing
	float closestdist = -100.0f; // initial value for min distance
	for ( k = 0; k < npp; k++ ) // for each neighbor cell / point
	{
		
		// generate all random values of the cell / point
		seeding( uint( px[ mink[ k ] ] * 12.0 + 7.0 ), uint( py[ mink[ k ] ] * 12.0 + 1.0 ), 0u );
		float dalpha = 2.0 * M_PI / pow( 2.0, float(uint(arity+0.5)) );
		float rotalpha = dalpha * ( next() * 0.5 + 0.5 );
		//float npointvalue = mval + vval*next();
		//if (k == 0) pointvalue = npointvalue;
		int nn = Npmin + int( float( Npmax - Npmin ) * ( 0.5 * next() + 0.5 ) );
		// if ( nn > 20 )
		// {
			// printf("Npmin=%d, Npmax=%d\n", Npmin, Npmax); hvFatal("stop");
		// }
		
		//-----------------------
		// [5] Window Function: W
		//-----------------------
	
		// compute Window function: it is a blend between cellular and classical window functions
		
		////////////////////////////////////////////////////////////////////////
		float ddx = ( x - px[ mink[ k ] ] ); // / ndx[mink[k]];
		float ddy = ( y - py[ mink[ k ] ] ); // / ndy[mink[k]];
		float sdd = sqrt( ddx * ddx + ddy * ddy );
		float sddno = pow( pow( abs(ddx), norm ) + pow( abs(ddy), norm ), 1.0 / norm );
		if ( norm > 2.0 )  sddno = ( norm - 2.0 ) * ( abs(ddx) > abs(ddy) ? abs(ddx) : abs(ddy) ) + ( 1.0 - ( norm - 2.0 ) ) * sddno;	
		float gauss = 1.0;
		float foot[ 4 ] = { 1.5, 1.0, 1.0, 1.0 };
		float footprint = foot[ winshape ]; 
		if (tt>=10) footprint *= 0.4;

		switch ( winshape )
		{
			//case 0: if ( sddno < 1.0 * footprint / 4.0 ) gauss = 1.0; // tapered cosine window
			//		else if ( sddno < footprint ) gauss = 0.5 * ( 1.0 + cos( M_PI * 4.0 / footprint / 3.0 * ( sddno - footprint / 4.0 ) ) );
			//		else gauss = 0.0;
			//		break;
			case 0:
			case 1: gauss = ( exp( -2.0 * sddno ) - exp( -2.0 * footprint ) ) / ( 1.0 - exp( -2.0 * footprint ) ); break; // clamped gaussian
			case 2: gauss = 1.0 - sddno / footprint; break; // triangular window
			case 3: if (sddno > footprint) gauss = 0.0; break; // rectangular window
			
			default: gauss = 1.0 - sddno / footprint; break; // triangular window
		}
		if ( gauss < 0.0 ) gauss = 0.0; else if ( gauss > 1.0 ) gauss = 1.0; // clamp 
		
		//--------------------
		// [6] Cellular Window
		//--------------------
		
		// compute cellular distance value
		float cv = 0.0;
		//if ( k == 0 ) // if inside closest cell then compute cellular value, outside it is 0
		
		if ( k == 0 && sdd<0.0001 ) { cv=1.0; } // if on center of cell
        else if ( k == 0 ) // else if inside closest cell then compute cellular value, outside it is 0 
		{
			// Old version: single distance
			/*
			float celldd = celldist(x, y, mink[k], mink[0], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp);
			//float dd = hvNoise::cdistance(x, y, px[mink[k]], py[mink[k]], norm, ncx[mink[k]], ncy[mink[k]], ndx[mink[k]], ndy[mink[k]], larp);
			float dd = sdd;
			float dist = celldd < 0.0001 ? 0.0 : dd / (celldd);
			cv = 1.0 - dist;
			//printf("it celldd=%g, dd=%g, dist=%g, cv=%g\n", celldd, dd,dist,cv);
			if (cv < 0.0) cv = 0.0; else if (cv > 1.0) cv = 1.0;
			*/

			
			// new version: apply a smooth cellular function
			//////////////////////////////////////////////////////////
			// compute position inside cell in  polar coordinates
			ddx /= sdd; ddy /= sdd; 
			float dd = sdd;
			//float alpha = acos( ddx );
			float alpha = acos( clamp(ddx,-1.0, 1.0) );
			
			if ( ddy < 0.0 ) alpha = 2.0 * M_PI - alpha;
			float palpha = alpha - rotalpha;
			if ( palpha < 0.0 ) palpha += 2.0 * M_PI;
			int ka = int( palpha / dalpha );
			
			float rka = palpha / dalpha - float( ka );
			//printf("(%g,%g), (%g,%g) sdd=%g, ka=%d, alpha=%g\n", x,y,ddx, ddy, dd, ka, alpha/M_PI);
			float ptx = px[ mink[ 0 ] ] + 0.1 * cos( dalpha * float( ka ) + rotalpha );
			float pty = py[ mink[ 0 ] ] + 0.1 * sin( dalpha * float( ka ) + rotalpha );
			float celldd1 = celldist( ptx, pty, mink[ k ], mink[ 0 ], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp );
			//
			
			float startx = px[ mink[ 0 ] ] + celldd1 * cos( dalpha * float( ka ) + rotalpha );
			float starty = py[ mink[ 0 ] ] + celldd1 * sin( dalpha * float( ka ) + rotalpha );
			ptx = px[ mink[ 0 ] ] + 0.1 * cos( dalpha * float( ka ) + dalpha + rotalpha );
			pty = py[ mink[ 0 ] ] + 0.1 * sin( dalpha * float( ka ) + dalpha + rotalpha );
			float celldd2 = celldist( ptx, pty, mink[ k ], mink[ 0 ], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp );
			//
			
			float endx = px[ mink[ 0 ] ] + celldd2 * cos( dalpha * float( ka ) + dalpha + rotalpha );
			float endy = py[ mink[ 0 ] ] + celldd2 * sin( dalpha * float( ka ) + dalpha + rotalpha );

			// for smoothing the cell using Bezier
			float midx = ( startx + endx ) / 2.0;
			float midy = ( starty + endy ) / 2.0;
			float middx = ( midx - px[ mink[ 0 ] ] );
			float middy = ( midy - py[ mink[ 0 ] ] );
			float midno = sqrt( middx * middx + middy * middy );
			middx /= midno; middy /= midno;
			//printf("acos=%g, dalpha=%g\n", acos(middx*ddx + middy*ddy) / M_PI, dalpha/M_PI);
			//float midalpha = acos( middx );
			float midalpha = acos( clamp(middx,-1.0, 1.0) );
			
			if ( middy < 0.0 ) midalpha = 2.0 * M_PI - midalpha;
			//midalpha = midalpha - rotalpha;
			//if (midalpha >2.0*M_PI) midalpha -= 2.0*M_PI;
			float diff = alpha - midalpha; if ( diff < 0.0 ) diff = -diff;
			if ( diff > 2.0 * dalpha && alpha < 2.0 * dalpha ) midalpha -= 2.0 * M_PI;
			else if ( diff > 2.0 * dalpha && alpha > 2.0 * M_PI - 2.0 * dalpha ) midalpha += 2.0 * M_PI;
			//printf("alpha=%g, midalpha=%g, diff=%g \n", alpha / M_PI, midalpha / M_PI, alpha / M_PI- midalpha / M_PI);
			float splinex, spliney;
			float smoothx, smoothy;
			if ( alpha > midalpha )
			{
				//printf("up\n");
				ptx = px[ mink[ 0 ] ] + 0.1 * cos( dalpha * float( ka ) + 2.0 * dalpha + rotalpha );
				pty = py[ mink[ 0 ] ] + 0.1 * sin( dalpha * float( ka ) + 2.0 * dalpha + rotalpha );
				float celldd = celldist( ptx, pty, mink[ k ], mink[ 0 ], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp );
				// ADDED BY REMI
				// if (celldd < 0.0001) celldd = 0.0;
				
				float nendx = px[ mink[ 0 ] ] + celldd * cos( dalpha * float( ka ) + 2.0 * dalpha + rotalpha );
				float nendy = py[ mink[ 0 ] ] + celldd * sin( dalpha * float( ka ) + 2.0 * dalpha + rotalpha );
				float vvx = ( endx - startx ), vvy = ( endy - starty );
				float nn = sqrt( vvx * vvx + vvy * vvy ); vvx /= nn; vvy /= nn;
				float wwx = ( nendx - endx ), wwy = ( nendy - endy );
				nn = sqrt( wwx * wwx + wwy * wwy ); wwx /= nn; wwy /= nn;
				nendx = ( nendx + endx ) / 2.0; nendy = ( nendy + endy ) / 2.0;
				
				float lambda = interTriangle( px[ mink[ 0 ] ], py[ mink[ 0 ] ], ddx, ddy, midx, midy, nendx, nendy );
				float bordx = ddx * lambda + px[ mink[ 0 ] ];
				float bordy = ddy * lambda + py[ mink[ 0 ] ];
				float dirno = sqrt( ( nendx - midx ) * ( nendx - midx ) + ( nendy - midy ) * ( nendy - midy ) );
				float ts = sqrt( ( bordx - midx ) * ( bordx - midx ) + ( bordy - midy ) * ( bordy - midy ) );
				ts /= dirno;
				bezier2( ts, midx, midy, endx, endy, nendx, nendy, splinex, spliney );
				//hvNoise::splineCubic(ts, midx, midy, nendx, nendy, vvx, vvy, wwx, wwy, splinex, spliney);
				smoothx = bordx; smoothy = bordy;
			}
			else
			{
				//printf("low\n");
				ptx = px[ mink[ 0 ] ] + 0.1 * cos( dalpha * float( ka ) - dalpha + rotalpha );
				pty = py[ mink[ 0 ] ] + 0.1 * sin( dalpha * float( ka ) - dalpha + rotalpha );
				float celldd = celldist( ptx, pty, mink[ k ], mink[ 0 ], px, py, nc, 2.0, ncx, ncy, ndx, ndy, larp );
				// ADDED BY REMI
				//if (celldd < 0.0001) celldd = 0.0;
				
				float nstartx = px[ mink[ 0 ] ] + celldd * cos( dalpha * float( ka ) - dalpha + rotalpha );
				float nstarty = py[ mink[ 0 ] ] + celldd * sin( dalpha * float( ka ) - dalpha + rotalpha );
				float vvx = ( startx - nstartx ), vvy = ( starty - nstarty );
				float nn = sqrt( vvx * vvx + vvy * vvy ); vvx /= nn; vvy /= nn;
				float wwx = ( endx - startx ), wwy = ( endy - starty );
				nn = sqrt( wwx * wwx + wwy * wwy ); wwx /= nn; wwy /= nn;
				nstartx = ( nstartx + startx ) / 2.0; nstarty = ( nstarty + starty ) / 2.0;
				float lambda = interTriangle( px[ mink[ 0 ] ], py[ mink[ 0 ] ], ddx, ddy, nstartx, nstarty, midx, midy );
				float bordx = ddx * lambda + px[ mink[ 0 ] ];
				float bordy = ddy * lambda + py[ mink[ 0 ] ];
				float dirno = sqrt( ( midx - nstartx ) * ( midx - nstartx ) + ( midy - nstarty ) * ( midy - nstarty ) );
				float ts = sqrt( ( bordx - nstartx ) * ( bordx - nstartx ) + ( bordy - nstarty ) * ( bordy - nstarty ) );
				ts /= dirno;
				bezier2( ts, nstartx, nstarty, startx, starty, midx, midy, splinex, spliney );
				//hvNoise::splineCubic(ts, nstartx, nstarty, midx, midy, vvx, vvy, wwx, wwy, splinex, spliney);
				smoothx = bordx; smoothy = bordy;
			}

			float lambda = interTriangle( px[ mink[ 0 ] ], py[ mink[ 0 ] ], ddx, ddy, startx, starty, endx, endy );
			float bordx = ddx*lambda + px[ mink[ 0 ] ];
			float bordy = ddy*lambda + py[ mink[ 0 ] ];

			float smoothdist = sqrt( ( smoothx - px[ mink[ 0 ] ] ) * ( smoothx - px[ mink[ 0 ] ] ) + ( smoothy - py[ mink[ 0 ] ] ) * ( smoothy - py[ mink[ 0 ] ] ) );
			float splinedist = sqrt( ( splinex - px[ mink[ 0 ] ] ) * ( splinex - px[ mink[ 0 ] ] ) + ( spliney - py[ mink[ 0 ] ] ) * ( spliney - py[ mink[ 0 ] ] ) );
			
			//float squaredist = dd/lambda;
			//if (palpha <= M_PI / 4.0 || palpha >= 2.0*M_PI - M_PI / 4.0 || (palpha >= M_PI - M_PI / 4.0 && palpha <= M_PI + M_PI / 4.0))
			//	squaredist *= sqrt(tan(palpha)*tan(palpha) + 1.0);
			//else
			//	squaredist *= sqrt(tan(palpha - M_PI / 2.0)*tan(palpha - M_PI / 2.0) + 1.0);
			//float px = squaredist*cos(palpha);
			//float py = squaredist*sin(palpha);
			
			// ADDED BY REMI
			//float dist = 0.0;
			//if (smoothdist < 1.0) dist = dd;
			//else dist = dd / smoothdist;
			//
			
			if ( wsmooth < 1.0 ) cv = ( 1.0 - wsmooth ) * ( 1.0 - dd / lambda ) + wsmooth * ( 1.0 - dd / smoothdist );
			else if ( wsmooth < 2.0 ) cv = ( 2.0 - wsmooth ) * ( 1.0 - dd / smoothdist ) + ( wsmooth - 1.0 ) * ( 1.0 - dd / splinedist );
			else cv = 1.0 - dd / splinedist;
			
			//cv = 1.0 - dist;
			if ( cv < 0.0 ) cv = 0.0; else if ( cv > 1.0 ) cv = 1.0;
		}
		
		// compute the cellular window as a weighted sum of two windows
		//float coeff1 = normblend * pow( cv, normsig );
		float coeff1 = normblend * (exp( (cv-1.0)*normsig )- exp(-1.0*normsig))/(1.0-exp(-1.0*normsig));
		float coeff2 = ( 1.0 - normblend ) * gauss;
		//winsum += coeff;
		winsum += coeff1+coeff2;
		
		//---------------------
		// [7] Feature Function
		//---------------------
		
		// Feature function, by default is constant=1.0
		float feat = 0.0;
		float lx[20], ly[20], deltalx[20], deltaly[20], angle[20], prior[20], sigb[20], valb[20]; // , sigma[20];
		
		seeding( uint( px[ mink[ k ] ] * 15.0 + 2.0 ), uint( py[ mink[ k ] ] * 15.0 + 5.0 ), 0u );
		//valb[ 0 ] = next();
		//prior[ 0 ] = next() * 0.5 + 0.5;
		//lx[ 0 ] = px[ mink[ k ] ];
		//ly[ 0 ] = py[ mink[ k ] ];
		//deltalx[ 0 ] = ( x - lx[ 0 ] ) / ndx[ mink[ k ] ];
		//deltaly[ 0 ] = ( y - ly[ 0 ] ) / ndy[ mink[ k ] ];
		//angle[ 0 ] = deltaorient * next();
		//sigb[ 0 ] = sigcos * ( 1.0 + ( next() + 1.0 ) / 2.0 * sigcosvar );
		for ( int i = 0; i < nn; i++ )
		{
			valb[ i ] = next();
			prior[ i ] = next() * 0.5 + 0.5;
			lx[ i ] = ncx[ mink[ k ] ] + next() * 0.99 * ndx[ mink[ k ] ];
			ly[ i ] = ncy[ mink[ k ] ] + next() * 0.99 * ndy[ mink[ k ] ];
			lx[ i ] = winfeatcorrel*px[ mink[ k ] ] + (1.0-winfeatcorrel)*lx[i];
			ly[ i ] = winfeatcorrel*py[ mink[ k ] ] + (1.0-winfeatcorrel)*ly[i];
			deltalx[ i ] = ( x - lx[ i ] ) / ndx[ mink[ k ] ];
			deltaly[ i ] = ( y - ly[ i ] ) / ndy[ mink[ k ] ];
			angle[ i ] = deltaorient * next();
			sigb[ i ] = sigcos * ( 1.0 + ( next() + 1.0 ) / 2.0 * sigcosvar );
			//sigma[i] = (sigcos + sigcosvar*next())*(1.0 + 2.0*(float)nn / 9.0);
			//if (k==0) printf("n=%d, daa=%g, mdist=%g, x=%g, y=%g\n", i, daa, mdist, lx[i], ly[i]);
		}		
		//nn = hvNoise::featureFunc(nn, bomb, deltaorient, jitter, sigcos, sigcosvar, x, y, px[mink[k]], py[mink[k]], ncx[mink[k]], ncy[mink[k]], ndx[mink[k]], ndy[mink[k]],
		//	lx, ly, deltalx, deltaly, angle, prior, sigb, valb);
		
		feat = ( (bomb == 0 || bomb == 5) ? 1.0: 0.0 );
		for ( i = 0; i < nn; i++ )
		{
			//float iddy = deltaly[ i ] / pow(2.0,feataniso);
			//float ddx = ( deltalx[ i ] * cos( -angle[ i ] ) +  iddy * sin( -angle[ i ] ) );
			//float ddy = ( - deltalx[ i ] * sin( -angle[ i ] ) +  iddy * cos( -angle[ i ] ) );
			float ddx = ( deltalx[ i ] * cos( -angle[ i ] ) + deltaly[ i ] * sin( -angle[ i ] ) );
			float iddy = ( - deltalx[ i ] * sin( -angle[ i ] ) + deltaly[ i ] * cos( -angle[ i ] ) );
			float ddy = iddy / pow(2.0,feataniso);
			float dd2 = pow( pow( abs( ddx ), normfeat ) + pow( abs( ddy ), normfeat ), 1.0 / normfeat );
			if ( normfeat > 2.0 ) dd2 = ( normfeat - 2.0 ) * ( abs( ddx ) > abs( ddy ) ? abs( ddx ) : abs( ddy ) ) + ( 1.0 - ( normfeat - 2.0 ) ) * dd2;
			float ff = 0.5 + 0.5 * cos( M_PI * float( freq ) * sqrt( ddx * ddx * courbure * courbure + ddy * ddy ) );
			ff = pow( ff, 1.0f / (0.0001+thickness) );
			float ddist = dd2 / ( footprint / sigb[ i ] ); 
			float dwin = 1.0 - ddist; 
			if ( dwin < 0.0 ) dwin = 0.0;
			if ( bomb == 1 )
			{
				float val = ( valb[ i ] < 0.0 ? -0.25 + 0.75 * valb[ i ] : 0.25 + 0.75 * valb[ i ] );
				feat += ff * val * exp( -ddist );
				//if (k == 0) printf("i=%d, dd2=%g, feat=%g\n", i, dd2, feat);
			}
			else if ( bomb == 2 ) feat += ff * exp( -ddist );
			else if ( bomb == 3 )
			{
				//printf("cell %d: at %g,%g, coeff=%g, feat=%g, prior=%g, max=%g\n", k, px[mink[k]], py[mink[k]], coeff, feat, prior, pmax);
				// add contributions, except if bombing
				if ( pmax < prior[ i ] && dwin > 0.0 && ff>0.5 )
				{
					pmax = prior[ i ]; vv = 2.0*(ff-0.5) * exp( -ddist );
					//pointvalue = npointvalue;
					//centerx = lx[i]; centery = ly[i];
				}
			}
			else if ( bomb == 4 )
			{
				float ww = ff* exp( -ddist );
				if ( closestdist < ww )
				{
					vv = ww; closestdist = ww; //( 1.0 - dd2 / ( footprint ) );
					//pointvalue = npointvalue;
					//centerx = lx[i]; centery = ly[i];
				}
			}

			else feat = 1.0f;
		}
		
		if ( bomb == 1 ) feat = 0.5 * feat + 0.5; // normalization
		if ( bomb == 2) feat /= float( Npmax / 2 > 0 ? Npmax / 2 : 1 ); // normalization
		// apply phase which should be 0.0 or PI/2
		if ( bomb != 0) feat = feat * cos( phase ) * cos( phase ) + ( 1.0 - feat ) * sin( phase ) * sin( phase );
		//if ( feat < 0.0 ) feat = 0.0;
	
		//float correl = 1.0 - sddno / (footprint); if(correl<0.0) correl=0.0;
		//if ((1.0-winfeatcorrel) >= correl) correl=1.0;
		//else if ((1.0-winfeatcorrel) < correl*0.8) correl=0.0;
		//else correl = ((1.0-winfeatcorrel)-correl*0.8)/(correl*0.2);
		//if ( (bomb !=3 && bomb!=4) || winshape != 0 ) vv += coeff * ( 1.0 * ( 1.0 - correl ) + correl * feat );
		
		if ( bomb !=3 && bomb!=4 ) { vv += (coeff1+coeff2) * feat;  }
		//printf("k=%d, coeff=%g, feat=%g, vv=%g\n", k, coeff, feat, vv);
	}
	
	if ( ( bomb != 3 && bomb!=4) && winshape == 0 && winsum > 0.0 ) vv /= winsum;
	
	//if ( ( bomb != 3 && bomb!=4) && winshape == 0 && winsum > 0.0 ) vv = vv + vv2/winsum;
	//else if (bomb != 3 && bomb!=4) vv = vv + vv2;
	//if (  bomb != 3 && bomb!=4) vv += vv2/winsum;
	//else vv += vv2;
	if ( vv < 0.0 ) vv = 0.0;
	
	// ADDED BY REMI
	//if (vv > 0.8) vv = 1.0;
	
	//if (vv == 1.0) vv = 0.0;
			
	pptbf = vv;
	
	return pptbf;
}
 
////////////////////////////////////////////////////////////////////////////////
// Main Function
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
void main()
{
	// Bounds check for unneeded threads
	ivec2 imSize = imageSize( uPPTBFImage );
	ivec2 gid = ivec2( gl_GlobalInvocationID.xy );
	if ( gid.x < imSize.x && gid.y < imSize.y )
	{
		// PPTBF value
		float pptbf = 0.0;
		
		// Compute PROCEDURAL PPTBF
		float zoom = float( imSize.x ) / float( uResolution );
		float posx = float( gid.x ) / float( imSize.x );
		float posy = float( gid.y ) / float( imSize.y );
		
		float labelProbability;
		
		pptbf = procedural_pptbf(
			// position
			posx, posy,
			// model transform
			zoom, uRotation, uRescaleX,
			// deformation
			uTurbulenceAmplitude_0, uTurbulenceAmplitude_1, uTurbulenceAmplitude_2,
			// point set 
			uPointProcessTilingType, uPointProcessJitter,
			// window function
			uWindowShape, uWindowArity, uWindowLarp, uWindowSmooth,
			uWindowNorm, uWindowBlend, uWindowSigwcell,
			// feature function
			uFeatureBomb,
			uFeatureNorm, uFeatureWinfeatcorrel, uFeatureAniso,
			uFeatureNpmin, uFeatureNpmax, uFeatureSigcos, uFeatureSigcosvar,
			uFeatureFreq, uFeaturePhase, uFeatureThickness, uFeatureCourbure, uFeatureDeltaorient,
			// labeling
			labelProbability
		);
		
		// Write PPTBF value
		imageStore( uPPTBFImage, gid/*coordinate*/, vec4( pptbf, 0.0, 0.0, 0.0 )/*data*/ );
		
		// Labeling
		// - "on-the-fly" generation: simple "random" approach
		uint label = uint( float( uNbLabels ) * max( labelProbability, 0.0 ) );
		imageStore( uLabelImage, gid/*coordinate*/, uvec4( label, 0, 0, 0 )/*data*/ );
		// - random value in [0;1]
		imageStore( uRandomValueImage, gid/*coordinate*/, vec4( max( labelProbability, 0.0 ), 0, 0, 0 )/*data*/ );
	}
}
