// hvPicture.h: interface for the picture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURE_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
#define AFX_PICTURE_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvBitmap.h"
#include "hvPict.h"
#include "hvPictRGB.h"
#include "hvQPicture.h"

namespace hview {

////////////////////////////////////////////////////////////
// template <class T> class hvPict : public hvField2< T > 


	template <class T> template <class X, class Y>  hvPict<T>::hvPict(const hvPictRGB<X> &pict, Y scal, int x, int y, int sx, int sy) : hvField2< T >(sx - x + 1, sy - y + 1, T(0)), hvArray2< T >(sx - x + 1, sy - y + 1, T(0))
	{
		this->reset(pict.sizeX(), pict.sizeY(), T(0));
		int i, j;
		for (i = x; i <= sx; i++) for (j = y; j <= sy; j++)
		{
			this->update(i - x, j - y, T(Y((pict.get(i, j)).luminance())*scal));
		}
	}
	template <class T> template <class X, class Y> void hvPict<T>::convert(const hvPictRGB<X> &pict, hvPictComponent cc, Y scal)
	{
		this->reset(pict.sizeX(), pict.sizeY(), T(0));
		int i, j;
		for (i = 0; i<pict.sizeX(); i++) for (j = 0; j<pict.sizeY(); j++)
		{
			Y v;
			hvColRGB<X> co = pict.get(i, j);
			switch (cc)
			{
			case HV_RED: v = Y(co.RED()); break;
			case HV_GREEN: v = Y(co.GREEN()); break;
			case HV_BLUE: v = Y(co.BLUE()); break;
			default: v = Y(co.luminance());
			}
			this->update(i, j, T(v*scal));
		}
	}
	template <class T> template <class X, class Y> hvPict<T>::hvPict(const hvPictRGB<X> &pict, hvPictComponent cc, Y scal, int x, int y, int sx, int sy) : hvField2< T >(sx - x + 1, sy - y + 1, T(0)), hvArray2< T >(sx - x + 1, sy - y + 1, T(0))
	{
		int i, j;
		for (i = x; i <= sx; i++) for (j = y; j <= sy; j++)
		{
			Y v;
			hvColRGB<X> co = pict.get(i, j);
			switch (cc)
			{
			case HV_RED: v = Y(co.RED()); break;
			case HV_GREEN: v = Y(co.GREEN()); break;
			case HV_BLUE: v = Y(co.BLUE()); break;
			default: v = Y(co.luminance());
			}
			this->update(i - x, j - y, T(v*scal));
			//printf("%d,%d=%d\n",i,j,get(i-x,j-y));
		}
	}
	template <class T> template <class X, class Y> hvPict<T>::hvPict(const hvPictRGBA<X> &pict, Y scal, int x, int y, int sx, int sy) : hvField2< T >(sx - x + 1, sy - y + 1, T(0)), hvArray2< T >(sx - x + 1, sy - y + 1, T(0))
	{
		int i, j;
		for (i = x; i <= this->sizeX(); i++) for (j = y; j <= this->sizeY(); j++)
		{
			this->update(i - x, j - y, T(Y((pict.get(i, j)).luminance())*scal));
		}
	}
	template <class T> template <class X, class Y> hvPict<T>::hvPict(const hvPictRGBA<X> &pict, hvPictComponent cc, Y scal, int x, int y, int sx, int sy) : hvField2< T >(sx - x + 1, sy - y + 1, T(0)), hvArray2< T >(sx - x + 1, sy - y + 1, T(0))
	{
		int i, j;
		for (i = x; i <= this->sizeX(); i++) for (j = y; j <= this->sizeY(); j++)
		{
			Y v;
			hvColRGBA<X> co = pict.get(i, j);
			switch (cc)
			{
			case HV_RED: v = Y(co.RED()); break;
			case HV_GREEN: v = Y(co.GREEN()); break;
			case HV_BLUE: v = Y(co.BLUE()); break;
			case HV_ALPHA: v = Y(co.ALPHA()); break;
			default: v = Y(co.luminance());
			}
			this->update(i - x, j - y, T(v*scal));
		}
	}

