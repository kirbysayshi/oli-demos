//------------------------------------------------------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// File          : Heightmap.h
// 
// Created by    : OR - 11/02/2004 13:36:13
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

#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__


#include "Vector.h"

struct CHeightmap
{
public:
	void operator = (const CHeightmap& xHeightmap)
	{
		if (m_iglList)
			glDeleteLists(m_iglList, 1);

		memcpy(m_fHeight, xHeightmap.m_fHeight, sizeof(m_fHeight)); 
	}

	~CHeightmap()
	{
		DestroyGLList();
	}

	CHeightmap()
	{
		//----------------------------------------------------------------------------------------------- 
		// OR [11/02/2004 ] : compute point positions
		//----------------------------------------------------------------------------------------------- 
		for(int j = 0; j < eRows; j ++)
		{
			for(int i = 0; i < eColumns; i ++)
			{
				m_fHeight[j][i] = (float)(rand() & 63) / 255.0f;
			}
		}

		m_iglList = -1;
		m_iTex = -1;
	}

	void Render(void)
	{
		if (!glIsList(m_iglList))
			MakeGLList();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_iTex);
	
		SetMaterial(5, 1.0f);

		glCallList(m_iglList);

		glDisable(GL_TEXTURE_2D);
	}

	float GetHeight(float x, float z) const
	{
		int i = (int) x;
		int j = (int) z;

		if (i >= eRows || i < 0)
			return 0.0f;

		if (j >= eColumns || j < 0)
			return 0.0f;

		float dx = x - (float) i;
		float dz = z - (float) j;
		
		const float& a = m_fHeight[i  ][j  ];
		const float& b = m_fHeight[i+1][j  ];
		const float& c = m_fHeight[i+1][j+1];
		const float& d = m_fHeight[i  ][j+1];

		if (dx > dz)
		{
			 return b + (c - b) * dz + (a - b) * (1.0f - dx);
		}
		else
		{
			return d + (c - d) * dx + (a - d) * (1.0f - dz);
		}
	}

private:
	void DestroyGLList()
	{
		if (glIsTexture(m_iTex))
			glDeleteTextures(1, &m_iTex);
		
		if (glIsList(m_iglList))
			glDeleteLists(m_iglList, 1);
		
		m_iTex = -1;
		m_iglList = -1;
	}
	
	void MakeGLList()
	{
		DestroyGLList();

		unsigned char t[32 * 32][3];

		for(int i = 0; i < 32; i++)
		{
			for(int j = 0; j < 32; j ++)
			{
				int b = rand() & 255;

				t[i + j * 32][0] = b;
				t[i + j * 32][1] = b;
				t[i + j * 32][2] = b;
			}
		}
		
  
		glGenTextures(1, &m_iTex);
		glBindTexture(GL_TEXTURE_2D, m_iTex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, t);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float rgba[] = { 0.3f, 0.3f, 0.3f, 0.3f };
		glTexEnvi	   (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_LUMINANCE);
		glTexEnvfv	   (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, rgba);

		
		m_iglList = glGenLists(1);

		glNewList(m_iglList, GL_COMPILE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBegin(GL_TRIANGLES);

		for(int j = 0; j < eRows-1; j ++)
		{
			for(int i = 0; i < eColumns-1; i ++)
			{
				int i0 = i;
				int j0 = j;
				int i1 = i+1;
				int j1 = j+1;

				Vector A(i0, m_fHeight[i0][j0], j0);
				Vector B(i1, m_fHeight[i1][j0], j0);
				Vector C(i1, m_fHeight[i1][j1], j1);
				Vector D(i0, m_fHeight[i0][j1], j1);
				Vector N;
				Vector M;

				N.ComputeNormal(A, B, D);
				M.ComputeNormal(B, C, D);

				glNormal3f(N.x, N.y, N.z);
				glTexCoord2f(A.x, A.z);	glVertex3f(A.x, A.y, A.z);
				glTexCoord2f(B.x, B.z);	glVertex3f(B.x, B.y, B.z);
				glTexCoord2f(C.x, C.z);	glVertex3f(C.x, C.y, C.z);

				glNormal3f(M.x, M.y, M.z);
				glTexCoord2f(A.x, A.z);	glVertex3f(A.x, A.y, A.z);
				glTexCoord2f(C.x, C.z);	glVertex3f(C.x, C.y, C.z);
				glTexCoord2f(D.x, D.z);	glVertex3f(D.x, D.y, D.z);

				m_xNormal[i0][j0][0] = -N;
				m_xNormal[i0][j0][1] = -M;
			}
		}
		glEnd();

		glEndList();
	}
private:
	enum { eRows = 128, eColumns = 128 };

	float			m_fHeight[eRows][eColumns];
	Vector			m_xNormal[eRows][eColumns][2];
	int				m_iglList;
	unsigned int	m_iTex;


};

#endif//__HEIGHTMAP_H__