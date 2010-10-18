
#include "Vector.h"
#include "Matrix.h"
#include "Polygon.h"

// taken from 
// http://www.physicsforums.com/showthread.php?s=e251fddad79b926d003e2d4154799c14&t=25293&page=2&pp=15
float PolyColl::CalculateMass(const Vector* A, int Anum, float density)
{
	if (Anum < 2)
		return 5.0f * density;

	float mass = 0.0f;

	for(int j = Anum-1, i = 0; i < Anum; j = i, i ++)
	{
		Vector P0 = A[j];
		Vector P1 = A[i];
		mass +=  (float) fabs(P0 ^ P1);
	}
	if (Anum <= 2)
		mass = 10.0f;

	mass *= density * 0.5f;

	return mass;
}

// taken from 
// http://www.physicsforums.com/showthread.php?s=e251fddad79b926d003e2d4154799c14&t=25293&page=2&pp=15
float PolyColl::CalculateInertia(const Vector* A, int Anum, float mass)
{
	if (Anum == 1)	return 0.0f;

	float denom = 0.0f;
	float numer = 0.0f;

	for(int j = Anum-1, i = 0; i < Anum; j = i, i ++)
	{
		Vector P0 = A[j];
		Vector P1 = A[i];

		float a = (float) fabs(P0 ^ P1);
		float b = (P1*P1 + P1*P0 + P0*P0);

		denom += (a * b);
		numer += a;
	}
	float inertia = (mass / 6.0f) * (denom / numer);

	return inertia;
}


Vector* PolyColl::BuildBox(int& iNumVertices, float width, float height)
{
	Vector* axVertices = new Vector[4];
	
	axVertices[0] = Vector(-width/2, -height/2);
	axVertices[1] = Vector( width/2, -height/2);
	axVertices[2] = Vector( width/2,  height/2);
	axVertices[3] = Vector(-width/2,  height/2);
	iNumVertices = 4;
	return axVertices;
}

Vector* PolyColl::BuildBlob(int iNumVertices, float radiusx, float radiusy)
{
	if (iNumVertices == 0)
		return NULL;
	
	Vector* axVertices = new Vector[iNumVertices];

	if (iNumVertices == 1)
	{
		axVertices[0] = Vector(0, 0);
		return axVertices;
	}

	float a = Pi() / iNumVertices;
	float da = 2.0f * Pi() / iNumVertices;

	for(int i = 0; i < iNumVertices; i ++)
	{
		a += da;

		axVertices[i] = Vector(cos(a) * radiusx, sin(a) * radiusy);
	}
	return axVertices;
}
void GetInterval(const Vector *axVertices, int iNumVertices, 
				 const Vector& xAxis, float& min, float& max);

bool FindMTD(Vector* xAxis, float* taxis, int iNumAxes, Vector& N, float& t);

bool IntervalIntersect(const Vector* A, int Anum,
					   const Vector* B, int Bnum,
					   const Vector& xAxis, 
					   const Vector& xOffset, const Vector& xVel, const Matrix& xOrient,
					   float& taxis, float tmax);

