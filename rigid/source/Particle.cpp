//-----------------------------------------------------------
// Verlet rigid body dynamics
//-----------------------------------------------------------
//-----------------------------------------------------------
// standard includes
//-----------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "..\..\GLut\glut.h"

#include "Particle.h"


CParticle::CParticle(void)
: m_xAcceleration(0, 0)
, m_xP0(0.0f, 0.0f)
{
	m_xP1 = m_xP0;
	SetMass(frand(10.0f) + 5.0f);
}

//-----------------------------------------------------------
// Init the particle phsyics
//-----------------------------------------------------------
CParticle::CParticle(const Vector& xPos, float fRadius, float Mass)
: m_xAcceleration(0, 0)
, m_fRadius(fRadius)
, m_xP0(xPos)
, m_xP1(xPos)
{
	SetMass(Mass);
}

//-----------------------------------------------------------
// Particle has infinite mass, unmovable
//-----------------------------------------------------------
bool CParticle::IsUnmovable() const 
{ 
	return (m_fMass == 0.0f); 
}

//-----------------------------------------------------------
// Collide two particles together. 
// simple radius check. If they overlap, push them apart
//-----------------------------------------------------------
bool CParticle::Collide(CParticle& xParticle)
{
	//-----------------------------------------------------------
	// can't move them
	//-----------------------------------------------------------
	if (IsUnmovable() && xParticle.IsUnmovable())
		return false;

	//-----------------------------------------------------------
	// relative distance
	//-----------------------------------------------------------
	Vector D(m_xP1 - xParticle.m_xP1);

	float r = m_fRadius + xParticle.m_fRadius;

	float d2 = D * D;
	float r2 = r*r;

	//-----------------------------------------------------------
	// particles too far apart
	//-----------------------------------------------------------
	if (d2 > r2)
		return false;
	
	float d = (float) sqrt(d2);

	D /= d;

	float depth   = (r - d);
	
	//-----------------------------------------------------------
	// apply response (impact and friction).
	//-----------------------------------------------------------
	return CollisionResponse(xParticle, D, depth);

	return true;
}

//-----------------------------------------------------------
// Add force to the particle
//-----------------------------------------------------------
void CParticle::AddForce(const Vector& xForce) 
{ 
	if (IsUnmovable()) 
		return; 

	m_xAcceleration += xForce * m_fInvMass; 
}

//-----------------------------------------------------------
// Set mass. Check if it is infinite (well, = 0.0f)
//-----------------------------------------------------------
void CParticle::SetMass(float fMass)		
{ 
	if (fMass < 0.00001f)
	{
		m_fMass = 0.0f;
		m_fInvMass = 0.0f;
		return;
	}

	m_fMass  = fMass;
	m_fInvMass = 1.0f / m_fMass; 
}

//-----------------------------------------------------------
// Render particle, and bounding sphere
//-----------------------------------------------------------
void CParticle::Render(const float* color) const
{
	glColor4fv(color);
	RenderSolidCircle(m_xP0, m_fRadius);

	glPointSize(3.0f);
	glColor4f(color[0], color[1], color[2], 1.0f);
	glBegin(GL_POINTS);
	glVertex2f(m_xP0.x, m_xP0.y);
	glEnd();
}

void CParticle::Update(float dt)
{
	//----------------------------------------------------------
	// Stop particle if inmovable
	//----------------------------------------------------------
	if (IsUnmovable())
	{
		m_xAcceleration = Vector(0.0f, 0.0f);
		m_xP0			= m_xP1;
		return;
	}

	//----------------------------------------------------------
	// Verlet integration
	//----------------------------------------------------------
	Vector Temp		 = m_xP1;
	m_xP1			+= (m_xP1 - m_xP0) + m_xAcceleration * (dt * dt);
	m_xP0			 = Temp;
	m_xAcceleration  = Vector::Blank();
}

//-----------------------------------------------------------
// Calculate bounding box around particle (for rigid body bounding boxes)
//-----------------------------------------------------------
void CParticle::GetBoundingBox(Vector& Min, Vector& Max) const
{
	Min.x = m_xP1.x - m_fRadius;
	Min.y = m_xP1.y - m_fRadius;
//		Min.z = m_xP1.z - m_fRadius;

	Max.x = m_xP1.x + m_fRadius;
	Max.y = m_xP1.y + m_fRadius;
//		Max.z = m_xP1.z + m_fRadius;
}

//-----------------------------------------------------------
// collsion response of collision vs static geometry
//-----------------------------------------------------------
bool CParticle::CollisionResponse(const Vector& Ncoll, float dcoll)
{
	//-----------------------------------------------------------
	// the amount of friction
	//-----------------------------------------------------------
	const float friction = 0.5f;

	//-----------------------------------------------------------
	// Move particle away from plane
	//-----------------------------------------------------------
	m_xP1 += Ncoll * dcoll;

	Vector V = (m_xP1 - m_xP0);

	//-----------------------------------------------------------
	// calculate velcity along normal, and collision plane
	//-----------------------------------------------------------
	Vector Vn = (V * Ncoll) * Ncoll;
	Vector Vt = (V - Vn);

	//-----------------------------------------------------------
	// apply friction.
	//-----------------------------------------------------------
	m_xP1 -= Vt * friction;

	return true;
}

//-----------------------------------------------------------
// collsion response of collision vs particle
//-----------------------------------------------------------
bool CParticle::CollisionResponse(CParticle& xParticle, const Vector& Ncoll, float dcoll)
{
	//-----------------------------------------------------------
	// the amount of friction
	//-----------------------------------------------------------
	const float friction = 0.5f;

	//-----------------------------------------------------------
	// Move particle away from plane
	//-----------------------------------------------------------
	float invmass = (m_fInvMass + xParticle.m_fInvMass);
	
	xParticle.m_xP1 -= Ncoll * (dcoll * xParticle.m_fInvMass);
		      m_xP1 += Ncoll * (dcoll *           m_fInvMass);

	Vector V0 = (m_xP1 - m_xP0);
	Vector V1 = (xParticle.m_xP1 - xParticle.m_xP0);
	Vector V  = V0 - V1;

	//-----------------------------------------------------------
	// calculate velcity along normal, and collision plane
	//-----------------------------------------------------------
	Vector Vn = (V * Ncoll) * Ncoll;
	Vector Vt = (V - Vn);

	//-----------------------------------------------------------
	// apply friction.
	//-----------------------------------------------------------
	Vt /= invmass; 

			  m_xP1 -= Vt * friction *			 m_fInvMass;
	xParticle.m_xP1 += Vt * friction * xParticle.m_fInvMass;

	return true;
}
