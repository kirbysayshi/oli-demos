#ifndef OLI_GAMECODE_H
#define OLI_GAMECODE_H


/*
#include "GameCode.h"
*/

/*
------------------------------------------------------------------
File: GameCode.h
Started: 05/02/2004 22:59:09
  
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



#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include "Vector.h"
#include "Polygon.h"

extern float  dbg_world_size;

extern void  GameInit			();
extern void  GameUpdate			(float dt);
extern void  GameRender			(void);
extern void  GameOnKeyCallback	(int keypressed);
extern void  GameMouse			(float x, float y, int buttons);
extern void PrintString			(int x, int y, bool pixelcoords, unsigned int rgba, const char* str, ...);



#endif OLI_GAMECODE_H