bool PolyColl::Collide(const Vector* A, int Anum, const Vector& PA, const Vector& VA, const Matrix& OA,
					   const Vector* B, int Bnum, const Vector& PB, const Vector& VB, const Matrix& OB,
					   Vector& N, float& t)
{
	if (!A || !B) return false;

	if (Anum < 2 && Bnum < 2)
		return false;

	Matrix xOrient = OA ^ OB;
	Vector xOffset = (PA - PB) ^ OB;
	Vector xVel    = (VA - VB) ^ OB;
	
	// All the separation axes
	Vector xAxis[64]; // note : a maximum of 32 vertices per poly is supported
	float  taxis[64];
	int    iNumAxes=0;

	float fVel2 = xVel * xVel;

	if (fVel2 > 0.00001f)
	{
		xAxis[iNumAxes] = Vector(-xVel.y, xVel.x);

		if (!IntervalIntersect(	A, Anum, 
								B, Bnum, 
								xAxis[iNumAxes], 
								xOffset, xVel, xOrient,
								taxis[iNumAxes], t))
		{
			return false;
		}
		iNumAxes++;
	}

	if (Anum > 1)
	{
		// test separation axes of A
		for(int j = Anum-1, i = 0; i < Anum; j = i, i ++)
		{
			Vector E0 = A[j];
			Vector E1 = A[i];
			Vector E  = E1 - E0;
			xAxis[iNumAxes] = Vector(-E.y, E.x) * xOrient;
			
			if (!IntervalIntersect(	A, Anum, 
									B, Bnum, 
									xAxis[iNumAxes], 
									xOffset, xVel, xOrient,
									taxis[iNumAxes], t))
			{
				return false;
			}

			iNumAxes++;
		}
	}

	// test separation axes of B
	if (Bnum > 1)
	{
		for(int j = Bnum-1, i = 0; i < Bnum; j = i, i ++)
		{
			Vector E0 = B[j];
			Vector E1 = B[i];
			Vector E  = E1 - E0;
			xAxis[iNumAxes] = Vector(-E.y, E.x);

			if (!IntervalIntersect(	A, Anum, 
									B, Bnum, 
									xAxis[iNumAxes], 
									xOffset, xVel, xOrient,
									taxis[iNumAxes], t))
			{
				return false;
			}

			iNumAxes++;
		}
	}

	// special case for segments
	if (Bnum == 2)
	{
		Vector E  = B[1] - B[0];
		xAxis[iNumAxes] = E;
		
		if (!IntervalIntersect(	A, Anum, 
								B, Bnum, 
								xAxis[iNumAxes], 
								xOffset, xVel, xOrient,
								taxis[iNumAxes], t))
		{
			return false;
		}

		iNumAxes++;
	}

	// special case for segments
	if (Anum == 2)
	{
		Vector E  = A[1] - A[0];
		xAxis[iNumAxes] = E * xOrient;
		
		if (!IntervalIntersect(	A, Anum, 
								B, Bnum, 
								xAxis[iNumAxes], 
								xOffset, xVel, xOrient,
								taxis[iNumAxes], t))
		{
			return false;
		}

		iNumAxes++;
	}

	if (!FindMTD(xAxis, taxis, iNumAxes, N, t))
		return false;

	// make sure the polygons gets pushed away from each other.
	if (N * xOffset < 0.0f)
		N = -N;

	N *= OB;

	return true;
}

// calculate the projection range of a polygon along an axis
void GetInterval(const Vector *axVertices, int iNumVertices, const Vector& xAxis, float& min, float& max)
{
	min = max = (axVertices[0] * xAxis);

	for(int i = 1; i < iNumVertices; i ++)
	{
		float d = (axVertices[i] * xAxis);
		if (d < min) min = d; else if (d > max) max = d;
	}
}

bool IntervalIntersect(const Vector* A, int Anum,
					   const Vector* B, int Bnum,
					   const Vector& xAxis, 
					   const Vector& xOffset, const Vector& xVel, const Matrix& xOrient,
					   float& taxis, float tmax)
{
	float min0, max0;
	float min1, max1;
	GetInterval(A, Anum, xAxis ^ xOrient, min0, max0);
	GetInterval(B, Bnum, xAxis, min1, max1);

	float h = xOffset * xAxis;
	min0 += h;
	max0 += h;

	float d0 = min0 - max1; // if overlapped, do < 0
	float d1 = min1 - max0; // if overlapped, d1 > 0

	// separated, test dynamic intervals
	if (d0 > 0.0f || d1 > 0.0f) 
	{
		float v = xVel * xAxis;

		// small velocity, so only the overlap test will be relevant. 
		if (fabs(v) < 0.0000001f)
			return false;

		float t0 =-d0 / v; // time of impact to d0 reaches 0
		float t1 = d1 / v; // time of impact to d0 reaches 1

		if (t0 > t1) { float temp = t0; t0 = t1; t1 = temp; }
		taxis  = (t0 > 0.0f)? t0 : t1;

		if (taxis < 0.0f || taxis > tmax)
			return false;

		return true;
	}
	else
	{
		// overlap. get the interval, as a the smallest of |d0| and |d1|
		// return negative number to mark it as an overlap
		taxis = (d0 > d1)? d0 : d1;
		return true;
	}
}
bool FindMTD(Vector* xAxis, float* taxis, int iNumAxes, Vector& N, float& t)
{
	// find collision first
	int mini = -1;
	t = 0.0f;
	for(int i = 0; i < iNumAxes; i ++)
	{	
		if (taxis[i] > 0)
		{
			if (taxis[i] > t)
			{
				mini = i;
				t = taxis[i];
				N = xAxis[i];
				N.Normalise();
			}
		}
	}

	// found one
	if (mini != -1)
		return true; 

	// nope, find overlaps
	mini = -1;
	for(int i = 0; i < iNumAxes; i ++)
	{
		float n = xAxis[i].Normalise();
		taxis[i] /= n;

		if (taxis[i] > t || mini == -1)
		{
			mini = i;
			t = taxis[i];
			N = xAxis[i];
		}
	}

	if (mini == -1)
		printf("Error\n");

	return (mini != -1);
}


