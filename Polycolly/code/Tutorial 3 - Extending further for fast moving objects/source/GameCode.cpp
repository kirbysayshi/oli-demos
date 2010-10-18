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

// collision data
Vector Ncoll;
float  tcoll;
	
// the two polygons
Vector	m_xPosition[2];
Vector  m_xDisplacement[2];
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
		m_xDisplacement[i] = Vector(0, 0);
		m_xPosition[i].Randomise(Vector(dbg_world_size * 0.25f, dbg_world_size * 0.25f), Vector(dbg_world_size * 0.75f, dbg_world_size * 0.75f));
		m_iNumVertices[i] = rand() % 6 + 3;
		float fRadius = frand(dbg_world_size * 0.05f) + dbg_world_size * 0.1f;
		m_pxVertices[i] = PolyColl::BuildBlob(m_iNumVertices[i], fRadius);
	}
}
	

void GameUpdate(float dt)
{
	tcoll = 1.0f;

	dbg_collided = PolyColl::Collide(m_pxVertices[0], m_iNumVertices[0], 
									 m_pxVertices[1], m_iNumVertices[1], 
									 m_xPosition[0] - m_xPosition[1], 
									 m_xDisplacement[0] - m_xDisplacement[1], 
									 Ncoll, tcoll);
}

void Separate()
{
	if (dbg_collided && tcoll < 0.0f)
	{
		m_xPosition[0] -= Ncoll * (tcoll * 1.01f);
	}
}

void GameRender(void)
{
	u_int uColor = (dbg_collided && tcoll < 0.0f)? 0x80FF8080 : 0x8080FF80;
	int y = 1;

	PolyColl::Render(m_xPosition[0], uColor, 0xFFFFFFFF, m_pxVertices[0], m_iNumVertices[0]);
	PolyColl::Render(m_xPosition[1], uColor, 0xFF000000, m_pxVertices[1], m_iNumVertices[1]);

	PrintString(m_xPosition[0].x, m_xPosition[0].y, true, 0xFFFFFFFF, "Poly A");
	PrintString(m_xPosition[1].x, m_xPosition[1].y, true, 0xFFFFFFFF, "Poly B");

	// render the collision stuff
	RenderArrow(m_xPosition[0], m_xDisplacement[0].Direction(), m_xDisplacement[0].Length(), 0xFFFFFFFF);

	if (tcoll > 0.0f)
	{
		u_int uColor = (dbg_collided)? 0x40FF8080 : 0x4080FF80;
	
		Vector Pcoll = m_xPosition[0] + m_xDisplacement[0] * tcoll;
		PolyColl::Render(Pcoll, uColor, 0x40FFFFFF, m_pxVertices[0], m_iNumVertices[0]);
	}

	if (dbg_collided)
	{
		float length = (tcoll < 0.0f)? fabs(tcoll) : dbg_world_size * 0.1f;

		// render the collision stuff
		RenderArrow((m_xPosition[0] + m_xPosition[1]) * 0.5f, Ncoll, length, 0xFFFFFFFF);
		
		if (tcoll < 0.0f)
		{
			PrintString(1, y++, false, 0xFFFFFFFF, "Press 'RETURN' to separate the objects");
		}
	}
	PrintString(1, y++, false, 0xFFFFFFFF, "Press mnouse button 1 to launch poly A towards mouse cursor");
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
	if (buttons & 1)
		m_xDisplacement[0] = Vector(x, y) - m_xPosition[0];
}

