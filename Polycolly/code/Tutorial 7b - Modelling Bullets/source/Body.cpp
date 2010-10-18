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
#include "Contact.h"

extern float  dbg_world_size;

CBody::CBody()
{
	m_fptrHandlePlayerContact = NULL;
	m_axVertices = 0;
	m_iNumVertices = 0;
	
	Vector xPos    = Vector(frand(dbg_world_size * 1.0f) + dbg_world_size * 0.0f, frand(dbg_world_size * 1.0f) + dbg_world_size * 0.0f);
	float  fHeight = frand(dbg_world_size) * 0.04f + dbg_world_size * 0.02f;
	float  fWidth  = fHeight * (frand(0.8f) + 0.6f);
	float  fDensity= (frand(1.0f) < 0.4f)? 0.0f : 1.0f;
	
	int iNumVertices = rand() % 6 + 2;
	Vector* axVertices = PolyColl::BuildBlob(iNumVertices, fWidth, fHeight);

	Initialise(xPos, fDensity, axVertices, iNumVertices);
}

void CBody::Shutdown()
{
	if (m_axVertices)
		delete[] m_axVertices;

	m_axVertices = NULL;
	m_iNumVertices = 0;

	m_xNetForce = Vector(0, 0);
	m_fNetTorque = 0.0f;
	m_xVelocity	= Vector(0, 0);
}

CBody::CBody(const Vector& Min, const Vector& Max, float fDensity)
{
	m_fptrHandlePlayerContact = NULL;
	m_axVertices = 0;
	m_iNumVertices = 0;
	
	Vector xPos = (Max + Min) * 0.5f;
	Vector xExt = (Max - Min) * 0.5f;
		
	int iNumVertices;
	Vector* axVertices = PolyColl::BuildBox(iNumVertices, xExt.x, xExt.y);
	
	Initialise(xPos, fDensity, axVertices, iNumVertices);
}

void CBody::Initialise_Bullet(const Vector& xPosition, const Vector& xVelocity, float fMass)
{
	Shutdown();

	m_xNetForce    = Vector(0, 0);
	m_fNetTorque   = 0.0f;
	m_xVelocity	   = Vector(0, 0);
	m_xPosition    = xPosition;
	m_fAngVelocity = 0.0f;
	SetOrientation(0.0f);

	int iNumVertices = 1;
	Vector* axVertices = PolyColl::BuildBlob(iNumVertices, 0, 0);
	m_xVelocity = xVelocity;

	m_axVertices   = axVertices;
	m_iNumVertices = iNumVertices;

	m_fMass    = fMass;
	m_fInertia = m_fMass * 10.0f;
	m_fInvMass = (m_fMass > 0.0f)? 1.0f / m_fMass : 0.0f;
	m_fInvInertia = (m_fInertia > 0.0f)? 1.0f / m_fInertia : 0.0f;
}


void CBody::Initialise(const Vector& xPosition, float fDensity, Vector* axVertices, int iNumVertices)
{
	Shutdown();

	m_xPosition = xPosition;
	
	m_axVertices   = axVertices;
	m_iNumVertices = iNumVertices;

	SetDensity(fDensity);

	m_fAngVelocity = 0.0f; //frand(3.0f) + 1.0f;

	if (!IsUnmovable() || m_iNumVertices > 4)
		SetOrientation(frand(3.0f) + 1.0f);
	else
		SetOrientation(0.0f);
}

void CBody::SetOrientation(float fAngle)
{
	m_fOrientation = fAngle;
	m_xOrientation = Matrix(m_fOrientation);
}

CBody::CBody(const Vector& xPosition, float fDensity, float width, float height)
{
	m_fptrHandlePlayerContact = NULL;
	m_axVertices = 0;
	m_iNumVertices = 0;

	int iNumVertices;
	Vector* axVertices = PolyColl::BuildBox(iNumVertices, width, height);
	
	Initialise(xPosition, fDensity, axVertices, iNumVertices);
}

void CBody::SetDensity(float fDensity)
{
	m_fMass			= 0.0f;
	m_fInertia		= 0.0f;
	m_fInvMass		= 0.0f;
	m_fInvInertia	= 0.0f;
	m_fDensity		= fDensity;

	if (m_fDensity > 0.0f && m_axVertices && m_iNumVertices) 
	{
		m_fMass			= PolyColl::CalculateMass   (m_axVertices, m_iNumVertices, m_fDensity);
		m_fInertia		= PolyColl::CalculateInertia(m_axVertices, m_iNumVertices, m_fMass);
		m_fInvMass		= (m_fMass > 0.0f)? 1.0f / m_fMass : 0.0f;
		m_fInvInertia	= (m_fInertia > 0.0f)? 1.0f / m_fInertia : 0.0f;
	}
}

