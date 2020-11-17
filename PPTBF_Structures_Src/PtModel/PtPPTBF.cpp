/*
 * Publication: Semi-Procedural Textures Using Point Process Texture Basis Functions
 * Authors: anonymous
 *
 * Code author: Pascal Guehl
 *
 * anonymous
 * anonymous
 */

/** 
 * @version 1.0
 */

#include "PtPPTBF.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>
#include <cmath>

// Project
#include "PtWindow.h"
#include "PtFeature.h"
#include "PtNoise.h"

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Pt;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/
	
/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

float PtPPTBF::getNorm( float inorm )
{
	return inorm < 0.5f ? 1.0f + std::pow( inorm / 0.5f, 0.2f ) : 2.0f + std::pow( ( inorm - 0.5f ) / 0.5f, 5.0f ) * 50.0f;
}

/******************************************************************************
 * Constructor
 ******************************************************************************/
PtPPTBF::PtPPTBF()
:	mWindow( nullptr )
,	mFeature( nullptr )
,	mNoise( nullptr )
,	mWidth( 640 )
,	mHeight( 480 )
// Model transform
,	mResolution( 100 )
,	mShiftX( 0.f )
,	mShiftY( 0.f )
,	mAlpha( 0.f )
,	mRescalex( 1.f )
	// Deformation
,	mTurbulenceAmplitude0( 0.f )
,	mTurbulenceAmplitude1( 0.f )
,	mTurbulenceAmplitude2( 0.f )
	// Point process
,	mTilingType( PtPPTBF::REGULAR )
,	mJittering( 0.f )
,	mCellSubdivisionProbability( 0.f )
,	mNbRelaxationIterations( 0 )
	// Window function
,	mCellularToGaussianWindowBlend( 0.f )
,	mCellularWindowNorm( 0.5f )
,	mRectangularToVoronoiShapeBlend( 0.f )
,	mCellularWindowDecay( 1.f )
,	mWindowShape( 0 )
,	mWindowArity( 0.f )
,	mWindowLarp( 0.f )
,	mWindowNorm( 0.f )
,	mWindowSmooth( 0.f )
,	mWindowBlend( 0.f )
,	mWindowSigwcell( 0.f )
,	mGaussianWindowDecay( 1.f )
,	mGaussianWindowDecayJittering( 0.f )
	// Feature function
,	mMinNbGaborKernels( 0 )
,	mMaxNbGaborKernels( 0 )
,	mFeatureNorm( 0.5f )
,	mGaborStripesFrequency( 4 )
,	mGaborStripesCurvature( 0.15f )
,	mGaborStripesOrientation( 0.2f )
,	mGaborStripesThickness( 0.15f )
,	mGaborDecay( 1.f )
,	mGaborDecayJittering( 0.f )
,	mFeaturePhaseShift( 0.f )
,	mBombingFlag( 0 )
	// Recursivity
