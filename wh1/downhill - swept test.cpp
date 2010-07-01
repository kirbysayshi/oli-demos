#include <gl\glut.h>
#include <math.h>
#include <stdio.h>

#define KeyPressed(key) (GetAsyncKeyState(key)&0x8000)

#define DebouncedKeyPressed(key) (GetAsyncKeyState(key)&0x0001)

#define TIMESTEP 0.03

int sgn(float a)
{
	return (a>0);
}
float clamp(float x, float min, float max)
{
	return (x < min)? min : (x > max)? max : x;
}


struct Vector
{
	float x, y;

	Vector()
	{}

	Vector(float _x, float _y)
	: x(_x)
	, y(_y)
	{}

	Vector& operator +=(const Vector& V) { x += V.x;  y += V.y; return *this; }
	Vector& operator -=(const Vector& V) { x -= V.x;  y -= V.y; return *this; }
	Vector& operator *=(float k)         { x *= k  ;  y *= k  ; return *this; }
	Vector& operator /=(float k)         { x /= k  ;  y /= k  ; return *this; }
		
	Vector operator + (const Vector& V) const { return Vector(x + V.x, y + V.y); }
	Vector operator - (const Vector& V) const { return Vector(x - V.x, y - V.y); }
	Vector operator * (float k)         const { return Vector(x * k, y * k); }
	Vector operator / (float k)         const { return Vector(x / k, y / k); }
	
	float  operator * (const Vector& V) const { return x * V.x + y * V.y; }
	float  operator ^ (const Vector& V) const { return x * V.y - y * V.x; }

	float	Length() const { return sqrt(x*x + y*y); }
	float	Normalise()    { float l = Length(); x /= l; y /= l; return l; }

	Vector  Scale   (const Vector& xScale) const { return Vector(x * xScale.x,  y * xScale.y); }
	Vector  InvScale(const Vector& xScale) const { return Vector(x / xScale.x,  y / xScale.y); }

	Vector Rotate(const Vector& C, float a) const
	{
		float px = (x - C.x) * cos(a) - (y - C.y) * sin(a) + C.x;
		float py = (x - C.x) * sin(a) + (y - C.y) * cos(a) + C.y;

		return Vector(px, py);
	}
};

struct CColour
{
	float r;
	float g;
	float b;
	float a;

	CColour(float R, float G, float B, float A)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	}
	void Render() const
	{
		glColor4f(r, g, b, a);
	}
};


int screen_width  = 640;
int screen_height = 480;
Vector cam_min;
Vector cam_max;
Vector cam_cen;
float  cam_zoom;


struct CAABBox
{
	CAABBox()
	{}

	CAABBox(float minx, float miny, float maxx, float maxy)
	{
		m_xMin = Vector(minx, miny);
		m_xMax = Vector(maxx, maxy);
	}

	CAABBox(const Vector& xMin, const Vector& xMax)
	{
		m_xMin = xMin;
		m_xMax = xMax;
	}

	bool Intersect(const CAABBox& xBox) const
	{
		if (m_xMin.x > xBox.m_xMax.x || m_xMax.x < xBox.m_xMin.x) return false;
		if (m_xMin.y > xBox.m_xMax.y || m_xMax.y < xBox.m_xMin.y) return false;
		return true;
	}

	bool Contains(const Vector& xP) const
	{
		if (m_xMin.x > xP.x || m_xMax.x < xP.x) return false;
		if (m_xMin.y > xP.y || m_xMax.y < xP.y) return false;
		return true;
	}

	Vector GetCentre  () const { return (m_xMax + m_xMin) * 0.5f; }
	Vector GetHalfSize() const { return (m_xMax + m_xMin) * 0.5f; }
	Vector GetSize    () const { return (m_xMax + m_xMin); }

	void Reset(const Vector& xP=Vector(0.0f, 0.0f))
	{
		m_xMin = m_xMax = xP;
	}

	void Bound(const Vector& xP)
	{
		if (xP.x < m_xMin.x) m_xMin.x = xP.x;
		if (xP.y < m_xMin.y) m_xMin.y = xP.y;
		if (xP.x > m_xMax.x) m_xMax.x = xP.x;
		if (xP.y > m_xMax.y) m_xMax.y = xP.y;
	}