//----------------------------------------------------------------------------------------------- 
// Find closest point on a segment to a vertex
//----------------------------------------------------------------------------------------------- 
bool ProjectPointOnSegment(const Vector& V, const Vector& A, const Vector& B, Vector& W, float* pt=NULL)
{
	Vector AV = V - A;
	Vector AB = B - A;
	float t = (AV * AB) / (AB * AB);

	if (t < 0.0f) t = 0.0f; else if (t > 1.0f) t = 1.0f;

	if (pt)	*pt = t;

	W = A + t * AB;

	return true;
}

//----------------------------------------------------------------------------------------------- 
// Tranform a contact point into world collision space
//----------------------------------------------------------------------------------------------- 
static Vector Transform(const Vector& Vertex, const Vector& P, const Vector& V, const Matrix& xOrient, float t)
{
	Vector T = P + (Vertex * xOrient);

	if(t > 0.0f)
		T += V * t;

	return T;
}

//----------------------------------------------------------------------------------------------- 
// find the support points of a convex shape along a direction
//----------------------------------------------------------------------------------------------- 
int FindSupportPoints(const Vector& N, float t, const Vector* A, int Anum, const Vector& PA, const Vector& VA, const Matrix& OA, Vector* S)
{
	Vector Norm = N ^ OA;

	float d[32];
	float dmin;
	dmin = d[0] = A[0] * Norm;
	
	for(int i = 1; i < Anum; i ++)
	{
		d[i] = A[i] * Norm;

		if (d[i] < dmin)
		{
			dmin = d[i];
		}
	}

	// we will limit the number of support points to only 2. 
	// if we have more than 2 support points, we take the extremums.
	int Snum = 0;
	const float threshold = 1.0E-3f;
	float s[2];
	float sign = false;

	Vector Perp(-Norm.y, Norm.x);

	for(int i = 0; i < Anum; i ++)
	{
		if (d[i] < dmin + threshold)
		{
			Vector Contact = Transform(A[i], PA, VA, OA, t);

			float c = Contact * Perp;

			if (Snum < 2)
			{
				s[Snum] = c;
				S[Snum] = Contact;
				Snum++;

				if (Snum > 1)
				{
					sign = (s[1] > s[0]);
				}
			}
			else
			{
				float&  min = (sign)? s[0] : s[1];
				float&  max = (sign)? s[1] : s[0];
				Vector& Min = (sign)? S[0] : S[1];
				Vector& Max = (sign)? S[1] : S[0];

				if (c < min)
				{
					min = c;
					Min = Contact;
				}
				else if (c > max)
				{
					max = c;
					Max = Contact;
				}
			}
		}
	}
	return Snum;
}

