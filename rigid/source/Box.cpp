/*
------------------------------------------------------------------
File: Box.cpp
Started: 19/01/2004 21:40:17
  
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

#include "Box.h"

//-----------------------------------------------------------
// simple square box body	
//-----------------------------------------------------------
CBox::CBox(const Vector& xPos, float fSize, float fParticleMass, int iRigidity)
{
	SetColor(1.0f, 0.3f, 0.3f, 0.5f);

	SetBox(xPos, fSize, fParticleMass, iRigidity);
}

void CBox::SetBox(const Vector& xPos, float fSize, float fParticleMass, int iRigidity)
{
	Allocate(4);

	SetRigidity(iRigidity);

	
	float fRad = fSize * (float) sqrt(2.0f) / 2.0f;
	
	float s = fSize / 2.0f;
	
	Vector Corners[4] = { Vector(-s, -s), Vector(s, -s), Vector(s, s), Vector(-s, s) };
	Corners[0] += xPos;
	Corners[1] += xPos;
	Corners[2] += xPos;
	Corners[3] += xPos;

	//-----------------------------------------------------------
	// simple square box body Some masses will be set to < 0.0f,
	// marking the particle as unmovable.
	//-----------------------------------------------------------
	AddParticle(CParticle(Corners[0], fRad, fParticleMass));
	AddParticle(CParticle(Corners[1], fRad, fParticleMass));
	AddParticle(CParticle(Corners[2], fRad, fParticleMass));
	AddParticle(CParticle(Corners[3], fRad, fParticleMass));

	SetRigidBodyConstraints();
}

