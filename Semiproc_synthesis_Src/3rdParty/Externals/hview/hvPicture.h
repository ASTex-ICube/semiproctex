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
