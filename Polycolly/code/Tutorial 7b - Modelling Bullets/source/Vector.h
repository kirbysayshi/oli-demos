#ifndef OLI_VECTOR_H
#define OLI_VECTOR_H


/*
------------------------------------------------------------------
File: Vector.h
Started: 09/01/2004 22:20:40
  
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl/glut.h>

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

#define for if(0); else for

inline float sign(float x)
{
	return (x < 0.0f)? -1.0f : 1.0f;
}

inline float frand(float x=1.0f)
{
	return (rand() / (float) RAND_MAX) * x;
}
inline swapf(float& a, float& b)
{
	float c = a;
	a = b;
	b = c;
}

inline float clampf(float x, float min, float max)
{
	return (x < min)? min : (x > max)? max : x;
}
inline float wrapf(float x, float min, float max)
{
	return (x < min)? (x - min) + max : (x > max)? (x - max) + min : x;

}
inline float Pi()
{
    const float pi = atan(1.0f) * 4.0f;

	return pi;
}

inline float TwoPi()
{
	const float two_pi = 2.0f * Pi();
	return two_pi;
}

inline float RadiansToDegrees(float rad)
{
	const float k = 180.0f / Pi();
	return rad * k;
}

inline float DegreesToRadians(float deg)
{
	const float k = Pi() / 180.0f;
	return deg * k;
}

//===========================================================================
// VECTORS
//===========================================================================
class Matrix;

class Vector
{
public:
	float x,y;

	static const Vector& Blank() { static const Vector V(0, 0); return V; }
public:
	inline Vector(void)
	{}

	inline Vector(float Ix,float Iy)
	: x(Ix)
	, y(Iy)
	{}

	inline Vector &operator /=(const float Scalar)	{ x /= Scalar; y /= Scalar;		return *this; }

	inline Vector &operator *=(const float Scalar)	{ x *= Scalar; y *= Scalar;		return *this; }
	
	inline Vector &operator +=(const Vector &Other) { x += Other.x;	y += Other.y;	return *this; }

	inline Vector &operator -=(const Vector &Other) { x -= Other.x;	y -= Other.y;	return *this;	}

	inline float operator ^ (const Vector &V)	const	{	return (x * V.y) - (y * V.x); } // cross product

	inline float operator * (const Vector &V)	const	{	return (x*V.x) + (y*V.y); } // dot product

	inline Vector operator * (float  s)			const	{	return Vector(x*s, y*s); }
	
	inline Vector operator / (float  s)			const	{	return Vector(x/s, y/s); }
	
	inline Vector operator + (const Vector &V)	const	{	return Vector(x+V.x, y+V.y); }
		
	inline Vector operator - (const Vector &V)	const	{	return Vector(x-V.x, y-V.y); }

	friend Vector operator * (float k, const Vector& V) {	return Vector(V.x*k, V.y*k); }

	Vector operator * (const Matrix& M) const;
	
	Vector operator ^ (const Matrix& M) const;
	
	Vector& operator *=(const Matrix& M);
	
	Vector& operator ^=(const Matrix& M);

	inline Vector operator -(void) const { return Vector(-x, -y); }
	
	inline float Length(void) const { return (float) sqrt(x*x + y*y); }

	float Normalise(void) 
	{	
		float fLength = Length();	
		
		if (fLength == 0.0f) 
			return 0.0f; 
		
		(*this) *= (1.0f / fLength); 
	
		return fLength;	
	}

	Vector Direction(void) const
	{
		Vector Temp(*this);

		Temp.Normalise();

		return Temp;
	}

	float Angle(const Vector& xE)
	{
		float dot = (*this) * xE;
		float cross = (*this) ^ xE;
		
		// angle between segments
		float angle = (float) atan2(cross, dot);

		return angle;
	}

	Vector& Rotate(float angle)
	{
		float tx = x;
		x =  x * cos(angle) - y * sin(angle);
		y = tx * sin(angle) + y * cos(angle);

		return *this;
	}

	Vector& Rotate(const Vector& xCentre, float fAngle)
	{
		Vector D = *this - xCentre;
		D.Rotate(fAngle);

		*this = xCentre + D;

		return *this;
	}

	void Clamp(const Vector& min, const Vector& max)
	{
		x = (x < min.x)? min.x : (x > max.x)? max.x : x;
		y = (y < min.y)? min.y : (y > max.y)? max.y : y;
	}

	void Randomise(const Vector& xMin, const Vector& xMax)
	{
		x = frand(xMax.x - xMin.x) + xMin.x;
		y = frand(xMax.y - xMin.y) + xMin.y;
	}

	void Render(void) const
	{
		glColor3f(1.0f, 1.0f, 0.1f);
		glPointSize(3.0f);
		glBegin(GL_POINTS);
		glVertex2f(x, y);
		glEnd();
	}
};

#define ARGB_A(u) (((u)>>24) & 0x000000FF)
#define ARGB_R(u) (((u)>>16) & 0x000000FF)
#define ARGB_G(u) (((u)>> 8) & 0x000000FF)
#define ARGB_B(u) (((u)>> 0) & 0x000000FF)

extern void RenderSegment		 (const Vector& A, const Vector& B, u_int uARGB, u_short uStipple, float fPointSize, float fLineWidth);
extern void RenderArrow			 (const Vector& P, const Vector& D, float length, u_int uARGB);
extern void	RenderSolidCircle	 (const Vector& xPos, float fRad);
extern void	RenderCircle		 (const Vector& xPos, float fRad);
extern bool FindRoots			 (float a, float b, float c, float& t0, float& t1);
extern bool RaySphereIntersection(const Vector& C, float r, const Vector& O, const Vector& D, float tmin, float tmax, float& t);



#endif OLI_VECTOR_H