
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "..\..\GLut\glut.h"
#include "Body.h"

//-----------------------------------------------------------
// a group of constrain particles, stored in one body
//-----------------------------------------------------------
CBody::CBody(int iRigidity)
: m_xParticles(NULL)
, m_xConstraints(NULL)
, m_iNumParticles(0)
, m_iNumConstraints(0)
, m_iMaxParticles(0)
, m_iMaxConstraints(0)
, m_fBoundingRad(0.0f)
, m_xBoundingPos(Vector::Blank())
, m_iRigidity((iRigidity < 1)? 1 : iRigidity)
{
	m_afColor[0] = 0.0f;
	m_afColor[1] = 1.0f;
	m_afColor[2] = 0.0f;
	m_afColor[3] = 0.5f;
}

CBody::~CBody(void)
{
	Free();
}

//-----------------------------------------------------------
// Copy a rigid body
//-----------------------------------------------------------
CBody& CBody::operator = (const CBody& xBody)
{
	m_iRigidity = (xBody.m_iRigidity <= 0)? 1 : xBody.m_iRigidity;

	//-----------------------------------------------------------
	// Allocate memory space
	//-----------------------------------------------------------
	Allocate(xBody.m_iMaxParticles, xBody.m_iMaxConstraints);

	//-----------------------------------------------------------
	// copy particles
	//-----------------------------------------------------------
	for(int i = 0; i < xBody.m_iNumParticles; i ++)
	{
		AddParticle(xBody.m_xParticles[i]);
	}

	//-----------------------------------------------------------
	// Copy constraints
	//-----------------------------------------------------------
	for(int i = 0; i < xBody.m_iNumConstraints; i ++)
	{
		CLinConstraint* pxConstraint = &xBody.m_xConstraints[i];

		//-----------------------------------------------------------
		// the indices of the particles in the constraint
		//-----------------------------------------------------------
		int i0 = (pxConstraint->m_pxPA - xBody.m_xParticles);
		int i1 = (pxConstraint->m_pxPB - xBody.m_xParticles);

		if (i0 > xBody.m_iNumParticles || i0 < 0)
			continue;

		if (i1 > xBody.m_iNumParticles || i1 < 0)
			continue;

		AddConstraint(CLinConstraint(&m_xParticles[i0], &m_xParticles[i1]));
	}

	return *this;
}

void CBody::RenderBoundingSphere() const
{
	glLineWidth(1.0f);
	glColor4f(0.8f, 0.8f, 0.8f, 0.3f);
	RenderCircle(m_xBoundingPos, m_fBoundingRad);
}

void CBody::Render(void) const
{
	RenderBoundingSphere();

	//---------------------------------------------------------------
	// Render particles
	//---------------------------------------------------------------
	for(int i=0; i< m_iNumParticles; i++) 
		m_xParticles[i].Render(m_afColor);
	
	//---------------------------------------------------------------
	// Render cosntraints
	//---------------------------------------------------------------
	glLineWidth(1.0f);
	glColor4f(m_afColor[0], m_afColor[1], m_afColor[2], 0.75f);

	glBegin(GL_LINES);
	for(int i=0; i<m_iNumConstraints; i++) 
		m_xConstraints[i].Render();
	glEnd();
}

void CBody::AttractParticles(const Vector* pxAttractor)
{
	//-----------------------------------------------------------
	// Add an attractor, for fun
	//-----------------------------------------------------------
	for(int i = 0; i < m_iNumParticles; i ++)
	{
		if (pxAttractor)
		{
			m_xParticles[i].AddForce((*pxAttractor - m_xParticles[i].m_xP0) / 4.0f);
		}
		else
		{
			m_xParticles[i].AddForce(Vector(0.0f, -10.0f) * m_xParticles[i].m_fMass);
		}
	}
}

//-----------------------------------------------------------
// Update particle's estimated position		
//-----------------------------------------------------------
void CBody::UpdateParticles(float dt)
{
	for(int i=0; i< m_iNumParticles; i++) 
	{
		m_xParticles[i].Update(dt);
	}
}

//-----------------------------------------------------------
// Checks if two particle's bounding vilume intersect
// simple radius check test
//-----------------------------------------------------------
bool CBody::BoundingVolumesIntersect(const CBody& xBody) const
{
	Vector D(m_xBoundingPos - xBody.m_xBoundingPos);

	float r = m_fBoundingRad + xBody.m_fBoundingRad;

	float d2 = D * D;
	float r2 = r*r;

	return (d2 < r2);
}

