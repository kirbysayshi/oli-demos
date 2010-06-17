#ifndef OLI_INCLUDES_H
#define OLI_INCLUDES_H


/*
#include "includes.h"
*/

/*
------------------------------------------------------------------
File: includes.h
Started: 22/11/2003 22:42:57
  
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


#include <math.h>
#include <conio.h>
#include <math.h>
#include <time.h>		// 3x3 matrix class
#include <stdio.h>		// 3x3 matrix class
#include <stdlib.h>		// 3x3 matrix class
#include <gl/glut.h>
#include <windows.h>
#include <winuser.h>
#include "Vector.h"

//------------------------------------------------------------------------------------------------ 
////////////////////////////////////////////////////////////////////////////////////////////////// 
// 
// OR [11/02/2004 ] : Game interface
// 
////////////////////////////////////////////////////////////////////////////////////////////////// 
//------------------------------------------------------------------------------------------------ 
extern void GameInit			(const Vector& xWorldMin, const Vector& xWorldMax);
extern void	GameShutdown		();
extern void GameUpdate			(float dt);
extern void GameRender			();
extern void GameSetControls		(unsigned char* keys, int mouse_x, int mouse_y, int mouse_b);
extern void GameGetCamera		(Vector& CameraEye, Vector& CameraTarget, Vector& CameraUp);


#endif //OLI_INCLUDES_H
