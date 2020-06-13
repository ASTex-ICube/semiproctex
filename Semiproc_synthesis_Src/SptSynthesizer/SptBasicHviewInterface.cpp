/*
 * Publication: Semi-Procedural Textures Using Point Process Texture Basis Functions
 *              Computer Graphics Forum (EGSR 2020 special issue)
 * Authors: P. Guehl , R. AllEgre , J.-M. Dischler, B. Benes , and E. Galin
 *
 * Code author: Pascal Guehl
 */

/** 
 * @version 1.0
 */

#include "SptBasicHviewInterface.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// Project
#include "SptImageHelper.h" 

// System
#include <cassert>

// STL
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <fstream>
#include <sstream>

// hview
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#include <hvVec2.h>
#include <hvArray2.h>
#include <hvBitmap.h>
#include <hvPictRGB.h>
#include "hvPicture.h"

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// Project
using namespace Spt;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/**
 * OUTPUT SIZE
 */
const int SS = 400;

const int NPPTBFS = 1;
const int MAXPPTBF = 10;

int pptbfid = 0;
int di = 1; // for transition, next pptbf is pptbfid + di 

std::string pptbfParameters[NPPTBFS];
char *pptbf_param[NPPTBFS] = {
	"0 0.91 75 0 3 0 0 1 1 8 0.9 2 0 1 0.7 1 2 0 0 5 5 1 0.2 2 0 0.1 0.2 0"
};

const bool TRANSITION = false;
const float threshtransition = 1.0; // 1.0 = keep same tresh

const int SPOSX = 0;
const int SPOSY = 0;

//float ZZ = 1.0;
/*const */int PSIZE = 512;
/*const*/ int STARTX = 10 * PSIZE;
/*const*/ int STARTY = 10 * PSIZE;
int shiftx = STARTX + SPOSX * PSIZE + 0 * PSIZE, shifty = STARTY + SPOSY * PSIZE;

float featpercent[10];

const int npptbf = 1;

char* name[npptbf] = {
		"cracked_asphalt_160796"
};

int do_tiling[NPPTBFS];

float jittering[MAXPPTBF];
int resolution[MAXPPTBF];
float rotation[MAXPPTBF];
float rescalex[MAXPPTBF];
float turbAmp[MAXPPTBF][3];

int windowShape[MAXPPTBF];
float windowArity[MAXPPTBF];
float windowLarp[MAXPPTBF];
float windowNorm[MAXPPTBF];
float windowSmoothness[MAXPPTBF];
float windowBlend[MAXPPTBF];
float windowSigwcell[MAXPPTBF];

int featureBombing[MAXPPTBF];
float featureNorm[MAXPPTBF];
float featureWinfeatcorrel[MAXPPTBF];
float featureAniso[MAXPPTBF];
int featureMinNbKernels[MAXPPTBF], featureMaxNbKernels[MAXPPTBF];
float featureSigcos[MAXPPTBF];
float featureSigcosvar[MAXPPTBF];
int featureFrequency[MAXPPTBF];
float featurePhaseShift[MAXPPTBF];
float featureThickness[MAXPPTBF];
float featureCurvature[MAXPPTBF];
float featureOrientation[MAXPPTBF];

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

namespace hview
{
	class hvPictureMetrics
	{
	public:
		
		static void histogramm(hview::hvPict<float> &pp, float *histo, int bins, float vmin, float vmax)
		{
			int i, j;
			for (i = 0; i < bins; i++) histo[i] = 0.0f;
			for (j = 0; j < pp.sizeY(); j++) for (i = 0; i < pp.sizeX(); i++)
			{
				int iv = (int)((float)(bins)*(pp.get(i, j) - vmin) / (vmax - vmin));
				if (iv >= bins) iv = bins - 1;
				histo[iv] += 1.0f / (float)(pp.sizeX()*pp.sizeY());
			}
		}

		static float computeThresh(float percent, float *histo, int bins, float vmin, float vmax)
		{
			int i;
			float sum = 0.0;
			for (i = 0; i < bins && sum < percent; i++) sum += histo[i];
			if (i == bins) return vmax;
			float ratio = (percent - (sum - histo[i])) / histo[i];
			//float ratio = 0.5f;
			//printf("compute Thresh bin =%d, sum=%g, percent=%g\n", i, sum, percent);
			return vmin + (vmax - vmin)*((float)(i - 1) / (float)bins + ratio / (float)bins);
		}
	};
}

/******************************************************************************
 * PPTBF synthesis
 *
 * @param pixelzoom
 * @param pptbfpi
 * @param ppval
 ******************************************************************************/