	template <class T> void hvPict<T>::squaredDifference(int px, int py, int dx, int dy, const hvPictRGB<unsigned char> &pia, int ix, int iy, const hvPictRGB<unsigned char> &pib)
	{
		int i, j;
		this->reset(dx, dy, T(0));
		for (i = 0; i<dx; i++) for (j = 0; j<dy; j++)
		{
			int kx = px + i; while (kx<0) kx += pia.sizeX(); while (kx >= pia.sizeX()) kx -= pia.sizeX();
			int ky = py + j; while (ky<0) ky += pia.sizeY(); while (ky >= pia.sizeY()) ky -= pia.sizeY();
			this->update(i, j, T(pia.get(kx, ky).squaredDifference(pib.get(ix + i, iy + j))));
		}
	}

////////////////////////////////////////////////////////////
//template <class T> class hvPictRGB : public hvField2< hvColRGB<T> >

	template <class T>  template <class U, class V> hvPictRGB<T>::hvPictRGB(const hvPict<U> &p, V scal)
	{
		int i, j;
		V val;
		hvArray2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
		{
			val = V(p.get(i, j));
			val *= scal;
			this->update(i, j, hvColRGB<T>(T(val), T(val), T(val)));
		}
	}
	template <class T> template <class U, class V> hvPictRGB<T>::hvPictRGB(const hvPict<U> &p, V scal, V shift)
	{
		int i, j;
		V val;
		hvArray2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
		{
			val = V(p.get(i, j));
			val *= scal;
			val += shift;
			update(i, j, hvColRGB<T>(T(val), T(val), T(val)));
		}
	}
	template <class T> template <class U, class V> hvPictRGB<T>::hvPictRGB(const hvPict<U> &p, V scal, int x, int y, int sx, int sy)
	{
		int i, j;
		V val;
		hvArray2< hvColRGB<T> >::reset(sx - x + 1, sy - y + 1, hvColRGB<T>(0));
		for (i = x; i <= sx; i++) for (j = y; j <= sy; j++)
		{
			val = V(p.get(i, j));
			val *= scal;
			this->update(i - x, j - y, hvColRGB<T>(T(val), T(val), T(val)));
		}
	}
	template <class T> template <class U, class V> void hvPictRGB<T>::convert(const hvPict<U> &p, V scal, int x, int y, int sx, int sy)
	{
		int i, j;
		V val;
		this->reset(sx - x + 1, sy - y + 1, hvColRGB<T>(T(0)));
		for (i = x; i <= sx; i++) for (j = y; j <= sy; j++)
		{
			val = V(p.get(i, j));
			val *= scal;
			this->update(i - x, j - y, hvColRGB<T>(T(val), T(val), T(val)));
		}
	}
	template <class T> template <class U, class V> void hvPictRGB<T>::convertloga(const hvPict<U> &p, V loga, V max, V scal, int x, int y, int sx, int sy)
	{
		int i, j;
		V val;
		this->reset(sx - x + 1, sy - y + 1, hvColRGB<T>(T(0)));
		for (i = x; i <= sx; i++) for (j = y; j <= sy; j++)
		{
			val = V(p.get(i, j));
			val = (V)(log(1.0 + (double)loga*(double)val / (double)max) / log((double)loga + 1.0));
			if (val>V(1)) val = V(1);
			val *= scal;
			this->update(i - x, j - y, hvColRGB<T>(T(val), T(val), T(val)));
		}
	}
	template <class T> template <class U, class V> hvPictRGB<T>::hvPictRGB(const hvPict<U> &p, V scal, V min, V max)
	{
		int i, j;
		V val;
		hvArray2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
		{
			val = V(p.get(i, j));
			val = scal*(val - min) / (max - min);
			this->update(i, j, hvColRGB<T>(T(val), T(val), T(val)));
		}
	}
	template <class T> template <class U> hvPictRGB<T>::hvPictRGB(const hvPict<U> &p, const std::vector<hvColRGB<unsigned char> > &lcol)
	{
		int i, j;
		hvArray2< hvColRGB<T> >::reset(p.sizeX(), p.sizeY(), hvColRGB<T>(0));
		for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
		{
			U val = p.get(i, j);
			this->update(i, j, lcol.at(val));
		}
	}

		
////////////////////////////////////////////////////////////
//template <class T, unsigned int n> class hvQPict : public hvArray2< T >, public std::vector<unsigned char>  // T is an unsigned integer type (char, short or int)

template <class T, unsigned int n> void hvQPictRGB<T, n>::update(const hvPictRGB<unsigned char> &pi)
{
	int i, j;
	for (i = 0; i<pi.sizeX(); i++)
		for (j = 0; j<pi.sizeY(); j++)
		{
			hvColRGB<unsigned char> val = pi.get(i, j);
			hvArray2< T >::update(i, j, (T)closest(val));
		}
}
template <class T, unsigned int n> void hvQPictRGB<T, n>::updateTable(const hvPictRGB<unsigned char> &pi)
{
	int i, j;
	std::vector<hvColRGB<double> > coltab(n); coltab.clear();
	std::vector<int> count(n); count.clear();
	for (i = 0; i<std::vector<hvColRGB<unsigned char> >::size(); i++) { coltab.push_back(hvColRGB<double>(0.0)); count.push_back(0); }
	for (i = 0; i<pi.sizeX(); i++)
		for (j = 0; j<pi.sizeY(); j++)
		{
			hvColRGB<double> val = hvColRGB<double>(pi.get(i, j));
			val += coltab.at(hvArray2< T >::get(i, j));
			coltab[hvArray2< T >::get(i, j)] = val;
			count[hvArray2< T >::get(i, j)] = count.at(hvArray2< T >::get(i, j)) + 1;
		}
	for (i = 0; i<std::vector<hvColRGB<unsigned char> >::size(); i++)
	{
		hvColRGB<double> val = coltab.at(i);
		val /= (double)count.at(i);
		//printf("table %d: %g,%g,%g, count=%d\n", i, val.RED(), val.GREEN(), val.BLUE(), count.get(i));
		std::vector<hvColRGB<unsigned char> >::operator[](i) = hvColRGB<unsigned char>(val);
	}
}

template <class T, unsigned int n> hvQPictRGB<T, n>::hvQPictRGB(const hvPictRGB<unsigned char> &pi, int nbcol) : hvArray2< T >(pi.sizeX(), pi.sizeY(), T(0)), std::vector<hvColRGB<unsigned char> >(n)
{
	std::vector<hvColRGB<unsigned char> >::clear();
	this->quantize(pi, nbcol, 0);
}
template <class T, unsigned int n> hvQPictRGB<T, n>::hvQPictRGB(const hvPictRGB<unsigned char> &pi, int nbcol, int level) : hvArray2< T >(pi.sizeX(), pi.sizeY(), T(0)), std::vector<hvColRGB<unsigned char> >(n)
{
	std::vector<hvColRGB<unsigned char> >::clear();
	this->quantize(pi, nbcol, level);
}
template <class T, unsigned int n> hvQPictRGB<T, n>::hvQPictRGB(const hvPictRGB<unsigned char> &pi, const hvPictRGB<unsigned char> &ps, int nbcol) : hvArray2< T >(pi.sizeX(), pi.sizeY(), T(0)), std::vector<hvColRGB<unsigned char> >(n)
{
	std::vector<hvColRGB<unsigned char> >::clear();
	quantize(pi, ps, nbcol);
}

template <class T, unsigned int n> void hvQPictRGB<T, n>::quantizeHue( const hvPictRGB<unsigned char> &pi, int nbcol, hvColRGB<double> ww, int level )
{
	hvPictRGB<unsigned char> pihsv;
	//pihsv.toHSV(pi, 255, ww);
	pihsv.toLUV(pi, 255, ww);
	quantize(pihsv, nbcol, level);
	updateTable(pi);
}
template <class T, unsigned int n> void hvQPictRGB<T, n>::quantizeLuv(const hvPictRGB<unsigned char> &pi, int nbcol, hvColRGB<double> ww, int level)
{
	hvPictRGB<unsigned char> pihsv;
	//pihsv.toHSV(pi, 255, ww);
	pihsv.toLUV(pi, 255, ww);
	quantize(pihsv, nbcol, level);
	updateTable(pi);
}


template <class T, unsigned int n> void hvQPictRGB<T, n>::quantize(const hvPictRGB<unsigned char> &pi, int nbcol, int level)
{
	//if (sizeX()!=pi.sizeX() || sizeY()!=pi.sizeY()) { hvFatal("hvQPictRGB<T,n>::quantize pictures must have same resolution"); return; }
	this->reset(pi.sizeX(), pi.sizeY(), T(0));
	if (nbcol <= 1) { std::vector<hvColRGB<unsigned char> >::push_back(pi.avg()); return; }
	int nc = (nbcol >= 8 ? nbcol : 8);
	hvOctree<hvQPictRGBVal> *troot = hvOctree<hvQPictRGBVal>::createOctree(hvQPictRGBVal());
	int nleaf = 0;
	hvPictRGB<unsigned char> ps;
	int i, j;
	//if (level>0) step=1<<level;
	if (level <= 1) ps.clone(pi, 0, 0, pi.sizeX() - 1, pi.sizeY() - 1);
	else {
		hvPictRGB<unsigned char> pbuf; pbuf.clone(pi, 0, 0, pi.sizeX() - 1, pi.sizeY() - 1);
		for (i = 1; i<level; i++) { ps.shrink(&pbuf); pbuf.clone(ps, 0, 0, ps.sizeX() - 1, ps.sizeY() - 1); }
	}
	for (i = 0; i<ps.sizeX(); i++)
		for (j = 0; j<ps.sizeY(); j++)
		{
			// insert the value
			hvColRGB<unsigned char> val = ps.get(i, j);
			hvQPictRGB<T, n>::insertValue(troot, val);
			// test the number of leafs
			hvQPictRGB<T, n>::updateTree(troot, nc);
		}
	// create the table
	std::vector<hvColRGB<unsigned char> >::clear();
	this->updateTable(troot);
	hvOctree<hvQPictRGBVal>::destroy(troot);
	this->update(pi);
	this->updateTable(pi);
	while (std::vector<hvColRGB<unsigned char> >::size()>nbcol)
	{
		this->reduce();
		this->update(pi);
		this->updateTable(pi);
	}
	//printf("Ncolors=%d\n", hvAList<hvColRGB<unsigned char>,n>::length());
}
template <class T, unsigned int n> void hvQPictRGB<T, n>::quantize(const hvPictRGB<unsigned char> &pi, const hvPictRGB<unsigned char> &ps, int nbcol)
{
	//if (sizeX()!=pi.sizeX() || sizeY()!=pi.sizeY()) { hvFatal("hvQPictRGB<T,n>::quantize pictures must have same resolution"); return; }
	this->reset(pi.sizeX(), pi.sizeY(), T(0));
	if (nbcol <= 1) { std::vector<hvColRGB<unsigned char> >::push_back(pi.avg()); return; }
	int nc = (nbcol >= 4 ? nbcol : 4);
	hvOctree<hvQPictRGBVal> *troot = hvOctree<hvQPictRGBVal>::createOctree(hvQPictRGBVal());
	int nleaf = 0;
	int i, j;
	for (i = 0; i<ps.sizeX(); i++)
		for (j = 0; j<ps.sizeY(); j++)
		{
			// insert the value
			hvColRGB<unsigned char> val = ps.get(i, j);
			hvQPictRGB<T, n>::insertValue(troot, val);
			// test the number of leafs
			hvQPictRGB<T, n>::updateTree(troot, nc);
		}
	// create the table
	std::vector<hvColRGB<unsigned char> >::clear();
	this->updateTable(troot);
	hvOctree<hvQPictRGBVal>::destroy(troot);
	this->update(pi);
	this->updateTable(pi);
	while (std::vector<hvColRGB<unsigned char> >::size()>nbcol)
	{
		this->reduce();
		this->update(pi);
		this->updateTable(pi);
	}
	//printf("Ncolors=%d\n", hvAList<hvColRGB<unsigned char>,n>::length());
}

template <class T, unsigned int n> void hvQPictRGB<T, n>::apply(T scal, hvPictRGB<unsigned char> &pi, std::vector<hvFrame3<double> > &lfr, double offset, double rescal)
{
	int i, j;
	for (i = 0; i<pi.sizeX(); i++) for (j = 0; j<pi.sizeY(); j++)
	{
		hvColRGB<T> v = pi.get(i, j);
		hvVec3<double> col((double)v.RED() / (double)scal, (double)v.GREEN() / (double)scal, (double)v.BLUE() / (double)scal);
		int q = this->hvArray2< T >::get(i, j);
		hvFrame3<double> fr = lfr.at(q);
		hvLinearTransform3<double> t; t.inverseFrame3(fr);
		col = t.apply(col);
		double rr = (col.X()*rescal + offset);
		if (rr<0.0) rr = 0.0; else if (rr>1.0) rr = 1.0;
		double gg = (col.Y()*rescal + offset);
		if (gg<0.0) gg = 0.0; else if (gg>1.0) gg = 1.0;
		double bb = (col.Z()*rescal + offset);
		if (bb<0.0) bb = 0.0; else if (bb>1.0) bb = 1.0;
		v = hvColRGB<T>((T)(rr*(double)scal), (T)(gg*(double)scal), (T)(bb*(double)scal));
		pi.update(i, j, v);
	}
}
template <class T, unsigned int n> void hvQPictRGB<T, n>::applyInverse(T scal, hvPictRGB<unsigned char> &pi, std::vector<hvFrame3<double> > &lfr, double offset, double rescal)
{
	int i, j;
	for (i = 0; i<pi.sizeX(); i++) for (j = 0; j<pi.sizeY(); j++)
	{
		int q = this->getIndex(i, j);
		hvFrame3<double> fr = lfr.at(q);
		hvLinearTransform3<double> t; t = hvLinearTransform3<double>(fr);
		hvColRGB<unsigned char> cc = pi.get(i, j);
		hvVec3<double> col(((double)cc.RED() / (double)scal - offset) / rescal, ((double)cc.GREEN() / (double)scal - offset) / rescal, ((double)cc.BLUE() / (double)scal - offset) / rescal);
		//hvVec3<double> col( ((double)cc.RED()/255.0-0.5)/0.7, 0.0,0.0);
		col = t.apply(col);
		double rr = col.X()*(double)scal; if (rr<0.0) rr = 0.0; else if (rr>(double)scal) rr = (double)scal;
		double gg = col.Y()*(double)scal; if (gg<0.0) gg = 0.0; else if (gg>(double)scal) gg = (double)scal;
		double bb = col.Z()*(double)scal; if (bb<0.0) bb = 0.0; else if (bb>(double)scal) bb = (double)scal;
		pi.update(i, j, hvColRGB<unsigned char>((unsigned char)(rr), (unsigned char)(gg), (unsigned char)(bb)));
	}
}

template <class T, unsigned int n> void hvQPictRGB<T, n>::update(double ww, T scal, hvPictRGB<unsigned char> &pi, std::vector<hvFrame3<double> > &lfr, double offset, double rescal)
{
	int i, j, q;
	T indmin;
	double errmin;
	for (i = 0; i<pi.sizeX(); i++)
		for (j = 0; j<pi.sizeY(); j++)
		{
			hvColRGB<T> v = pi.get(i, j);
			hvVec3<double> col((double)v.RED() / (double)scal, (double)v.GREEN() / (double)scal, (double)v.BLUE() / (double)scal);
			for (q = 0; q<this->ncolors(); q++)
			{
				hvColRGB<unsigned char> tabv = std::vector<hvColRGB<unsigned char> >::at(q);
				hvVec3<double> tabval((double)tabv.RED() / (double)scal, (double)tabv.GREEN() / (double)scal, (double)tabv.BLUE() / (double)scal);
				hvFrame3<double> fr = lfr.at(q);
				hvLinearTransform3<double> t;
				t.inverseFrame3(fr);
				hvVec3<double> collfr = col;
				collfr = t.apply(collfr);
				collfr = hvVec3<double>(collfr.X(), 0.0, 0.0);
				t = hvLinearTransform3<double>(fr);
				collfr = t.apply(collfr);
				hvVec3<double> errp; errp.PVec(col, collfr);
				hvVec3<double> errptab; errptab.PVec(tabval, col);
				if (q == 0) { errmin = ww*errp.norm() + (1.0 - ww)*errptab.norm(); indmin = 0; }
				else if (ww*errp.norm() + (1.0 - ww)*errptab.norm()<errmin) { errmin = ww*errp.norm() + (1.0 - ww)*errptab.norm(); indmin = q; }
			}
			//if (hvArray2< T >::get(i,i)!=indmin) printf("pixel %d,%d from %d to %d\n", i,j,hvArray2< T >::get(i,i),indmin);
			hvArray2< T >::update(i, j, (T)indmin);
		}
}

template <class T, unsigned int n> void hvQPictRGB<T, n>::plsr(T scal, const hvPictRGB<unsigned char> &pi, std::vector<hvFrame3<double> > &lfr) const
{
	lfr.clear();
	int i;
	for (i = 0; i<this->ncolors(); i++)
	{
		hvBitmap mm; this->convert(mm, i);
		hvFrame3<double> fr = pi.pca(255, &mm);
		lfr.push_back(fr);
	}
}
template <class T, unsigned int n> hvVec3<double> hvQPictRGB<T, n>::plsrmean(const hvPictRGB<unsigned char> &pi, const std::vector<hvFrame3<double> > &lfr, std::vector<double> &lvar) const
{
	lvar.clear();
	hvVec3<double> mean(0.0);
	for (int i = 0; i<this->ncolors(); i++)
	{
		hvBitmap mm; this->convert(mm, i);
		int count = mm.count();
		hvFrame3<double> fr = lfr.at(i);
		hvVec3<double> vv = fr.origin();
		vv.scale((double)count / (double)(this->sizeX()*this->sizeY()));
		mean += vv;
		int ii, jj;
		double var = 0.0;
		for (ii = 0; ii<pi.sizeX(); ii++) for (jj = 0; jj<pi.sizeY(); jj++)
		{
			if (mm.get(ii, jj))
			{
				hvColRGB<T> v = pi.get(ii, jj);
				hvVec3<double> col((double)v.RED() / (double)255, (double)v.GREEN() / (double)255, (double)v.BLUE() / (double)255);
				hvLinearTransform3<double> t; t.inverseFrame3(fr);
				col = t.apply(col);
				if (col.X()>var) var = col.X();
				//var+=abs(col.X());
			}
		}
		//var /= (double)count;
		lvar.push_back(var);
	}
	return mean;
}
template <class T, unsigned int n> void hvQPictRGB<T, n>::load(FILE *fd)
{
	int  sx, sy;
	int i, j, ncol;
	char buff[256];
	hvColRGB<T> co;

	hvPictRGB<T>::readPPMLine(fd, buff);
	if (strcmp(buff, "PQ\n") != 0) { hvFatal("cannot read PictQuant, not correct format"); return; }
	hvPictRGB<T>::readPPMLine(fd, buff);
	sscanf(buff, "%d %d %d", &ncol, &sx, &sy);
	if (ncol>n) { hvFatal("cannot read PictQuant, too many color indices"); return; }
	hvPictRGB<T>::readPPMLine(fd, buff);
	if (strcmp(buff, "255\n") != 0) { hvFatal("Not the right PictQuant Format"); }
	hvArray2< T >::reset(sx, sy);
	std::vector<hvColRGB<T> >::clear();
	for (i = 0; i<ncol; i++)
	{
		unsigned char r, g, b;
		fread(&r, 1, sizeof(unsigned char), fd);
		fread(&g, 1, sizeof(unsigned char), fd);
		fread(&b, 1, sizeof(unsigned char), fd);
		std::vector<hvColRGB<T> >::push_back(hvColRGB<T>((T)r, (T)g, (T)b));
	}
	for (i = 0; i<sy; i++)
		for (j = 0; j<sx; j++)
		{
			unsigned char v;
			fread(&v, 1, sizeof(unsigned char), fd);
			hvArray2<T>::update(j, sy - i - 1, (T)v);
		}
}

/////////////////////////////////////////////////////
// class hvBitmap
template <class T, class U> hvBitmap::hvBitmap(const hvPict<T> &p, hvBitmap::operation op, U value) : hvBoolArray2(p.sizeX(), p.sizeY(), false)
{
	int i, j;
	for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
	{
		switch (op)
		{
		case LESS: if (U(p.get(i, j))<value) set(i, j, true); break;
		case LEQUAL: if (U(p.get(i, j)) <= value) set(i, j, true); break;
		case EQUAL: if (U(p.get(i, j)) == value) set(i, j, true); break;
		case GEQUAL: if (U(p.get(i, j)) >= value) set(i, j, true); break;
		case GREATER: if (U(p.get(i, j))>value) set(i, j, true); break;
		case NOTEQUAL: if (U(p.get(i, j)) != value) set(i, j, true); break;
		default: break;
		}
	}
}
template <class T, class U> void hvBitmap::convert(const hvPict<T> &p, hvBitmap::operation op, U value)
{
	int i, j;
	hvBoolArray2::reset(p.sizeX(), p.sizeY(), false);
	for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
	{
		switch (op)
		{
		case LESS: if (U(p.get(i, j))<value) set(i, j, true); break;
		case LEQUAL: if (U(p.get(i, j)) <= value) set(i, j, true); break;
		case EQUAL: if (U(p.get(i, j)) == value) set(i, j, true); break;
		case GEQUAL: if (U(p.get(i, j)) >= value) set(i, j, true); break;
		case GREATER: if (U(p.get(i, j))>value) set(i, j, true); break;
		case NOTEQUAL: if (U(p.get(i, j)) != value) set(i, j, true); break;
		default: break;
		}
	}
}
template <class T, class U> void hvBitmap::convert(const hvPict<T> &p, hvBitmap::operation op, int nn, U value[])
{
	int i, j, k;
	hvBoolArray2::reset(p.sizeX(), p.sizeY(), false);
	for (i = 0; i<p.sizeX(); i++) for (j = 0; j<p.sizeY(); j++)
	{
		for (k = 0; k<nn && get(i, j) == false; k++)
		{
			switch (op)
			{
			case LESS: if (U(p.get(i, j))<value[k]) set(i, j, true); break;
			case LEQUAL: if (U(p.get(i, j)) <= value[k]) set(i, j, true); break;
			case EQUAL: if (U(p.get(i, j)) == value[k]) set(i, j, true);  break;
			case GEQUAL: if (U(p.get(i, j)) >= value[k]) set(i, j, true); break;
			case GREATER: if (U(p.get(i, j))>value[k]) set(i, j, true); break;
			case NOTEQUAL: if (U(p.get(i, j)) != value[k]) set(i, j, true); break;
			default: break;
			}
		}
	}
}

}

#endif // !efined(AFX_COLOR_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
