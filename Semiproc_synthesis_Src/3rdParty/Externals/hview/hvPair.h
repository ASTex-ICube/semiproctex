/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hvPair.h: interface for the pair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAIR_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
#define AFX_PAIR_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvError.h"

namespace  hview {

template <class T, class U> class hvPair  
{
protected:
	T			left;
	U			right;

public:
	hvPair() { left=T(0); right=U(0); }
	hvPair(const T &x, const U &y) { left=x; right=y; }

	void setLeft(const T &x) { left=x; }
	void setRight(const U &x) { right=x; }
	T getLeft() { return left; }
	U getRight() { return right; }
	bool operator==(const hvPair<T,U> &pp) const
	{
		return left==pp.left && right==pp.right;
	}

	// complex numbers
	double mod() { return sqrt((double)left*(double)left+(double)right*(double)right); }
	double energy() { return (double)left*(double)left+(double)right*(double)right; }
	double phase()
	{
	double rr, r = (double)left, i = (double)right;

	if (r==0.0 && i==0.0) return(0.0);
	if ((r>0.0?r:-r)>(i>0.0?i:-i))
		{
		rr = i/r;
		if (r<0.0) rr = M_PI+atan(rr);
		else rr = atan(rr);
		}
	else
		{
		rr = r/i;
		if (i>0.0) rr = M_PI/2.0-atan(rr);
		else rr = 3.0*M_PI/2.0-atan(rr);
		}
	if (rr>M_PI) return(rr-2.0*M_PI);
	else if (rr<-M_PI) return(rr+2.0*M_PI);
	return(rr);
	}

};

}
#endif // !defined(AFX_PAIR_H__09002DDD_2472_43B9_B7B6_FCB6FF1B6B0D__INCLUDED_)
