/*
 * ...
 */

 /**
  * @version 1.0
  */

#ifndef _HV_SYNTHESIZER_
#define _HV_SYNTHESIZER_

  /******************************************************************************
   ******************************* INCLUDE SECTION ******************************
   ******************************************************************************/

// System
#include <cstdio>

// STL
#include <numeric>
#include <string.h>
#include <chrono>
#include <iostream>

namespace hview {


	/******************************************************************************
	 * parallel controllable texture synthesis with distance map and guidance
	 ******************************************************************************/
	void semiProcTexwdistguidanceV2(
		char *name,
		int STOPATLEVEL,
		int posx, int posy,
		const hvPictRGB<T> &example, const hvPictRGB<T> &exdist,
		double weight, // weight color vs distance
		double powr, float indweight, int neighbor, int atlevel, int bsize, float ERR,
		const hvBitmap &mask, const hvPictRGB<T> &guidance,
		hvArray2<hvVec2<int> > &index )
	{
		// - timer
		auto startTime = std::chrono::high_resolution_clock::now();
		
		//--------------------------------------
		// Preprocessing stage
		//--------------------------------------

		std::cout << "\n[PREPROCESSING stage]\n" << std::endl;

		std::cout << "- build multiscale pyramid" << std::endl;

		char buff[256];
		int i, j, ii, jj;
		FILE *fd;
		
		hvPictRGB<T> pyr[10], synth[10], pyrdist[10], synthdist[10], guid[10], dist;
		hvPict<unsigned char> pyrlabels[10], labels[10], synthlabels[10], exlabels;
		hvBitmap gmask[10];
		
		if (atlevel < 1) atlevel = 1;
		int s = 0, factor = 1;
		
		exlabels.reset(exdist.sizeX(), exdist.sizeY(), 0);
		dist.reset(exdist.sizeX(), exdist.sizeY(), 0);
		for (ii = 0; ii < dist.sizeX(); ii++) for (jj = 0; jj < dist.sizeY(); jj++)
		{
			hvColRGB<unsigned char> col = exdist.get(ii, jj);
			dist.update(ii, jj, hvColRGB<unsigned char>(col.RED(), col.GREEN(), 0));
			exlabels.update(ii, jj, col.BLUE());
		}
		example.makepyr( name, dist, exlabels, s, factor, pyr, pyrdist, pyrlabels, atlevel );
		
		guid[0].reset(guidance.sizeX(), guidance.sizeY(), hvColRGB<unsigned char>(0));
		labels[0].reset(guidance.sizeX(), guidance.sizeY(), 0);
		for (ii = 0; ii < guid[0].sizeX(); ii++) for (jj = 0; jj < guid[0].sizeY(); jj++)
		{
			hvColRGB<unsigned char> col = guidance.get(ii, jj);
			guid[0].update(ii, jj, hvColRGB<unsigned char>(col.RED(), col.GREEN(), 0));
			labels[0].update(ii, jj, col.BLUE());
		}
		gmask[0] = mask;
		for (i = 1; i <= s; i++)
		{
			guid[i].reset(guid[i - 1].sizeX() / 2, guid[i - 1].sizeY() / 2, hvColRGB<unsigned char>(0));
			labels[i].reset(labels[i - 1].sizeX() / 2, labels[i - 1].sizeY() / 2, 0);
			for (ii = 0; ii < guid[i].sizeX(); ii++) for (jj = 0; jj < guid[i].sizeY(); jj++)
			{
				hvColRGB<unsigned char> col[4];
				col[0] = guid[i - 1].get(ii * 2, jj * 2);
				col[1] = guid[i - 1].get(ii * 2 + 1, jj * 2);
				col[2] = guid[i - 1].get(ii * 2, jj * 2 + 1);
				col[3] = guid[i - 1].get(ii * 2 + 1, jj * 2 + 1);
				unsigned char rr = (unsigned char)(((float)col[0].RED() + (float)col[1].RED() + (float)col[2].RED() + (float)col[3].RED()) / 4.0);
				unsigned char gg = (unsigned char)(((float)col[0].GREEN() + (float)col[1].GREEN() + (float)col[2].GREEN() + (float)col[3].GREEN()) / 4.0);
				guid[i].update(ii, jj, hvColRGB<unsigned char>(rr, gg, 0));
				labels[i].update(ii, jj, labels[i - 1].get(ii * 2, jj * 2));
			}
			//guid[i].shrink(&(guid[i - 1]), 0);
			gmask[i].shrink(gmask[i - 1]);

#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
			sprintf(buff, "%s_semiProcTex_%02d_guidance.ppm", name, i);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			guid[i].savePPM(fd, 1);
			fclose(fd);

			hvPictRGB<unsigned char> labelsrgb(labels[i], 1);
			sprintf(buff, "%s_semiProcTex_%02d_guidancelabels.ppm", name, i);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			labelsrgb.savePPM(fd, 1);
			fclose(fd);
#endif
		}
		index.reset(guidance.sizeX() / factor, guidance.sizeY() / factor, hvVec2<int>());
		synth[s].reset(guidance.sizeX() / factor , guidance.sizeY() / factor , hvColRGB<T>());
		synthdist[s].reset(guidance.sizeX() / factor , guidance.sizeY() / factor , hvColRGB<T>());
		synthlabels[s].reset(guidance.sizeX() / factor , guidance.sizeY() / factor , 0);
		
		// - timer
		auto endTime = std::chrono::high_resolution_clock::now();
		float elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
		std::cout << "- time: " << elapsedTime << " ms\n";

		//------------------------------------------------
		// SMART initialization
		//------------------------------------------------

		startTime = std::chrono::high_resolution_clock::now();

		std::cout << "\n---------------------------------------------------------------------------------------" << std::endl;

		std::cout << "\n[SMART INITIALIZATION stage]" << std::endl;

		printf("\n- smart blocs init...\n");
		printf("- blocs init: %d,%d blocs (at lev %d, bsize=%d)...\n", index.sizeX() / (bsize), index.sizeY() / (bsize), s, bsize);
		if ( guidance.sizeX() % (factor*bsize) != 0 || guidance.sizeY() % (factor*bsize) != 0 )
		{
			hvFatal( "guidance size must be divisible by (factor*bsize)" );
		}
		// initialization
		this->smartinitV2(posx/factor-bsize, posy/factor-bsize, ERR, indweight, bsize, 
			guid[s], labels[s], gmask[s], pyr[s], pyrdist[s], pyrlabels[s],
			index, synth[s], synthdist[s], synthlabels[s]);

		// - timer
		endTime = std::chrono::high_resolution_clock::now();
		elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
		std::cout << "- time: " << elapsedTime << " ms\n";

		hvPictRGB<unsigned char> pinit(index.sizeX()*factor, index.sizeY()*factor, hvColRGB<unsigned char>(0));
		hvPictRGB<unsigned char> finit(index.sizeX()*factor, index.sizeY()*factor, hvColRGB<unsigned char>(0));
		hvPictRGB<unsigned char> labinit(index.sizeX()*factor, index.sizeY()*factor, hvColRGB<unsigned char>(0));
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			hvVec2<int> pos = index.get(i, j);
			for (int ii = 0; ii < factor; ii++) for (int jj = 0; jj < factor; jj++)
			{
				pinit.update(i*factor + ii, j*factor + jj, example.get(pos.X()*factor + ii, pos.Y()*factor + jj));
				finit.update(i*factor + ii, j*factor + jj, dist.get(pos.X()*factor + ii, pos.Y()*factor + jj));
				labinit.update(i*factor + ii, j*factor + jj, hvColRGB<unsigned char>(exlabels.get(pos.X()*factor + ii, pos.Y()*factor + jj)));
			}
		}

#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
		sprintf(buff, "%s_semiProcTex_%02d_blocinit.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		pinit.savePPM(fd, 1);
		fclose(fd);

		sprintf(buff, "%s_semiProcTex_%02d_distinit.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		finit.savePPM(fd, 1);
		fclose(fd);

		sprintf(buff, "%s_semiProcTex_%02d_labinit.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		labinit.savePPM(fd, 1);
		fclose(fd);
#endif

		//synth[s].fastchaosblocs(pyr[s], 10, index);
		//printf("%s_semiProcTex_%02d_init\n", name, s);
#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
		sprintf(buff, "%s_semiProcTex_%02d_init.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synth[s].savePPM(fd, 1);
		fclose(fd);

		sprintf(buff, "%s_semiProcTex_%02d_dist.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synthdist[s].savePPM(fd, 1);
		fclose(fd);

		hvPictRGB<unsigned char> synthlabrgb(synthlabels[s], 1);
		sprintf(buff, "%s_semiProcTex_%02d_distlabel.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synthlabrgb.savePPM(fd, 1);
		fclose(fd);
#endif

		//-------------------------------------------------
		// SYNTHESIS stage
		//-------------------------------------------------

		startTime = std::chrono::high_resolution_clock::now();

		std::cout << "\n---------------------------------------------------------------------------------------" << std::endl;

		std::cout << "\n[SYNTHESIS stage]\n" << std::endl;

		const int niter = 2;
		//synthdist[s].imagefromindex(pyrdist[s], index);
		printf("starting semiProcTex at level:%d, shift:%d,%d\n", s, posx / factor - bsize/factor, posy / factor - bsize/factor);
		this->semiProcTexiterwdistguidanceV2(
					name,
					STOPATLEVEL,
					posx / factor - bsize/factor, posy / factor - bsize/factor,
					pyr, pyrdist, pyrlabels, // exemplar maps
					synth, synthdist, synthlabels, // synthesized maps
					guid, labels, gmask, // guidance maps
					index, // index map (synthesized)
					weight, powr,
					s, factor,
					neighbor,
					niter,
					indweight );

		// - timer
		endTime = std::chrono::high_resolution_clock::now();
		elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
		std::cout << "\nTOTAL time: " << elapsedTime << " ms\n";
	}

	
#endif // !efined(AFX_PICTRGB_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
