#ifndef OLI_VECTOR_H
#define OLI_VECTOR_H

/*
#include "Vector.h"
*/

/*
------------------------------------------------------------------
File: Vector.h
Started: 22/11/2003 22:39:05
  
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

#include <Math.h>
#include <stdio.h>
#include <gl/glut.h>

#define for if(0); else for

//------------------------------------------------------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// OR [11/02/2004 ] : some useful functions
// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
//------------------------------------------------------------------------------------------------ 

//--------------------------------------------------------------------------
// random float
//--------------------------------------------------------------------------
inline float frand(float range)
{
	return range * (rand() / (float)RAND_MAX);
}

//--------------------------------------------------------------------------
// openGL tool function
//--------------------------------------------------------------------------
inline void SetMaterial(int ColorIndex, float transparency)
{
	float t = transparency;
	float a = 0.5f, b = 1.0f;

	static GLfloat Color[16][4] = {	{ 0, 0, 0, t }, { a, 0, 0, t }, { 0, a, 0, t }, { 0, 0, a, t }, 
									{ a, a, 0, t }, { a, 0, a, t }, { 0, a, a, t }, { a, a, a, t }, 
		 							{ a, a, a, 1 }, { b, 0, 0, 1 }, { 0, b, 0, 1 }, { 0, 0, b, 1 }, 
									{ b, b, 0, 1 }, { b, 0, b, 1 }, { 0, b, b, 1 }, { b, b, b, 1 } };
							
	GLfloat Black[4] = { 0, 0, 0, 1 };
	GLfloat White[4] = { 1, 1, 1, 1 };

	if (transparency != 1.0f)
	{
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glColor4f(Color[ColorIndex][0], Color[ColorIndex][1], Color[ColorIndex][2], transparency);
	}
	else
	{
		glEnable(GL_LIGHTING);
		int AmbientIndex = ColorIndex % 8;
		int DiffuseIndex = (AmbientIndex + 3) % 16;
		int SepcularIndex= (AmbientIndex + 5) % 16;
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , Color[AmbientIndex]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , Color[DiffuseIndex]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , Black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , Color[SepcularIndex]);
		glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
	}
}


//===========================================================================
// VECTORS
//===========================================================================
class Vector
{
public:
	float x,y,z;

public:
	inline Vector(void)
	{}

	inline Vector(float Ix,float Iy,float Iz): x(Ix), y(Iy), z(Iz) {}

	inline Vector &operator /=(const float Scalar) { *this *= (1.0f / Scalar);	return *this; }

	inline Vector &operator *=(const float Scalar) { x *= Scalar; y *= Scalar; z *= Scalar;	return *this; }
	
	inline Vector &operator +=(const Vector &Other) { x += Other.x;	y += Other.y;	z += Other.z; return *this; }

	inline Vector &operator -=(const Vector &Other) { x -= Other.x;	y -= Other.y;	z -= Other.z; return *this;	}

	inline Vector& operator ^=(const Vector &V) // Cross product
	{
		float Tempx	= (y * V.z) - (z * V.y);
		float Tempy	= (z * V.x) - (x * V.z);
		      z		= (x * V.y) - (y * V.x);
		      x     = Tempx;
		      y     = Tempy;

		return *this;
	}
	
	inline Vector operator ^ (const Vector& V) const {	Vector Temp(*this); return Temp ^= V; }

	inline Vector operator * (float  s)		   const {	Vector Temp(*this); return Temp *= s; };
	
	inline Vector operator / (float  s)		   const {	Vector Temp(*this); return Temp /= s; }
	
	inline Vector operator + (const Vector &V) const {	Vector Temp(*this); return Temp += V; }
		
	inline Vector operator - (const Vector &V) const {	Vector Temp(*this); return Temp -= V; }

	inline bool	  operator ==(const Vector &V) const { return !((*this) != V); }


	friend Vector operator * (float k, const Vector& V) { return V * k; } // dot product

	inline float operator * (const Vector &V) const { return (x * V.x) + (y * V.y) + (z * V.z); }

	inline Vector operator -(void) const { return Vector(-x, -y, -z); }

	inline bool operator != (const Vector &Other) const
	{
		const float	Epsilon = 1.0E-6f;

		if ((fabs(Other.x - x) > Epsilon) ||
			(fabs(Other.y - y) > Epsilon) ||
			(fabs(Other.z - z) > Epsilon))
		{
			return true;
		}

		return false;
	}

	
	inline float GetLength(void) const { return (float) sqrt((*this) * (*this)); }

	float Normalise() 
	{	
		float Length = GetLength();	
		
		if (Length == 0.0f) 
			return 0.0f; 
		
		(*this) *= (1.0f / Length); 
	
		return Length;	
	}

	static Vector Random(const Vector& Radius=Vector(1.0f, 1.0f, 1.0f))
	{
		return Vector(frand(Radius.x), frand(Radius.y), frand(Radius.z));
	}

	static Vector Random(float radius=1.0f)
	{
		return Vector(frand(radius), frand(radius), frand(radius));
	}

	//-------------------------------------------------------------------------
	// Compute normal of a triangle. return normal length
	//-------------------------------------------------------------------------
	float ComputeNormal(const Vector& V0, const Vector& V1, const Vector& V2)
	{
		Vector E = V1; E -= V0;
		Vector F = V2; F -= V1;

		(*this)  = E ^ F;
		
		return (*this).Normalise();
	}

	Vector& RotateAroundAxis(const Vector& P, const Vector& D, float angle)
	{
		//rotate around the vector parallel to (u,v,w) that passes through (a,b,c)
		float axx,axy,axz,ax1;
		float ayx,ayy,ayz,ay1;
		float azx,azy,azz,az1;

		float u,v,w;
		float u2,v2,w2;
		float a,b,c;

		float sa,ca;

		sa=sin(angle);
		ca=cos(angle);

		u = D.x;
		v = D.y;
		w = D.z;
		
		u2 = u*u;
		v2 = v*v;
		w2 = w*w;

		a = P.x;
		b = P.y;
		c = P.z;

		axx = u2+(v2+w2)*ca;		axy = u*v*(1-ca)-w*sa;	axz = u*w*(1-ca)+v*sa;	ax1 = a*(v2+w2)-u*(b*v+c*w)+(u*(b*v+c*w)-a*(v2+w2))*ca+(b*w-c*v)*sa;
		ayx = u*v*(1-ca)+w*sa;		ayy = v2+(u2+w2)*ca;	ayz = v*w*(1-ca)-u*sa;	ay1 = b*(u2+w2)-v*(a*u+c*w)+(v*(a*u+c*w)-b*(u2+w2))*ca+(c*u-a*w)*sa;
		azx = u*w*(1-ca)-v*sa;		azy = v*w*(1-ca)+u*sa;	azz = w2+(u2+v2)*ca;	az1 = c*(u2+v2)-w*(a*u+b*v)+(w*(a*u+b*v)-c*(u2+v2))*ca+(a*v-b*u)*sa;

		Vector W;
		W.x = axx * x + axy * y + axz * z + ax1;
		W.y = ayx * x + ayy * y + ayz * z + ay1;
		W.z = azx * x + azy * y + azz * z + az1;

		*this = W;

		return *this;
	}
};




#endif //OLI_VECTOR_H