void SptBasicHviewInterface::pptbfshader( float pixelzoom, hview::hvPict< float >& pptbfpi, hview::hvPict< float >& ppval )
{
#ifndef USE_MULTITHREADED_PPTBF
	int i, j;
#else
	float pointvalue, cellpointx, cellpointy;
	float pointvalue2, cellpointx2, cellpointy2;
#endif

	for (int ipara = 0; ipara < NPPTBFS; ipara++)
	{
#if 0
		sscanf(pptbf_param[ipara], "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n",
			&do_tiling[ipara], &jittering[ipara],
			&resolution[ipara], &rotation[ipara], &rescalex[ipara],
			&turbAmp[ipara][0], &turbAmp[ipara][1], &turbAmp[ipara][2],
			&windowShape[ipara], &windowArity[ipara], &windowLarp[ipara], &windowNorm[ipara], &windowSmoothness[ipara], &windowBlend[ipara], &windowSigwcell[ipara],
			&featureBombing[ipara], &featureNorm[ipara], &featureWinfeatcorrel[ipara], &featureAniso[ipara], &featureMinNbKernels[ipara], &featureMaxNbKernels[ipara], &featureSigcos[ipara], &featureSigcosvar[ipara], &featureFrequency[ipara], &featurePhaseShift[ipara], &featureThickness[ipara], &featureCurvature[ipara], &featureOrientation[ipara]);
#else
		sscanf(pptbfParameters[ipara].c_str(), "%d %g %d %g %g %g %g %g %d %g %g %g %g %g %g %d %g %g %g %d %d %g %g %d %g %g %g %g\n",
			&do_tiling[ipara], &jittering[ipara],
			&resolution[ipara], &rotation[ipara], &rescalex[ipara],
			&turbAmp[ipara][0], &turbAmp[ipara][1], &turbAmp[ipara][2],
			&windowShape[ipara], &windowArity[ipara], &windowLarp[ipara], &windowNorm[ipara], &windowSmoothness[ipara], &windowBlend[ipara], &windowSigwcell[ipara],
			&featureBombing[ipara], &featureNorm[ipara], &featureWinfeatcorrel[ipara], &featureAniso[ipara], &featureMinNbKernels[ipara], &featureMaxNbKernels[ipara], &featureSigcos[ipara], &featureSigcosvar[ipara], &featureFrequency[ipara], &featurePhaseShift[ipara], &featureThickness[ipara], &featureCurvature[ipara], &featureOrientation[ipara]);
#endif
	}

	//for (int ipara = 0; ipara<NPPTBFS; ipara++) turbAmp[ipara][0] *= 2.0;

	//hvPictRGB<unsigned char> testImage(pptbfpi.sizeX(), example[pptbfid].sizeY(), hvColRGB<unsigned char>(0));

#ifdef USE_MULTITHREADED_PPTBF
	const int nbPixels = pptbfpi.sizeX() * pptbfpi.sizeY();
	PtThreadPool.AppendTask([&](const MyThreadPool::ThreadData* thread)
		{
			int beg = thread->id * pptbfpi.sizeY() / thread->nThreads;
			int end = (thread->id + 1) * pptbfpi.sizeY() / thread->nThreads;
			end = std::min(end, pptbfpi.sizeY());

			for (int j = beg; j < end; ++j)
				for (int i = 0; i < pptbfpi.sizeX(); ++i)
					//for ( int k = thread->id; k < nbPixels; k += thread->nThreads )
#else
	//#pragma omp parallel
	for (i = 0; i < pptbfpi.sizeX(); i++) for (j = 0; j < pptbfpi.sizeY(); j++)
#endif
	{
#ifdef USE_MULTITHREADED_PPTBF
		//int i = k % pptbfpi.sizeX();
		//int j = k / pptbfpi.sizeX();
		//std::cout << "thread->id: " << thread->id << " (" << i << "," << j << ")" << std::endl;
#endif
		float pointvalue, cellpointx, cellpointy;
		float pointvalue2, cellpointx2, cellpointy2;

		float x = ((float)i*pixelzoom + (float)(shiftx - padding)) / (float)SS + 10.0;
		float y = ((float)j*pixelzoom + (float)(shifty - padding)) / (float)SS + 10.0;
		float zoom = 0.0;
		float pptbfvv = 0.f;

		float transition = (float)i / (float)pptbfpi.sizeX(); // +0.3*(1.0 - 2.5*hvNoise::turbulence((double)x*2.0, (double)y*2.0, 1.24, 0.0001));
		if (transition < 0.25) transition = 0.0;
		else if (transition < 0.75) transition = (transition - 0.25) / 0.5;
		else transition = 1.0;

		if (!TRANSITION)
		{
			int qq = pptbfid;
			zoom = (float)SS / (float)resolution[qq];
			//windowBlend[qq] = transition;
			//if (j > 200) windowLarp[qq] = 0.0;
			//else windowLarp[qq] = 0.5-0.5*(float)j / 200.0;
			//windowSmoothness[qq] = 1.5;
			//if (i >= pptbfpi.sizeX() - 100) windowSmoothness[qq] -= 0.5*(float)(i - pptbfpi.sizeX() + 100) / 100.0;
			pptbfvv = hview::hvNoise::cpptbf_gen_v2c(x, y,
				zoom, rotation[qq] * M_PI, rescalex[qq], turbAmp[qq],
				do_tiling[qq], jittering[qq],
				windowShape[qq], windowArity[qq], windowLarp[qq], windowSmoothness[qq], windowNorm[qq], windowBlend[qq], windowSigwcell[qq],
				featureBombing[qq], featureNorm[qq], featureWinfeatcorrel[qq], featureAniso[qq],
				featureMinNbKernels[qq], featureMaxNbKernels[qq], featureSigcos[qq], featureSigcosvar[qq],
				featureFrequency[qq], featurePhaseShift[qq] * M_PI*0.5, featureThickness[qq], featureCurvature[qq], featureOrientation[qq] * M_PI,
				pointvalue, cellpointx, cellpointy
			);
			//pptbfvv = 1.0 - pptbfvv;
			//pptbfvv = 1.0 - pptbfvv;
			//int subid = pptbfid+1;
			//float scx = 1.0, scy = 1.0;
			//if (pptbfvv >= 0.0) //thresh[pptbfid])
			//{
			//	zoom = (float)SS / (float)resolution[subid];
			//	float pptbfvvsub = hvNoise::cpptbf_gen_v2(x*scx, y*scy, //cellpointx*100.0 + x*scx, cellpointy*100.0 + y*scy,
			//		zoom, rotation[subid], rescalex[subid], turbAmp[subid],
			//		do_tiling[subid], jittering[subid],
			//		windowShape[subid], windowArity[subid], windowLarp[subid], windowSmoothness[subid], windowNorm[subid], windowBlend[subid], windowSigwcell[subid],
			//		featureBombing[subid], featureNorm[subid], featureWinfeatcorrel[subid], featureAniso[subid],
			//		featureMinNbKernels[subid], featureMaxNbKernels[subid], featureSigcos[subid], featureSigcosvar[subid],
			//		featureFrequency[subid], featurePhaseShift[subid], featureThickness[subid], featureCurvature[subid], featureOrientation[subid]
			//	);
			//	//pptbfvv = pptbfvvsub;
			//	pptbfvv = pow(pptbfvv < pptbfvvsub ? pptbfvv : pptbfvvsub, 0.5);
			//	//pptbfvv = pow(pptbfvvsub, 1.0);
			//}
		}
		else
		{
			// Not provided in this version...
			assert( false );
		}

		if (!std::isnan(pptbfvv) && !std::isinf(pptbfvv))
		{
			// Write data
			// - pptbf
			pptbfpi.update(i, j, pptbfvv);
			// - random value
			ppval.update(i, j, pointvalue);
		}
		else
		{
			printf("\nPPTBF: nan or inf...");
		}
	}
#ifdef USE_MULTITHREADED_PPTBF
		}); // multi-thread
#endif
}

/******************************************************************************
 * Constructor
 ******************************************************************************/
