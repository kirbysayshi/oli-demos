#ifndef OLI_MESH_H
#define OLI_MESH_H


/*
#include <Mesh.h>
*/

/*
------------------------------------------------------------------
File: Mesh.h
Started: 18/01/2004 16:09:27
  
$Header: $
$Revision: $
$Locker: $
$Date: $
  
Author: Olivier renault
------------------------------------------------------------------
Module: 
Description: 
------------------------------------------------------------------
$History: $
------------------------------------------------------------------
*/

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "C:\dev\oli\GLut\glut.h"
#include "Vector.h"
#include "Particle.h"
#include "Body.h"

//-------------------------------------------------
// we can use the normal at vertex to smooth out the collsion
// response, by pushing the sphere along the vertex
// normal until no penetration. That will avoid 'sticky'
// collsions of spheres against vertices, where objects
// bump on flat vertices. 
//-------------------------------------------------
#define SMOOTH_CORNERS

//-------------------------------------------------
// Vertex class. 
// Has a normal as well as a position
// We can use the normal to smooth out collsion response
// when the vertex is 'flat'. 
// also, it avoids objects getting stuck inside meshes,
// as it pushes vertices outward. 
//-------------------------------------------------
class CVertex
{
public:
	CVertex();

	CVertex(const Vector& xPos);

	CVertex(const Vector& xPos, const Vector& xNorm);

	//-------------------------------------------------
	// collide with a particle
	// simple radius check.
	// then, we push the sphere away. 
	// we can use the normal to smooth out the collsion
	// response, by pushing the sphere along the vertex
	// normal until no penetration. That will avoid 'sticky'
	// collsions of spheres against vertices, where objects
	// 'bump. on flat vertices. 
	//-------------------------------------------------
	bool Collide(CParticle& xParticle) const;

public:
	Vector m_xPosition;
	Vector m_xNormal;
};



//------------------------------------------------------------------	
// Static meshes. loop of vertices.
//------------------------------------------------------------------
class CMesh
{
public:
	CMesh();

	virtual ~CMesh();

	//--------------------------------------------------------------------------
	// Set closed loop of vertices. if necessary, invert the winding order
	//--------------------------------------------------------------------------
	void SetVertices(const Vector* xVertices, int iNumVertices, bool bInvert);

	      CVertex& GetVertex     (int i)       { return m_xVertices[i]; }
	const CVertex& GetVertex     (int i) const { return m_xVertices[i]; }
	int            GetNumVertices(int i) const { return m_iNumVertices; }

	//------------------------------------------------------------------
	// test collsion with a body
	//------------------------------------------------------------------
	bool Collide(CBody& xBody) const;

	//------------------------------------------------------------------
	// Collide with particle in a body
	//------------------------------------------------------------------
	bool Collide(CParticle& xParticle) const;

	//------------------------------------------------------------------
	// Collide a particle with an edge
	//------------------------------------------------------------------
	bool Collide(CParticle& xParticle, const CVertex& V0, const CVertex& V1) const;

	void Render() const;

private:
	CVertex*	m_xVertices;
	int			m_iNumVertices;
	
	float			m_fBoundingRad;
	Vector			m_xBoundingPos;

	//--------------------------------------------
	// General bounding sphere around the body 
	// (for culling)
	//--------------------------------------------
	void ComputeBoundingSphere();

	//-----------------------------------------------------------
	// Checks if two particle's bounding vilume intersect
	// simple radius check test
	//-----------------------------------------------------------
	bool BoundingVolumesIntersect(const CBody& xBody) const;
	void Free();

	void Allocate(int iNumVertices);

	void RenderBoundingSphere() const;

};

#endif OLI_MESH_H