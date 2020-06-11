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
#include <cstring>

// STL
#include <numeric>
#include <chrono>
#include <iostream>

// Project
#include "hvLinearTransform3.h"
#include "hvBitmap.h"
#include "hvField2.h"
#include "hvColor.h"
#include "hvPict.h"
#include "MyThreadPool.h"
#include "hvNoise.h"

/**
 * MACRO
 */
#define USE_MULTITHREADED_SYNTHESIS
//#define USE_NO_TEMPORARY_IMAGE_EXPORT

/**
 * MACRO used to remove guidance constraints at 2nd correction step
 * => this forces to reinject exemplar data similarity into optimization
 */
#define USE_NO_GUIDANCE_RELAXATION




namespace hview {

	// from Ward's RGBE images
const int MAX_SYNTHESIS_PIXEL_LIST = 5;
typedef struct {
  int valid;            /* indicate which fields are valid */
  char programtype[16]; /* listed at beginning of file to identify it 
                         * after "#?".  defaults to "RGBE" */ 
  float gamma;          /* image has already been gamma corrected with 
                         * given gamma.  defaults to 1.0 (no correction) */
  float exposure;       /* a value of 1.0 in an image corresponds to
			 * <exposure> watts/steradian/m^2. 
			 * defaults to 1.0 */
} rgbe_header_info;

/* flags indicating which fields in an rgbe_header_info are valid */
#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

template <class T> class hvPictRGBA;
template <class R, unsigned int n> class hvQPictRGB;

////////////////////////////////////////////////////////////
template <class T> class hvPictRGB : public hvField2< hvColRGB<T> >  
////////////////////////////////////////////////////////////
{
public:
	hvPictRGB<T>() : hvField2< hvColRGB<T> >(),hvArray2< hvColRGB<T> >() { }
	hvPictRGB<T>(int sx, int sy, const hvColRGB<T> &nil) : hvField2< hvColRGB<T> >(sx, sy, nil),hvArray2< hvColRGB<T> >(sx, sy, nil) { }
	void reset(int sx, int sy,const hvColRGB<T> &nil)
	{
		hvField2< hvColRGB<T> >::reset(sx,sy, nil);
	}
	void reset()
	{
		hvField2< hvColRGB<T> >::reset();
	}
	void clone(const hvPictRGB<T> &pict,int x, int y, int sx, int sy)
	{
		hvField2< hvColRGB<T> >::reset(sx-x+1, sy-y+1, hvColRGB<T>(0));
		int i,j;
		for (i=x; i<=sx; i++) for (j=y; j<=sy; j++)
		{
			this->update(i-x,j-y,pict.get(i,j));
		}
	}
	template <class U> void clone(const hvPictRGB<U> &pict, U scalu, U gamma, U scalt, int x, int y, int sx, int sy)
	{
		hvField2< hvColRGB<T> >::reset(sx-x+1, sy-y+1, hvColRGB<T>(0));
		int i,j;
		for (i=x; i<= sx; i++) for (j=y; j<= sy; j++)
		{
			hvColRGB<U> col = pict.get(i,j);
			col.scale(1.0/scalu);
			col.gamma(1.0, gamma);
			this->update(i-x,j-y,hvColRGB<T>( (T)(scalt*col.RED()),(T)(scalt*col.GREEN()),(T)(scalt*col.BLUE()) ));
		}
	}
	hvPictRGB<T>(const hvBitmap &pict, const hvColRGB<T> &va, const hvColRGB<T> &vb): hvField2< hvColRGB<T> >(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0)), hvArray2< hvColRGB<T> >(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0))
	{
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			if (pict.get(i,j)) this->update(i,j,va); else this->update(i,j,vb);
		}
	}
	void convert(const hvBitmap &pict, const hvColRGB<T> &va, const hvColRGB<T> &vb)
	{
		hvField2< hvColRGB<T> >::reset(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0));
		//hvArray2< hvColRGB<T> >::reset(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0));
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			if (pict.get(i,j)) this->update(i,j,va); else this->update(i,j,vb);
		}
	}
	hvPictRGB<T>(const hvPictRGBA<T> &pict, hvPictComponent cc): hvField2< hvColRGB<T> >(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0)), hvArray2< hvColRGB<T> >(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0))
	{
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			hvColRGB<T> v;
			hvColRGBA<T> co = pict.get(i,j);
			switch(cc) 
			{
				case HV_RED: v=hvColRGB<T>(co.RED()); break;
				case HV_GREEN: v=hvColRGB<T>(co.GREEN()); break;
				case HV_BLUE: v=hvColRGB<T>(co.BLUE()); break;
				case HV_ALPHA: v=hvColRGB<T>(co.ALPHA()); break;
				case HV_RGB: v=hvColRGB<T>(co); break;
				default: v = hvColRGB<T>(co.luminance());
			}
			this->update(i,j,v); 
		}
	}
	hvPictRGB<T>(const hvPictRGB<T> &pict, hvPictComponent cc): hvField2< hvColRGB<T> >(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0)), hvArray2< hvColRGB<T> >(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0))
	{
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			hvColRGB<T> v;
			hvColRGB<T> co = pict.get(i,j);
			switch(cc) 
			{
				case HV_RED: v=hvColRGB<T>(co.RED()); break;
				case HV_GREEN: v=hvColRGB<T>(co.GREEN()); break;
				case HV_BLUE: v=hvColRGB<T>(co.BLUE()); break;
				case HV_RGB: v=hvColRGB<T>(co); break;
				default: v = hvColRGB<T>(co.luminance());
			}
			this->update(i,j,v); 
		}
	}
	template <class U, class V> hvPictRGB(const hvPict<U> &p, V scal);
	template <class U, class V> hvPictRGB(const hvPict<U> &p, V scal, V shift);
	template <class U, class V> hvPictRGB(const hvPict<U> &p, V scal, int x, int y, int sx, int sy);
	template <class U, class V> void convert(const hvPict<U> &p, V scal, int x, int y, int sx, int sy);
	template <class U, class V> void convertloga(const hvPict<U> &p, V loga, V max, V scal, int x, int y, int sx, int sy);
	template <class U, class V> hvPictRGB(const hvPict<U> &p, V scal, V min, V max);
	template <class U> hvPictRGB(const hvPict<U> &p, const std::vector<hvColRGB<unsigned char> > &lcol);


	// choose RGB components from two input
	void merge(const hvPictRGB<T> &pa, const hvPictRGB<T> &pb, bool rr, bool gg, bool bb)
	{
		int i,j;
		hvField2< hvColRGB<T> >::reset(pa.sizeX(), pa.sizeY(), hvColRGB<T>(0));
		for (i=0; i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> ca = pa.get(i,j);
			hvColRGB<T> cb = pb.get(i,j);
			hvColRGB<T> v=hvColRGB<T>(rr?ca.RED():cb.RED(), gg?ca.GREEN():cb.GREEN(), bb?ca.BLUE():cb.BLUE()); 
			this->update(i,j,v);
		}
	}

	// keep only some RGB components
	void extract(bool rr, bool gg, bool bb, T cr, T cg, T cb)
	{
		int i,j;
		for (i=0; i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> co = this->get(i,j);
			hvColRGB<T> v=hvColRGB<T>(rr?co.RED():cr,gg?co.GREEN():cg,bb?co.BLUE():cb); 
			this->update(i,j,v);
		}
	}
	// Convert into HSV color space
	void toHSV(const hvPictRGB<T> &p, T scal, hvColRGB<double> weights=hvColRGB<double>(1.0,1.0,1.0))
	{
		int i,j;
		hvField2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i=0; i<p.sizeX(); i++) for (j=0; j<p.sizeY(); j++)
		{
			hvColRGB<T> val = p.get(i,j);
			hvColRGB<T> valhsv; valhsv.tohsv(val, scal);
			this->update(i,j,hvColRGB<T>(T((double)valhsv.RED()*weights.RED()), T((double)valhsv.GREEN()*weights.GREEN()), T((double)valhsv.BLUE()*weights.BLUE())));
		}
	}
	// Convert into XYZ color space
	void toXYZ(const hvPictRGB<T> &p, T scal, hvColRGB<double> weights=hvColRGB<double>(1.0,1.0,1.0))
	{
		int i,j;
		hvField2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i=0; i<p.sizeX(); i++) for (j=0; j<p.sizeY(); j++)
		{
			hvColRGB<T> val = p.get(i,j);
			hvColRGB<T> valhsv; valhsv.toxyz(val, scal);
			this->update(i,j,hvColRGB<T>(T((double)valhsv.RED()*weights.RED()), T((double)valhsv.GREEN()*weights.GREEN()), T((double)valhsv.BLUE()*weights.BLUE())));
		}
	}
	// Convert into LUV color space
	void toLUV(const hvPictRGB<T> &p, T scal, hvColRGB<double> weights=hvColRGB<double>(1.0,1.0,1.0))
	{
		int i,j;
		hvField2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i=0; i<p.sizeX(); i++) for (j=0; j<p.sizeY(); j++)
		{
			hvColRGB<T> val = p.get(i,j);
			hvColRGB<T> valhsv; valhsv.toLuv(val, scal);
			this->update(i,j,hvColRGB<T>(T((double)valhsv.RED()*weights.RED()), T((double)valhsv.GREEN()*weights.GREEN()), T((double)valhsv.BLUE()*weights.BLUE())));
		}
	}
	// Convert from XYZ color space back to RGB
	void fromXYZ(const hvPictRGB<T> &p, T scal, hvColRGB<double> weights=hvColRGB<double>(1.0,1.0,1.0))
	{
		int i,j;
		hvField2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i=0; i<p.sizeX(); i++) for (j=0; j<p.sizeY(); j++)
		{
			hvColRGB<T> val = p.get(i,j);
			hvColRGB<T> valhsv; valhsv.fromxyz(val, scal);
			this->update(i,j,hvColRGB<T>(T((double)valhsv.RED()*weights.RED()), T((double)valhsv.GREEN()*weights.GREEN()), T((double)valhsv.BLUE()*weights.BLUE())));
		}
	}
	// Convert from LUV color space back to RGB
	void fromLUV(const hvPictRGB<T> &p, T scal, hvColRGB<double> weights=hvColRGB<double>(1.0,1.0,1.0))
	{
		int i,j;
		hvField2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i=0; i<p.sizeX(); i++) for (j=0; j<p.sizeY(); j++)
		{
			hvColRGB<T> val = p.get(i,j);
			hvColRGB<T> valhsv; valhsv.fromLuv(val, scal);
			this->update(i,j,hvColRGB<T>(T((double)valhsv.RED()*weights.RED()), T((double)valhsv.GREEN()*weights.GREEN()), T((double)valhsv.BLUE()*weights.BLUE())));
		}
	}
	// difference between two images of same resolution
	void difference(const hvPictRGB<T> &pia,const hvPictRGB<T> &pib)
	{
		hvField2< hvColRGB<T> >::reset(pia.sizeX(), pia.sizeY(), hvColRGB<T>(0));
		int i,j;
		for (i=0; i<pia.sizeX(); i++) for (j=0; j<pia.sizeY(); j++)
		{
			hvColRGB<T> cola = pia.get(i,j);
			hvColRGB<T> colb = pib.get(i,j);
			hvColRGB<T> cold; cold.difference(cola,colb);
			this->update(i,j,cold);
		}
	}
	// difference between two images of same resolution
	void difference(const hvPictRGB<T> &pia,const hvPictRGB<T> &pib, double scale, double offset)
	{
		hvField2< hvColRGB<T> >::reset(pia.sizeX(), pia.sizeY(), hvColRGB<T>(0));
		int i,j;
		for (i=0; i<pia.sizeX(); i++) for (j=0; j<pia.sizeY(); j++)
		{
			hvColRGB<T> cola = pia.get(i,j);
			hvColRGB<T> colb = pib.get(i,j);
			hvColRGB<T> cold; cold.difference(cola,colb, scale, offset);
			this->update(i,j,cold);
		}
	}
	// difference between two images of same resolution
	void differenceMask(double sqdthresh, const hvPictRGB<T> &pi, hvBitmap &diff)
	{
		diff.reset(pi.sizeX(), pi.sizeY(), true);
		int i,j;
		for (i=0; i<pi.sizeX(); i++) for (j=0; j<pi.sizeY(); j++)
		{
			hvColRGB<T> cola = pi.get(i,j);
			hvColRGB<T> colb = this->get(i,j);
			diff.set(i,j,(cola.squaredDifference(colb)>sqdthresh));
		}
	}

	void squaredDifference(int px, int py, int dx, int dy, const hvPictRGB<unsigned char> &pia, int ix, int iy, const hvPictRGB<unsigned char> &pib);

	double meansquareDifference(const hvPictRGB<T> &pi, hvBitmap &mask)
	{
		int i,j;
		int count=0;
		double sum=0.0;
		for (i=0; i<pi.sizeX(); i++) for (j=0; j<pi.sizeY(); j++)
		{
			if (mask.get(i,j))
			{
			hvColRGB<T> cola = pi.get(i,j);
			hvColRGB<T> colb = this->get(i,j);
			sum+=cola.squaredDifference(colb);
			count++;
			}
		}
		return sum/(double)count;
	}

	double meansquareDifference(int u, int v, const hvPictRGB<T> &pi, int x, int y, int sx, int sy)
	{
		int i,j;
		int count=0;
		double sum=0.0;
		for (i=-sx; i<=sx; i++) for (j=-sy; j<=sy; j++)
		{
			int xx = x+i; if (xx<0) xx+= pi.sizeX(); else if (xx>=pi.sizeX()) xx-=pi.sizeX();
			int yy = y+j; if (yy<0) yy+= pi.sizeY(); else if (yy>=pi.sizeY()) yy-=pi.sizeY();
			int uu = u+i; if (uu<0) uu+= this->sizeX(); else if (uu>=this->sizeX()) uu-=this->sizeX();
			int vv = v+j; if (vv<0) vv+= this->sizeY(); else if (vv>=this->sizeY()) vv-=this->sizeY();
			hvColRGB<T> cola = pi.get(xx,yy);
			hvColRGB<T> colb = this->get(uu,vv);
			sum+=cola.squaredDifference(colb)/3.0;
			count++;
		}
		return sum/(double)count;
	}

	// applies a blurring mask of size sx,sy
	// scal is the normalization factor, usually sx*sy
	void blur(T scal, int sx, int sy)
	{
		hvField2<hvColRGB<T> > source; source.clone(this);
		hvField2<hvColRGB<T> >:: template blur<hvColRGB<double> >(&source, sx, sy, hvColRGB<double>(1.0/(double)scal));
	}
	void gaussianBlur(int sx, int sy)
	{
		hvField2<hvColRGB<T> > source; source.clone(this);
		hvField2<hvColRGB<T> >:: template gaussianBlur<hvColRGB<double> >(&source, sx, sy);
	}
	// applies a deblurring mask of size sx,sy
	void deblur(int niter, int sx, int sy, double scal, double min, double max)
	{
		hvField2<hvColRGB<T> > source; source.clone(this);
		hvField2<hvColRGB<T> >:: template deblur<hvColRGB<double> >(&source, niter, sx, sy, scal, min, max);
	}
	// Difference of Gaussians
	void DoG(const hvPictRGB<T> &pia, int sx, int sy, int nrec=-1)
	{
		hvPictRGB<T> pi, pib;
		pi.clone(pia, 0, 0, pia.sizeX()-1, pia.sizeY()-1);
		if (nrec>=0) { pib.shrink(&pi,nrec); pi.clone(pib,0,0,pib.sizeX()-1,pib.sizeY()-1); }
		else pib.clone(pia, 0, 0, pia.sizeX()-1, pia.sizeY()-1);
		pib.gaussianBlur(sx,sy);
		this->difference(pi,pib);
	}
	// Difference of Gaussians
	void DoG(const hvPictRGB<T> &pia, int sx, int sy, double scale, double offset)
	{
		hvPictRGB<T> pib;
		pib.clone(pia, 0, 0, pia.sizeX()-1, pia.sizeY()-1);
		pib.gaussianBlur(sx,sy);
		this->difference(pia,pib, scale, offset);
	}
	// Difference of Gaussians
	void DoG(hvPict<T> &pires, int sx, int sy)
	{
		int i,j;
		hvPictRGB<T> pi;
		pi.clone(*this, 0, 0, this->sizeX()-1, this->sizeY()-1);
		pi.gaussianBlur(sx,sy);
		pires.reset(this->sizeX(), this->sizeY(),0);
		for (i=0; i<pi.sizeX(); i++) for (j=0; j<pi.sizeY(); j++)
		{
			pires.update(i,j,(T)(sqrt(pi.get(i,j).squaredDifference(this->get(i,j))/3.0)));
		}
	}

	void histogramm(double *histo, int NN, double norm, int x, int y, int fx, int fy, hvPictComponent comp)
	{
		int i,j;
		for (i=0; i<NN; i++) histo[i]=0.0;
		for (i=x; i<=fx; i++) for (j=y; j<=fy; j++)
		{
			hvColRGB<T> col = this->get(i,j);
			double v;
			switch(comp)
			{
				case HV_RED: v=col.RED(); break;
				case HV_GREEN: v=col.GREEN(); break;
				case HV_BLUE: v=col.BLUE(); break;
				default: v = col.luminance();
			}
			v /= norm;
			int bin = v*(double)NN; if (bin<0) bin=0; else if (bin>=NN-1) bin=NN-1;
			histo[bin]+=1.0;
		}
		for (i=0; i<NN; i++) histo[i]/=(double)((fx-x+1)*(fy-y+1));
	}

	// checks if R,G,B values are local max in mask size [-1,1] , returns value or 0 if not max
	hvColRGB<T> isLocalMaximum(const hvPictRGB<T> &dog2, int x, int y) const
	{
		hvColRGB<T> vxy = this->get(x,y);
		int i,j;
		bool rr=true, gg=true, bb=true;
		for (i=-1; i<=1; i++) for (j=-1; j<=1; j++) 
		{
			if (x+i>=0 && x+i<this->sizeX() && y+j>=0 && y+j<this->sizeY())
			{
				hvColRGB<T> v = this->get(x+i,y+j);
				if (vxy.RED()<v.RED()) rr=false;
				if (vxy.GREEN()<v.GREEN()) gg=false;
				if (vxy.BLUE()<v.BLUE()) bb=false;
				v = dog2.get((x+i)/2,(y+j)/2);
				if (vxy.RED()<v.RED()) rr=false;
				if (vxy.GREEN()<v.GREEN()) gg=false;
				if (vxy.BLUE()<v.BLUE()) bb=false;
			}
		}
		return hvColRGB<T>(rr?vxy.RED():T(0),gg?vxy.GREEN():T(0),bb?vxy.BLUE():T(0));
	}

	// computes local gradient using central differences along X and Y axes
	void gradient(int x, int y, T scale, hvColRGB<double> *dx, hvColRGB<double> *dy) const
	{
		if (x-1>=0 && x+1<this->sizeX())
		{
			hvColRGB<T> cola= this->get(x-1,y);
			hvColRGB<T> colb= this->get(x+1,y);
			*dx=hvColRGB<double>((double)colb.X()/(double)scale-(double)cola.X()/(double)scale,
				(double)colb.Y()/(double)scale-(double)cola.Y()/(double)scale,
				(double)colb.Z()/(double)scale-(double)cola.Z()/(double)scale);
		}
		if (y-1>=0 && y+1<this->sizeY())
		{
			hvColRGB<T> cola= this->get(x,y-1);
			hvColRGB<T> colb= this->get(x,y+1);
			*dy=hvColRGB<double>((double)colb.X()/(double)scale-(double)cola.X()/(double)scale,
				(double)colb.Y()/(double)scale-(double)cola.Y()/(double)scale,
				(double)colb.Z()/(double)scale-(double)cola.Z()/(double)scale);
		}
	}

	// devides size by factor of 2^(nrec+1), nrec=0 means factor 2, nrec=1 means factor 4, etc.
	void shrink(hvPictRGB<T> *p, int nrec=0)
	{
		if (nrec>0) { hvPictRGB<T> source; source.shrink(p,0); this->shrink(&source, nrec-1); }
		else 
		{
			this->reset(p->sizeX()/2, p->sizeY()/2, hvColRGB<T>());
			int ii,jj;
			for (ii=0; ii<this->sizeX(); ii++) for (jj=0; jj<this->sizeY(); jj++)
			{
				hvColRGB<T> cc[4];
				cc[0]=p->get(2*ii,2*jj); cc[1]=p->get(2*ii+1,2*jj); cc[2]=p->get(2*ii+1,2*jj+1); cc[3]=p->get(2*ii,2*jj+1);
				hvColRGB<T> col; col.mean(4,cc);
				this->update(ii,jj,col);
			}
		}
	}
	void shrinkx(hvPictRGB<T> *p, int nrec=0)
	{
		if (nrec>0) { hvPictRGB<T> source; source.shrinkx(p,0); this->shrinkx(&source, nrec-1); }
		else 
		{
			this->reset(p->sizeX()/2, p->sizeY(), hvColRGB<T>());
			int ii,jj;
			for (ii=0; ii<this->sizeX(); ii++) for (jj=0; jj<this->sizeY(); jj++)
			{
				hvColRGB<T> cc[2];
				cc[0]=p->get(2*ii,jj); cc[1]=p->get(2*ii+1,jj);
				hvColRGB<T> col; col.mean(2,cc);
				this->update(ii,jj,col);
			}
		}
	}
	void shrinky(hvPictRGB<T> *p, int nrec=0)
	{
		if (nrec>0) { hvPictRGB<T> source; source.shrinky(p,0); this->shrinky(&source, nrec-1); }
		else 
		{
			this->reset(p->sizeX(), p->sizeY()/2, hvColRGB<T>());
			int ii,jj;
			for (ii=0; ii<this->sizeX(); ii++) for (jj=0; jj<this->sizeY(); jj++)
			{
				hvColRGB<T> cc[2];
				cc[0]=p->get(ii,2*jj); cc[1]=p->get(ii,2*jj+1); 
				hvColRGB<T> col; col.mean(2,cc);
				this->update(ii,jj,col);
			}
		}
	}
	// multiplies size by factor of 2^(nrec+1), nrec=0 means factor 2, nrec=1 means factor 4, etc.
	void enlarge(hvPictRGB<T> *p, int nrec=0)
	{
		if (nrec>0) { hvPictRGB<T> source; source.hvField2<hvColRGB<T> >::enlarge(p); this->enlarge(&source, nrec-1); }
		else hvField2<hvColRGB<T> >::enlarge(p);
	}
	// applies a local operator mm on a mask of size [-size,+size]
	hvVec3<double> maskPixel(T scal, int size, int x, int y, hvPictMask mm) const
	{
		int ii,jj,i,j;
		hvVec3<double> res(0.0);
		for (ii=-size; ii<=size; ii++) for (jj=-size; jj<=size; jj++)
		{
			int px = x+ii; if (px<0) px=0; else if (px>= this->sizeX()) px= this->sizeX()-1;
			int py = y+jj; if (py<0) py=0; else if (py>= this->sizeY()) py= this->sizeY()-1;
			hvColRGB<T> v = this->get(px,py);
			hvVec3<double> vd((double)v.RED()/(double)scal,(double)v.GREEN()/(double)scal,(double)v.BLUE()/(double)scal);
			switch(mm)
			{
			case HV_EDGE_VERT: if (ii<0) res-=vd; else if (ii>0) res+=vd; break;
			case HV_EDGE_HORIZ: if (jj<0) res-=vd; else if (jj>0) res+=vd; break;
			case HV_EDGE_DIAG1: if (jj>ii) res-=vd; else if (jj<ii) res+=vd; break;
			case HV_EDGE_DIAG2: if (jj>-ii) res-=vd; else if (jj<-ii) res+=vd; break;
			case HV_DESCR_VERT: if (ii<0) res-=vd; else res+=vd; break;
			case HV_DESCR_HORIZ: if (jj<0) res-=vd; else res+=vd; break;
			case HV_DESCR_DIAG1: if ((ii<0 && jj<0)||(ii>=0 && jj>=0)) res-=vd; else res+=vd; break;
			case HV_DESCR_DIAG2: if ((ii<0 && jj>0)||(ii>=0 && jj<=0)) res-=vd; else res+=vd; break;
			//case HV_DESCR_DOTS: i=ii<0?-ii:ii; j=jj<0?-jj:jj; i/=2; j/=2; if ((i%2==0 && j%2!=0)||(i%2!=0 && j%2==0)) res-=vd; else res+=vd; break;
			case HV_DESCR_DOTS: i=ii<0?-ii:ii; j=jj<0?-jj:jj; if ((i+j)%2==0) res-=vd; else res+=vd; break;
			default: res+=vd; break;
			}
		}
		return res;
	}
	void mask(const hvPictRGB<T> &p, T scal, int size, hvPictMask mm, double norm)
	{
		this->reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvVec3<double> res=p.maskPixel(scal,size,i,j,mm);
			res.scale(1.0/norm);
			res.abs();
			this->update(i,j,hvColRGB<T>((T)(res.X()*(double)scal),(T)(res.Y()*(double)scal),(T)(res.Z()*(double)scal))); 
		}
	}
	void erase(const hvBitmap &mm, const hvColRGB<T> &vv)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			if (!mm.get(i,j)) this->update(i,j,vv);
		}
	}
	void edge(const hvPictRGB<T> &p, T scal, int size)
	{
		this->reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvVec3<double> vert=p.maskPixel(scal,size,i,j,HV_EDGE_VERT);
			hvVec3<double> horiz=p.maskPixel(scal,size,i,j,HV_EDGE_HORIZ);
			hvVec3<double> diag1=p.maskPixel(scal,size,i,j,HV_EDGE_DIAG1);
			hvVec3<double> diag2=p.maskPixel(scal,size,i,j,HV_EDGE_DIAG2);
			vert.scale(1.0/(double)(size*(size*2+1)));
			vert.abs();
			horiz.scale(1.0/(double)(size*(size*2+1)));
			horiz.abs();
			diag1.scale(1.0/(double)(size*(size*2+1)));
			diag1.abs();
			diag2.scale(1.0/(double)(size*(size*2+1)));
			diag2.abs();
			vert.keepMax(vert, horiz);
			vert.keepMax(vert, diag1);
			vert.keepMax(vert, diag2);
			this->update(i,j,hvColRGB<T>((T)(vert.X()*(double)scal),(T)(vert.Y()*(double)scal),(T)(vert.Z()*(double)scal)));
		}
	}
	void discont(const hvPictRGB<T> &p, const hvColRGB<T> c1, const hvColRGB<T> c2)
	{
		this->reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		int i,j,ii,jj;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> cc = p.get(i,j);
			bool eq=true;
			for (ii=-1; ii<=1;ii++) for (jj=-1; jj<=1; jj++)
			{
				int x = i+ii, y = j+jj;
				if (x<0) x+= this->sizeX(); x %= this->sizeX();
				if (y<0) y+= this->sizeY(); y %= this->sizeY();
				if (!cc.equals(p.get(x,y))) eq=false;
			}
			this->update(i,j,eq?c1:c2);
		}
	}

	hvColRGB<T> avg() const 
	{
		int i,j;
		double vr=0.0, vg=0.0, vb=0.0;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			vr+=(double)v.RED(); vg+=(double)v.GREEN(); vb+=(double)v.BLUE();
		}
		return hvColRGB<T>((T)(vr/(double)(this->sizeX()*this->sizeY())),(T)(vg/(double)(this->sizeX()*this->sizeY())),(T)(vb/(double)(this->sizeX()*this->sizeY())) ) ;
	}
	hvColRGB<T> avg(const hvBitmap &bm) const 
	{
		int i,j,n=0;
		double vr=0.0, vg=0.0, vb=0.0;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			if (bm.get(i,j))
			{
				hvColRGB<T> v = this->get(i,j);
				vr+=(double)v.RED(); vg+=(double)v.GREEN(); vb+=(double)v.BLUE();
				n++;
			}
		}
		return hvColRGB<T>((T)(vr/(double)(n)),(T)(vg/(double)(n)),(T)(vb/(double)(n)) ) ;
	}

	void gammaNormalizedMax(T scal, double power)
	{
		int i,j;
		hvColRGB<T> min,max;
		this->minmax(min,max);
		printf("Max %d,%d,%d\n", max.RED(), max.GREEN(), max.BLUE());
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			v.gammaNormalizedMax(max,scal, power);
			this->update(i,j,v);
		}
	}
	void gammaClampedMax(T scal, double thresh)
	{
		int i, j;
		hvColRGB<T> min, max;
		this->minmax(min, max);
		printf("Max %d,%d,%d\n", max.RED(), max.GREEN(), max.BLUE());
		for (i = 0; i<this->sizeX(); i++) for (j = 0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i, j);
			v.gammaClampedMax(max, scal, thresh);
			this->update(i, j, v);
		}
	}
	void minmax(hvColRGB<T> &min, hvColRGB<T> &max)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			if (i==0 && j==0) { min=v; max=v; }
			min.keepMin(min,v);
			max.keepMax(max,v);
		}
	}
	void normalize(const hvColRGB<T> &min, const hvColRGB<T> &max, double scal)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			v.normalize(min,max, scal);
			this->update(i,j,v);
		}
	}
	void luminance(T scal, double factor)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			double rr=(double)v.RED()*factor; if (rr>(double)scal) rr=(double)scal;
			double gg=(double)v.GREEN()*factor; if (gg>(double)scal) gg=(double)scal;
			double bb=(double)v.BLUE()*factor; if (bb>(double)scal) bb=(double)scal;
			this->update(i,j,hvColRGB<T>(T((rr+gg+bb)/3.0)));
		}
	}
	void scale(T scal, double factor)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			double rr=(double)v.RED()*factor; if (rr>(double)scal) rr=(double)scal;
			double gg=(double)v.GREEN()*factor; if (gg>(double)scal) gg=(double)scal;
			double bb=(double)v.BLUE()*factor; if (bb>(double)scal) bb=(double)scal;
			this->update(i,j,hvColRGB<T>(T(rr),T(gg),T(bb)));
		}
	}
	template <class X> void scale(T scal, const hvPict<X> &sc, double norm)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			double factor = (double)sc.get(i,j)/norm;
			double rr=(double)v.RED()*factor; if (rr>(double)scal) rr=(double)scal;
			double gg=(double)v.GREEN()*factor; if (gg>(double)scal) gg=(double)scal;
			double bb=(double)v.BLUE()*factor; if (bb>(double)scal) bb=(double)scal;
			this->update(i,j,hvColRGB<T>(T(rr),T(gg),T(bb)));
		}
	}
	void step(T scal, T ss)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			this->update(i,j,hvColRGB<T>(v.RED()<ss?0:scal, v.GREEN()<ss?0:scal, v.BLUE()<ss?0:scal));
		}
	}
	void drawRect(int px, int py, int sx, int sy, hvColRGB<T> v)
	{
		int i,j;
		for (i=px; i<px+sx; i++) for (j=py; j<py+sy; j++)
		{
			if (i>=0 && i<this->sizeX() && j>=0 && j<this->sizeY()) this->update(i,j,v);
		}
	}
	void drawRectBlend(int px, int py, int sx, int sy, hvColRGB<T> v, double alpha)
	{
		int i,j;
		for (i=px; i<px+sx; i++) for (j=py; j<py+sy; j++)
		{
			hvColRGB<unsigned char> cc; cc.blend(v,this->get(i,j),alpha);
			if (i>=0 && i<this->sizeX() && j>=0 && j<this->sizeY()) this->update(i,j,cc);
		}
	}

	void copy(int x, int y, const hvPictRGB<T> &pict)
	{
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			this->update(x+i,y+j,pict.get(i,j));
		}
	}
	void copyRect(int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pict, const hvBitmap &mask)
	{
		int i,j;
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
			if (mask.get(x+i, y+j))
			{
				if (px+i>=0 && px+i<this->sizeX() && py+j>=0 && py+j<this->sizeY()) this->update(px+i,py+j,pict.get(x+i,y+j));
			}
		}
	}
	void copyRect(int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pict)
	{
		int i,j;
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
				if (px+i>=0 && px+i<this->sizeX() && py+j>=0 && py+j<this->sizeY()) this->update(px+i,py+j,pict.get(x+i,y+j));
		}
	}
	void copyRect(int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pict, hvColRGB<unsigned char> col, int dd=2, int bscale=1)
	{
		int i,j;
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
				if (px+i>=0 && px+i<this->sizeX() && py+j>=0 && py+j<this->sizeY())
				{
					if (i<=dd || j<=dd || i>=sx-1-dd || j>=sy-1-dd) this->update(px+i,py+j,col);
					else this->update(px+i,py+j,pict.get(x+i*bscale,y+j*bscale));
				}
		}
	}
	void copyRectShadow(int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pict, hvColRGB<T> col, int hh)
	{
		int i,j;
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
				if (px+i>=0 && px+i<this->sizeX() && py+j>=0 && py+j<this->sizeY())
				{
					if (i==0 || j==0 || i==sx-1 || j==sy-1) this->update(px+i,py+j,col);
					else this->update(px+i,py+j,pict.get(x+i,y+j));
				}
		}
		for (i=0; i<sx; i++) for (j=1; j<=hh; j++)
		{
				if (px+i+j>=0 && px+i+j<this->sizeX() && py-j>=0 && py-j<this->sizeY())
				{
					hvColRGB<T> cc = this->get(px+i+j,py-j);
					cc.scale(1.0-(double)j/(double)(hh+1));
					this->update(px+i+j,py-j,cc);
				}
		}
	}

	double minShift(double scale, const hvPictRGB<T> &pi, int posx, int posy, int deltax, int deltay, int &minx, int &miny)
	{
		minx=0; miny=0;
		double minv=1000000.0;
		int i,j;
		for (i=-deltax; i<=deltax; i++) for (j=-deltay; j<=deltay; j++)
		{
			hvColRGB<double> rr;
			this->squareDifference(scale,this->sizeX()/2-posx, this->sizeY()/2-posy,pi.sizeX()/2-posx+i,pi.sizeY()/2-posy+j,10,10,pi,rr);
			double vv = rr.RED()+rr.GREEN()+rr.BLUE();
			if (vv<minv) { minx=i, miny=j; minv=vv; }
		}
		return minv;
	}

	void apply(T scal, const hvLinearTransform3<double> &t)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			hvVec3<double> col((double)v.RED()/(double)scal, (double)v.GREEN()/(double)scal,(double)v.BLUE()/(double)scal);
			col = t.apply(col);
			v=hvColRGB<T>((T)(col.X()*(double)scal),(T)(col.Y()*(double)scal),(T)(col.Z()*(double)scal));
			this->update(i,j,v);
		}
	}
	void applyinverse(T scal, const hvFrame3<double> &fr, double offset, double rescal)
	{
		hvLinearTransform3<double> t(fr);
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			hvVec3<double> col(((double)v.RED()/(double)scal-offset)/rescal, ((double)v.GREEN()/(double)scal-offset)/rescal,((double)v.BLUE()/(double)scal-offset)/rescal);
			col = t.apply(col);
			v=hvColRGB<T>((T)(col.X()*(double)scal),(T)(col.Y()*(double)scal),(T)(col.Z()*(double)scal));
			this->update(i,j,v);
		}
	}
	void apply(T scal, const hvFrame3<double> &fr, double offset, double rescal)
	{
		hvLinearTransform3<double> t; t.inverseFrame3(fr);
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			hvVec3<double> col((double)v.RED()/(double)scal, (double)v.GREEN()/(double)scal,(double)v.BLUE()/(double)scal);
			col = t.apply(col);
			double rr = (col.X()*rescal+offset);
			if (rr<0.0) rr=0.0; else if (rr>1.0) rr=1.0;
			double gg = (col.Y()*rescal+offset);
			if (gg<0.0) gg=0.0; else if (gg>1.0) gg=1.0;
			double bb = (col.Z()*rescal+offset);
			if (bb<0.0) bb=0.0; else if (bb>1.0) bb=1.0;
			v=hvColRGB<T>((T)(rr*(double)scal),(T)(gg*(double)scal),(T)(bb*(double)scal));
			this->update(i,j,v);
		}
	}

	hvFrame3<double> pca(T scal, hvBitmap *mask=0) const
	{
		int i,j;
		hvVec3<double> sum;
		int npix=0;
		/*** computing mean value ***/
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			hvVec3<double> col;
			bool ok=true;
			if (mask!=0) ok = mask->get(i,j);
			if (ok)
			{ 
				npix++; 
				col = hvVec3<double>((double)v.RED()/(double)scal, (double)v.GREEN()/(double)scal,(double)v.BLUE()/(double)scal);
				sum += col;
			}
		}
		sum /= (double)(npix);
		
		/*** computing Covariance matrix covar ***/
		hvMat3<double> covar;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			bool ok=true;
			if (mask!=0) ok = mask->get(i,j);
			if (ok)
			{ 
				hvColRGB<T> v = this->get(i,j);
				hvVec3<double> col((double)v.RED()/(double)scal, (double)v.GREEN()/(double)scal,(double)v.BLUE()/(double)scal);
				col -= sum;
				covar += hvMat3<double>( 
					hvVec3<double>(col.X()*col.X(), col.X()*col.Y(), col.X()*col.Z()), 
					hvVec3<double>(col.Y()*col.X(), col.Y()*col.Y(), col.Y()*col.Z()),
					hvVec3<double>(col.Z()*col.X(), col.Z()*col.Y(), col.Z()*col.Z())   );
			}
		}
		hvMat3<double> rr = covar.eigen();
		return hvFrame3<double>(sum, rr);
	}


	static void readPPMLine(FILE *fd, char buff[256]) {
	do {
        fgets(buff,255,fd);
        } while(buff[0]=='#');
	}

	void loadPPM(FILE *fd, T norm)
	{
		int  sx, sy;
		int i,j, type;
		char buff[256];
		hvColRGB<T> co;

		readPPMLine(fd,buff);
		if (strcmp(buff,"P6\n")==0) type = 0;
		else if (strcmp(buff,"P3\n")==0) type = 1;
		else { type = 2; printf("unknown picture PPM type=%d (%s)\n", type,buff); } 
		readPPMLine(fd,buff);
		sscanf(buff,"%d %d",&sx,&sy);
		readPPMLine(fd,buff);
		if (strcmp(buff,"255\n")!=0){ printf("type=%d\n", type); reset(); }
		reset(sx, sy, hvColRGB<T>()); 
		for (i=0; i<sy; i++)
		for (j=0; j<sx; j++)
			{
				unsigned char r,g,b;
				if (type==0)
				{
					fread(&r,1,sizeof(unsigned char),fd);
					fread(&g,1,sizeof(unsigned char),fd);
					fread(&b,1,sizeof(unsigned char),fd);
				}
				else if (type==1)
				{
					int rr, gg, bb;
					fscanf(fd, "%d %d %d", &rr, &gg, &bb);
					r= (unsigned char)rr;
					g= (unsigned char)gg;
					b= (unsigned char)bb;
				}
				else { r=0; g=0; b=0; }
				hvArray2< hvColRGB<T> >::update(j,sy-i-1,hvColRGB<T>((T)r/norm, (T)g/norm, (T)b/norm));
			}
	}

	void savePPM(FILE *fd, T norm)
	{
		int i,j;
		hvColRGB<T> co;
		unsigned char v;

		fprintf(fd,"P6\n");
		fprintf(fd,"%d %d\n",this->sizeX(),this->sizeY());
		fprintf(fd,"255\n");
		for (i=0; i<this->sizeY(); i++)
		for (j=0; j<this->sizeX(); j++)
			{
			co = hvArray2< hvColRGB<T> >::get(j, this->sizeY()-i-1);
			v = (unsigned char)((T)co.RED()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)co.GREEN()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)co.BLUE()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			}
	}

	static void RGBE_WriteHeader(FILE *fp, int width, int height, rgbe_header_info *info)
	{
	  char *programtype = "RGBE";

	  if (info && (info->valid & RGBE_VALID_PROGRAMTYPE)) programtype = info->programtype;
	  fprintf(fp,"#?%s\n",programtype);
	  if (info && (info->valid & RGBE_VALID_GAMMA)) { fprintf(fp,"GAMMA=%g\n",info->gamma); }
	  if (info && (info->valid & RGBE_VALID_EXPOSURE)) { fprintf(fp,"EXPOSURE=%g\n",info->exposure); }
	  fprintf(fp,"FORMAT=32-bit_rle_rgbe\n\n");
	  fprintf(fp, "-Y %d +X %d\n", height, width);
	}

	bool RGBE_ReadHeader(FILE *fp, int *width, int *height, rgbe_header_info *info)
	{
	  char buf[256];
	  int found_format;
	  float tempf;
	  int i;

	  found_format = 0;
	  if (info) 
	  {
		info->valid = 0;
		info->programtype[0] = 0;
		info->gamma = 1.0;
		info->exposure = 1.0;
	  }
	  else hvFatal("no rgbe_header_info pointer given");
	  if (fgets(buf,256,fp) == 0) return false;
	  if ((buf[0] != '#')||(buf[1] != '?')) return false;
	  info->valid |= RGBE_VALID_PROGRAMTYPE;
	  for(i=0;i<sizeof(info->programtype)-1;i++) 
		{
		  if ((buf[i+2] == 0) || buf[i+2]==' ') break;
		  info->programtype[i] = buf[i+2];
		}
	  info->programtype[i] = 0;
	  bool cont=true;
	  while(cont) 
	  {
		if (fgets(buf,256,fp) == 0) return false;
		if (strcmp(buf,"FORMAT=32-bit_rle_rgbe\n") == 0) { }
		else if (sscanf(buf,"GAMMA=%g",&tempf) == 1) 
		{
		  info->gamma = tempf;
		  info->valid |= RGBE_VALID_GAMMA;
		}
		else if (sscanf(buf,"EXPOSURE=%g",&tempf) == 1) 
		{
		  info->exposure = tempf;
		  info->valid |= RGBE_VALID_EXPOSURE;
		}
		else if (sscanf(buf,"-Y %d +X %d",height,width) == 2) cont=false;
		//if (cont) if (fgets(buf,256,fp) == 0) return false;
	  }
	  return true;
	}

	void RGBE_WritePixels(FILE *fp, T norm)
	{
		unsigned char rgbe[4];
		int i,j;
		hvColRGB<T> co;
		unsigned char v;

		for (i=0; i<this->sizeY(); i++)
		for (j=0; j<this->sizeX(); j++)
			{
			co = hvArray2< hvColRGB<T> >::get(j, this->sizeY()-i-1);
			co.torgbe(norm, rgbe);
			fwrite(rgbe, 4, 1, fp);
			}
	}

	bool RGBE_ReadPixels_RLE(FILE *fp, T norm, int scanline_width, int num_scanlines)
	{
	  unsigned char rgbe[4], *scanline_buffer, *ptr, *ptr_end;
	  int i, count;
	  unsigned char buf[2];

	  if ((scanline_width < 8)||(scanline_width > 0x7fff)) { printf("not RLE encoded\n"); return false; } //RGBE_ReadPixels(fp,data,scanline_width*num_scanlines);
	  reset(scanline_width, num_scanlines, hvColRGB<T>());
	  scanline_buffer = 0;
	  /* read in each successive scanline */
	  while(num_scanlines > 0) 
	  {
		    if (fread(rgbe,sizeof(rgbe),1,fp) < 1) { printf("file corrupt in line %d\n",num_scanlines);  if (scanline_buffer!=0) free(scanline_buffer); return false; }
			if ((rgbe[0] != 2)||(rgbe[1] != 2)||(rgbe[2] & 0x80)) 
			{
				printf("file is not run length encoded in line %d\n",num_scanlines);
				/* this file is not run length encoded */
				//rgbe2float(&data[0],&data[1],&data[2],rgbe);
				//data += RGBE_DATA_SIZE;
				if (scanline_buffer!=0) free(scanline_buffer);
				return false; //RGBE_ReadPixels(fp,data,scanline_width*num_scanlines-1);
			}
			if ((((int)rgbe[2])<<8 | rgbe[3]) != scanline_width) 
			{
				if (scanline_buffer!=0)  free(scanline_buffer);
				printf("wrong scanline width in line %d\n",num_scanlines);
				return false;
			}
			if (scanline_buffer == 0) scanline_buffer = (unsigned char *)malloc(sizeof(unsigned char)*4*scanline_width);	    
			ptr = &scanline_buffer[0];
		/* read each of the four channels for the scanline into the buffer */
			for(i=0;i<4;i++) 
			{
				ptr_end = &scanline_buffer[(i+1)*scanline_width];
				while(ptr < ptr_end) 
				{
					if (fread(buf,sizeof(buf[0])*2,1,fp) < 1) { free(scanline_buffer); printf("file corrupt\n"); return false; }
					if (buf[0] > 128) 
					{
						/* a run of the same value */
						count = buf[0]-128;
						if ((count == 0)||(count > ptr_end - ptr)) { free(scanline_buffer); printf("bad scanline data"); return false; }
						while(count-- > 0) *ptr++ = buf[1];
					}
					else 
					{
						/* a non-run */
						count = buf[0];
						if ((count == 0)||(count > ptr_end - ptr)) { free(scanline_buffer); printf("bad scanline data"); return false; }
						*ptr++ = buf[1];
						if (--count > 0) 
						{
							if (fread(ptr,sizeof(*ptr)*count,1,fp) < 1) { free(scanline_buffer); printf("file corrupt\n"); return false; }
							ptr += count;
						}
					}
				}
			}
		/* now convert data from buffer into floats */
		for(i=0;i<scanline_width;i++) 
		{
		  rgbe[0] = scanline_buffer[i];
		  rgbe[1] = scanline_buffer[i+scanline_width];
		  rgbe[2] = scanline_buffer[i+2*scanline_width];
		  rgbe[3] = scanline_buffer[i+3*scanline_width];
		  hvColRGB<T> col; col.fromrgbe(norm, rgbe);
		  hvArray2< hvColRGB<T> >::update(i, num_scanlines-1,  col);
		}
		num_scanlines--;
	  }
	  free(scanline_buffer);
	  return true;
	}

	
	
	
	/******************************************************************************
	 * neighborMatchBlocErrorV2()
	 ******************************************************************************/
	double neighborMatchBlocErrorV2(
		const hvBitmap &yet,
		int i, int j, // exemplar coordinate
		float indweight,
		const hvPict<unsigned char> &labels, // guidance labels
		double scale, // normalization factor for data (ex: 255 to work with uchar data from [0;255] to [0;1])
		int bsize,
		const hvBitmap &wmask, // guidance mask
		const hvPictRGB<unsigned char> &ex, const hvPict<unsigned char> &exlabel, // exemplar colors and labels
		int x, int y ) // block coordinate
	{
		int ii, jj;

		// counter of "processed" pixels in block
		int count = 0;

		// Initialize block error
		double err = 0.0;

		// Iterate through block
		for ( jj = 0; jj < bsize; jj++ )
		{
			for ( ii = 0; ii < bsize ; ii ++ )
			{
				int px = ii;
				int py = jj;

				// check bounds
				if ( x + px >= wmask.sizeX() ) px = wmask.sizeX() - 1 - x;
				if ( y + py >= wmask.sizeY() ) py = wmask.sizeY() - 1 - y;
				
				// only if in mask
				if ( !yet.get( x + px, y + py ) && wmask.get( x + px, y + py ) )
				{
					// Compute distance error
					double r, g, b;
					// - guidance
					hvColRGB<unsigned char> col = this->get(x + px, y + py);
					r = (double)col.RED() / scale;
					g = (double)col.GREEN() / scale;
					b = (double)col.BLUE() / scale;
					// - exemplar
					hvColRGB<unsigned char> cole = ex.get(i + px, j + py);
					// - error (L2)
					r = r - (double)cole.RED() / scale;
					g = g - (double)cole.GREEN() / scale;
					b = b - (double)cole.BLUE() / scale;
					err += r*r + g*g + b*b;

					//  Compute label error
					// - guidance
					unsigned char ind = labels.get(x + px, y + py);
					// - exemplar
					unsigned char inde = exlabel.get(i + px, j + py);
					// - error
					if ( ind != inde )
					{
						// add a user given penalty
						err += indweight;
					}
					
					// Update counter (processed pixels)
					count++;
				}
			}
		}
		
		if ( count == 0 )
		{
			// retrun a max error
			err = 255.0;
#if 1 // DEBUG
			printf( "\nneighborMatchBlocErrorV2(): count=0 => err=255" );
#endif
		}
		else
		{
			// Normalize error
			err /= (double)count;
		}
	
		return err;
	}


	/******************************************************************************
	 * Use a random walk like strategy to find best match
	 *
	 * - best candidate will be returned in (rx,ry)
	 ******************************************************************************/
	double bestNeighborMatchBlocV2(
		const hvBitmap &yet,
		hvBitmap &keepit, const float MERR, // per-pixel threshold error in a block to keep it or not
		const int shiftx, const int shifty,
		const float indweight, const hvPict<unsigned char> &labels,
		const double scale, // normalization error
		const int bsize, // block size
		const int factor, // unused parameter
		const hvBitmap &wmask,
		const hvPictRGB<unsigned char> &ex, const hvPict<unsigned char> &exlabel,
		const int x, const int y,
		int &rx, int &ry,
		float rnd = 1.0 )
	{
		int i, j, k, ii, jj;

		// Initialize PRNG (per block)
		unsigned int seed;
		hvNoise::seeding( shiftx + x, shifty + y, 2, seed );

		double errmin = -1.0;

		//rx = 0; ry = 0;
		
		int pcount = 0;
		
		// vector of random sample points
		std::vector<int> ppi, ppj;
		ppi.reserve( 10 );
		ppj.reserve( 10 );
		
		// Initialize first sample
		// - x
		int rposx = (int)( (double)( ex.sizeX() - 2 ) * ( 1.0 - rnd ) );
		if ( rposx < 1 ) rposx = 1;
		// - y
		int rposy = (int)( (double)( ex.sizeY() - 2 ) * ( 1.0 - rnd ) );
		if ( rposy < 1 ) rposy = 1;

		int count = 0;
		for (i = 0; i < bsize; i++) for (j = 0; j < bsize; j++) if ( yet.get( x + i, y + j ) ) count++;
		if ( count == bsize * bsize )
		{
			return 0.0;
		}

		//if (!first)
		//{
		//	double err = neighborMatchBlocErrorV2(rx, ry, indweight, labels, scale, bsize, wmask, ex, exlabel, x, y);
		//	if (err <= MERR)
		//	{
		//		printf("keep %d,%d=>%d,%d, err=%g\n", x, y, rx, ry, err);
		//		return err;
		//	}
		//}
		
		// Sample exemplar coordinates uniformly
		for ( i = 0; i < ex.sizeX() - bsize - 3*rposx/2 - 1; i += rposx ) for ( j = 0; j < ex.sizeY() - bsize -3*rposy/2 - 1; j += rposy )
		{
			ppi.push_back( i ); ppj.push_back( j ); pcount++;

			ppi.push_back( i + rposx/2 ); ppj.push_back( j + rposy/2 ); pcount++;
		}

		// Handle error
		if ( ppi.empty() || ppj.empty() )
		{
			printf( "\nERROR: no sample point in bestNeighborMatchBloc !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
			assert( false );
			return 1.0; // TODO: which value to return?
		}

		// Shuffle coordinates
		// TODO : size of list divided by 2 (or less, ex: > 1/4)
		for ( k = 0; k < 200; k++) // number of passes of shuffle
		{
			int ind1 = (int)((0.5*hvNoise::next(seed)+0.5)*(double)(pcount));
			if ( ind1 >= pcount ) ind1 = pcount - 1;
			ind1 = std::max( 0, ind1 );
			
			int ind2 = (int)((0.5*hvNoise::next(seed) + 0.5)*(double)(pcount));
			if ( ind2 >= pcount ) ind2 = pcount - 1;
			ind2 = std::max( 0, ind1 );
			
			rx = ppi[ ind1 ]; ry = ppj[ ind1 ];
			ppi[ ind1 ] = ppi[ ind2 ]; ppj[ ind1 ] = ppj[ ind2 ];
			ppi[ ind2 ] = rx; ppj[ ind2 ] = ry;
		}

		// Iterate through candidate samples and sample1 point around randomly
		for ( k = 0; k < pcount; k++ )
		{
			double fact = 0.75*hvNoise::next( seed ) + 0.75;
			
			// Sample new candidate inside a radius from candidate
			i = ppi[ k ]; j = ppj[ k ];
			int decalx = (int)( ( 0.5 * hvNoise::next( seed ) + 0.5 ) * (double)( rposx ) );
			int decaly = (int)( ( 0.5 * hvNoise::next( seed ) + 0.5 ) * (double)( rposy ) );
			// - compute block error (MSE)
			double err = neighborMatchBlocErrorV2( yet, i + decalx, j + decaly, indweight, labels, scale, bsize, wmask, ex, exlabel, x, y );
			
			// Update best candidate info
			//if (err==0.0) err = 1.0 / 255.0*(double)rand() / (double)RAND_MAX;
			if (errmin == -1.0) { errmin = err; rx = i + decalx; ry = j + decaly; }
			//else if (errmin>err && err<fact*MERR) { errmin = err; rx = i + decalx; ry = j + decaly; }
			else if (errmin>err) { errmin = err; rx = i + decalx; ry = j + decaly; }
			
			//printf("pixel %d,%d, pos %d,%d -> err=%g (min=%g)\n", x, y, rx, ry, err, errmin);
		}

		// Iterate through best candidate block
		// to update map telling whether or not to keep its pixels according to a per-pixel threshold error
		for ( i = 0; i < bsize; i++ ) for ( j = 0; j < bsize; j++ ) if ( !yet.get( x + i, y + j ) )
		{
			// - compute block error (MSE) => per pixel (see: 1/*block size*/)
			double err = neighborMatchBlocErrorV2( yet, rx+i, ry+j, indweight, labels, scale, 1/*block size*/, wmask, ex, exlabel, x+i, y+j );

			// Update "keep" map
			keepit.set( i, j, err <= MERR );
		}

		//if (!first) printf("modified %d,%d=>%d,%d, err=%g\n", x, y, rx, ry, errmin);
		
		// Return best block error
		return errmin;
	}

	
	// Use a random walk like strategy to find best match
	void refineBestNeighborMatch(const hvPictRGB<unsigned char> &ex, int x, int y, int neighbor, int ix[], int iy[], int nn, int &rx, int &ry, int SX = 0, int SY = 0, int DX = 0, int DY = 0)
	{
		const int NSAMPLES = 5;
		const int DEPTH = 3;
		if (SX == 0) { SX = ex.sizeX(); }
		if (SY == 0) { SY = ex.sizeY(); }
		int RADIUS = 4*neighbor;
		if (RADIUS < 1) RADIUS = 1;
		int i, j, k;
		double minerr, searcherr, besterr;
		int spx, spy, bestx, besty;
		for (i = 0; i < nn; i++)
		{
			int deltax = ix[i] / SX, deltay = iy[i] / SY;
			for (j = 0; j < NSAMPLES; j++) // first level is random
			{
				int px = ix[i] - deltax*SX - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
				if (px < neighbor) px = neighbor;
				if (px >= SX - neighbor) px = SX - neighbor - 1;
				int py = iy[i] - deltay*SY - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
				if (py < neighbor) py = neighbor;
				if (py >= SY - neighbor) py = SY - neighbor - 1;
				px += SX*deltax; py += SY*deltay;
				if (px < neighbor) px = neighbor; if (py < neighbor) py = neighbor;
				if (px >= this->sizeX() - neighbor) px = this->sizeX() - neighbor - 1;
				if (py >= this->sizeY() - neighbor) py = this->sizeY() - neighbor - 1;
				double err = this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor);
				if (j == 0) { searcherr = err; spx = px; spy = py; }
				else if (err < searcherr) { searcherr = err; spx = px; spy = py; }
			}
			minerr = searcherr; bestx = spx; besty = spy;
			//printf("i=%d, %d,%d (neigh=%d, S %d,%d, delta %d,%d\n", i, bestx, besty,neighbor,SX,SY,deltax,deltay);
			RADIUS /= 2; if (RADIUS < 1) RADIUS = 1;
			for (k = 0; k < DEPTH && RADIUS>1; k++)
			{
				int deltax = bestx / SX, deltay = besty / SY;
				for (j = 0; j < NSAMPLES; j++) // next levels are closer and closer to previous best
				{
					int px = bestx - deltax*SX - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
					if (px < neighbor) px = neighbor;
					if (px >= SX - neighbor) px = SX - neighbor - 1;
					int py = besty - deltay*SY - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
					if (py < neighbor) py = neighbor;
					if (py >= SY - neighbor) py = SY - neighbor - 1;
					px += SX*deltax; py += SY*deltay;
					if (px < neighbor) px = neighbor; if (py < neighbor) py = neighbor;
					if (px >= this->sizeX() - neighbor) px = this->sizeX() - neighbor - 1;
					if (py >= this->sizeY() - neighbor) py = this->sizeY() - neighbor - 1;
					double err = this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor);
					if (err < searcherr) { searcherr = err; spx = px; spy = py; }
				}
				if (searcherr < minerr) { minerr = searcherr; bestx = spx; besty = spy; }
				RADIUS /= 2; if (RADIUS < 1) RADIUS = 1;
			}
			if (i == 0) { besterr = minerr; rx = bestx; ry = besty; }
			else if (besterr > minerr) { besterr = minerr; rx = bestx; ry = besty; }
			//printf("i=%d, best = %d,%d\n", i, rx, ry);
		}
	}
	// Use a random walk like strategy to find best match based on a distance / feature image
	void refineBestNeighborMatchwdist(hvPictRGB<unsigned char> &thisdist, const hvPictRGB<unsigned char> &ex, const hvPictRGB<unsigned char> &exdist, double weight, int x, int y, int neighbor, int ix[], int iy[], int nn, int &rx, int &ry, int SX = 0, int SY = 0, int DX = 0, int DY = 0)
	{
		const int NSAMPLES = 5;
		const int DEPTH = 3;
		if (SX == 0) { SX = ex.sizeX(); }
		if (SY == 0) { SY = ex.sizeY(); }
		int RADIUS = 5 * neighbor;
		if (RADIUS < 1) RADIUS = 1;
		int i, j, k;
		double minerr, searcherr, besterr;
		int spx, spy, bestx, besty;
		for (i = 0; i < nn; i++)
		{
			int deltax = ix[i] / SX, deltay = iy[i] / SY;
			for (j = 0; j < NSAMPLES; j++) // first level is random
			{
				int px = ix[i] - deltax*SX - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
				if (px < neighbor) px = neighbor;
				if (px >= SX - neighbor) px = SX - neighbor - 1;
				int py = iy[i] - deltay*SY - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
				if (py < neighbor) py = neighbor;
				if (py >= SY - neighbor) py = SY - neighbor - 1;
				px += SX*deltax; py += SY*deltay;
				if (px < neighbor) px = neighbor; if (py < neighbor) py = neighbor;
				if (px >= this->sizeX() - neighbor) px = this->sizeX() - neighbor - 1;
				if (py >= this->sizeY() - neighbor) py = this->sizeY() - neighbor - 1;
				double err = this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor)*(1.0 - weight)
					+ thisdist.meanSquareDifference(exdist, 255.0, x, y, px, py, neighbor)*weight;
				if (j == 0) { searcherr = err; spx = px; spy = py; }
				else if (err < searcherr) { searcherr = err; spx = px; spy = py; }
			}
			minerr = searcherr; bestx = spx; besty = spy;
			//printf("i=%d, %d,%d (neigh=%d, S %d,%d, delta %d,%d\n", i, bestx, besty,neighbor,SX,SY,deltax,deltay);
			RADIUS /= 2; if (RADIUS < 1) RADIUS = 1;
			for (k = 0; k < DEPTH && RADIUS>1; k++)
			{
				int deltax = bestx / SX, deltay = besty / SY;
				for (j = 0; j < NSAMPLES; j++) // next levels are closer and closer to previous best
				{
					int px = bestx - deltax*SX - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
					if (px < neighbor) px = neighbor;
					if (px >= SX - neighbor) px = SX - neighbor - 1;
					int py = besty - deltay*SY - RADIUS + (int)(2.0*(double)rand() / (double)RAND_MAX*(double)RADIUS);
					if (py < neighbor) py = neighbor;
					if (py >= SY - neighbor) py = SY - neighbor - 1;
					px += SX*deltax; py += SY*deltay;
					if (px < neighbor) px = neighbor; if (py < neighbor) py = neighbor;
					if (px >= this->sizeX() - neighbor) px = this->sizeX() - neighbor - 1;
					if (py >= this->sizeY() - neighbor) py = this->sizeY() - neighbor - 1;
					//double err = this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor);
					double err = this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor)*(1.0 - weight)
						+ thisdist.meanSquareDifference(exdist, 255.0, x, y, px, py, neighbor)*weight;
					if (err < searcherr) { searcherr = err; spx = px; spy = py; }
				}
				if (searcherr < minerr) { minerr = searcherr; bestx = spx; besty = spy; }
				RADIUS /= 2; if (RADIUS < 1) RADIUS = 1;
			}
			if (i == 0) { besterr = minerr; rx = bestx; ry = besty; }
			else if (besterr > minerr) { besterr = minerr; rx = bestx; ry = besty; }
			//printf("i=%d, best = %d,%d\n", i, rx, ry);
		}
	}
	// Use a random walk like strategy to find best match based on a distance / feature image and guidance map
	inline bool keepit(int px, int py, const int excludex[], const int excludey[], int nex)
	{
		if (nex == 0) return true;
		for (int i = 0; i < nex; i++) if (px == excludex[i] && py == excludey[i]) return false;
		return true;
	}
	
	/******************************************************************************
	* refineBestNeighborMatchwdistguidanceV2
	*
	* Refine candidate (look for a better one and store its exemplar's position in (px,py)
	******************************************************************************/
	void refineBestNeighborMatchwdistguidanceV2( int shiftx, int shifty, float indweight, int NSAMPLES,
		const hvPictRGB<unsigned char> &thisdist, const hvPict<unsigned char> &thislabels,
		const hvPictRGB<unsigned char> &ex, const hvPictRGB<unsigned char> &exdist, const hvPict<unsigned char> &exlabels,
		const hvPictRGB<unsigned char> &guidance, const hvPict<unsigned char> &labels, const hvBitmap &mask,
		double weight, double strength, int x, int y, int neighbor,
		const int ix[], const int iy[], int nn,
		int &rx, int &ry, // return best candidate position in exemplar
		const int excludex[], const int excludey[], int nex )
	{
		unsigned int seed;
		hvNoise::seeding( shiftx + x, shifty + y, 3, seed );

		//const int NSAMPLES = 10;
		const int DEPTH = 2;
		int RADIUS = 2 * neighbor;
		if (RADIUS < 1) RADIUS = 2;
		int i, j, k;
		double minerr, searcherr, besterr;
		int spx, spy, bestx, besty;
		bool first = true;
		
		//---------------------------------------------------------------------
		// Search for best candidate
		//---------------------------------------------------------------------

		// Iterate through candidates (ex: 5x5 neighborhoo)
		// and find best one based on criteria : MSE on small neighborhood (2*neighbor+1) on color, distance and guidance
		for (i = 0; i < nn; i++)
		{
			int px = ix[i];
			int py = iy[i];
			if ( keepit( px, py, excludex, excludey, nex ) )
			{
				// Compute error on colors, distances and guidance (pptbf + labels + mask)
				double err = (1.0 - strength) *
								( this->meanSquareDifference( ex, 255.0, x, y, px, py, neighbor ) * ( 1.0 - weight )
								+ thisdist.meanSquareDifference( exdist, 255.0, x, y, px, py, neighbor) * weight )
							+ strength * guidance.meanSquareDifferenceGuidance( indweight, labels, mask, exdist, exlabels, 255.0, x, y, px, py, neighbor );

				// Update best candidate info (error + position)
				if ( first )
				{
					first = false;  besterr = err; rx = px; ry = py;
				}
				else if ( !first && err < besterr )
				{
					besterr = err; rx = px; ry = py;
				}
			}
		}

		//---------------------------------------------------------------------
		// Try to find a better candidate by sampling randomly around it (with decreasing radius search)
		//---------------------------------------------------------------------

		// Iterate through candidates (ex: 5x5 neighborhoo)
		// and find best one based on criteria : MSE on small neighborhood (2*neighbor+1) on color, distance and guidance
		for ( i = 0; i < nn; i++ )
		{
			// Try to find a better candidate by sampling ramdomly round

			bool startd = true;
			while ( startd )
			{
				for ( j = 0; j < NSAMPLES; j++ ) // first level is random
				{
					int px = ix[i] - RADIUS + (int)(2.0*(0.5+0.5*hvNoise::next(seed))*(double)RADIUS);
					if (px < neighbor) px = neighbor;
					if (px >= ex.sizeX() - neighbor) px = ex.sizeX() - neighbor - 1;
					
					int py = iy[i] - RADIUS + (int)(2.0*(0.5 + 0.5*hvNoise::next(seed))*(double)RADIUS);
					if (py < neighbor) py = neighbor;
					if (py >= ex.sizeY() - neighbor) py = ex.sizeY() - neighbor - 1;

					if ( keepit( px, py, excludex, excludey, nex ) )
					{
						double err = (1.0 - strength) *
										( this->meanSquareDifference( ex, 255.0, x, y, px, py, neighbor ) * ( 1.0 - weight )
										+ thisdist.meanSquareDifference(exdist, 255.0, x, y, px, py, neighbor) * weight )
									+ strength * guidance.meanSquareDifferenceGuidance( indweight, labels, mask, exdist, exlabels, 255.0, x, y, px, py, neighbor );

						if (startd) { startd = false;  searcherr = err; spx = px; spy = py; }
						else if (!startd && err < searcherr) { searcherr = err; spx = px; spy = py; }
					}
				}
				if (startd) RADIUS *= 2;
			}
			minerr = searcherr; bestx = spx; besty = spy;
			//printf("i=%d, %d,%d (neigh=%d, S %d,%d, delta %d,%d\n", i, bestx, besty,neighbor,SX,SY,deltax,deltay);
			
			// Restart process within a smaller radius search
			
			RADIUS /= 2; if (RADIUS < 2) RADIUS = 2;
			for ( k = 0; k < DEPTH && RADIUS>2; k++ )
			{
				for ( j = 0; j < NSAMPLES; j++ ) // next levels are closer and closer to previous best
				{
					int px = bestx - RADIUS + (int)(2.0*(0.5 + 0.5*hvNoise::next(seed))*(double)RADIUS);
					if (px < neighbor) px = neighbor;
					if (px >= ex.sizeX() - neighbor) px = ex.sizeX() - neighbor - 1;
					int py = besty - RADIUS + (int)(2.0*(0.5 + 0.5*hvNoise::next(seed))*(double)RADIUS);
					if (py < neighbor) py = neighbor;
					if (py >= ex.sizeY() - neighbor) py = ex.sizeY() - neighbor - 1;
					//double err = this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor);
					if (keepit(px, py, excludex, excludey, nex))
					{
						double err = (1.0 - strength)*(this->meanSquareDifference(ex, 255.0, x, y, px, py, neighbor)*(1.0 - weight)
							+ thisdist.meanSquareDifference(exdist, 255.0, x, y, px, py, neighbor)*weight) +
							strength*guidance.meanSquareDifferenceGuidance(indweight, labels, mask, exdist, exlabels, 255.0, x, y, px, py, neighbor);
						if (err < searcherr) { searcherr = err; spx = px; spy = py; }
					}
				}
				
				if (searcherr < minerr) { minerr = searcherr; bestx = spx; besty = spy; }

				RADIUS /= 2; if (RADIUS < 1) RADIUS = 1;
			}
			//if (i == 0) { besterr = minerr; rx = bestx; ry = besty; }
			//else 
			if (besterr > minerr) { besterr = minerr; rx = bestx; ry = besty; }
			//printf("i=%d, best = %d,%d\n", i, rx, ry);
		}
	}


	
	////////////////////////////////////////////////////////////
	// compute square differences
	////////////////////////////////////////////////////////////
	int squareDifference(double scale, int x, int y, int i, int j, int neighbor, const hvBitmap &bseed, const hvBitmap &bm, hvColRGB<double> &res)
	{
		int count=0, nx, ny;
		double errr=0.0, errg=0.0, errb=0.0;
		for (nx=-neighbor;nx<=neighbor; nx++)
		for (ny=-neighbor;ny<=neighbor; ny++)
		{
			int px, py, epx, epy;
			px = x+nx; if (px<0) px+=this->sizeX(); else if (px>= this->sizeX()) px-= this->sizeX();
			py = y+ny; if (py<0) py+= this->sizeY(); else if (py>= this->sizeY()) py-= this->sizeY();
			epx = i+nx; if (epx<0) epx+= this->sizeX(); else if (epx>= this->sizeX()) epx-= this->sizeX();
			epy = j+ny; if (epy<0) epy+= this->sizeY(); else if (epy>= this->sizeY()) epy-= this->sizeY();
			if (bseed.get(px,py)==true && bm.get(epx, epy)==true)
			{
				double r,g,b;
				hvColRGB<unsigned char> col = this->get(px,py);
				r = (double)col.RED()/scale;
				g = (double)col.GREEN()/scale;
				b = (double)col.BLUE()/scale;
				col = this->get(epx,epy);
				r = r - (double)col.RED()/scale;
				g = g - (double)col.GREEN()/scale;
				b = b - (double)col.BLUE()/scale;
				errr += r*r; errg += g*g; errb += b*b;
				count++;
			}
		}
		res = hvColRGB<double>(errr, errg, errb);
		return count;
	}
	
	double meanSquareDifference(const hvPictRGB<unsigned char> &ex, double scale, int x, int y, int i, int j, int neighbor) const
	{
		int count = 0, nx, ny;
		double errr = 0.0, errg = 0.0, errb = 0.0;
		for (nx = -neighbor; nx <= neighbor; nx++)
			for (ny = -neighbor; ny <= neighbor; ny++)
			{
				int px, py, epx, epy;
				px = x + nx; if (px<0) px += this->sizeX(); else if (px >= this->sizeX()) px -= this->sizeX();
				py = y + ny; if (py<0) py += this->sizeY(); else if (py >= this->sizeY()) py -= this->sizeY();
				epx = i + nx; if (epx<0) epx += ex.sizeX(); else if (epx >= ex.sizeX()) epx -= ex.sizeX();
				epy = j + ny; if (epy<0) epy += ex.sizeY(); else if (epy >= ex.sizeY()) epy -= ex.sizeY();
				if (px>=0 && px<this->sizeX() && py >= 0 && py<this->sizeY() &&
					epx>=0 && epx<ex.sizeX() && epy >= 0 && epy<ex.sizeY() )
				{
					double r, g, b;
					hvColRGB<unsigned char> col = this->get(px, py);
					r = (double)col.RED() / scale;
					g = (double)col.GREEN() / scale;
					b = (double)col.BLUE() / scale;
					col = ex.get(epx, epy);
					r = r - (double)col.RED() / scale;
					g = g - (double)col.GREEN() / scale;
					b = b - (double)col.BLUE() / scale;
					errr += r*r; errg += g*g; errb += b*b;
					count++;
				}
			}
		if (count == 0) hvFatal("could not find neighborhood");
		return (errr+errg+errb)/3.0/(double)count;
	}
	double meanSquareDifference(const hvBitmap &mask, const hvPictRGB<unsigned char> &ex, double scale, int x, int y, int i, int j, int neighbor) const
	{
		int count = 0, nx, ny;
		double errr = 0.0, errg = 0.0, errb = 0.0;
		for (nx = -neighbor; nx <= neighbor; nx++)
			for (ny = -neighbor; ny <= neighbor; ny++)
			{
				int px, py, epx, epy;
				px = x + nx; if (px<0) px += this->sizeX(); else if (px >= this->sizeX()) px -= this->sizeX();
				py = y + ny; if (py<0) py += this->sizeY(); else if (py >= this->sizeY()) py -= this->sizeY();
				epx = i + nx; if (epx<0) epx += ex.sizeX(); else if (epx >= ex.sizeX()) epx -= ex.sizeX();
				epy = j + ny; if (epy<0) epy += ex.sizeY(); else if (epy >= ex.sizeY()) epy -= ex.sizeY();
				if (px >= 0 && px<this->sizeX() && py >= 0 && py<this->sizeY() &&
					epx >= 0 && epx<ex.sizeX() && epy >= 0 && epy<ex.sizeY())
				{
					if (mask.get(px, py))
					{
						double r, g, b;
						hvColRGB<unsigned char> col = this->get(px, py);
						r = (double)col.RED() / scale;
						g = (double)col.GREEN() / scale;
						b = (double)col.BLUE() / scale;
						col = ex.get(epx, epy);
						r = r - (double)col.RED() / scale;
						g = g - (double)col.GREEN() / scale;
						b = b - (double)col.BLUE() / scale;
						errr += r*r; errg += g*g; errb += b*b;
						count++;
					}
				}
			}
		if (count == 0) return 0.0;
		return (errr + errg + errb) / 3.0 / (double)count;
	}
	double meanSquareDifferenceGuidance(float indweight, const hvPict<unsigned char> &labels, const hvBitmap &mask, const hvPictRGB<unsigned char> &ex, const hvPict<unsigned char> &exlabels, double scale, int x, int y, int i, int j, int neighbor) const
	{
		int count = 0, nx, ny;
		double errr = 0.0, errg = 0.0, errb = 0.0, errl=0.0;
		for (nx = -neighbor; nx <= neighbor; nx++)
			for (ny = -neighbor; ny <= neighbor; ny++)
			{
				int px, py, epx, epy;
				px = x + nx; if (px<0) px += this->sizeX(); else if (px >= this->sizeX()) px -= this->sizeX();
				py = y + ny; if (py<0) py += this->sizeY(); else if (py >= this->sizeY()) py -= this->sizeY();
				epx = i + nx; if (epx<0) epx += ex.sizeX(); else if (epx >= ex.sizeX()) epx -= ex.sizeX();
				epy = j + ny; if (epy<0) epy += ex.sizeY(); else if (epy >= ex.sizeY()) epy -= ex.sizeY();
				if (px >= 0 && px<this->sizeX() && py >= 0 && py<this->sizeY() &&
					epx >= 0 && epx<ex.sizeX() && epy >= 0 && epy<ex.sizeY())
				{
					if (mask.get(px, py))
					{
						double r, g, b;
						hvColRGB<unsigned char> col = this->get(px, py);
						r = (double)col.RED() / scale;
						g = (double)col.GREEN() / scale;
						b = (double)col.BLUE() / scale;
						hvColRGB<unsigned char> cole = ex.get(epx, epy);
						r = r - (double)cole.RED() / scale;
						g = g - (double)cole.GREEN() / scale;
						b = b - (double)cole.BLUE() / scale;
						errr += r*r; errg += g*g; errb += b*b;
						if (labels.get(px, py) != exlabels.get(epx,epy)) errl += indweight;
						count++;
					}
				}
			}
		if (count == 0) return 0.0;
		return ((errr + errg + errb ) / 3.0+errl) / (double)count;
	}
	int squareDifference(double scale, int x, int y, int i, int j, int neighbor, hvColRGB<double> &res)
	{
		int count=0, nx, ny;
		double errr=0.0, errg=0.0, errb=0.0;
		for (nx=-neighbor;nx<=neighbor; nx++)
		for (ny=-neighbor;ny<=neighbor; ny++)
		{
			int px, py, epx, epy;
			px = x+nx; if (px<0) px+= this->sizeX(); else if (px>= this->sizeX()) px-= this->sizeX();
			py = y+ny; if (py<0) py+= this->sizeY(); else if (py>= this->sizeY()) py-= this->sizeY();
			epx = i+nx; if (epx<0) epx+= this->sizeX(); else if (epx>= this->sizeX()) epx-= this->sizeX();
			epy = j+ny; if (epy<0) epy+= this->sizeY(); else if (epy>= this->sizeY()) epy-= this->sizeY();
			double r,g,b;
			hvColRGB<unsigned char> col = this->get(px,py);
			r = (double)col.RED()/scale;
			g = (double)col.GREEN()/scale;
			b = (double)col.BLUE()/scale;
			col = this->get(epx,epy);
			r = r - (double)col.RED()/scale;
			g = g - (double)col.GREEN()/scale;
			b = b - (double)col.BLUE()/scale;
			errr += r*r; errg += g*g; errb += b*b;
			count++;
		}
		res = hvColRGB<double>(errr, errg, errb);
		return count;
	}
	void squareDifference(double scale, int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pi, const hvBitmap &mask, hvColRGB<double> &res)
	{
		int count=0, i,j;
		double errr=0.0, errg=0.0, errb=0.0;
		for (i=0; i<sx;i++)
		for (j=0; j<sy;j++)
		{
			if (mask.get(x+i,y+j))
			{
				double r,g,b;
				//if (px+i<0 || px+i>=sizeX() || py+j<0 || py+j>=sizeY()) { printf("out of this picture range: %d,%d\n", px+i,py+j); }
				int ppx=px+i, ppy=py+j;
				if (ppx<0) ppx+= this->sizeX(); else if (ppx>= this->sizeX()) ppx-= this->sizeX();
				if (ppy<0) ppy+= this->sizeY(); else if (ppy>= this->sizeY()) ppy-= this->sizeY();
				hvColRGB<unsigned char> col = this->get(ppx,ppy);
				r = (double)col.RED()/scale;
				g = (double)col.GREEN()/scale;
				b = (double)col.BLUE()/scale;
				//if (x+i<0 || x+i>=pi.sizeX() || y+j<0 || y+j>=pi.sizeY()) { printf("out of pi picture range: %d,%d\n", x+i,y+j); }
				col = pi.get(x+i,y+j);
				r = r - (double)col.RED()/scale;
				g = g - (double)col.GREEN()/scale;
				b = b - (double)col.BLUE()/scale;
				errr += r*r; errg += g*g; errb += b*b;
			}
		}
		res = hvColRGB<double>(errr, errg, errb);
	}
	void squareDifference(double scale, int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pi, hvColRGB<double> &res, int step=1)
	{
		int count=0, i,j;
		double errr=0.0, errg=0.0, errb=0.0;
		for (i=0; i<sx;i+=step)
		for (j=0; j<sy;j+=step)
		{
				double r,g,b;
				//if (px+i<0 || px+i>=sizeX() || py+j<0 || py+j>=sizeY()) { printf("out of this picture range: %d,%d\n", px+i,py+j); }
				int ppx=px+i, ppy=py+j;
				if (ppx<0) ppx+= this->sizeX(); else if (ppx>= this->sizeX()) ppx-= this->sizeX();
				if (ppy<0) ppy+= this->sizeY(); else if (ppy>= this->sizeY()) ppy-= this->sizeY();
				hvColRGB<unsigned char> col = this->get(ppx,ppy);
				r = (double)col.RED()/scale;
				g = (double)col.GREEN()/scale;
				b = (double)col.BLUE()/scale;
				//if (x+i<0 || x+i>=pi.sizeX() || y+j<0 || y+j>=pi.sizeY()) { printf("out of pi picture range: %d,%d\n", x+i,y+j); }
				col = pi.get(x+i,y+j);
				r = r - (double)col.RED()/scale;
				g = g - (double)col.GREEN()/scale;
				b = b - (double)col.BLUE()/scale;
				errr += r*r; errg += g*g; errb += b*b;
		}
		res = hvColRGB<double>(errr, errg, errb);
	}
	void squareDifferenceBorder(double scale, int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pi, hvColRGB<double> &res, int depth = 1)
	{
		int count = 0, i, j;
		double errr = 0.0, errg = 0.0, errb = 0.0;
		for (i = 0; i<sx; i += 1)
			for (j = 0; j<sy; j += 1)
			{
				if (i < depth || i >= sx - depth || j < sy || j >= sy - depth)
				{
					double r, g, b;
					//if (px+i<0 || px+i>=sizeX() || py+j<0 || py+j>=sizeY()) { printf("out of this picture range: %d,%d\n", px+i,py+j); }
					int ppx = px + i, ppy = py + j;
					if (ppx < 0) ppx += this->sizeX(); else if (ppx >= this->sizeX()) ppx -= this->sizeX();
					if (ppy < 0) ppy += this->sizeY(); else if (ppy >= this->sizeY()) ppy -= this->sizeY();
					hvColRGB<unsigned char> col = this->get(ppx, ppy);
					r = (double)col.RED() / scale;
					g = (double)col.GREEN() / scale;
					b = (double)col.BLUE() / scale;
					//if (x+i<0 || x+i>=pi.sizeX() || y+j<0 || y+j>=pi.sizeY()) { printf("out of pi picture range: %d,%d\n", x+i,y+j); }
					col = pi.get(x + i, y + j);
					r = r - (double)col.RED() / scale;
					g = g - (double)col.GREEN() / scale;
					b = b - (double)col.BLUE() / scale;
					errr += r*r; errg += g*g; errb += b*b;
				}
			}
		res = hvColRGB<double>(errr, errg, errb);
	}

	void weightedSquareDifference(double scale, int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &pi, const hvPict<double> &weight, hvColRGB<double> &res)
	{
		int count=0, i,j;
		double errr=0.0, errg=0.0, errb=0.0;
		for (i=0; i<sx;i++)
		for (j=0; j<sy;j++)
		{
			if (weight.get(x+i,y+j)>0.0)
			{
				double r,g,b;
				double ww = weight.get(x+i,y+j);
				//if (px+i<0 || px+i>=sizeX() || py+j<0 || py+j>=sizeY()) { printf("out of this picture range: %d,%d\n", px+i,py+j); }
				int ppx=px+i, ppy=py+j;
				if (ppx<0) ppx+= this->sizeX(); else if (ppx>= this->sizeX()) ppx-= this->sizeX();
				if (ppy<0) ppy+= this->sizeY(); else if (ppy>= this->sizeY()) ppy-= this->sizeY();
				hvColRGB<unsigned char> col = this->get(ppx,ppy);
				r = (double)col.RED()/scale;
				g = (double)col.GREEN()/scale;
				b = (double)col.BLUE()/scale;
				//if (x+i<0 || x+i>=pi.sizeX() || y+j<0 || y+j>=pi.sizeY()) { printf("out of pi picture range: %d,%d\n", x+i,y+j); }
				col = pi.get(x+i,y+j);
				r = r - (double)col.RED()/scale;
				g = g - (double)col.GREEN()/scale;
				b = b - (double)col.BLUE()/scale;
				errr += r*r*ww; errg += g*g*ww; errb += b*b*ww;
			}
		}
		res = hvColRGB<double>(errr, errg, errb);
	}
	
	void imagefromindex(const hvPictRGB<T> &example, hvArray2<hvVec2<int> > &index)
	{
		this->reset(index.sizeX(), index.sizeY(), hvColRGB<T>());
		int i, j;
		for (i = 0; i < this->sizeX(); i++) for (j = 0; j < this->sizeY(); j++)
		{
			hvVec2<int> p = index.get(i, j);
			this->update(i, j, example.get(p.X(), p.Y()));
		}
	}

	// make gaussian stack
	void makepyr( const char *name, const hvPictRGB<T> &dist, const hvPict<T> &exlabel, int &s, int &factor, hvPictRGB<T> pyr[10], hvPictRGB<T> pyrdist[10], hvPict<T> pyrlabel[10], int maxs=9) const
	{
		char buff[256];
		FILE *fd;	
		int ii, jj;
		s = 0; factor = 1;
		pyr[0].clone(*this, 0, 0, this->sizeX() - 1, this->sizeY() - 1);
		pyrdist[0].clone(dist, 0, 0, dist.sizeX() - 1, dist.sizeY() - 1);
		pyrlabel[0].clone(exlabel, 0, 0, exlabel.sizeX() - 1, exlabel.sizeY() - 1);
		while (s < maxs && pyr[s].sizeX()>16 && pyr[s].sizeY() > 16)
		{
			s++; factor *= 2;  
			pyr[s].shrink(&(pyr[s - 1]), 0);
			sprintf(buff, "%s_pcts_%02d_ex.ppm", name, s);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			pyr[s].savePPM(fd, 1);
			fclose(fd);
			pyrdist[s].shrink(&(pyrdist[s - 1]), 0);
			sprintf(buff, "%s_pcts_%02d_mask.ppm", name, s);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			pyrdist[s].savePPM(fd, 1);
			fclose(fd);
			pyrlabel[s].reset(pyrlabel[s - 1].sizeX()/2, pyrlabel[s - 1].sizeY()/2, 0);
			for (ii = 0; ii < pyrlabel[s].sizeX(); ii++) for (jj = 0; jj < pyrlabel[s].sizeY(); jj++)
			{
				pyrlabel[s].update(ii, jj, pyrlabel[s-1].get(2 * ii, 2 * jj));
			}
			hvPictRGB<unsigned char> labelrgb(pyrlabel[s], 1);
			sprintf(buff, "%s_pcts_%02d_label.ppm", name, s);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			labelrgb.savePPM(fd, 1);
			fclose(fd);
		}
	}

	/******************************************************************************
	 * smartinitV2()
	 ******************************************************************************/
	void smartinitV2( const int shiftx, const int shifty, const float MERR, const float indweight, int bb,
		hvPictRGB<T> &guidance, const hvPict<T> &labels, const hvBitmap &mask,
		const hvPictRGB<T> &example, const hvPictRGB<T> &distance, const hvPict<T> &exlabels,
		hvArray2<hvVec2<int> > &index,
		hvPictRGB<T> &synth, hvPictRGB<T> &synthdist, hvPict<T> &synthlabels
	)
	{
		int i, j;
		int rx, ry;

		printf( "- start smart init box size=%d, at shift %d,%d\n", bb, shiftx, shifty );
		
		// Iterate through scales: from top (fine) to bottom (coarse)
		// - refining best block candidates accorind to criteria (user provided min/threshold errors on colors, distances, guidance [pptbfn labels])
		bool first = true;
		hvBitmap yet( index.sizeX(), index.sizeY(), false );
		do {
			// Iterate through blocks
			for ( j = 0; j < index.sizeY(); j += bb )
			{
				for ( i = 0; i < index.sizeX(); i += bb )
				{
					// Per-block bitmap telling whether or not to keep a pixel given its error
					hvBitmap keepit( bb, bb, false );
				
					//if (!first) 
					//{ 
					//	hvVec2<int> pos = index.get(i, j);  
					//	rx = pos.X(); 
					//	ry = pos.Y(); 
					//	//printf("at %d,%d -> %d,%d\n", i, j, rx, ry);
					//}
					//else { rx = 0; ry = 0; }
				
					// Search for best block (position in [rx,ry]) by sampling blocks in exemplar
					rx = 0; ry = 0;
					double err = guidance.bestNeighborMatchBlocV2(
									yet,
									keepit, MERR,
									shiftx, shifty,
									indweight, labels,
									255.0,
									bb,
									2/*unused parameter*/,
									mask,
									distance, exlabels,
									i, j,
									rx, ry, // returned best candidate coordinate
									0.95 );
				
					// Store/write best block data
					// Iterate through best candidate block
					for ( int jj = 0; jj < bb; jj++ )
					{
						for ( int ii = 0; ii < bb; ii++ )
						{
							// Check index map bounds
							if ( i + ii < index.sizeX() && j + jj < index.sizeY() && !yet.get(i+ii,j+jj) && (keepit.get(ii,jj) || bb/2<8) )
							{
								int posx = rx + ii, posy = ry + jj;
						
								// Check exemplar bounds
								if ( posx >= example.sizeX() ) posx = example.sizeX() - 1;
								if ( posy >= example.sizeY() ) posy = example.sizeY() - 1;
						
								// Update index map
								index.update( i + ii, j + jj, hvVec2<int>(posx, posy) );

								// Update synthesized maps (colors, distance, labels)
								synth.update( i + ii, j + jj, example.get(posx, posy) );
								synthdist.update( i + ii, j + jj, distance.get(posx, posy) );
								synthlabels.update( i + ii, j + jj, exlabels.get(posx, posy) );

								// Update "yet" map
								yet.set( i + ii, j + jj, true );
							}
						}
					}
				}
			}
			
			// Divide block size by 2 (to work on smaller blocks)
			bb /= 2;
			
			// Update flag
			first = false; 
		}
		while ( bb >= 4 ); // stop criterion
	}

	/******************************************************************************
	 * parallel controllable texture synthesis with distance map and guidance
	 ******************************************************************************/
	void pctswdistguidanceV2(
		const char *name,
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
			sprintf(buff, "%s_pcts_%02d_guidance.ppm", name, i);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			guid[i].savePPM(fd, 1);
			fclose(fd);

			hvPictRGB<unsigned char> labelsrgb(labels[i], 1);
			sprintf(buff, "%s_pcts_%02d_guidancelabels.ppm", name, i);
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
		sprintf(buff, "%s_pcts_%02d_blocinit.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		pinit.savePPM(fd, 1);
		fclose(fd);

		sprintf(buff, "%s_pcts_%02d_distinit.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		finit.savePPM(fd, 1);
		fclose(fd);

		sprintf(buff, "%s_pcts_%02d_labinit.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		labinit.savePPM(fd, 1);
		fclose(fd);
#endif

		//printf("%s_pcts_%02d_init\n", name, s);
#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
		sprintf(buff, "%s_pcts_%02d_init.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synth[s].savePPM(fd, 1);
		fclose(fd);

		sprintf(buff, "%s_pcts_%02d_dist.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synthdist[s].savePPM(fd, 1);
		fclose(fd);

		hvPictRGB<unsigned char> synthlabrgb(synthlabels[s], 1);
		sprintf(buff, "%s_pcts_%02d_distlabel.ppm", name, s);
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
		printf("starting pcts at level:%d, shift:%d,%d\n", s, posx / factor - bsize/factor, posy / factor - bsize/factor);
		this->pctsiterwdistguidanceV2(
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
	
/******************************************************************************
 * pctsiterwdistguidanceV2
 ******************************************************************************/
void pctsiterwdistguidanceV2( const char *name, int STOPATLEVEL, int shiftx, int shifty, hvPictRGB<T> pyr[], hvPictRGB<T> pyrdist[], hvPict<unsigned char> pyrlabels[],
	hvPictRGB<T> synth[], hvPictRGB<T> synthdist[], hvPict<unsigned char> synthlabels[],
	hvPictRGB<T> guid[], hvPict<unsigned char> labels[], hvBitmap gmask[], hvArray2<hvVec2<int> > &index,
	double weight, double powr,
	int s, int factor,
	int neighbor,
	int niter, // nb correction passes
	float indweight ) // label error penalty
{
	char buff[256];

	int i, j, k;
	
	FILE *fd;

	// Gudance strength vary per pyramid level
#ifndef USE_NO_GUIDANCE_RELAXATION
	double step = 1.0 / (double)( s + 1 );
#else
	double step = 1.0 / (double)( s + 2 ); // to avoid strength = 0 when s = 0 (at finest resolution)
#endif
	double strength = 1.0 - step;

	hvArray2<hvVec2<int> > oldindex;
	hvPictRGB<unsigned char> prefine;
	hvBitmap refinebin;
	hvPictRGB<unsigned char> offsets(index.sizeX(), index.sizeY(), hvColRGB<unsigned char>());
	for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
	{
		offsets.update(i, j, hvColRGB<unsigned char>((unsigned char)((double)index.get(i, j).X() / (double)pyr[s].sizeX() * 128.0 + 128.0),
			(unsigned char)((double)index.get(i, j).Y() / (double)pyr[s].sizeY() * 128.0 + 128.0), 0));
	}

#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
	sprintf(buff, "%s_pcts_%02d_offsets.ppm", name, s);
	fd = fopen(buff, "wb");
	if (fd == 0) { perror("cannot load file:"); }
	offsets.savePPM(fd, 1);
	fclose(fd);
#endif

	// Iterate thruogh pyramid levels
	//indweight = 0.0;
	while ( s > 0 )
	{
		printf( "\n-----------------------------------" );
		printf( "\nlevel %d, factor %d\n", s, factor );
		printf( "-----------------------------------\n" );

		auto startTime = std::chrono::high_resolution_clock::now();

		// Iterate through correction passes (usually 2)
		for ( k = 0; k < niter; k++ )
		{
			oldindex.clone(index);
			prefine.reset(index.sizeX(), index.sizeY(), hvColRGB<unsigned char>(0));
			if ( s >= STOPATLEVEL )
			{
#ifndef USE_NO_GUIDANCE_RELAXATION
				// Correction pass
				synth[ s ].correctionPasswdistguidanceV2(
					s,
					shiftx+k, shifty+k,
					k % 2 != 0,
					k % 2 == 0 ? indweight : 0.0,
					6,
					synthdist[s], synthlabels[s],
					pyr[s], pyrdist[s], pyrlabels[s],
					guid[s], labels[s], gmask[s],
					weight,
					neighbor,
					k % 2 == 0 ? pow(strength, powr) : 0.0,
					index,
					refinebin );
#else
				// Correction pass
				synth[ s ].correctionPasswdistguidanceV2(
					s, // pyramid level
					shiftx+k, shifty+k,
					k % 2 != 0, // remove doubles (only at 2nd correction step)
					indweight, // label weight (none at at 2nd correction step)
					6, // nb samples to search randomly around current candidate
					synthdist[s], synthlabels[s], // synthesized distance and labels
					pyr[s], pyrdist[s], pyrlabels[s], // exemplar color, distance and labels
					guid[s], labels[s], gmask[s], // guidance pptbf, labels and mask
					weight, // weight between color and distance
					neighbor, // neighbor size for MSE (during best candidate search)
					pow( strength, powr ), // guidance weight
					index, // index map (uv)
					refinebin );
#endif
			}
			else
			{
				refinebin.reset( synth[s].sizeX(), synth[s].sizeY(), false );
			}

			hvPictRGB<unsigned char> offsets(index.sizeX(), index.sizeY(), hvColRGB<unsigned char>());
			for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
			{
				hvVec2<int> npos = index.get(i, j);
				offsets.update(i, j, hvColRGB<unsigned char>((unsigned char)((double)npos.X() / (double)pyr[s].sizeX() * 128.0 + 128.0),
					(unsigned char)((double)npos.Y() / (double)pyr[s].sizeY() * 128.0 + 128.0), 0));
				//if (npos.X() != oldindex.get(i, j).X() || npos.Y() != oldindex.get(i, j).Y()) 
				if (refinebin.get(i, j))
					prefine.update(i, j, synth[s].get(i, j));
				else
					prefine.update(i, j, hvColRGB<unsigned char>(255));
			}
#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
			sprintf(buff, "%s_pcts_%02d_corr%doffsets.ppm", name, s, k);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			offsets.savePPM(fd, 1);
			fclose(fd);

			sprintf(buff, "%s_pcts_%02d_corr%d.ppm", name, s, k);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			synth[s].savePPM(fd, 1);
			fclose(fd);

			sprintf(buff, "%s_pcts_%02d_dist%d.ppm", name, s, k);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			synthdist[s].savePPM(fd, 1);
			fclose(fd);

			sprintf(buff, "%s_pcts_%02d_ref%d.ppm", name, s, k);
			fd = fopen(buff, "wb");
			if (fd == 0) { perror("cannot load file:"); }
			prefine.savePPM(fd, 1);
			fclose(fd);
#endif
			//printf("hit return\n"); fgets(buff, 5, stdin);
		}

		synth[s - 1].upscaleJitterPass(pyr[s - 1], 2, 0, index);
		synthdist[s - 1].imagefromindex(pyrdist[s - 1], index);
		synthlabels[s - 1].imagefromindex(pyrlabels[s - 1], index);
		s--; factor /= 2; strength -= step; if (strength < 0.0) strength = 0.0;
		shiftx *= 2; shifty *= 2;
		
#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
		sprintf(buff, "%s_pcts_%02d_init.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synth[s].savePPM(fd, 1);
		fclose(fd);
		
		sprintf(buff, "%s_pcts_%02d_dist.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		synthdist[s].savePPM(fd, 1);
		fclose(fd);
		
		offsets.reset(index.sizeX(), index.sizeY(), hvColRGB<unsigned char>());
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			offsets.update(i, j, hvColRGB<unsigned char>((unsigned char)((double)index.get(i, j).X() / (double)pyr[s].sizeX() * 128.0 + 128.0),
				(unsigned char)((double)index.get(i, j).Y() / (double)pyr[s].sizeY() * 128.0 + 128.0), 0));
		}
		sprintf(buff, "%s_pcts_%02d_offsets.ppm", name, s);
		fd = fopen(buff, "wb");
		if (fd == 0) { perror("cannot load file:"); }
		offsets.savePPM(fd, 1);
		fclose(fd);
#endif

		// - timer
		auto endTime = std::chrono::high_resolution_clock::now();
		float elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
		std::cout << "\ntime: " << elapsedTime << " ms\n";
	}
	
	// Final correction pass
	oldindex.clone(index);
	prefine.reset(index.sizeX(), index.sizeY(), hvColRGB<unsigned char>(0));
	if (s == 0)
	{
		printf("\n-----------------------------------");
		printf("\nfinal correction pass...");
		printf("\n-----------------------------------\n");

		auto startTime = std::chrono::high_resolution_clock::now();

#ifndef USE_NO_GUIDANCE_RELAXATION
		if (STOPATLEVEL == 0) synth[s].correctionPasswdistguidanceV2(s, shiftx, shifty, false, 0.0, 4, synthdist[s], synthlabels[s], pyr[s], pyrdist[s], pyrlabels[s], guid[s], labels[s], gmask[s], weight, neighbor, 0.0, index, refinebin);
#else
		if ( STOPATLEVEL == 0 )
		{
			synth[s].correctionPasswdistguidanceV2(
								s, // pyramid level
								shiftx, shifty,
								false, // remove doubles
								indweight, // label error penalty
								4, // nb samples to search randomly around current candidate
								synthdist[s], synthlabels[s], // synthesized maps
								pyr[s], pyrdist[s], pyrlabels[s], // exemplar maps
								guid[s], labels[s], gmask[s], // guidance maps
								weight, // weight between color and distance
								neighbor, // neighbor size for MSE (during best candidate search)
								pow( strength, powr ), // guidance weight
								index, // index map (synthesized uv)
								refinebin );
			}
#endif
		
		this->clone(synth[s], 0, 0, synth[s].sizeX() - 1, synth[s].sizeY() - 1);

		// - timer
		auto endTime = std::chrono::high_resolution_clock::now();
		float elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
		std::cout << "\ntime: " << elapsedTime << " ms\n";

		offsets.reset(index.sizeX(), index.sizeY(), hvColRGB<unsigned char>());
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			//offsets.update(i, j, hvColRGB<unsigned char>((unsigned char)((double)index.get(i, j).X() / (double)pyr[s].sizeX() * 128.0 + 128.0),
			//	(unsigned char)((double)index.get(i, j).Y() / (double)pyr[s].sizeY() * 128.0 + 128.0), 0));
			hvVec2<int> npos = index.get(i, j);
			offsets.update(i, j, hvColRGB<unsigned char>((unsigned char)((double)npos.X() / (double)pyr[s].sizeX() * 128.0 + 128.0),
				(unsigned char)((double)npos.Y() / (double)pyr[s].sizeY() * 128.0 + 128.0), 0));
			if (npos.X() != oldindex.get(i, j).X() || npos.Y() != oldindex.get(i, j).Y()) prefine.update(i, j, hvColRGB<unsigned char>(255));
			else prefine.update(i, j, hvColRGB<unsigned char>(0));
		}
	}

#ifndef USE_NO_TEMPORARY_IMAGE_EXPORT
	sprintf(buff, "%s_semiproc_offsets.ppm", name);
	fd = fopen(buff, "wb");
	if (fd == 0) { perror("cannot load file:"); }
	offsets.savePPM(fd, 1);
	fclose(fd);

	sprintf(buff, "%s_semiproc_dist.ppm", name);
	fd = fopen(buff, "wb");
	if (fd == 0) { perror("cannot load file:"); }
	synthdist[s].savePPM(fd, 1);
	fclose(fd);

	hvPictRGB<unsigned char> labrgb(synthlabels[s], 1);
	sprintf(buff, "%s_semiproc_labels.ppm", name);
	fd = fopen(buff, "wb");
	if (fd == 0) { perror("cannot load file:"); }
	labrgb.savePPM(fd, 1);
	fclose(fd);

	sprintf(buff, "%s_semiproc_refine.ppm", name);
	fd = fopen(buff, "wb");
	if (fd == 0) { perror("cannot load file:"); }
	prefine.savePPM(fd, 1);
	fclose(fd);
#endif
}



	void upscaleJitterPass(const hvPictRGB<T> &example, int scale, int jitter, hvArray2<hvVec2<int> > &index)
	{
		this->reset(index.sizeX()*scale, index.sizeY()*scale, hvColRGB<T>());
		hvArray2<hvVec2<int> > newid(index.sizeX()*scale, index.sizeY()*scale, hvVec2<int>());
		int i, j, ii, jj;
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			for (ii = 0; ii < scale; ii++) for (jj = 0; jj < scale; jj++)
			{
				hvVec2<int> pos = index.get(i, j);
				int px = pos.X()*scale + ii, py = pos.Y()*scale + jj;
				px += (int)(((double)rand() / (double)RAND_MAX*2.0 - 1.0)*(double)(jitter));
				if (px < 0) px = 0; if (px >= example.sizeX()) px = example.sizeX() - 1;
				py += (int)(((double)rand() / (double)RAND_MAX*2.0 - 1.0)*(double)(jitter));
				if (py < 0) py = 0; if (py >= example.sizeY()) py = example.sizeY() - 1;
				if (i*scale + ii < this->sizeX() && j*scale + jj < this->sizeY())
				{
					this->update(i*scale + ii, j*scale + jj, example.get(px, py));
					newid.update(i*scale + ii, j*scale + jj, hvVec2<int>(px, py));
				}
			}
		}
		index.reset(newid.sizeX(), newid.sizeY(), hvVec2<int>());
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			index.update(i, j, newid.get(i, j));
		}
	}

	/******************************************************************************
	 * correctionPasswdistguidanceV2()
	 ******************************************************************************/
	void correctionPasswdistguidanceV2(
		int sscale, // pyramid level
		int shiftx, int shifty,
		bool wdoublon, // remove doubles (only at 2nd correction step)
		float indweight, // label weight (none at at 2nd correction step)
		int samples, // nb samples to search randomly around current candidate
		hvPictRGB< T >& synthdist, hvPict< unsigned char >& synthlabels, // synthesized distance and label maps
		const hvPictRGB< T >& example, const hvPictRGB< T >& distance, const hvPict< unsigned char >& exlabels, // exemplar color, distance and label maps
		const hvPictRGB< T >& guidance, const hvPict< unsigned char >& labels, const hvBitmap& mask, // guidance pptbf, labels and mask maps
		double weight, // weight between color and distance
		int neighbor, // neighbor size for MSE (during best candidate search)
		double strength, // guidance weight (distances and labels)
		hvArray2< hvVec2< int > >& index, // index map (synthesized uv)
		hvBitmap& refine )
	{
#ifdef USE_MULTITHREADED_SYNTHESIS
		MyThreadPool threadPool;
#endif

		// Min threshold error for guidance distance (i.e. PPTBF) to check whether or not to refine (per pixel)
		// - linear interpolation between [0.05;2.0] based on "strength" in [0.0;1.0]
		// - i.e: [min,max]=[0.05;2.0] is the threshold error on guidance distance
		const float MINTHRESH = 0.05 * strength + 0.2 * ( 1.0 - strength );
		printf( "\n[x] strength=%g, minthresh to change=%g, scale=%d, index=%d,%d\n", strength, MINTHRESH, sscale, shiftx, shifty );

		//strength = 0.0;
		//int nneigh = 3;
		//int nextx[] = { 0,2,0,2,1, 1, 0, 2, 1 };
		//int nexty[] = { 0,0,2,2,1, 0, 1, 1, 2 };
		int nneigh = 2;
		int nextx[] = { 0,1,0,1 };
		int nexty[] = { 0,1,1,0 };
		int count = 0;
		refine.reset(this->sizeX(), this->sizeY(), false);
		//hvPictRGB<unsigned char> newthis(this->sizeX(), this->sizeY(), hvColRGB<unsigned char>(0));
		//hvPictRGB<unsigned char> newdist(synthdist.sizeX(), synthdist.sizeY(), hvColRGB<unsigned char>(0));
		//hvArray2<hvVec2<int> > newindex(index.sizeX(), index.sizeY(), hvVec2<int>(0, 0));
		
		//---------------------------------------------------------------------
		// Pre-processing pass : check whether or not to refine (per pixel)
		//---------------------------------------------------------------------

		std::cout << "\npre-processing: check whether or not to refine" << std::endl;

		// - timer
		auto startTime = std::chrono::high_resolution_clock::now();

		// Iterate through the image and, for each pixel, check whether or not to refine
		// based on different criteria :
		// - not too far away for current spatial position
		// - in a region of guidance (mask)-
		// - and with an error lower than a given threshold for guidance maps (pptbf and labels)
		for ( int jj = 0; jj < this->sizeY(); jj++ ) for ( int ii = 0; ii < this->sizeX(); ii++ )
		{
			// Retrieve neighbor pixels in index map
			//int px= index.get(ii , jj ).X(); 
			//int py = index.get(ii, jj).Y();
			int ix[16], iy[16], px[16], py[16];
			ix[0] = index.get((ii - 1) >= 0 ? ii - 1 : 0, (jj - 1) >= 0 ? jj - 1 : 0).X(); iy[0] = index.get((ii - 1) >= 0 ? ii - 1 : 0, (jj - 1) >= 0 ? jj - 1 : 0).Y();
			ix[1] = index.get((ii + 2) >= index.sizeX() ? index.sizeX() - 1 : ii + 2, (jj - 1) >= 0 ? jj - 1 : 0).X(); iy[1] = index.get((ii + 2) >= index.sizeX() ? index.sizeX() - 1 : ii + 2, (jj - 1) >= 0 ? jj - 1 : 0).Y();
			ix[2] = index.get((ii - 1) >= 0 ? ii - 1 : 0, (jj + 2) >= index.sizeY() ? index.sizeY() - 1 : jj + 2).X(); iy[2] = index.get((ii - 1) >= 0 ? ii - 1 : 0, (jj + 2) >= index.sizeY() ? index.sizeY() - 1 : jj + 2).Y();
			ix[3] = index.get((ii + 2) >= index.sizeX() ? index.sizeX() - 1 : ii + 2, (jj + 2) >= index.sizeY() ? index.sizeY() - 1 : jj + 2).X(); iy[3] = index.get((ii + 2) >= index.sizeX() ? index.sizeX() - 1 : ii + 2, (jj + 2) >= index.sizeY() ? index.sizeY() - 1 : jj + 2).Y();
			px[0] = ii - 1 >= 0 ? ii - 1 : 0; py[0] = jj - 1 >= 0 ? jj - 1 : 0;
			px[1] = (ii + 2) >= this->sizeX() ? this->sizeX() - 1 : ii + 2; py[1] = jj - 1 >= 0 ? jj - 1 : 0;
			px[2] = ii - 1 >= 0 ? ii - 1 : 0; py[2] = (jj + 2) >= this->sizeY() ? this->sizeY() - 1 : jj + 2;
			px[3] = (ii + 2) >= this->sizeX() ? this->sizeX() - 1 : ii + 2; py[3] = (jj + 2) >= this->sizeY() ? this->sizeY() - 1 : jj + 2;
			
			//for (int ri = 0; ri < 3; ri++) for (int rj = 0; rj < 3; rj++)
			//{
			//	int pi = ii + ri -1; if (pi < 0) pi = 0;
			//	int pj = jj + rj -1; if (pj < 0) pj = 0;
			//	ix[ri + rj * 3] = index.get(pi % index.sizeX(), pj % index.sizeY()).X();
			//	iy[ri + rj * 3] = index.get(pi % index.sizeX(), pj % index.sizeY()).Y();
			//}
			
			//int avgx = 0; for (int ri=0; ri<9; ri++) avgx+=ix[ri] ;
			//int avgy = 0; for (int rj = 0; rj<9; rj++) avgy += iy[rj];
			//avgx /= 9; avgy /= 9;
			
			// Compute associated average spatial position in exemplar
			int avgx = (ix[0] + ix[1] + ix[2] + ix[3]) / 4;
			int avgy = (iy[0] + iy[1] + iy[2] + iy[3]) / 4;
			int maxx = 0, maxy = 0;
			for (int k = 0; k < 4; k++) {
				int deltax = ix[k] - avgx; if (deltax < 0) deltax = -deltax;
				int deltay = iy[k] - avgy; if (deltay < 0) deltay = -deltay;
				if (deltax > maxx) maxx = deltax;
				if (deltay > maxy) maxy = deltay;
			}
			
			// Check if not too far away from current spatial position
			bool prefine = true;
			if ( maxx <= 2 * neighbor && maxy <= 2 * neighbor )
			{
				// Check whether or not we are in the mask region
				if ( !mask.get( ii, jj ) )
				{
					prefine = false;
				}
				else
				{
					// If yes, compute error on guidance (pptbf + labels)
					bool var = false;
					int iix = index.get( ii, jj ).X();
					int iiy = index.get( ii, jj ).Y();
					
					// Check for guidance distance error (pptbf) (MSE L2)
					float errg = (float)abs( distance.get(iix, iiy).GREEN() - guidance.get(ii, jj).GREEN() ) / 255.0;
					float errr = (float)abs( distance.get(iix, iiy).RED() - guidance.get(ii, jj).RED() ) / 255.0;
					float errb = (float)abs( distance.get(iix, iiy).BLUE() - guidance.get(ii, jj).BLUE() ) / 255.0;
					if ( errg*errg + errr*errr + errb*errb > 3.0 * ( MINTHRESH * MINTHRESH ) )
					{
						var = true;
					}
					
					// Check for guidance label error
					if ( indweight > 0.0 && exlabels.get( iix, iiy ) != labels.get( ii, jj ) )
					{
						var = true;
					}

					//for (k = 0; k < 4; k++)
					//{
					//	float err = 0.0;
					//	float errg = (float)abs(distance.get(ix[k], iy[k]).GREEN() - guidance.get(px[k], py[k]).GREEN())/255.0;
					//	float errr = (float)abs(distance.get(ix[k], iy[k]).RED() - guidance.get(px[k], py[k]).RED()) / 255.0;
					//	float errb = (float)abs(distance.get(ix[k], iy[k]).BLUE() - guidance.get(px[k], py[k]).BLUE()) / 255.0;
					//	err += errg*errg + errr*errr + errb*errb;
					//	if (err > 3.0*(MINTHRESH*MINTHRESH)) var = true;
					//	if (indweight > 0.0 && exlabels.get(ix[k], iy[k]) != labels.get(px[k], py[k])) var = true;
					//	//if (distance.get(ix[4], iy[4]).squaredDifference(guidance.get(ii, jj)) > 10.0*10.0) var = true;
					//}
					if (!var) prefine = false;
				}
				if (!prefine) count++;
			}

			// Update refinement map
			refine.set( ii, jj, prefine );
		}

		// - timer
		auto endTime = std::chrono::high_resolution_clock::now();
		float elapsedTime = static_cast< float >( std::chrono::duration_cast< std::chrono::milliseconds >( endTime - startTime ).count() );
		std::cout << "time: " << elapsedTime << " ms\n";

		//---------------------------------------------------------------------
		// Refinement pass
		//---------------------------------------------------------------------

		std::cout << "\nrefinement pass..." << std::endl;

		// - timer
		startTime = std::chrono::high_resolution_clock::now();

		float nstrength = strength;
		for ( int k = 0; k < 1; k++ ) // number of refinement pass
		{
			// Correction subpasses : "jump" pixels because the pixels are corrected according to neighborhoods that are also changing
			// - we partition pass into a sequence of subpasses on subsets of nonadjacent pixels
			//for ( i = 0; i < nneigh; i++ )  for ( j = 0; j < nneigh; j++ )
			//for (i=0; i<nneigh*nneigh; i++)
			const int subpassCorrectionStep = 3; // because of the 5x5 neighborhood collision/overlapping during correction
			for ( int i = 0; i < subpassCorrectionStep; i++ )  for ( int j = 0; j < subpassCorrectionStep; j++ )
			{
#ifdef USE_MULTITHREADED_SYNTHESIS
				std::mutex mutex;
				int nextPixelToProcessX = i;
				int nextPixelToProcessY = j;
				bool isRunning = true;

				threadPool.AppendTask( [&](const MyThreadPool::ThreadData* thread )
					{
						while ( isRunning )
						{
							mutex.lock();
							int iii = nextPixelToProcessX;
							int jjj = nextPixelToProcessY;

							nextPixelToProcessX += subpassCorrectionStep;
							if ( nextPixelToProcessX >= this->sizeX() )
							{
								nextPixelToProcessX = i;
								nextPixelToProcessY += subpassCorrectionStep;
								isRunning = ( nextPixelToProcessY < this->sizeY() );
							}
							mutex.unlock();

							// Manage exeception / handle error
							if ( iii >= this->sizeX() || jjj >= this->sizeY() )
							{
								//printf( "\nOK, error handled ! :)" );
								continue;
							}

							// Retrieve current neighborhood in index map (5x5)
							int px, py;
							int ix[25], iy[25];
							for ( int ri = 0; ri < 5; ri++ )
								for ( int rj = 0; rj < 5; rj++ )
							{
								//ix[0] = index.get(iii, jjj).X(); iy[0] = index.get(iii, jjj).Y();
								//ix[1] = index.get((iii + 2) % index.sizeX(), jjj).X(); iy[1] = index.get((iii + 2) % index.sizeX(), jjj).Y();
								//ix[2] = index.get(iii, (jjj + 2) % index.sizeY()).X(); iy[2] = index.get(iii, (jjj + 2) % index.sizeY()).Y();
								//ix[3] = index.get((iii + 2) % index.sizeX(), (jjj + 2) % index.sizeY()).X(); iy[3] = index.get((iii + 2) % index.sizeX(), (jjj + 2) % index.sizeY()).Y();
								int pi = iii + ri - 2; if (pi < 0) pi = 0;
								int pj = jjj + rj - 2; if (pj < 0) pj = 0;

								ix[ ri + rj * 5 ] = index.get( pi >= index.sizeX() ? index.sizeX() - 1 : pi, pj >= index.sizeY() ? index.sizeY() - 1 : pj ).X();
								iy[ ri + rj * 5 ] = index.get( pi >= index.sizeX() ? index.sizeX() - 1 : pi, pj >= index.sizeY() ? index.sizeY() - 1 : pj ).Y();
							}
							
							//if (!refine) printf("No refine in %d,%d\n",iii, jjj);
							//this->refineBestNeighborMatch(example, iii, jjj, neighbor, ix, iy, 4, px, py);
							//this->refineBestNeighborMatchwdist(synthdist, example, distance, weight, iii, jjj, neighbor, ix, iy, 4, px, py);
						
							// Check if refinement is required
							if ( refine.get( iii, jjj ) )
							{
									// Refine candidate (look for a better one and store its exemplar's position in (px,py)
									this->refineBestNeighborMatchwdistguidanceV2(
										shiftx, shifty,
										indweight,
										samples,
										synthdist, synthlabels,
										example, distance, exlabels,
										guidance, labels, mask,
										weight,
										mask.get( iii, jjj ) ? nstrength : 0.0,
										iii, jjj,
										neighbor,
										ix, iy, 25,
										px, py, // returned best candidate position in exemplar
										ix, iy,	0 );

									// Update index map
									index.update( iii, jjj, hvVec2< int >( px, py ) );

									// Update synthesis (color, distance, labels...)
									this->update( iii, jjj, example.get( px, py ) );
									synthdist.update( iii, jjj, distance.get( px, py ) );
									synthlabels.update( iii, jjj, exlabels.get( px, py ) );
							}
							
							//newthis.update(iii, jjj, example.get(px, py));
							//newdist.update(iii, jjj, distance.get(px, py));
							//newindex.update(iii, jjj, hvVec2<int>(px, py));
						}
					});
#else
				// subpasses depending on size of neighborhood
				//for (ii = nextx[i]; ii < this->sizeX(); ii += nneigh) for (jj = nexty[i]; jj < this->sizeY(); jj += nneigh)
				//for ( ii = i; ii < this->sizeX(); ii += nneigh ) for ( jj = j; jj < this->sizeY(); jj += nneigh )
				for (int jj = j; jj < this->sizeY(); jj += subpassCorrectionStep)
					for ( int ii = i; ii < this->sizeX(); ii += subpassCorrectionStep )
				{
					int px, py;
					int ix[25], iy[25];
					for ( int ri = 0; ri < 5; ri++ ) for ( int rj = 0; rj < 5; rj++ )
					{
						//ix[0] = index.get(ii, jj).X(); iy[0] = index.get(ii, jj).Y();
						//ix[1] = index.get((ii + 2) % index.sizeX(), jj).X(); iy[1] = index.get((ii + 2) % index.sizeX(), jj).Y();
						//ix[2] = index.get(ii, (jj + 2) % index.sizeY()).X(); iy[2] = index.get(ii, (jj + 2) % index.sizeY()).Y();
						//ix[3] = index.get((ii + 2) % index.sizeX(), (jj + 2) % index.sizeY()).X(); iy[3] = index.get((ii + 2) % index.sizeX(), (jj + 2) % index.sizeY()).Y();
						int pi = ii + ri - 2; if (pi < 0) pi = 0;
						int pj = jj + rj - 2; if (pj < 0) pj = 0;
						ix[ri + rj * 5] = index.get( pi >= index.sizeX() ? index.sizeX() - 1 : pi, pj >= index.sizeY() ? index.sizeY() - 1 : pj ).X();
						iy[ri + rj * 5] = index.get( pi >= index.sizeX() ? index.sizeX() - 1 : pi, pj >= index.sizeY() ? index.sizeY() - 1 : pj ).Y();
					}
				
					//if (!refine) printf("No refine in %d,%d\n",ii, jj);
					//this->refineBestNeighborMatch(example, ii, jj, neighbor, ix, iy, 4, px, py);
					//this->refineBestNeighborMatchwdist(synthdist, example, distance, weight, ii, jj, neighbor, ix, iy, 4, px, py);
					
					if ( refine.get( ii, jj ) )
					{
						this->refineBestNeighborMatchwdistguidanceV2( shiftx, shifty, indweight, samples, synthdist, synthlabels, example, distance, exlabels, guidance, labels, mask,
							weight, mask.get(ii, jj) ? nstrength : 0.0, ii, jj, neighbor, ix, iy, 25, px, py, ix, iy, 0 );

						this->update(ii, jj, example.get(px, py));
						synthdist.update(ii, jj, distance.get(px, py));
						synthlabels.update(ii, jj, exlabels.get(px, py));
						
						index.update(ii, jj, hvVec2<int>(px, py));
					}
					
					//newthis.update(ii, jj, example.get(px, py));
					//newdist.update(ii, jj, distance.get(px, py));
					//newindex.update(ii, jj, hvVec2<int>(px, py));
				}
#endif
			}
			
			printf( "refined %d / %d pixels.\n", this->sizeX() * this->sizeX() - count, this->sizeX() * this->sizeX() );
			
			//this->clone(newthis, 0, 0, this->sizeX() - 1, this->sizeY() - 1);
			//synthdist.clone(newdist, 0, 0, newdist.sizeX() - 1, newdist.sizeY() - 1);
			//index.clone(newindex);
			
			if ( strength < 0.5f )
			{
				// after 2nd pass, no constrain to enforce local coherence
				nstrength = 0.0;
			}
		}

		// - timer
		endTime = std::chrono::high_resolution_clock::now();
		elapsedTime = static_cast< float >( std::chrono::duration_cast< std::chrono::milliseconds >( endTime - startTime ).count() );
		std::cout << "time: " << elapsedTime << " ms\n";

		//--------------------------------------
		// Post-processing pass : remove duplicates
		//--------------------------------------

		if ( wdoublon )
		{
			printf( "\nremove doubles...\n" );
			
			// - timer
			startTime = std::chrono::high_resolution_clock::now();

			count = 0;

			for (int jj = 4; jj < this->sizeY(); jj++)
				for ( int ii = 4; ii < this->sizeX(); ii++ )
			{
				bool doublon = false;

				int px, py;
				int ix[16], iy[16];
				for ( int rj = 0; rj < 4; rj++ )
					for (int ri = 0; ri < 4; ri++)
					{
						int pi = std::min(std::max(0, ii - ri), index.sizeX() - 1);
						int pj = std::min(std::max(0, jj - rj), index.sizeY() - 1);

						ix[ri + rj * 4] = index.get(pi, pj).X();
						iy[ri + rj * 4] = index.get(pi, pj).Y();

						if ( ( ri != 0 || rj != 0 ) && ix[ 0 ] == ix[ ri + rj * 4 ] && iy[ 0 ] == iy[ ri + rj * 4 ] )
							doublon = true;
					}
				
				if ( doublon )
				{
//					this->refineBestNeighborMatchwdistguidanceV2Parallel(shiftx, shifty, indweight, samples, synthdist, synthlabels, example, distance, exlabels, guidance, labels, mask,
//						weight, mask.get(ii, jj) ? nstrength : 0.0, ii, jj, neighbor, ix, iy, 16, px, py, ix, iy, 1, threadPool);
					this->refineBestNeighborMatchwdistguidanceV2(shiftx, shifty, indweight, samples, synthdist, synthlabels, example, distance, exlabels, guidance, labels, mask,
						weight, mask.get(ii, jj) ? nstrength : 0.0, ii, jj, neighbor, ix, iy, 16, px, py, ix, iy, 1);

					// Update index map
					index.update(ii, jj, hvVec2<int>(px, py));

					// Update synthesis (color, distance, labels...)
					this->update(ii, jj, example.get(px, py));
					synthdist.update(ii, jj, distance.get(px, py));
					synthlabels.update(ii, jj, exlabels.get(px, py));
					
					count++;
				}
			}

			printf( "changed %d / %d double pixels.\n", count, this->sizeX() * this->sizeX() );

			// - timer
			endTime = std::chrono::high_resolution_clock::now();
			elapsedTime = static_cast< float >( std::chrono::duration_cast< std::chrono::milliseconds >( endTime - startTime ).count() );
			std::cout << "time: " << elapsedTime << " ms\n";
		}
		
		//char buff[10];
		//fgets(buff, 2, stdin);
	}


	void upscaleJitterPass(const hvPictRGB<T> &cumtransfer, hvPictRGB<T> examples[], int scale, int jitter, hvArray2<hvVec3<int> > &index)
	{
		this->reset(index.sizeX()*scale, index.sizeY()*scale, hvColRGB<T>());
		hvArray2<hvVec3<int> > newid(index.sizeX()*scale, index.sizeY()*scale, hvVec3<int>());
		int i, j, ii, jj;
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			for (ii = 0; ii < scale; ii++) for (jj = 0; jj < scale; jj++)
			{
				hvVec3<int> pos = index.get(i, j);
				int k = pos.Z();
				int px = pos.X()*scale + ii, py = pos.Y()*scale + jj;
				px += (int)(((double)rand() / (double)RAND_MAX*2.0 - 1.0)*(double)(jitter));
				if (px < 0) px = 0; if (px >= examples[k].sizeX()) px = examples[k].sizeX() - 1;
				py += (int)(((double)rand() / (double)RAND_MAX*2.0 - 1.0)*(double)(jitter));
				if (py < 0) py = 0; if (py >= examples[k].sizeY()) py = examples[k].sizeY() - 1;
				if (i*scale + ii < this->sizeX() && j*scale + jj < this->sizeY())
				{
					/////////////////////////////////////////////////////////////////
					hvColRGB<double> cc = examples[k].getDoG(px, py, 2);
					this->update(i*scale + ii, j*scale + jj, examples[k].get(px, py));
					newid.update(i*scale + ii, j*scale + jj, hvVec3<int>(px, py, k));
				}
			}
		}
		index.reset( newid.sizeX(), newid.sizeY(), hvVec3<int>() );
		for (i = 0; i < index.sizeX(); i++) for (j = 0; j < index.sizeY(); j++)
		{
			index.update(i, j, newid.get(i, j));
		}
	}

	////////////////////////////////////////////////////////////
	hvColRGB<double> getDoG(int x, int y, int ss)
	{
		int i, j, nn=0;
		hvColRGB<double> cc(0.0);
		for (i = -ss; i <= ss; i++) for (j = -ss; j <= ss; j++)
		{
			if (x + i >= 0 && y + j >= 0 && x + i < this->sizeX() && y + j < this->sizeY())
			{
				cc += (hvColRGB<double>)this->get(x + i, y + j);
				nn++;
			}
		}
		cc.scale(1.0 / (double)nn);
		return (hvColRGB<double>)this->get(x, y) - cc;
	}

	template <class U> void blend(const hvPictRGB<T> &example, const hvPict<U> &alpha, U scal, double power, const hvBitmap &mask)
	{
		int i,j;
		for (i=0; i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			if (mask.get(i,j))
			{
				double coeff = pow((double)alpha.get(i,j)/(double)scal, power);
				hvColRGB<T> col = this->get(i,j);
				hvColRGB<T> colex = example.get(i,j);
				hvColRGB<T> colres; colres.blend(colex,col, coeff);
				this->update(i,j,colres);
			}
		}
	}
	template <class U> void blendRect(int px, int py, int x, int y, int sx, int sy, const hvPictRGB<T> &example, const hvPict<U> &alpha, U scal, double power, const hvBitmap &mask, bool mshift=true)
	{
		int i,j;
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
			if (mask.get((mshift?x:0)+i,(mshift?y:0)+j))
			{
				if (px+i>=0 && px+i<this->sizeX() && py+j>=0 && py+j<this->sizeY())
				{
					double coeff = pow((double)alpha.get((mshift?x:0)+i,(mshift?y:0)+j)/(double)scal, power);
					hvColRGB<T> col = this->get(px+i,py+j);
					hvColRGB<T> colex = example.get(x+i,y+j);
					hvColRGB<T> colres; colres.blend(colex,col, coeff);
					//if (coeff<1.0) colres=hvColRGB<T>(T(255),T(255),T(0));
					//colres = hvColRGB<T>(0);
					this->update(px+i,py+j,colres);
				}
			}
		}
	}
	template <class U> void shiftedblend(int dx, int dy, const hvPictRGB<T> &example, const hvPict<U> &alpha, U scal, double power, const hvBitmap &mask, hvBitmap &affected, hvPict<hvVec2<int> > *index = 0)
	{
		int i,j;
		for (i=0; i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			int x = i-dx; if (x<0) x += mask.sizeX(); else if (x>=mask.sizeX()) x -= mask.sizeX();
			int y = j-dy; if (y<0) y += mask.sizeY(); else if (y>=mask.sizeY()) y -= mask.sizeY();
			if (mask.get(x,y))
			{
				double coeff = pow((double)alpha.get(x,y)/(double)scal, power);
				hvColRGB<T> col = this->get(i,j);
				hvColRGB<T> colex = example.get(x,y);
				hvColRGB<T> colres; colres.blend(colex,col, coeff);
				this->update(i,j,colres);
				affected.set(i,j,true);
				if (index != 0) index->update(i,j,hvVec2<int>(x, y));
			}
		}
	}
	
	void seedfill(const hvColRGB<T> &col, int x, int y, hvBitmap &bm, hvVec2<int> &min, hvVec2<int> &max) const
	{
		if (x<0 || y<0 || x>= this->sizeX() || y>= this->sizeY()) return;
		if (!(this->get(x,y).equals(col))) return;
		if (bm.get(x,y)) return;
		bm.set(x,y,true);
		min.keepMin(min,hvVec2<int>(x,y));
		max.keepMax(max,hvVec2<int>(x,y));
		int i,a,b;
		for (i=x+1; i<this->sizeX() && (!bm.get(i,y)) && this->get(i,y).equals(col); i++) { bm.set(i,y, true); max.keepMax(max,hvVec2<int>(i,y)); }
		b=i-1;
		for (i=x-1; i>=0 && (!bm.get(i,y)) && this->get(i,y).equals(col); i--) { bm.set(i,y, true); min.keepMin(min,hvVec2<int>(i,y)); }
		a = i+1;
		for (i=a; i<=b; i++) 
		{ 
			seedfill(col,i,y-1,bm,min,max); 
			seedfill(col,i,y+1,bm,min,max); 
		} 
	}
	void seedfill(const hvColRGB<T> &col, int x, int y, hvBitmap &bm, hvVec2<int> &min, hvVec2<int> &max, std::vector<hvVec2<unsigned short> > &lpts) const
	{
		if (x<0 || y<0 || x>= this->sizeX() || y>= this->sizeY()) return;
		if (!(this->get(x,y).equals(col))) return;
		if (bm.get(x,y)) return;
		bm.set(x,y,true);
		lpts.push_back(hvVec2<unsigned short>((unsigned short)x,(unsigned short)y));
		min.keepMin(min,hvVec2<int>(x,y));
		max.keepMax(max,hvVec2<int>(x,y));
		int i,a,b;
		for (i=x+1; i<this->sizeX() && (!bm.get(i,y)) && this->get(i,y).equals(col); i++) { bm.set(i,y, true); max.keepMax(max,hvVec2<int>(i,y)); }
		b=i-1;
		for (i=x-1; i>=0 && (!bm.get(i,y)) && this->get(i,y).equals(col); i--) { bm.set(i,y, true); min.keepMin(min,hvVec2<int>(i,y)); }
		a = i+1;
		for (i=a; i<=b; i++) 
		{ 
			seedfill(col,i,y-1,bm,min,max); 
			seedfill(col,i,y+1,bm,min,max); 
		} 
	}
	void seedfilltorus(const hvColRGB<T> &col, int xx, int yy, hvBitmap &bm, hvVec2<int> &min, hvVec2<int> &max) const
	{
		//if (x<0 || y<0 || x>=sizeX() || y>=sizeY()) return;
		int x = xx; while (x<0) x+= this->sizeX(); while (x>= this->sizeX()) x-= this->sizeX();
		int y = yy; while (y<0) y+= this->sizeY(); while (y>= this->sizeY()) y-= this->sizeY();
		if (!(this->get(x,y).equals(col))) return;
		if (bm.get(x,y)) return;
		bm.set(x,y,true);
		min.keepMin(min,hvVec2<int>(xx,yy));
		max.keepMax(max,hvVec2<int>(xx,yy));
		int i,a,b;
		for (i=x+1; i<this->sizeX() && (!bm.get(i,y)) && this->get(i,y).equals(col); i++) { bm.set(i,y, true); max.keepMax(max,hvVec2<int>(i-x+xx,yy)); }
		b=i-1;
		for (i=x-1; i>=0 && (!bm.get(i,y)) && this->get(i,y).equals(col); i--) { bm.set(i,y, true); min.keepMin(min,hvVec2<int>(i-x+xx,yy)); }
		a = i+1;
		for (i=a; i<=b; i++) 
		{ 
			seedfill(col,i-x+xx,yy-1,bm,min,max); 
			seedfill(col,i-x+xx,yy+1,bm,min,max); 
		} 
	}	

	hvVec2<int> chooseMinSquareDiff(int bx, int by, int bsx, int bsy, int x,int y,int sx,int sy, const hvPictRGB<T> &inpict, hvColRGB<double> &minerr) 
	{
		int i,j;
		hvColRGB<double> res;
		double minv=0.0;
		hvVec2<int> minpos(bx,by);
		bool first=true;

		for (i=bx; i<bx+bsx; i+=2) for (j=by; j<by+bsy; j+=2)
		{
				//printf("mask at %d,%d, %s\n", cx-i+x,cy-j+y, mask.get(cx-i+x,cy-j+y)?"true":"flase");
				this->squareDifference(255.0,i,j,x,y,sx,sy,inpict,res,4);
				double vv = res.norm();
				if (first) { first=false; minv=vv; minpos=hvVec2<int>(i,j); minerr=res; }
				else if (vv<minv) { minv=vv; minpos=hvVec2<int>(i,j); minerr=res; }
		}
		return minpos;
	}
	hvVec2<int> chooseMinSquareDiffBorder(int bx, int by, int bsx, int bsy, int x, int y, int sx, int sy, const hvPictRGB<T> &inpict, hvColRGB<double> &minerr)
	{
		int i, j;
		hvColRGB<double> res;
		double minv = 0.0;
		hvVec2<int> minpos(bx, by);
		bool first = true;

		for (i = bx; i<bx + bsx; i += 2) for (j = by; j<by + bsy; j += 2)
		{
			//printf("mask at %d,%d, %s\n", cx-i+x,cy-j+y, mask.get(cx-i+x,cy-j+y)?"true":"flase");
			this->squareDifferenceBorder(255.0, i, j, x, y, sx, sy, inpict, res, 4);
			double vv = res.norm();
			if (first) { first = false; minv = vv; minpos = hvVec2<int>(i, j); minerr = res; }
			else if (vv<minv) { minv = vv; minpos = hvVec2<int>(i, j); minerr = res; }
		}
		return minpos;
	}

	hvVec2<int> chooseMinSquareDiff(int bx, int by, int bsx, int bsy, int x,int y,int sx,int sy, const hvPictRGB<T> &inpict, const hvBitmap &mask, int cx, int cy, hvColRGB<double> &minerr) 
	{
		int i,j;
		hvColRGB<double> res;
		bool first=true;
		double minv=0.0;
		hvVec2<int> minpos(bx,by);

		for (i=bx; i<bx+bsx; i+=2) for (j=by; j<by+bsy; j+=2)
		{
			//if (mask.get(cx-i+x,cy-j+y) && mask.get(cx-i+x-1,cy-j+y) && mask.get(cx-i+x+1,cy-j+y) && mask.get(cx-i+x,cy-j+y-1) && mask.get(cx-i+x,cy-j+y+1))
			if (mask.get(cx-i+x,cy-j+y)) 
			{
				//printf("mask at %d,%d, %s\n", cx-i+x,cy-j+y, mask.get(cx-i+x,cy-j+y)?"true":"flase");
				this->squareDifference(255.0,i,j,x,y,sx,sy,inpict,mask,res);
				double vv = res.norm();
				if (first) { first=false; minv=vv; minpos=hvVec2<int>(i,j); minerr=res; }
				else if (vv<minv) { minv=vv; minpos=hvVec2<int>(i,j); minerr=res; }
			}
		}
		if (first) { printf("warning cannot find best minsquare diff on %d,%d\n", cx,cy); }
		return minpos;
	}
	hvVec2<int> chooseWeightedMinSquareDiff(int bx, int by, int bsx, int bsy, int x,int y,int sx,int sy, const hvPictRGB<T> &inpict,  const hvBitmap &mask, const hvPict<double> &weight, int cx, int cy, hvColRGB<double> &minerr) 
	{
		int i,j;
		hvColRGB<double> res;
		bool first=true;
		double minv=0.0;
		hvVec2<int> minpos(bx,by);

		for (i=bx; i<bx+bsx; i+=2) for (j=by; j<by+bsy; j+=2)
		{
			//if (mask.get(cx-i+x,cy-j+y) && mask.get(cx-i+x-1,cy-j+y) && mask.get(cx-i+x+1,cy-j+y) && mask.get(cx-i+x,cy-j+y-1) && mask.get(cx-i+x,cy-j+y+1))
			if (mask.get(cx-i+x,cy-j+y))
			{
				//printf("mask at %d,%d, %s\n", cx-i+x,cy-j+y, mask.get(cx-i+x,cy-j+y)?"true":"flase");
				this->weightedSquareDifference(255.0,i,j,x,y,sx,sy,inpict,weight,res);
				double vv = res.norm();
				if (first) { first=false; minv=vv; minpos=hvVec2<int>(i,j); minerr=res; }
				else if (vv<minv) { minv=vv; minpos=hvVec2<int>(i,j); minerr=res; }
			}
		}
		if (first) { printf("warning cannot find best minsquare diff on %d,%d\n", cx,cy); }
		return minpos;
	}
	

};

	


////////////////////////////////////////////////////////////
template <class T> class hvPictRGBA : public hvField2< hvColRGBA<T> >  
////////////////////////////////////////////////////////////
{
public:
	hvPictRGBA<T>() : hvField2< hvColRGBA<T> >() { }
	hvPictRGBA<T>(int sx, int sy, const hvColRGBA<T> &nil) : hvField2< hvColRGBA<T> >(sx, sy, nil),hvArray2< hvColRGBA<T> >(sx, sy, nil) { }
	
	void clone(const hvPictRGBA<T> &pict,int x, int y, int sx, int sy)
	{
		hvField2< hvColRGBA<T> >::reset(sx-x+1, sy-y+1, hvColRGBA<T>(0));
		int i,j;
		for (i=x; i<=sx; i++) for (j=y; j<=sy; j++)
		{
			this->update(i-x,j-y,pict.get(i,j));
		}
	}
	void clone(const hvPictRGB<T> &pict,const hvPict<T> &pa, int x, int y, int sx, int sy)
	{
		hvField2< hvColRGBA<T> >::reset(sx-x+1, sy-y+1, hvColRGBA<T>(0));
		int i,j;
		for (i=x; i<=sx; i++) for (j=y; j<=sy; j++)
		{
			hvColRGB<T> col = pict.get(i,j);
			hvColRGBA<T> cc(col, pa.get(i,j));
			//printf("clone %d,%d -> %d,%d,%d,%d\n", i,j,(int)cc.RED(), (int)cc.GREEN(), (int)cc.BLUE(), (int)cc.ALPHA());
			this->update(i-x,j-y,cc);
		}
	}
	void copy(int x, int y, const hvPictRGBA<T> &pict)
	{
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			this->update(x+i,y+j,pict.get(i,j));
		}
	}
	void copyRect(int px, int py, int x, int y, int sx, int sy, const hvPictRGBA<T> &pict, const hvBitmap &mask)
	{
		int i,j;
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
			if (mask.get(x+i, y+j))
			{
				this->update(px+i,py+j,pict.get(x+i,y+j));
			}
		}
	}

	void gamma(T scal, double power)
	{
		int i,j;
		for (i=0;i<this->sizeX(); i++) for (j=0; j<this->sizeY(); j++)
		{
			hvColRGB<T> v = this->get(i,j);
			v.gamma(scal, power);
			this->update(i,j,v);
		}
	}
	void convert(const hvBitmap &pict, const hvColRGBA<T> &va, const hvColRGBA<T> &vb)
	{
		hvField2< hvColRGBA<T> >::reset(pict.sizeX(), pict.sizeY(), hvColRGBA<T>(0));
		//hvArray2< hvColRGB<T> >::reset(pict.sizeX(), pict.sizeY(), hvColRGB<T>(0));
		int i,j;
		for (i=0; i<pict.sizeX(); i++) for (j=0; j<pict.sizeY(); j++)
		{
			if (pict.get(i,j)) this->update(i,j,va); else this->update(i,j,vb);
		}
	}
	void loadPPM(FILE *fd, T norm, T alpha)
	{
		int  sx, sy;
		int i,j, type;
		char buff[256];
		hvColRGB<T> co;

		hvPictRGB<T>::readPPMLine(fd,buff);
		if (strcmp(buff,"P6\n")==0) type = 0;
		else if (strcmp(buff,"P3\n")==0) type = 1;
		else { type = 2; printf("unknown picture PPM type=%d (%s)\n", type,buff); } 
		hvPictRGB<T>::readPPMLine(fd,buff);
		sscanf(buff,"%d %d",&sx,&sy);
		hvPictRGB<T>::readPPMLine(fd,buff);
		if (strcmp(buff,"255\n")!=0){ printf("type=%d\n", type); hvFatal("Not the right PPM Format"); }
		this->reset(sx, sy, hvColRGBA<T>());
		for (i=0; i<sy; i++)
		for (j=0; j<sx; j++)
			{
				unsigned char r,g,b;
				if (type==0)
				{
					fread(&r,1,sizeof(unsigned char),fd);
					fread(&g,1,sizeof(unsigned char),fd);
					fread(&b,1,sizeof(unsigned char),fd);
				}
				else if (type==1)
				{
					int rr, gg, bb;
					fscanf(fd, "%d %d %d", &rr, &gg, &bb);
					r= (unsigned char)rr;
					g= (unsigned char)gg;
					b= (unsigned char)bb;
				}
				else { r=0; g=0; b=0; }
				hvArray2< hvColRGBA<T> >::update(j,sy-i-1,hvColRGBA<T>((T)r/norm, (T)g/norm, (T)b/norm, alpha));
			}
	}
	void loadPPMA(FILE *fd, T norm)
	{
		int  sx, sy;
		int i,j, type;
		char buff[256];
		hvColRGB<T> co;

		hvPictRGB<T>::readPPMLine(fd,buff);
		if (strcmp(buff,"P6\n")==0) type = 0;
		else if (strcmp(buff,"P3\n")==0) type = 1;
		else { type = 2; printf("unknown picture PPM type=%d (%s)\n", type,buff); } 
		hvPictRGB<T>::readPPMLine(fd,buff);
		sscanf(buff,"%d %d",&sx,&sy);
		hvPictRGB<T>::readPPMLine(fd,buff);
		if (strcmp(buff,"255\n")!=0){ printf("type=%d\n", type); hvFatal("Not the right PPM Format"); }
		this->reset(sx, sy, hvColRGBA<T>());
		for (i=0; i<sy; i++)
		for (j=0; j<sx; j++)
			{
				unsigned char r,g,b,aa;
				if (type==0)
				{
					fread(&r,1,sizeof(unsigned char),fd);
					fread(&g,1,sizeof(unsigned char),fd);
					fread(&b,1,sizeof(unsigned char),fd);
					fread(&aa,1,sizeof(unsigned char),fd);
				}
				else if (type==1)
				{
					int rr, gg, bb, alpha;
					fscanf(fd, "%d %d %d %d", &rr, &gg, &bb, &alpha);
					r= (unsigned char)rr;
					g= (unsigned char)gg;
					b= (unsigned char)bb;
					aa = (unsigned char)alpha;
				}
				else { r=0; g=0; b=0; aa=0; }
				hvArray2< hvColRGBA<T> >::update(j,sy-i-1,hvColRGBA<T>((T)r/norm, (T)g/norm, (T)b/norm, (T)aa/norm));
			}
	}
	void savePPM(FILE *fd, T norm, bool alpha=false)
	{
		int i,j;
		hvColRGBA<T> co;
		unsigned char v;

		fprintf(fd,"P6\n");
		fprintf(fd,"%d %d\n", this->sizeX(), this->sizeY());
		fprintf(fd,"255\n");
		for (i=0; i<this->sizeY(); i++)
		for (j=0; j<this->sizeX(); j++)
			{
			co = hvArray2< hvColRGBA<T> >::get(j, this->sizeY()-i-1);
			v = (unsigned char)((T)(alpha?co.ALPHA():co.RED())*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)(alpha?co.ALPHA():co.GREEN())*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)(alpha?co.ALPHA():co.BLUE())*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			}
	}
	void savePPMA(FILE *fd, T norm)
	{
		int i,j;
		hvColRGBA<T> co;
		unsigned char v;

		fprintf(fd,"P6\n");
		fprintf(fd,"%d %d\n", this->sizeX(), this->sizeY());
		fprintf(fd,"255\n");
		for (i=0; i<this->sizeY(); i++)
		for (j=0; j<this->sizeX(); j++)
			{
			co = hvArray2< hvColRGBA<T> >::get(j, this->sizeY()-i-1);
			v = (unsigned char)((T)co.RED()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)co.GREEN()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)co.BLUE()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);
			v = (unsigned char)((T)co.ALPHA()*norm);
			fwrite(&v,1,sizeof(unsigned char),fd);			}
	}
};




}

#endif // !efined(AFX_PICTRGB_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
