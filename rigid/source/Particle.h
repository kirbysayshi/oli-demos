#ifndef OLI_PARTICLE_H
#define OLI_PARTICLE_H


/*
#include <Particle.h>
*/

/*
------------------------------------------------------------------
File: Particle.h
Started: 18/01/2004 14:08:22
  
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

//-----------------------------------------------------------
// Verlet rigid body dynamics
//-----------------------------------------------------------
#include "Vector.h"

//-----------------------------------------------------------
// Basic particle class
//-----------------------------------------------------------
class CParticle
{
public:
	Vector		m_xP0;					// Current positions
	Vector		m_xP1;					// Previous positions
	Vector		m_xAcceleration;		// Force accumulators
	float       m_fMass;				// mass
	float		m_fInvMass;				// iverse mass
	float		m_fRadius;				// radius of the particle

public:
	//-----------------------------------------------------------
	// Init the particle phsyics
	//-----------------------------------------------------------
	CParticle(void);

	CParticle(const Vector& xPos, float fRadius, float Mass=1.0f);

	//-----------------------------------------------------------
	// Particle has infinite mass, unmovable
	//-----------------------------------------------------------
	bool IsUnmovable() const;

	//-----------------------------------------------------------
	// Collide two particles together. 
	// simple radius check. If they overlap, push them apart
	//-----------------------------------------------------------
	bool Collide(CParticle& xParticle);
	
	//-----------------------------------------------------------
	// Add force to the particle
	//-----------------------------------------------------------
	void AddForce(const Vector& xForce);

	//-----------------------------------------------------------
	// Set mass. Check if it is infinite (well, = 0.0f)
	//-----------------------------------------------------------
	void SetMass(float fMass);

	//-----------------------------------------------------------
	// Render particle, and bounding sphere
	//-----------------------------------------------------------
	void Render(const float* color) const;
	
	//-----------------------------------------------------------
	// update particle using verlet integration
	//-----------------------------------------------------------
	void Update(float dt);

	//-----------------------------------------------------------
	// Calculate bounding box around particle (for rigid body bounding boxes)
	//-----------------------------------------------------------
	void GetBoundingBox(Vector& Min, Vector& Max) const;

	//-----------------------------------------------------------
	// collsion response of collision vs static geometry
	//-----------------------------------------------------------
	bool CollisionResponse(const Vector& Ncoll, float dcoll);
	
	//-----------------------------------------------------------
	// collsion response of collision vs particle
	//-----------------------------------------------------------
	bool CollisionResponse(CParticle& xParticle, const Vector& Ncoll, float dcoll);	
};



#endif OLI_PARTICLE_H