	void Expand(float threshold)
	{
		m_xMin -= Vector(threshold, threshold);
		m_xMax += Vector(threshold, threshold);
	}

	void Render(CColour col=CColour(0.0f, 1.0f, 0.0f, 0.3f)) const
	{
		col.Render();
		glLineWidth(1);
		glBegin(GL_QUADS);
		glVertex2f(m_xMin.x, m_xMin.y);
		glVertex2f(m_xMax.x, m_xMin.y);
		glVertex2f(m_xMax.x, m_xMax.y);
		glVertex2f(m_xMin.x, m_xMax.y);
		glEnd();
	}
	
	Vector m_xMin;
	Vector m_xMax;
};

struct CCamera
{	
	CCamera(const CAABBox& xViewport = CAABBox(Vector(0.0f, 0.0f), Vector(1.0f, 1.0f)), 
			const Vector& xScreenSize=Vector(640, 480), 
			float fAspectRatio=1.0f, 
			float fZoom=1.0f, 
			const Vector& xPos=Vector(0.0f, 0.0f))
	{
		m_xScreenSize   = xScreenSize;
		m_fAspectRatio	= fAspectRatio;
		m_xViewport		= xViewport;
		m_fZoom			= fZoom;
		m_xPos			= xPos;

		CalculateViewArea();
		
	}

	void ReshapeScreen(const Vector& xScreenSize)
	{
		m_xScreenSize = xScreenSize;
		
		CalculateViewArea();
	}

	void MoveCamera(const Vector& xPos, float fZoom = -1.0f)
	{
		m_xPos = xPos;
		
		if (fZoom > 0.0f)
			m_fZoom = fZoom;

		CalculateViewArea();
	}

	bool IsVisible(const CAABBox& xBox) const
	{
		return m_xViewArea.Intersect(xBox);
	}

	void ApplyRenderSettings()
	{
		Vector xMin = m_xViewport.m_xMin.Scale(m_xScreenSize);
		Vector xMax = m_xViewport.m_xMax.Scale(m_xScreenSize);

		glViewport(	xMin.x,  xMin.y, 
					xMax.x - xMin.x, 
					xMax.y - xMin.y);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(m_xViewArea.m_xMin.x, m_xViewArea.m_xMax.x,
				   m_xViewArea.m_xMax.y, m_xViewArea.m_xMin.y);
					
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void CalculateViewArea()
	{
		Vector xViewSize = m_xScreenSize / m_fZoom;
		xViewSize.InvScale(m_xViewport.m_xMax - m_xViewport.m_xMin);

		Vector xViewportRatio = m_xViewport.m_xMax - m_xViewport.m_xMin;
		m_fAspectRatio = xViewportRatio.y / xViewportRatio.x;
		xViewSize.y *= m_fAspectRatio;
		
		m_xViewArea.m_xMin.x = m_xPos.x - xViewSize.x;
		m_xViewArea.m_xMin.y = m_xPos.y - xViewSize.y;
		m_xViewArea.m_xMax.x = m_xPos.x + xViewSize.x;
		m_xViewArea.m_xMax.y = m_xPos.y + xViewSize.y;
	}

	Vector	m_xScreenSize;
	Vector	m_xPos;
	float	m_fZoom;
	float	m_fAspectRatio;

	CAABBox m_xViewport;
	CAABBox m_xViewArea;
};

struct TParticle
{
	TParticle(const Vector& xPos=Vector(0.0f, 0.0f), float fMass=1.0f, float fFriction=0.1f, float fRadius=5.0f)
	{
		m_xForces		= Vector(0.0f, 0.0f);
		m_xCurrPos		= xPos;
		m_xNewPos		= xPos;
		m_fFriction		= fFriction;
		m_fMass			= fMass;
		m_fInvMass		= (m_fMass > 0.0f)? 1.0f / m_fMass : 0.0f;
		m_fRadius		= fRadius;
		
		CalculateBoundingBox();
	}

	const Vector& GetCurrentPos() const { return m_xCurrPos; }
	const Vector& GetTargetPos () const { return m_xNewPos;  }
		  Vector& GetTargetPos ()		{ return m_xNewPos;  }
	Vector		GetDisplacement() const { return m_xNewPos - m_xCurrPos; }
	float			  GetRadius() const { return m_fRadius;  }
	
