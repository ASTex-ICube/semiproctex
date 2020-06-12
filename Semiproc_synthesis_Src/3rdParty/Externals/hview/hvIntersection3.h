/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// intersection3.h: interface for the intersection 3D class.
//
// intersection3<T>: is an Intersection computed in dimension 3
// An intersection is composed of two elements: a set of points  and a number of intersections.
// 
// By JMD 22/5/07
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERSECTION3_H__EC87CD56_6E08_4390_B876_CEC6D44EEA86__INCLUDED_)
#define AFX_INTERSECTION3_H__EC87CD56_6E08_4390_B876_CEC6D44EEA86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvLine3.h"
#include "hvPlan3.h"

namespace hview {

	template <class T> class intersection3
	{
	protected:
		hvVec3<T>		pt[2];	// intersection points
		T			dist[2];	// distance of point according to line vector
		int			n;		// number of intersections
	public:
		// constructors
		intersection3<T>() : n(0) { }
		intersection3<T>(const hvVec3<T> &x) { pt[0] = x; dist[0] = T(0); n = 1; }
		intersection3<T>(const hvVec3<T> &x, const hvVec3<T> &y) { pt[0] = x; dist[0] = T(0); pt[1] = y; dist[1] = T(0); n = 2; }
		intersection3<T>(const hvVec3<T> &x, T dd) { pt[0] = x; dist[0] = dd; n = 1; }
		intersection3<T>(const hvVec3<T> &x, T dx, const hvVec3<T> &y, T dy) { pt[0] = x; dist[0] = dx; pt[1] = y; dist[1] = dy; n = 2; }

		// selectors
		int count() const { return n; }

		hvVec3<T> &operator[](int i)
		{
			if (i < 0 || i >= n) { printf("no %d intersection point available (%d)!\n", i, n); return pt[0]; }
			return pt[i];
		}
		T distance(int i)
		{
			if (i < 0 || i >= n) { printf("no distance %d intersection available (%d)!\n", i, n); return T(-1); }
			return dist[i];
		}

		hvVec3<T> valueOf() const { return pt[0]; }
		operator hvVec3<T>() { return pt[0]; }
		operator T() { return dist[0]; }

		// compute intersection between two lines
		// works only if lines belong to same plane
		// returns false if lines are parallel
		bool intersect(const line3<T> &li1, const line3<T> &li2)
		{
			hvVec3<T> v; v.cross(li1.direction(), li2.direction());
			if (v.isNull()) { n = 0; return false; }
			hvVec3<T> w = v; w.abs();
			hvVec3<T> dd; dd.PVec(li2.origin(), li1.origin());
			hvVec3<T> r; r.cross(li2.direction(), dd);
			T l;
			if (w.X() > w.Y() && w.X() > w.Z())
			{
				l = r.X() / v.X();
			}
			else if (w.Y() > w.Z())
			{
				l = r.Y() / v.Y();
			}
			else l = r.Z() / v.Z();
			pt[0] = li1.pointOfLine(l);
			dist[0] = l;
			n = 1;
			return true;
		}

		// compute intersection between line and plane
		// returns false if line li is quasi parallel to plane (no intersection)
		bool intersect(const line3<T> &li, const plan3<T> &pl, T err)
		{
			T d = li.direction().dot(pl.normal());
			if (d >= -err && d <= err) { n = 0; return false; }
			T u = -pl.pointInPlan(li.origin());
			T ll = u / d;
			pt[0] = li.pointOfLine(ll);
			dist[0] = ll;
			n = 1;
			return true;
		}

		// compute intersection between line and plane
		// returns false if line li is parallel to plane (no intersection)
		bool intersect(const line3<T> &li, const plan3<T> &pl)
		{
			T d = li.direction().dot(pl.normal());
			if (d == (T)0) { n = 0; return false; }
			T u = -pl.pointInPlan(li.origin());
			T ll = u / d;
			pt[0] = li.pointOfLine(ll);
			dist[0] = ll;
			n = 1;
			return true;
		}

		// compute intersection between ray and plane
		// returns false if line li is parallel to plane (no intersection)
		// Note: line is oriented, the intersection is ignored if "behind" the line origin
		bool intersectRay(const line3<T> &li, const plan3<T> &pl)
		{
			T d = li.direction().dot(pl.normal());
			if (d == (T)0) { n = 0; return false; }
			T u = -pl.pointInPlan(li.origin());
			T ll = u / d;
			if (ll < T(0)) { n = 0; return false; }
			pt[0] = li.pointOfLine(ll);
			dist[0] = ll;
			n = 1;
			return true;
		}

		// compute intersection between ray and plane
		// returns false if line li is parallel to plane (no intersection)
		// Note: line is oriented, the intersection is ignored if "behind" the line origin
		bool intersectRay(const line3<T> &li, const plan3<T> &pl, T err)
		{
			T d = li.direction().dot(pl.normal());
			if (d >= err && d <= err) { n = 0; return false; }
			T u = -pl.pointInPlan(li.origin());
			T ll = u / d;
			if (ll < T(0)) { n = 0; return false; }
			pt[0] = li.pointOfLine(ll);
			dist[0] = ll;
			n = 1;
			return true;
		}

		// compute intersection between a segment (two points a and b) and a plan
		// NOTE: a /= b
		// err allows a percentage error, that is the segment is elongated by err% on each side
		// returns false if there is no intersection
		bool intersect(const hvVec3<T> &a, const hvVec3<T> &b, const plan3<T> &pl, T err)
		{
			hvVec3<T> dir; dir.PVec(a, b);
			if (dir.isNull()) { n = 0; return false; }
			T no = dir.norm();
			T d = dir.dot(pl.normal());
			if (d == T(0)) { n = 0; return false; }
			T u = -pl.pointInPlan(a);
			T ll = u / d / no;
			if (ll<-err || ll>err + T(1)) { n = 0; return false; }
			dir.scale(ll*no);
			pt[0].add(a, dir);
			dist[0] = ll;
			n = 1;
			return true;
		}

		// compute intersection between a ray and a box
		// returns the number of intersection points (0 if there is no intersection)
		int intersectRay(const line3<T> &li, const hvVec3<T> &bbmin, const hvVec3<T> &bbmax)
		{
			T l, lb1 = T(0), lb2 = T(0), dd1, dd2, dd3;
			hvVec3<T> pp;

			n = 0;
			if ((li.direction().X() > T(0)) && (bbmax.X() < li.origin().X())) return 0;
			if ((li.direction().X() < T(0)) && (bbmin.X() > li.origin().X())) return 0;
			if ((li.direction().Y() > T(0)) && (bbmax.Y() < li.origin().Y())) return 0;
			if ((li.direction().Y() < T(0)) && (bbmin.Y() > li.origin().Y())) return 0;
			if ((li.direction().Z() > T(0)) && (bbmax.Z() < li.origin().Z())) return 0;
			if ((li.direction().Z() < T(0)) && (bbmin.Z() > li.origin().Z())) return 0;
			if (li.direction().X() != T(0))
			{
				l = (bbmin.X() - li.origin().X()) / li.direction().X();
				if (l > T(0))
				{
					pp = li.pointOfLine(l);
					if ((pp.Y() >= bbmin.Y()) && (pp.Y() <= bbmax.Y())
						&& (pp.Z() >= bbmin.Z()) && (pp.Z() <= bbmax.Z()))
					{
						n = 1; pt[0] = pp; lb1 = l;
					}
				}
				l = (bbmax.X() - li.origin().X()) / li.direction().X();
				if (l > T(0))
				{
					pp = li.pointOfLine(l);
					if ((pp.Y() >= bbmin.Y()) && (pp.Y() <= bbmax.Y())
						&& (pp.Z() >= bbmin.Z()) && (pp.Z() <= bbmax.Z()))
					{
						if (n == 0) { n = 1; pt[0] = pp; lb1 = l; }
						else { n = 2; pt[1] = pp; lb2 = l; }
					}
				}
			}
			if (li.direction().Y() != T(0))
			{
				l = (bbmin.Y() - li.origin().Y()) / li.direction().Y();
				if (l > T(0))
				{
					pp = li.pointOfLine(l);
					if ((pp.X() >= bbmin.X()) && (pp.X() <= bbmax.X())
						&& (pp.Z() >= bbmin.Z()) && (pp.Z() <= bbmax.Z()))
					{
						if (n == 0) { n = 1; pt[0] = pp; lb1 = l; pt[1] = pp; lb2 = l; }
						else if (n == 1) { n = 2; pt[1] = pp; lb2 = l; }
						else
						{
							dd1 = lb1 - l; if (dd1 < T(0)) dd1 = -dd1;
							dd2 = lb2 - l; if (dd2 < T(0)) dd2 = -dd2;
							dd3 = lb2 - lb1; if (dd3 < T(0)) dd3 = -dd3;
							if (dd1 > dd3) { n = 2; pt[1] = pp; lb2 = l; }
							else if (dd2 > dd3) { n = 2; pt[0] = pp; lb1 = l; }
						}
					}
				}
				l = (bbmax.Y() - li.origin().Y()) / li.direction().Y();
				if (l > T(0))
				{
					pp = li.pointOfLine(l);
					if ((pp.X() >= bbmin.X()) && (pp.X() <= bbmax.X())
						&& (pp.Z() >= bbmin.Z()) && (pp.Z() <= bbmax.Z()))
					{
						if (n == 0) { n = 1; pt[0] = pp; lb1 = l; pt[1] = pp; lb2 = l; }
						else if (n == 1) { n = 2; pt[1] = pp; lb2 = l; }
						else
						{
							dd1 = lb1 - l; if (dd1 < T(0)) dd1 = -dd1;
							dd2 = lb2 - l; if (dd2 < T(0)) dd2 = -dd2;
							dd3 = lb2 - lb1; if (dd3 < T(0)) dd3 = -dd3;
							if (dd1 > dd3) { n = 2; pt[1] = pp; lb2 = l; }
							else if (dd2 > dd3) { n = 2; pt[0] = pp; lb1 = l; }
						}
					}
				}
			}
			if (li.direction().Z() != T(0))
			{
				l = (bbmin.Z() - li.origin().Z()) / li.direction().Z();
				if (l > T(0))
				{
					pp = li.pointOfLine(l);
					if ((pp.X() >= bbmin.X()) && (pp.X() <= bbmax.X())
						&& (pp.Y() >= bbmin.Y()) && (pp.Y() <= bbmax.Y()))
					{
						if (n == 0) { n = 1; pt[0] = pp; lb1 = l; pt[1] = pp; lb2 = l; }
						else if (n == 1) { n = 2; pt[1] = pp; lb2 = l; }
						else
						{
							dd1 = lb1 - l; if (dd1 < T(0)) dd1 = -dd1;
							dd2 = lb2 - l; if (dd2 < T(0)) dd2 = -dd2;
							dd3 = lb2 - lb1; if (dd3 < T(0)) dd3 = -dd3;
							if (dd1 > dd3) { n = 2; pt[1] = pp; lb2 = l; }
							else if (dd2 > dd3) { n = 2; pt[0] = pp; lb1 = l; }
						}
					}
				}
				l = (bbmax.Z() - li.origin().Z()) / li.direction().Z();
				if (l > T(0))
				{
					pp = li.pointOfLine(l);
					if ((pp.X() >= bbmin.X()) && (pp.X() <= bbmax.X())
						&& (pp.Y() >= bbmin.Y()) && (pp.Y() <= bbmax.Y()))
					{
						if (n == 0) { n = 1; pt[0] = pp; lb1 = l; pt[1] = pp; lb2 = l; }
						else if (n == 1) { n = 2; pt[1] = pp; lb2 = l; }
						else
						{
							dd1 = lb1 - l; if (dd1 < T(0)) dd1 = -dd1;
							dd2 = lb2 - l; if (dd2 < T(0)) dd2 = -dd2;
							dd3 = lb2 - lb1; if (dd3 < T(0)) dd3 = -dd3;
							if (dd1 > dd3) { n = 2; pt[1] = pp; lb2 = l; }
							else if (dd2 > dd3) { n = 2; pt[0] = pp; lb1 = l; }
						}
					}
				}
			}
			dist[0] = lb1; dist[1] = lb2;
			if (n == 1) { dist[1] = dist[0]; pt[1] = pt[0]; }
			return(n);
		}
	};

}

#endif // !defined(AFX_LINE3_H__EC87CD56_6E08_4390_B876_CEC6D44EEA86__INCLUDED_)
