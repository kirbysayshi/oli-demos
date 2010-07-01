#ifndef OLI_CAR_H
#define OLI_CAR_H


/*
#include "Car.h"
*/

/*
------------------------------------------------------------------
File: Car.h
Started: 06/03/2004 00:38:16
  
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


#include "Wheel.h"
#include "Chassis.h"
#include "BBox.h"

class CCar: public CBody
{
public:
	CCar(const Vector& xCentre = Vector(frand(64) + 32, frand(2) + 1, frand(3) + 2), const Vector& xExtents = Vector(frand(0.3f) + 0.5f, frand(0.2f) + 0.3f, frand(0.3f) + 0.7f))
	{
		float fRadius = xExtents.y;

		m_pxChassis = new CChassis(xCentre, xExtents);
		m_pxFL = new CSteeredWheel(fRadius, m_pxChassis->m_pxParticles[3], m_pxChassis->m_pxParticles[2], m_pxChassis->m_pxParticles[0]);
		m_pxFR = new CSteeredWheel(fRadius, m_pxChassis->m_pxParticles[2], m_pxChassis->m_pxParticles[3], m_pxChassis->m_pxParticles[1]);

		m_pxBL = new CWheel(fRadius, m_pxChassis->m_pxParticles[0], m_pxChassis->m_pxParticles[1]);
		m_pxBR = new CWheel(fRadius, m_pxChassis->m_pxParticles[1], m_pxChassis->m_pxParticles[0]);

		AttachNewBody(m_pxChassis);
		AttachNewBody(m_pxFL);
		AttachNewBody(m_pxFR);
		AttachNewBody(m_pxBL);
		AttachNewBody(m_pxBR);

		UpdateBoundingBox();
	}

	void Steer(float fSteer)
	{
		float fAngle = fSteer * 0.4f;
		m_pxFL->Steer(-fAngle);
		m_pxFR->Steer( fAngle);
	}

	void Accelerate(float fThrottle)
	{
		float fSpin = fThrottle * 0.03f;

		m_pxFL->Rotate( fSpin);
		m_pxFR->Rotate(-fSpin);
		m_pxBL->Rotate( fSpin);
		m_pxBR->Rotate(-fSpin);
	}

	Vector GetCentre(void) const
	{
		return m_pxChassis->GetCentre();
	}

	virtual void UpdateParticles(float dt)
	{
		PARENT::UpdateParticles(dt);

		UpdateBoundingBox();
	}

	void Constrain(CCar& xCar)
	{
		if (!m_xBBox.Intersect(xCar.m_xBBox, 0.5f))
			return;

		if (!m_pxChassis || !xCar.m_pxChassis)
			return;

		for(int i = 0; i < xCar.m_pxChassis->m_iNumParticles; i++)
		{
			m_xBBox.Constrain(xCar.m_pxChassis->m_pxParticles[i]->GetTargetPos());
		}

		for(int i = 0; i < m_pxChassis->m_iNumParticles; i++)
		{
			xCar.m_xBBox.Constrain(m_pxChassis->m_pxParticles[i]->GetTargetPos());
		}
	}

	virtual void Render()
	{
		Vector Cen = (m_xBBox.m_xMax + m_xBBox.m_xMin) * 0.5f;
		Vector Ext = (m_xBBox.m_xMax - m_xBBox.m_xMin) * 0.5f;

		SetMaterial(2, 1.0f);
		glPushMatrix();
		glTranslatef(Cen.x, Cen.y, Cen.z);
		glScalef(Ext.x, Ext.y, Ext.z);
		glutWireCube(2.0f);
		glPopMatrix();

		PARENT::Render();
	}

private:
	CChassis* m_pxChassis;
	CSteeredWheel* m_pxFL;
	CSteeredWheel* m_pxFR;
	CWheel* m_pxBL;
	CWheel* m_pxBR;

	CBBox m_xBBox;

	void UpdateBoundingBox()
	{
		if (!m_pxChassis || m_pxChassis->m_iNumParticles == 0)
		{
			m_xBBox.Reset(Vector(0.0f, 0.0f, 0.0f));
			return;
		}


		m_xBBox.Reset(m_pxChassis->m_pxParticles[0]->GetCurrentPos());

		for(int i = 1; i < m_pxChassis->m_iNumParticles; i ++)
		{
			m_xBBox.Bound(m_pxChassis->m_pxParticles[i]->GetCurrentPos());
		}

		m_xBBox.Expand(0.3f);
	}

	typedef CBody PARENT;
};




#endif OLI_CAR_H