	void SetCurrentPos(const Vector& xPos) { m_xCurrPos = xPos; CalculateBoundingBox(); }
	void SetTargetPos (const Vector& xPos) { m_xNewPos  = xPos; CalculateBoundingBox(); }

	void AddForce(const Vector& xForce)
	{
		m_xForces += xForce;
	}

	void Update(float dt)
	{
//		if (m_xNewPos.y > 200.0f)
//			printf("ERROR : Particle::Update()\n");

		AddForce(Vector(0, 40 * m_fMass));

		float friction = 1.0f;

		if (m_bCollided)
		{
			friction = (1.0f - m_fFriction);
		}

		Vector Temp = m_xNewPos;
		m_xNewPos  += (m_xNewPos - m_xCurrPos) * friction + m_xForces * (m_fInvMass * (dt*dt));
		m_xCurrPos	= Temp;
		m_xForces   = Vector(0.0f, 0.0f);
		m_bCollided = false;

		CalculateBoundingBox();
	}

	void Slide(float t, const Vector& N)
	{
		// collision response parameters
		float elasticity = 1.0f;
		float friction	 = m_fFriction;
		float threshold  = 0.1f; // avoid floating point innacuracy problems

		// current particle displacement
		Vector D = (m_xNewPos - m_xCurrPos)	;

		// new particle position at point of impact
		m_xCurrPos += D * t + N * threshold;

		// impact velocity
		float dn = (D * N);

		// make sure it's an impact, not a separation
		if (dn > 0.0f)
			dn = 0.0f;

		// split impact along normal and collision plane
		Vector Dn = N * dn;
		Vector Dt = D - Dn;

		// calculate impact response
		D = Dn * -elasticity + Dt * (1.0f - friction);

		// calcualte new particle target position after impact,
		// and add the threshold to conserve the momentum intact
		m_xNewPos = m_xCurrPos + D + N * threshold;

		// flag as collided this frame
		m_bCollided = true;
	}

	void Render(CColour col = CColour(0.0f, 1.0f, 0.0f, 1.0f))
	{
		col.Render();
		glBegin(GL_POINTS);
		glVertex2f(m_xCurrPos.x, m_xCurrPos.y);
		glEnd();
	}

	void CalculateBoundingBox()
	{
		m_xBox.Reset(m_xCurrPos);
		m_xBox.Bound(m_xNewPos);
		m_xBox.Expand(1.0f);
	}
	
	Vector	m_xCurrPos;
	Vector	m_xNewPos;
	Vector	m_xForces;
	float	m_fFriction;
	float	m_fRadius;
	float	m_fMass;
	float	m_fInvMass;
	bool    m_bCollided;
	CAABBox m_xBox;
};

struct TObstacle
{
	TObstacle()
	{}

	TObstacle(const Vector& xP0, const Vector& xP1)
	{
		m_xP0	  = xP0;
		m_xP1	  = xP1;
		Update();
	}

	void Update()
	{
		m_xDir			= m_xP1 - m_xP0;
		m_fLength		= m_xDir.Normalise();
//		m_xPlaneNormal	= Vector(-m_xDir.y, m_xDir.x);
		m_xPlaneNormal	= Vector( m_xDir.y,-m_xDir.x); // inverted, since game is in screen coords. sys.
		m_fPlaneDist	= m_xPlaneNormal * m_xP0;

		m_xBox.Reset(m_xP0);
		m_xBox.Bound(m_xP1);
		m_xBox.Expand(1.0f);
	}

	void Render(CColour col=CColour(0.1f, 1.0f, 0.1f, 1.0f)) const
	{
		col.Render();
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2f(m_xP0.x,m_xP0.y);
		glVertex2f(m_xP1.x,m_xP1.y);
		glEnd();

//		m_xBox.Render();
	}

