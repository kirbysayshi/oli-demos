
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "C:\dev\oli\GLut\glut.h"

#include "BodyList.h"

//-----------------------------------------------------------
// Holds all the bodies in the game	
// also holds the static meshes
//-----------------------------------------------------------
CBodyList::CBodyList()
: m_iNumBodies(0)
, m_iNumMeshes(0)
{
}

CBodyList::~CBodyList()
{
	DeleteAll();
}

void CBodyList::DeleteAll()
{
	while (m_iNumBodies)
		DeleteBody(m_pxBodies[0]);

	while (m_iNumMeshes)
		DeleteMesh(m_pxMeshes[0]);

}

void CBodyList::DeleteBody(CBody* pxBody)
{
	if (pxBody == NULL)
		return;

	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i] == pxBody)
		{
			delete m_pxBodies[i];

			m_iNumBodies--;
			m_pxBodies[i] = m_pxBodies[m_iNumBodies];
			m_pxBodies[m_iNumBodies] = NULL;
			return;
		}
	}
}

void CBodyList::NewBody(CBody* pxBody)
{
	if (m_iNumBodies >= eMaxBodies)
		return;

	m_pxBodies[m_iNumBodies] = pxBody;

	m_iNumBodies++;
}

void CBodyList::DeleteMesh(CMesh* pxMesh)
{
	if (pxMesh == NULL)
		return;

	for(int i = 0; i < m_iNumMeshes; i ++)
	{
		if (m_pxMeshes[i] == pxMesh)
		{
			delete m_pxMeshes[i];

			m_iNumMeshes--;
			m_pxMeshes[i] = m_pxMeshes[m_iNumMeshes];
			m_pxMeshes[m_iNumMeshes] = NULL;
			return;
		}
	}
}

void CBodyList::NewMesh(CMesh* pxMesh)
{
	if (m_iNumMeshes >= eMaxMeshes)
		return;

	m_pxMeshes[m_iNumMeshes] = pxMesh;

	m_iNumMeshes++;
}

//-----------------------------------------------------------
// update all bodies. Move their particles, apply 
// collision constraints, then stiff cosntraints
//-----------------------------------------------------------
void CBodyList::Update(float dt, const Vector* pxAttractor)
{

	//-----------------------------------------------------------
	// either attract the bodies to a position, or use gravity
	//-----------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i])
			m_pxBodies[i]->AttractParticles(pxAttractor);
	}

	
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i])
			m_pxBodies[i]->UpdateParticles(dt);
	}

	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i] == NULL)
			continue;

		//---------------------------------------------
		// Do special self collisions
		//---------------------------------------------
		m_pxBodies[i]->SelfCollide();
		
		for(int j = i+1; j < m_iNumBodies; j ++)
		{
			if (m_pxBodies[j] == NULL)
				continue;

			m_pxBodies[i]->Collide(*m_pxBodies[j]);
		}
	}

	//-----------------------------------------------------------
	// Collide with static geometry
	//-----------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i] == NULL)
			continue;

		for(int j = 0; j < m_iNumMeshes; j ++)
		{
			if (m_pxMeshes[j] == NULL)
				continue;

			m_pxMeshes[j]->Collide(*m_pxBodies[i]);
		}
	}

	//------------------------------------------------------------------
	// Update cosntraints
	//------------------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i])
		{
			m_pxBodies[i]->UpdateConstraints();
		}
	}

	//------------------------------------------------------------------
	// Update the bounding volume
	//------------------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i])
		{
			m_pxBodies[i]->UpdateBoundingVolume();
		}
	}
}

void CBodyList::Render()
{
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i])
			m_pxBodies[i]->Render();
	}
	for(int i = 0; i < m_iNumMeshes; i ++)
	{
		if (m_pxMeshes[i])
			m_pxMeshes[i]->Render();
	}
}
