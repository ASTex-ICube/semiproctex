// hvField2.h: interface for the scalar field in 1D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELD2D_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
#define AFX_FIELD2D_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////
/*
Field1<X>: is a regular grid of dimension N=1 where each grid point contains elements of type X (scalars or vectors).
X : is either a vector : hvVec3<U> or a scalar 
*/ 
////////////////////////////////////////////
////////////////////////////////////////////

#include "hvField1.h"
#include "hvArray2.h"
#include "hvList.h"

namespace hview {

template <class X> class hvField2 : public virtual hvField<X>, public virtual hvArray2<X>  
{
public:
	hvField2<X>(int sx, int sy, X nil):hvField<X>(nil),hvArray2<X>(sx,sy,nil) { hvField<X>::loop[0]=hvField<X>::CLAMP; hvField<X>::loop[1]=hvField<X>::CLAMP; }
	hvField2<X>():hvField<X>(),hvArray2<X>() { hvField<X>::loop[0]=hvField<X>::CLAMP; hvField<X>::loop[1]=hvField<X>::CLAMP; }

	virtual int	size(int dim) const
	{
		if (dim!=0 && dim!=1) hvFatal("dim must be 0 or 1 in hvField2::size(int dim)");
		if (dim==0) return hvArray2<X>::sizeX();
		else return hvArray2<X>::sizeY();
	}
	virtual int dimension() const { return 2; }
	virtual X operator()(int ind[]) const 
	{ 
		int i,j;
		if (ind[0]<0 || ind[0]>=this->sizeX()) 
		{
			if (hvField<X>::loop[0]==hvField<X>::CLAMP) return hvField<X>::clampvalue;
			i = hvField<X>::index(ind[0], hvField<X>::loop[0],this->sizeX());
		}
		else i=ind[0];
		if (ind[1]<0 || ind[1]>=this->sizeY()) 
		{
			if (hvField<X>::loop[1]==hvField<X>::CLAMP) return hvField<X>::clampvalue;
			j = hvField<X>::index(ind[1], hvField<X>::loop[1],this->sizeY());
		}
		else j=ind[1];
		return hvArray2<X>::get(i,j); 
	}
	virtual X get(int ind[]) const 
	{ 
		int i,j;
		if (ind[0]<0 || ind[0]>=this->sizeX()) 
		{
			if (hvField<X>::loop[0]==hvField<X>::CLAMP) return hvField<X>::clampvalue;
			i = hvField<X>::index(ind[0], hvField<X>::loop[0],this->sizeX());
		}
		else i=ind[0];
		if (ind[1]<0 || ind[1]>=this->sizeY()) 
		{
			if (hvField<X>::loop[1]==hvField<X>::CLAMP) return hvField<X>::clampvalue;
			j = hvField<X>::index(ind[1], hvField<X>::loop[1],this->sizeY());
		}
		else j=ind[1];
		return hvArray2<X>::get(i,j); 
	}
	void update(int ind[], X val) 
	{ 
		//if (ind[0]<0 || ind[0]>=this->sizeX()) return; 
		//if (ind[1]<0 || ind[1]>=this->sizeY()) return; 
		assert(!(ind[0] < 0 || ind[0] >= this->sizeX()));
		assert(!(ind[1] < 0 || ind[1] >= this->sizeY()));
		hvArray2<X>::update(ind[0], ind[1], val);
	}
	X get(int x, int y) const { return hvArray2<X>::get(x,y); }
	void update(int x, int y, X val) { hvArray2<X>::update(x,y,val); }

	virtual void clear(X k)
	{
		for (int i=0; i<this->sizeX()*this->sizeY(); i++) hvArray2<X>::t[i] = k;
	}
	virtual void scale(int k)
	{
		for (int i=0; i<this->sizeX()*this->sizeY(); i++) { hvArray2<X>::t[i] *= k; }
	}
	virtual void scale(float k)
	{
		for (int i=0; i<this->sizeX()*this->sizeY(); i++) { hvArray2<X>::t[i] *= k; }
	}
	virtual void scale(double k)
	{
		for (int i=0; i<this->sizeX()*this->sizeY(); i++) { hvArray2<X>::t[i] *= k; }
	}
	virtual void add(hvField<X> *f)
	{
		int ind[2];
		if (f->dimension()!=2 || this->sizeX()!=f->size(0) || this->sizeY()!=f->size(1)) { hvFatal("cannot add in hvField2"); return; }
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++)
		{
			ind[0]=i; ind[1]=j;
			X val = hvArray2<X>::get(i,j);
			val += f->get(ind);
			hvArray2<X>::update(i,j,val);
		}
	}

