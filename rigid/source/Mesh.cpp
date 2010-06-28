
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "..\..\GLut\glut.h"

#include "Mesh.h"

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
CVertex::CVertex()
{}

CVertex::CVertex(const Vector& xPos)
: m_xPosition(xPos)
, m_xNormal(Vector::Blank())
{}

CVertex::CVertex(const Vector& xPos, const Vector& xNorm)
: m_xPosition(xPos)
, m_xNormal(xNorm)
{}

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
bool CVertex::Collide(CParticle& xParticle) const
{
	//-------------------------------------------------
	// radius check
	//-------------------------------------------------
	Vector D(xParticle.m_xP1 - m_xPosition);

	float d2 = D * D;

	float r  = xParticle.m_fRadius;
	float r2 = r * r;

	if (d2 > r2)
		return false;

	float t;

#ifdef SMOOTH_CORNERS
	if (RaySphereIntersection(xParticle.m_xP1, xParticle.m_fRadius, m_xPosition, -m_xNormal, 0.0f, 1000.0f, t))
	{
		return xParticle.CollisionResponse(m_xNormal, t);
	}
	else
#endif
	{
		//-------------------------------------------------
		// Calculate the relative direction of article to vertex
		//-------------------------------------------------
		float d = (float) sqrt(d2);

		D /= d;

		//-------------------------------------------------
		// Calculate the depth of intersection along that vector
		//-------------------------------------------------
		float depth = r - d;

		//-------------------------------------------------
		// Push the sphere away.
		// as said, you can try to find the depth of intersection 
		// along the vertex normal, and push the sphere that way, 
		// to smooth out response. 
		// but it's simpler to not bother. 
		//-------------------------------------------------
		return xParticle.CollisionResponse(D, depth);
	}
	return true;
}



//------------------------------------------------------------------	
// Static meshes. loop of vertices.
//------------------------------------------------------------------
CMesh::CMesh()
: m_xVertices(NULL)
, m_iNumVertices(0)
, m_fBoundingRad(0.0f)
, m_xBoundingPos(Vector::Blank())
{}

CMesh::~CMesh()
{
	Free();
}

//--------------------------------------------------------------------------
// Set closed loop of vertices. if necessary, invert the winding order
//--------------------------------------------------------------------------
void CMesh::SetVertices(const Vector* xVertices, int iNumVertices, bool bInvert)
{
	Allocate(iNumVertices);

	//-------------------------------------------------
	// copy the vertices
	//-------------------------------------------------
	for(int i = 0; i < iNumVertices; i ++)
	{
		int j = i;

		if (bInvert)
			j = (iNumVertices - 1 - i);

		m_xVertices[j] = CVertex(xVertices[i]);
	}

	//----------------------------------------
	// compute vertex normals using the 
	// vector from previous vertex to next
	// vertex in list, and taking the perpendicular
	// vector.
	//----------------------------------------
	for(int i = 0; i < iNumVertices; i ++)
	{
		int j = (i - 1);
		int k = (i + 1);
		if (j < 0) j += iNumVertices;
		if (k >= iNumVertices) k -= iNumVertices;

		Vector P = m_xVertices[j].m_xPosition;
		Vector Q = m_xVertices[k].m_xPosition;

		Vector E = (Q - P);
		Vector N = Vector(-E.y, E.x);

#ifndef SMOOTH_CORNERS
		N.Normalise();
#endif

		m_xVertices[i].m_xNormal += N;
	}
	//----------------------------------------
	// Normalise the vertices normals
	//----------------------------------------
	for(int i = 0; i < m_iNumVertices; i ++)
	{
		m_xVertices[i].m_xNormal.Normalise();
	}

	ComputeBoundingSphere();
}

//------------------------------------------------------------------
// test collsion with a body
//------------------------------------------------------------------
bool CMesh::Collide(CBody& xBody) const
{
	//------------------------------------------------------------------
	// bounding not intersecting
	//------------------------------------------------------------------
	if (!BoundingVolumesIntersect(xBody))
		return false;

	bool bCollided = false;

	//------------------------------------------------------------------
	// Collide with each particles
	//------------------------------------------------------------------
	for(int i = 0; i < xBody.GetNumParticles(); i ++)
	{
		bCollided |= Collide(xBody.GetParticle(i));
	}
	return bCollided;
}

//------------------------------------------------------------------
// Collide with particle in a body
//------------------------------------------------------------------
bool CMesh::Collide(CParticle& xParticle) const
{
	bool bCollided = false;

	int j = m_iNumVertices-1;
	for(int i = 0; i < m_iNumVertices; j = i, i ++)
	{
		const CVertex& V0 = m_xVertices[j];
		const CVertex& V1 = m_xVertices[i];

		bCollided |= Collide(xParticle, V0, V1);
	}

	return bCollided;
}

