// hvField1.h: interface for the scalar field in 1D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIELD1D_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
#define AFX_FIELD1D_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_

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

#include "hvField.h"
#include "hvArray1.h"

namespace hview {

template <class X> class hvField1 : public virtual hvField<X>, public virtual hvArray1<X>  
{
public:
	hvField1<X>(int s, X nil):hvField<X>(nil),hvArray1<X>(s,nil) { hvField<X>::loop[0]=hvField<X>::CLAMP; }
	hvField1<X>():hvField<X>(),hvArray1<X>() { hvField<X>::loop[0]=hvField<X>::CLAMP; }

	virtual int	size(int dim) const
	{
		if (dim!=0) hvFatal("dim must be 0 in hvField1::size(int dim)");
		return hvArray1< X>::size();
	}
	int sizeX() const { return hvArray1< X>::size(); }
	virtual int dimension() const { return 1; }
	virtual X operator()(int ind[]) const 
	{ 
		int i;
		if (ind[0]<0||ind[0]>=hvArray1<X>::size()) 
		{
			if (hvField<X>::loop[0]==hvField<X>::CLAMP) return hvField<X>::clampvalue;
			i = ind[0]%hvArray1<X>::size();
			if (hvField<X>::loop[0]==hvField<X>::MIRROR && (ind[0]/hvArray1<X>::size())%2==1) i = hvArray1<X>::size()-i-1;
		}
		else i=ind[0];
		return hvArray1<X>::get(i); 
	}
	virtual X get(int ind[]) const 
	{ 
		int i;
		if (ind[0]<0||ind[0]>=hvArray1<X>::size()) 
		{
			if (hvField<X>::loop[0]==hvField<X>::CLAMP) return hvField<X>::clampvalue;
			i = ind[0]%hvArray1<X>::size();
			if (hvField<X>::loop[0]==hvField<X>::MIRROR && (ind[0]/hvArray1<X>::size())%2==1) i = hvArray1<X>::size()-i-1;
		}
		else i=ind[0];
		return hvArray1<X>::get(i); 
	}
	virtual void update(int ind[], X val) 
	{ 
		if (ind[0]<0||ind[0]>=hvArray1<X>::size()) return; 
		hvArray1<X>::update(ind[0], val); 
	}

	virtual void clear(X k)
	{
		for (int i=0; i<hvArray1<X>::size(); i++) hvArray1<X>::t[i] = k;
	}
	virtual void scale(int k)
	{
		for (int i=0; i<hvArray1<X>::size(); i++) { hvArray1<X>::t[i] *= k; }
	}
	virtual void scale(float k)
	{
		for (int i=0; i<hvArray1<X>::size(); i++) { hvArray1<X>::t[i] *= k; }
	}
	virtual void scale(double k)
	{
		for (int i=0; i<hvArray1<X>::size(); i++) { hvArray1<X>::t[i] *= k; }
	}
	virtual void add(hvField<X> *f)
	{
		if (f->dimension()!=1 || hvArray1<X>::size()!=f->size(0) ) { hvFatal("cannot add in hvField1"); return; }
		for (int i=0; i<hvArray1<X>::size(); i++) hvArray1<X>::t[i] += f->get(&i);
	}

	virtual void clone(hvField<X> *f)
	{
		if (f->dimension()!=1) { hvFatal("cannot clone in hvField1"); return; }
		if ( hvArray1<X>::size()!=f->size(0) ) this->reset(f->size(0));
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		for (int i=0; i<hvArray1<X>::size(); i++) hvArray1<X>::t[i] = f->get(&i);
	}
	virtual void shrink(hvField<X> *f)
	{
		int ind1, ind2;
		if (f->dimension()!=1) { hvFatal("cannot clone in hvField1"); return; }
		this->reset(f->size(0)/2);
		hvField<X>::clampvalue = f->getClampValue();
		hvField<X>::loop[0]=f->getLoop(0);
		for (int i=0; i<hvArray1<X>::size(); i++) 
		{ 
			ind1=i*2; ind2=i*2+1; 
			X cste(2);
			X sum = f->get(&ind1); 
			X val = f->get(&ind2); sum += val;
			sum /= cste;
			hvArray1<X>::update(i,sum);
		}
	}
	template <class Y> void convolve(hvField<X> *f, hvField<Y> *mask)
	{
		if (mask->dimension()!=1) { hvFatal("cannot convolve in hvScalarField1"); return; }
		if (f->dimension()!=1) { hvFatal("cannot convolve in hvScalarField1"); return; }
		if ( hvArray1<X>::size()!=f->size(0) ) this->reset(f->size(0));
		int s = mask->size(0)/2;
		for (int i=0; i<hvArray1<X>::size(); i++) 
		{ 
			int ind = i-s, j=0; 
			Y vv=Y(f->get(&ind))*mask->get(&j);
			for (j=1; j<mask->size(0); j++) 
			{
				ind = i+j-s;
				vv += Y(f->get(&ind))*mask->get(&j);
			}
			update(&i, X(vv));
		}
		
	}
	virtual void derivative(hvField<X> *f, int dim)
	{
		if (dim!=0) hvFatal("dim must be 0 in hvField1::derivative(...)");
		if (f->dimension()!=1) { hvFatal("cannot compute gradient in hvField1"); return; }
		if ( hvArray1<X>::size()!=f->size(0) ) this->reset(f->size(0));
		for (int i=0; i<hvArray1<X>::size(); i++) 
		{ 
			int ind=i+1;
			X sum = f->get(&ind); 
			sum -= f->get(&i);
			hvArray1<X>::update(i,sum);
		}

	}

	template <class Y> hvArray1<hvPair<Y,Y> > *fft(bool centred, Y scal, Y offset) 
	{ 		
		int i;
		X rr;
		int pow_2=1, nn=2;
		while(nn<sizeX()) { pow_2=pow_2+1; nn *= 2; }
		hvArray1<hvPair<Y,Y> > *ft = new hvArray1<hvPair<Y,Y> >(nn, hvPair<Y,Y>(Y(0),Y(0)));
		for (i=0; i<nn; i++)
			{
			if (i<sizeX()) rr = get(&i); else rr = X(0);
			Y xx = (Y(rr)-offset) / scal;
			ft->update(i, hvPair<Y,Y>(centred && i%2==1 ? Y(0)-xx: xx,Y(0)));
			}
		hvArray1<Y>::fft(*ft,pow_2,1,0,true);		
		return ft;
	}
};



}

#endif // !defined(AFX_SCAL_H__B6AC0A32_75EF_428E_BC10_6219F619FA29__INCLUDED_)