,	mRecursiveWindowSubdivisionProbability( 0.f )
,	mRecursiveWindowSubdivisionScale( 0.5f )
{
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtPPTBF::~PtPPTBF()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void PtPPTBF::initialize()
{
	mWindow = new PtWindow();
	mFeature = new PtFeature();
	mNoise = new PtNoise();
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void PtPPTBF::finalize()
{
	delete mWindow;
	mWindow = nullptr;
	delete mFeature;
	mFeature = nullptr;
	delete mNoise;
	mNoise = nullptr;
}

const int MAX_NOISE_RAND = 1024;

//static unsigned int P[MAX_NOISE_RAND];

unsigned int P[MAX_NOISE_RAND] = {
	673, 514, 228, 3, 157, 394, 315, 202, 123, 20,
	606, 878, 605, 77, 926, 117, 581, 850, 1019, 282,
	665, 939, 604, 814, 7, 1006, 922, 27, 28, 835,
	30, 822, 285, 255, 851, 400, 330, 927, 38, 39,
	119, 240, 176, 391, 295, 142, 191, 67, 379, 49,
	50, 93, 398, 873, 286, 127, 693, 669, 793, 1020,
	559, 278, 140, 410, 430, 411, 46, 47, 736, 13,
	481, 160, 246, 772, 318, 82, 367, 660, 78, 291,
	863, 445, 727, 483, 745, 968, 622, 894, 838, 661,
	90, 91, 335, 700, 94, 994, 549, 97, 945, 161,
	486, 340, 8, 418, 104, 303, 344, 232, 588, 45,
	503, 615, 284, 787, 71, 728, 544, 691, 762, 149,
	634, 452, 996, 203, 505, 178, 58, 638, 220, 260,
	130, 139, 934, 16, 958, 631, 194, 967, 511, 195,
	441, 181, 1015, 970, 999, 907, 911, 458, 815, 252,
	808, 151, 53, 672, 671, 929, 658, 697, 251, 215,
	819, 846, 267, 226, 689, 239, 518, 836, 210, 68,
	771, 171, 943, 336, 235, 685, 893, 270, 349, 839,
	180, 914, 182, 125, 901, 504, 597, 978, 862, 189,
	783, 976, 1009, 844, 66, 859, 806, 109, 530, 871,
	896, 539, 805, 348, 204, 205, 417, 720, 128, 237,
	992, 25, 991, 773, 214, 159, 977, 80, 841, 219,
	73, 292, 465, 813, 320, 225, 163, 227, 993, 229,
	230, 231, 913, 407, 904, 371, 256, 872, 238, 65,
	833, 306, 242, 243, 385, 831, 995, 148, 341, 22,
	713, 334, 192, 253, 965, 100, 201, 257, 294, 321,
	809, 590, 343, 959, 188, 853, 902, 95, 905, 633,
	366, 383, 5, 652, 937, 135, 494, 710, 183, 395,
	280, 325, 480, 308, 670, 92, 974, 287, 612, 431,
	855, 589, 607, 677, 722, 949, 342, 353, 523, 299,
	300, 301, 356, 738, 450, 305, 402, 268, 359, 854,
	576, 196, 116, 313, 908, 548, 316, 538, 377, 168,
	572, 690, 459, 307, 887, 85, 326, 533, 856, 51,
	798, 437, 746, 111, 107, 600, 816, 389, 1022, 382,
	743, 567, 358, 528, 923, 439, 323, 223, 103, 373,
	350, 351, 32, 776, 397, 29, 245, 715, 216, 442,
	766, 361, 521, 19, 912, 224, 427, 752, 368, 234,
	254, 174, 546, 478, 363, 208, 34, 74, 378, 155,
	447, 463, 425, 384, 108, 566, 721, 150, 388, 337,
	825, 37, 989, 915, 261, 279, 675, 885, 666, 596,
	903, 401, 571, 24, 852, 657, 75, 169, 36, 409,
	990, 560, 786, 1018, 324, 415, 502, 346, 63, 707,
	900, 547, 740, 579, 455, 218, 18, 580, 428, 244,
	531, 289, 432, 433, 906, 701, 436, 331, 414, 88,
	297, 200, 131, 443, 115, 594, 703, 217, 129, 678,
	552, 406, 121, 453, 870, 561, 884, 86, 310, 562,
	557, 823, 462, 381, 1, 986, 102, 467, 920, 969,
	477, 298, 52, 626, 474, 355, 471, 770, 587, 42,
	281, 118, 479, 613, 592, 83, 784, 365, 488, 360,
	765, 491, 492, 522, 731, 647, 137, 529, 498, 4,
	500, 617, 11, 540, 273, 464, 328, 507, 508, 490,
	197, 369, 364, 15, 961, 290, 988, 264, 629, 1007,
	987, 495, 81, 272, 524, 824, 166, 527, 193, 653,
	1008, 880, 532, 889, 69, 535, 536, 812, 723, 712,
	955, 422, 542, 718, 637, 954, 510, 421, 794, 236,
	120, 868, 412, 608, 277, 79, 556, 555, 309, 403,
	213, 434, 212, 898, 973, 493, 732, 35, 170, 569,
	570, 725, 57, 709, 769, 938, 472, 619, 578, 761,
	515, 802, 749, 352, 910, 698, 302, 956, 435, 399,
	623, 444, 10, 136, 473, 779, 262, 405, 598, 526,
	475, 897, 602, 739, 879, 370, 283, 271, 944, 89,
	610, 792, 599, 429, 639, 132, 616, 803, 618, 936,
	59, 259, 33, 925, 696, 250, 734, 627, 724, 826,
	630, 628, 152, 985, 877, 635, 222, 101, 892, 960,
	591, 931, 187, 456, 834, 916, 646, 54, 789, 179,
	680, 651, 1004, 857, 460, 64, 206, 748, 156, 499,
	184, 0, 952, 778, 664, 869, 269, 759, 979, 843,
	112, 23, 941, 942, 496, 258, 624, 714, 656, 611,
	338, 962, 1001, 683, 684, 935, 643, 681, 275, 413,
	509, 207, 692, 755, 1011, 860, 818, 122, 70, 828,
	662, 21, 84, 470, 963, 705, 550, 380, 387, 154,
	764, 96, 339, 575, 733, 449, 317, 717, 788, 497,
	134, 512, 781, 767, 393, 867, 241, 1021, 553, 519,
	797, 593, 642, 757, 729, 735, 534, 737, 583, 791,
	72, 668, 742, 14, 1013, 890, 585, 141, 565, 830,
	795, 751, 296, 753, 754, 485, 849, 26, 758, 980,
	332, 423, 747, 625, 327, 345, 645, 114, 87, 347,
	609, 420, 774, 319, 322, 655, 162, 777, 883, 595,
	780, 957, 782, 304, 31, 785, 173, 167, 649, 760,
	153, 1012, 614, 948, 886, 601, 796, 730, 997, 799,
	659, 333, 489, 874, 804, 667, 875, 807, 790, 933,
	810, 848, 506, 60, 487, 586, 372, 817, 636, 221,
	98, 821, 554, 708, 876, 964, 124, 165, 469, 829,
	704, 248, 641, 741, 543, 288, 858, 953, 147, 454,
	840, 12, 76, 190, 461, 845, 837, 847, 44, 891,
	687, 158, 516, 265, 199, 482, 1000, 971, 266, 654,
	438, 861, 177, 525, 864, 865, 468, 702, 551, 56,
	568, 408, 390, 811, 476, 679, 881, 558, 632, 249,
	314, 41, 882, 621, 577, 396, 711, 55, 888, 138,
	145, 133, 311, 448, 603, 895, 983, 110, 563, 744,
	172, 650, 564, 209, 775, 113, 573, 376, 211, 185,
	688, 146, 582, 517, 584, 424, 620, 917, 918, 919,
	866, 921, 676, 827, 924, 43, 975, 966, 928, 466,
	930, 233, 932, 644, 446, 99, 106, 501, 768, 440,
	998, 198, 820, 175, 541, 695, 946, 329, 186, 719,
	950, 951, 357, 17, 545, 263, 40, 726, 386, 947,
	756, 404, 537, 374, 62, 640, 247, 457, 686, 520,
	143, 842, 126, 763, 2, 451, 513, 663, 682, 706,
	972, 981, 982, 9, 984, 426, 694, 144, 674, 419,
	362, 105, 416, 832, 48, 909, 274, 1002, 574, 293,
	164, 484, 699, 1003, 312, 354, 276, 801, 648, 940,
	1010, 800, 1005, 899, 1014, 61, 1016, 1017, 392, 716,
	6, 750, 375, 1023
};

static int phi( int x )
{
	x = x % MAX_NOISE_RAND;
	if ( x < 0 ) x = x + MAX_NOISE_RAND;
	return ( P[ x ] );
}

static unsigned int seed;

static void seeding(unsigned int x, unsigned int y, unsigned int z)
//{ seed=x%unsigned int(1024)+(y%unsigned int(1024))*unsigned int(1024)+(z%unsigned int(1024))*unsigned int(1024*1024); }
{
	seed = phi(x + phi(y + phi(z))) % (unsigned int)(1 << 15) + (phi(3 * x + phi(4 * y + phi(z))) % (unsigned int)(1 << 15))*(unsigned int)(1 << 15);
}

/******************************************************************************
 * Random value between -1.0 and 1.0
 ******************************************************************************/
static float next()
{
	seed *= (unsigned int)(3039177861);
	float res = (float)( ( double(seed) / 4294967296.0 ) * 2.0 - 1.0 );
	return res;
}

/******************************************************************************
 * pave() for regular and irregular tilings (REGULAR, IRREGULAR, IRREGULARX, IRREGULARY)
 *
 * @param xp x position
 * @param yp y position
 * @param Nx used to create a shift on x axis
 * @param randx
 * @param randy
 * @param cx[9] [OUT] 3x3 kernel of point position on x axis
 * @param cy[9] [OUT] 3x3 kernel of point position on y axis
 * @param dx[9] [OUT] 3x3 kernel of point distance to border on x axis
 * @param dy[9] [OUT] 3x3 kernel of point distance to border on y axis
 ******************************************************************************/
static void pave(
	const float xp, const float yp,
	// pavement parameters
	const int Nx,
	const float randx, const float randy,
	float cx[ 9 ], float cy[ 9 ], float dx[ 9 ], float dy[ 9 ] )
{
	const float x = xp;
	const float y = yp;

	// Variables used to pave the plan (floor() and fract())
	const int ix = (int)floor( x );
	const float xx = x - (float)ix;
	const int iy = (int)floor( y );
	const float yy = y - (float)iy;
	
	// Iterate through 3x3 neighborhood (cells)
	int i, j;
	int nc = 0;
	for ( j = -1; j <= +1; j++ )
	{
		for ( i = -1; i <= +1; i++ )
		{
			float rxi, rxs, ryi, rys;

			// Neighbor cell
			float ivpx = (float)ix + (float)i;
			float ivpy = (float)iy + (float)j;
			
			// [0;1]
			float decalx = (float)( (int)ivpy % Nx ) / (float)Nx;
			
			// Random point in current cell
			// [-0.5;0.5]
			seeding( (unsigned int)( ivpx + 5 ), (unsigned int)( ivpy + 10 ), 0 );
			rxi = next() * randx * 0.5f; //printf("rx %d,%d=%g, ", (unsigned int)(ivpx + 5), (unsigned int)(ivpy + 10), rx);
			// [-0.5;0.5]
			seeding( 3, (unsigned int)( ivpy + 10 ), 0 );
			ryi = next() * randy * 0.5f;
			
			// Random point in current cell
			// [-0.5;0.5]
			seeding( (unsigned int)( ivpx + 1 + 5 ), (unsigned int)( ivpy + 10 ), 0 );
			rxs = next() * randx * 0.5f; //printf("rxs %d,%d=%g\n", (unsigned int)(ivpx +1 + 5), (unsigned int)(ivpy + 10), rxs);
			// [-0.5;0.5]
			seeding( 3, (unsigned int)( ivpy + 1 + 10 ), 0 );
			rys = next() * randy * 0.5f;

			// point distance to current rectangular cell border
			dx[ nc ] = ( 0.5f * ( rxs + 1.0f - rxi ) ); // lies in [0;1]
			dy[ nc ] = 0.5f * ( rys + 1.0f - ryi ); // lies in [0;1]
			// point coordinates
			cx[ nc ] = ( ivpx + decalx + rxi + dx[ nc ] );
			cy[ nc ] = ivpy + ryi + dy[ nc ];
			
			// Update counter
			nc++;
		}
	}
}

/******************************************************************************
 * pave() for regular and irregular tilings (REGULAR, IRREGULAR, IRREGULARX, IRREGULARY)
 *
 * @param xp x position
 * @param yp y position
 * @param Nx used to create a shift on x axis
 * @param randx
 * @param randy
 * @param cx[9] [OUT] 3x3 kernel of point position on x axis
 * @param cy[9] [OUT] 3x3 kernel of point position on y axis
 * @param dx[9] [OUT] 3x3 kernel of point distance to border on x axis
 * @param dy[9] [OUT] 3x3 kernel of point distance to border on y axis
 ******************************************************************************/
// BISQUARE
static void paveb(float x, float y,
	// pavement parameters
	float cx[9], float cy[9], float dx[9], float dy[9])
{
	int i, j;
	int nc = 0;
	int ii, jj;
	int ix = (int)floor(x); float xx = x - (float)ix;
	int iy = (int)floor(y); float yy = y - (float)iy;
	int qx = (int)(xx*(int)5);
	int qy = (int)(yy*(int)5);
	for (i = 0; i < 3; i++)  for (j = 0; j<3; j++)
	{
		if (qx >= -2 + i * 2 + j && qx <= -2 + i * 2 + 1 + j
			&& qy >= 1 - i + 2 * j && qy <= 1 - i + 2 * j + 1)
		{
			for (ii = 0; ii <= 2; ii++) for (jj = 0; jj <= 2; jj++)
			{
				if (i == 1 || j == 1)
				{
					int rx = -2 + i * 2 + j - 3 + ii * 2 + jj;
					int ry = 1 - i + 2 * j - 1 + jj * 2 - ii;
					dx[nc] = 1.0f / 5.0f; dy[nc] = 1.0f / 5.0f;
					cx[nc] = (float)ix + (float)rx / 5.0f + 1.0f / 5.0f;
					cy[nc] = (float)iy + (float)ry / 5.0f + 1.0f / 5.0f;
					nc++;
				}
			}

			int rx = -2 + i * 2 + j;
			int ry = 1 - i + 2 * j;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(rx - 1) / 5.0f + 0.5f / 5.0f;
			cy[nc] = (float)iy + (float)ry / 5.0f + 0.5f / 5.0f;
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)rx / 5.0f + 0.5f / 5.0f;
			cy[nc] = (float)iy + (float)(ry + 2) / 5.0f + 0.5f / 5.0f;
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(rx + 2) / 5.0f + 0.5f / 5.0f;
			cy[nc] = (float)iy + (float)(ry + 1) / 5.0f + 0.5f / 5.0f;
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(rx + 1) / 5.0f + 0.5f / 5.0f;
			cy[nc] = (float)iy + (float)(ry - 1) / 5.0f + 0.5f / 5.0f;
			nc++;
			return;
		}
	}

	for (i = 0; i < 3; i++)  for (j = 0; j < 2; j++)
	{
		if (qx == i * 2 + j && qy == 2 + 2 * j - i)
		{
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)qx / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)qy / 5.0f + dy[nc];
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(qx - 2) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy + 1) / 5.0f + dy[nc];
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(qx + 1) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy + 2) / 5.0f + dy[nc];
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(qx - 1) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 2) / 5.0f + dy[nc];
			nc++;
			dx[nc] = 0.5f / 5.0f; dy[nc] = 0.5f / 5.0f;
			cx[nc] = (float)ix + (float)(qx + 2) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 1) / 5.0f + dy[nc];
			nc++;

			dx[nc] = 1.0f / 5.0f; dy[nc] = 1.0f / 5.0f;
			cx[nc] = (float)ix + (float)(qx - 2) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 1) / 5.0f + dy[nc];
			nc++;
			dx[nc] = 1.0f / 5.0f; dy[nc] = 1.0f / 5.0f;
			cx[nc] = (float)ix + (float)(qx - 1) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy + 1) / 5.0f + dy[nc];
			nc++;
			dx[nc] = 1.0f / 5.0f; dy[nc] = 1.0f / 5.0f;
			cx[nc] = (float)ix + (float)(qx + 1) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy) / 5.0f + dy[nc];
			nc++;
			dx[nc] = 1.0f / 5.0f; dy[nc] = 1.0f / 5.0f;
			cx[nc] = (float)ix + (float)(qx) / 5.0f + dx[nc];
			cy[nc] = (float)iy + (float)(qy - 2) / 5.0f + dy[nc];
			nc++;
			return;
		}
	}

	printf("error!!!\n");

	return;
}

