#ifndef OLI_POLYGON_H
#define OLI_POLYGON_H


/*
#include "Polygon.h"
*/

/*
------------------------------------------------------------------
File: Polygon.h
Started: 12/06/2004 14:37:12
  
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


class Vector;

class PolyColl
{
public:
	static float    CalculateMass   (const Vector* A, int Anum, float density);
	static float    CalculateInertia(const Vector* A, int Anum, float mass);

	static Vector*	BuildBox	(int& iNumVertices, float width, float height);
	static Vector*	BuildBlob	(int iNumVertices, float radiusx, float radiusy);

	static void		Render		(const Vector& xOffset, float angle, u_int ARGBfill, u_int ARGBline, const Vector* axVertices, int iNumVertices);
	static bool		Collide		(const Vector* A, int Anum, const Vector& PA, const Vector& VA, const Matrix& OA,
								 const Vector* B, int Bnum, const Vector& PB, const Vector& VB, const Matrix& OB,
								 Vector& N, float& t);

	// calculate contact points of two colliding polygons
	static bool FindContacts(const Vector* A, int Anum, const Vector& PA, const Vector& VA, const Matrix& OA,
							 const Vector* B, int Bnum, const Vector& PB, const Vector& VB, const Matrix& OB,
							 const Vector& N, float t,
							 Vector* CA, 
							 Vector* CB, 
							 int& Cnum);

	bool ClipSegment(const Vector* A, int Anum, const Vector& xOffset, const Vector& xStart, const Vector& xEnd, float& tnear, float& tfar, Vector& Nnear, Vector& Nfar);
};



#endif OLI_POLYGON_H