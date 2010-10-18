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

static int  ikeypressedtimer = 0;
static int  itimer			 = 0;
static float mousex, mousey;
static int mouseb;

enum { eNumBodies= 1000, ePlayer = 4, eBulletStart = 5, eBulletEnd = 35, eNumBodiesDefault = eBulletEnd + 5, eMinBodies = eBulletEnd };
CBody *m_pxBodies[eNumBodies];
int  m_iNumBodies = eNumBodiesDefault;

//------------------------------------------------------
// player stuff
//------------------------------------------------------
unsigned char keybuf[256];
float KeyVal(int key) { return (keybuf[key] & (1 << 7))?  1.0f : 0.0f; }
CBody *m_pxPlayer = m_pxBodies[ePlayer];
float fPlayerAirbornTimer = 0.0f;
float fLaserDelay = 0.0f;
bool bLaserFire = false;
Vector LaserPos;
Vector LaserEndPos;
bool HandlePlayerContact(const Vector& N, float& t, CBody* pxPlayerBody, CBody* pxOtherBody);
int m_iNumBullets = 0;

enum { eMaxBulletHit = 5 };

int iBulletHitCount[eBulletEnd - eBulletStart] = { 0 };

int FindBody(CBody* pxBody)
{
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (m_pxBodies[i] == pxBody)
			return i;
	}
	return -1;
}


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
	
	m_pxPlayer = m_pxBodies[4] = new CBody(Vector(dbg_world_size * 0.5f, dbg_world_size * 0.5f), 0.4f, dbg_world_size * 0.07f, dbg_world_size * 0.10f);
	m_pxPlayer->SetCollisionCallback(HandlePlayerContact);
	m_pxPlayer->GetInvInertia() = 0.0f;
	m_pxPlayer->GetInertia() = 0.0f;
	m_pxPlayer->GetAngVelocity() = 0.0f;
	m_pxPlayer->SetOrientation(0.0f);

	for(int i = eMinBodies; i < eNumBodies; i ++)
	{
		m_pxBodies[i] = new CBody();
	}
}

void UpdatePlayerMovement(float dt)
{		
	fPlayerAirbornTimer -= dt;

	if (fPlayerAirbornTimer < 0.0f)
		fPlayerAirbornTimer = 0.0f;

	Vector xPlayerImpulse;
	xPlayerImpulse.x = (KeyVal(VK_RIGHT)  - KeyVal(VK_LEFT)) * 20.0f * m_pxPlayer->GetMass() * (fPlayerAirbornTimer * 0.4f + 0.6f);
	xPlayerImpulse.y = (KeyVal(VK_UP   ) * 30.0f - KeyVal(VK_DOWN) * 10.0f) * m_pxPlayer->GetMass() * (fPlayerAirbornTimer * 0.4f + 0.6f);
	xPlayerImpulse  -= m_pxPlayer->GetLinVelocity() * 0.2f / dt;
	m_pxPlayer->AddForce(xPlayerImpulse);
}

bool HandlePlayerContact(const Vector& N, float& t, CBody* pxThisBody, CBody* pxOtherBody)
{
	if (N.y > 0.0f)
		fPlayerAirbornTimer = (N.y* N.y);

	return true;
}

//------------------------------------------------------
// player stuff
//------------------------------------------------------

bool BulletHitCallback(const Vector& N, float& t, CBody* pxBullet, CBody* pxOtherBody)
{
	int iBulletIndex = FindBody(pxBullet);

	if (iBulletIndex > 0)
	{
		iBulletHitCount[iBulletIndex - eBulletStart]++;
	}
	return true;
}

void FireLaser()
{
	m_iNumBullets++;

	if (m_iNumBullets >= (eBulletEnd - eBulletStart))
	{
		m_iNumBullets = 0;
	}
	iBulletHitCount[m_iNumBullets] = 0;

	CBody* &pxBullet = m_pxBodies[eBulletStart + m_iNumBullets];

	if (pxBullet == NULL)
	{
		pxBullet = new CBody();
	}

	Vector LaserDir = LaserEndPos - LaserPos;
	LaserDir.Normalise();

	Vector BulletStart = LaserEndPos;
	Vector BulletVel   = LaserDir * (dbg_world_size * 1.0f);
	float BulletMass   = 3.0f;

	pxBullet->Initialise_Bullet(BulletStart, BulletVel, BulletMass);
	pxBullet->SetCollisionCallback(BulletHitCallback);

	return;
	
/*
	float t = 1.0f;
	Vector N;
	CBody* pxBody = NULL;

	//-------------------------------------------------------
	// Integrate
	//-------------------------------------------------------
	for(int i = 0; i < m_iNumBodies; i ++)
	{
		if (!m_pxBodies[i])
			continue;

		if (i == ePlayer)
			continue;

		float tcoll;
		Vector Ncoll;
		if (m_pxBodies[i]->IntersectSegment(LaserPos, LaserEndPos, t, Ncoll, tcoll))
		{
			t = tcoll;
			N = Ncoll;
			pxBody = m_pxBodies[i];
		}
	}

	if (pxBody)
	{
		Vector Dir = (LaserEndPos - LaserPos);
		LaserEndPos = LaserPos + t * Dir;

		if (!pxBody->IsUnmovable())
		{
			Vector F = Dir;
			F.Normalise();
			F *= 15000.0f;
			pxBody->AddForce(F, LaserEndPos);
		}
	}
/**/
}

