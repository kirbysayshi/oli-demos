//
// Body.h
//
// Created by OR - 05/03/2004 16:38:58
//
// Copyright (C) 2004 Rebellion, All Rights Reserved.
//

#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __BODY_H__
#define __BODY_H__


#include "Heightmap.h"
#include "Particle.h"
#include "Constraint.h"

class CBody
{
public:
	CBody()
	: m_pxParent(NULL)
	, m_iNumParticles(0)
	, m_iNumConstraints(0)
	, m_iNumBodies(0)
	{}

	~CBody()
	{
		while(m_iNumParticles)
		{
			RemoveParticle(m_pxParticles[0]);
		}
		while(m_iNumConstraints)
		{
			RemoveConstraint(m_pxConstraints[0]);
		}

		while (m_iNumBodies)
		{
			DetachBody(m_pxBodies[0]);
		}
	}

	CParticle* AddParticle(const CParticle& xParticle)
	{
		if (m_iNumParticles >= eMaxParticles)
			return NULL;

		m_pxParticles[m_iNumParticles] = new CParticle(xParticle);

		return m_pxParticles[m_iNumParticles++];
	}

	void RemoveParticle(CParticle* pxParticle)
	{
		for(int i = 0; i < m_iNumParticles; i ++)
		{
			if (m_pxParticles[i] == pxParticle)
			{
				delete m_pxParticles[i];
				
				m_pxParticles[i] = m_pxParticles[--m_iNumParticles];
				
				return;
			}
		}
	}

	CConstraint* AddConstraint(const CConstraint& xConstraint)
	{
		if (m_iNumConstraints >= eMaxConstraints)
			return NULL;

		m_pxConstraints[m_iNumConstraints] = new CConstraint(xConstraint);

		return m_pxConstraints[m_iNumConstraints++];
	}

	void RemoveConstraint(CConstraint* pxConstraint)
	{
		for(int i = 0; i < m_iNumConstraints; i ++)
		{
			if (m_pxConstraints[i] == pxConstraint)
			{
				delete m_pxConstraints[i];
				
				m_pxConstraints[i] = m_pxConstraints[--m_iNumConstraints];
				
				return;
			}
		}
	}

	CBody* AttachNewBody(CBody* pxBody)
	{
		if (m_iNumBodies >= eMaxBodies)
			return NULL;

		m_pxBodies[m_iNumBodies] = pxBody;

		pxBody->SetParent(this);

		return m_pxBodies[m_iNumBodies++];
	}

	void DetachBody(CBody* pxBody)
	{
		for(int i = 0; i < m_iNumBodies; i ++)
		{
			if (m_pxBodies[i] == pxBody)
			{
				pxBody->SetParent(NULL);
	
				delete m_pxBodies[i];

				m_pxBodies[i] = m_pxBodies[--m_iNumBodies];
				return;
			}
		}
	}

	virtual void UpdateParticles(float dt)
	{
		for(int i = 0; i < m_iNumParticles; i ++)
		{
			if (m_pxParticles[i])
				m_pxParticles[i]->Update(dt);
		}

		for(int i = 0; i < m_iNumBodies; i ++)
		{
			if (m_pxBodies[i])
				m_pxBodies[i]->UpdateParticles(dt);
		}
	}

	virtual void Collide(const CHeightmap& xHeightmap)
	{
		for(int i = 0; i < m_iNumParticles; i ++)
		{
			if (m_pxParticles[i])
				CollideParticle(*m_pxParticles[i], xHeightmap);
		}
		for(int i = 0; i < m_iNumBodies; i ++)
		{
			if (m_pxBodies[i])
				m_pxBodies[i]->Collide(xHeightmap);
		}
	}
	
	virtual void SatisfyConstraints()
	{
		for(int i = 0; i < m_iNumConstraints; i ++)
		{
			if (m_pxConstraints[i])
				m_pxConstraints[i]->SatisfyConstraint();
		}
	
		for(int i = 0; i < m_iNumBodies; i ++)
		{
			if (m_pxBodies[i])
				m_pxBodies[i]->SatisfyConstraints();
		}
	}


	virtual void Render(void) const
	{
		static bool bRenderParticles   = false;
		static bool bRenderConstraints = false;

		if (bRenderParticles || bRenderConstraints)
		{
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 1.0f);
			glLineWidth(1.0f);
			glPointSize(3.0f);

			if (bRenderParticles)
			{
				glBegin(GL_POINTS);
				for(int i = 0; i < m_iNumParticles; i ++)
				{
					if (m_pxParticles[i])
						m_pxParticles[i]->Render();
				}
				glEnd();
			}

			if (bRenderConstraints)
			{
				glBegin(GL_LINES);
				for(int i = 0; i < m_iNumConstraints; i ++)
				{
					if (m_pxConstraints[i])
						m_pxConstraints[i]->Render();
				}
				glEnd();
			}
		}

		for(int i = 0; i < m_iNumBodies; i ++)
		{
			if (m_pxBodies[i])
				m_pxBodies[i]->Render();
		}
	}


protected:
	void SetParent(CBody* pxParent) { m_pxParent = pxParent; }

	void InterlinkParticles()
	{
		for(int i = 0; i < m_iNumParticles; i ++)
		{
			if (m_pxParticles[i])
			{
				for(int j = i+1; j < m_iNumParticles; j ++)
				{
					if (m_pxParticles[j])
					{
						CConstraint xConstraint(m_pxParticles[i], m_pxParticles[j]);
						
						AddConstraint(xConstraint);
					}
				}
			}
		}
	}

private:
	void CollideParticle(CParticle& xParticle, const CHeightmap& xHeightmap)
	{
		Vector& P = xParticle.GetTargetPos();
		
		float fHeight = xHeightmap.GetHeight(P.x, P.z);

		if (P.y < fHeight)
		{
			P.y = fHeight;
			xParticle.SetCollided();
		}
	}

public:
	enum { eMaxParticles   = 128 };
	enum { eMaxConstraints = 512 };
	enum { eMaxBodies      =  16 };

	CParticle*		m_pxParticles[eMaxParticles];
	CConstraint*	m_pxConstraints[eMaxConstraints];
	CBody*			m_pxBodies[eMaxBodies];
	CBody*			m_pxParent;

	int m_iNumParticles;
	int m_iNumConstraints;
	int m_iNumBodies;
};

#endif//__BODY_H__