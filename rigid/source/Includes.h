#ifndef OLI_INCLUDES_H
#define OLI_INCLUDES_H


/*
#include <Includes.h>
*/

/*
------------------------------------------------------------------
File: Includes.h
Started: 19/01/2004 21:08:53
  
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

//-----------------------------------------------------------
// standard includes
//-----------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

#include "Vector.h"
#include "Particle.h"
#include "LinConstraint.h"
#include "Body.h"
#include "BodyList.h"
#include "Chain.h"
#include "SoftBody.h"
#include "RigidBody.h"
#include "Box.h"
#include "Mesh.h"
#include "MeshBlob.h"

extern void	RenderSolidCircle(const Vector& xPos, float fRad);
extern void	RenderCircle	 (const Vector& xPos, float fRad);


#endif OLI_INCLUDES_H