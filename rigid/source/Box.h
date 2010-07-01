#ifndef OLI_BOX_H
#define OLI_BOX_H


/*
#include "Box.h"
*/

/*
------------------------------------------------------------------
File: Box.h
Started: 19/01/2004 21:40:51
  
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


#include "RigidBody.h"

//-----------------------------------------------------------
// simple square box body	
//-----------------------------------------------------------
class CBox: public CRigidBody
{
public:
	CBox(const Vector& xPos, float fSize, float fParticleMass, int iRigidity);

	void SetBox(const Vector& xPos, float fSize, float fParticleMass, int iRigidity);
};



#endif OLI_BOX_H