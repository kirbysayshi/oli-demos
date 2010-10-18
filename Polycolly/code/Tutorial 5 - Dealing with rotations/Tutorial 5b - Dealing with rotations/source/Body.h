#ifndef OLI_BODY_H
#define OLI_BODY_H


/*
#include "Body.h"
*/

/*
------------------------------------------------------------------
File: Body.h
Started: 07/02/2004 20:55:15
  
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
#include "Matrix.h"
#include "Polygon.h"

class CBody
{
public:
	CBody();
	
	CBody(const Vector& xPosition, float fMass, float width, float height);

	void Initialise(const Vector& xPosition, float fMass, Vector* axVertices, int iNumVertices);

	void Shutdown();
	
	~CBody();

	void AddImpulse(const Vector& F, float dt);
	
	bool Collide(CBody& Body);
	
	void Update(float dt);

	void Render() const;

	bool IsUnmovable() const { return (m_fMass < 0.0001f); }
	Vector& GetPosition		() { return m_xPosition; }
	Vector& GetDisplacement () { return m_xDisplacement; }
	float&  GetAngVelocity  () { return m_fAngVelocity; }
	float&  GetMass			() { return m_fMass; }
	float&  GetInvMass		() { return m_fInvMass; }
		
	const Vector& GetPosition	() const { return m_xPosition; }
	const Vector& GetDisplacement() const { return m_xDisplacement; }
	float GetMass				() const { return m_fMass; }
	float GetInvMass			() const { return m_fInvMass; }
	
public:
	static float GetFriction   () { return s_fFriction; }
	static float GetRestitution() { return s_fRestitution; }
	static float GetGlue	   () { return s_fGlue; }

	static SetFriction   (float fCoF) { s_fFriction    = fCoF; }
	static SetRestitution(float fCoR) { s_fRestitution = fCoR; }
	static SetGlue		 (float fCoS) { s_fGlue  = fCoS; }

private:
	static float s_fFriction;
	static float s_fRestitution;
	static float s_fGlue;

protected:
	void ProcessCollision(CBody& xBody, const Vector& N, float t);
	void ProcessOverlap  (CBody& xBody, const Vector& MTD);

	Vector* m_axVertices;
	int     m_iNumVertices;
	
	Vector	m_xDisplacement;
	Vector	m_xPosition;
	
	float	m_fMass;
	float	m_fInvMass;

	float m_fAngle;
	float m_fAngVelocity;
	Matrix m_xOrientation;
};



#endif OLI_BODY_H