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
	CBody(const Vector& xPosition, float fDensity, float width, float height);
	CBody(const Vector& Min, const Vector& Max, float fDensity);
	void Initialise(const Vector& xPosition, float fDensity, Vector* axVertices, int iNumVertices);
	void Initialise_Bullet(const Vector& xPosition, const Vector& xVelocity, float fMass);

	void Shutdown();
	
	~CBody();

	void AddForce(const Vector& F);
	
	void AddForce(const Vector& F, const Vector& P);
	
	bool Collide(CBody& Body, float dt);
	
	bool IntersectSegment(const Vector& Start, const Vector& End, float fDist, Vector& N, float& t) const;

	void Update(float dt);

	void Render() const;

	void SetOrientation(float fAngle);
	void SetDensity(float fDensity);

	bool IsUnmovable() const { return (m_fMass < 0.0001f); }
	Vector& GetPosition		() { return m_xPosition; }
	Vector& GetLinVelocity	() { return m_xVelocity; }
	float&  GetAngVelocity  () { return m_fAngVelocity; }
	float&  GetMass			() { return m_fMass; }
	float&  GetInvMass		() { return m_fInvMass; }
	float&  GetInertia		() { return m_fInertia; }
	float&  GetInvInertia	() { return m_fInvInertia; }
		
	const Vector& GetPosition	() const { return m_xPosition; }
	const Vector& GetLinVelocity() const { return m_xVelocity; }
	float GetAngVelocity		() const { return m_fAngVelocity; }
	float GetMass				() const { return m_fMass; }
	float GetInvMass			() const { return m_fInvMass; }
	float GetInvInertia			() const { return m_fInvInertia; }

	void SetCollisionCallback(bool (*fptrHandlePlayerContact)(const Vector& N, float& t, CBody* pxThisBody, CBody* pxOtherBody));

protected:
	void ProcessCollision(CBody& xBody, const Vector& N, float t);
	void ProcessOverlap  (CBody& xBody, const Vector& MTD);

	Vector* m_axVertices;
	int     m_iNumVertices;
	
	Vector	m_xVelocity;
	Vector	m_xPosition;

	float	m_fDensity;
	float	m_fMass;
	float   m_fInertia;
	float	m_fInvMass;
	float   m_fInvInertia;

	float m_fOrientation;
	float m_fAngVelocity;
	Matrix m_xOrientation;

	Vector m_xNetForce;
	float  m_fNetTorque;

	bool (*m_fptrHandlePlayerContact)(const Vector& N, float& t, CBody* pxThisBody, CBody* pxOtherBody);

};



#endif OLI_BODY_H