void UpdatePlayerLaser(float dt)
{

	for(int i = 0; i < (eBulletEnd - eBulletStart); i ++)
	{
		if (iBulletHitCount[i] > eMaxBulletHit)
		{
			if (m_pxBodies[eBulletStart + i])
			{
				delete m_pxBodies[eBulletStart + i];
				m_pxBodies[eBulletStart + i] = NULL;
			}
			iBulletHitCount[i] = 0;
		}
	}


	LaserPos = m_pxPlayer->GetPosition();
	LaserEndPos = Vector(mousex, mousey);

	if (fLaserDelay > 0.2f && mouseb)
	{
		Vector Dir = LaserEndPos - LaserPos;
		Dir.Normalise();
		LaserEndPos = LaserPos + Dir * dbg_world_size * 10.0f;

		LaserEndPos = LaserPos + Dir * dbg_world_size * 0.1f;

		bLaserFire = true;
		FireLaser();
		fLaserDelay = 0.0f;
	}
	else
	{
		bLaserFire = false;
		Vector Dir = LaserEndPos - LaserPos;
		Dir.Normalise();
		LaserEndPos = LaserPos + Dir * dbg_world_size * 0.1f;
	}
	fLaserDelay += dt;
}

void RenderPlayer()
{
	PrintString(m_pxPlayer->GetPosition().x, m_pxPlayer->GetPosition().y, true, 0xFFFFFFFF, "player");
	
	if (0)//bLaserFire)
	{
		RenderSegment(LaserPos, LaserEndPos, 0xFFFF0000, 0xFF00, 0.0f, 2.0f);
	}
	else
	{
		RenderSegment(LaserPos, LaserEndPos, 0xFFFFFFFF, 0x0000, 2.0f, 1.0f);
	}
}


void UpdatePlayer(float dt)
{
	UpdatePlayerMovement(dt);
	UpdatePlayerLaser(dt);
}

void GameUpdate(float dt)
{
	GetKeyboardState(keybuf);

	if (dbg_Pause)
		return;

	UpdatePlayer(dt);

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
		if (m_pxBodies[i])
			m_pxBodies[i]->Render();
	}
	RenderPlayer();

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
	mouseb = buttons;
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

		PrintString(2, 3, false, c, "- Mouse      [%d, %d]", (int) mousex, (int) mousey);
		PrintString(2, 4, false, c, "- Gravity    [%s]", dbg_UseGravity   ? "On" : "Off");
		PrintString(2, 5, false, c, "- Impulses   [%s]", dbg_UseImpulses  ? "On" : "Off");
		PrintString(2, 6, false, c, "- Friction   [%s]", dbg_UseFriction  ? "On" : "Off");
		PrintString(2, 7, false, c, "- Boxes      [%d]", m_iNumBodies);
		PrintString(2, 9, false, c, "- restitution[%1.2f]", s_xContactMaterial.GetRestitution());
		PrintString(2,10, false, c, "- Stat.Fric. [%1.2f]", s_xContactMaterial.GetStaticFriction());
		PrintString(2, 8, false, c, "- Dyn.Fric.  [%1.2f]", s_xContactMaterial.GetFriction());
		
		int x = 47;
		int y = 3;
		PrintString(x, y++, false, c, "--------------------------------");
		PrintString(x, y++, false, c, "- Pause      On/Off : 'p'");
		PrintString(x, y++, false, c, "- Gravity    On/Off : 'g'");
		PrintString(x, y++, false, c, "- Impulses   On/Off : 'i'");
		PrintString(x, y++, false, c, "- Stat.Fric. On/Off : 's'");
		PrintString(x, y++, false, c, "- Dyn.Fric.  On/Off : 'f'");
		PrintString(x, y++, false, c, "- Add Box           : '+'");
		PrintString(x, y++, false, c, "- Remove box        : '-'");
		PrintString(x, y++, false, c, "- Reset box count   : 'return'");
		PrintString(x, y++, false, c, "- Reset simulation  : 'spacebar'");
		PrintString(x, y++, false, c, "- Restitution       : 'numpad 8-5'");
		PrintString(x, y++, false, c, "- dynamic Friction  : 'numpad 7-4'");
		PrintString(x, y++, false, c, "- static friction   : 'numpad 9-6'");
		PrintString(x, y++, false, c, "- help              : 'h'");
		PrintString(x, y++, false, c, "--------------------------------");

		ikeypressedtimer+=8;
	}
}