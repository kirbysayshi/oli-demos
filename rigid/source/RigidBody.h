#ifndef OLI_RIGIDBODY_H
#define OLI_RIGIDBODY_H


/*
#include "RigidBody.h"
*/

/*
------------------------------------------------------------------
File: RigidBody.h
Started: 19/01/2004 22:23:51
  
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



#include "Body.h"


class CRigidBody: public CBody
{
public:
	CRigidBody();

	void SetRigidBodyConstraints();

	void Allocate(int iNumParticles);
	
	//--------------------------------------------
	// Number of constraints connections for a given 
	// number of particles, so that every particle is linked 
	// to every other particles.
	//--------------------------------------------
	int CalculateNumRigidBodyConstraints(int iNumParticles);
};



#endif OLI_RIGIDBODY_H