#ifndef OLI_BBOX_H
#define OLI_BBOX_H


/*
#include "BBox.h"
*/

/*
------------------------------------------------------------------
File: BBox.h
Started: 06/03/2004 01:42:29
  
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

#include "Vector.h"

class CBBox
{
public:

	CBBox()
	{}

	void Reset(const Vector& xPos)
	{
		m_xMin = m_xMax = xPos;
	}
	void Bound(const Vector& xPos)
	{
		if (xPos.x < m_xMin.x) m_xMin.x = xPos.x; else if (xPos.x > m_xMax.x) m_xMax.x = xPos.x;
		if (xPos.y < m_xMin.y) m_xMin.y = xPos.y; else if (xPos.y > m_xMax.y) m_xMax.y = xPos.y;
		if (xPos.z < m_xMin.z) m_xMin.z = xPos.z; else if (xPos.z > m_xMax.z) m_xMax.z = xPos.z;
	}

	void Expand(float threshold)
	{
		m_xMin -= Vector(threshold, threshold, threshold);
		m_xMax += Vector(threshold, threshold, threshold);
	}

	bool Intersect(const CBBox& xBox, float threshold)
	{
		if (m_xMin.x > xBox.m_xMax.x + threshold || xBox.m_xMin.x > m_xMax.x + threshold)
			return false;
		if (m_xMin.z > xBox.m_xMax.z + threshold || xBox.m_xMin.z > m_xMax.z + threshold)
			return false;
		if (m_xMin.y > xBox.m_xMax.y + threshold || xBox.m_xMin.y > m_xMax.y + threshold)
			return false;

		return true;
	}

	bool Constrain(Vector& P)
	{
		float dx0 = P.x - m_xMin.x;
		float dx1 = m_xMax.x - P.x;
		if (dx0 < 0.0f || dx1 < 0.0f)
			return false;
			
		float dz0 = P.z - m_xMin.z;
		float dz1 = m_xMax.z - P.z;
		if (dz0 < 0.0f || dz1 < 0.0f)
			return false;

		float dy0 = P.y - m_xMin.y;
		float dy1 = m_xMax.y - P.y;
		if (dy0 < 0.0f || dy1 < 0.0f)
			return false;
		
		float dx = (dx0 < dx1)? dx0 : -dx1;
		float dy = (dy0 < dy1)? dy0 : -dy1;
		float dz = (dz0 < dz1)? dz0 : -dz1;

		Vector D(dx, dy, dz);

		float min = fabs(dx);

		if (fabs(dy) < min)
		{
			min = fabs(dy);
			D.x = 0.0f;
		}
		else
		{
			D.y = 0.0f;
		}

		if (fabs(dz) < min)
		{
			D.x = D.y = 0.0f;
		}
		else
		{
			D.z = 0.0f;
		}

		P -= D;

		return true;
	}

public:
	Vector m_xMin;
	Vector m_xMax;
};



#endif OLI_BBOX_H