	bool Collide(const TParticle& xParticle, float &tcoll, Vector& Ncoll) const
	{
//		if (!m_xBox.Intersect(xParticle.m_xBox))
//			return false;

		const Vector&	e0 = m_xP0;
		const Vector&	e1 = m_xP1;
		const Vector&	e  = m_xDir;
		float			l  = m_fLength;
		Vector			d  = xParticle.GetDisplacement();
		float			r  = xParticle.GetRadius();
		const Vector&	p0 = xParticle.GetCurrentPos();
		const Vector&	p1 = xParticle.GetTargetPos();
		const Vector&	n  = m_xPlaneNormal;
		float			m  = m_fPlaneDist;
		float			s0 = p0 * n;
		float			s1 = p1 * n;
		
		float t = (m - s0) / (s1 - s0);

		if (t > tcoll || t < 0.0f)
			return false;

		Vector pcoll;
		pcoll = p0 + d * t;
		
		float le = (pcoll - e0) * e;

		if (le < 0.0f || le > l)
			return false;
	
		tcoll = t;
		Ncoll = n;
		
		return true;
	}

public:
	Vector	m_xP0;
	Vector	m_xP1;
	Vector	m_xDir;
	Vector  m_xPlaneNormal;
	float   m_fPlaneDist;
	float	m_fLength;
	CAABBox m_xBox;
};



struct CTerrain
{
	CTerrain(float fRandomness)
	{
		m_xFirstVert = Vector(300.0f, 200.0f);
		m_xLastVert = m_xFirstVert;

		int r = 0.0f;
		int sgn_dr = 1;

		for (int i = 0; i < eNumObstacles; i++)
		{
			Vector Point = m_xLastVert;

			int prob_plus  = (48 + fRandomness * 16);
			int prob_minus = (20 + fRandomness * 8);

			int dr =  rand() % (prob_plus) - rand() % (prob_minus);

			if ((i % 4) == 0)
			{
				if (r > 0)
					sgn_dr = -1;
				else
					sgn_dr = 1;
			}

			r += dr * sgn_dr;


			m_xLastVert = Vector(m_xLastVert.x + 100.0f, m_xLastVert.y + r);

			m_xObstacles[i] = TObstacle(Point, m_xLastVert);
		}
	}

	void Collide(TParticle* xParticles, int iNumParticles) const
	{
		for(int j = 0; j < iNumParticles; j ++)
		{
			float  tcoll;
			Vector Ncoll;

			const int iter = 2;

			for(int i = 0; i < iter; i ++)
			{
				if (Collide(xParticles[j], tcoll, Ncoll) == false)
					break;
			
				xParticles[j].Slide(tcoll, Ncoll);
			}

			if (i == iter) 
			{
				printf("Too many collisions\n");
				xParticles[j].m_xNewPos = xParticles[j].m_xCurrPos;
			}
		}
	}


	bool Collide(const TParticle& xParticle, float &tcoll, Vector& Ncoll) const
	{
		bool bCollided = false;
		tcoll = 1.0f;

		for(int i =0; i < eNumObstacles; i ++)
		{
			if (m_xObstacles[i].Collide(xParticle, tcoll, Ncoll))
			{
				bCollided = true;
			}
		}

		return bCollided;
	}

	void Render(const CCamera& Camera) const
	{
		for(int i = 0; i < eNumObstacles; i ++)
		{
			if (Camera.IsVisible(m_xObstacles[i].m_xBox))
				m_xObstacles[i].Render();
		}
	}

	enum { eNumObstacles = 100 };
	TObstacle	m_xObstacles[eNumObstacles];
	Vector		m_xFirstVert;
	Vector		m_xLastVert;
};

struct TConstraint
{
	TConstraint(TParticle* pxParticle0=NULL, TParticle* pxParticle1=NULL, float fRigidity=1.0f, float fMinLength=-1.0f)
	{
		m_pxParticle[0] = pxParticle0;
		m_pxParticle[1] = pxParticle1;

		if (!pxParticle0 || !pxParticle1)
			return;

		m_fRigidity		= fRigidity;
		m_fMinLength	= fMinLength;
		m_fRestLength	= (m_pxParticle[0]->GetCurrentPos() - m_pxParticle[1]->GetCurrentPos()).Length();

		if (m_fMinLength < 0.0f)
			m_fMinLength = m_fRestLength * 0.5f;
	}

