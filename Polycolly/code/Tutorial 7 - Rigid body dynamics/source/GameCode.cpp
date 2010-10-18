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
bool   dbg_Pause		 = false;
bool   dbg_UseImpulses   = true;
bool   dbg_UseFriction   = true;
bool   dbg_UseGravity    = true;
bool   dbg_UseAttractor  = true;
bool   dbg_UseRepulsor   = true;

Vector dbg_xAttractor;

static int  ikeypressedtimer = 0;
static int  itimer			 = 0;
static float mousex, mousey;


enum { eNumBodies= 1000, eMinBodies = 5, eNumBodiesDefault = 20 };
CBody *m_pxBodies[eNumBodies];
int  m_iNumBodies = eNumBodiesDefault;


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
	Vector xCentre= Vector( dbg_world_size * 0.50f,  dbg_world_size * 0.50f);

	m_pxBodies[0] = new CBody(xBottom, 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[1] = new CBody(xTop	 , 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[2] = new CBody(xLeft	 , 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[3] = new CBody(xRight , 0.0f, dbg_world_size, dbg_world_size);
	m_pxBodies[4] = new CBody(xCentre, 0.0f, dbg_world_size * 0.01f, dbg_world_size * 0.01f);
	for(int i = 5; i < eNumBodies; i ++)
	{
		m_pxBodies[i] = new CBody();
	}
}
	

void GameUpdate(float dt)
{
	if (dbg_Pause)
		return;

	//-------------------------------------------------------
	// add forces
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])	continue;
		if (m_pxBodies[i]->IsUnmovable())	continue;

		if (dbg_UseGravity)
		{
			m_pxBodies[i]->AddForce(Vector(0, -5 * m_pxBodies[i]->GetMass()));
		}

		Vector xDist   = (dbg_xAttractor - m_pxBodies[i]->GetPosition());
		float fDist    = xDist.Normalise();
		float fMinDist = dbg_world_size / 10.0f;
		float fMaxDist = dbg_world_size /  4.0f;
		float fratio   = (fDist - fMinDist) / (fMaxDist - fMinDist);
		float fForce   = 500.0f;
		fratio = (fratio < 0.0f)? 0.0f : (fratio > 1.0f)? 1.0f : fratio;

		Vector xForce(0.0f, 0.0f);

		if (dbg_UseRepulsor)
		{
			xForce = -xDist * ((1.0f - fratio) * fForce);
		}
		if (dbg_UseAttractor)
		{
			xForce = xDist * (fratio * fForce);
		}

		m_pxBodies[i]->AddForce(xForce);
	}

	//-------------------------------------------------------
	// test collisions
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])	continue;
		
		for(int j = i+1; j < m_iNumBodies; j ++)
		{
			if (!m_pxBodies[j])	continue;

			if (m_pxBodies[i]->IsUnmovable() && m_pxBodies[j]->IsUnmovable()) continue;
			
			m_pxBodies[i]->Collide(*(m_pxBodies[j]), dt);
		}
	}

	//-------------------------------------------------------
	// Integrate
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])					continue;
		if ( m_pxBodies[i]->IsUnmovable())	continue;
		
		m_pxBodies[i]->Update(dt);
	}
}

void GameRender(void)
{
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		m_pxBodies[i]->Render();
	}
	GameRenderStats();
}

float fCoF = 0.3f;
float fCoR = 0.7f;
float fCoS = 0.5f;

float Clamp(float a, float min, float max)
{
	return (a < min)? min : (a > max)? max : a;
}

