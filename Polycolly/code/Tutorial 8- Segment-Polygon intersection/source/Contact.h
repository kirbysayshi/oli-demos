//------------------------------------------------------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// File          : Contact.h
// 
// Created by    : OR - 07/02/2004 12:42:04
// 
// Copyright (C) : 2004 Rebellion, All Rights Reserved.
// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// Description   : 
// --------------- 
// 
// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
//------------------------------------------------------------------------------------------------ 


#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __CONTACT_H__
#define __CONTACT_H__

#include "Vector.h"

class CContact
{
public:
	enum { eMaxContacts = 2 };

	class CBody* m_pxBodies [2];
	Vector       m_xContacts[eMaxContacts][2];
	Vector		 m_xNormal;
	float		 m_t;
	int			 m_iNumContacts;
	
	CContact();
	
	CContact(const Vector* CA, const Vector* CB, int iCnum, 
			 const Vector& N, float t, 
			 CBody* pxBodyA, CBody* pxBodyB);

	void Reset();
		
	void Render(void) const;

	class CBody* GetBody(int i) { return m_pxBodies[i]; }

	void Solve();
	
private:
	void ResolveCollision();
	void ResolveOverlap  ();

	void ResolveCollision(const Vector& CA, const Vector& CB);
	void ResolveOverlap  (const Vector& CA, const Vector& CB);
	void AddContactPair	 (const Vector& CA, const Vector& CB);
};


class CMaterial
{
public:
	CMaterial(float fCoF = 0.2f, float fCoR = 0.3f, float fCoS = 0.4f, float fSep=0.5f)
	: m_fCoF(fCoF)
	, m_fCoR(fCoR)
	, m_fCoS(fCoS)
	, m_fSep(fSep)
	{}

	void SetSeparation		(float fSep) { m_fSep = fSep; }
	void SetFriction		(float fCoF) { m_fCoF = fCoF; }
	void SetStaticFriction	(float fCoS) { m_fCoS = fCoS; }
	void SetRestitution		(float fCoR) { m_fCoR = fCoR; }

	float GetSeparation		() const { return m_fSep; }
	float GetFriction		() const { return m_fCoF; }
	float GetStaticFriction	() const { return m_fCoS; }
	float GetRestitution	() const { return m_fCoR; }

private:
	float m_fCoF, m_fCoR, m_fSep, m_fCoS;
};

// HACK : use a shared material for all objects
extern CMaterial s_xContactMaterial;

#endif//__CONTACT_H__