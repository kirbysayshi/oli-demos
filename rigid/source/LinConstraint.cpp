/*
------------------------------------------------------------------
File: LinConstraint.cpp
Started: 19/01/2004 21:27:26
  
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
#include "LinConstraint.h"


//-----------------------------------------------------------
// Constrain two particles together (forces a distance equality)
//-----------------------------------------------------------
CLinConstraint::CLinConstraint(void)
: m_pxPA(0)
, m_pxPB(0)
, m_fRestLength(0.0f)
{}

CLinConstraint::CLinConstraint(CParticle* pA, CParticle* pB)
: m_pxPA(pA)
, m_pxPB(pB)
, m_fRestLength(0.0f)
{
	if (!m_pxPA || !m_pxPB)
		return;
	
	//------------------------------------------------------------------
	// Setup the rest length
	//------------------------------------------------------------------
	const Vector& x1 = m_pxPA->m_xP1;
	const Vector& x2 = m_pxPB->m_xP1;
	
	m_fRestLength = (x1 - x2).Length();
}

//-----------------------------------------------------------
// Update the stiff constraint. enforce the distance between two
// particles to be equal to rest distance
//-----------------------------------------------------------
void CLinConstraint::Update(void)
{
	if (!m_pxPA || !m_pxPB)
		return;
	
	//-----------------------------------------------------------
	// cache stuff
	//-----------------------------------------------------------
	float restlength  = m_fRestLength;
	float restlength2 = restlength*restlength;
	float invmass1    = m_pxPA->m_fInvMass;
	float invmass2    = m_pxPB->m_fInvMass;
	float invmass     = invmass1+invmass2;
	
	if (invmass < 0.00001f)
		return;

	//-----------------------------------------------------------
	// relative position of particles
	//-----------------------------------------------------------
	Vector& x1        = m_pxPA->m_xP1;
	Vector& x2        = m_pxPB->m_xP1;
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

void CLinConstraint::Render(void) const
{
	if (!m_pxPA || !m_pxPB)
		return;
	
	Vector& x1 = m_pxPA->m_xP1;
	Vector& x2 = m_pxPB->m_xP1;
	
	glVertex2f(x1.x, x1.y);
	glVertex2f(x2.x, x2.y);
}

//------------------------------------------------------------------
// See if the link is duplicated
//------------------------------------------------------------------
bool CLinConstraint::operator == (const CLinConstraint& xConstraint) const
{
	if (m_pxPA == xConstraint.m_pxPA && m_pxPB == xConstraint.m_pxPB)
		return true;

	if (m_pxPB == xConstraint.m_pxPA && m_pxPA == xConstraint.m_pxPB)
		return true;

	return false;
}



