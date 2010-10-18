//------------------------------------------------------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// File          : Matrix.h
// 
// Created by    : OR - 07/02/2004 13:28:09
// 
// Copyright (C) : 2004 Rebellion, All Rights Reserved.
// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// Description   : 
// --------------- 
// 
// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
//------------------------------------------------------------------------------------------------ 


#ifdef ASURA_USE_PRAGMA_ONCE
	#pragma once
#endif

#ifndef __MATRIX_H__
#define __MATRIX_H__


#include "Vector.h"

class Matrix
{
public:

	union
	{
		struct
		{
			float e11;
			float e12;
			float e21;
			float e22;
		};

		float e[2][2];
	};

	Matrix(float _e11, float _e12, float _e21, float _e22)
	: e11(_e11)
	, e12(_e12)
	, e21(_e21)
	, e22(_e22)
	{}
	
	Matrix()
	{}

	Matrix(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);

		e11 = c; e12 = s;
		e21 =-s; e22 = c;
	}

	float  operator()(int i, int j) const { return e[i][j]; }
	float& operator()(int i, int j)       { return e[i][j]; }

	
	const Vector& operator[](int i) const
	{
		return reinterpret_cast<const Vector&>(e[i][0]);
	}
	
	Vector& operator[](int i)
	{
		return reinterpret_cast<Vector&>(e[i][0]);
	}		

	static Matrix Identity()
	{
		static const Matrix T(1.0f, 0.0f, 0.0f, 1.0f);

		return T;
	}

	static Matrix Zer0()
	{
		static const Matrix T(0.0f, 0.0f, 0.0f, 0.0f);

		return T;
	}


	Matrix Tranpose() const
	{
		Matrix T;

		T.e11 = e11;
		T.e21 = e12;
		T.e12 = e21;
		T.e22 = e22;

		return T;
	}

	Matrix operator * (const Matrix& M) const 
	{
		Matrix T;

		T.e11 = e11 * M.e11 + e12 * M.e21;
		T.e21 = e21 * M.e11 + e22 * M.e21;
		T.e12 = e11 * M.e12 + e12 * M.e22;
		T.e22 = e21 * M.e12 + e22 * M.e22;
		
		return T;
	}

	Matrix operator ^ (const Matrix& M) const 
	{
		Matrix T;

		T.e11 = e11 * M.e11 + e12 * M.e12;
		T.e21 = e21 * M.e11 + e22 * M.e12;
		T.e12 = e11 * M.e21 + e12 * M.e22;
		T.e22 = e21 * M.e21 + e22 * M.e22;
		
		return T;
	}

	inline Matrix operator * ( float s) const
	{
		Matrix T;

		T.e11 = e11 * s;
		T.e21 = e21 * s;
		T.e12 = e12 * s;
		T.e22 = e22 * s;
		
		return T;
	}



};
#endif//__MATRIX_H__