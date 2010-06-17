/*
------------------------------------------------------------------
File: dynamics.cpp
Started: 18/01/2004 14:15:44
  
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
// Verlet rigid body dynamics
//-----------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "C:\dev\oli\GLut\glut.h"
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

bool   dbg_UseAttractor = false;
float  dbg_world_size = 600;
Vector dbg_Attractor(0, 0);

//------------------------------------------------------------------
// the list of bodies
//------------------------------------------------------------------
CBodyList gBodies;


void GameInit(void)
{
	//------------------------------------------------------------------
	// Clean up
	//------------------------------------------------------------------
	gBodies.DeleteAll();

	enum { eNumBoxes = 4, eNumChains = 3, eNumSoftBodies = 5, eNumMeshBlobs = 6 };

	//------------------------------------------------------------------
	// Set the world outer limit
	//------------------------------------------------------------------
	gBodies.NewMesh(new CMeshBlob(Vector(dbg_world_size / 2, dbg_world_size/2), dbg_world_size * 0.8f, 24, 0.2f, true));

	//------------------------------------------------------------------
	// setup a bunch of soft bodies
	//------------------------------------------------------------------
	for(int i = 0; i < eNumSoftBodies; i ++)
	{
		Vector P0 = Vector(frand(dbg_world_size), frand(dbg_world_size));
		
		float fBodyRadius = frand(dbg_world_size / 30.0f) + dbg_world_size / 20.0f;
		
		int iNumParticles = 10;

		int iRigidity = rand() & 1 + 2;
		
		float fMass = frand(3.0f) + 1.0f;

		gBodies.NewBody(new CSoftBody(P0, fBodyRadius, iNumParticles, fMass, iRigidity));
	}
	
	//------------------------------------------------------------------
	// setup a bunch of boxes
	//------------------------------------------------------------------
	for(int i = 0; i < eNumBoxes; i ++)
	{
		Vector Pos  = Vector(frand(dbg_world_size), frand(dbg_world_size));
		
		float fSize = frand(dbg_world_size / 30.0f) + dbg_world_size / 40.0f;
		
		float fMass = frand(3.0f)  + 1.5f;

		int iRigidity = rand() & 1 + 1;
		
		gBodies.NewBody(new CBox(Pos, fSize, fMass, iRigidity));
	}

	//------------------------------------------------------------------
	// setup a bunch of chains
	//------------------------------------------------------------------
	for(int i = 0; i < eNumChains; i ++)
	{
		Vector P0 = Vector(frand(dbg_world_size), frand(dbg_world_size));
		
		float fLength = frand(dbg_world_size / 10.0f) + dbg_world_size / 6.0f;

		float angle = frand(Pi() * 2.0f);

		Vector P1 = P0 + Vector(cos(angle), sin(angle)) * fLength;
		
		int iNumParticles = 8;

		int iRigidity = rand() & 1 + 1;

		float fRadius = 0.75f * fLength / (iNumParticles);

		float fMass = frand(3.0f) + 1.0f;

		gBodies.NewBody(new CChain(P0, P1, iNumParticles, iRigidity, fRadius, fMass));
	}

	//------------------------------------------------------------------
	// setup a bunch of static meshes
	//------------------------------------------------------------------
	for(int i = 0; i < eNumMeshBlobs; i ++)
	{
		Vector P = Vector(frand(dbg_world_size * 1.2f), frand(dbg_world_size * 1.2f));

		float fRadius = frand(dbg_world_size * 0.1f) + dbg_world_size * 0.05f;

		gBodies.NewMesh(new CMeshBlob(P, fRadius, 8, 0.4f, false));
	}
}

void GameUpdate(float dt)
{
	if (dbg_UseAttractor)
		gBodies.Update(dt, &dbg_Attractor);
	else
		gBodies.Update(dt, NULL);

	dbg_UseAttractor = false;
}

void GameRender(void)
{
	gBodies.Render();
}


void GameSetAttractor(const Vector& xAttractor)
{
	dbg_Attractor		= xAttractor;
	dbg_UseAttractor	= true;
}


