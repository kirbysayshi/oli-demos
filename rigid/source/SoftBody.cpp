
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "C:\dev\oli\GLut\glut.h"

#include "SoftBody.h"

CSoftBody::CSoftBody(const Vector& xPos, float fBodyRadius, int iNumParticles, float fParticleMass, int iRigidity)
{
	SetColor(0.6f, 0.6f, 1.0f, 0.5f);

	SetSoftBody(xPos, fBodyRadius, iNumParticles, fParticleMass, iRigidity);
}

void CSoftBody::SetSoftBody(const Vector& xPos, float fBodyRadius, int iNumParticles, float fParticleMass, int iRigidity)
{
	//-----------------------------------------------------
	// make sure we have sufficient number of particles
	//-----------------------------------------------------
	if (iNumParticles < 4)
		iNumParticles = 4;

	//-----------------------------------------------------
	// make sure we have an even number of particles
	//-----------------------------------------------------
	iNumParticles += (iNumParticles & 1);

	//-----------------------------------------------------
	// example : 8 particles at surface, plus one particle at
	// centre, all particles at surface linked to particles at centre
	// num particles : 8 + 1
	// num constraints : 8 + 8 (8 surface cosntraints + 8 to the centre)
	//-----------------------------------------------------
	Allocate(iNumParticles, iNumParticles * 3);

	SetRigidity(iRigidity);

	//-----------------------------------------------------
	// example : 4 particles => r = R * sqrt(2) / 2
	// a bit dodgy. works at the moment
	//-----------------------------------------------------
	float fParticleRadius = (sqrt(2.0f) * 0.75f) * fBodyRadius * (4 / (float) iNumParticles);

	//-----------------------------------------------------
	// set particles in a circular fashion
	//-----------------------------------------------------
	int i = 0;
	for(i = 0; i < iNumParticles; i ++)
	{
		float angle = (Pi() * 2.0f) * (i / (float) iNumParticles);

		float fDist = frand(fBodyRadius * 0.2f) + fBodyRadius * 0.8f;

		Vector Pos = xPos + Vector(cos(angle), sin(angle)) * fDist;

		AddParticle(CParticle(Pos, fParticleRadius, fParticleMass));
	}

	int iNumConstraints = 0;

	//-----------------------------------------------------
	// set surface links
	//-----------------------------------------------------
	for(int i = 0; i < iNumParticles; i ++)
	{
		int j = (i + 1) % iNumParticles;

		AddConstraint(CLinConstraint(&GetParticle(i), &GetParticle(j)));
	}

	//-----------------------------------------------------
	// link particles with opposite particle on the body
	//-----------------------------------------------------
	for(int i = 0; i < iNumParticles / 2; i ++)
	{
		int j = (i + iNumParticles / 2) % iNumParticles;

		AddConstraint(CLinConstraint(&GetParticle(i), &GetParticle(j)));
	}
}
