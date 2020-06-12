/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

#if !defined(AFX_BITMAP_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
#define AFX_BITMAP_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <math.h>

#include "hvBoolArray2.h"
#include "hvVec2.h"

namespace hview {

template <class T> class hvPict ;

////////////////////////////////////////////////////////////
class hvBitmap : public hvBoolArray2  
////////////////////////////////////////////////////////////
{
public:
	enum operation { LESS, LEQUAL, EQUAL, GEQUAL, GREATER, NOTEQUAL };
	hvBitmap(): hvBoolArray2() { }
	hvBitmap(int x, int y, bool nil): hvBoolArray2(x,y,nil) { }

	template <class T, class U> hvBitmap(const hvPict<T> &p, hvBitmap::operation op, U value);
	template <class T, class U> void convert(const hvPict<T> &p, hvBitmap::operation op, U value);
	template <class T, class U> void convert(const hvPict<T> &p, hvBitmap::operation op, int nn, U value[]);

	void operator~() { hvBoolArray2::operator ~(); }
	void operator|=(const hvBitmap &x) {  hvBoolArray2::operator|=(x); }
	void operator&=(const hvBitmap &x) {  hvBoolArray2::operator&=(x); }
	void operator^=(const hvBitmap &x) {  hvBoolArray2::operator^=(x); }
	void operatorOr(int x, int y, int px, int py, int dx, int dy, const hvBitmap &bb)
	{
		int i,j;
		for (i=0; i<dx; i++) for (j=0; j<dy; j++)
		{
			if (x+i>=0 && x+i<sizeX() && y+j>=0 && y+j<sizeY())
			{
				set(x+i,y+j,get(x+i,y+j)||bb.get(px+i,py+j));
			}
		}
	}
	int count() const
	{
		int i,j, c=0;
		for (i=0; i<sizeX(); i++) for (j=0; j<sizeY(); j++) if (get(i,j)) c++;
		return c;
	}
	int count(int px, int py, int sx, int sy) const
	{
		int i,j, c=0;
		for (i=px; i<=sx; i++) for (j=py; j<=sy; j++) if (get(i,j)) c++;
		return c;
	}
	void bary(int &x, int &y) const
	{
		int i,j, c=0;
		x=0; y=0;
		for (i=0; i<sizeX(); i++) for (j=0; j<sizeY(); j++) if (get(i,j)) { x+=i; y+=j; c++; }
		x /= c; y/=c;
	}
	void shrink(const hvBitmap &bm, bool def)
	{
		reset(bm.sizeX()/2, bm.sizeY()/2, false);
		for (int i=0; i<sizeX(); i++) 
		for (int j=0; j<sizeY(); j++)
		{
			int sum=bm.get(2*i,2*j)?1:0;
			sum += bm.get(2*i+1,2*j)?1:0;
			sum += bm.get(2*i,2*j+1)?1:0;
			sum += bm.get(2*i+1,2*j+1)?1:0;
			bool v=def;
			if (sum==0) v=false; else if (sum==4) v=true;
			set(i,j,v);
		}
	}
	void shrink(const hvBitmap &bm)
	{
		reset(bm.sizeX()/2, bm.sizeY()/2, false);
		for (int i=0; i<sizeX(); i++) 
		for (int j=0; j<sizeY(); j++)
		{
			int sum=bm.get(2*i,2*j)?1:0;
			sum += bm.get(2*i+1,2*j)?1:0;
			sum += bm.get(2*i,2*j+1)?1:0;
			sum += bm.get(2*i+1,2*j+1)?1:0;
			bool v;
			if (sum>2) v=true; else v=false;
			set(i,j,v);
		}
	}
	void rescalex(const hvBitmap &bm, float scx)
	{
		reset((int)((float)bm.sizeX()*scx) , bm.sizeY() , false);
		for (int i = 0; i<this->sizeX(); i++)
			for (int j = 0; j<this->sizeY(); j++)
			{
				int x = (int)((float)i / scx);
				if (x >= bm.sizeX()) x = bm.sizeX() - 1;
				set(i, j, bm.get(x, j));
			}
	}
	void rotation(const hvBitmap &bm, float angle)
	{
		int i, j, ii, jj;
		int dx = bm.sizeX() / 2, dy = bm.sizeY() / 2;
		int sx = 0, sy = 0;
		for (ii = 0; ii <= 1; ii++) for (jj = 0; jj <= 1; jj++)
		{
			int rx = (int)((float)(ii==0?dx:-dx)*cos(angle) - (float)(jj==0?dy:-dy)*sin(angle));
			int ry = (int)((float)(ii==0?dx:-dx)*sin(angle) + (float)(jj==0?dy:-dy)*cos(angle));
			if (rx > sx) sx = rx;
			if (ry > sy) sy = ry;
		}
		bool cont = true;
		for (i = 1; i <= (sx < sy ? sx:sy) && cont; i++)
		{
			bool valid = true;
			for (j = -i; j <= i && valid; j++)
			{
				int xx = bm.sizeX() / 2 + (int)((float)j*cos(-angle) - (float)i*sin(-angle));
				int yy = bm.sizeY() / 2 + (int)((float)j*sin(-angle) + (float)i*cos(-angle));
				if (xx < 0 || xx >= bm.sizeX() || yy < 0 || yy >= bm.sizeY()) valid = false;
				xx = bm.sizeX() / 2 + (int)((float)j*cos(-angle) + (float)i*sin(-angle));
				yy = bm.sizeY() / 2 + (int)((float)j*sin(-angle) - (float)i*cos(-angle));
				if (xx < 0 || xx >= bm.sizeX() || yy < 0 || yy >= bm.sizeY()) valid = false;
				xx = bm.sizeX() / 2 + (int)((float)i*cos(-angle) - (float)j*sin(-angle));
				yy = bm.sizeY() / 2 + (int)((float)i*sin(-angle) + (float)j*cos(-angle));
				if (xx < 0 || xx >= bm.sizeX() || yy < 0 || yy >= bm.sizeY()) valid = false;
				xx = bm.sizeX() / 2 + (int)(-(float)i*cos(-angle) - (float)j*sin(-angle));
				yy = bm.sizeY() / 2 + (int)(-(float)i*sin(-angle) + (float)j*cos(-angle));
				if (xx < 0 || xx >= bm.sizeX() || yy < 0 || yy >= bm.sizeY()) valid = false;
			}
			if (!valid) cont = false;
		}
		int resol = i - 1;
		this->reset(2*resol+1, 2*resol+1, false);
		for (int i = -resol; i<=resol; i++)
			for (int j = -resol; j<=resol; j++)
			{
				int xx = bm.sizeX() / 2 + (int)((float)i*cos(-angle) - (float)j*sin(-angle));
				int yy = bm.sizeY() / 2 + (int)((float)i*sin(-angle) + (float)j*cos(-angle));
				if (xx >= 0 && xx < bm.sizeX() && yy >= 0 && yy < bm.sizeY())
				{
					this->set(i+resol, j+resol, bm.get(xx, yy));
				}
			}
	}

	void seedfill(int x, int y)
	{
		if (x<0 || y<0 || x>=sizeX() || y>=sizeY()) return;
		if (get(x,y)) return;
		set(x,y,true);
		int i,a,b;
		for (i=x+1; i<sizeX() && !get(i,y); i++) set(i,y, true);
		b=i-1;
		for (i=x-1; i>=0 && !get(i,y); i--) set(i,y, true);
		a = i+1;
		for (i=a; i<=b; i++) 
		{ 
			seedfill(i,y-1); 
			seedfill(i,y+1); 
		} 
	}
	void seedfill(int x, int y, hvBitmap &bm)
	{
		if (x<0 || y<0 || x>=sizeX() || y>=sizeY()) return;
		if (get(x,y)) return;
		set(x,y,true); bm.set(x,y,true);
		int i,a,b;
		for (i=x+1; i<sizeX() && !get(i,y); i++) { set(i,y, true); bm.set(i,y, true); }
		b = i - 1; if (b >= sizeX()) b = sizeX() - 1;
		for (i=x-1; i>=0 && !get(i,y); i--) { set(i,y, true); bm.set(i,y, true); }
		a = i + 1; if (a < 0) a = 0;
		for (i=a; i<=b; i++) 
		{ 
			if (y>=1) if (!get(i,y-1)) seedfill(i,y-1,bm); 
			if (y<=sizeY()-2) if (!get(i,y+1)) seedfill(i,y+1,bm); 
		} 
	}
	void seedfill(int x, int y, hvBitmap &bm, hvVec2<int> &min, hvVec2<int> &max )
	{
		if (x<0 || y<0 || x>=sizeX() || y>=sizeY()) return;
		if (get(x,y)) return;
		set(x,y,true); bm.set(x,y,true);
		min.keepMin(min,hvVec2<int>(x,y));
		max.keepMax(max,hvVec2<int>(x,y));
		int i,a,b;
		for (i=x+1; i<sizeX() && !get(i,y); i++) { set(i,y, true); bm.set(i,y, true); min.keepMin(min,hvVec2<int>(i,y)); max.keepMax(max,hvVec2<int>(i,y)); }
		b=i-1;
		for (i=x-1; i>=0 && !get(i,y); i--) { set(i,y, true); bm.set(i,y, true); min.keepMin(min,hvVec2<int>(i,y)); max.keepMax(max,hvVec2<int>(i,y)); }
		a = i+1;
		for (i=a; i<=b; i++) 
		{ 
			this->seedfill(i,y-1,bm,min,max); 
			this->seedfill(i,y+1,bm,min,max); 
		} 
	}
	bool seedfillCorners(int x, int y, hvBitmap &bml, hvBitmap &bmr, hvBitmap &bmb, hvBitmap &bmt, int qq, int &mx, int &my)
	{
		if (get(x,y)) return true;
		set(x,y,true); 
		//printf("(%d,%d):%d\n",x,y,qq);
		if(qq==3) { bml.set(x,y,true); mx+=x+this->sizeX(); my+=y+this->sizeY(); }
		else if (qq==2) { bmr.set(x,y,true); mx+=x; my+=y+this->sizeY(); }
		else if (qq==1) { bmb.set(x,y,true); mx+=x+this->sizeX(); my+=y; }
		else { bmt.set(x,y,true); mx+=x; my+=y; }
		int i,a,b;
		for (i=x+1; i<sizeX() && !get(i,y); i++) 
		{ 
			set(i,y, true); 
			if(qq==3) { bml.set(i,y,true); mx+=i+this->sizeX(); my+=y+this->sizeY(); }
			else if (qq==2) { bmr.set(i,y,true); mx+=i; my+=y+this->sizeY(); }
			else if (qq==1) { bmb.set(i,y,true); mx+=i+this->sizeX(); my+=y; }
			else { bmt.set(i,y,true); mx+=i; my+=y; }
		}
		b=i-1;
		if (b==sizeX()-1)
		{
			if (qq==2) { if (!this->seedfillCorners(0,y,bml,bmr,bmb,bmt,3,mx,my)) return false; }
			else if (qq==0) { if (!this->seedfillCorners(0,y,bml,bmr,bmb,bmt,1,mx,my)) return false; }
			else { std::cout<<"component over two borders b (q="<<qq<<"\n"; return false; }
		}
		for (i=x-1; i>=0 && !get(i,y); i--) 
		{ 
			set(i,y, true); 
			if(qq==3) { bml.set(i,y,true); mx+=i+this->sizeX(); my+=y+this->sizeY(); }
			else if (qq==2) { bmr.set(i,y,true); mx+=i; my+=y+this->sizeY(); }
			else if (qq==1) { bmb.set(i,y,true); mx+=i+this->sizeX(); my+=y; }
			else { bmt.set(i,y,true); mx+=i; my+=y; }
		}
		a = i+1;
		if (a==0)
		{
			if (qq==3) { if (!this->seedfillCorners(sizeX()-1,y,bml,bmr,bmb,bmt,2,mx,my)) return false; }
			else if (qq==1) { if (!this->seedfillCorners(sizeX()-1,y,bml,bmr,bmb,bmt,0,mx,my)) return false; }
			else { std::cout << "component over two borders b (q=" << qq << ")\n"; return false; }
		}
		for (i=a; i<=b; i++) 
		{ 
			if (y>0) { if (!this->seedfillCorners(i,y-1,bml, bmr, bmb, bmt,qq, mx, my)) return false; }
			else 
			{
				if (qq==3) { if (!this->seedfillCorners(i,sizeY()-1,bml, bmr, bmb, bmt, 1, mx, my)) return false; }
				else if (qq==2) { if (! this->seedfillCorners(i,sizeY()-1,bml, bmr, bmb, bmt, 0, mx, my)) return false; }
				else { std::cout<<"component over two borders bottom (q="<<qq<<", y="<<y<<")\n"; return false; }
			}
			if (y<sizeY()-1) { if (!this->seedfillCorners(i,y+1,bml,bmr,bmb,bmt,qq,mx,my)) return false; }
			else
			{
				if (qq==1) { if (!this->seedfillCorners(i,0,bml, bmr, bmb, bmt, 3, mx, my)) return false; }
				else if (qq==0) { if (!this->seedfillCorners(i,0,bml, bmr, bmb, bmt, 2, mx, my)) return false; }
				else { std::cout << "component over two borders top (q=" << qq << ", y=" << y<<")\n"; return false; }
			}
		} 
		return true;
	}

	void extract(std::vector<hvVec2<int> >  &ll, int step=100)
	{
		int i,j;
		for (i=0; i<sizeX(); i++) for (j=0; j<sizeY(); j++)
		{
			if (get(i,j))
			{
				if (ll.size()==ll.capacity()) ll.resize(ll.size()+step);
				ll.push_back(hvVec2<int>(i,j));
			}
		}

	}


	bool extractCC(hvBitmap &bm)
	{
		int i,j;
		bm.reset(sizeX(),sizeY(), false);
		for (i=0; i<sizeX(); i++) for (j=0; j<sizeY(); j++)
		{
			if (get(i,j))
			{
				hvBitmap bb; bb=*this;
				~bb;
				bb.seedfill(i,j,bm);
				return true;
			}
		}
		return false;
	}
	void mirror()
	{
		int i,j;
		for (i=0; i<sizeX(); i++) for (j=0; j<sizeY(); j++)
		{
			if (get(i,j))
			{
				if (sizeX()-i>0 && sizeX()-i<sizeX() && sizeY()-j>0 && sizeY()-j<sizeY()) set(sizeX()-i,sizeY()-j,true);
			}
		}
	}

	int extractCC(std::vector<hvBitmap *> &ll)
	{
		int num=0;
		hvBitmap bm; bm = *this;
		while (bm.count()>0)
		{
			hvBitmap *ccmask;
			int i,j;
			ccmask = new hvBitmap(sizeX(),sizeY(), false);
			bool cont=true;
			for (i=0; i<sizeX() && cont; i++) for (j=0; j<sizeY() && cont; j++)
			{
				if (bm.get(i,j))
				{
					~bm;
					bm.seedfill(i,j,*ccmask);
					~bm;
					ll.push_back(ccmask);
					num++;
					cont=false;
				}
			}
		}
		return num;
	}

	void holes()
	{
		int i;
		for (i=0; i<sizeX(); i++) seedfill(i,0);
		for (i=0; i<sizeX(); i++) seedfill(i,sizeY()-1);
		for (i=0; i<sizeY(); i++) seedfill(0,i);
		for (i=0; i<sizeY(); i++) seedfill(sizeX()-1,i);
		operator~();
	}
	void fillholes()
	{
		hvBitmap bm; bm = *this;
		bm.holes();
		operator|=(bm);
	}
	void holesw()
	{
		operator~();
		int i;
		for (i = 0; i<sizeX(); i++) seedfill(i, 0);
		for (i = 0; i<sizeX(); i++) seedfill(i, sizeY() - 1);
		for (i = 0; i<sizeY(); i++) seedfill(0, i);
		for (i = 0; i<sizeY(); i++) seedfill(sizeX(), i);
	}
	void fillholesw()
	{
		hvBitmap bm; bm = *this;
		bm.holesw();
		operator|=(bm);
	}



	// standard filter orperators
	void median(int kernelsx, int kernelsy)
	{
		hvBitmap bm; bm = *this;
		median(bm, kernelsx, kernelsy);
	}

	void median(const hvBoolArray2 &x, int kernelsx, int kernelsy)
	{
		int ksx = kernelsx/2, ksy = kernelsy/2;
		int i, j, ii, jj, npos, nneg;
		reset(x.sizeX(),x.sizeY(), false);
		for (i=0; i<x.sizeX(); i++)
		for (j=0; j<x.sizeY(); j++)
		{
			npos=0; nneg=0;
			for (ii=-ksx; ii<=ksx; ii++)
			for (jj=-ksy; jj<=ksy; jj++)
			{
				if (i+ii>=0 && i+ii<x.sizeX() && j+jj>=0 && j+jj<x.sizeY()) { if (x.get(i+ii,j+jj)) npos++; else nneg++; }
			}
			if (npos>=nneg) set(i,j, true); else set(i,j, false);
		}
	}
	void dilatation(int kernelsx, int kernelsy)
	{
		hvBitmap bm; bm = *this;
		dilatation(bm, kernelsx, kernelsy);
	}
	void dilatation(const hvBoolArray2 &x, int kernelsx, int kernelsy)
	{
		int ksx = kernelsx/2, ksy = kernelsy/2;
		int i, j, ii, jj, npos, nneg;
		reset(x.sizeX(),x.sizeY(), false);
		for (i=0; i<x.sizeX(); i++)
		for (j=0; j<x.sizeY(); j++)
		{
			npos=0; nneg=0;
			for (ii=-ksx; ii<=ksx; ii++)
			for (jj=-ksy; jj<=ksy; jj++)
			{
				if (sqrt((double)(ii*ii)+(double)(jj*jj))<=(ksx+ksy)/2)
				if (i+ii>=0 && i+ii<x.sizeX() && j+jj>=0 && j+jj<x.sizeY()) { if (x.get(i+ii,j+jj)) npos++; else nneg++; }
			}
			if (npos>=1) set(i,j,true); else set(i,j,false);
		}
	}
	void erosion(int kernelsx, int kernelsy)
	{
		hvBitmap bm; bm = *this;
		erosion(bm, kernelsx, kernelsy);
	}
	void erosion(const hvBoolArray2 &x, int kernelsx, int kernelsy)
	{
		int ksx = kernelsx/2, ksy = kernelsy/2;
		int i, j, ii, jj, npos, nneg;
		reset(x.sizeX(),x.sizeY(), false);
		for (i=0; i<x.sizeX(); i++)
		for (j=0; j<x.sizeY(); j++)
		{
			npos=0; nneg=0;
			for (ii=-ksx; ii<=ksx; ii++)
			for (jj=-ksy; jj<=ksy; jj++)
			{
				if (i+ii>=0 && i+ii<x.sizeX() && j+jj>=0 && j+jj<x.sizeY()) { if (x.get(i+ii,j+jj)) npos++; else nneg++; }
				else nneg++;
			}
			if (nneg>=1) set(i,j,false); else set(i,j,true);
		}
	}
	void erosionTorus(const hvBoolArray2 &x, int kernelsx, int kernelsy)
	{
		int ksx = kernelsx/2, ksy = kernelsy/2;
		int i, j, ii, jj, npos, nneg;
		reset(x.sizeX(),x.sizeY(), false);
		for (i=0; i<x.sizeX(); i++)
		for (j=0; j<x.sizeY(); j++)
		{
			npos=0; nneg=0;
			for (ii=-ksx; ii<=ksx; ii++)
			for (jj=-ksy; jj<=ksy; jj++)
			{
				int xx=i+ii; if (xx<0) xx+=x.sizeX(); else if (xx>=x.sizeX()) xx-=x.sizeX(); 
				int yy=j+jj; if (yy<0) yy+=x.sizeY(); else if (yy>=x.sizeY()) yy-=x.sizeY(); 
				if (x.get(xx,yy)) npos++; else nneg++;
			}
			if (nneg>=1) set(i,j,false); else set(i,j,true);
		}
	}
	void erosionTorus(const hvBoolArray2 &x, int kernelsx, int kernelsy, int minx, int miny, int maxx, int maxy)
	{
		int ksx = kernelsx/2, ksy = kernelsy/2;
		int i, j, ii, jj, npos, nneg;
		reset(x.sizeX(),x.sizeY(), false);
		for (i=minx; i<=maxx; i++)
		for (j=miny; j<=maxy; j++)
		{
			npos=0; nneg=0;
			for (ii=-ksx; ii<=ksx; ii++)
			for (jj=-ksy; jj<=ksy; jj++)
			{
				int xx=i+ii; if (xx<0) xx+=x.sizeX(); else if (xx>=x.sizeX()) xx-=x.sizeX(); 
				int yy=j+jj; if (yy<0) yy+=x.sizeY(); else if (yy>=x.sizeY()) yy-=x.sizeY(); 
				if (x.get(xx,yy)) npos++; else nneg++;
			}
			if (nneg>=1) set(i,j,false); else set(i,j,true);
		}
	}
	void erosion(const hvBoolArray2 &x, int kernelsx, int kernelsy, int startx, int starty, int endx, int endy)
	{
		int ksx = kernelsx/2, ksy = kernelsy/2;
		int i, j, ii, jj, npos, nneg;
		reset(x.sizeX(),x.sizeY(), false);
		for (i=startx; i<=endx; i++)
		for (j=starty; j<=endy; j++)
		{
			npos=0; nneg=0;
			for (ii=-ksx; ii<=ksx; ii++)
			for (jj=-ksy; jj<=ksy; jj++)
			{
				if (i+ii>=0 && i+ii<x.sizeX() && j+jj>=0 && j+jj<x.sizeY()) { if (x.get(i+ii,j+jj)) npos++; else nneg++; }
			}
			if (i>=0 && i<x.sizeX() && j>=0 && j<x.sizeY()) if (nneg>=1) set(i,j,false); else set(i,j,true);
		}
	}

	bool thinning()
	{
		hvBitmap bm; bm=*this;
		thinning(bm);
	}
	
	bool thinning(const hvBitmap &pi)
	{
		int i,j;
		hvBitmap pres; pres=pi;
		bool modif;
		int nnul;

		modif=false;
		*this = pres;
		for (i=1; i<pi.sizeX()-1; i++)
		for (j=1; j<pi.sizeY()-1; j++)
		{
				if ( pres.get(i,j) ) 
				{
					nnul=0; 
					for (int ii=0; ii<=2; ii++) for (int jj=0; jj<=2; jj++) if (pres.get(i+ii-1,j+jj-1)) { nnul++; } 
					if (nnul>2 && nnul!=9)
					{
						if ( (	!pres.get(i-1,j-1) && !pres.get(i,j-1) && !pres.get(i+1,j-1) &&
								pres.get(i-1,j+1) && pres.get(i,j+1) && pres.get(i+1,j+1) ) ||
							  (	!pres.get(i,j-1) && !pres.get(i+1,j-1) &&
								pres.get(i-1,j) && !pres.get(i+1,j) && pres.get(i,j+1) ) ||

							  (	!pres.get(i-1,j-1) && pres.get(i+1,j-1) && !pres.get(i-1,j) &&
								pres.get(i+1,j) && !pres.get(i-1,j+1) && pres.get(i+1,j+1) ) ||
							  (	!pres.get(i-1,j-1) && !pres.get(i,j-1) &&
								!pres.get(i-1,j) && pres.get(i+1,j) && pres.get(i,j+1) ) ||

							  (	pres.get(i-1,j-1) && pres.get(i,j-1) && pres.get(i+1,j-1) &&
								!pres.get(i-1,j+1) && !pres.get(i,j+1) && !pres.get(i+1,j+1) ) ||
							  (	pres.get(i,j-1) && !pres.get(i-1,j) &&
								pres.get(i+1,j) && !pres.get(i-1,j+1) && !pres.get(i,j+1) ) ||

							  (	pres.get(i-1,j-1) && !pres.get(i+1,j-1) && pres.get(i-1,j) &&
								!pres.get(i+1,j) && pres.get(i-1,j+1) && !pres.get(i+1,j+1) ) ||
							  (	pres.get(i,j-1) && pres.get(i-1,j) &&
								!pres.get(i+1,j) && !pres.get(i,j+1) && !pres.get(i+1,j+1) ) 
							)
							{
								set(i,j, false); modif=true;
							}
							else set(i,j, true);
					}
					else set(i,j, true);
				}
				else set(i,j, false);
		}
		return modif;
	}

	void skeleton(const hvBitmap &pi)
	{
		hvBitmap pres=pi;
		bool modif = true;
		do {
			if (thinning(pres)) pres = *this;
			else modif=false;
		} while(modif);
	}

	// drawing shapes

	void bresenham(int spx, int spy, int epx, int epy, bool val)
	{
		int i, dx, dy, delta=0, px=0, py=0;

		if (spx>epx) { i=spx; spx=epx; epx=i;  i=spy; spy=epy; epy=i; }
		dx = epx-spx; dy = epy-spy;
		if (dx>=(dy>0?dy:-dy))
		{
		if (dy>=0)
			{
			for (i=0; i<dx; i++)
				{
				if (delta<dx-dy)  delta += dy;
				else { delta += (-dx+dy); py++; }
				set(spx+i,spy+py, val);
				} 
			}
		else
			{
			dy = -dy;
			for (i=0; i<dx; i++)
				{
				if (delta<dx-dy)  delta += dy;
				else { delta += (-dx+dy); py--; }
				set(spx+i,spy+py, val);
				} 

			}
		}
		else
		{
			if (dy>=0)
			{
			for (i=0; i<dy; i++)
				{
				if (delta<dy-dx)  delta += dx;
				else { delta += (-dy+dx); px++; }
				set(spx+px,spy+i, val);
				} 
			}
			else
			{
			dy = -dy;
			for (i=0; i<dy; i++)
				{
				if (delta<dy-dx)  delta += dx;
				else { delta += (-dy+dx); px++; }
				set(spx+px,spy-i, val);
				} 
			}
		}
	}

	void drawRect(int spx, int spy, int dx, int dy, bool val)
	{
		int i,j;
		for (i=spx; i<spx+dx; i++)
		for (j=spy; j<spy+dy; j++)
		{
			set(i,j,val);
		}
	}

	void drawPolygon(const std::vector<hvVec2<int> > &vert, bool val=true)
	{
		hvVec2<int> vvs, vve;
		hvBitmap pres(sizeX(), sizeY(), false);
		int i;
		for (i=0; i<(int)vert.size();i++) 
		{
			vvs = vert.at(i);
			if (i+1==vert.size()) vve=vert.at(0); else vve = vert.at(i+1);
			pres.bresenham(vvs.X(), vvs.Y(), vve.X(), vve.Y(), true);
		}
		pres.fillholes();
		if (val) operator|=(pres);
		else { ~pres; operator&=(pres); }
	}

	void drawEllipse(int x, int y, int rx, int ry, double alpha, bool val=true)
	{
		double i,j;
		bool cont;

		for (i=0.0; i<=(double)rx; i+=0.5)
		{
			cont=true;
			for (j=0.0; j<=(double)ry && cont; j+=0.5)
			{
				double vv;
				if (rx!=0 && ry!=0) vv = i*i/(double)(rx*rx)+j*j/(double)(ry*ry);
				else if (rx==0) vv = j*j/(double)(ry*ry);
				else vv = i*i/(double)(rx*rx);
				if (vv <= 1.0)
				{
					int a = x+(int)(i*cos(alpha)-j*sin(alpha));
					int b = y+(int)(i*sin(alpha)+j*cos(alpha));
					if (a>=0 && a<sizeX() && b>=0 && b<sizeY()) set(a, b, val);
					a = x+(int)(-i*cos(alpha)-j*sin(alpha));
					b = y+(int)(-i*sin(alpha)+j*cos(alpha));
					if (a>=0 && a<sizeX() && b>=0 && b<sizeY()) set(a, b, val);
					a = x+(int)(-i*cos(alpha)+j*sin(alpha));
					b = y+(int)(-i*sin(alpha)-j*cos(alpha));
					if (a>=0 && a<sizeX() && b>=0 && b<sizeY()) set(a, b, val);
					a = x+(int)(i*cos(alpha)+j*sin(alpha));
					b = y+(int)(i*sin(alpha)-j*cos(alpha));
					if (a>=0 && a<sizeX() && b>=0 && b<sizeY()) set(a, b, val);
				}
				else cont=false;
			}
		}
	}

	// analysis and information extraction
	void extractBoxes(std::vector<hvVec2<int> > &blist, int sx, int sy) const
	{
		int i,j,ii,jj;
		for (i=0; i<this->sizeX()-sx; i+=2)
		for (j=0; j<this->sizeY()-sy; j+=2)
		{
			if (this->get(i,j))
			{
				bool cont=true;
				for (ii=i; ii<i+sx && cont; ii++)
				for (jj=j; jj<j+sy && cont; jj++)
				{
					if (!this->get(ii,jj)) cont=false;
				}
				blist.push_back(hvVec2<int>(i,j));
			}
		}
	}
	

	void box(hvVec2<int> &min, hvVec2<int> &max) const
	{
		min = hvVec2<int>(sizeX(), sizeY());
		max = hvVec2<int>(0,0);
		int i,j;
		for (i=0; i<sizeX(); i++)
		for (j=0; j<sizeY(); j++)
		{
			if (get(i,j))
			{
				hvVec2<int> vv(i,j);
				min.keepMin(min, vv);
				max.keepMax(max, vv);
			}
		}
	}

	void findMainAxis(int cx, int cy, double &alpha, int &dist)
	{
		int i,j;
		dist=0;
		for (i=0; i<sizeX(); i++) for (j=0; j<sizeY(); j++)
		{
			double dd = sqrt((double)((i-cx)*(i-cx)+(j-cy)*(j-cy)));
			if (get(i,j) && (int)dd>dist) { if (i==cx) alpha=M_PI/2.0; else alpha = atan((double)(j-cy)/(double)(i-cx)); dist=(int)dd; }
		}
	}
	int findBestMatchingEllipse(int cx, int cy, int &rx, int &ry, double &alpha)
	{
		int i,j, rad, nn, pas;
		double aa, amoy;
		int count = sizeX()*sizeY();

		findMainAxis(cx, cy, amoy, rad);
		if (amoy==0.0) { nn=0; pas=0; } else { nn = (int)(M_PI/amoy); pas = 32/nn; }
		//printf("main axis=%g at %d, nn=%d, pas=%d\n",amoy,rad,nn,pas);  
		//for (aa=(double)(pas-2)*M_PI/32.0; aa<=(double)(pas+2)*M_PI/32.0; aa+=M_PI/32.0)
		//for (i=rad-5; i<rad+5; i++)
		aa = amoy;
		i=rad;
		for (j=i/5; j<=i; j++)
		{
			if (i>=1)
			{
				hvBitmap be(sizeX(), sizeY(), false);
				be.drawEllipse(cx, cy, i,j, aa);
				be ^= *this;
				int cc = be.count();
				if (cc<count) { rx=i; ry=j; alpha=aa; count = cc; }
			}
		}
		//printf("Main ellipse: %d,%d, a=%g, count=%d\n",rx,ry,alpha, count);
		return count;
	}

	int findBestMatchingEllipseRing(int cx, int cy, int &rx, int &ry, double &alpha, double &ratio)
	{
		int k;
		int count;
		double rr;
		
		count=findBestMatchingEllipse(cx,cy,rx,ry,alpha);
		ratio=0.0;
		for (k=1; k<rx-1; k++)
		{
			rr = (double)k/(double)rx;
			hvBitmap be(sizeX(), sizeY(), false);
			be.drawEllipse(cx, cy, rx,ry, alpha);
			be.drawEllipse(cx, cy, (int)((double)rx*rr),(int)((double)ry*rr), alpha, false);
			be ^= *this;
			int cc = be.count();
			if (cc<count) { ratio=rr; count = cc; }
		}
		return count;
	}

	// other  operations
	// standard filter orperators
	/*
	void noiseDistortion(const hvBitmap &bm, double fx, double fy, int dx, int dy)
	{
		*this = bm;
		int i,j;
		for (i=0; i<sizeX(); i++)
		for (j=0; j<sizeY(); j++)
		{
			int px = i+(int)((double)dx*hvNoise::gnoise((double)i*(double)fx+23.987, (double)j*(double)fy-6.3327, 42.12345));
			int py = j+(int)((double)dy*hvNoise::gnoise((double)i*(double)fx-13.987, (double)j*(double)fy+36.3427, -22.345));
			if (px<0) px=0; else if (px>=sizeX()) px=sizeX()-1;
			if (py<0) py=0; else if (py>=sizeY()) py=sizeY()-1;
			set(i,j,bm.get(px,py));
		}
	}
	*/
	void randomPosition(bool state, hvVec2<int> &pos) const
	{
		int px, py;
		int count=0;
		do {
			count++;
			px=(int)((double)rand()/(double)RAND_MAX*(double)sizeX());
			if (px>=sizeX()) px=sizeX()-1;
			py=(int)((double)rand()/(double)RAND_MAX*(double)sizeY());
			if (py>=sizeY()) py=sizeY()-1;
			if (get(px,py)==state) { pos=hvVec2<int>(px,py); return; }
		} while (count<20);
		int i,j;
		for (j=py; j<sizeY(); j++) for (i=px; i<sizeX(); i++) if (get(i,j)==state) { pos=hvVec2<int>(i,j); return; } 
		for (j=py; j<sizeY(); j++) for (i=px; i>=0; i--) if (get(i,j)==state) { pos=hvVec2<int>(i,j); return; } 
		for (j=py; j>=0; j--) for (i=px; i<sizeX(); i++) if (get(i,j)==state) { pos=hvVec2<int>(i,j); return; } 
		for (j=py; j>=0; j--) for (i=px; i>=0; i--) if (get(i,j)==state) { pos=hvVec2<int>(i,j); return; } 
		hvFatal("no position possible in randomPosition");
	}
};



}

#endif // !efined(AFX_BITMAP_H__098453F0_1C38_49E9_A6F4_AABF90AA55E8__INCLUDED_)
