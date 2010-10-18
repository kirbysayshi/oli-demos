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

static float mousex, mousey;
static int mouseb;

Vector *A;
Vector *B;
Vector *C;
int Anum, Bnum;
int Cnum;

//------------------------------------------------------
// player stuff
//------------------------------------------------------
void GameShutdown()
{
	 delete[] A;
	 delete[] B;
	 A = B = NULL;
	 Anum = 0;
	 Bnum = 0;
}

void GameInit(void)
{
	GameShutdown();

	Vector PA, PB;
	float oa, ob;
	float ra, rb;

	PA.Randomise(Vector(dbg_world_size * 0.5f, dbg_world_size * 0.5f),
	   		     Vector(dbg_world_size * 0.5f, dbg_world_size * 0.5f));
	   	 				   
	PB.Randomise(Vector(dbg_world_size * -0.15f, dbg_world_size * -0.15f),
 	             Vector(dbg_world_size *  0.15f, dbg_world_size *  0.15f));
 	             
	PB += PA;

	oa = frand();
	ob = frand();
	ra = dbg_world_size * (frand(0.1f) + 0.15f);
	rb = dbg_world_size * (frand(0.1f) + 0.15f);
	
	Anum = rand()%2+3;
	Bnum = rand()%5+3;
	Cnum = 32;
	
	A = PolyClip::BuildBlob(Anum, ra, ra);
	B = PolyClip::BuildBlob(Bnum, rb, rb);
	C = PolyClip::BuildBlob(Cnum, 1, 1);

	PolyClip::Transform(A, Anum, A, PA, oa);
	PolyClip::Transform(B, Bnum, B, PB, ob);
}

void GameUpdate(float dt)
{
 PolyClip::PolygonClip(A, Anum, B, Bnum, C, Cnum);
}

void GameRender(void)
{
	 PolyClip::Render(Vector(0, 0), 0.0f, 0X30000080, 0XFF0000FF, B, Bnum);
	 PolyClip::Render(Vector(0, 0), 0.0f, 0X20FFFFFF, 0XFFFFFFFF, A, Anum);
	 PolyClip::Render(Vector(0, 0), 0.0f, 0X30FF0000, 0XFFFF0000, C, Cnum);
}

void GameOnKeyCallback(char keypressed)
{
	switch(keypressed)
	{
	case ' ':
		GameInit();
		break;
	}
}


void GameMouse(float x, float y, int buttons)
{
	mousex = x;
	mousey = y;
	mouseb = buttons;
}

