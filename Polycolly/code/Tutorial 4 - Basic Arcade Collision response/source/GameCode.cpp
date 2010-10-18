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
#include <windows.h>

#include "GameCode.h"

float  dbg_world_size    = 100;
bool   dbg_Pause		 = false;
bool   dbg_UseFriction   = true;
bool   dbg_UseGravity    = true;

static int  ikeypressedtimer = 0;
static int  itimer			 = 0;
static float mousex, mousey;


enum { eNumBodies= 1000, eMinBodies = 5, eNumBodiesDefault = 10 };
CBody *m_pxBodies[eNumBodies];
CBody *m_pxPlayer = m_pxBodies[eMinBodies];
int  m_iNumBodies = eNumBodiesDefault;

float fCoF  = 0.0f;
float fCoR  = 0.1f;
float fGlue = 0.01f;
Vector xPlayerImpulse = Vector(0, 0);

void GameShutdown()
{
	for(int i = 0; i < eNumBodies; i ++)
	{
		if (m_pxBodies[i])
			delete m_pxBodies[i];

		m_pxBodies[i] = NULL;
	}
}

void GameInit(void)
{
	GameShutdown();

	Vector xBottom= Vector( dbg_world_size * 0.50f, -dbg_world_size * 0.49f);
	Vector xTop   = Vector( dbg_world_size * 0.50f,  dbg_world_size * 1.49f);
	Vector xLeft  = Vector(-dbg_world_size * 0.49f,  dbg_world_size * 0.50f);
	Vector xRight = Vector( dbg_world_size * 1.49f,  dbg_world_size * 0.50f);

	m_pxBodies[0] = new CBody(xBottom, 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[1] = new CBody(xTop	 , 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[2] = new CBody(xLeft	 , 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[3] = new CBody(xRight , 0.0f, dbg_world_size, dbg_world_size);

	m_pxPlayer = m_pxBodies[4] = new CBody(Vector(dbg_world_size * 0.5f, dbg_world_size * 0.5f), 100.0f, dbg_world_size * 0.1f, dbg_world_size * 0.15f);

	for(int i = eMinBodies; i < eNumBodies; i ++)
	{
		m_pxBodies[i] = new CBody();
	}
}


unsigned char keybuf[256];

float KeyVal(int key) 
{ 
	if (keybuf[key] & (1 << 7))
	{
		return 1.0f;
	}
	return 0.0f; 
}

void GameUpdate(float dt)
{
	GetKeyboardState(keybuf);

	if (dbg_Pause)
		return;

	//-------------------------------------------------------
	// add forces
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])	continue;
		
		if (dbg_UseGravity)
		{
			m_pxBodies[i]->AddImpulse(Vector(0, -25.0f * m_pxBodies[i]->GetMass()), dt);
		}
	}

	if(m_pxPlayer)
	{
		xPlayerImpulse.x = (KeyVal(VK_RIGHT)  - KeyVal(VK_LEFT)) * 100.0f * m_pxPlayer->GetMass();
		xPlayerImpulse.y = (KeyVal(VK_UP   )) * 200.0f * m_pxPlayer->GetMass();
		xPlayerImpulse  -= m_pxPlayer->GetDisplacement() * 0.3f / dt;
		m_pxPlayer->AddImpulse(xPlayerImpulse, dt);
	}

	xPlayerImpulse = Vector(0, 0);

	//-------------------------------------------------------
	// test collisions
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])	continue;
		
		for(int j = i+1; j < m_iNumBodies; j ++)
		{
			if (!m_pxBodies[j])	continue;

			m_pxBodies[i]->Collide(*(m_pxBodies[j]));
		}
	}

	//-------------------------------------------------------
	// Integrate
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])	continue;

		m_pxBodies[i]->Update(dt);
	}
}

void GameRender(void)
{
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])	continue;

		m_pxBodies[i]->Render();
	}

	if (m_pxPlayer)
	{
		PrintString(m_pxPlayer->GetPosition().x, m_pxPlayer->GetPosition().y, true, 0xFFFFFFFF, "player");
		RenderArrow(m_pxPlayer->GetPosition(), m_pxPlayer->GetDisplacement().Direction(), m_pxPlayer->GetDisplacement().Length() + 10.0f, 0xFFFFFFFF);
	}
	GameRenderStats();
}

void GameOnExtKeyCallback(int keypressed)
{
	switch(keypressed)
	{
	case GLUT_KEY_DOWN:		xPlayerImpulse.y =-1.0f;	break;
	case GLUT_KEY_UP:		xPlayerImpulse.y = 1.0f;	break;
	case GLUT_KEY_LEFT:		xPlayerImpulse.x =-1.0f;	break;
	case GLUT_KEY_RIGHT:	xPlayerImpulse.x = 1.0f;	break;
	}
}

