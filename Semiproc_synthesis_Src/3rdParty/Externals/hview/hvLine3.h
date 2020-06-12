/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// line3.h: interface for the line3 class.
//
// line3 defines a line in a 3D space: L= O+Dl, where O is origin and D direction
// 
// By JMD 10/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE3_H__EC87CD56_6E08_4390_B876_CEC6D44EEA86__INCLUDED_)
#define AFX_LINE3_H__EC87CD56_6E08_4390_B876_CEC6D44EEA86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvVec3.h"

namespace hview {

	template <class T> class line3
	{
	protected:
		hvVec3<T>		o;	// origin
		hvVec3<T>		d;	// direction
	public:
		// constructors
		line3<T>() { }

		// defines a line by a point pt (origin) and a vector dir (direction)
		// dir must be normalized
		line3<T>(const hvVec3<T> &pt, const hvVec3<T> &dir) { o = pt; d = dir; }

		// defines a line from origin (0,0,0) in direction dir
		// dir must be normalized
		line3<T>(const hvVec3<T> &dir) : o(T(0)) { d = dir; }

		// selectors
		hvVec3<T> origin() const { return o; }
		hvVec3<T> direction() const { return d; }

		// Compute a point (result) on the line at distance l from the origin
		hvVec3<T> pointOfLine(T l) const
		{
			hvVec3<T> p; p.scale(d, l);
			p += o;
			return p;
		}

		void reverse() { d.reverse(); }

	};

}
#endif // !defined(AFX_LINE3_H__EC87CD56_6E08_4390_B876_CEC6D44EEA86__INCLUDED_)