CBody::~CBody()
{
	Shutdown();
}

void CBody::AddForce(const Vector& F)
{
	if (IsUnmovable()) return;

	m_xNetForce += F;
}

void CBody::AddForce(const Vector& F, const Vector& P)
{
	if (IsUnmovable()) return;

	m_xNetForce += F;
	m_fNetTorque -= (P - m_xPosition) ^ F; // SIGN IS WRONG?!?
}

void CBody::Update(float dt)
{
	if (IsUnmovable())
	{
		m_xVelocity = Vector(0, 0);
		m_fAngVelocity = 0.0f;
		return;
	}

	//-------------------------------------------------------
	// Integrate position (verlet integration)
	//-------------------------------------------------------
	m_xPosition		+= m_xVelocity * dt;
	m_fOrientation	+= m_fAngVelocity * dt;
	m_fOrientation   = wrapf(m_fOrientation, -TwoPi(), TwoPi());
	m_xOrientation   = Matrix(m_fOrientation);

	//-------------------------------------------------------
	// Integrate velocity (implicit linear velocity)
	//-------------------------------------------------------
	m_xVelocity		+= m_xNetForce  * (m_fInvMass    * dt);
	m_fAngVelocity	+= m_fNetTorque * (m_fInvInertia * dt);

	//-------------------------------------------------------
	// clear forces
	//-------------------------------------------------------
	m_xNetForce		 = Vector(0.0f, 0.0f);
	m_fNetTorque	 = 0.0f;
}

void CBody::Render() const
{
	u_int uFill = (IsUnmovable())? 0x808080FF : 0x8080FF80;
	u_int uLine = (IsUnmovable())? 0xFF8080FF : 0xFF00A000;

	if (m_iNumVertices == 1)
	{
		uFill = uLine = 0xFFFF0000;
	}

	PolyColl::Render(m_xPosition, m_fOrientation, uFill, uLine, m_axVertices, m_iNumVertices);
}

bool CBody::IntersectSegment(const Vector& xStart, const Vector& xEnd, float fDist, Vector& N, float& t) const
{
	if (m_iNumVertices <= 2)
		return false;

	Vector Nnear;
	Vector Nfar;
	float tnear = 0.0f;
	float tfar = fDist;
	bool bIntersect = PolyColl::ClipSegment(m_axVertices, m_iNumVertices, 
											m_xPosition, m_xVelocity, m_xOrientation, 
											xStart, xEnd, tnear, tfar, Nnear, Nfar);

	if (bIntersect)
	{
		if (tnear < fDist)
		{
			N = Nnear;
			t = tnear;
			return true;
		}
	}
	return false;
}


bool CBody::Collide(CBody& xBody, float dt)
{
	if (IsUnmovable() && xBody.IsUnmovable())	
		return false;

	float  t = dt;
	Vector N;
	
	const Vector* A  = m_axVertices;
	int Anum         = m_iNumVertices;
	const Vector& PA = m_xPosition;
	const Vector& VA = m_xVelocity;
	Matrix OA        = m_xOrientation;//Matrix::Identity();
	
	const Vector* B  = xBody.m_axVertices;
	int Bnum         = xBody.m_iNumVertices;
	const Vector& PB = xBody.m_xPosition;
	const Vector& VB = xBody.m_xVelocity;
	Matrix OB        = xBody.m_xOrientation;//Matrix::Identity();

	
	if (PolyColl::Collide(A, Anum, PA, VA, OA,
						  B, Bnum, PB, VB, OB,
						  N, t))
	{
		if (m_fptrHandlePlayerContact)
		{
			if (!m_fptrHandlePlayerContact(N, t, this, &xBody))
				return true;
		}

		if (xBody.m_fptrHandlePlayerContact)
		{
			if (!xBody.m_fptrHandlePlayerContact(-N, t, &xBody, this))
				return true;
		}

		Vector CA[4];
		Vector CB[4];
		int Cnum;

		PolyColl::FindContacts( A, Anum, PA, VA, OA, 
								B, Bnum, PB, VB, OB, 
								N, t, 
								CA, CB, Cnum);

		CContact xContact(CA, CB, Cnum, N, t, this, &xBody);
		xContact.Solve();
		return true;
	}
	return false;
}

void CBody::SetCollisionCallback(bool (*fptrHandlePlayerContact)(const Vector& N, float& t, CBody* pxThisBody, CBody* pxOtherBody))
{
	m_fptrHandlePlayerContact = fptrHandlePlayerContact;
}