//------------------------------------------------------------------
// Collide a particle with an edge
//------------------------------------------------------------------
bool CMesh::Collide(CParticle& xParticle, const CVertex& V0, const CVertex& V1) const
{
	//------------------------------------------------------------------
	// edge direction
	//------------------------------------------------------------------
	Vector E = V1.m_xPosition - V0.m_xPosition;

	//------------------------------------------------------------------
	// relative position of particle to edge
	//------------------------------------------------------------------
	Vector D = xParticle.m_xP1 - V0.m_xPosition;

	//------------------------------------------------------------------
	// See where particle's prjection on edge is (before, after, or on edge)
	//------------------------------------------------------------------
	float de = D * E;

	//------------------------------------------------------------------
	// Before : test with the start vertex
	//------------------------------------------------------------------
	if (de < 0.0f)
	{
		return V0.Collide(xParticle);
	}

	//------------------------------------------------------------------
	// After : test with the end vertex
	//------------------------------------------------------------------
	float e2 = E * E;

	if (de > e2)
	{
		return V1.Collide(xParticle);
	}


	//-------------------------------------------------
	// On edge : Interpolate to find point closest to 
	// particle on the segment. Interpolate normal as well
	//-------------------------------------------------
	float t = de / e2;		// interpolant
	float u = (1.0f - t);

	Vector Pseg = V0.m_xPosition + E * t;				// point on segment

	Vector Nseg = V0.m_xNormal * u + V1.m_xNormal * t;	// normal at point on segment

	Nseg.Normalise();									// normalise

	CVertex Vseg(Pseg, Nseg);							// setup vertex
	
	return Vseg.Collide(xParticle);						// collide
}

void CMesh::Render() const
{
	//-----------------------------------------------------------
	// Draw edges
	//-----------------------------------------------------------
	glLineWidth(2.0f);
	glColor4f(0.7f, 0.7f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	
	int j = m_iNumVertices-1;
	for(int i = 0; i < m_iNumVertices; j = i, i ++)
	{
		Vector P = m_xVertices[i].m_xPosition;
		Vector Q = m_xVertices[j].m_xPosition;
		
		glVertex2f(P.x, P.y);
		glVertex2f(Q.x, Q.y);
	}
	glEnd();
	
/*	//-----------------------------------------------------------
	// Draw vertices
	//-----------------------------------------------------------
	glPointSize(3.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	
	for(int i = 0; i < m_iNumVertices; i ++)
		glVertex2f(m_xVertices[i].m_xPosition.x, m_xVertices[i].m_xPosition.y);

	glEnd();

	//-----------------------------------------------------------
	// Draw vertices normals
	//-----------------------------------------------------------
	glLineWidth(1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glBegin(GL_LINES);
	
	for(int i = 0; i < m_iNumVertices; i ++)
	{
		Vector P = m_xVertices[i].m_xPosition;
		Vector N = m_xVertices[i].m_xNormal;
		Vector Q = P + N * 15.0f;

		glVertex2f(P.x, P.y);
		glVertex2f(Q.x, Q.y);
	}
	glEnd();


	glLineWidth(0.75f);
	glColor4f(0.7f, 0.7f, 1.0f, 0.3f);
	glBegin(GL_LINES);
	for(int i = 0; i < m_iNumVertices; i ++)
	{
		int j = (i - 1);
		int k = (i + 1);
		if (j < 0) j += m_iNumVertices;
		if (k >= m_iNumVertices) k -= m_iNumVertices;

		Vector P = m_xVertices[j].m_xPosition;
		Vector Q = m_xVertices[k].m_xPosition;
		
		glVertex2f(P.x, P.y);
		glVertex2f(Q.x, Q.y);
	}
	glEnd();
/**/
//		RenderBoundingSphere();
}


//--------------------------------------------
// General bounding sphere around the body 
// (for culling)
//--------------------------------------------
void CMesh::ComputeBoundingSphere()
{
	if (m_iNumVertices == 0)
		return;

	Vector Min = m_xVertices[0].m_xPosition;
	Vector Max = Min;
	
	for(int i = 1; i < m_iNumVertices; i ++)
	{
		const Vector& P = m_xVertices[i].m_xPosition;

		if (P.x < Min.x) Min.x = P.x;
		if (P.y < Min.y) Min.y = P.y;
//			if (P.z < Min.z) Min.z = P.z;
		
		if (P.x > Max.x) Max.x = P.x;
		if (P.y > Max.y) Max.y = P.y;
//			if (P.z > Max.z) Max.z = P.z;
	}
	m_xBoundingPos = (Max + Min) * 0.5f;
	m_fBoundingRad = ((Max - Min) * 0.5f).Length();
}

//-----------------------------------------------------------
// Checks if two particle's bounding vilume intersect
// simple radius check test
//-----------------------------------------------------------
bool CMesh::BoundingVolumesIntersect(const CBody& xBody) const
{
	Vector D(m_xBoundingPos - xBody.GetBoundingPos());

	float r = m_fBoundingRad + xBody.GetBoundingRad();

	float d2 = D * D;
	float r2 = r*r;

	return (d2 < r2);
}

void CMesh::Free()
{
	if (m_xVertices)
		delete[] m_xVertices;

	m_iNumVertices = 0;
}

void CMesh::Allocate(int iNumVertices)
{
	Free();

	m_iNumVertices = iNumVertices;

	m_xVertices = new CVertex[iNumVertices];
}

void CMesh::RenderBoundingSphere() const
{
	glLineWidth(1.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
	RenderCircle(m_xBoundingPos, m_fBoundingRad);
}
