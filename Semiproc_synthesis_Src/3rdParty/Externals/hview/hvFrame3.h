/*
 * Code author: Jean-Michel Dischler
 */

/**
 * @version 1.0
 */

// hvFrame3.h: interface for the frame3 class.
//
// hvFrame3: defines a frame for an euclidian 3D space
// a frame is composed of an origin point and 3 orthonormal vectors
// frame3 is composed of a vec3<T> and a mat3<T>
// main operations allow to change points and vectors from one frame to another
// by extracting transfer matrices
//
// By JMD 10/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAME3_H__824D3162_521A_4FD7_9D86_19B3BA026926__INCLUDED_)
#define AFX_FRAME3_H__824D3162_521A_4FD7_9D86_19B3BA026926__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hvMat4.h"

namespace hview { 

template <class T> class hvFrame3 
{
protected:

	hvVec3<T> ori;
	hvMat3<T> mat;

public:
	// constructors
	hvFrame3<T>():mat(), ori(hvVec3<T>(T(0),T(0),T(0))) { }
	hvFrame3(const hvVec3<T> &o, const hvVec3<T> &x, const hvVec3<T> &y, const hvVec3<T> &z) : mat(x,y,z),ori(o) { }
	hvFrame3(const hvVec3<T> &o):mat(), ori(o) { }
	hvFrame3(const hvVec3<T> &o, const hvMat3<T> &m):mat(m), ori(o) { }

	void setOrigin(const hvVec3<T> &o) { ori=o; }

	// selectors
	hvVec3<T> origin() const { return ori; }
	hvVec3<T> I() const { return mat.I(); }
	hvVec3<T> J() const { return mat.J(); }
	hvVec3<T> K() const { return mat.K(); }


	hvVec3<T> coordinates(const hvVec3<T> &pt) const
	{
		hvVec3<T> p; p.PVec(ori,pt);
		return hvVec3<T>(p.dot(this->I())/ this->I().normSquared(),p.dot(this->J())/ this->J().normSquared(),p.dot(this->K())/ this->K().normSquared());
	}

	T det() const { return mat.det(); }
	bool operator==(const hvFrame3<T> &f) const
	{
		return ori==f.ori && mat==f.mat;
	}
	// Compute change of frame matrix (dimension 3) for vectors
	// Resulting matrix allows to express a vector given in base coordinates :
	// (1,0,0);(0,1,0);(0,0,1) into the frame coordinates  
	hvMat3<T> changeToFrame3(T det) const
	{
		hvMat3<T> r;
		r.inverse(mat, det);
		return r;
	}

	// Compute change of frame matrix (dimension 3) for vectors
	// Resulting matrix allows to express a vector given in frame coordinates
	// into basic coordinates, e.g. (1,0,0);(0,1,0);(0,0,1)   
	hvMat3<T> changeFromFrame3() const 
	{
		return mat;
	}

	// Compute change of frame matrix (dimension 4) for points
	// Resulting matrix allows to express a point given in base coordinates :
	// O(0,0,0);i(1,0,0);j(0,1,0);k(0,0,1) into the frame coordinates  
	hvMat4<T> changeToFrame4(T det) const 
	{
		hvVec3<T> io=ori; io.reverse();
		hvMat4<T> m1(hvMat3<T>(), io);
		hvMat3<T> r=this->changeToFrame3(det);
		hvMat4<T> m2(r);
		m1.mult(m1, m2);
		return m1;
	}

	// Compute change of frame matrix (dimension 4) for points
	// Resulting matrix allows to express a point given in frame coordinates
	// into basic coordinates, e.g. O(0,0,0);i(1,0,0);j(0,1,0);k(0,0,1)   
	hvMat4<T> changeFromFrame4() const
	{
		hvMat3<T> r=this->changeFromFrame3();
		hvMat4<T> m1(r);
		hvVec3<T> io=ori;
		hvMat4<T> m2(hvMat3<T>(), io);
		m1.mult(m1, m2);
		return m1;
	}

};

}


#endif // !defined(AFX_FRAME3_H__824D3162_521A_4FD7_9D86_19B3BA026926__INCLUDED_)