void GameOnKeyCallback(int keypressed)
{
	switch(keypressed)
	{
	case '7':
		fCoF = clampf(fCoF + 0.05f, 0.0f, 1.0f);
		break;
	case '4':
		fCoF = clampf(fCoF - 0.05f, 0.0f, 1.0f);
		break;
	case '8':
		fCoR = clampf(fCoR  + 0.05f, 0.0f, 1.0f);
		break;
	case '5':
		fCoR = clampf(fCoR  - 0.05f, 0.0f, 1.0f);
		break;
	case '9':
		fGlue = clampf(fGlue  + 0.01f, 0.0f, 3.0f);
		break;
	case '6':
		fGlue = clampf(fGlue  - 0.01f, 0.0f, 3.0f);
		break;
	case 'p':
	case 'P':
		dbg_Pause = !dbg_Pause;
		break;
	case 'f':
	case 'F':
		dbg_UseFriction = !dbg_UseFriction;
		break;
	case 'g':
	case 'G':
		dbg_UseGravity = !dbg_UseGravity;
		break;
	case '-':
	case '_':
		m_iNumBodies--;
		if (m_iNumBodies < eMinBodies)
			m_iNumBodies = eMinBodies;
		break;
	case 13:
		m_iNumBodies = eNumBodiesDefault;
		return;
	case '+':
	case '=':
		m_iNumBodies++;
		if (m_iNumBodies >= eNumBodies)
			m_iNumBodies = eNumBodies-1;
		break;
	case ' ':
		GameInit();
		break;
	case 'h':
	case 'H':
		ikeypressedtimer = 0;
		break;
	default:
		printf("\n");
		printf("--------------------------------\n");
		printf("- Keys                         -\n");
		printf("--------------------------------\n");
		printf("- Pause      On/Off : 'p'\n");
		printf("- Gravity    On/Off : 'g'\n");
		printf("- Glue       On/Off : 's'\n");
		printf("- Friction   On/Off : 'f'\n");
		printf("- Add Box           : '+'\n");
		printf("- Remove box        : '-'\n");
		printf("- Reset box count   : 'return'\n");
		printf("- Reset simulation  : 'spacebar'\n");
		printf("- Friction			: 'numpad 7-4'\n");
		printf("- Restitution       : 'numpad 8-5'\n");
		printf("- help              : 'h'\n");
		printf("--------------------------------\n");
		printf("\n");
		break;
	}

	CBody::SetRestitution(fCoR);
	CBody::SetFriction   (fCoF);
	CBody::SetGlue       (fGlue);

	printf("\n");
	printf("--------------------------------\n");
	printf("- Flags                        -\n");
	printf("--------------------------------\n");
	printf("- Gravity    [%s]\n", dbg_UseGravity   ? "On" : "Off");
	printf("- Friction   [%s]\n", dbg_UseFriction  ? "On" : "Off");
	printf("- boxes      [%d]\n", m_iNumBodies);
	printf("- Friction.  [%1.2f]\n", CBody::GetFriction());
	printf("- Restitution[%1.2f]\n", CBody::GetRestitution());
	printf("- Glue       [%1.2f]\n", CBody::GetGlue());
	printf("--------------------------------\n");
	printf("\n");
}


void GameMouse(float x, float y, int buttons)
{
	mousex = x;
	mousey = y;
}

void GameRenderStats()
{
	if (dbg_Pause)
		ikeypressedtimer = 0;

	if (ikeypressedtimer < 256)
	{
		unsigned int a = (255 - ikeypressedtimer);
		unsigned int r = 0xFF;
		unsigned int g = 0xFF;
		unsigned int b = 0xFF;
		unsigned int c = ((a << 24) | (r << 16) | (g << 8) | b);

		int x = 2;
		int y = 3;

		PrintString(x, y++, false, c, "- Mouse      [%d, %d]", (int) mousex, (int) mousey);
		PrintString(x, y++, false, c, "- Gravity    [%s]", dbg_UseGravity   ? "On" : "Off");
		PrintString(x, y++, false, c, "- Friction   [%s]", dbg_UseFriction  ? "On" : "Off");
		PrintString(x, y++, false, c, "- Boxes      [%d]", m_iNumBodies);
		PrintString(x, y++, false, c, "- friction   [%1.2f]", CBody::GetFriction());
		PrintString(x, y++, false, c, "- restitution[%1.2f]", CBody::GetRestitution());
		PrintString(x, y++, false, c, "- Glue       [%1.2f]", CBody::GetGlue());
	
		x = 47;
		y = 3;
		PrintString(x, y++, false, c, "--------------------------------");
		PrintString(x, y++, false, c, "- Pause      On/Off : 'p'");
		PrintString(x, y++, false, c, "- Gravity    On/Off : 'g'");
		PrintString(x, y++, false, c, "- Glue       On/Off : 's'");
		PrintString(x, y++, false, c, "- Friction   On/Off : 'f'");
		PrintString(x, y++, false, c, "- Add Box           : '+'");
		PrintString(x, y++, false, c, "- Remove box        : '-'");
		PrintString(x, y++, false, c, "- Reset box count   : 'return'");
		PrintString(x, y++, false, c, "- Reset simulation  : 'spacebar'");
		PrintString(x, y++, false, c, "- Friction          : 'numpad 7-4'");
		PrintString(x, y++, false, c, "- Restitution       : 'numpad 8-5'");
		PrintString(x, y++, false, c, "- Glue              : 'numpad 9-6'");
		PrintString(x, y++, false, c, "- help              : 'h'");
		PrintString(x, y++, false, c, "--------------------------------");

		ikeypressedtimer+=8;
	}
}