	float CalculateRigidity(float delta2)
	{
		m_fStress = 0.0f;
		
		if (m_fRigidity == 1.0f)
			return 1.0f;

		float r       = m_fRestLength - m_fMinLength;
		float l2      = m_fRestLength * m_fRestLength;
		float r2	  = r * r;
		float d2      = fabs(delta2 - l2);
		
		if (d2 > r2)
		{
			m_fStress = 1.0f;
			return 1.0f;
		}

		float ratio = d2 / r2;

		float fRigidity = m_fRigidity + (ratio * ratio) * (1.0f - m_fRigidity);

		m_fStress = (ratio * ratio); 

		return fRigidity;
	}

	void SatisfyConstraints()
	{
		if (!m_pxParticle[0] || !m_pxParticle[1])
			return;

		//-----------------------------------------------------------
		// cache stuff
		//-----------------------------------------------------------
		float restlength  = m_fRestLength;
		float restlength2 = restlength*restlength;
		float invmass1    = m_pxParticle[0]->m_fInvMass;
		float invmass2    = m_pxParticle[1]->m_fInvMass;
		float invmass     = invmass1+invmass2;
	
		if (invmass < 0.00001f)
			return;

		//-----------------------------------------------------------
		// relative position of particles
		//-----------------------------------------------------------
		Vector& x1        = m_pxParticle[0]->GetTargetPos();
		Vector& x2        = m_pxParticle[1]->GetTargetPos();
		Vector delta      = x2 - x1;

		float diff;
		float delta2 = delta*delta;
		
		float fRigidity = CalculateRigidity(delta2);

		//-----------------------------------------------------------
		// Using Square root approximation
		// calcualte the difference from the current distance to the ideal distance
		//-----------------------------------------------------------
		diff  = restlength2/(delta2 + restlength2)-0.5f;
		diff *= -2.0f;

		//-----------------------------------------------------------
		// move particles so their distance = ideal distance
		//-----------------------------------------------------------
		delta *= diff / invmass;
		x1    += delta*(invmass1) * fRigidity;
		x2    -= delta*(invmass2) * fRigidity;
  }


	void Render(void) const
	{
		if (!m_pxParticle[0] || !m_pxParticle[1])
			return;

		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor3f(1, 1-m_fStress, 1-m_fStress);
		glVertex2f(m_pxParticle[0]->GetCurrentPos().x, m_pxParticle[0]->GetCurrentPos().y);
		glVertex2f(m_pxParticle[1]->GetCurrentPos().x, m_pxParticle[1]->GetCurrentPos().y);
		glEnd();
	}

	TParticle*	m_pxParticle[2];
	float		m_fRestLength;
	float		m_fMinLength;
	float		m_fRigidity;
	float		m_fStress;
};

struct CBody
{
public:
	CBody(int iNumParticles, int iNumConstraints)
	{
		m_xParticles		= NULL;
		m_xConstraints		= NULL;
		m_iNumParticles		= 0;
		m_iNumConstraints	= 0;
		m_iMaxParticles		= 0;
		m_iMaxConstraints	= 0;

		Allocate(iNumParticles, iNumConstraints);
	}

	virtual ~CBody()
	{
		Free();
	}

	virtual void Update(float dt)
	{
		for(int i = 0; i < m_iNumParticles; i ++)
		{
			m_xParticles[i].Update(dt);
		}
	}

	virtual void SatisfyConstraints()
	{
		int dir = 1;
		for (int iter = 0; iter < 5; iter ++)
		{
			dir = !dir;
			for(int i = 0; i < m_iNumConstraints; i ++)
			{
				if (dir)
					m_xConstraints[i].SatisfyConstraints();
				else
					m_xConstraints[(m_iNumConstraints-1)-i].SatisfyConstraints();
			}
		}
	}

	virtual void Collide(const CTerrain* pxTerrain)
	{
		if (!pxTerrain)
			return;

		pxTerrain->Collide(m_xParticles, m_iNumParticles);
	}

	TParticle* AddParticle(const TParticle& xParticle)
	{
		if (m_iNumParticles >= m_iMaxParticles)
			return NULL;

		m_xParticles[m_iNumParticles++] = xParticle;

		return &m_xParticles[m_iNumParticles-1];
	}

	TConstraint* AddConstraint(const TConstraint& xConstraint)
	{
		if (m_iNumConstraints >= m_iMaxConstraints)
			return NULL;

		m_xConstraints[m_iNumConstraints++] = xConstraint;

		return &m_xConstraints[m_iNumConstraints-1];
	}