bool CBody::SelfCollide()
{
	return false;
}

//-----------------------------------------------------------
// collide two bodies. Shift particles around so they stop intersecting
//-----------------------------------------------------------
bool CBody::Collide(CBody& xBody)
{
	if (!BoundingVolumesIntersect(xBody))
		return false;
	
	bool bCollided = false;

	for(int i=0; i< m_iNumParticles; i++) 
	{
		for(int j = 0; j < xBody.m_iNumParticles; j ++)
		{
			if (m_xParticles[i].Collide(xBody.m_xParticles[j]))
				bCollided = true;
		}
	}
	return bCollided;
}

//-----------------------------------------------------------
// update the constraints. run the iterations several times
//-----------------------------------------------------------
void CBody::UpdateConstraints()
{
	for(int iter = 0; iter < m_iRigidity; iter++)
	{
		//-----------------------------------------------------------
		// Apply stiff constraint
		//-----------------------------------------------------------
		for(int i = 0; i < m_iNumConstraints; i ++)
		{
			m_xConstraints[i].Update();
		}
	}
}

//--------------------------------------------
// update the bounding volume for culling
//--------------------------------------------
void CBody::UpdateBoundingVolume()
{
	ComputeBoundingSphere();
}

void CBody::AddParticle(const CParticle& xParticle)
{
	if (m_iNumParticles >= m_iMaxParticles)
		return;

	m_xParticles[m_iNumParticles++] = xParticle;
}

void CBody::AddConstraint(const CLinConstraint& xConstraint)
{
	if (m_iNumConstraints >= m_iMaxConstraints)
		return;

	if (FindConstraint(xConstraint))
		return;

	m_xConstraints[m_iNumConstraints++] = xConstraint;
}

bool CBody::FindConstraint(const CLinConstraint& xConstraint) const
{
	for(int i = 0; i < m_iNumConstraints; i ++)
	{
		if (m_xConstraints[i] == xConstraint)
			return true;
	}
	return false;
}

//-----------------------------------------------------------
// Free allocs
//-----------------------------------------------------------
void CBody::Free()
{
	if (m_xParticles)
		delete[] m_xParticles;

	if (m_xConstraints)
		delete[] m_xConstraints;

	m_xParticles		= NULL;
	m_xConstraints		= NULL;
	m_iMaxParticles		= 0;
	m_iMaxConstraints	= 0;
	m_iNumParticles		= 0;
	m_iNumConstraints	= 0;
}

//-----------------------------------------------------------
// alloc the arays
//-----------------------------------------------------------
void CBody::Allocate(int iNumParticles, int iNumConstraints)
{
	Free();

	if (iNumParticles <= 0)
		return;

	if (iNumConstraints <= 0)
		return;

	m_iNumParticles   = 0;
	m_iNumConstraints = 0;

	m_iMaxParticles   = iNumParticles;
	m_iMaxConstraints = iNumConstraints;

	m_xParticles	= new CParticle		[m_iMaxParticles];
	m_xConstraints	= new CLinConstraint[m_iMaxConstraints];
}

//-----------------------------------------------------------
// Compute the bounding box around the particles of the body
// (very simple)
//-----------------------------------------------------------
void CBody::ComputeBoundingSphere()
{
	if (m_iNumParticles <= 0)
		return;

	Vector Min, Max;

	m_xParticles[0].GetBoundingBox(Min, Max);

	for(int i = 1; i < m_iNumParticles; i ++)
	{
		Vector Pmin, Pmax;
		m_xParticles[i].GetBoundingBox(Pmin, Pmax);

		if (Pmin.x < Min.x) Min.x = Pmin.x;
		if (Pmin.y < Min.y) Min.y = Pmin.y;
//			if (Pmin.z < Min.z) Min.z = Pmin.z;
		
		if (Pmax.x > Max.x) Max.x = Pmax.x;
		if (Pmax.y > Max.y) Max.y = Pmax.y;
//			if (Pmax.z > Max.z) Max.z = Pmax.z;
	}

	m_xBoundingPos = (Max + Min) * 0.5f;
	m_fBoundingRad = ((Max - Min) * 0.5f).Length();
}


void CBody::SetColor(float r, float g, float b, float a) 
{ 
	m_afColor[0] = r; 
	m_afColor[1] = g; 
	m_afColor[2] = b; 
	m_afColor[3] = a; 
}