	virtual void clone(hvField<X> *f)
	{
		int ind[2];
		if (f->dimension()!=2) { hvFatal("cannot clone in hvField2"); return; }
		if ( this->sizeX()!=f->size(0) || this->sizeY()!=f->size(1) ) this->reset(f->size(0),f->size(1));
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		hvField<X>::loop[1]=f->getLoop(1);
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++)
		{
			ind[0]=i; ind[1]=j;
			hvArray2<X>::update(i,j,f->get(ind));
		}
	}

	virtual void shrink(hvField<X> *f)
	{
		int ind[2];
		if (f->dimension()!=2) { hvFatal("cannot shrink hvField2"); return; }
		this->reset(f->size(0)/2, f->size(1)/2);
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		hvField<X>::loop[1]=f->getLoop(1);
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++)
		{
			ind[0]=2*i; ind[1]=2*j;
			X cste(4);
			X sum = f->get(ind); sum /= cste;
			ind[0]=2*i+1; ind[1]=2*j;
			X val = f->get(ind); val /= cste; sum += val; 
			ind[0]=2*i; ind[1]=2*j+1;
			val = f->get(ind); val /= cste; sum += val; 
			ind[0]=2*i+1; ind[1]=2*j+1;
			val = f->get(ind); val /= cste; sum += val; 
			hvArray2<X>::update(i,j,sum);
		}
	}
	virtual void enlarge(hvField<X> *f)
	{
		int ind[2];
		if (f->dimension()!=2) { hvFatal("cannot enlarge hvField2"); return; }
		this->reset(f->size(0)*2, f->size(1)*2);
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		hvField<X>::loop[1]=f->getLoop(1);
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++)
		{
			ind[0]=i/2; ind[1]=j/2;
			X cste(4);
			X sum = f->get(ind); sum /= cste;
			ind[0]=i/2+(i%2); ind[1]=j/2;
			X val = f->get(ind); val /= cste; sum += val; 
			ind[0]=i/2; ind[1]=j/2+(j%2);
			val = f->get(ind); val /= cste; sum += val; 
			ind[0]=i/2+(i%2); ind[1]=j/2+(j%2);
			val = f->get(ind); val /= cste; sum += val; 
			hvArray2<X>::update(i,j,sum);
		}
	}
	template <class T> void deblur(hvField2<X> *f, int niter, int sx, int sy, double scal, double min, double max)
	{
		int k, ind[2];
		hvField2<X> *fs = new hvField2<X>(); 
		this->clone(f);
		for (k=0; k<niter; k++)
		{
			fs->deltaGaussianBlur(this,sx,sy);
			for (int i=0; i<this->sizeX(); i++) 
			for (int j=0; j<this->sizeY(); j++)
			{
				ind[0]=i; ind[1]=j;
				T sum = T(this->get(ind));  sum /= T(scal);
				T diff = T(fs->get(ind)); diff /= T(scal);
				diff -= sum;
				diff.clamp(min,max); diff *= T(0.2);
				sum = T(this->get(ind)); sum /= T(scal); sum -= diff; sum.clamp(min,max);
				sum *= T(scal);
				hvArray2<X>::update(i,j,X(sum));
			}
		}
	}
	void deblurs(hvField2<X> *f, int niter, int sx, int sy, double scal, double min, double max)
	{
		int k, ind[2];
		hvField2<X> *fs = new hvField2<X>(); 
		this->clone(f);
		for (k=0; k<niter; k++)
		{
			fs->deltaGaussianBlur(this,sx,sy);
			for (int i=0; i<this->sizeX(); i++) 
			for (int j=0; j<this->sizeY(); j++)
			{
				ind[0]=i; ind[1]=j;
				double sum = double(this->get(ind));  sum /= scal;
				double diff = double(fs->get(ind)); diff /= scal;
				diff -= sum; if (diff<min) diff=min; else if (diff>max) diff=max;
				diff *= 0.2;
				sum = double(this->get(ind)); sum /= scal; sum -= diff; 
				if (sum<min) sum=min; else if (sum>max) sum=max;
				sum *= scal;
				hvArray2<X>::update(i,j,X(sum));
			}
		}
	}
	// applies a rectangular blur mask containing values val
	template <class U> void blur(hvField<X> *f,int sx, int sy, U val)
	{
		hvField2<U> mm(sx, sy, val);
		this->convolve(f, &mm);
	}
	// applies a Gaussian blur mask
	template <class U> void gaussianBlur(hvField<X> *f,int sx, int sy, double sigma=2.0)
	{
		int i,j;
		double no=0.0;
		hvField2<double> mm(sx, sy, 1.0);
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
			double dx = ((double)i-(double)sx*0.5)*2.0/(double)sx;
			double dy = ((double)j-(double)sy*0.5)*2.0/(double)sy;
			double vv = exp(-sigma*(dx*dx+dy*dy));
			mm.update(i,j,vv);
			no+=vv;
		}
		hvField2<U> mmy(sx, sy, U(1.0));
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
			mmy.update(i,j,U(mm.get(i,j)/no));
		}
		this->convolve(f, &mmy);
	}


	// applies a (Gaussian - dirac) filter, sx and sy must be odd numbers like 3, 5, 7 etc.
	template <class U>  void deltaGaussianBlur(hvField<X> *f,int sx, int sy)
	{
		int i,j;
		double no=0.0;
		hvField2<double> mm(sx, sy, 1.0);
		for (i=-sx/2; i<=sx/2; i++) for (j=-sy/2; j<=sy/2; j++)
		{
			double dx = (double)i*2.0/(double)sx;
			double dy = (double)j*2.0/(double)sy;
			double vv = 1.0;
			if (i==0 && j==0) vv=0.0;
			mm.update(i+sx/2,j+sy/2,vv);
			no+=vv;
		}
		hvField2<U> mmy(sx, sy, U(1.0));
		for (i=0; i<sx; i++) for (j=0; j<sy; j++)
		{
			mmy.update(i,j,U(mm.get(i,j)/no));
		}
		this->convolve(f, &mmy);
	}
	// applies a Gaussian blur mask
	template <class U> void gaborFilter(hvField<X> *f,int sx, int sy, double theta, double freq, double sigma, double phase)
	{
		int i,j;
		double sno, no=0.0;
		hvField2<double> mm(2*sx+1, 2*sy+1, 1.0);
		for (i=0; i<2*sx+1; i++) for (j=0; j<2*sy+1; j++)
		{
			double dx = ((double)i-(double)sx);
			double dy = ((double)j-(double)sy);
			double vv = 1.0/(2.0*M_PI*sigma*sigma)*exp(-(dx*dx+dy*dy)/2.0/(double)sigma/(double)sigma)*cos(2.0*M_PI*freq*(dx*cos(theta)+dy*sin(theta))+phase);
			mm.update(i,j,vv);
			no+=(vv>0.0?vv:-vv);
		}
		sno=no;
		if (no==0.0) no=1.0;
		//no = 1.0;
		hvField2<U> mmy(2*sx+1, 2*sy+1, U(1.0));
		//printf("Gabor filter:\n");
		for (i=0; i<2*sx+1; i++) for (j=0; j<2*sy+1; j++)
		{
			//printf("%3d:%4.2f ", j, 100.0*mm.get(i,j)/no);
			//if (j==2*sy) printf("\n");
			mmy.update(i,j,U(mm.get(i,j)/no));
		}
		//printf("somme:%g\n", sno);
		this->convolve(f, &mmy);
	}

	template <class U> void anisoGaborFilter(hvField<X> *f,int sx, int sy, double alpha, double theta, double freq, double sigmax, double sigmay, double phase)
	{
		int i,j;
		double sno, no=0.0;
		hvField2<double> mm(2*sx+1, 2*sy+1, 1.0);
		for (i=0; i<2*sx+1; i++) for (j=0; j<2*sy+1; j++)
		{
			double dx = ((double)i-(double)sx);
			double dy = ((double)j-(double)sy);
			double vv = 1.0/(2.0*M_PI*sigmax*sigmay)*exp(-(dx*dx/(double)sigmax/(double)sigmax+dy*dy/(double)sigmay/(double)sigmay)/2.0)*cos(2.0*M_PI*freq*(dx*cos(theta)+dy*sin(theta))+phase);
			mm.update(i,j,vv);
			no+=abs(vv);
		}
		sno=no;
		if (no==0.0) no=1.0;
		//no = 1.0;
		hvField2<U> mmy(2*sx+1, 2*sy+1, U(1.0));
		//printf("Gabor filter:\n");
		for (i=0; i<2*sx+1; i++) for (j=0; j<2*sy+1; j++)
		{
			//printf("%3d:%4.2f ", j, 100.0*mm.get(i,j)/no);
			//if (j==2*sy) printf("\n");
			mmy.update(i,j,U(mm.get(i,j)/no));
		}
		//printf("somme:%g\n", sno);
		this->convolveRotation(f, &mmy, alpha);
	}

	// applies a median filter: assumes X can be converted into a "double" scalar
	void median(hvField<X> *f,int ss)
	{
		int ind[2];
		hvSortedList<double> li((2*ss+1)*(2*ss+1));
		std::vector<X> liv((2*ss+1)*(2*ss+1));
		this->reset(f->size(0),f->size(1));
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++) 
		{ 
			int ii,jj;
			li.clear(); liv.clear();
			for (ii=-ss; ii<=ss; ii++) for (jj=-ss; jj<=ss; jj++)
			{
				ind[0]=i+ii; ind[1]=j+jj;
				if (i+ii>=0 && i+ii<f->size(0) && j+jj>=0 && j+jj<f->size(1))
				{
					X val = f->get(ind);
					double tt = double(val);
					int pos = li.pushSorted(tt);
					liv.insert(val, liv.begin()+pos);
				}
			}
			this->update(i,j,liv.at(liv.size()/2));
		}
	}

	template <class Y> void convolve(hvField<X> *f, hvField<Y> *mask)
	{
		int ind[2];
		if (mask->dimension()!=2) { hvFatal("cannot convolve in hvField2"); return; }
		if (f->dimension()!=2) { hvFatal("cannot convolve in hvField2"); return; }
		if ( this->sizeX()!=f->size(0) || this->sizeY()!=f->size(1) ) this->reset(f->size(0),f->size(1));
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		hvField<X>::loop[1]=f->getLoop(1);
		int sx = mask->size(0)/2;
		int sy = mask->size(1)/2;
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++) 
		{ 
			ind[0] = i-sx, ind[1]= j-sy;
			int indj[2]; indj[0]=0; indj[1]=0;
			Y vv(0);
			for (int k=0; k<mask->size(0); k++) 
			for (int l=0; l<mask->size(1); l++) 
			{
				ind[0] = i+k-sx; ind[1] = j+l-sy;
				indj[0]=k; indj[1]=l;
				Y aa=Y(f->get(ind));
				Y bb = mask->get(indj);
				aa *= bb;
				vv += aa;
			}
			hvArray2<X>::update(i,j, X(vv));
		}
		
	}
	template <class Y> void convolveRotation(hvField<X> *f, hvField<Y> *mask, double alpha)
	{
		int ind[2];
		if (mask->dimension()!=2) { hvFatal("cannot convolve in hvField2"); return; }
		if (f->dimension()!=2) { hvFatal("cannot convolve in hvField2"); return; }
		if (this->sizeX()!=f->size(0) || this->sizeY()!=f->size(1) ) this->reset(f->size(0),f->size(1));
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		hvField<X>::loop[1]=f->getLoop(1);
		int sx = mask->size(0)/2;
		int sy = mask->size(1)/2;
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++) 
		{ 
			ind[0] = i-sx, ind[1]= j-sy;
			int indj[2]; indj[0]=0; indj[1]=0;
			Y vv(0);
			for (int k=0; k<mask->size(0); k++) 
			for (int l=0; l<mask->size(1); l++) 
			{
				ind[0] = i+(int)((double)(k-sx)*cos(alpha)+(double)(l-sy)*sin(alpha)); ind[1] = j+(int)((double)(-k+sx)*sin(alpha)+(double)(l-sy)*cos(alpha));
				indj[0]=k; indj[1]=l;
				Y aa=Y(f->get(ind));
				Y bb = mask->get(indj);
				aa *= bb;
				vv += aa;
			}
			hvArray2<X>::update(i,j, X(vv));
		}
		
	}
	
	template <class Y>  void adaptiveGaussian(hvField<X> *f, Y powx, Y powy, Y factorx, Y factory, bool centered)
	{
		int ind[2];
		if (f->dimension()!=2) { hvFatal("cannot convolve in hvField2"); return; }
		for (int i=0; i<this->sizeX(); i++) 
		for (int j=0; j<this->sizeY(); j++) 
		{ 
			Y fx = Y(centered ? (i- this->sizeX()/2<0? this->sizeX()/2-i:i- this->sizeX()/2) : (i<this->sizeX()/2?i: this->sizeX()-i))/Y(this->sizeX())*Y(2);
			Y fy = Y(centered ? (j- this->sizeY()/2<0? this->sizeY()/2-j:j- this->sizeY()/2) : (j<this->sizeY()/2?j: this->sizeY()-j))/Y(this->sizeY())*Y(2);
			int sx = int(pow(fx,powx)*factorx);
			if (sx==0) sx=1;
			int sy = int(pow(fy,powy)*factory);
			if (sy==0) sy=1;
			Y vv(0);
			for (int k=0; k<sx; k++) 
			for (int l=0; l<sy; l++) 
			{
				ind[0] = i+k-sx/2; if (ind[0]<0) ind[0] += this->sizeX(); else if (ind[0]>= this->sizeX()) ind[0] -= this->sizeX();
				ind[1] = j+l-sy/2; if (ind[1]<0) ind[1] += this->sizeY(); else if (ind[1]>= this->sizeY()) ind[1] -= this->sizeY();
				vv += f->get(ind);
			}
			vv /= Y(sx*sy);
			hvArray2<X>::update(i,j, X(vv));
		}
		
	}
	
	virtual void derivative(hvField<X> *f, int dim)
	{
		if (dim!=0 && dim!=1) hvFatal("dim must be 0 or 1 in hvField2::derivative(...)");
		if (f->dimension()!=2) { hvFatal("cannot compute gradient in hvField2"); return; }
		if (this->sizeX()!=f->size(0) || this->sizeY()!=f->size(1) ) this->reset(f->size(0),f->size(1));
		for (int i=0; i<this->sizeX(); i++)
		for (int j=0; j<this->sizeY(); j++)
		{ 
			int ind[2]; ind[0]=i; ind[1]=j;
			if (dim==0) ind[0]=i+1; else ind[1]=j+1;
			X val = f->get(ind);
			ind[0]=i; ind[1]=j;
			val -= f->get(ind); 
			hvArray2<X>::update(i,j,val);
		}

	}

	void pow2(int &pow_2, int &nn) const
	{
		pow_2=1; nn=2;
		while(nn<this->sizeX() || nn<this->sizeY()) { pow_2=pow_2+1; nn *= 2; }
	}

	template <class Y> hvArray1<hvPair<Y,Y> > *fft(bool centred, Y scal, Y offset, int pow_2=0) const
	{ 
		int i,j,nn;
		X rr;
		if (pow_2==0) { pow_2=1, nn=2; pow2(pow_2,nn); }
		else nn=1<<pow_2;
		if (nn>this->sizeX() || nn>this->sizeY()) { std::cout<<"hvField2 too small for fft "<< this->sizeX()<<","<< this->sizeY()<<","<< nn<<"\n"; hvFatal("take smaller pow2"); }
		hvArray1<hvPair<Y,Y> > *ft = new hvArray1<hvPair<Y,Y> >(nn*nn, hvPair<Y,Y>(Y(0),Y(0)));
		for (i=0; i<nn; i++)
		for (j=0; j<nn; j++)
			{
			if (i<this->sizeX()&&j<this->sizeY()) rr = get(i,j); else rr = X(0);
			Y xx = (Y(rr)-offset) / scal;
			ft->update(i+j*nn, hvPair<Y,Y>((centred && (i+j)%2==1) ? Y(0)-xx : xx,Y(0)));
			}
		for (i=0; i<nn; i++) hvArray1<Y>::fft(*ft, pow_2,1,i*nn,false);
		for (i=0; i<nn; i++) hvArray1<Y>::fft(*ft, pow_2,nn,i,false);
		for (i=0; i<nn*nn; i++) ft->update(i, hvPair<Y,Y>(ft->get(i).getLeft()/(Y)(nn*nn),ft->get(i).getRight()/(Y)(nn*nn)) );
		return ft;
	}
	template <class Y> void inversefft(hvArray1<hvPair<Y,Y> > *ft, Y scal, Y offset, int pow_2=0)
	{ 
		int i,j,nn;
		Y min, max;
		if (pow_2==0) { pow_2=1, nn=2; pow2(pow_2,nn); }
		else nn=1<<pow_2;
		if (nn>this->sizeX() || nn>this->sizeY()) { std::cout << "hvField2 too small for fft " << this->sizeX() << "," << this->sizeY() << "," << nn << "\n"; hvFatal("take smaller pow2"); }
		for (i=0; i<nn; i++)
		for (j=0; j<nn; j++)
			{
			hvPair<Y,Y> c = ft->get(i+j*nn);
			ft->update(i+j*nn, hvPair<Y,Y>( c.getLeft(), -c.getRight() ));
			}
		for (i=0; i<nn; i++) hvArray1<Y>::fft(*ft, pow_2,1,i*nn,false);
		for (i=0; i<nn; i++) hvArray1<Y>::fft(*ft, pow_2,nn,i,false);
		for (i=0; i<nn; i++)
		for (j=0; j<nn; j++)
			{
			hvPair<Y,Y> c = ft->get(i+j*nn);
			Y val = c.mod()*cos(c.phase());
			if (i==0 && j==0) {min=val; max=val; }
			if (min>val) min=val;
			if (max<val) max=val;
			}
		printf("fft min=%g, max=%g\n", min, max);
		for (i=0; i<nn; i++)
		for (j=0; j<nn; j++)
			{
			hvPair<Y,Y> c = ft->get(i+j*nn);
			Y val = (2.0*(c.mod()*cos(c.phase())-min)/(max-min)-1.0)*scal+offset;
			hvArray2<X>::update(i,j,(X)val);
			}
	}
	template <class Y> hvArray1<Y> *statfft(bool centred, Y scal, Y offset, int pow_2, int niter, bool amplitude=false) const
	{ 
		int i,j,k;
		X rr;
		int nn=1<<pow_2;
		if (nn>= this->sizeX() || nn>= this->sizeY()) { std::cout << "hvField2 too small for statfft " << this->sizeX() << "," << this->sizeY() << "," << nn << "\n"; hvFatal("take smaller pow2"); }
		hvArray1<hvPair<Y,Y> > *ft = new hvArray1<hvPair<Y,Y> >(nn*nn, hvPair<Y,Y>(Y(0),Y(0)));
		hvArray1<Y> *sumft = new hvArray1<Y>(nn*nn, Y(0));
		for (k=0; k<niter; k++)
		{
			int indx = int((double)rand()/(double)RAND_MAX*(double)(this->sizeX()-nn));
			int indy = int((double)rand()/(double)RAND_MAX*(double)(this->sizeY()-nn));
			for (i=0; i<nn; i++)
			for (j=0; j<nn; j++)
				{
				rr = get(i+indx,j+indy);
				Y xx = (Y(rr)-offset) / scal;
				ft->update(i+j*nn, hvPair<Y,Y>((centred && (i+j)%2==1) ? Y(0)-xx : xx,Y(0)));
				}
			for (i=0; i<nn; i++) hvArray1<Y>::fft(*ft, pow_2,1,i*nn,false);
			for (i=0; i<nn; i++) hvArray1<Y>::fft(*ft, pow_2,nn,i,false);
			for (i=0; i<nn*nn; i++)
			{
				hvPair<Y,Y> v = ft->get(i);
				Y val;
				if (amplitude) val = Y(sqrt(double(v.getLeft()*v.getLeft()+v.getRight()*v.getRight())));
				else val = v.getLeft()*v.getLeft()+v.getRight()*v.getRight();
				sumft->update(i, sumft->get(i)+val/(Y)(nn*nn) );
			}
		}
		for (i=0; i<nn*nn; i++) sumft->update(i, sumft->get(i)/(Y)(niter) );
		delete ft;
		return sumft;
	}

};




}

#endif // !defined(AFX_SCAL_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
