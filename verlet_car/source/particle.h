//
// particle.h
//
// Created by OR - 05/03/2004 13:22:29
//
// Copyright (C) 2004 Rebellion, All Rights Reserved.
//

#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Vector.h"

class CParticle
{
public:
	CParticle(const Vector& xPos = Vector(frand(60) + 40, 2, frand(60) + 40), float fMass=1.0f)
	{
		m_xCurrentPos = xPos;
		m_xTargetPos  = xPos;
		m_fMass       = (fMass < 0.0001f)? 0.0f : fMass;
		m_fInvMass	  = (fMass < 0.0001f)? 0.0f : 1.0f / fMass; 
		m_xForces	  = Vector(0.0f, 0.0f, 0.0f);
		m_fCoF		  = 0.2f;
		m_bCollided	  = false;
	}

	void Update(float dt)
	{
		Vector Temp		 = m_xTargetPos;
		Vector Vel       = (m_xTargetPos - m_xCurrentPos);
		
		if (m_bCollided)
			Vel *= m_fCoF;

		m_xTargetPos	+= Vel + m_xForces * (dt*dt * m_fInvMass);
		m_xCurrentPos	 = Temp;
		m_xForces		 = Vector(0.0f, -0.5f, 0.0f);
		m_bCollided		 = false;
	}

	void SetCollided() { m_bCollided = true; }

	Vector& GetTargetPos() { return m_xTargetPos; }
	const Vector& GetTargetPos() const { return m_xTargetPos; }

	const Vector& GetCurrentPos() const { return m_xCurrentPos; }

	float GetMass() const { return m_fMass; }
	float GetInvMass() const { return m_fInvMass; }

	void Render() const
	{
		glVertex3f(m_xCurrentPos.x, m_xCurrentPos.y, m_xCurrentPos.z);
	}

private:
	Vector m_xCurrentPos;
	Vector m_xTargetPos;
	Vector m_xForces;
	float  m_fMass;
	float  m_fInvMass;

	bool   m_bCollided;
	float  m_fCoF;
};

#endif//__PARTICLE_H__