//
// Box.h
//
// Created by OR - 05/03/2004 17:04:56
//
// Copyright (C) 2004 Rebellion, All Rights Reserved.
//

#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __BOX_H__
#define __BOX_H__

#include "Body.h"

class CBox: public CBody
{
public:
	CBox(const Vector& xCentre = Vector(frand(64) + 20, 5.0f, frand(64) + 20), const Vector& xExtents = Vector(frand(0.3f) + 0.1f, frand(0.3f) + 0.1f, frand(0.5) + 0.1f))
	{
		Vector xTopCentre  = xCentre;
		Vector xTopExtents = xExtents;
		Vector xBotCentre  = xCentre;
		Vector xBotExtents = xExtents;
		xTopExtents.x *= 0.8f;
		xTopExtents.y *= 0.5f;
		xTopExtents.z *= 0.65f;
		xTopCentre.z += xTopExtents.z * 0.1f;

		PARENT::AddParticle(CParticle(xBotCentre + Vector(-xBotExtents.x, -xBotExtents.y, -xBotExtents.z)));
		PARENT::AddParticle(CParticle(xBotCentre + Vector( xBotExtents.x, -xBotExtents.y, -xBotExtents.z)));
		PARENT::AddParticle(CParticle(xBotCentre + Vector( xBotExtents.x, -xBotExtents.y,  xBotExtents.z)));
		PARENT::AddParticle(CParticle(xBotCentre + Vector(-xBotExtents.x, -xBotExtents.y,  xBotExtents.z)));
		PARENT::AddParticle(CParticle(xTopCentre + Vector(-xTopExtents.x,  xTopExtents.y, -xTopExtents.z)));	
		PARENT::AddParticle(CParticle(xTopCentre + Vector( xTopExtents.x,  xTopExtents.y, -xTopExtents.z)));
		PARENT::AddParticle(CParticle(xTopCentre + Vector( xTopExtents.x,  xTopExtents.y,  xTopExtents.z)));
		PARENT::AddParticle(CParticle(xTopCentre + Vector(-xTopExtents.x,  xTopExtents.y,  xTopExtents.z)));

		InterlinkParticles();
	}

	Vector GetCentre(void) const
	{
		Vector Cen(0.0f, 0.0f, 0.0f);

		for(int i = 0; i < 8; i ++)
		{
			if (m_pxParticles[i])
			{
				Cen += m_pxParticles[i]->GetCurrentPos();
			}
		}

		Cen /= 8.0f;

		return Cen;
	}

private:
	typedef CBody PARENT;

};


#endif//__BOX_H__