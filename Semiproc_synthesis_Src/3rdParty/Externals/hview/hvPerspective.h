/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// perspect.h: interface for the perspective class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERSPECT_H__BB034B5A_4CC1_4FBC_B131_9AECA8B630DB__INCLUDED_)
#define AFX_PERSPECT_H__BB034B5A_4CC1_4FBC_B131_9AECA8B630DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvFrame3.h"
#include "hvTransform.h"
#include "hvLine3.h"

namespace hview {

	// Defines a perspective projection in 3D space: a linear transform that changes a 3D point into another 3D point belonging to a projection
	// plan. The result of projection is a point P(x,y,z) where x,y,z are normalized between -1 and 1
	// (x,y) defines a 2D vector according to a 2D frame located on the projection plane
	// z is a distance measurement of the projected point towards the projection plane (it is normalized according
	// to a maximal distance called farplane)

	template <class T> class perspective : public transform<T>
	{
	protected:
		hvFrame3<T>			eyeframe;
		hvVec3<T>				lookat;
		T					width, height, focus, farplane;

	public:
		perspective<T>() : eyeframe(), lookat() { width = T(0.0); height = T(0.0); focus = T(0.0); farplane = T(1.0); }

		perspective<T>(const hvVec3<T> &eye, const hvVec3<T> &at, const hvVec3<T> &up, T w, T h, T n, T f)
		{
			hvVec3<T> zaxis; zaxis.PVec(eye, at);
			zaxis.normalize(zaxis.norm());
			hvVec3<T> xaxis;
			xaxis.cross(up, zaxis);
			xaxis.normalize(xaxis.norm());
			zaxis.reverse();
			eyeframe = hvFrame3<T>(eye, xaxis, up, zaxis);
			lookat = at;
			width = w; height = h; focus = n; farplane = f;
			this->frustum(-w / T(2.0), w / T(2.0), -h / T(2.0), h / T(2.0), n, f);
			this->lookAt(eye, at, up);
		}

		// selectors
		hvVec3<T> getOrigin() const { return eyeframe.origin(); }
		hvFrame3<T> getFrame() const { return eyeframe; }
		hvVec3<T> getLookAt() const { return lookat; }

		bool project(const hvVec3<T> &p, hvVec3<T> &res) const
		{
			hvVec3<T> pv; pv.PVec(eyeframe.origin(), p);
			if (pv.dot(eyeframe.K()) >= 0.0) return false;
			hvVec4<T> u(p);
			hvVec4<T> v = this->mult(u);
			if (v.W() == T(0.0)) return false;
			res = hvVec3<T>(v);
			return true;
		}

		line3<T> reverse(const hvVec3<T> &p) const
		{
			hvVec3<T> r = eyeframe.K();
			r.scale(-focus);
			hvVec3<T> xx = eyeframe.I();
			xx.scale(p.X()*width / T(2.0));
			hvVec3<T> yy = eyeframe.J();
			yy.scale(p.Y()*height / T(2.0));
			r += xx; r += yy;
			r.normalize(r.norm());
			return line3<T>(eyeframe.origin(), r);
		}

	};

}
#endif // !defined(AFX_MONOPERSPECT_H__BB034B5A_4CC1_4FBC_B131_9AECA8B630DB__INCLUDED_)