	virtual void Render() const
	{
		for(int i = 0; i < m_iNumConstraints; i ++)
		{
			m_xConstraints[i].Render();
		}
//		for(int i = 0; i < m_iNumParticles; i ++)
//		{
//			m_xParticles[i].Render();
//		}
	}

protected:
	void Allocate(int iNumParticles, int iNumConstraints)
	{
		Free();
		m_xParticles		= new TParticle[iNumParticles];
		m_xConstraints		= new TConstraint[iNumConstraints];
		m_iMaxParticles		= iNumParticles;
		m_iMaxConstraints	= iNumConstraints;
	}

	void Free()
	{
		if (m_xParticles)
			delete[] m_xParticles;

		if (m_xConstraints)
			delete[] m_xConstraints;

		m_xParticles		= NULL;
		m_xConstraints		= NULL;
		m_iNumParticles		= 0;
		m_iNumConstraints	= 0;
		m_iMaxParticles		= 0;
		m_iMaxConstraints	= 0;
	}


	TParticle*		m_xParticles;
	TConstraint*	m_xConstraints;
	int				m_iNumParticles;
	int				m_iNumConstraints;
	int				m_iMaxParticles;
	int				m_iMaxConstraints;
};

struct CWheel: public CBody
{
	CWheel(const Vector& xPos, float fRadius, int iNumParticles)
	: CBody(iNumParticles + 1, iNumParticles * 2)
	{
		float a=0.0;

		m_pxAxle = AddParticle(TParticle(xPos, 3));

		for (int i = 0; i < iNumParticles; i++)
		{
			TParticle xParticle(Vector(cos(a)* fRadius + xPos.x, sin(a) * fRadius + xPos.y), 1, 1.0f);
			
			AddParticle(xParticle);

			a+=6.28f / iNumParticles;
		}

		for (i = 0; i < iNumParticles; i++)
		{
			int j = (i + 1) % iNumParticles;

			TParticle* pxP0 = &m_xParticles[i+1];
			TParticle* pxP1 = &m_xParticles[j+1];

			AddConstraint(TConstraint(pxP0, pxP1    , 1.0f));
			AddConstraint(TConstraint(pxP0, m_pxAxle, 1.0f));
		}
	}

	void Rotate(float a)
	{
		for(int i = 0; i < m_iNumParticles; i ++)
		{
			if (&m_xParticles[i] == m_pxAxle)
				continue;

			Vector xNewPos = m_xParticles[i].GetTargetPos().Rotate(m_pxAxle->GetCurrentPos(), a);
			
			m_xParticles[i].SetTargetPos(xNewPos);
		}
	}

	TParticle* GetAxle() { return m_pxAxle; }

private:
	TParticle* m_pxAxle;
};

struct CCar: public CBody
{
	CCar(const Vector& xPos = Vector(460, 50), int iKeyForward=VK_RIGHT, int iKeyBackwards=VK_LEFT)
	: CBody(10, 20)
	, m_iKeyForward(iKeyForward)
	, m_iKeyBackwards(iKeyBackwards)
	, m_pxFrontWheel(NULL)
	, m_pxRearWheel(NULL)
	{
		m_pxFrontWheel = new CWheel(xPos, 20, 10);
		m_pxRearWheel  = new CWheel(xPos + Vector(-90, 0), 20, 10);

		TParticle* pxFrontAxle = m_pxFrontWheel->GetAxle();
		TParticle* pxRearAxle  = m_pxRearWheel ->GetAxle();

		TParticle* pxFrontTop = AddParticle(TParticle(xPos + Vector( -30, -40), 1));
		TParticle* pxRearTop  = AddParticle(TParticle(xPos + Vector( -80, -40), 1));
		
		AddConstraint(TConstraint(pxFrontTop,  pxRearTop,    1.0f)); // top
		AddConstraint(TConstraint(pxFrontAxle, pxRearAxle,	 1.0f)); // chassis
		AddConstraint(TConstraint(pxFrontTop,  pxFrontAxle,  1.0f)); // front vertical strut
		AddConstraint(TConstraint(pxRearTop ,  pxRearAxle ,  0.2f)); // rear  vertical strut
		AddConstraint(TConstraint(pxFrontTop,  pxRearAxle ,  0.2f)); // front cross strut
		AddConstraint(TConstraint(pxRearTop ,  pxFrontAxle,  1.0f)); // rear  cross strut
	}

