/*
------------------------------------------------------------------
File: Vector.cpp
Started: 19/01/2004 21:43:13
  
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

bool FindRoots(float a, float b, float c, float& t0, float& t1)
{
	float d = b*b - (4.0f * a * c);

	if (d < 0.0f)
		return false;

	d = (float) sqrt(d);

	float one_over_two_a = 1.0f / (2.0f * a);

	t0 = (-b - d) * one_over_two_a;
	t1 = (-b + d) * one_over_two_a;

	if (t1 < t0)
	{
		float t = t0;
		t0 = t1;
		t1 = t;
	}
	return true;
}


bool RaySphereIntersection(const Vector& C, float r, const Vector& O, const Vector& D, float tmin, float tmax, float& t)
{
	float t0, t1;

	Vector H = (O - C);
	float  a = (D * D);
	float  b = (H * D) * 2.0f;
	float  c = (H * H) - r*r;

	if (!FindRoots(a, b, c, t0, t1))
		return false;

	if (t0 > tmax || t1 < tmin)
		return false;

	t = t0;

	if (t0 < tmin)
		t = t1;

	return true;
}


//--------------------------------------------------------
// Tools
//--------------------------------------------------------
void RenderSolidCircle(const Vector& xPos, float fRad)
{
	static int glList = -1;

	if (!glIsList(glList))
	{
		glList = glGenLists(1);

		glNewList(glList, GL_COMPILE_AND_EXECUTE);

		glBegin(GL_TRIANGLE_FAN);
		
		glVertex2f(0.0f, 0.0f);
		
		for(int i = 0; i < 16 + 2; i ++)
		{
			Vector P(cos(Pi() * (i / 8.0f)), sin(Pi() * (i / 8.0f)));

			glVertex2f(P.x, P.y);
		}

		glEnd();

		glEndList();
	}

	glPushMatrix();
	glTranslatef(xPos.x, xPos.y, 0.0f);
	glScalef(fRad, fRad, fRad);
	glCallList(glList);
	glPopMatrix();
}


void RenderCircle(const Vector& xPos, float fRad)
{
	static int glList = -1;

	if (!glIsList(glList))
	{
		glList = glGenLists(1);

		glNewList(glList, GL_COMPILE_AND_EXECUTE);

		glBegin(GL_LINE_LOOP);

		for(int i = 0; i < 16 + 1; i ++)
		{
			Vector P(cos(Pi() * (i / 8.0f)), sin(Pi() * (i / 8.0f)));

			glVertex2f(P.x, P.y);
		}

		glEnd();

		glEndList();
	}

	glPushMatrix();
	glTranslatef(xPos.x, xPos.y, 0.0f);
	glScalef(fRad, fRad, fRad);
	glCallList(glList);
	glPopMatrix();
}

void RenderArrow(const Vector& P, const Vector& D, float length, u_int uARGB)
{
	float fAngle = atan2(D.y, D.x);

	glPushMatrix();
	glTranslatef(P.x, P.y, 0.0f);
	glRotatef(RadiansToDegrees(fAngle), 0.0f, 0.0f, 1.0f);
	glScalef(length, length, 1.0f);

	glColor4ub(ARGB_R(uARGB), ARGB_G(uARGB), ARGB_B(uARGB), ARGB_A(uARGB));

	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f, 0.2f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f,-0.2f);
	glEnd();

	glPopMatrix();
}

void RenderSegment(const Vector& A, const Vector& B, u_int uARGB, u_short uStipple, float fPointSize, float fLineWidth)
{
	if (fPointSize > 0.0f)
	{
		glEnable(GL_POINT_SIZE);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(fPointSize);
	}

	if (fLineWidth > 0.0f)
	{
		glEnable(GL_LINE_WIDTH);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(fLineWidth);
	}

	if (uStipple > 0)
	{
		glEnable(GL_LINE_STIPPLE);
		glEnable(GL_LINE_STIPPLE_REPEAT);
		glLineStipple(1, uStipple);
	}
	

	glColor4ub(ARGB_R(uARGB), ARGB_G(uARGB), ARGB_B(uARGB), ARGB_A(uARGB));
	glBegin(GL_POINTS);
		glVertex2f(A.x, A.y);
		glVertex2f(B.x, B.y);
	glEnd();
	
	glLineWidth(2.0f);
	glBegin(GL_LINES);
		glVertex2f(A.x, A.y);
		glVertex2f(B.x, B.y);
	glEnd();
	
	glDisable(GL_LINE_STIPPLE);
	glDisable(GL_LINE_WIDTH);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SIZE);
	glDisable(GL_POINT_SMOOTH);

}

void RenderPlane(const Vector& xNormal, const Vector& xOrigin, u_int uARGB)
{
 	 extern float dbg_world_size;
 	 
	 Vector P[2];
	 Vector N = xNormal;
	 N.Normalise();
	 Vector D(-N.y, N.x);

	 P[0] = xOrigin + D * dbg_world_size * 2.0f;
 	 P[1] = xOrigin - D * dbg_world_size * 2.0f;
 	 
 	 RenderSegment(P[0], P[1], uARGB, 0x33333333, 0.0f, 1.0f);
 	 RenderArrow(xOrigin, N, dbg_world_size * 0.075f, uARGB);
}
