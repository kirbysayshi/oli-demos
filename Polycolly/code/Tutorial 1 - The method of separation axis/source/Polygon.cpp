
#include "Vector.h"
#include "Polygon.h"

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

Vector* PolyColl::BuildBlob(int iNumVertices, float radius)
{
	Vector* axVertices = new Vector[iNumVertices];

	for(int i = 0; i < iNumVertices; i ++)
	{
		float a = 2.0f * Pi() * (i / (float) iNumVertices);

		axVertices[i] = Vector(cos(a), sin(a)) * radius;
	}
	return axVertices;
}

void PolyColl::Render(const Vector& xOffset, u_int ARGBfill, u_int ARGBline, const Vector* axVertices, int iNumVertices)
{
#define ARGB_A(u) (((u)>>24) & 0x000000FF)
#define ARGB_R(u) (((u)>>16) & 0x000000FF)
#define ARGB_G(u) (((u)>> 8) & 0x000000FF)
#define ARGB_B(u) (((u)>> 0) & 0x000000FF)

	if (!axVertices) return;

	glPushMatrix();
	glTranslatef(xOffset.x, xOffset.y, 0.0f);

	glColor4ub(ARGB_R(ARGBfill), ARGB_G(ARGBfill), ARGB_B(ARGBfill), ARGB_A(ARGBfill));

	glBegin(GL_TRIANGLE_FAN);

	for(int i = 0; i < iNumVertices; i ++)
	{
		glVertex2f(axVertices[i].x, axVertices[i].y);
	}
	glVertex2f(axVertices[0].x, axVertices[0].y);
	glEnd();

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


void GetInterval(const Vector *axVertices, int iNumVertices, 
				 const Vector& xAxis, float& min, float& max);

bool IntervalIntersect(const Vector* A, int Anum,
					   const Vector* B, int Bnum,
					   const Vector& xAxis, 
					   const Vector& xOffset);

bool PolyColl::Collide(const Vector* A, int Anum, 
					   const Vector* B, int Bnum, 
					   const Vector& xOffset)
{
	if (!A || !B) return false;

	// All the separation axes
	Vector xAxis[64]; // note : a maximum of 32 vertices per poly is supported
	int    iNumAxes=0;

	// test separation axes of A
	for(int j = Anum-1, i = 0; i < Anum; j = i, i ++)
	{
		Vector E0 = A[j];
		Vector E1 = A[i];
		Vector E  = E1 - E0;
		xAxis[iNumAxes] = Vector(-E.y, E.x);

		if (!IntervalIntersect(	A, Anum, 
								B, Bnum, 
								xAxis[iNumAxes], 
								xOffset))
		{
			return false;
		}

		iNumAxes++;
	}

	// test separation axes of B
	for(int j = Bnum-1, i = 0; i < Bnum; j = i, i ++)
	{
		Vector E0 = B[j];
		Vector E1 = B[i];
		Vector E  = E1 - E0;
		xAxis[iNumAxes] = Vector(-E.y, E.x);

		if (!IntervalIntersect(	A, Anum, 
								B, Bnum, 
								xAxis[iNumAxes], 
								xOffset))
		{
			return false;
		}

		iNumAxes++;
	}

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
					   const Vector& xOffset)
{
	float min0, max0;
	float min1, max1;
	GetInterval(A, Anum, xAxis, min0, max0);
	GetInterval(B, Bnum, xAxis, min1, max1);

	float h = xOffset * xAxis;
	min0 += h;
	max0 += h;

	float d0 = min0 - max1; 
	float d1 = min1 - max0; 

	if (d0 > 0.0f || d1 > 0.0f) 
	{
		return false;
	}
	else
	{
		return true;
	}
}
