#ifndef OLI_SOFTBODY_H
#define OLI_SOFTBODY_H


/*
#include "SoftBody.h"
*/

/*
------------------------------------------------------------------
File: SoftBody.h
Started: 19/01/2004 22:24:56
  
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

class CSoftBody: public CBody
{
public:
	CSoftBody(const Vector& xPos, float fBodyRadius, int iNumParticles, float fParticleMass, int iRigidity);

	void SetSoftBody(const Vector& xPos, float fBodyRadius, int iNumParticles, float fParticleMass, int iRigidity);
};




#endif OLI_SOFTBODY_H