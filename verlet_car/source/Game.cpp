//------------------------------------------------------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// File          : Game.cpp
// 
// Created by    : OR - 11/02/2004 12:51:04
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



#include "Game.h"
#include "Car.h"

CHeightmap* gpxHeightmap=NULL;

enum { eNumCars = 12 };

CCar* gpxCars[eNumCars] = { 0 };

void GameInit(const Vector& xWorldMin, const Vector& xWorldMax)
{
	srand(clock());

	GameShutdown();

	gpxHeightmap = new CHeightmap();

	for(int i = 0; i < eNumCars; i ++)
	{
		gpxCars[i] = new CCar();
	}
}

void GameShutdown()
{
	if (gpxHeightmap)
		delete gpxHeightmap;
	gpxHeightmap = NULL;

	for(int i = 0; i < eNumCars; i ++)
	{
		if (gpxCars[i])
			delete gpxCars[i];
		gpxCars[i] = NULL;
	}

}

void GameUpdateCar(float dt, CCar& Car)
{
	Car.UpdateParticles(dt);

	if (gpxHeightmap)
		Car.Collide(*gpxHeightmap);

	for (int i = 0; i < 6; i ++)
	{
		if (gpxCars[i] == &Car)
			continue;

		if (gpxCars[i])
			gpxCars[i]->Constrain(Car);
	}

	for(int i = 0; i < 5; i ++)
	{
		Car.SatisfyConstraints();
	}
}

void GameUpdate(float dt)
{	
	if (!gpxHeightmap)
		return;

	for(int i = 0; i < eNumCars; i ++)
	{
		if (gpxCars[i])
			GameUpdateCar(dt, *gpxCars[i]);
	}
}

void GameRender()
{
	if (gpxHeightmap)
		gpxHeightmap->Render();

	for(int i = 0; i < eNumCars; i ++)
	{
		if (gpxCars[i])
			gpxCars[i]->Render();
	}
}


float r = 2.0f;
float a = 0.0f;
float b = 0.0f;
int iCurrentCar = 0;


char keybuf[2][256];
int ibuf = 0;

bool WasKeyPressed		(char key) { return (keybuf[1-ibuf][key] & (1 << 7)) != 0; }
bool KeyPressed		    (char key) { return (keybuf[ibuf  ][key] & (1 << 7)) != 0; }
bool DebouncedKeyPressed(char key) { return KeyPressed(key) && !WasKeyPressed(key);}

void GameSetControls(unsigned char* key, int mouse_x, int mouse_y, int mouse_b)
{

	memcpy(keybuf[ibuf], key, 256);
	
	if (DebouncedKeyPressed(' '))
	{
		iCurrentCar = (iCurrentCar + 1) % eNumCars;
	}

	r += KeyPressed(VK_CAPITAL)? 0.1f : KeyPressed(VK_TAB)? -0.1f : 0.0f;
	a += KeyPressed('A'		  )? 0.1f : KeyPressed('D'   )? -0.1f : 0.0f;
	b += KeyPressed('S'       )? 0.1f : KeyPressed('W'   )? -0.1f : 0.0f;
		
	const float two_pi = atan(1.0f) * 8.0f;

	r = (r < 1.0f)? 1.0f : (r > 30.0f)? 30.0f : r;
	a = (a < 0.0f)? a + two_pi : (a > two_pi)? a - two_pi : a;
	b = (b < 0.0f)? 0.0f       : (b > two_pi / 4)? two_pi/4 : b;


	// HACK : random steering, except for car 1, which is player's car
	static int icount = 0;
	static float fCarSteering[eNumCars];
	static float fCarThrottle[eNumCars];

	if (icount == 0)
	{
		memset(fCarSteering, 0, sizeof(fCarSteering));
		memset(fCarThrottle, 0, sizeof(fCarThrottle));
	}

	if (gpxCars[0])
	{
		float fThrottle = KeyPressed(VK_UP  )?  1.0f : KeyPressed(VK_DOWN )? -0.3f : 0.0f;
		float fSteering = KeyPressed(VK_LEFT)? -1.0f : KeyPressed(VK_RIGHT)?  1.0f : 0.0f;

		fCarSteering[0] += (fSteering - fCarSteering[0]) * 0.2f;
		fCarThrottle[0] += (fThrottle - fCarThrottle[0]) * 0.2f;

		gpxCars[0]->Accelerate(fCarThrottle[0]);
		gpxCars[0]->Steer     (fCarSteering[0]);
	}

	for(int i = 1; i < eNumCars; i ++)
	{
		if ((icount + i - 1) % (eNumCars-1))
			continue;

		if (gpxCars[i])
		{
			fCarSteering[i] = frand(0.5f) + 0.5f;
			fCarThrottle[i] = frand(2.0f) - 1.0f;

			gpxCars[i]->Accelerate(fCarSteering[i]);
			gpxCars[i]->Steer     (fCarThrottle[i]);
		}
	}
	icount++;
	// END HACK


	ibuf = 1 - ibuf;
}

void GameGetCamera (Vector& CameraEye, Vector& CameraTarget, Vector& CameraUp)
{
	Vector D;
	D.x = cos(a) * cos(b);
	D.y = sin(b);
	D.z = sin(a) * cos(b);

	Vector S;
	S.x = -sin(a);
	S.y = 0.0f;
	S.z = cos(a);

	Vector U = S ^ D;
	
	if (gpxCars[iCurrentCar])
	{
		Vector P	 = gpxCars[iCurrentCar]->GetCentre();
		CameraEye    = P + D * r;
		CameraTarget = P;
		CameraUp     = U;
	}
	else
	{
		CameraEye    = Vector(64.0f, 32.0f, 64.0f);
		CameraTarget = Vector(64.0f, 0.0f, 64.0f);
		CameraUp     = Vector(0, 0, 1);
	}
}
