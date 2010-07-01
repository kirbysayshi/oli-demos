
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "..\..\GLut\glut.h"

#include "Chain.h"


CChain::CChain(const Vector& P0, const Vector& P1, int iNumParticles, int iRigidity, float fParticleRadius, float fParticleMass)
{
	SetColor(0.0f, 1.0f, 0.3f, 0.5f);

	SetChain(P0, P1, iNumParticles, iRigidity, fParticleRadius, fParticleMass);
}

void CChain::SetChain(const Vector& P0, const Vector& P1, int iNumParticles, int iRigidity, float fParticleRadius, float fParticleMass)
{
	Free();

	Allocate(iNumParticles, iNumParticles-1);

	if (iNumParticles < 2)
		return;

	if (iRigidity <= 0)
		iRigidity = 1;

	SetRigidity(iRigidity);

	//-----------------------------------------------------------
	// copy particles
	//-----------------------------------------------------------
	Vector P = P0;
	Vector D = (P0 - P1) / ((float) iNumParticles);

	for(int i = 0; i < iNumParticles; i ++)
	{
		AddParticle(CParticle(P, fParticleRadius, (i != 0)? fParticleMass : 0.0f));

		P += D;
	}

	//------------------------------------------------------------------
	// link particles together
	//------------------------------------------------------------------
	for(int i = 0; i < iNumParticles-1; i ++)
	{
		AddConstraint(CLinConstraint(&GetParticle(i), &GetParticle(i+1)));

		P += D;
	}
	
	ComputeBoundingSphere();

	m_iSelfCollisionStep = 2;
}


//------------------------------------------------------------------
// collide joints with each other. Skip particles too close
//------------------------------------------------------------------
bool CChain::SelfCollide()
{
	bool bCollided = false;

	for(int i = 0; i < GetNumParticles(); i ++)
	{
		for (int j = i + m_iSelfCollisionStep; j < GetNumParticles(); j ++)
		{
			bCollided |= GetParticle(i).Collide(GetParticle(j));
		}
	}
	return bCollided;
}
