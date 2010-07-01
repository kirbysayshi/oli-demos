#ifndef OLI_LINCONSTRAINT_H
#define OLI_LINCONSTRAINT_H


/*
#include "LinConstraint.h"
*/

/*
------------------------------------------------------------------
File: LinConstraint.h
Started: 19/01/2004 21:31:17
  
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

#include "Particle.h"

//-----------------------------------------------------------
// Constrain two particles together (forces a distance equality)
//-----------------------------------------------------------
class CLinConstraint
{
public:	
	CParticle* m_pxPA;			// the two particles
	CParticle* m_pxPB;			// attached
	float      m_fRestLength;	// distance the particles should be 

	//-------------------------------------------------------------
	// 
	//-------------------------------------------------------------
	CLinConstraint(void);

	CLinConstraint(CParticle* pA, CParticle* pB);

	//------------------------------------------------------------------
	// See if the link is duplicated
	//------------------------------------------------------------------
	bool operator == (const CLinConstraint& xConstraint) const;

	//-----------------------------------------------------------
	// Update the stiff constraint. enforce the distance between two
	// particles to be equal to rest distance
	//-----------------------------------------------------------
	void Update(void);

	void Render(void) const;
};




#endif OLI_LINCONSTRAINT_H