/******************************************************************************
 * pave() for regular and irregular tilings (REGULAR, IRREGULAR, IRREGULARX, IRREGULARY)
 *
 * @param xp x position
 * @param yp y position
 * @param Nx used to create a shift on x axis
 * @param randx
 * @param randy
 * @param cx[9] [OUT] 3x3 kernel of point position on x axis
 * @param cy[9] [OUT] 3x3 kernel of point position on y axis
 * @param dx[9] [OUT] 3x3 kernel of point distance to border on x axis
 * @param dy[9] [OUT] 3x3 kernel of point distance to border on y axis
 ******************************************************************************/
// CROSS
static void paved(float x, float y,
	// pavement parameters
	int Nx,
	float cx[9], float cy[9], float dx[9], float dy[9])
{
	int i, j;
	int ix = (int)floor(x); float xx = x - (float)ix;
	int iy = (int)floor(y); float yy = y - (float)iy;
	int qx = (int)(xx*(int)(2 * Nx));
	int qy = (int)(yy*(int)(2 * Nx));

	// horizontal
	if ((qx >= qy && qx <= qy + Nx - 1) || (qx >= qy - 2 * Nx && qx <= qy + Nx - 1 - 2 * Nx))
	{
		int rx, ry;
		if (qx >= qy && qx <= qy + Nx - 1) { rx = qy; ry = qy; }
		else { rx = qy - 2 * Nx; ry = qy; }
		for (i = 0; i < 3; i++)
		{
			cx[3 * i] = (float)ix + ((float)rx + (float)(i - 1) + (float)(Nx) *0.5f) / (float)(2 * Nx);
			cy[3 * i] = (float)iy + ((float)ry + (float)(i - 1) + 0.5f) / (float)(2 * Nx);
			dx[3 * i] = ((float)Nx*0.5f) / (float)(2 * Nx);
			dy[3 * i] = 0.5f / (float)(2 * Nx);

			cx[3 * i + 1] = (float)ix + ((float)rx + (float)(i - 2) + 0.5f) / (float)(2 * Nx);
			cy[3 * i + 1] = (float)iy + ((float)ry + (float)(i - 1) + (float)(Nx) *0.5f) / (float)(2 * Nx);
			dx[3 * i + 1] = 0.5f / (float)(2 * Nx);
			dy[3 * i + 1] = ((float)Nx*0.5f) / (float)(2 * Nx);

			cx[3 * i + 2] = (float)ix + ((float)rx + (float)(i - 1) + (float)Nx + 0.5f) / (float)(2 * Nx);
			cy[3 * i + 2] = (float)iy + ((float)ry + (float)(i)-(float)(Nx)*0.5f) / (float)(2 * Nx);
			dx[3 * i + 2] = 0.5f / (float)(2 * Nx);
			dy[3 * i + 2] = ((float)Nx*0.5f) / (float)(2 * Nx);
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
			cx[3 * i] = (float)ix + ((float)rx + (float)(i - 1) + 0.5f) / (float)(2 * Nx);
			cy[3 * i] = (float)iy + ((float)ry + (float)(i - 1) + (float)(Nx) *0.5f) / (float)(2 * Nx);
			dx[3 * i] = 0.5f / (float)(2 * Nx);
			dy[3 * i] = ((float)Nx*0.5f) / (float)(2 * Nx);

			cx[3 * i + 1] = (float)ix + ((float)rx + (float)(i - 1) + (float)(Nx)*0.5f) / (float)(2 * Nx);
			cy[3 * i + 1] = (float)iy + ((float)ry + (float)(i - 2) + 0.5f) / (float)(2 * Nx);
			dx[3 * i + 1] = ((float)Nx*0.5f) / (float)(2 * Nx);
			dy[3 * i + 1] = 0.5f / (float)(2 * Nx);

			cx[3 * i + 2] = (float)ix + ((float)rx + (float)(i - 1) - (float)(Nx)*0.5f) / (float)(2 * Nx);
			cy[3 * i + 2] = (float)iy + ((float)ry + (float)(i - 1) + (float)(Nx - 1) + 0.5f) / (float)(2 * Nx);
			dx[3 * i + 2] = ((float)Nx*0.5f) / (float)(2 * Nx);
			dy[3 * i + 2] = 0.5f / (float)(2 * Nx);
		}
	}
}

/******************************************************************************
 * pavement()
 *
 * @param x x position
 * @param y y position
 * @param tt tiling type
 * @param decalx
 * @param Nx
 * @param ccx [OUT]
 * @param ccy [OUT]
 * @param cdx [OUT]
 * @param cdy [OUT]
 ******************************************************************************/
static void pavement(
	const float x, const float y,
	const PtPPTBF::tilingtype tt, const int decalx, const int Nx,
	float ccx[ 9 ], float ccy[ 9 ], float cdx[ 9 ], float cdy[ 9 ] )
{
	switch ( tt )
	{
		case PtPPTBF::REGULAR: pave( x, y, decalx, 0.0f, 0.0f, ccx, ccy, cdx, cdy );
			break;
		
		case PtPPTBF::IRREGULAR: pave(x, y, decalx, 0.8f, 0.8f, ccx, ccy, cdx, cdy);
			break;
		
		case PtPPTBF::CROSS: paved(x, y, Nx, ccx, ccy, cdx, cdy);
			break;
		
		case PtPPTBF::BISQUARE: paveb(x, y, ccx, ccy, cdx, cdy);
			break;
		
		case PtPPTBF::IRREGULARX: pave(x, y, decalx, 0.8f, 0.0f, ccx, ccy, cdx, cdy);
			break;
		
		case PtPPTBF::IRREGULARY: pave(x, y, decalx, 0.0f, 0.8f, ccx, ccy, cdx, cdy);
			break;
		
		default: pave(x, y, decalx, 0.0f, 0.0f, ccx, ccy, cdx, cdy);
			break;
	}
}

