#ifndef OLI_BODY_H
#define OLI_BODY_H


/*
#include <Body.h>
*/

/*
------------------------------------------------------------------
File: Body.h
Started: 18/01/2004 14:10:32
  
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
#include "..\..\GLut\glut.h"

#include "Particle.h"
#include "LinConstraint.h"

//-----------------------------------------------------------
// a group of constrain particles, stored in one body
//-----------------------------------------------------------
class CBody
{
public:
	CBody(int iRigidity=1);

	virtual ~CBody(void);

	//-----------------------------------------------------------
	// Copy a rigid body
	//-----------------------------------------------------------
	CBody& operator = (const CBody& xBody);

	void Render(void) const;

	void AttractParticles(const Vector* pxAttractor);

	//-----------------------------------------------------------
	// Update particle's estimated position		
	//-----------------------------------------------------------
	void UpdateParticles(float dt);

	//-----------------------------------------------------------
	// collide with self
	//-----------------------------------------------------------
	virtual bool SelfCollide();
	
	//-----------------------------------------------------------
	// collide two bodies. Shift particles around so they stop intersecting
	//-----------------------------------------------------------
	bool Collide(CBody& xBody);

	//-----------------------------------------------------------
	// update the constraints. run the iterations several times
	//-----------------------------------------------------------
	void UpdateConstraints();
	
	//--------------------------------------------
	// update the bounding volume for culling
	//--------------------------------------------
	void UpdateBoundingVolume();

	void AddParticle(const CParticle& xParticle);

	void AddConstraint(const CLinConstraint& xConstraint);

	      CParticle& GetParticle(int i)       { return m_xParticles[i]; }
	const CParticle& GetParticle(int i) const { return m_xParticles[i]; }

	      CLinConstraint& GetConstraint(int i)       { return m_xConstraints[i]; }
	const CLinConstraint& GetConstraint(int i) const { return m_xConstraints[i]; }

	int GetNumParticles() const { return m_iNumParticles; }

	int GetNumConstraints() const { return m_iNumConstraints; }

	int GetRigidity() const { return m_iRigidity; }

	void SetRigidity(int iRigidity) { m_iRigidity = (iRigidity < 1)? 1 : iRigidity; }

	const Vector& GetBoundingPos() const { return m_xBoundingPos; }
	float         GetBoundingRad() const { return m_fBoundingRad; }

	void SetColor(float r, float g, float b, float a);

protected:
	//-----------------------------------------------------------
	// Free allocs
	//-----------------------------------------------------------
	void Free();

	//-----------------------------------------------------------
	// alloc the arays
	//-----------------------------------------------------------
	void Allocate(int iNumParticles, int iNumConstraints);

	//-----------------------------------------------------------
	// Compute the bounding box around the particles of the body
	// (very simple)
	//-----------------------------------------------------------
	void ComputeBoundingSphere();

	void RenderBoundingSphere() const;
	
	//-----------------------------------------------------------
	// Checks if two particle's bounding vilume intersect
	// simple radius check test
	//-----------------------------------------------------------
	bool BoundingVolumesIntersect(const CBody& xBody) const;

	bool FindConstraint(const CLinConstraint& xConstraint) const;


private:
	//--------------------------------------------
	// particles and constraints on a body
	//--------------------------------------------
	CParticle*		m_xParticles;
	CLinConstraint*	m_xConstraints;
	int				m_iMaxParticles;
	int				m_iMaxConstraints;

	int				m_iNumParticles;
	int				m_iNumConstraints;
	
	int				m_iRigidity;

	//--------------------------------------------
	// General bounding sphere around the body 
	// (for culling)
	//--------------------------------------------
	float			m_fBoundingRad;
	Vector			m_xBoundingPos;
	float			m_afColor[4];
};



#endif OLI_BODY_H