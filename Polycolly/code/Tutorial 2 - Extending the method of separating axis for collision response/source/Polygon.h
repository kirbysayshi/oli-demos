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
	static Vector*	BuildBox	(int& iNumVertices, float width, float height);
	static Vector*	BuildBlob	(int iNumVertices, float radius);
	static void		Render		(const Vector& xOffset, u_int ARGBfill, u_int ARGBline, const Vector* axVertices, int iNumVertices);
	static bool		Collide		(const Vector* A, int Anum, const Vector* B, int Bnum, const Vector& xOffset, Vector& N, float& t);
};



#endif OLI_POLYGON_H