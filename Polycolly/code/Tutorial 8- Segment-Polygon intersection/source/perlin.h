#ifndef OLI_PERLIN_H
#define OLI_PERLIN_H


/*
#include "perlin.h"
*/

/*
------------------------------------------------------------------
File: perlin.h
Started: 20/05/2004 00:18:39
  
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

#include <math.h>

// based on : 
// http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
class CPerlin
{
public:
	static float Linear_Interpolate(float a, float b, float x)
	{
		return  a*(1.0f - x) + b * x;
	}

	static float Cosine_Interpolate(float a, float b, float x)
	{
		static const float pi = (float)atan(1.0f) * 4.0f;
		float ft = x * pi;
		float f  = (1 - cos(ft)) * 0.5f;
		return  a*(1.0f - f) + b * f;
	}

	static float Cubic_Interpolate(float v0, float v1, float v2, float v3, float x)
	{
		float P = (v3 - v2) - (v0 - v1);
		float Q = (v0 - v1) - P;
		float R = v2 - v0;
		float S = v1;
		float x2 = x*x;
		float x3 = x2*x;
		return P*x3 + Q*x2 + R*x + S;
	}

	static float Noise1(int x, int y)
	{
		int n = x + y * 57;
		n = (n<<13) ^ n;
		int d = (n * (n*n * 15731 + 789221) + 1376312589) & 0x7fffffff;
	
		return 1.0f - (float) d / 1073741824.0f;
	}

	static float SmoothNoise_1(float x, float y)
	{
		float corners = ( Noise1(x-1, y-1) + Noise1(x+1, y-1) + Noise1(x-1, y+1) + Noise1(x+1, y+1) ) / 16.0f;
		float sides   = ( Noise1(x-1, y)   + Noise1(x+1, y)   + Noise1(x, y-1)   + Noise1(x, y+1) ) /  8.0f;
		float center  =  Noise1(x, y) / 4.0f;
		
		return corners + sides + center;
	}
	
	static float InterpolatedNoise_1(float x, float y)
	{
		int		integer_Y    = int(y);
		int		integer_X    = int(x);
		float	fractional_X = x - integer_X;
		float	fractional_Y = y - integer_Y;

		float v1 = SmoothNoise_1(integer_X,     integer_Y);
		float v2 = SmoothNoise_1(integer_X + 1, integer_Y);
		float v3 = SmoothNoise_1(integer_X,     integer_Y + 1);
		float v4 = SmoothNoise_1(integer_X + 1, integer_Y + 1);

		float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
		float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);

		return Cosine_Interpolate(i1 , i2 , fractional_Y);
	}

	static float PerlinNoise_2D(float x, float y, float persistence=0.25f, int number_of_octaves=5)
	{
		float total = 0.0f;
		float p		= persistence;
		int   n		= number_of_octaves - 1;

		static const float pi = (float)atan(1.0f) * 4.0f;
	
		for(int i = 0; i < n; i ++)
		{
			float frequency = (float) pow(2, i);
			float amplitude = (float) pow(p, i);
	        total += InterpolatedNoise_1(x * frequency, y * frequency) * amplitude;
		}
		return total;
	}
};


#endif OLI_PERLIN_H