/******************************************************************************
 * pointset()
 *
 * Point process generation, approximated by non-uniform and irregular grids made of rectangular cells Ri.
 *
 * Generated points xi are randomly generated inside cell Ri using jittering amplitude.
 * The Ri are recursively subdivided, according to a given probability thus generating
 * clusters of poits instead of uniform distributions.
 *
 * Points are generated at current cell plus its 1-neighborhood cells (for next nth-closest point search).
 *
 * @param psubx subdivision probality of a cell Ri on x axis
 * @param psuby subdivision probality of a cell Ri on y axis
 * @param jitx jittering amplitude on x axis
 * @param jity jittering amplitude on y axis
 * @param ccx root/parent cell center on x axis
 * @param ccy root/parent cell center on y axis
 * @param cdx root/parent cell's closest distance to border on x axis
 * @param cdy root/parent cell's closest distance to border on y axis
 * @param cx [out] list of generated points coordinates on x axis
 * @param cy [out] list of generated points coordinates on y axis
 * @param pFeatureCellCenterX [out] list of associated cells centers on x axis
 * @param pFeatureCellCenterY [out] list of associated cells centers on y axis
 * @param ndx [out] list of associated cells closest distance to border on x axis
 * @param ndy [out] list of associated cells closest distance to border on y axis
 ******************************************************************************/
