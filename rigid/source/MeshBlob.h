#ifndef OLI_MESHBLOB_H
#define OLI_MESHBLOB_H


/*
#include <MeshBlob.h>
*/

/*
------------------------------------------------------------------
File: MeshBlob.h
Started: 18/01/2004 16:50:29
  
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
#include "C:\dev\oli\GLut\glut.h"
#include "Vector.h"
#include "Particle.h"
#include "Body.h"
#include "Mesh.h"


class CMeshBlob: public CMesh
{
public:
	CMeshBlob(const Vector& xPos, float fMeshRadius, int iNumVertices, float fRandomness, bool bInvert=false)
	{
		SetBlob(xPos, fMeshRadius, iNumVertices, fRandomness, bInvert);
	}

	void SetBlob(const Vector& xPos, float fMeshRadius, int iNumVertices, float fRandomness, bool bInvert=false)
	{
		if (iNumVertices < 4)
			iNumVertices = 4;

		Vector* V = new Vector[iNumVertices];

		for(int i = 0; i < iNumVertices; i ++)
		{
			float angle = -Pi() * 2.0f * (i / (float) iNumVertices);

			float fRad = frand(fMeshRadius * fRandomness) + fMeshRadius * (1.0f - fRandomness);

			V[i] = xPos + Vector(cos(angle), sin(angle)) * fRad;
		}

		SetVertices(V, iNumVertices, bInvert);

		delete[] V;
	}

private:
};

#endif OLI_MESHBLOB_H