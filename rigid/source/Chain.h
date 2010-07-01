#ifndef OLI_CHAIN_H
#define OLI_CHAIN_H


/*
#include <Chain.h>
*/

/*
------------------------------------------------------------------
File: Chain.h
Started: 18/01/2004 14:11:52
  
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
#include "Vector.h"
#include "Particle.h"
#include "Body.h"


class CChain: public CBody
{
public:
	CChain(const Vector& P0, const Vector& P1, int iNumParticles, int iRigidity, float fParticleRadius, float fParticleMass);

	void SetChain(const Vector& P0, const Vector& P1, int iNumParticles, int iRigidity, float fParticleRadius, float fParticleMass);

	//------------------------------------------------------------------
	// collide joints with each other. Skip particles too close
	//------------------------------------------------------------------
	virtual bool SelfCollide();

private:
	int m_iSelfCollisionStep;
};



#endif OLI_CHAIN_H