

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "..\..\GLut\glut.h"

#include "RigidBody.h"


CRigidBody::CRigidBody()
{}

void CRigidBody::SetRigidBodyConstraints()
{
	//-----------------------------------------------------------
	// link particles to every others
	//-----------------------------------------------------------
	for(int i = 0; i < GetNumParticles(); i ++)
	{
		for(int j = i+1; j < GetNumParticles(); j ++)
		{
			AddConstraint(CLinConstraint(&GetParticle(i), &GetParticle(j)));
		}
	}

	//-----------------------------------------------------------
	// Calculate the bounding volume for the body
	//-----------------------------------------------------------
	ComputeBoundingSphere();
}

void CRigidBody::Allocate(int iNumParticles)
{
	int iNumConstraints = CalculateNumRigidBodyConstraints(iNumParticles);

	CBody::Allocate(iNumParticles, iNumConstraints);
}

//--------------------------------------------
// Number of constraints connections for a given 
// number of particles, so that every particle is linked 
// to every other particles.
//--------------------------------------------
int CRigidBody::CalculateNumRigidBodyConstraints(int iNumParticles)
{
	if (iNumParticles < 2)
		return 0;

	return (iNumParticles-1) + CalculateNumRigidBodyConstraints(iNumParticles-1);
}