	Vector GetPos() const 
	{ 
		if (m_pxFrontWheel)
			return m_pxFrontWheel->GetAxle()->GetCurrentPos(); 
	
		if (m_pxRearWheel)
			return m_pxRearWheel->GetAxle()->GetCurrentPos(); 

		return m_xParticles[0].GetCurrentPos();
	}

	virtual ~CCar()
	{
		if (m_pxFrontWheel)
			delete m_pxFrontWheel;
		if (m_pxRearWheel)
			delete m_pxRearWheel;
	}

	virtual void SatisfyConstraints()
	{
		CBody::SatisfyConstraints();
		
		if (m_pxRearWheel)
			m_pxRearWheel ->SatisfyConstraints();
		
		if (m_pxFrontWheel)
			m_pxFrontWheel->SatisfyConstraints();
	}
	
	virtual void Update(float dt)
	{
		CBody::Update(dt);

		if (m_pxRearWheel)
			m_pxRearWheel ->Update(dt);

		if (m_pxFrontWheel)
			m_pxFrontWheel->Update(dt);

		if (KeyPressed(m_iKeyBackwards))
		{
			if (m_pxFrontWheel)
				m_pxFrontWheel->Rotate(-0.010f);
			if (m_pxRearWheel)
				m_pxRearWheel ->Rotate(-0.010f);
		}
		if (KeyPressed(m_iKeyForward))
		{
			if (m_pxFrontWheel)
				m_pxFrontWheel->Rotate(+0.010f);
			
			if (m_pxRearWheel)
				m_pxRearWheel ->Rotate(+0.010f);
		}
	}

	virtual void Collide(const CTerrain* pxTerrain)
	{
		if (!pxTerrain)
			return;

		CBody::Collide(pxTerrain);
		
		if (m_pxRearWheel)
			m_pxRearWheel ->Collide(pxTerrain);

		if (m_pxFrontWheel)
			m_pxFrontWheel->Collide(pxTerrain);
	}

