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

#include "PtPPTBFLoader.h"
 
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

//// Project
//#include "PtPPTBF.h"

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

/******************************************************************************
 * Constructor
 ******************************************************************************/
PtPPTBFLoader::PtPPTBFLoader()
{
	// Point process
	PtPPTBF::tilingtype pTilingType = PtPPTBF::REGULAR;
	pJittering = 0.f;
	pCellSubdivisionProbability = 0.f;
	pNbRelaxationIterations = 0;
	// Window function
	pCellularToGaussianWindowBlend = 0.f;
	pCellularWindowNorm = 0.5;;
	pRectangularToVoronoiShapeBlend = 0.f;
	pCellularWindowDecay = 1.f;
	pGaussianWindowDecay = 1.f;
	pGaussianWindowDecayJittering = 0.f;
	// Feature function
	pMinNbGaborKernels = 0;
	pMaxNbGaborKernels = 0;
	pFeatureNorm = 0.5f;
	pGaborStripesFrequency = 4;
	pGaborStripesCurvature = 0.15f;
	pGaborStripesOrientation = 0.2f;
	pGaborStripesThickness = 0.15f;
	pGaborDecay = 1.f;
	pGaborDecayJittering = 0.f;
	pFeaturePhaseShift = 0.f;
	pBombingFlag = false;
	// Others
	pRecursiveWindowSubdivisionProbability = 0.f;
	pRecursiveWindowSubdivisionScale = 0.5f;
	// Debug
	pShowWindow = true;
	pShowFeature = true;
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
PtPPTBFLoader::~PtPPTBFLoader()
{
}

/******************************************************************************
 * Initialize
 ******************************************************************************/
void PtPPTBFLoader::initialize()
{
}

/******************************************************************************
 * Finalize
 ******************************************************************************/
void PtPPTBFLoader::finalize()
{
}

/******************************************************************************
 * Import
 ******************************************************************************/
PtPPTBF* PtPPTBFLoader::import( const char* pFilename )
{
	PtPPTBF* pptbf = new PtPPTBF();

	/*char *name;*/ float origpercent;
		int RESOL; float alpha; float rescalex;
		float thresh;
		// point set parameters
		PtPPTBF::tilingtype tt; float ppointsub; int decalx; int Nx; int nrelax; float jitter;
		float inorm; float windowblend; float sigwcell; float inorm2; float sigwgauss; float sigwgaussvar;
		float larp;
		float pointval; float varpointval;
		float psubdiv; float sigwcellfact; float sigwgaussfact; float subdivscale;
		bool bomb; float sigcos; float sigcosvar; int Npmin; int Npmax;
		int freq; float phase; float thickness; float courbure; float deltaorient;
		float ampli[3];

	char buff[100]; char *str;
			//sprintf(buff, "%s_params.txt", name);
			//FILE *fd = fopen(buff, "r");
			FILE *fd = fopen( pFilename, "r" );
			if (fd == 0) { perror("cannot save parameters  file:"); return nullptr; }
			fscanf(fd, "%s\n", buff);
			//printf("load data for %s : %s\n", name, buff);
			printf( "load data for %s : %s\n", pFilename, buff );
			//fgets(buff, 100, fd); str = buff; while (*str != '=' && *str!=0) str++;
			//sscanf(str+1, "%d\n", &RESOL);
			//fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			//sscanf(str+1, "%f\n", &alpha);
			//fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			//sscanf(str+1, "%f\n", &rescalex);
			int type = 0;
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 't' || buff[1] != 'y') printf("incoherent file: %s", buff);
			sscanf(str+1, "%d\n", &type);
			if (type <= 2) { tt = PtPPTBF::REGULAR; decalx = 1 + type; Nx = 1; }
			else if (type ==3) { tt = PtPPTBF::IRREGULAR; decalx = 1 ; Nx = 1; }
			else if (type==4) { tt = PtPPTBF::IRREGULARX; decalx = 1; Nx = 1; }
			else if (type == 5) { tt = PtPPTBF::IRREGULARY; decalx = 1; Nx = 1; }
			else if (type <=8) { tt = PtPPTBF::CROSS; decalx = 1; Nx = type-6+2; }
			else if (type == 9) { tt = PtPPTBF::BISQUARE; decalx = 1; Nx = 1; }
			else { tt = PtPPTBF::IRREGULAR; decalx = 1; Nx = 1; }
			//fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			//sscanf(str+1, "%d\n", &decalx);
			//fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			//sscanf(str+1, "%d\n", &Nx);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'l' || buff[1] != 'a') printf("incoherent file: %s", buff);
			sscanf(str+1, "%f\n", &larp);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'i' || buff[1] != 'n') printf("incoherent file: %s", buff);
			sscanf(str+1, "%f\n", &inorm );
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'i' || buff[1] != 'n') printf("incoherent file: %s", buff);
			sscanf(str+1, "%f", &inorm2);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'n' || buff[1] != 'r') printf("incoherent file: %s", buff);
			sscanf(str+1, "%d\n", &nrelax);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'p' || buff[1] != 's') printf("incoherent file: %s", buff);
			sscanf(str+1, "%f\n", &psubdiv);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigwcellfact);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigwgaussfact);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &subdivscale);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &ppointsub);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &jitter);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'w' || buff[1] != 'i') printf("incoherent file: %s", buff);
			sscanf(str+1, "%f\n", &windowblend);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigwcell);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigwgauss);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &pointval);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &varpointval);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigwgaussvar);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			if (buff[0] != 'b' || buff[1] != 'o') printf("incoherent file: %s", buff);
			if (str[2] != '0' && str[2] != '1')
			{
				//hvFatal("incoherent file");
				printf( "incoherent file" );
			}
			bomb = (str[2] == '1');
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%d\n", &Npmin);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%d\n", &Npmax);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigcos);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &sigcosvar);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%d\n", &freq);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &phase);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &thickness);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &courbure);
			fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
			sscanf(str+1, "%f\n", &deltaorient);		
			for (int k = 0; k < 3; k++)
			{
				fgets(buff, 100, fd); str = buff; while (*str != '=' && *str != 0) str++;
				sscanf(str+1, "%g", &ampli[k]);
			}
			fclose(fd);
			fd = stdout;
			//fprintf(fd, "resol = %d\n", RESOL);
			//fprintf(fd, "alpha = %g\n", alpha);
			//fprintf(fd, "rescalex = %g\n", rescalex);
			fprintf(fd, "type = %d\n", (int)tt);
			fprintf(fd, "decalx = %d\n", decalx);
			fprintf(fd, "Nx = %d\n", Nx);
			fprintf(fd, "larp = %g\n", larp);
			fprintf(fd, "inormc = %g\n", inorm);
			fprintf(fd, "inormg = %g\n", inorm2);
			fprintf(fd, "nrelax = %d\n", nrelax);
			fprintf(fd, "psubdiv = %g\n", psubdiv);
			fprintf(fd, "sigwcellfact = %g\n", sigwcellfact);
			fprintf(fd, "sigwgaussfact = %g\n", sigwgaussfact);
			fprintf(fd, "subdivscale = %g\n", subdivscale);
			fprintf(fd, "ppointsub = %g\n", ppointsub);
			fprintf(fd, "jitter = %g\n", jitter);
			fprintf(fd, "windowblend = %g\n", windowblend);
			fprintf(fd, "sigwcell = %g\n", sigwcell);
			fprintf(fd, "sigwgauss = %g\n", sigwgauss);
			fprintf(fd, "pointval = %g\n", pointval);
			fprintf(fd, "varpointval = %g\n", 1.0 - pointval);
			fprintf(fd, "sigwgaussvar = %g\n", sigwgaussvar);
			fprintf(fd, "Npmin = %d\n", Npmin);
			fprintf(fd, "Npmax = %d\n", Npmax);
			fprintf(fd, "sigcos = %g\n", sigcos);
			fprintf(fd, "sigcosvar = %g\n", sigcosvar);
			fprintf(fd, "freq = %d\n", freq);
			fprintf(fd, "phase = %g\n", phase);
			fprintf(fd, "thickness = %g\n", thickness);
			fprintf(fd, "courbure = %g\n", courbure);
			fprintf(fd, "deltaorient = %g\n", deltaorient);
			for (int k = 0; k < 3; k++) fprintf(fd, "ampli[%d]=%g\n", k, ampli[k]);
			float norm = PtPPTBF::getNorm(inorm);// < 0.5 ? 1.0f + inorm / 0.5f : 2.0f + pow((inorm - 0.5f) / 0.5f, 0.5f)*50.0f;
			float norm2 = PtPPTBF::getNorm(inorm2);// < 0.5 ? 1.0f + inorm2 / 0.5f : 2.0f + pow((inorm2 - 0.5f) / 0.5f, 0.5f)*50.0f;
			//const int HISTOS = 50;
			//int precision = 40;
			//hvPict<float> ** pval;
			//hvPictureMetrics::evalneighbor_pptbf_cell(pval, precision, RESOL, 1,
			//	tt, ppointsub,
			//	decalx, Nx, nrelax, jitter,
			//	norm, windowblend, sigwcell, norm2, sigwgauss, sigwgaussvar,
			//	larp, pointval, varpointval,
			//	psubdiv, sigwcellfact, sigwgaussfact, subdivscale,
			//	bomb, sigcos, sigcosvar,
			//	Npmin, Npmax, freq, phase, thickness, courbure, deltaorient, ampli);
			//float histosample[HISTOS];
			//hvPictureMetrics::histogramm(pval, precision, histosample, HISTOS, 0.0, 1.5f);
			//thresh = hvPictureMetrics::computeThresh(1.0f - origpercent, histosample, HISTOS, 0.0f, 1.5f);
			////thresh = hvPictureMetrics::computeThresh(1.0f - percent, histosample, HISTOS, 0.0f, 1.5f);			thresh = hvPictureMetrics::computeThresh(1.0f - featpercent, histosample, HISTOS, 0.0f, 1.5f);
			//printf("final thresh=%g\n", thresh);
			//for (int i = 0; i < precision*precision; i++) delete pval[i];
			
			/*char *name;*/ //float origpercent;
			//int RESOL; float alpha; float rescalex;
			//float thresh;
			// point set parameters
			//int decalx; int Nx;
			/*float inorm;*/ /*float inorm2;*/
			//float pointval; float varpointval;
			//float sigwcellfact; float sigwgaussfact; // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//float ampli[3];

			// Point process
			PtPPTBF::tilingtype pTilingType = tt;
			pJittering = jitter;
			pCellSubdivisionProbability = ppointsub;
			pNbRelaxationIterations = nrelax;
			// Window function
			pCellularToGaussianWindowBlend = windowblend;
			pCellularWindowNorm = norm;
			pRectangularToVoronoiShapeBlend = larp;
			pCellularWindowDecay = sigwcell;
			pGaussianWindowDecay = sigwgauss; // ????????
			pGaussianWindowDecayJittering = sigwgaussvar; // ????????
			// Feature function
			pMinNbGaborKernels = Npmin;
			pMaxNbGaborKernels = Npmax;
			pFeatureNorm = norm2;
			pGaborStripesFrequency = freq;
			pGaborStripesCurvature = courbure;
			pGaborStripesOrientation = deltaorient;
			pGaborStripesThickness = thickness;
			pGaborDecay = sigcos;
			pGaborDecayJittering = sigcosvar;
			pFeaturePhaseShift = phase;
			pBombingFlag = bomb;
			// Deformation
			turbulenceA0 = ampli[ 0 ];
			turbulenceA1 = ampli[ 1 ];
			turbulenceA2 = ampli[ 2 ];
			// Others
			pRecursiveWindowSubdivisionProbability = psubdiv;
			pRecursiveWindowSubdivisionScale = subdivscale;
			// Debug
			pShowWindow = true;
			pShowFeature = true;

	return pptbf;
}


