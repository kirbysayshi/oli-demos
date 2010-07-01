#ifndef OLI_CHASSIS_H
#define OLI_CHASSIS_H


/*
#include "Chassis.h"
*/

/*
------------------------------------------------------------------
File: Chassis.h
Started: 06/03/2004 00:32:12
  
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


#include "Body.h"

class CChassis: public CBody
{
public:
	CChassis(const Vector& xCentre = Vector(frand(64) + 20, 5.0f, frand(64) + 20), const Vector& xExtents = Vector(frand(0.1f) + 0.3f, frand(0.1f) + 0.2f, frand(0.1f) + 0.4f))
	{
		Vector xTopCentre  = xCentre;
		Vector xTopExtents = xExtents;
		Vector xBotCentre  = xCentre;
		Vector xBotExtents = xExtents;
		xTopExtents.x *= 0.9f;
		xTopExtents.y *= 0.5f;
		xTopExtents.z *= 0.8f;
		xTopCentre.z -= xTopExtents.z * 0.1f;

		PARENT::AddParticle(CParticle(xBotCentre + Vector(-xBotExtents.x, -xBotExtents.y, -xBotExtents.z)));
		PARENT::AddParticle(CParticle(xBotCentre + Vector( xBotExtents.x, -xBotExtents.y, -xBotExtents.z)));
		PARENT::AddParticle(CParticle(xBotCentre + Vector( xBotExtents.x, -xBotExtents.y,  xBotExtents.z)));
		PARENT::AddParticle(CParticle(xBotCentre + Vector(-xBotExtents.x, -xBotExtents.y,  xBotExtents.z)));
		PARENT::AddParticle(CParticle(xTopCentre + Vector(-xTopExtents.x,  xTopExtents.y, -xTopExtents.z), 0.6f));	
		PARENT::AddParticle(CParticle(xTopCentre + Vector( xTopExtents.x,  xTopExtents.y, -xTopExtents.z), 0.6f));
		PARENT::AddParticle(CParticle(xTopCentre + Vector( xTopExtents.x,  xTopExtents.y,  xTopExtents.z), 0.6f));
		PARENT::AddParticle(CParticle(xTopCentre + Vector(-xTopExtents.x,  xTopExtents.y,  xTopExtents.z), 0.6f));

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

	virtual void Render(void) const
	{
		static int t[] = { 0, 1, 2, 0, 2, 3, 
						   4, 5, 6, 4, 6, 7, 
						   0, 1, 5, 0, 5, 4, 
						   2, 3, 6, 3, 6, 7,
						   0, 3, 7, 0, 7, 4,
						   1, 2, 6, 1, 6, 5,
		};

		glEnable(GL_LIGHTING);

		SetMaterial(4, 1.0f);
		
		glBegin(GL_TRIANGLES);

		for(int i = 0; i < sizeof(t) / sizeof(*t); i += 3)
		{
			Vector N;
			N.ComputeNormal(m_pxParticles[t[i + 0]]->GetCurrentPos(), 
							m_pxParticles[t[i + 1]]->GetCurrentPos(),
							m_pxParticles[t[i + 2]]->GetCurrentPos());
			glNormal3f(N.x, N.y, N.z);
			m_pxParticles[t[i + 0]]->Render();
			m_pxParticles[t[i + 1]]->Render();
			m_pxParticles[t[i + 2]]->Render();
		}
		glEnd();

		PARENT::Render();
	}

private:
	typedef CBody PARENT;

};



#endif OLI_CHASSIS_H