void GameOnKeyCallback(char keypressed)
{
	switch(keypressed)
	{
	case '7':
		fCoF = Clamp(fCoF + 0.05f, 0.0f, 1.0f);
		break;
	case '4':
		fCoF = Clamp(fCoF - 0.05f, 0.0f, 1.0f);
		break;
	case '8':
		fCoR = Clamp(fCoR  + 0.05f, 0.0f, 1.0f);
		break;
	case '5':
		fCoR = Clamp(fCoR  - 0.05f, 0.0f, 1.0f);
		break;
	case '9':
		fCoS = Clamp(fCoS  + 0.01f, 0.0f, 1.5f);
		break;
	case '6':
		fCoS = Clamp(fCoS - 0.01f, 0.0f, 1.5f);
		break;
	case 'p':
	case 'P':
		dbg_Pause = !dbg_Pause;
		break;
	case 'f':
	case 'F':
		dbg_UseFriction = !dbg_UseFriction;
		break;
	case 'i':
	case 'I':
		dbg_UseImpulses = !dbg_UseImpulses;
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
	default:
		printf("\n");
		printf("--------------------------------\n");
		printf("- Keys                         -\n");
		printf("--------------------------------\n");
		printf("- Pause      On/Off : 'p'\n");
		printf("- Gravity    On/Off : 'g'\n");
		printf("- Impulses   On/Off : 'i'\n");
		printf("- dyna.fric. On/Off : 'f'\n");
		printf("- stat.fric. On/Off : 's'\n");
		printf("- Add Box           : '+'\n");
		printf("- Remove box        : '-'\n");
		printf("- Reset box count   : 'return'\n");
		printf("- Reset simulation  : 'spacebar'\n");
		printf("- attract / repulse : 'mouse'\n");
		printf("- Restitution       : 'numpad 8-5'\n");
		printf("- dynamic friction  : 'numpad 7-4'\n");
		printf("- static  friction  : 'numpad 9-6'\n");
		printf("- help              : 'h'\n");
		printf("--------------------------------\n");
		printf("\n");
		break;
	}

	s_xContactMaterial.SetRestitution(fCoR);
	s_xContactMaterial.SetFriction   (fCoF);
	s_xContactMaterial.SetStaticFriction(fCoS);

	printf("\n");
	printf("--------------------------------\n");
	printf("- Flags                        -\n");
	printf("--------------------------------\n");
	printf("- Gravity    [%s]\n", dbg_UseGravity   ? "On" : "Off");
	printf("- Impulses   [%s]\n", dbg_UseImpulses  ? "On" : "Off");
	printf("- Friction   [%s]\n", dbg_UseFriction  ? "On" : "Off");
	printf("- boxes      [%d]\n", m_iNumBodies);
	printf("- Restitution[%1.2f]\n", s_xContactMaterial.GetRestitution());
	printf("- Stat.Fric. [%1.2f]\n", s_xContactMaterial.GetStaticFriction());
	printf("- Dyn.Fric.  [%1.2f]\n", s_xContactMaterial.GetFriction());
	printf("--------------------------------\n");
	printf("\n");
}


void GameMouse(float x, float y, int buttons)
{
	mousex = x;
	mousey = y;
	dbg_xAttractor = Vector(x, y);
	dbg_UseAttractor = (buttons & 1)? true : false;
	dbg_UseRepulsor  = (buttons & 2)? true : false;
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

		Printf(2, 3, c, "- Mouse      [%d, %d]", (int) mousex, (int) mousey);
		Printf(2, 4, c, "- Gravity    [%s]", dbg_UseGravity   ? "On" : "Off");
		Printf(2, 5, c, "- Impulses   [%s]", dbg_UseImpulses  ? "On" : "Off");
		Printf(2, 6, c, "- Friction   [%s]", dbg_UseFriction  ? "On" : "Off");
		Printf(2, 7, c, "- Boxes      [%d]", m_iNumBodies);
		Printf(2, 9, c, "- restitution[%1.2f]", s_xContactMaterial.GetRestitution());
		Printf(2,10, c, "- Stat.Fric. [%1.2f]", s_xContactMaterial.GetStaticFriction());
		Printf(2, 8, c, "- Dyn.Fric.  [%1.2f]", s_xContactMaterial.GetFriction());
		
		int x = 47;
		int y = 3;
		Printf(x, y++, c, "--------------------------------");
		Printf(x, y++, c, "- Pause      On/Off : 'p'");
		Printf(x, y++, c, "- Gravity    On/Off : 'g'");
		Printf(x, y++, c, "- Impulses   On/Off : 'i'");
		Printf(x, y++, c, "- Stat.Fric. On/Off : 's'");
		Printf(x, y++, c, "- Dyn.Fric.  On/Off : 'f'");
		Printf(x, y++, c, "- Add Box           : '+'");
		Printf(x, y++, c, "- Remove box        : '-'");
		Printf(x, y++, c, "- Reset box count   : 'return'");
		Printf(x, y++, c, "- Reset simulation  : 'spacebar'");
		Printf(x, y++, c, "- attract / repulse : 'mouse'");
		Printf(x, y++, c, "- Restitution       : 'numpad 8-5'");
		Printf(x, y++, c, "- dynamic Friction  : 'numpad 7-4'");
		Printf(x, y++, c, "- static friction   : 'numpad 9-6'");
		Printf(x, y++, c, "- help              : 'h'");
		Printf(x, y++, c, "--------------------------------");

		ikeypressedtimer+=8;
	}
}