SptBasicHviewInterface::SptBasicHviewInterface()
:	mExemplarName()
,	GUIDE( 0.f )
,	STRENGTH( 0.f )
,	INITLEVEL( 0 )
,	BLOCSIZE( 0 )
,	INITERR( 0.f )
,	INDEXWEIGHT( 0.f )
,	padding( 0 )
{
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
SptBasicHviewInterface::~SptBasicHviewInterface()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void SptBasicHviewInterface::initialize()
{
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void SptBasicHviewInterface::finalize()
{
}

/******************************************************************************
 * Launch the synthesis pipeline
 ******************************************************************************/
void SptBasicHviewInterface::execute()
{
	// TODO:
	// Put here, code of the basic software algorithm
	// ...

	/*const*/ int STOPATLEVEL = 0;

	//int labelmapType = std::stoi(pArgv[indexParameter++]); // default: 0: random, 1: classification
	int labelmapType = 0;

	const bool TRANSITION = false;
	const float threshtransition = 1.0; // 1.0 = keep same tresh

	const int SPOSX = 0;
	const int SPOSY = 0;

	float vvmin[10] = { 0.0 };
	float vvmax[10] = { 1.0 };

	char pname[ 500 ];

	hview::hvPictRGB<unsigned char> example[NPPTBFS];
	hview::hvPictRGB<unsigned char> exalbedo[NPPTBFS];
	hview::hvBitmap exseg[NPPTBFS];

	int i, j, k, ii, jj;

	// Update automatic padding
	padding = (1 << INITLEVEL) * BLOCSIZE;

	hview::hvColRGB<unsigned char> pptbf_col1[] = { hview::hvColRGB<unsigned char>(208,68,43),hview::hvColRGB<unsigned char>(208,68,43),hview::hvColRGB<unsigned char>(93,80,68), hview::hvColRGB<unsigned char>(200,150,130),hview::hvColRGB<unsigned char>(200,150,130), hview::hvColRGB<unsigned char>(113,174, 210),hview::hvColRGB<unsigned char>(113,174, 210),
		//hvColRGB<unsigned char>(177,157,133),hvColRGB<unsigned char>(177,157,133),  hvColRGB<unsigned char>(208,208,208),hvColRGB<unsigned char>(208,208,208),hvColRGB<unsigned char>(200,126,113), hvColRGB<unsigned char>(200,126,113), 
		hview::hvColRGB<unsigned char>(239,224,95),hview::hvColRGB<unsigned char>(239,224,195), hview::hvColRGB<unsigned char>(87,80,71),hview::hvColRGB<unsigned char>(87,80,71),hview::hvColRGB<unsigned char>(179, 129, 103), hview::hvColRGB<unsigned char>(179, 129, 103), hview::hvColRGB<unsigned char>(68,89,113), hview::hvColRGB<unsigned char>(68,89,113) };
	hview::hvColRGB<unsigned char> pptbf_col2[] = { hview::hvColRGB<unsigned char>(108,38,23),hview::hvColRGB<unsigned char>(108,38,23), hview::hvColRGB<unsigned char>(208, 206, 202),hview::hvColRGB<unsigned char>(208, 206, 202), hview::hvColRGB<unsigned char>(90,80,70),hview::hvColRGB<unsigned char>(90,80,70), hview::hvColRGB<unsigned char>(154,104,80),hview::hvColRGB<unsigned char>(154,104,80),
		//hvColRGB<unsigned char>(85,74,64),hvColRGB<unsigned char>(85,74,64), hvColRGB<unsigned char>(63,63,65),hvColRGB<unsigned char>(63,63,65),hvColRGB<unsigned char>(70,55,55), hvColRGB<unsigned char>(70,50,55), 
		hview::hvColRGB<unsigned char>(123,84,44),hview::hvColRGB<unsigned char>(123,84,44), hview::hvColRGB<unsigned char>(27,26,23),hview::hvColRGB<unsigned char>(27,26,23), hview::hvColRGB<unsigned char>(156,165,163), hview::hvColRGB<unsigned char>(156,165,163), hview::hvColRGB<unsigned char>(201,230,249), hview::hvColRGB<unsigned char>(201,230,249) };

	k = 0;
	name[ 0 ] = const_cast< char* >( mExemplarName.c_str() );

	sprintf( pname, "%s_scrop.png", name[ k ] );

	// Read image
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	const int desired_channels = 0; // TODO: handle this parameter!
	unsigned char* data = nullptr;
	SptImageHelper::loadImage( pname, width, height, nrChannels, data, desired_channels );
	if ( data == nullptr )
	{
		std::cout << "Failed to load image: " << pname << std::endl;
		assert( false );
		//return 1; // TODO: handle this!
	}

	///////////////////////////////////////////////////////////////////////
	assert( width == height );
	//PSIZE = 2 * width; // we want 2 times the original size for our tests !!

	//---------------------------------------------------------------------------------
	// PSIZE must be a multiple of BLOCSIZE
	//int deltaPSIZE = 0;
	//PSIZE += deltaPSIZE;
	{
		// mimic makepyr() method
		int s = 0;
		int f = 1;
		int w = width;
		int h = height;
		while ( s < INITLEVEL && w > 16 && h > 16 )
		{
			// shrink size
			w /= 2;
			h /= 2;

			// update pyramid info
			s++;
			f *= 2;
		}

		// Handle error
		if ( w < 2 * BLOCSIZE )
		{
			printf( "\nERROR: block size %d must be less or equal than %d", w / 2 );
			assert( false );
		}

		int outputSize = 2 * width; // we want 2 times the original size for our tests !!
		int exemplarBlockSize = f * BLOCSIZE;
		if ( width < exemplarBlockSize || height < exemplarBlockSize )
		{
			int maxBlockSize = std::min( width, height ) / f;
			printf( "\ERROR: block size %d must be less or equal to %d", BLOCSIZE, maxBlockSize );
			assert( false );
		}
		int nbBlocks = outputSize / ( f * BLOCSIZE ) + ( ( outputSize % ( f * BLOCSIZE ) ) == 0 ? 0 : 1 );
		PSIZE = nbBlocks * ( f * BLOCSIZE );
	}
	//---------------------------------------------------------------------------------

	STARTX = 10 * PSIZE;
	STARTY = 10 * PSIZE;
	shiftx = STARTX + SPOSX * PSIZE + 0 * PSIZE;
	shifty = STARTY + SPOSY * PSIZE;
	///////////////////////////////////////////////////////////////////////

	exalbedo[k].reset(width, height, hview::hvColRGB<unsigned char>(0));
	for (int ii = 0; ii < width; ii++) for (int jj = 0; jj < height; jj++)
	{
		exalbedo[k].update(ii, jj, hview::hvColRGB<unsigned char>(data[nrChannels * (ii + jj*width)], data[nrChannels * (ii + jj*width) + 1], data[nrChannels * (ii + jj*width) + 2]));
	}
	SptImageHelper::freeImage( data );

	sprintf(pname, "%s_seg_scrop.png", name[k]);
	data = nullptr;
	SptImageHelper::loadImage( pname, width, height, nrChannels, data, desired_channels );
	if (data == nullptr)
	{
		std::cout << "Failed to load image: " << pname << std::endl;
		assert(false);
		//return 1;
	}
	exseg[k].reset(width, height, false);
	for (int ii = 0; ii < width; ii++) for (int jj = 0; jj < height; jj++)
	{
		exseg[k].set(ii, jj, (int)data[nrChannels * (ii + jj*width)] > 128 ? true : false);
	}
	SptImageHelper::freeImage( data );

	////////////////////////////////////////////////////////////////////////////////////////
	sprintf(pname, "%s_seg_scrop_pptbf_params.txt", name[k]);
	std::ifstream estimatedParameterFile(pname);

	std::string lineData;

	// Get threshold
	float threshold;
	std::getline(estimatedParameterFile, lineData);
	std::stringstream ssthreshold(lineData);
	ssthreshold >> threshold;

	// Get PPTBF parameters
	std::getline(estimatedParameterFile, lineData);

	// Store data
	float thresh[2] = {
		0.899284
	};
#if 0
	thresh[k] = threshold;
#else
	thresh[k] = 0.9f * threshold;
	printf("\nBEWARE: use0.95*threshold to test thin space between cells !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
#endif
	pptbf_param[k] = const_cast<char*>(lineData.c_str());
	pptbfParameters[k] = lineData;

	// LOG info
	std::cout << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << "PPTBF" << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	// PPTBF parameters
	std::vector< float > pptbf;
	std::stringstream sspptbf(lineData);
	float value;
	while (sspptbf >> value)
	{
		// Fill parameters
		pptbf.push_back(value);
	}
	int h = 0;
	// Point Process
	std::cout << "[POINT PROCESS]" << std::endl;
	std::cout << "tiling type: " << pptbf[h++] << std::endl;
	std::cout << "jittering: " << pptbf[h++] << std::endl;

	// Transformation
	std::cout << std::endl;
	std::cout << "[TRANSFORMATION]" << std::endl;
	std::cout << "resolution: " << pptbf[h++] << std::endl;
	std::cout << "rotation: " << pptbf[h++] << std::endl;
	std::cout << "aspectRatio: " << pptbf[h++] << std::endl;

	// Turbulence
	std::cout << std::endl;
	std::cout << "[TURBULENCE]" << std::endl;
	std::cout << "base amplitude: " << pptbf[h++] << std::endl;
	std::cout << "gain: " << pptbf[h++] << std::endl;
	std::cout << "frequency: " << pptbf[h++] << std::endl;

	// Window Function
	std::cout << std::endl;
	std::cout << "[WINDOW FUNCTION]" << std::endl;
	std::cout << "shape: " << pptbf[h++] << std::endl;
	std::cout << "arity: " << pptbf[h++] << std::endl;
	std::cout << "larp: " << pptbf[h++] << std::endl;
	std::cout << "norm: " << pptbf[h++] << std::endl;
	std::cout << "smoothness: " << pptbf[h++] << std::endl;
	std::cout << "blend: " << pptbf[h++] << std::endl;
	std::cout << "decay: " << pptbf[h++] << std::endl;

	// Feature Function
	std::cout << std::endl;
	std::cout << "[FEATURE FUNCTION]" << std::endl;
	std::cout << "type: " << pptbf[h++] << std::endl;
	std::cout << "norm: " << pptbf[h++] << std::endl;
	std::cout << "window feature correlation: " << pptbf[h++] << std::endl;
	std::cout << "anisotropy: " << pptbf[h++] << std::endl;
	std::cout << "nb min kernels: " << pptbf[h++] << std::endl;
	std::cout << "nb max kernels: " << pptbf[h++] << std::endl;
	std::cout << "decay: " << pptbf[h++] << std::endl;
	std::cout << "decay delta: " << pptbf[h++] << std::endl;
	std::cout << "frequency: " << pptbf[h++] << std::endl;
	std::cout << "phase shift: " << pptbf[h++] << std::endl;
	std::cout << "thickness: " << pptbf[h++] << std::endl;
	std::cout << "curvature: " << pptbf[h++] << std::endl;
	std::cout << "orientation: " << pptbf[h++] << std::endl;

	//////////////////////////////////////////////////////////////////////////////////////////
	//// CUSTOM PCTS-like synthesis parameters
	//sprintf( pname, "%s_synthesis_params.txt", name[ k ] );
	//std::ifstream synthesisParameterFile( pname );
	//
	//// Get threshold
	//std::string text;

	//synthesisParameterFile >> text;
	//synthesisParameterFile >> GUIDE;

	//synthesisParameterFile >> text;
	//synthesisParameterFile >> STRENGTH;

	//synthesisParameterFile >> text;
	//synthesisParameterFile >> INITLEVEL;

	//synthesisParameterFile >> text;
	//synthesisParameterFile >> BLOCSIZE;

	//// Update automatic padding
	//padding = ( 1 << INITLEVEL ) * BLOCSIZE;

	//synthesisParameterFile >> text;
	//synthesisParameterFile >> INITERR;

	//synthesisParameterFile >> text;
	//synthesisParameterFile >> INDEXWEIGHT;

	// Update automatic padding
	//padding = ( 1 << INITLEVEL ) * BLOCSIZE;

	// LOG info
	std::cout << std::endl;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << "CUSTOM PCTS-like synthesis parameters:" << std::endl;
	std::cout << "--------------------------------------------------------\n" << std::endl;
	std::cout << "- GUIDE = " << GUIDE << std::endl;
	std::cout << "- STRENGTH = " << STRENGTH << std::endl;
	std::cout << "- INITLEVEL = " << INITLEVEL << std::endl;
	std::cout << "- BLOCSIZE = " << BLOCSIZE << std::endl;
	std::cout << "- INITERR = " << INITERR << std::endl;
	std::cout << "- INDEXWEIGHT = " << INDEXWEIGHT << std::endl;
	std::cout << "- labelmapType = " << labelmapType << std::endl;
	std::cout << std::endl;
	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// SAVE CUSTOM PCTS-like synthesis parameters
	sprintf(pname, "%s_synthesis_params.txt", name[k]);
	std::ofstream synthesisParameterFile(pname);
	synthesisParameterFile << "GUIDE ";
	synthesisParameterFile << GUIDE;
	synthesisParameterFile << std::endl;
	synthesisParameterFile << "STRENGTH ";
	synthesisParameterFile << STRENGTH;
	synthesisParameterFile << std::endl;
	synthesisParameterFile << "INITLEVEL ";
	synthesisParameterFile << INITLEVEL;
	synthesisParameterFile << std::endl;
	synthesisParameterFile << "BLOCSIZE ";
	synthesisParameterFile << BLOCSIZE;
	synthesisParameterFile << std::endl;
	synthesisParameterFile << "INITERR ";
	synthesisParameterFile << INITERR;
	synthesisParameterFile << std::endl;
	synthesisParameterFile << "INDEXWEIGHT ";
	synthesisParameterFile << INDEXWEIGHT;
	synthesisParameterFile << std::endl;
	synthesisParameterFile << "labelmapType ";
	synthesisParameterFile << labelmapType;
	synthesisParameterFile << std::endl;

	printf("TEXTURE: %s\n", name[0]);
	printf("\ninput size: (%d,%d)", width, height);
	printf("\noutput size: (%d,%d)\n", PSIZE + 2 * padding, PSIZE + 2 * padding);
	std::cout << "- (automatic) padding: " << padding << std::endl;
	std::cout << std::endl;

	example[k].reset(exalbedo[k].sizeX(), exalbedo[k].sizeY(), hview::hvColRGB<unsigned char>(0));
	for (int ii = 0; ii < width; ii++) for (int jj = 0; jj < height; jj++)
	{
		example[k].update(ii, jj, exalbedo[k].get(ii, jj));
	}

	std::cout << "--------------------------------------------" << std::endl;
	std::cout << "CONNECTED COMPONENTS EXTRACTION" << std::endl;
	std::cout << "--------------------------------------------\n" << std::endl;
	// - timer
	auto startTime = std::chrono::high_resolution_clock::now();

	int nbLabels = 0;
	std::vector< hview::hvColRGB< unsigned char > > labelColors;

	//--------------------------------------------------------------------------------------
	hview::hvPictRGB< unsigned char > featrndcol(example[pptbfid].sizeX(), example[pptbfid].sizeY(), hview::hvColRGB< unsigned char >(0));
	switch (labelmapType)
	{
		// random
	case 0:
		std::cout << "labelmap: random" << std::endl;
		std::cout << std::endl;
		break;

		// classification
	case 1:
	{
		// at least one label (black for background)
		//nbLabels = 1;
		//labelColors.push_back( hvColRGB< unsigned char >( 0, 0, 0 ) );

		// Load label map
		sprintf(pname, "%s_featcol_class.png", name[0]);
		int width, height, nrChannels;
		unsigned char* data = nullptr;
		//stbi_set_flip_vertically_on_load(1);
		SptImageHelper::loadImage( pname, width, height, nrChannels, data, desired_channels );
		if (data == nullptr)
		{
			std::cout << "Failed to load image: " << pname << std::endl;
			assert(false);
			//return 1;
		}

		// Count nb features
		int c = 0;
		for (int xxx = 0; xxx < width * height; xxx++)
		{
			int r = data[c++];
			int g = data[c++];
			int b = data[c++];
			hview::hvColRGB< unsigned char > labelColor(r, g, b);

			// Search color
			bool found = false;
			for (const auto& color : labelColors)
			{
				if (color == labelColor)
				{
					found = true;
				}
			}

			// Store color if not found
			if (!found)
			{
				labelColors.push_back(labelColor);
			}
		}
		nbLabels = labelColors.size();
		std::cout << "labelmap: with classification" << std::endl;
		std::cout << "nb labels: " << nbLabels << std::endl;
		int xxx = 0;
		std::cout << "LABEL COLORS:" << std::endl;
		for (const auto& c : labelColors)
		{
			std::cout << static_cast<int>(c.RED()) << " " << static_cast<int>(c.GREEN()) << " " << static_cast<int>(c.BLUE()) << std::endl;
		}

		// Update label map
		for (int jj = 0; jj < featrndcol.sizeY(); jj++)
		{
			for (int ii = 0; ii < featrndcol.sizeX(); ii++)
			{
				featrndcol.update(ii, jj,
					hview::hvColRGB< unsigned char >(data[nrChannels * (ii + jj * width)],
						data[nrChannels * (ii + jj * width) + 1],
						data[nrChannels * (ii + jj * width) + 2]));
			}
		}
		char buff[256];
		sprintf(buff, "%s_featcol.%s", name[pptbfid], "ppm");
		FILE* fd = fopen(buff, "wb");
		//if (fd == 0) { perror("cannot load file: "); return 1; }
		featrndcol.savePPM(fd, 1);
		fclose(fd);

		// Free memory
		SptImageHelper::freeImage(data);
	}
	break;

	default:
		break;
	}
	//--------------------------------------------------------------------------------------

	const int MEDIANFILT = 5;
	const int EROSION = 1;
	hview::hvBitmap ffiltex; ffiltex.median(exseg[pptbfid], MEDIANFILT / 2, MEDIANFILT / 2);
	for (int kk = 0; kk < EROSION; kk++) ffiltex.erosion(3, 3);
	hview::hvPict< float > fpval(example[pptbfid].sizeX(), example[pptbfid].sizeY(), 1.0f);
	hview::hvBitmap excomp, ccomp, allfeat(example[pptbfid].sizeX(), example[pptbfid].sizeY(), false);
	excomp = ffiltex;
	const int MAX_FEAT = 1000;
	float featsizes[MAX_FEAT];
	float meansize = 0.0;
	int nnfeatures = 0, largefeatures = 0;
	float vrand[MAX_FEAT];
	std::vector<hview::hvVec2<int> > fcenters;
	//hvPictRGB<unsigned char> featrndcol(example[pptbfid].sizeX(), example[pptbfid].sizeY(), hvColRGB<unsigned char>(0));

	//--------------------------------------------------------------------------------------
	switch (labelmapType)
	{
		// classification
	case 1:
	{
		// Reset values
		for (int i = 0; i < MAX_FEAT; i++)
		{
			vrand[i] = 0.f;
		}

		// Fill random values per label
		vrand[0] = 0.1f + 0.9f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		//for ( int i = 1; i <= nbLabels; i++ )
		for (int i = 1; i < nbLabels; i++)
		{
			const float v = 0.1f + 0.9f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			vrand[i] = v;
		}

		// BEWARE: update connected component (or label) counter
		//nnfeatures = nbLabels;
	}
	break;

	default:
		break;
	}
	//--------------------------------------------------------------------------------------


	//if ( labelmapType == 0 ) // random
	//{

	if (labelmapType == 0) // random
	{
		vrand[0] = 0.1f + 0.9f*(float)rand() / (float)RAND_MAX;
	}
	while (excomp.extractCC(ccomp)) {
		hview::hvColRGB<unsigned char> frndcol(50 + (unsigned char)((float)rand() / (float)RAND_MAX*200.0),
			50 + (unsigned char)((float)rand() / (float)RAND_MAX*200.0),
			50 + (unsigned char)((float)rand() / (float)RAND_MAX*200.0));
		hview::hvVec2<int> min, max;
		ccomp.box(min, max);
		if ((max.X() - min.X() > 20 && max.Y() - min.Y() > 20) && (min.X() > 4 && max.X() < excomp.sizeX() - 1 - 4 && min.Y() > 4 && max.Y() < excomp.sizeY() - 1 - 4)
			||
			(max.X() - min.X() > 100 || max.Y() - min.Y() > 100))
		{
			int ncount = 0;
			hview::hvVec2<int> bary(0, 0);

			float val = 0.f;
			if (labelmapType == 0) // random
			{
				val = 0.1f + 0.9f*(float)rand() / (float)RAND_MAX;
				vrand[nnfeatures + 1] = val;
			}

			int pixelcount = ccomp.count();
			meansize += pixelcount;
			largefeatures++;
			printf("CComp : %d,%d \n", (min.X() + max.X()) / 2, (min.Y() + max.Y()) / 2);

			hview::hvBitmap ccompl; ccompl.dilatation(ccomp, 3, 3);
			for (int ii = min.X(); ii <= max.X(); ii++) for (int jj = min.Y(); jj <= max.Y(); jj++)
				if (ccompl.get(ii, jj))
				{
					if (labelmapType == 1) // classification
					{
						// Retrieve class ID

						const hview::hvColRGB< unsigned char >& labelColor = featrndcol.get(ii, jj);

						// Search color
						int classID = 0;
						for (const auto& color : labelColors)
						{
							if (color == labelColor)
							{
								break;
							}

							classID++;
						}

						val = vrand[classID];
						//printf( " %d", classID ); // for debug
					}
					fpval.update(ii, jj, val);

					if (labelmapType == 0) // random
					{
						featrndcol.update(ii, jj, frndcol);
					}

					ncount++;

					bary += hview::hvVec2<int>(ii, jj);
				}

			featsizes[nnfeatures] = (float)ncount / (float)(example[pptbfid].sizeX()* example[pptbfid].sizeY());
			nnfeatures++;
			bary = hview::hvVec2<int>(bary.X() / ncount, bary.Y() / ncount);
			fcenters.push_back(bary);
		}
		allfeat |= ccomp;
		~ccomp; excomp &= ccomp;
	}
	for (int ii = 0; ii < fpval.sizeX(); ii++) for (int jj = 0; jj < fpval.sizeY(); jj++)
	{
		if (!ffiltex.get(ii, jj)) fpval.update(ii, jj, vrand[0]);
	}
	printf("\nnfeat=%d\n", nnfeatures);

	//	}
		// - timer
	auto endTime = std::chrono::high_resolution_clock::now();
	float elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
	std::cout << "\ntime: " << elapsedTime << " ms\n";

	if (labelmapType == 1) // classification
	{
		nnfeatures = nbLabels - 1; // nnfeatures do not have background
	}

	// Apply the normalization
	const int EROD = 10;
	const float FEATPOWER = 0.1;
	hview::hvBitmap ffilt; ffilt.median(exseg[pptbfid], MEDIANFILT / 2, MEDIANFILT / 2);
	hview::hvBitmap ffeat; ffeat = ffilt;  //ffeat.dilatation(EROD, EROD);
	hview::hvPict<unsigned char> featbw(ffeat, EROD, 255);
	unsigned char minbw, maxbw; featbw.minmax(minbw, maxbw);
	~ffilt; ffeat = ffilt;  //ffeat.dilatation(EROD/2, EROD/2);
	hview::hvPict<unsigned char> featbwr(ffeat, EROD, 255);
	unsigned char minbwr, maxbwr; featbwr.minmax(minbwr, maxbwr);
	hview::hvPict<float> featnormal(ffeat.sizeX(), ffeat.sizeY(), 0.0);
	hview::hvPictRGB<unsigned char> featdog(example[pptbfid].sizeX(), example[pptbfid].sizeY(), hview::hvColRGB<unsigned char>(0));
	hview::hvPictRGB<unsigned char> featdogpastel(example[pptbfid].sizeX(), example[pptbfid].sizeY(), hview::hvColRGB<unsigned char>(0));
	for (i = 0; i < example[pptbfid].sizeX(); i++) for (j = 0; j < example[pptbfid].sizeY(); j++)
	{
		float vbw = (float)featbw.get(i, j) / (float)maxbw;
		float vbwr = (float)featbwr.get(i, j) / (float)maxbwr;

		featnormal.update(i, j, vbw > vbwr ? vbw : vbwr);

		hview::hvColRGB<unsigned char> col = hview::hvColRGB<unsigned char>(
			(unsigned char)(100 + (int)(pow(vbw, FEATPOWER)*150.0)),
			100 + (int)(pow(vbwr, FEATPOWER)*150.0),
			(unsigned char)(fpval.get(i, j)*150.0)); // WARNING: casting to uchar with x150 can erase and mix different labels (ex: 0.478 and 0.475)
#if 0
			// label map with classification
		if (labelmapType == 1)
		{
			const int labelID = std::max(0, static_cast<int>(std::floor(fpval.get(i, j) * nbLabels)) - 1);
			col[2] = static_cast<unsigned char>(labelID);
		}
#endif
		//featdog.update(i, j, hvColRGB<unsigned char>(featbw.get(i, j), featbwr.get(i, j), (unsigned char)(255.0*fpval.get(i, j))));
		featdog.update(i, j, col);

		featdogpastel.update(i, j, hview::hvColRGB<unsigned char>(150 + (unsigned char)(pow(vbw, 0.2)*100.0*(0.5*fpval.get(i, j) + 0.5)) + pow(vbwr, 0.2)*100.0));
		//if (fpval.get(i, j) == 1.0f && featbw.get(i, j) == 255) printf("max val at %d,%d\n", i, j);
		//0*(pquant.getIndex(i,j)==imax?0:pifilt.get(i,j).RED())));
	}

	char buff[256];

	char* extension = "ppm";

	sprintf(buff, "%s_feat.%s", name[pptbfid], extension);

	FILE* fd = fopen(buff, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	featdog.savePPM(fd, 1);
	fclose(fd);

	sprintf(buff, "%s_feat_pastel.%s", name[pptbfid], extension);

	fd = fopen(buff, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	featdogpastel.savePPM(fd, 1);
	fclose(fd);

	sprintf(buff, "%s_feat_pastel.%s", name[pptbfid], extension);

	fd = fopen(buff, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	featdogpastel.savePPM(fd, 1);
	fclose(fd);

	hview::hvPictRGB<unsigned char> featnormalrgb(featnormal, 255.0);
	sprintf(buff, "%s_featnormal.%s", name[pptbfid], extension);

	fd = fopen(buff, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	featnormalrgb.savePPM(fd, 1);
	fclose(fd);

	//printf( "Synthesis of plane...\n" );

	hview::hvPictRGB<unsigned char> nouv(PSIZE + 2 * padding, PSIZE + 2 * padding, hview::hvColRGB<unsigned char>());
	hview::hvPict<hview::hvVec2<int> > index(PSIZE + 2 * padding, PSIZE + 2 * padding, hview::hvVec2<int>(0));

	hview::hvPict<float> pptbfpi(PSIZE + 2 * padding, PSIZE + 2 * padding, 0.f);
	hview::hvPict<float> ppval(PSIZE + 2 * padding, PSIZE + 2 * padding, 0.f);
	hview::hvPictRGB<unsigned char> pptbfrgb(PSIZE + 2 * padding, PSIZE + 2 * padding, 0.0);
	hview::hvPictRGB<unsigned char> pptbfbin(PSIZE + 2 * padding, PSIZE + 2 * padding, 0.0);

	std::cout << "\n--------------------------------------------" << std::endl;
	std::cout << "PPTBF SYNTHESIS" << std::endl;
	std::cout << "--------------------------------------------" << std::endl;
	// - timer
	startTime = std::chrono::high_resolution_clock::now();
	//-------------------------------------
	// Launch PPTBF software generation computer
	pptbfshader( 1.f, pptbfpi, ppval );
	//-------------------------------------
	// - timer
	endTime = std::chrono::high_resolution_clock::now();
	elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
	std::cout << "\ntime: " << elapsedTime << " ms\n";

	//featpercent[pptbfid] = (float)exseg[pptbfid].count() / (float)(exseg[pptbfid].sizeX()*exseg[pptbfid].sizeY());
	float pvmin, pvmax;
	pptbfpi.minmax(pvmin, pvmax);
	const int NHBINS = 128;
	float histopptbf[NHBINS];
	hview::hvPictureMetrics::histogramm(pptbfpi, histopptbf, NHBINS, pvmin, pvmax);

	featpercent[pptbfid] = thresh[pptbfid];

	float pptbfthresh = hview::hvPictureMetrics::computeThresh(1.0 - featpercent[pptbfid], histopptbf, NHBINS, pvmin, pvmax);

	thresh[pptbfid] = pptbfthresh;
	printf("\n%g percent = threshold for PPTBF %d is %g, min=%g, max=%g\n", featpercent[pptbfid], pptbfid, pptbfthresh, pvmin, pvmax);
	vvmin[pptbfid] = pvmin;
	vvmax[pptbfid] = pvmax;
	thresh[pptbfid + di] = thresh[pptbfid] * threshtransition;
	vvmin[pptbfid + di] = pvmin;
	vvmax[pptbfid + di] = pvmax;

	int ipara = 0;
	// file already exists, but threshold is different => it's the % instead of the real pptbf value to use

	for (ii = 0; ii < nouv.sizeX(); ii++)
	{
		//float transition = (float)ii / (float)nouv.sizeX();
		for (jj = 0; jj < nouv.sizeY(); jj++)
		{
			//float x = (float)(ii*ZZ + shiftx*ZZ) / (float)SS + 10.0;
			//float y = (float)(jj*ZZ + shifty*ZZ) / (float)SS + 10.0;
			//float turb;
			float transition = (float)ii / (float)nouv.sizeX(); // +0.3*(1.0 - 2.5*hvNoise::turbulence((double)x*2.0, (double)y*2.0, 1.24, 0.0001));
			if (transition < 0.25) transition = 0.0;
			else if (transition < 0.75) transition = (transition - 0.25) / 0.5;
			else transition = 1.0;
			if (TRANSITION) pptbfthresh = thresh[pptbfid] * (1.0 - transition) + thresh[pptbfid + di] * transition;
			//else pptbfthresh = thresh[pptbfid];
			//float valf = (pptbfpi.get(ii, jj) - vvmin[pptbfid]) / (vvmax[pptbfid] - vvmin[pptbfid]);
			//float valf2 = (pptbfpi.get(ii, jj) - vvmin[pptbfid + 1]) / (vvmax[pptbfid + 1] - vvmin[pptbfid + 1]);


			float valf = pptbfpi.get(ii, jj);
			if (valf < 0.0) valf = 0.0; else if (valf > 1.0) valf = 1.0;
			unsigned char val = (unsigned char)(255.0*valf);
			pptbfrgb.update(ii, jj, hview::hvColRGB<unsigned char>(val, val, val));

			//i = (int)((float)(ii*ZZ)*FSCALE + shiftx);
			//j = (int)((float)(jj*ZZ)*FSCALE + shifty);

			hview::hvColRGB<unsigned char> col1, col1b;
			hview::hvVec4<int> pos1;
			hview::hvVec4<int> vv1;

			col1 = pptbf_col1[pptbfid];
			if (TRANSITION) col1b = pptbf_col1[pptbfid + di];

			hview::hvColRGB<unsigned char> col2, col2b;
			hview::hvVec4<int> pos2;
			hview::hvVec4<int> vv2;

			col2 = pptbf_col2[pptbfid];
			if (TRANSITION) col2b = pptbf_col2[pptbfid + di];

			if (pptbfpi.get(ii, jj) > pptbfthresh) {
				if (TRANSITION) col1.interpolate(col1, col1b, transition);
				if (pptbfpi.get(ii, jj) < pptbfthresh*1.2) col1.interpolate(col2, col1, (pptbfpi.get(ii, jj) - pptbfthresh) / (pptbfthresh*1.2 - pptbfthresh));
				nouv.update(ii, jj, col1);
				index.update(ii, jj, hview::hvVec2<int>(pos1.X(), pos1.Y()));
				pptbfbin.update(ii, jj, hview::hvColRGB<unsigned char>(255, 255, 255));
			}
			else {
				if (TRANSITION) col2.interpolate(col2, col2b, transition);
				nouv.update(ii, jj, col2);
				index.update(ii, jj, hview::hvVec2<int>(256, 493));
				pptbfbin.update(ii, jj, hview::hvColRGB<unsigned char>(0, 0, 0));
			}
		}
	}

	sprintf(pname, "%s_col%s.%s", name[pptbfid], TRANSITION ? "_t" : "", extension);

	fd = fopen(pname, "wb");
	nouv.savePPM(fd, 1);
	fclose(fd);

	sprintf(pname, "%s_pptbf%s.%s", name[pptbfid], TRANSITION ? "_t" : "", extension);

	fd = fopen(pname, "wb");
	pptbfrgb.savePPM(fd, 1);
	fclose(fd);

	sprintf(pname, "%s_pptbf_bin%s.%s", name[pptbfid], TRANSITION ? "_t" : "", extension);

	fd = fopen(pname, "wb");
	pptbfbin.savePPM(fd, 1);
	fclose(fd);

	float rangea = hview::hvPictureMetrics::computeThresh((1.0 - featpercent[pptbfid])*GUIDE, histopptbf, NHBINS, pvmin, pvmax);
	float rangeb = hview::hvPictureMetrics::computeThresh((1.0 - featpercent[pptbfid]) + featpercent[pptbfid] * (1.0 - GUIDE), histopptbf, NHBINS, pvmin, pvmax);
	float thresha = hview::hvPictureMetrics::computeThresh((1.0 - featpercent[pptbfid])*0.95, histopptbf, NHBINS, pvmin, pvmax);
	float threshb = hview::hvPictureMetrics::computeThresh((1.0 - featpercent[pptbfid]) + featpercent[pptbfid] * (1.0 - 0.95), histopptbf, NHBINS, pvmin, pvmax);
	printf("rangea=%g, rangeb=%g\n", rangea, rangeb);

	hview::hvPictRGB<unsigned char> pictdog(nouv.sizeX(), nouv.sizeY(), hview::hvColRGB<unsigned char>(0));
	hview::hvPictRGB<unsigned char> pictdogpastel(nouv.sizeX(), nouv.sizeY(), hview::hvColRGB<unsigned char>(0));
	hview::hvPictRGB<unsigned char> pictdogmask(nouv.sizeX(), nouv.sizeY(), hview::hvColRGB<unsigned char>(0));
	hview::hvBitmap mask(nouv.sizeX(), nouv.sizeY(), false);
	hview::hvPict<float> ppvalue(nouv.sizeX(), nouv.sizeY(), 0.0);

	for (j = 0; j < nouv.sizeY(); j++) for (i = 0; i < nouv.sizeX(); i++)
	{
		float transition = (float)ii / (float)nouv.sizeX(); // +0.3*(1.0 - 2.5*hvNoise::turbulence((double)x*2.0, (double)y*2.0, 1.24, 0.0001));
		if (transition < 0.25) transition = 0.0;
		else if (transition < 0.75) transition = (transition - 0.25) / 0.5;
		else transition = 1.0;
		if (TRANSITION) pptbfthresh = thresh[pptbfid] * (1.0 - transition) + thresh[pptbfid + di] * transition;

		float valf = pptbfpi.get(i, j);
		//if (valf < 0.0) valf = 0.0; else if (valf > 1.0) valf = 1.0;
		if (i >= 2 && j >= 2 && i <= nouv.sizeX() - 3 && j <= nouv.sizeY() - 3 && (valf<rangea || valf>rangeb)) mask.set(i, j, true);

		int bind = 1 + (int)((float)nnfeatures*(ppval.get(i, j)*0.5 + 0.5));
		if (bind >= nnfeatures + 1)
		{
			bind = nnfeatures + 1; // BUG ? should be clamp to "nnfeatures" instead ?
			printf("\nWARNING: random selected ID (%d) seems to exceed nb labels or connected components (error?)", bind);
		}
		ppvalue.update(i, j, vrand[bind]);
		if (valf < pptbfthresh) ppvalue.update(i, j, vrand[0]);

		float vbw = 0.0;
		float vbwr = 0.0;
		float vnorm;
		if (valf < pptbfthresh)
		{
			vnorm = (1.0 - valf / pptbfthresh);
			vbwr = vnorm;
		}
		else
		{
			vnorm = (valf - pptbfthresh) / (pvmax - pptbfthresh);
			vbw = vnorm;
		}
		//pictnorm.update(i, j, vnorm);

		// Guidance map: pptbf
		hview::hvColRGB<unsigned char> col = hview::hvColRGB<unsigned char>(
			(unsigned char)(100 + (int)(pow(vbw, FEATPOWER)*150.0)),
			100 + (int)(pow(vbwr, FEATPOWER)*150.0),
			(unsigned char)(150.0*ppvalue.get(i, j)));
#if 0
		// label map with classification
		if (labelmapType == 1)
		{
			const int labelID = std::max(0, static_cast<int>(std::floor(ppvalue.get(i, j) * nbLabels)) - 1);
			col[2] = static_cast<unsigned char>(labelID);
		}
#endif
		pictdog.update(i, j, col);

		pictdogmask.update(i, j, mask.get(i, j) ? pictdog.get(i, j) : hview::hvColRGB<unsigned char>(0));
		if (valf < pptbfthresh) pictdogpastel.update(i, j, pptbf_col2[pptbfid]);
		else
		{
			pictdogpastel.update(i, j, pptbf_col1[pptbfid]);
			//float colc = pow(vbw, 0.2)*(0.5*ppvalue.get(i, j) + 0.5);
			//pictdogpastel.update(i, j, hvColRGB<unsigned char>((unsigned char)((float)pptbf_col1[pptbfid].RED()*colc), (unsigned char)((float)pptbf_col1[pptbfid].GREEN()*colc), (unsigned char)((float)pptbf_col1[pptbfid].BLUE()*colc)));
			//pictdogpastel.update(i, j, hvColRGB<unsigned char>(150 + (unsigned char)(pow(vbw, 0.2)*100.0*(0.5*ppvalue.get(i, j) + 0.5)) + pow(vbwr, 0.2)*100.0));
		}
	}

	char fname[256];

	sprintf(fname, "%s_pptbf_feat.%s", name[pptbfid], extension);

	fd = fopen(fname, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	pictdog.savePPM(fd, 1);
	fclose(fd);

	sprintf(fname, "%s_pptbf_featpastel.%s", name[pptbfid], extension);

	fd = fopen(fname, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	pictdogpastel.savePPM(fd, 1);
	fclose(fd);

	sprintf(fname, "%s_pptbf_featmask.%s", name[pptbfid], extension);

	fd = fopen(fname, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	pictdogmask.savePPM(fd, 1);
	fclose(fd);

	hview::hvPictRGB<unsigned char> res, resfeat;
	hview::hvArray2<hview::hvVec2<int> > iindex, indexx;
	hview::hvPictRGB<unsigned char> pictdogextrapol;
	hview::hvBitmap maskextrapol;
	if (TRANSITION)
	{
		pictdogextrapol.clone(pictdog, 0, 0, pictdog.sizeX() - 1, pictdog.sizeY() - 1);
		maskextrapol = mask;
	}
	else
	{
		int ppx = nouv.sizeX() - 2 * PSIZE; if (ppx < 0) ppx = 0;
		pictdogextrapol.clone(pictdog, ppx, 0, pictdog.sizeX() - 1, pictdog.sizeY() - 1);
		maskextrapol.reset(nouv.sizeX() - ppx, nouv.sizeY(), false);
		for (int ii = 0; ii < maskextrapol.sizeX(); ii++) for (int jj = 0; jj < maskextrapol.sizeY(); jj++)
			if (mask.get(ii + ppx, jj)) maskextrapol.set(ii, jj, true);
	}
	///////////////////////////////////////////////////////////////

	std::cout << "\n--------------------------------------------" << std::endl;
	std::cout << "COLOR/MATERIAL SYNTHESIS" << std::endl;
	std::cout << "--------------------------------------------" << std::endl;

	// - timer
	startTime = std::chrono::high_resolution_clock::now();

	// Launch color/material synthesis
	res.semiProceduralTextureSynthesis(const_cast<char*>(name[pptbfid]),
		STOPATLEVEL,
		shiftx, shifty,
		example[pptbfid], featdog,
		0.8, // weight color vs distance
		STRENGTH, // weight to accentuate or diminish guidance (as a power)
		3.0*INDEXWEIGHT*INDEXWEIGHT, // label weight
		2, // neighbor size for comparing neighbor candidate
		INITLEVEL, // level at which to start pcst synthesis vs smart initialization
		BLOCSIZE, // size of copied block of UV at smart initialization
		INITERR, // maximum acceptable error for smart initialization
		maskextrapol, // guidance mask (binary pptbf)
		pictdogextrapol, // guidance (pptbf)
		indexx); // main uv map (i.e. indirection map)

	// - timer
	endTime = std::chrono::high_resolution_clock::now();
	elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
	std::cout << "- time: " << elapsedTime << " ms\n";

	hview::hvPictRGB<unsigned char> synthalbedo(indexx.sizeX(), indexx.sizeY(), hview::hvColRGB<unsigned char>());
	synthalbedo.imagefromindex(exalbedo[pptbfid], indexx);
	sprintf(buff, "%s_semiproc_albedo.%s", name[pptbfid], extension);

	fd = fopen(buff, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	synthalbedo.savePPM(fd, 1);
	fclose(fd);

	hview::hvPictRGB<unsigned char> synthpastel(indexx.sizeX(), indexx.sizeY(), hview::hvColRGB<unsigned char>());
	synthpastel.imagefromindex(featdogpastel, indexx);
	sprintf(buff, "%s_semiproc_featpastel.%s", name[pptbfid], extension);

	fd = fopen(buff, "wb");
	//if (fd == 0) { perror("cannot load file:"); return 1; }
	synthpastel.savePPM(fd, 1);
	fclose(fd);

	hview::hvPictRGB<unsigned char> synthfeat(indexx.sizeX(), indexx.sizeY(), hview::hvColRGB<unsigned char>());
	synthfeat.imagefromindex(featdog, indexx);
	sprintf(buff, "%s_semiproc_feat.%s", name[pptbfid], extension);

	fd = fopen(buff, "wb");
//	if (fd == 0) { perror("cannot load file:"); return 1; }
	synthfeat.savePPM(fd, 1);
	fclose(fd);

	printf("\nfinish.(press return)\n");
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setExemplarName( const char* pText )
{
	mExemplarName = std::string( pText );
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setGUIDE( const float pValue )
{
	GUIDE = pValue;
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setSTRENGTH( const float pValue )
{
	STRENGTH = pValue;
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setINITLEVEL( const int pValue )
{
	INITLEVEL = pValue;
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setBLOCSIZE( const int pValue )
{
	BLOCSIZE = pValue;
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setINITERR( const float pValue )
{
	INITERR = pValue;
}

/******************************************************************************
 * Semi-procedural texture synthesis parameters
 ******************************************************************************/
void SptBasicHviewInterface::setINDEXWEIGHT( const float pValue )
{
	INDEXWEIGHT = pValue;
}
