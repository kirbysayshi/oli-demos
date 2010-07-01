//
// constraint.h
//
// Created by OR - 05/03/2004 16:31:48
//
// Copyright (C) 2004 Rebellion, All Rights Reserved.
//

#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __CONSTRAINT_H__
#define __CONSTRAINT_H__

class CConstraint
{
public:
	CConstraint(CParticle* pxParticle0=NULL, CParticle* pxParticle1=NULL)
	{
		m_pxParticle[0] = pxParticle0;
		m_pxParticle[1] = pxParticle1;

		if (!pxParticle0 || !pxParticle1)
			return;

		m_fRestLength = (m_pxParticle[0]->GetCurrentPos() - m_pxParticle[1]->GetCurrentPos()).GetLength();
	}

	void SatisfyConstraint()
	{
		if (!m_pxParticle[0] || !m_pxParticle[1])
			return;

		//-----------------------------------------------------------
		// cache stuff
		//-----------------------------------------------------------
		float restlength  = m_fRestLength;
		float restlength2 = restlength*restlength;
		float invmass1    = m_pxParticle[0]->GetInvMass();
		float invmass2    = m_pxParticle[1]->GetInvMass();
		float invmass     = invmass1+invmass2;
	
		if (invmass < 0.00001f)
			return;

		//-----------------------------------------------------------
		// relative position of particles
		//-----------------------------------------------------------
		Vector& x1        = m_pxParticle[0]->GetTargetPos();
		Vector& x2        = m_pxParticle[1]->GetTargetPos();
		Vector delta      = x2 - x1;

		float diff;
		float delta2 = delta*delta;
		
		//-----------------------------------------------------------
		// Using Square root approximation
		// calcualte the difference from the current distance to the ideal distance
		//-----------------------------------------------------------
		diff  = restlength2/(delta2 + restlength2)-0.5f;
		diff *= -2.0f;

		//-----------------------------------------------------------
		// move particles so their distance = ideal distance
		//-----------------------------------------------------------
		delta *= diff / invmass;
		x1    += delta*(invmass1);
		x2    -= delta*(invmass2);
  }

	void Render(void) const
	{
		if (!m_pxParticle[0] || !m_pxParticle[1])
			return;

		m_pxParticle[0]->Render();
		m_pxParticle[1]->Render();
	}


private:
	CParticle* m_pxParticle[2];
	float m_fRestLength;
};

#endif//__CONSTRAINT_H__