	virtual void Render() const
	{
		CBody::Render();

		if (m_pxRearWheel)
			m_pxRearWheel ->Render();
		
		if (m_pxFrontWheel)
			m_pxFrontWheel->Render();
	}
	float GetSpeed() const { return m_xParticles[0].GetDisplacement().Length(); }
	
private:
	CWheel* m_pxFrontWheel;
	CWheel* m_pxRearWheel;
	int m_iKeyForward;
	int m_iKeyBackwards;

};


enum { eMaxPlayers = 4, eNumPlayers = 3 };


CAABBox	  gxViewports[eMaxPlayers][eMaxPlayers] = { { CAABBox(0.0f, 0.0f, 1.0f, 1.0f) },
													{ CAABBox(0.0f, 0.0f, 0.5f, 1.0f), CAABBox(0.5f, 0.0f, 1.0f, 1.0f) },
													{ CAABBox(0.0f, 0.0f, 0.5f, 0.5f), CAABBox(0.5f, 0.0f, 1.0f, 0.5f), CAABBox(0.0f, 0.5f, 0.5f, 1.0f) },
													{ CAABBox(0.0f, 0.0f, 0.5f, 0.5f), CAABBox(0.5f, 0.0f, 1.0f, 0.5f), CAABBox(0.0f, 0.5f, 0.5f, 1.0f), CAABBox(0.5f, 0.5f, 1.0f, 1.0f) } 
												};

int giKeys[eMaxPlayers][3] = { { 'A', 'D', 'W' }, { 'J', 'L', 'I' }, { VK_LEFT, VK_RIGHT, VK_UP }, { VK_NUMPAD4, VK_NUMPAD6, VK_NUMPAD8 } };

CCamera*		gpxCamera	[eMaxPlayers] = { NULL, NULL };
CCar*			gpxCar		[eMaxPlayers] = { NULL, NULL };
CTerrain*		gpxTerrain=NULL;

int levelrand[] = { 2123, 23112, 4442, 6421 ,43345, 6745, 7685, 54423,12123, 451324, 34242 ,56456, 68267, 7342, 56354 };
int level = 0;


//-------------------------------------------------------------------------------------------------
//
//    OPENGL Functions
//
//-------------------------------------------------------------------------------------------------



void init(bool advanceLevel = true)
{
	if (advanceLevel)
	{
		level = (level + 1) % (sizeof(levelrand) / sizeof(levelrand[0]));
	}
	srand(levelrand[level]);
	char name[48];
	sprintf(name, "level : %d", level);
	glutSetWindowTitle(name);

	for(int i = 0; i < eNumPlayers; i++)
	{
		if (gpxCar[i])
			delete gpxCar[i];

		if (gpxCamera[i])
			delete gpxCamera[i];

		gpxCamera[i] = new CCamera(gxViewports[eNumPlayers-1][i]);

		gpxCar[i] = new CCar(Vector(460, 50), giKeys[i][1], giKeys[i][0]);
	}

	if (gpxTerrain)
		delete gpxTerrain;

	gpxTerrain = new CTerrain(level);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	for(int i = 0; i < eNumPlayers; i++)
	{
		if (!gpxCar[i] || !gpxCamera[i])
			continue;

		float fMinSpeed = 0.2f;
		float fMaxSpeed = 4.0f;
		float fMinZoom = 1.0f;
		float fMaxZoom = 3.0f;
		float fSpeed = gpxCar[i]->GetSpeed();

		float fZoom = gpxCamera[i]->m_fZoom;

		float fRatio = clamp((fSpeed - fMinSpeed) / (fMaxSpeed - fMinSpeed), 0.0f, 1.0f);

		float fNewZoom = fMinZoom + (1.0f - fRatio) * (fMaxZoom - fMinZoom);
		
		fZoom += (fNewZoom - fZoom) * 0.01f;

		gpxCamera[i]->MoveCamera(gpxCar[i]->GetPos(), fZoom);
		gpxCamera[i]->ApplyRenderSettings();

		for(int j = 0; j < eNumPlayers; j ++)
		{
			if (gpxCar[j])
				gpxCar[j]->Render();
	
			if (gpxTerrain)
				gpxTerrain->Render(*gpxCamera[i]);
		}
	}

	glutSwapBuffers();
}

void reshape(int w,int h)
{
	for(int i = 0; i < eNumPlayers; i++)
	{
		if (gpxCamera[i])
			gpxCamera[i]->ReshapeScreen(Vector(w, h));
	}
}

void animate()
{
	//---------------------------------------------------------------------------
	// pink car in case of trouble
	//---------------------------------------------------------------------------
	for(int i = 0; i < eNumPlayers; i++)
	{
		if (!gpxCar[i])
			continue;

		if (DebouncedKeyPressed(giKeys[i][2]))
		{
			Vector xPos = gpxCar[i]->GetPos();

			delete gpxCar[i];
			
			gpxCar[i] = new CCar(xPos + Vector(0.0f, -50.0f), giKeys[i][1], giKeys[i][0]);
		}
	}
	
	//---------------------------------------------------------------------------
	// important. do these step by step
	//---------------------------------------------------------------------------
	for(i = 0; i < eNumPlayers; i++)
	{
		if (gpxCar[i])
			gpxCar[i]->Update(1.0f / 30.0f);
	}

	for(i = 0; i < eNumPlayers; i++)
	{
		if (gpxCar[i])
		gpxCar[i]->SatisfyConstraints();
	}
	
	for(i = 0; i < eNumPlayers; i++)
	{
		if (gpxCar[i])
		gpxCar[i]->Collide(gpxTerrain);
	}
}

void idle()
{
	animate();
	display();
}

void ticker(int i)
{
	idle();
	glutTimerFunc(4, ticker, 0);
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == ' ')
		init();
}
int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(screen_width,screen_height);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
//	glutIdleFunc(idle);
	glutTimerFunc(33, ticker, 0);

	glClearColor(0.0f,0.0f,0.3f,0.1f);
	glPointSize(8);
	glEnable(GL_POINT_SMOOTH);

	glEnable(GL_LINE_SMOOTH);
	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	

	init(false);
	glutMainLoop();
	return 0;

}