static int pointset(
	// point set parameters
	const float psubx, const float psuby, const float jitx, const float jity,
	float ccx[9], float ccy[9], float cdx[9], float cdy[9],
	float cx[100], float cy[100], float pFeatureCellCenterX[100], float pFeatureCellCenterY[100], float ndx[100], float ndy[100] )
{
	// Loop counter
	int i, j, k;

	// Number of randomly generated points (counter)
	int nc = 0;

	// Iterate through 3x3 neighborhood cells
	for ( k = 0; k < 9; k++ )
	{
		// Variables used to pave the plan (floor() and fract())
		int ix = (int)floor(ccx[k]); float xx = ccx[k] - (float)ix;
		int iy = (int)floor(ccy[k]); float yy = ccy[k] - (float)iy;
		
		// To decide whether a cell Ri is subdivided or not, we generate a random number in its center and compare it to user "p_sub"
		// - generate probability [0;1] based on cell center (ccx;ccy)
		seeding( (unsigned int)((int)floor( ccx[ k ] * 10.0f ) + 10 ), (unsigned int)( (int)floor( ccy[ k ] * 10.0f ) + 3 ), 0 );
		float subx = next() * 0.5f + 0.5f;
		float suby = next() * 0.5f + 0.5f;
		if ( subx < psubx && suby < psuby ) // Subdivide cell on x axis and y axis
		{
			// Cell subdivision on x and y axis
			// - generate 4 points (1 in each cell)

			// Random split around cell's center
			float cutx = 0.5f/*cell center*/ + 0.3f * next() * jitx/*random cut*/;
			float cuty = 0.5f/*cell center*/ + 0.3f * next() * jity/*random cut*/;

			float ncdx, ncdy, nccx, nccy, rx, ry;

			// new cell size (distance from center to border)
			ncdx = ( cutx * 2.0f * cdx[k] ) * 0.5f;
			ncdy = ( cuty * 2.0f * cdy[k] ) * 0.5f;
			// new cell center
			nccx = ccx[ k ] - cdx[ k ] + ncdx;
			nccy = ccy[ k ] - cdy[ k ] + ncdy;
			// Generate random point position in cell
			// - random amplitude around cell center
			rx = ncdx * next() * jitx;
			ry = ncdy * next() * jity;
			// - random point position
			cx[ nc ] = nccx + rx;
			cy[ nc ] = nccy + ry;
			// Store cell info
			// - cell center
			pFeatureCellCenterX[ nc ] = nccx;
			pFeatureCellCenterY[ nc ] = nccy;
			// - closest distance to border
			ndx[ nc ] = ncdx;
			ndy[ nc ] = ncdy;
			// Update point counter
			nc++;

			ncdx = ((1.0f - cutx)*2.0f*cdx[k])*0.5f;
			ncdy = (cuty*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + (cutx*2.0f*cdx[k]) + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;

			ncdx = (cutx*2.0f*cdx[k])*0.5f;
			ncdy = ((1.0f - cuty)*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + (cuty*2.0f*cdy[k]) + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;

			ncdx = ((1.0f - cutx)*2.0f*cdx[k])*0.5f;
			ncdy = ((1.0f - cuty)*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + (cutx*2.0f*cdx[k]) + ncdx;
			nccy = ccy[k] - cdy[k] + (cuty*2.0f*cdy[k]) + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;
		}
		else if ( subx < psubx ) // Subdivide cell only on x axis
		{
			// Cell subdivision on x axis
			// - generate 2 points (1 in each cell)

			float cutx = 0.3f + 0.4f*(next()*0.5f + 0.5f);
			float cuty = 1.0f;
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = (cutx*2.0f*cdx[k])*0.5f;
			ncdy = (cuty*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;

			ncdx = ((1.0f - cutx)*2.0f*cdx[k])*0.5f;
			ncdy = (cuty*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + (cutx*2.0f*cdx[k]) + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;
		}
		else if ( suby < psuby ) // Subdivide cell only on y axis
		{
			// Cell subdivision on y axis
			// - generate 2 points (1 in each cell)

			float cutx = 1.0f;
			float cuty = 0.3f + 0.4f*(next()*0.5f + 0.5f);
			float ncdx, ncdy, nccx, nccy, rx, ry;

			ncdx = (cutx*2.0f*cdx[k])*0.5f;
			ncdy = (cuty*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;

			ncdx = (cutx*2.0f*cdx[k])*0.5f;
			ncdy = ((1.0f - cuty)*2.0f*cdy[k])*0.5f;
			nccx = ccx[k] - cdx[k] + ncdx;
			nccy = ccy[k] - cdy[k] + (cuty*2.0f*cdy[k]) + ncdy;
			rx = ncdx * next()*jitx;
			ry = ncdy * next()*jity;
			cx[nc] = nccx + rx;
			cy[nc] = nccy + ry;
			pFeatureCellCenterX[nc] = nccx; pFeatureCellCenterY[nc] = nccy; ndx[nc] = ncdx; ndy[nc] = ncdy;
			// Update point counter
			nc++;
		}
		else
		{
			// No cell subdivision
			// - generate 1 point in cell
			float rx = cdx[ k ] * next() * jitx;
			float ry = cdy[ k ] * next() * jity;
			cx[ nc ] = ccx[ k ] + rx;
			cy[ nc ] = ccy[ k ] + ry;
			pFeatureCellCenterX[nc] = ccx[ k ]; pFeatureCellCenterY[ nc ] = ccy[ k ]; ndx[ nc ] = cdx[ k ]; ndy[ nc ] = cdy[ k ];
			// Update point counter
			nc++;
		}
	}

	// Return number of randomly generated points
	return nc;
}

/******************************************************************************
 * relax()
 *
 * ...
******************************************************************************/
static void relax(int pNbRelaxationIterations, int n, float bx, float by, float dx, float dy, 
	float cx[100], float cy[100], float bcx[100], float bcy[100], float dcx[100], float dcy[100])
{
	int i, j, k;
	float mcx[100], mcy[100];
	for (i = 0; i < pNbRelaxationIterations; i++)
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
				float no1 = static_cast< float >( sqrt( (double)(dx1*dx1 + dy1*dy1) ) ); if (no1 == 0.0f) no1 = 1.0f;
				float dx2 = cx[ind2] - cx[k];
				float dy2 = cy[ind2] - cy[k];
				float no2 = static_cast< float >( sqrt( (double)(dx2*dx2 + dy2*dy2) ) ); if (no2 == 0.0) no2 = 1.0f;
				float dx3 = cx[ind3] - cx[k];
				float dy3 = cy[ind3] - cy[k];
				float no3 = static_cast< float >( sqrt( (double)(dx3*dx3 + dy3*dy3) ) ); if (no3 == 0.0) no3 = 1.0f;
				float dirx = dx1 / no1 / no1 + dx2 / no2 / no2 + dx3 / no3 / no3;
				float diry = dy1 / no1 / no1 + dy2 / no2 / no2 + dy3 / no3 / no3;
				float no = sqrt(dirx*dirx + diry*diry); if (no == 0.0) no = 1.0;
				mcx[k] = cx[k] - (dirx / no * 0.05f);
				mcy[k] = cy[k] - (diry / no * 0.05f);
				if (mcx[k] < bcx[k] - dcx[k] + 0.05f) mcx[k] = bcx[k] - dcx[k] + 0.05f;
				if (mcx[k] > bcx[k] + dcx[k] - 0.05f) mcx[k] = bcx[k] + dcx[k] - 0.05f;
				if (mcy[k] < bcy[k] - dcy[k] + 0.05f) mcy[k] = bcy[k] - dcy[k] + 0.05f;
				if (mcy[k] > by + dy) mcy[k] = bcy[k] + dcy[k] - 0.05f;
			}
			else { mcx[k] = cx[k]; mcy[k] = cy[k]; }
		for (k = 0; k < n; k++) { cx[k] = mcx[k]; cy[k] = mcy[k]; }
	}
}

/******************************************************************************
 * distributeElements()
 *
 * Distribution of elements: spatial point process.
 *
 * @param pFeaturePointX current x position
 * @param pFeaturePointY current y position
 * @param tt tiling type
 * @param psubx subdivision probability of a rectangular cell along x axis [0;1]
 * @param psuby subdivision probability of a rectangular cell along y axis [0;1]
 * @param decalx
 * @param Nx
 * @param pNbRelaxationIterations number of iteration of Llyod relaxation algorithm (for repulsive force)
 * @param pJittering jittering: amplitude of distance in [0;1] to generate position in cells around their center
 * @param cx [out] list of generated points coordinates on x axis
 * @param cy [out] list of generated points coordinates on y axis
 * @param pFeatureCellCenterX [out] list of associated cells centers on x axis
 * @param pFeatureCellCenterY [out] list of associated cells centers on y axis
 * @param ndx [out] list of associated cells closest distance to border on x axis
 * @param ndy [out] list of associated cells closest distance to border on y axis
 ******************************************************************************/
static int distributeElements(
	const float pFeaturePointX, const float pFeaturePointY,
	// point set parameters
	const PtPPTBF::tilingtype tt,
	const float psubx, const float psuby,
	const int decalx, const int Nx,
	const int pNbRelaxationIterations,
	const float pJittering,
	float cx[ 100 ], float cy[ 100 ],
	float pFeatureCellCenterX[ 100 ], float pFeatureCellCenterY[ 100 ],
	float ndx[ 100 ], float ndy[ 100 ] )
{
	// Loop counters
	int i, k;

	float ccx[ 9 ]; float ccy[ 9 ];
	float cdx[ 9 ]; float cdy[ 9 ];
	
	// Retrieve plan pavement of 1-neighborhood (i.e. tiling of rectangular cells Ri)
	// cc: cells centers
	// cd: associated closest distances to cell borders
	pavement(
		pFeaturePointX, pFeaturePointY,
		tt,
		decalx,
		Nx,
		ccx, ccy, // OUT: cells centers of 1-neighborhood cells 
		cdx, cdy ); // OUT: closest distances to borders of 1-neighborhood cells

	////----------------------------------
	//for ( int i = 0; i < 9; i++ )
	//{
	//	printf( "PAVEMENT: (%f;%f) (%f;%f)\n", ccx[i], ccy[i], cdx[i], cdy[i] );
	//}
	////----------------------------------

	int np = 0;

	// Check whether or not to apply relaxation (i.e. add repulsive force)
	if ( pNbRelaxationIterations == 0 )
	{
		// Point process generation, approximated by non-uniform and irregular grids made of rectangular cells Ri.
		// Generated points xi are randomly generated inside cell Ri using jittering amplitude.
		// The Ri are recursively subdivided, according to a given probability thus generating
		// clusters of poits instead of uniform distributions.
		// Points are generated at current cell plus its 1-neighborhood cells(for next nth - closest point search).
		np = pointset(
			psubx, psuby,
			0.8f, 0.8f, // jittering
			ccx, ccy, // cells centers of 1-neighborhood cells 
			cdx, cdy, // closest distances to borders of 1-neighborhood cells
			cx, cy, // OUT
			pFeatureCellCenterX, pFeatureCellCenterY, // OUT
			ndx, ndy ); // OUT
	}
	else
	{
		for ( k = 0; k < 9; k++ )
		{
			float gccx[ 9 ]; float gccy[ 9 ];
			float gcdx[ 9 ]; float gcdy[ 9 ];
			
			float gcx[ 100 ]; float gcy[ 100 ];
			float gncx[ 100 ]; float gncy[ 100 ];
			float gndx[ 100 ]; float gndy[ 100 ];

			pavement( ccx[k], ccy[k], tt, decalx, Nx, gccx, gccy, gcdx, gcdy );
			
			int npk = pointset( psubx, psuby, 0.8f, 0.8f, gccx, gccy, gcdx, gcdy, gcx, gcy, gncx, gncy, gndx, gndy );
			
			relax( pNbRelaxationIterations, npk, ccx[k], ccy[k], cdx[k], cdy[k], gcx, gcy, gncx, gncy, gndx, gndy );
			
			for ( i = 0; i < npk; i++ )
			{
				if ( gcx[i] >= ccx[k] - cdx[k] && gcx[i] <= ccx[k] + cdx[k] &&
					gcy[i] >= ccy[k] - cdy[k] && gcy[i] <= ccy[k] + cdy[k] )
				{
					cx[np] = gcx[i]; cy[np] = gcy[i];
					pFeatureCellCenterX[np] = gncx[i]; pFeatureCellCenterY[np] = gncy[i];
					ndx[np] = gndx[i]; ndy[np] = gndy[i];

					np++;
				}
			}
		}
	}

	// Blend randomly generated point positions with their associated cell centers
	// - with jittering: we use pure random points
	// - without jittering: we use cells centers 
	// Iterate through poinset
	for ( i = 0; i < np; i++ )
	{
		// Blend between point and its center cell
		cx[ i ] = cx[ i ] * pJittering + pFeatureCellCenterX[ i ] * ( 1.0f - pJittering );
		cy[ i ] = cy[ i ] * pJittering + pFeatureCellCenterY[ i ] * ( 1.0f - pJittering );
	}

	// return nb points in poinset (between 9 and 9*4)
	return np;
}

/******************************************************************************
 * cdistance()
 *
 * Computer closest distance to cell border the point lies in.
 * Smooth blend between rectangular cell shapes and p-norm Voronoï cell shapes.
 *
 * @param x1 current point position on x axis
 * @param y1 current point position on y axis
 * @param x2 feature point position on x axis
 * @param y2 feature point position on y axis
 * @param pCellularWindowNorm p-norm used for Voronoi cell shapes
 * @param cx current cell's center position on x axis
 * @param cy current cell's center position on y axis
 * @param dx current cell's closest distance to border on x axis
 * @param dy current cell's closest distance to border on y axis
 * @param pRectangularToVoronoiShapeBlend cellular anisotropic distance blend between rectangular cell shapes and p-norm Voronoï cell shapes
 ******************************************************************************/
static float cdistance(
	const float x1, const float y1, const float x2, const float y2, const float pCellularWindowNorm, 
	const float cx, const float cy, const float dx, const float dy, const float pRectangularToVoronoiShapeBlend )
{
	//seeding((unsigned int)(cx*12.0 + 5.0), (unsigned int)(cy*12.0 + 11.0), 0);
	//float ss = size*next();

	// Distance between current point (x1,y1) and feature point (x2,y2)
	const float ddx = ( x1 - x2 );
	const float ddy = ( y1 - y2 );

	// normalized rho-norm (rectangular cell shapes)
	const float ex = ddx < 0.0 ? -ddx / ( x2 - cx + dx ) : ddx / ( cx + dx - x2 );
	const float ey = ddy < 0.0 ? -ddy / ( y2 - cy + dy ) : ddy / ( cy + dy - y2 );
	
	//printf("cdist: ddx=%g,ddy=%g, ex=%g, ey=%g, dx=%g,dy=%g\n", ddx, ddy, ex, ey, dx, dy);
	
	/*return ( pRectangularToVoronoiShapeBlend * (float)std::pow( std::pow( abs( ddx ), pCellularWindowNorm ) + std::pow( abs( ddy ), pCellularWindowNorm ), 1.0 / pCellularWindowNorm )
		+ ( 1.0 - pRectangularToVoronoiShapeBlend ) * ( ex > ey ? ex : ey ) );*/

	// p-norm (Voronoi cell shapes)
	const float distanceToVoronoiCell = static_cast< float >( std::pow( std::pow( abs( ddx ), pCellularWindowNorm ) + std::pow( abs( ddy ), pCellularWindowNorm ), 1.0 / pCellularWindowNorm ) );
	// normalized rho-norm (rectangular cell shapes)
	const float distanceToRectangularCell = ( ex > ey ? ex : ey );

	// Smooth blend between rectangular cell shapes and p-norm Voronoï cell shapes
	return ( pRectangularToVoronoiShapeBlend * distanceToVoronoiCell + ( 1.f - pRectangularToVoronoiShapeBlend ) * distanceToRectangularCell );
}

/******************************************************************************
 * cclosest()
 *
 * ...
 *
 * @param pRectangularToVoronoiShapeBlend cellular anisotropic distance blend between rectangular cell shapes and p-norm Voronoï cell shapes
 ******************************************************************************/
static int cclosest(float xx, float yy, float cx[], float cy[], int nc, 
	float pCellularWindowNorm, float cnx[], float cny[], float dx[], float dy[], float pRectangularToVoronoiShapeBlend)
{
	int closestFeatureID = 0;
	float mind = 0.0;
	int k;
	for (k = 0; k < nc; k++)
	{
		float dd = cdistance(xx, yy, cx[k], cy[k], pCellularWindowNorm, cnx[k], cny[k], dx[k], dy[k], pRectangularToVoronoiShapeBlend);
		//float dx = xx - cx[k];
		//float dy = yy - cy[k];
		//float dd = (float)std::pow(std::pow(abs(dx), pCellularWindowNorm) + std::pow(abs(dy), pCellularWindowNorm), 1.0 / pCellularWindowNorm);
		if (k == 0) { mind = dd; }
		else if (mind > dd) { closestFeatureID = k; mind = dd; }
	}
	return closestFeatureID;
}

/******************************************************************************
 * celldist()
 *
 * ...
 *
 * @param pRectangularToVoronoiShapeBlend cellular anisotropic distance blend between rectangular cell shapes and p-norm Voronoï cell shapes
 ******************************************************************************/
static float celldist(float ixx, float iyy, int k, int closestFeatureID, float cx[], float cy[], int nc,
	float pCellularWindowNorm, float cnx[], float cny[], float dx[], float dy[], float pRectangularToVoronoiShapeBlend)
{
	float delta = 0.2f;
	int count, nk;
	float xx, yy, ddx, ddy, dd;
	do {
		xx = ixx; yy = iyy;
		ddx = cx[k] - xx; ddy = cy[k] - yy;
		dd = (float)sqrt(ddx*ddx + ddy*ddy);
		if (dd < 0.001f) return 0.0f;
		ddx *= delta / dd; ddy *= delta / dd;
		if (k == closestFeatureID) { ddx = -ddx; ddy = -ddy; }
		//printf("start  with cell %d, %d, %g,%g, %g,%g,  %g,%g ->%g,%g\n", k, closestFeatureID, ddx, ddy, ixx, iyy, cx[k], cy[k], xx, yy);
		//printf("cell is: %g,%g, (%g,%g)\n", cnx[k], cny[k], dx[k], dy[k]);
		//printf("mincell is: %g,%g, (%g,%g)\n", cnx[closestFeatureID], cny[closestFeatureID], dx[closestFeatureID], dy[closestFeatureID]);
		count = 0;
		//nk = cclosest(xx+ddx, yy+ddy, cx, cy, nc, pCellularWindowNorm, cnx, cny, dx, dy, pRectangularToVoronoiShapeBlend);
		//if (!((k == closestFeatureID && nk == k) || (k != closestFeatureID&&nk != k))) {
		//	printf("start problem with cell, %d, %d, %g,%g, %g,%g,  %g,%g ->%g,%g\n", k, closestFeatureID, dx, dy, ixx, iyy, cx[k], cy[k], xx, yy); 
		//}
		do {
			xx += ddx; yy += ddy;
			nk = cclosest(xx, yy, cx, cy, nc, pCellularWindowNorm, cnx, cny, dx, dy, pRectangularToVoronoiShapeBlend);
			//if (count>97) printf("%d, nk=%d, xx=%g,yy=%g, delta=%g\n", count, nk, xx, yy, delta);
			count++;
		} while (((k == closestFeatureID && nk == k) || (k != closestFeatureID&&nk != k)) && count < 100);
		if (count == 100 && delta <= 0.009) {
			printf("problem with cell, %d, %d, displ:%g,%g, \nfrom: %g,%g,  cell:%g,%g ->at: %g,%g\n", k, closestFeatureID, ddx, ddy, ixx, iyy, cx[k], cy[k], xx, yy);
			printf("in cell: %g,%g, (%g,%g)\n", cnx[k], cny[k], dx[k], dy[k]);
			for (int u = 0; u < nc; u++) if (u != k)
				printf("neighbor cell %d: %g,%g, (%g,%g)\n", u, cnx[u], cny[u], dx[u], dy[u]);
			//hvFatal("error");
		}
		delta /= 2.0f;
	} while (count == 100 && delta >= 0.01f);
	float xa = xx - ddx, ya = yy - ddy;
	float midx = (xa + xx) / 2.0f, midy = (ya + yy) / 2.0f;
	//printf("refine ddx=%g, ddy=%g, midx=%g,midy=%g, cx=%g,cy=%g,cnx=%g,cny=%g,dx=%g,dy=%g\n", ddx, ddy, midx, midy, cx[k], cy[k], cnx[k], cny[k], dx[k], dy[k]);
	for (int i = 0; i < 5; i++)
	{
		nk = cclosest(midx, midy, cx, cy, nc, pCellularWindowNorm, cnx, cny, dx, dy, pRectangularToVoronoiShapeBlend);
		if (((k == closestFeatureID && nk == k) || (k != closestFeatureID&&nk != k))) { xa = midx; ya = midy; }
		else { xx = midx; yy = midy; }
		midx = (xa + xx) / 2.0f; midy = (ya + yy) / 2.0f;
	}
	float cdi = cdistance(midx, midy, cx[k], cy[k], pCellularWindowNorm, cnx[k], cny[k], dx[k], dy[k], pRectangularToVoronoiShapeBlend);
	//printf("%g : k=%d, closestFeatureID=%d, ddx=%g, ddy=%g, midx=%g,midy=%g, cx=%g,cy=%g,cnx=%g,cny=%g,dx=%g,dy=%g\n", cdi, k, closestFeatureID, ddx, ddy, midx, midy, cx[k], cy[k], cnx[k], cny[k], dx[k], dy[k]);
	return cdi;
	//dx = cx[k] - midx, dy = cy[k] - midy;
	//return (float)std::pow(std::pow(abs(dx), pCellularWindowNorm) + std::pow(abs(dy), pCellularWindowNorm), 1.0 / pCellularWindowNorm);
	//return sqrt(dx*dx + dy*dy);
}

/******************************************************************************
 * nthclosest()
 *
 * Computer distances to nth-closest points and sort them from closest to farthest.
 *
 * @param closestFeatureIDs [OUT]
 * @param nn
 * @param xx
 * @param yy
 * @param cx
 * @param cy
 * @param nc
 * @param pCellularWindowNorm
 * @param pFeatureCellCenterX
 * @param pFeatureCellCenterY
 * @param dx
 * @param dy
 * @param pRectangularToVoronoiShapeBlend
 ******************************************************************************/
static void nthclosest(
	int closestFeatureIDs[],
	int nn, float xx, float yy, float cx[], float cy[], int nc, 
	float pCellularWindowNorm, float pFeatureCellCenterX[], float pFeatureCellCenterY[], float dx[], float dy[], float pRectangularToVoronoiShapeBlend )
{
	// Loop counter
	int i, k;

	float dist[ 200 ];

	// Iterate through poinset (elements)
	for ( k = 0; k < nc; k++ )
	{
		// Compute distance to current point in poinset
		// Computer closest distance to cell border the point lies in.
		// Smooth blend between rectangular cell shapes and p-norm Voronoï cell shapes.
		float dd = cdistance( xx, yy, cx[ k ], cy[ k ], pCellularWindowNorm, pFeatureCellCenterX[ k ], pFeatureCellCenterY[ k ], dx[ k ], dy[ k ], pRectangularToVoronoiShapeBlend );
		
		//float dx = xx - cx[k];
		//float dy = yy - cy[k];
		//float dd = (float)std::pow(std::pow(abs(dx), pCellularWindowNorm) + std::pow(abs(dy), pCellularWindowNorm), 1.0 / pCellularWindowNorm);

		dist[ k ] = dd;
	}

	// Sort points from closest to farthest
	for ( i = 0; i < nn; i++ )
	{
		int mk = 0;
		for ( k = 1; k < nc; k++ )
		{
			if ( dist[ mk ] > dist[ k ] )
			{
				mk = k;
			}
		}
		closestFeatureIDs[ i ] = mk; 
		dist[ mk ] = 100000.0;
	}
}

/******************************************************************************
 * Evaluate PPTBF
 ******************************************************************************/
float PtPPTBF::eval( const float x, const float y,
	// Point process
	const PtPPTBF::tilingtype pTilingType, const float pJittering, const float pCellSubdivisionProbability, const int pNbRelaxationIterations,
	// Window function
	const float pCellularToGaussianWindowBlend, /*const*/ float pCellularWindowNorm, const float pRectangularToVoronoiShapeBlend, const float pCellularWindowDecay, const float pGaussianWindowDecay, const float pGaussianWindowDecayJittering,
	// Feature function
	const int pMinNbGaborKernels, const int pMaxNbGaborKernels, /*const*/ float pFeatureNorm, const int pGaborStripesFrequency, const float pGaborStripesCurvature, const float pGaborStripesOrientation, const float pGaborStripesThickness, const float pGaborDecay, const float pGaborDecayJittering, const float pFeaturePhaseShift, const bool pBombingFlag,
	// Others
	const float pRecursiveWindowSubdivisionProbability, const float pRecursiveWindowSubdivisionScale,
	// Debug
	const bool pShowWindow, const bool pShowFeature )
{
	// PPTBF value
	float pptbf = 0.f;

	// Loop counters
	int i, k;

	int decalx = /*tt*/2;
	int Nx = 1;
	
	float featurePointX[ 9 * 4 ], featurePointY[ 9 * 4 ], featureCellCenterX[ 9 * 4 ], featureCellCenterY[ 9 * 4 ], ndx[ 9 * 4 ], ndy[ 9 * 4 ];

	// Distribution of elements: spatial point process.
	// Points are generated for 1-neighborhood (for next stage of nth-closest point search)
	int nc = distributeElements( x, y, pTilingType, pCellSubdivisionProbability, pCellSubdivisionProbability, decalx, Nx, pNbRelaxationIterations, pJittering,
		featurePointX, featurePointY, // OUT: list of generated points coordinates
		featureCellCenterX, featureCellCenterY, // OUT: list of associated cells centers
		ndx, ndy ); // OUT: list of associated cells closest distance to borders

	// Clamp max nb of "elements" to 18
	int npp = ( nc < 18 ? nc : 18 );
	// Compute distances to nth-closest points and sort them from closest to farthest.
	int closestFeatureIDs[ 20 ];
	nthclosest( closestFeatureIDs/*OUT*/, npp, x, y, featurePointX, featurePointY, nc, pCellularWindowNorm, featureCellCenterX, featureCellCenterY, ndx, ndy, pRectangularToVoronoiShapeBlend );

	// Closest point
	float centerx, centery;
	centerx = featurePointX[ closestFeatureIDs[ 0 ] ]; centery = featurePointY[ closestFeatureIDs[ 0 ] ];

	//float inorm = 1.0f;
	//float inormgauss = 1.0f;

	float pointvalue;

	float mval = 1.f;
	float vval = 0.f;

	float pmax = -2.0f;

	//------------------------------------
	//params / pptbf_001_0_00_05_0_00_00_00_10_05_20_0
	//type = 0
	//tt = static_cast< PtPPTBF::tilingtype >( 0 );
	//larp = 0;
	//inorm = 0.5;
	//inormgauss = 0.5;
	//nrelax = 0;
	//pRecursiveWindowSubdivisionProbability = 0;
	//sigwcellfact = 0.960124;
	//sigwgaussfact = 0.873739;
	//pRecursiveWindowSubdivisionScale = 0.520615;
	//pCellSubdivisionProbability = 0;
	//jitter = 0;
	//pCellularToGaussianWindowBlend = 1;
	//pCellularWindowDecay = 0.945958;
	//pGaussianWindowDecay = 2;
	//pGaussianWindowDecayJittering = 0.582092;
	//pBombingFlag = 0;
	//pMinNbGaborKernels = 0;
	//pMaxNbGaborKernels = 0;
	//pGaborDecay = 1;
	//pGaborDecayJittering = 0;
	//pGaborStripesFrequency = 0;
	//phase = 0;
	//pGaborStripesThickness = 1;
	//pGaborStripesCurvature = 0;
	//pGaborStripesOrientation = 1.5708;
	/*ampli[0] = 0.0487949;
	ampli[1] = 0.0461046;
	ampli[2] = 0.0271428;*/
	//pCellularWindowNorm = getNorm( inorm );//<=0.5 ? 1.0f + inorm / 0.5f : 2.0f + std::pow((inorm - 0.5f) / 0.5f, 0.5f)*50.0f;
	//pFeatureNorm = getNorm( inormgauss ); // <= 0.5 ? 1.0f + inormgauss / 0.5f : 2.0f + std::pow((inormgauss - 0.5f) / 0.5f, 0.5f)*50.0f;
	//------------------------------------

	// Sparse convolution: P x ( F W )
	// Iterate through n-th closest points (sorted elements)
	for ( k = 0; k < npp; k++ ) // for each neighbor cell / point
	{
		// Initialize random process based on current point coordinate (k-th closest point)
		seeding( (unsigned int)( featurePointX[ closestFeatureIDs[ k ] ] * 12.0 + 7.0 ), (unsigned int)( featurePointY[ closestFeatureIDs[ k ] ] * 12.0 + 1.0 ), 0 );
		
		float npointvalue = mval + vval * next();
		if ( k == 0 )
		{
			pointvalue = npointvalue;
		}
		//
		float nsig2 = pGaussianWindowDecay + pGaussianWindowDecayJittering * next();
		//
		float subdiv = next() * 0.5f + 0.5f;
		//
		float prior = next() * 0.5f + 0.5f;

		// Number of Gabor kernels
		const int nbGaborKernels = pMinNbGaborKernels + (int)( (float)( pMaxNbGaborKernels - pMinNbGaborKernels ) * ( 0.5 * next() + 0.5 ) );
		if ( nbGaborKernels > 10 )
		{
			printf( "pMinNbGaborKernels=%d, pMaxNbGaborKernels=%d\n", pMinNbGaborKernels, pMaxNbGaborKernels );// hvFatal( "stop" );
		}
		
		// Compute feature function, by default is constant=1.0
		float featureFunction = 1.0;
		float lx[ 10 ], ly[ 10 ], angle[ 10 ], sigma[ 10 ];
		// Initialize Gabor kernels
		if ( nbGaborKernels > 0 ) // set parameters of cosines
		{
			//lx[0] = featurePointX[closestFeatureIDs[k]]; ly[0] = featurePointY[closestFeatureIDs[k]]; angle[0] = pGaborStripesOrientation*next();
			//sigma[0] = (pGaborDecay + pGaborDecayJittering*next())*(1.0 + 2.0*(float)nbGaborKernels / 9.0);

			// Iterate over Gabor kernels
			for ( i = 0; i < nbGaborKernels; i++ )
			{
				// Random position xj selected inside Ri to get random phase shifts
				lx[ i ] = featureCellCenterX[ closestFeatureIDs[ k ] ] + next() * pJittering * ndx[ closestFeatureIDs[ k ] ];
				ly[ i ] = featureCellCenterY[ closestFeatureIDs[ k ] ] + next() * pJittering * ndy[ closestFeatureIDs[ k ] ];
				// Random orientation
				angle[ i ] = pGaborStripesOrientation * next();
				// Random decay
				sigma[ i ] = ( pGaborDecay + pGaborDecayJittering * next() ) * ( 1.0f + 2.0f * (float)nbGaborKernels / 9.0f );

				//if (k==0) printf("n=%d, daa=%g, mdist=%g, x=%g, y=%g\n", i, daa, mdist, lx[i], ly[i]);
			}
		}

		// do sum of cosines 
		if ( nbGaborKernels > 0 && pGaborStripesFrequency >= 0 )
		{
			featureFunction = 0.0;
			// Iterate through Gabor kernels
			for ( i = 0; i < nbGaborKernels; i++ )
			{
				// Stretch, rotation
				float deltax = ( x - lx[ i ] ) / ndx[ closestFeatureIDs[ k ] ];
				float deltay = ( y - ly[ i ] ) / ndy[ closestFeatureIDs[ k ] ];
				float ddx = pGaborStripesCurvature * ( deltax * cos( -angle[ i ] ) - deltay * sin( -angle[ i ] ) );
				float ddy = deltax * sin( -angle[ i ] ) + deltay * cos( -angle[ i ] );
				float dd2 = (float)std::pow( std::pow( abs( ddx ), pFeatureNorm ) + std::pow( abs( ddy ), pFeatureNorm ), 1.0 / pFeatureNorm );
				
				const float gaborSinusoidalWave = 0.5f + 0.5f * cos( 2.0f * static_cast< float >( M_PI ) * ( (float)pGaborStripesFrequency ) * dd2 );
				const float gaborGaussian = exp( -dd2 * sigma[ i ] ); // beware: gaussian vs RBF !!!!
				const float gaborKernel = std::pow( gaborSinusoidalWave, 1.0f / pGaborStripesThickness ) * gaborGaussian;
				
				featureFunction += gaborKernel;
			}

			// Normalization
			featureFunction /= (float)( nbGaborKernels / 4 > 0 ? nbGaborKernels / 4 : 1 );
			if ( featureFunction > 1.0 ) featureFunction = 1.0f;

			// Add global phase shift to be able to carve patterns in window function W
			// - apply phase which should be 0.0 or PI/2
			featureFunction = featureFunction * cos( pFeaturePhaseShift ) * cos( pFeaturePhaseShift ) + ( 1.0f - featureFunction ) * sin( pFeaturePhaseShift ) * sin( pFeaturePhaseShift );
		}

		// Compute window function: blend between cellular and gaussian

		// Gaussian
		// NOTE: erreur !!!!!! gaussian vs radial basis function
		float ddx = ( x - featurePointX[ closestFeatureIDs[ k ] ] ) / ndx[ closestFeatureIDs[ k ] ];
		float ddy = ( y - featurePointY[ closestFeatureIDs[ k ] ] ) / ndy[ closestFeatureIDs[ k ] ];
		float sdd = sqrt( ddx * ddx + ddy * ddy );
		float gaussianWindow = exp( -nsig2 * sdd );

		// cellular
		float cellularWindow = 0.0f;
		if ( pCellularToGaussianWindowBlend > 0.05f && k == 0 )
		{
			// LOG
			//printf("n ");

			float celldd = celldist(x, y, closestFeatureIDs[k], closestFeatureIDs[0], featurePointX, featurePointY, nc, pCellularWindowNorm, featureCellCenterX, featureCellCenterY, ndx, ndy, pRectangularToVoronoiShapeBlend);
			float dd = cdistance(x, y, featurePointX[closestFeatureIDs[k]], featurePointY[closestFeatureIDs[k]], pCellularWindowNorm, featureCellCenterX[closestFeatureIDs[k]], featureCellCenterY[closestFeatureIDs[k]], ndx[closestFeatureIDs[k]], ndy[closestFeatureIDs[k]], pRectangularToVoronoiShapeBlend);
			float dist = celldd < 0.0001f ? 0.0f : dd / (celldd);
			
			// Distance is 0 on borders (i.e. inverse of Worley approach)
			cellularWindow = 1.0f - dist;
			
			// LOG
			//printf("it celldd=%g, dd=%g, dist=%g, cellularWindow=%g\n", celldd, dd,dist,cellularWindow);
			
			// Clamp data in [0;1]
			if ( cellularWindow < 0.0f )
			{
				cellularWindow = 0.0f;
			}
			else if ( cellularWindow > 1.0f )
			{
				cellularWindow = 1.0f;
			}

			cellularWindow = std::pow( cellularWindow, pCellularWindowDecay );
		}

		// Blend between cellular and gaussian windows
		float windowFunction = pCellularToGaussianWindowBlend * cellularWindow + ( 1.0f - pCellularToGaussianWindowBlend ) * gaussianWindow;
		//printf("it k=%d, dd2=%g, cellularWindow =%g, windowFunction=%g, nbGaborKernels=%d, featureFunction=%g\n", k, dd2, cellularWindow,windowFunction,nbGaborKernels, featureFunction);

		//// TEST --------------------------
		//pptbf += windowFunction * featureFunction;

		// recursive subdivision of the cell for window function only
		if ( subdiv < pRecursiveWindowSubdivisionProbability )
		{
			float ncenterx, ncentery;
		/*	float ncoeff = eval(x*pRecursiveWindowSubdivisionScale + 10.0*featurePointX[closestFeatureIDs[k]], y*pRecursiveWindowSubdivisionScale + 4.0*featurePointY[closestFeatureIDs[k]], tt, 0.0, decalx, Nx, 0, pJittering,
				pCellularWindowNorm, pCellularToGaussianWindowBlend, pCellularWindowDecay*sig1fact, pFeatureNorm, pGaborDecay*sig2fact, pGaborDecayJittering*sig2fact,
				pRectangularToVoronoiShapeBlend, mval, vval, 0.0, 0.0, 0.0, 0.0,
				false, 0, 0, 0.0, 0.0, 0, 0.0, 0, 1.0, 0.0,
				ampli, pointvalue, ncenterx, ncentery);*/
			//--------------------------
			//float ncoeff = 1.f; // TEST !!!!!
			//--------------------------------------------
			float sig1fact = 1.f;
			float sig2fact = 1.f;
			//--------------------------------------------
			//float ncoeff = eval( x * pRecursiveWindowSubdivisionScale + 10.0 * featurePointX[ closestFeatureIDs[ k ] ], y * pRecursiveWindowSubdivisionScale + 4.0 * featurePointY[ closestFeatureIDs[ k ] ],
			//	pTilingType, 0.0/*ppointsub*/,
			//	decalx, Nx, 0/*nrelax*/, pJittering,
			//	pCellularWindowNorm, pCellularToGaussianWindowBlend, pCellularWindowDecay * sig1fact,
			//	pFeatureNorm, pGaborDecay * sig2fact, pGaborDecayJittering * sig2fact,
			//	pRectangularToVoronoiShapeBlend,
			//	mval, vval,
			//	0.0/*psubdivcell*/, 0.0/*sig1fact*/, 0.0/*sig2fact*/, 0.0/*subdivscale*/,
			//	false/*bombing*/,
			//	0/*Npmin*/, 0/*Npmax*/,
			//	0.0/*sigcos*/, 0.0/*sigcosvar*/,
			//	0/*freq*/, 0.0/*phase*/, 0/*thickness*/, 1.0/*courbure*/, 0.0/*deltaorient*/,
			//	ampli,
			//	pointvalue, ncenterx, ncentery );
			float ncoeff = eval(
				// Position
				x * pRecursiveWindowSubdivisionScale + 10.0f * featurePointX[ closestFeatureIDs[ k ] ], y * pRecursiveWindowSubdivisionScale + 4.0f * featurePointY[ closestFeatureIDs[ k ] ],
				// Point process
				pTilingType, pJittering, 0.0f/*pCellSubdivisionProbability*/, 0/*pNbRelaxationIterations*/,
				// Window function
				pCellularToGaussianWindowBlend, pCellularWindowNorm, pRectangularToVoronoiShapeBlend, pCellularWindowDecay, pGaussianWindowDecay, pGaussianWindowDecayJittering,
				// Feature function
				0/*pMinNbGaborKernels*/, 0/*pMaxNbGaborKernels*/, pFeatureNorm, 0/*pGaborStripesFrequency*/, 1.0f/*pGaborStripesCurvature*/, 0.f/*pGaborStripesOrientation*/, 0/*pGaborStripesThickness*/, pGaborDecay, pGaborDecayJittering, 0.0f/*pFeaturePhaseShift*/, false/*pBombingFlag*/,
				// Others
				0.f/*pRecursiveWindowSubdivisionProbability*/, 0.f/*pRecursiveWindowSubdivisionScale*/, pShowWindow, pShowFeature );
			//float PtPPTBF::eval( const float x, const float y,
			//// Point process
			//const PtPPTBF::tilingtype pTilingType, const float pJittering, const float pCellSubdivisionProbability, const int pNbRelaxationIterations,
			//// Window function
			//const float pCellularToGaussianWindowBlend, /*const*/ float pCellularWindowNorm, const float pRectangularToVoronoiShapeBlend, const float pCellularWindowDecay, const float pGaussianWindowDecay, const float pGaussianWindowDecayJittering,
			//// Feature function
			//const int pMinNbGaborKernels, const int pMaxNbGaborKernels, /*const*/ float pFeatureNorm, const int pGaborStripesFrequency, const float pGaborStripesCurvature, const float pGaborStripesOrientation, const float pGaborStripesThickness, const float pGaborDecay, const float pGaborDecayJittering, const float pFeaturePhaseShift, const bool pBombingFlag,
			//// Others
			//const float pRecursiveWindowSubdivisionProbability, const float pRecursiveWindowSubdivisionScale )
			//--------------------------

			// keep min window value
			if ( windowFunction > ncoeff )
			{
				centerx = ( ncenterx - 10.0f * featurePointX[ closestFeatureIDs[ k ] ] ) / pRecursiveWindowSubdivisionScale;
				centery = ( ncentery - 4.0f * featurePointY[ closestFeatureIDs[ k ] ] ) / pRecursiveWindowSubdivisionScale;
				windowFunction = ncoeff;
			}
		}

		// Check whether or not to use bombing
		if ( pBombingFlag )
		{
			//printf("cell %d: at %g,%g, windowFunction=%g, featureFunction=%g, prior=%g, max=%g\n", k, featurePointX[closestFeatureIDs[k]], featurePointY[closestFeatureIDs[k]], windowFunction, featureFunction, prior, pmax);

			// add contributions, except if bombing
			if ( pmax < prior && windowFunction > 0.1f )
			{
				pmax = prior;
				pptbf = windowFunction * featureFunction;
				
				pointvalue = npointvalue;

				centerx = featurePointX[ closestFeatureIDs[ k ] ];
				centery = featurePointY[ closestFeatureIDs[ k ] ];
			}
		}
		else
		{
			if ( ! pShowWindow )
			{
				windowFunction = 1.f;
			}
			if ( ! pShowFeature )
			{
				featureFunction = 1.f;
			}
			pptbf += windowFunction * featureFunction;
		}
	}

	return pptbf;
}
