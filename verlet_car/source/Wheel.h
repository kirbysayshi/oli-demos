//
// Wheel.h
//
// Created by OR - 05/03/2004 18:17:34
//
// Copyright (C) 2004 Rebellion, All Rights Reserved.
//

#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __WHEEL_H__
#define __WHEEL_H__

#include "Body.h"


class CWheel: public CBody
{
public:
	// if the wheel is steered, the steering particle is free floating, and should be 
	// constrained by a steered wheel or another process
	CWheel(float fRadius, CParticle* pxBearing, CParticle* pxSteering, bool bCopySteeringParticle=false)
	{
		m_pxBearing  = pxBearing;
		m_pxSteering = pxSteering;
	
		float twopi = atan(1.0f) * 8.0f;

		for(int i = 0; i < GetNumWheelParticles(); i ++)
		{
			float a = twopi * i / (float) GetNumWheelParticles();

			Vector D(0.0f, cos(a), sin(a));
			D *= fRadius;
			D += m_pxBearing->GetCurrentPos();

			AddParticle(CParticle(D, 1.0f));
		}

		if (bCopySteeringParticle)
			m_pxSteering = AddParticle(*pxSteering);
	
		for(int i = 0; i < GetNumWheelParticles(); i++)
		{
			AddConstraint(CConstraint(GetWheelParticle(i), GetWheelParticle(i+1)));
			AddConstraint(CConstraint(GetWheelParticle(i), m_pxBearing));
			AddConstraint(CConstraint(GetWheelParticle(i), m_pxSteering));
		}
	}

	int GetNumWheelParticles() const { return 10; }

	CParticle* GetWheelParticle(int i)
	{
		i %= GetNumWheelParticles();
	
		return m_pxParticles[i];
	}

	const CParticle* GetWheelParticle(int i) const
	{
		i %= GetNumWheelParticles();

		return m_pxParticles[i];
	}

	
	void Rotate(float fAngle)
	{
		Vector P = m_pxBearing->GetCurrentPos();
		Vector D = m_pxSteering->GetCurrentPos() - m_pxBearing->GetCurrentPos();
		D.Normalise();

		for(int i = 0; i < GetNumWheelParticles(); i++)
		{
			GetWheelParticle(i)->GetTargetPos().RotateAroundAxis(P, D, fAngle);
		}
	}

	virtual void Render(void) const
	{
		glEnable(GL_LIGHTING);

		SetMaterial(6, 1.0f);
		
		Vector D = m_pxSteering->GetCurrentPos() - m_pxBearing->GetCurrentPos();
		D.Normalise();

		glNormal3f(D.x, D.y, D.z);

		glBegin(GL_TRIANGLES);

		for(int i = 0; i < GetNumWheelParticles(); i++)
		{
			GetWheelParticle(i)->Render();
			GetWheelParticle(i+1)->Render();
			m_pxBearing->Render();
		}
		glEnd();

		PARENT::Render();
	}
	

protected:
	typedef CBody PARENT;

	CParticle* m_pxSteering;
	CParticle* m_pxBearing;

};



class CSteeredWheel: public CWheel
{
public:
	// assuming car is directed towards Z axis
	CSteeredWheel(float fRadius, CParticle* pxBearing, CParticle* pxSupportA, CParticle* pxSupportB)
	: CWheel(fRadius, pxBearing, pxSupportA, true)
	, m_fSteerAngle(0.0f)
	{
		m_pxSupport[0] = pxBearing;
		m_pxSupport[1] = pxSupportA;
		m_pxSupport[2] = pxSupportB;
	}
	virtual void UpdateParticles(float dt)
	{
		PARENT::UpdateParticles(dt);

		CalculateChassisPlane();
	}

	virtual void SatisfyConstraints()
	{
		PARENT::SatisfyConstraints();

		ConstraintToChassisPlane();
	}

	virtual void Steer(float fAngle) { m_fSteerAngle = fAngle; }

private:
	typedef CWheel PARENT;

	virtual void CalculateChassisPlane()
	{
		m_xChassisNormal.ComputeNormal(	m_pxSupport[0]->GetCurrentPos(),
										m_pxSupport[1]->GetCurrentPos(),
										m_pxSupport[2]->GetCurrentPos());
		m_xChassisNormal.Normalise();
	}

	void ConstraintToChassisPlane()
	{
		// the position of the steering particle, if no steering angle
		m_pxSteering->GetTargetPos() = m_pxSupport[1]->GetTargetPos();

		// Steer the particle, by rotating around the chassis up vector and wheel bearing particle
		m_pxSteering->GetTargetPos().RotateAroundAxis(m_pxSupport[0]->GetCurrentPos(), m_xChassisNormal, m_fSteerAngle);
	}
private:
	// the particles that forms the chassis base
	CParticle* m_pxSupport[3];
	Vector m_xChassisNormal;

	// the steering angle
	float  m_fSteerAngle;
};

#endif//__WHEEL_H__
