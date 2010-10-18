/*
------------------------------------------------------------------
File: dynamics.cpp
Started: 18/01/2004 14:15:44
  
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

//-----------------------------------------------------------
// Verlet rigid body dynamics
//-----------------------------------------------------------
#include "GameCode.h"

float  dbg_world_size    = 100;
bool   dbg_collided = false;
static float mousex, mousey;

// collision data
Vector Ncoll;
float  dcoll;
	
// the two polygons
Vector	m_xPosition[2];
Vector *m_pxVertices[2] = { NULL, NULL };
int m_iNumVertices[2];

void GameShutdown()
{
	for(int i = 0; i < 2; i ++)
	{
		if (m_pxVertices[i])
			delete[] m_pxVertices[i];
		m_pxVertices[i] = NULL;
	}
}

void GameInit(void)
{
	GameShutdown();

	for(int i = 0; i < 2; i ++)
	{
		m_xPosition[i].Randomise(Vector(dbg_world_size * 0.25f, dbg_world_size * 0.25f), Vector(dbg_world_size * 0.75f, dbg_world_size * 0.75f));
		m_iNumVertices[i] = rand() % 6 + 3;
		float fRadius = frand(dbg_world_size * 0.05f) + dbg_world_size * 0.1f;
		m_pxVertices[i] = PolyColl::BuildBlob(m_iNumVertices[i], fRadius);
	}
}
	

void GameUpdate(float dt)
{
	dbg_collided = PolyColl::Collide(m_pxVertices[0], m_iNumVertices[0], m_pxVertices[1], m_iNumVertices[1], m_xPosition[0] - m_xPosition[1], Ncoll, dcoll);
}

void Separate()
{
	if (dbg_collided)
	{
		m_xPosition[0] -= Ncoll * (dcoll * 1.01f);
	}
}

void GameRender(void)
{
	u_int uColor = (dbg_collided)? 0x80FF8080 : 0x8080FF80;

	PolyColl::Render(m_xPosition[0], uColor, 0xFFFFFFFF, m_pxVertices[0], m_iNumVertices[0]);
	PolyColl::Render(m_xPosition[1], uColor, 0xFF000000, m_pxVertices[1], m_iNumVertices[1]);

	PrintString(m_xPosition[0].x, m_xPosition[0].y, true, 0xFFFFFFFF, "Poly A");
	PrintString(m_xPosition[1].x, m_xPosition[1].y, true, 0xFFFFFFFF, "Poly B");

	if (dbg_collided)
	{
		// render the collision stuff
		RenderArrow((m_xPosition[0] + m_xPosition[1]) * 0.5f, Ncoll, fabs(dcoll), 0xFFFFFFFF);

		PrintString(1, 1, false, 0xFFFFFFFF, "Press 'RETURN' to separate the objects");
	}
}

void GameOnKeyCallback(int keypressed)
{
	switch(keypressed)
	{
	case ' ':				GameInit();					break;
	case 13:				Separate();					break;
	case GLUT_KEY_DOWN:		m_xPosition[0].y -= 2.0f;	break;
	case GLUT_KEY_UP:		m_xPosition[0].y += 2.0f;	break;
	case GLUT_KEY_LEFT:		m_xPosition[0].x -= 2.0f;	break;
	case GLUT_KEY_RIGHT:	m_xPosition[0].x += 2.0f;	break;

	}
}


void GameMouse(float x, float y, int buttons)
{
	mousex = x;
	mousey = y;
}

