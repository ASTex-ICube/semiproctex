// transform.h: interface for the transform class.
//
// Uses a 4x4 matrix in homogenous coordinates to define linear transforms (rotation, shift, etc.)
//
// By JMD 9/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFORM_H__D8969113_4A06_4FBC_AADF_632871632A1E__INCLUDED_)
#define AFX_TRANSFORM_H__D8969113_4A06_4FBC_AADF_632871632A1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvMat4.h"

namespace hview {

	template <class T> class transform : public hvMat4<T>
	{
	public:
		// constructor: defines identity transform
		transform<T>() { }

		void setIdentity() { hvMat4<T>::i = hvVec4<T>(1, 0, 0, 0); hvMat4<T>::j = hvVec4<T>(0, 1, 0, 0); hvMat4<T>::k = hvVec4<T>(0, 0, 1, 0); hvMat4<T>::l = hvVec4<T>(0, 0, 0, 1); }

		// List of transforms: each is composed with the current one

		// shift of vector v
		void translation(const hvVec3<T> &v)
		{
			hvMat4<T> m(hvMat3<T>(), v);
			mult(*this, m);
		}

		// scale the three components by factors given by v
		void scale(const hvVec3<T> &v)
		{
			hvMat4<T> m(v);
			mult(*this, m);
		}

		// rotation along X axis by angle a in radiant
		void rotX(T a)
		{
			hvMat4<T> m(hvVec4<T>(1.0, 0.0, 0.0, 0.0),
				hvVec4<T>(0.0, cos(a), sin(a), 0.0),
				hvVec4<T>(0.0, -sin(a), cos(a), 0.0),
				hvVec4<T>(0.0, 0.0, 0.0, 1.0));
			mult(*this, m);
		}

		// rotation along Y axis by angle a in radiant
		void rotY(T a)
		{
			hvMat4<T> m(hvVec4<T>(cos(a), 0.0, -sin(a), 0),
				hvVec4<T>(0.0, 1.0, 0.0, 0.0),
				hvVec4<T>(sin(a), 0.0, cos(a), 0.0),
				hvVec4<T>(0.0, 0.0, 0.0, 1.0));
			mult(*this, m);
		}

		// rotation along Z axis by angle a in radiant
		void rotZ(T a)
		{
			hvMat4<T> m(hvVec4<T>(cos(a), sin(a), 0.0, 0.0),
				hvVec4<T>(-sin(a), cos(a), 0.0, 0.0),
				hvVec4<T>(0.0, 0.0, 1.0, 0.0),
				hvVec4<T>(0.0, 0.0, 0.0, 1.0));
			mult(*this, m);
		}

		// rotation along axis v by angle a in radiant
		// v must be normalized
		void rotation(const hvVec3<T> &v, T a)
		{
			hvMat3<T> m(v);
			hvMat3<T> s(hvVec3<T>(0.0, v.Z(), -v.Y()),
				hvVec3<T>(-v.Z(), 0.0, v.X()),
				hvVec3<T>(v.Y(), -v.X(), 0.0));
			s.scale(sin(a));
			hvMat3<T> t;
			hvMat3<T> n;
			n.sub(t, m);
			n.scale(cos(a));
			t.add(m, n);
			t.add(t, s);
			hvMat4<T> res(t);
			mult(*this, res);
		}

		// defines perspective projection: (l,b) resp. (r,t) define the bottom-left resp.
		// top-right corners, n and f define the near / far clipping planes
		// NOTE: n<f, l<r, b<t, n>0, f>0 
		// result of Apply is vec3 with x,y,z normalized between -1 and 1 if inside frustum
		// z represents the distance from eye, z>1 means farther than far plan,
		// z<-1 means closer or behind near plane, z=-INF means on eye plane
		// use only Apply for vector with z<0 
		void frustum(T l, T r, T b, T t, T n, T f)
		{
			hvMat4<T> res(hvVec4<T>(2.0*n / (r - l), 0.0, 0.0, 0.0),
				hvVec4<T>(0.0, 2.0*n / (t - b), 0.0, 0.0),
				hvVec4<T>((r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1.0),
				hvVec4<T>(0.0, 0.0, -2.0*f*n / (f - n), 0.0));
			this->mult(*this, res);
		}

		// defines orthographic projection: (l,b) resp. (r,t) define the bottom-left resp.
		// top-right corners, n and f define the near / far clipping planes
		// NOTE: n<f, l<r, b<t, n>0, f>0 
		// result of Apply is vec3 with x,y,z normalized between -1 and 1 if inside frustum
		// z represents the distance from eye plane, z>1 means farther than far plane,
		// z<-1 means closer or behind near plane
		// use only Apply for vector with z<0 
		void ortho(T l, T r, T b, T t, T n, T f)
		{
			hvMat4<T> res(hvVec4<T>(2.0 / (r - l), 0.0, 0.0, 0.0),
				hvVec4<T>(0.0, 2.0 / (t - b), 0.0, 0.0),
				hvVec4<T>(0.0, 0.0, -2.0 / (f - n), 0.0),
				hvVec4<T>(-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0));
			this->mult(*this, res);
		}

		// defines a change of vector frame given by:
		// vi,vj,vk -> new frame 
		// NOTE: vi,vj,vk must be orthogonal vectors
		void frame(const hvVec3<T> &vi, const hvVec3<T> &vj, const hvVec3<T> &vk)
		{
			hvMat3<T> mat(vi, vj, vk);
			mat.inverse();
			hvMat4<T> m1(mat);
			this->mult(*this, m1);
		}
		// defines a change of vector frame given by:
		// new frame  -> vi,vj,vk
		// NOTE: vi,vj,vk must be orthogonal vectors
		void inverseframe(const hvVec3<T> &vi, const hvVec3<T> &vj, const hvVec3<T> &vk)
		{
			hvMat3<T> mat(vi, vj, vk);
			hvMat4<T> m1(mat);
			this->mult(*this, m1);
		}

		// defines a change of frame given by:
		// eye -> eye position
		// at -> focus point
		// up -> up vector to define a full local frame on eye
		// vector(at, eye) defines Z axis, up defines Y axis, and X axis is computed
		// from cross product
		// NOTE: up must be normalized and perpendicular to vector(at, eye), and  at /= eye
		void lookAt(const hvVec3<T> &eye, const hvVec3<T> &at, const hvVec3<T> &up)
		{
			hvVec3<T> zaxis; zaxis.PVec(at, eye);
			zaxis.normalize(zaxis.norm());
			hvVec3<T> xaxis;
			xaxis.cross(up, zaxis);
			xaxis.normalize(xaxis.norm());

			hvVec3<T> io = eye; io.reverse();
			hvMat4<T> m1(hvMat3<T>(), io);
			hvMat3<T> ba(xaxis, up, zaxis);
			hvMat3<T> r; r.inverse(ba, ba.det());
			hvMat4<T> m2(r);
			this->mult(*this, m2);
			this->mult(*this, m1);
		}

		// compose two transforms with each other
		void compose(const transform<T> &a, const transform<T> &b) { this->mult(a, b); }
		// compose two transforms with each other
		void compose(const transform<T> &a) { this->operator*=(a); }

		// apply a transform to a 3D point, result is a 3D point
		hvVec3<T> apply(const hvVec3<T> &v) const { hvVec4<T> u(v); return (hvVec3<T>)this->mult(u); }
		hvVec4<T> apply(const hvVec4<T> &v) const { return this->mult(v); }

		// apply transform to a vector, result is a new vector
		// this is the same as for point, but no translation is applied
		// NOTE: the norm of resulting vector is not preserved
		hvVec3<T> applyVec3(const hvVec3<T> &v) const
		{
			hvMat4<T> m = *this;
			hvMat3<T> mm = (hvMat3<T>)m;
			return mm.mult(v);
		}

		template <class U> void getMatrix(U mm[16]) const
		{
			mm[0] = U(hvMat4<T>::i.X()); mm[1] = U(hvMat4<T>::i.Y()); mm[2] = U(hvMat4<T>::i.Z()); mm[3] = U(hvMat4<T>::i.W());
			mm[4] = U(hvMat4<T>::j.X()); mm[5] = U(hvMat4<T>::j.Y()); mm[6] = U(hvMat4<T>::j.Z()); mm[7] = U(hvMat4<T>::j.W());
			mm[8] = U(hvMat4<T>::k.X()); mm[9] = U(hvMat4<T>::k.Y()); mm[10] = U(hvMat4<T>::k.Z()); mm[11] = U(hvMat4<T>::k.W());
			mm[12] = U(hvMat4<T>::l.X()); mm[13] = U(hvMat4<T>::l.Y()); mm[14] = U(hvMat4<T>::l.Z()); mm[15] = U(hvMat4<T>::l.W());
		}

	};

}
#endif // !defined(AFX_TRANSFORM_H__D8969113_4A06_4FBC_AADF_632871632A1E__INCLUDED_)
