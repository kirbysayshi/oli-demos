/*
------------------------------------------------------------------
File: Body.cpp
Started: 08/02/2004 12:05:02
  
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


float CBody::s_fFriction	= 0.0f;
float CBody::s_fRestitution = 0.1f;
float CBody::s_fGlue		= 0.01f;

extern float  dbg_world_size;

CBody::CBody()
{
	m_axVertices = 0;
	m_iNumVertices = 0;
	
	Vector xPos    = Vector(frand(dbg_world_size * 1.0f) + dbg_world_size * 0.0f, frand(dbg_world_size * 1.0f) + dbg_world_size * 0.0f);
	float  fSize   = frand(dbg_world_size) * 0.02f + dbg_world_size * 0.04f;
	float  fDensity= (frand(1.0f) < 0.7f)? 0.0f : 1.0f;
	
	if (fDensity < 0.0001f) fDensity = 0.0f;
	
	float fMass = fDensity * fSize * fSize;

	if (fMass < 0.0001f) fMass = 0.0f;
	
	int iNumVertices = rand() % 6 + 2;
	Vector* axVertices = PolyColl::BuildBlob(iNumVertices, fSize);

	Initialise(xPos, fMass, axVertices, iNumVertices);
}

void CBody::Shutdown()
{
	if (m_axVertices)
		delete[] m_axVertices;

	m_axVertices = NULL;
	m_iNumVertices = 0;
}
void CBody::Initialise(const Vector& xPosition, float fMass, Vector* axVertices, int iNumVertices)
{
	Shutdown();

	m_xDisplacement	= Vector(0, 0);
	m_xPosition		= xPosition;
	m_fMass			= fMass;
	m_fInvMass		= (fMass > 0.0000001f)? 1.0f / fMass : 0.0f;

	m_axVertices   = axVertices;
	m_iNumVertices = iNumVertices;

	m_fAngle = (iNumVertices != 4)? frand(3.0f) : 0.0f;
	m_fAngVelocity = frand(3.0f) + 1.0f;
	m_xOrientation = Matrix(m_fAngle);
}

CBody::CBody(const Vector& xPosition, float fMass, float width, float height)
{
	m_axVertices = 0;
	m_iNumVertices = 0;

	int iNumVertices = rand() % 8 + 3;
	Vector* axVertices = PolyColl::BuildBox(iNumVertices, width, height);
	Initialise(xPosition, fMass, axVertices, iNumVertices);
}

CBody::~CBody()
{
	Shutdown();
}

void CBody::AddImpulse(const Vector& F, float dt)
{
	if (IsUnmovable()) return;

	m_xDisplacement += F * (m_fInvMass * dt*dt);
}

void CBody::Update(float dt)
{
	if (IsUnmovable())
	{
		m_xDisplacement = Vector(0, 0);
		return;
	}
	m_xPosition += m_xDisplacement;

	m_fAngle      += m_fAngVelocity * dt;
	m_xOrientation = Matrix(m_fAngle);

	wrapf(m_fAngle, 0, TwoPi());
}

void CBody::Render() const
{	
	u_int uFill = (IsUnmovable())? 0x808080FF : 0x8080FF80;
	u_int uLine = (IsUnmovable())? 0xFF8080FF : 0xFF008000;

	PolyColl::Render(m_xPosition, m_fAngle, uFill, uLine, m_axVertices, m_iNumVertices);
}

bool CBody::Collide(CBody& xBody)
{
	if (IsUnmovable() && xBody.IsUnmovable())	
		return false;

    Vector xMTD;
    float  t = 1.0f;
	Vector N;

	const Vector* A  = m_axVertices;
	int Anum         = m_iNumVertices;
	const Vector& PA = m_xPosition;
	const Vector& VA = m_xDisplacement;
	Matrix OA        = m_xOrientation;//Matrix::Identity();
	
	const Vector* B  = xBody.m_axVertices;
	int Bnum         = xBody.m_iNumVertices;
	const Vector& PB = xBody.m_xPosition;
	const Vector& VB = xBody.m_xDisplacement;
	Matrix OB        = xBody.m_xOrientation;//Matrix::Identity();

	if (PolyColl::Collide(A, Anum, PA, VA, OA,
						  B, Bnum, PB, VB, OB,
						  N, t))
	{
		if (t < 0.0f)
		{
			ProcessOverlap(xBody, N * -t);
		}
		else
		{
			ProcessCollision(xBody, N, t);
		}
		return true;
	}
	return false;
}

// two objects collided at time t. stop them at that time
void CBody::ProcessCollision(CBody& xBody, const Vector& N, float t)
{
	Vector D = m_xDisplacement - xBody.m_xDisplacement;

	float n  = D * N;
	
	Vector Dn = N * n;
	Vector Dt = D - Dn;
	
	if (n > 0.0f) Dn = Vector(0, 0);

	float dt  = Dt * Dt;
	float CoF = s_fFriction;
	
	if (dt < s_fGlue*s_fGlue) CoF = 1.01f;

	D = -(1.0f + s_fRestitution) * Dn - (CoF) * Dt;

	float m0 =       GetInvMass();
	float m1 = xBody.GetInvMass();
	float m  = m0 + m1;
	float r0 = m0 / m;
	float r1 = m1 / m;
	
		  m_xDisplacement += D * r0;
	xBody.m_xDisplacement -= D * r1;
}

// two objects overlapped. push them away from each other
void CBody::ProcessOverlap(CBody& xBody, const Vector& xMTD)
{
	if (IsUnmovable())
	{
		xBody.m_xPosition -= xMTD;
	}
	else if (xBody.IsUnmovable())
	{
		m_xPosition += xMTD;
	}
	else
	{
			  m_xPosition += xMTD * 0.5f;
		xBody.m_xPosition -= xMTD * 0.5f;
	}

	Vector N = xMTD;
	N.Normalise();
	ProcessCollision(xBody, N, 0.0f);
}