// convert sets of support points to contact points.
bool ConvertSupportPointsToContacts(const Vector& N,
									Vector* S0, int S0num, 
									Vector* S1, int S1num,
									Vector* C0,
									Vector* C1,
									int& Cnum)
{
	Cnum = 0;

	if (S0num == 0 || S1num == 0)
		return false;

	if (S0num == 1 && S1num == 1)
	{
		C0[Cnum] = S0[0];
		C1[Cnum] = S1[0];
		Cnum++;
		return true;
	}

	Vector xPerp(-N.y, N.x);

	float min0 = S0[0] * xPerp;
	float max0 = min0;
	float min1 =  S1[0] * xPerp;
	float max1 = min1;

	if (S0num == 2)
	{
		max0 = S0[1] * xPerp;

		if (max0 < min0) 
		{ 
			swapf(min0, max0);

			Vector T = S0[0];
			S0[0] = S0[1];
			S0[1] = T;
		}
	}

	if (S1num == 2)
	{
		max1 = S1[1] * xPerp;

		if (max1 < min1) 
		{ 
			swapf(min1, max1);

			Vector T = S1[0];
			S1[0] = S1[1];
			S1[1] = T;
		}
	}

	if (min0 > max1 || min1 > max0)
		return false;

	if (min0 > min1)
	{
		Vector Pseg;
		if (ProjectPointOnSegment(S0[0], S1[0], S1[1], Pseg))
		{
			C0[Cnum] = S0[0];
			C1[Cnum] = Pseg;
			Cnum++;
		}
	}
	else
	{
		Vector Pseg;
		if (ProjectPointOnSegment(S1[0], S0[0], S0[1], Pseg))
		{
			C0[Cnum] = Pseg;
			C1[Cnum] = S1[0];
			Cnum++;
		}
	}

	if (max0 != min0 && max1 != min1)
	{
		if (max0 < max1)
		{
			Vector Pseg;
			if (ProjectPointOnSegment(S0[1], S1[0], S1[1], Pseg))
			{
				C0[Cnum] = S0[1];
				C1[Cnum] = Pseg;
				Cnum++;
			}
		}
		else
		{
			Vector Pseg;
			if (ProjectPointOnSegment(S1[1], S0[0], S0[1], Pseg))
			{
				C0[Cnum] = Pseg;
				C1[Cnum] = S1[1];
				Cnum++;
			}
		}
	}
	return true;
}

bool PolyColl::FindContacts(const Vector* A, int Anum, const Vector& PA, const Vector& VA, const Matrix& OA,
							const Vector* B, int Bnum, const Vector& PB, const Vector& VB, const Matrix& OB,
							const Vector& N, float t,
							Vector* CA, 
							Vector* CB, 
							int& Cnum)
{
	Vector S0[4];
	Vector S1[4];
	int S0num = FindSupportPoints( N, t, A, Anum, PA, VA, OA, S0);
	int S1num = FindSupportPoints(-N, t, B, Bnum, PB, VB, OB, S1);

	if (!ConvertSupportPointsToContacts(N, S0, S0num, S1, S1num, CA, CB, Cnum))
		return false;

	return true;
}

// clip a ray to a polygon
bool PolyColl::ClipSegment(const Vector* A, int Anum, const Vector& PA, const Vector& VA, const Matrix& OA,
						   const Vector& xStart, const Vector& xEnd, 
						   float& tnear, float& tfar, Vector& Nnear, Vector& Nfar)
{
	if (!A) return false;

	Vector xPos = xStart - PA;
	Vector xDir = xEnd - xStart;
	xDir ^= OA;
	xPos ^= OA;
	
	// test separation axes of A
	for(int j = Anum-1, i = 0; i < Anum; j = i, i ++)
	{
		Vector E0 = A[j];
		Vector E1 = A[i];
		Vector E  = E1 - E0;
		Vector En(E.y, -E.x);
		Vector D = E0 - xPos;

		float denom = D * En;
		float numer = xDir * En;

		// ray parallel to plane
		if (fabs(numer) < 1.0E-8f)
		{
			// origin outside the plane, no intersection
			if (denom < 0.0f)
				return false;
		}
		else
		{
			float tclip = denom / numer;

			// near intersection
			if (numer < 0.0f)
			{
				if (tclip > tfar)
					return false;

				if (tclip > tnear)
				{
					tnear = tclip;
					Nnear = En;
					Nnear.Normalise();
					Nnear *= OA;
				}
			}
			// far intersection
			else
			{
				if (tclip < tnear)
					return false;

				if (tclip < tfar)
				{
					tfar = tclip;
					Nfar = En;
					Nfar.Normalise();
					Nfar *= OA;
				}
			}
		}
	}

	return true;
}




