#ifndef OLI_POLYCLIP_H
#define OLI_POLYCLIP_H


/*
#include "PolyClip.h"
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

class PolyClip
{
public:
	static Vector*	BuildBox	(int& iNumVertices, float width, float height);
	static Vector*	BuildBlob	(int iNumVertices, float radiusx, float radiusy);

	static void		Render		(const Vector& xOffset, float angle, u_int ARGBfill, u_int ARGBline, const Vector* axVertices, int iNumVertices);
	static void		Transform	(const Vector* axVertices, int iNumVertices, Vector* axTargetVertices, const Vector& xOffset, float angle);

	static bool		ClipSegment	(const Vector* A, int Anum, const Vector& PA, 
								 const Vector& xStart, const Vector& xEnd,
								 float& tnear, float& tfar, Vector& Nnear, Vector& Nfar);
								 
	static bool		PlaneClip	(const Vector* A, int Anum,
	 						     Vector* B, int& Bnum,
								 const Vector& xPlaneNormal, const Vector& xPlaneOrigin);

		static bool		PolygonClip	(const Vector* axClipperVertices, int iClipperNumVertices,
		   						 const Vector* axPolygonVertices, int iPolygonNumVertices,
								 Vector* axClippedPolygon, int& iClippedPolygonNumVertices);
};


#endif //OLI_POLYCLIP_H