/******************************************************************************
 * Write file
 ******************************************************************************/
void PtPPTBFLoader::write( const PtPPTBF* pDataModel, const char* pFilename)
{
#if 0
	char buff[100];
	//sprintf(buff, "%s_params.txt", name);
	FILE *fd = fopen(buff, "w");
	if (fd == 0) { perror("cannot save parameters  file:"); return; }
	//fprintf(fd, "%s\n", name);

	//fprintf(fd, "resol = %d\n", RESOL);
	//fprintf(fd, "alpha = %g\n", alpha);
	//fprintf(fd, "rescalex = %g\n", rescalex);
	int type = 0;
	switch (pDataModel->get) {
	case hvNoise::REGULAR: type = decalx - 1; break;
	case hvNoise::IRREGULAR: type = 3; break;
	case hvNoise::IRREGULARX: type = 4; break;
	case hvNoise::IRREGULARY: type = 5; break;
	case hvNoise::CROSS: type = 6 + Nx - 2; break;
	case hvNoise::BISQUARE: type = 9; break;
	}
	fprintf(fd, "type = %d\n", type);
	//fprintf(fd, "decalx = %d\n", decalx);
	//fprintf(fd, "Nx = %d\n", Nx);
	fprintf(fd, "larp = %g\n", larp);
	fprintf(fd, "inormc = %g\n", inorm);
	fprintf(fd, "inormg = %g\n", inorm2);
	fprintf(fd, "nrelax = %d\n", nrelax);
	fprintf(fd, "psubdiv = %g\n", psubdiv);
	fprintf(fd, "sigwcellfact = %g\n", sigwcellfact);
	fprintf(fd, "sigwgaussfact = %g\n", sigwgaussfact);
	fprintf(fd, "subdivscale = %g\n", subdivscale);
	fprintf(fd, "ppointsub = %g\n", ppointsub);
	fprintf(fd, "jitter = %g\n", jitter);
	fprintf(fd, "windowblend = %g\n", windowblend);
	fprintf(fd, "sigwcell = %g\n", sigwcell);
	fprintf(fd, "sigwgauss = %g\n", sigwgauss);
	fprintf(fd, "pointval = %g\n", pointval);
	fprintf(fd, "varpointval = %g\n", 1.0 - pointval);
	fprintf(fd, "sigwgaussvar = %g\n", sigwgaussvar);
	fprintf(fd, "bomb = %d\n", bomb ? 1 : 0);
	fprintf(fd, "Npmin = %d\n", Npmin);
	fprintf(fd, "Npmax = %d\n", Npmax);
	fprintf(fd, "sigcos = %g\n", sigcos);
	fprintf(fd, "sigcosvar = %g\n", sigcosvar);
	fprintf(fd, "freq = %d\n", freq);
	fprintf(fd, "phase = %g\n", phase);
	fprintf(fd, "thickness = %g\n", thickness);
	fprintf(fd, "courbure = %g\n", courbure);
	fprintf(fd, "deltaorient = %g\n", deltaorient);
	for (int k = 0; k < 3; k++) fprintf(fd, "ampli[%d] = %g\n", k, ampli[k]);

	fclose(fd);
#endif
}