#include "Perlin.h"

GLuint BindTexture()
{
	enum { width = 128, height = 128 };
	static unsigned char rgba[height][width][4];
	static GLuint texture = -1;
	
	if(!glIsTexture(texture))
	{
		//----------------------------------------------------------------------------------------------- 
		// OR [11/02/2004 ] : compute point positions
		//----------------------------------------------------------------------------------------------- 
		for(int j = 0; j < height; j ++)
		{
			for(int i = 0; i < width; i ++)
			{
				int r = (int)(CPerlin::PerlinNoise_2D(j, i) * 128.0f + 128.0f);
				int g = (int)(CPerlin::PerlinNoise_2D(j + width, i) * 128.0f + 128.0f);
				int b = (int)(CPerlin::PerlinNoise_2D(j + width, i + height) * 128.0f + 128.0f);
				int a = (int)(CPerlin::PerlinNoise_2D(j + width * 2, i) * 128.0f + 128.0f);

				rgba[j][i][0] = clampf(r, 0, 255);
				rgba[j][i][1] = clampf(g, 0, 255);
				rgba[j][i][2] = clampf(b, 0, 255);
				rgba[j][i][3] = clampf(a, 0, 255);
			}
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float col[] = { 0.3f, 0.3f, 0.3f, 0.3f };
		glTexEnvi	   (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_LUMINANCE);
		glTexEnvfv	   (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, col);
	}

	return texture;
}

void PolyColl::Render(const Vector& xOffset, float angle, u_int ARGBfill, u_int ARGBline, const Vector* axVertices, int iNumVertices)
{
	if (!axVertices) return;

	if (iNumVertices == 1)
	{
		glColor4ub(ARGB_R(ARGBfill), ARGB_G(ARGBfill), ARGB_B(ARGBfill), ARGB_A(ARGBfill));
		glEnable(GL_POINT_SIZE);
		glPointSize(3.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glTranslatef(xOffset.x, xOffset.y, 0.0f);
			glBegin(GL_POINTS);
				glVertex2f(axVertices[0].x, axVertices[0].y);
			glEnd();
		glPopMatrix();
		glDisable(GL_POINT_SIZE);
		return;
	}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	float scale = 0.06f;
	glScalef(scale, scale, scale);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, BindTexture());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(xOffset.x, xOffset.y, 0.0f);

	glRotatef(RadiansToDegrees(angle), 0, 0, -1);

	glColor4ub(ARGB_R(ARGBfill), ARGB_G(ARGBfill), ARGB_B(ARGBfill), ARGB_A(ARGBfill));

	glBegin(GL_TRIANGLE_FAN);

	for(int i = 0; i < iNumVertices; i ++)
	{
		glTexCoord2f(axVertices[i].x, axVertices[i].y);
		glVertex2f(axVertices[i].x, axVertices[i].y);
	}
	glTexCoord2f(axVertices[0].x, axVertices[0].y);
	glVertex2f(axVertices[0].x, axVertices[0].y);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	if (ARGBline != 0)
	{
		glColor4ub(ARGB_R(ARGBline), ARGB_G(ARGBline), ARGB_B(ARGBline), ARGB_A(ARGBline));
		
		glBegin(GL_LINE_LOOP);

		for(int i = 0; i < iNumVertices; i ++)
		{
			glVertex2f(axVertices[i].x, axVertices[i].y);
		}
		glVertex2f(axVertices[0].x, axVertices[0].y);
		glEnd();
	}
	glPopMatrix();
}	

