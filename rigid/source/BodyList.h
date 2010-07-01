#ifndef OLI_BODYLIST_H
#define OLI_BODYLIST_H


/*
#include "BodyList.h"
*/

/*
------------------------------------------------------------------
File: BodyList.h
Started: 19/01/2004 21:38:13
  
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
#include "Mesh.h"


//-----------------------------------------------------------
// Holds all the bodies in the game	
// also holds the static meshes
//-----------------------------------------------------------
class CBodyList
{
public:
	CBodyList();

	~CBodyList();

	void DeleteAll();

	void DeleteBody(CBody* pxBody);

	void NewBody(CBody* pxBody);

	void DeleteMesh(CMesh* pxMesh);

	void NewMesh(CMesh* pxMesh);

	//-----------------------------------------------------------
	// update all bodies. Move their particles, apply 
	// collision constraints, then stiff cosntraints
	// Supply an attractor point. for fun
	//-----------------------------------------------------------
	void Update(float dt, const Vector* pxAttractor=NULL);

	void Render();

private:
	enum { eMaxBodies = 256, eMaxMeshes = 256 };

	CBody* m_pxBodies[eMaxBodies];
	int m_iNumBodies;

	CMesh* m_pxMeshes[eMaxMeshes];
	int m_iNumMeshes;

};



#endif OLI_BODYLIST_H