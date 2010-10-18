/*
------------------------------------------------------------------
File: main.cpp
Started: 09/01/2004 22:21:45
  
$Header: $
$Revision: $
$Locker: $
$Date: $
  
Author: Olivier renault
------------------------------------------------------------------
Module: 
Description: 2D Swept collison etection algorithm. 
             Soldat like response
			 The demo wasn't written for efficiency, but for demonstration
			 It is robust, and also extendable to 3D easily. This is why I am
			 using root solvers, so you'll be to port the algorithm to 3D 
			 straight away. 

  			 - requirements : GLUT library, OpenGL support.

------------------------------------------------------------------
$History: $
------------------------------------------------------------------
*/

#include "GameCode.h"

//--------------------------------------------------------------------------
// mouse coords
//--------------------------------------------------------------------------
float mouse_x = 0.0f;
float mouse_y = 0.0f;
int   mouse_b = 0;

//--------------------------------------------------------------------------
// window size
//--------------------------------------------------------------------------
float width  = 640;
float height = 480;


void PrintString(int x, int y, bool pixelcoords, unsigned int rgba, const char* str, ...)
{
	static char buffer[512];

	va_list params;
	va_start(params, str);
	vsprintf(buffer, str, params);
	va_end(params);

	glPushMatrix();

	glColor4ubv((unsigned char*) &rgba);

	float fx = x;
	float fy = y;

	if (!pixelcoords)
	{
		const float font_width = 8;
		const float font_height = 13;

		x *= (font_width  * dbg_world_size / width);
		y *= (font_height * dbg_world_size / height);
		
		glRasterPos2f(x, dbg_world_size - y);
	}
	else
	{
		glRasterPos2f(x, y);
	}

	char* c = buffer;
	while(*c)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
		c++;
	}

	glPopMatrix();
}

//-----------------------------------------------------
// resets the world with random data
//-----------------------------------------------------
void Init()
{
	GameInit();
}

//-----------------------------------------------------
// Update the objects
//-----------------------------------------------------
void Update(float dt)
{
	GameUpdate(dt);
}

//-----------------------------------------------------
// displays the objects
//-----------------------------------------------------
void Display()
{
	//--------------------------------------------------------------------------
	// render stuff
	//--------------------------------------------------------------------------
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float aspect = 	width / (float) height;


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, dbg_world_size * aspect, 0, dbg_world_size, -100, 100);
	
	//-----------------------------------------------------------------
	// Setup the model view matrix
	//-----------------------------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	float fx = (       (mouse_x / (float) width )) * dbg_world_size * aspect;
	float fy = (1.0f - (mouse_y / (float) height)) * dbg_world_size;

	GameMouse(fx, fy, mouse_b);

	GameUpdate(1.0f / 60.0f);
	
	GameRender();

	glutSwapBuffers();
}


void Mouse(int buttons, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	
	if (buttons == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
			mouse_b |= 1;
		else
			mouse_b &= ~1;
	}

	if (buttons == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
			mouse_b |= 2;
		else
			mouse_b &= ~2;
	}
}
void PassiveMotion(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void Motion(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void Idle()
{
//	Update(1.0f / 60.0f);

	Display();
}

void Timer(int t)
{
	Idle();

	glutTimerFunc(t, Timer, (int) 500.0f / 60.0f);
}
	
void Reshape(int w, int h)
{
	width  = w;
	height = h;
	glViewport(	0, 0, w, h);
}


void Keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);

	GameOnKeyCallback(key);
}

void ExtKeyboard(int key, int x, int y)
{
	GameOnExtKeyCallback(key);

}

void main(int argc, char** argv)
{
	//--------------------------------------------------------------------------
	// OpenGL / GLUT init
	//--------------------------------------------------------------------------
    glutInit( &argc, argv );
	glutInitDisplayMode		(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowSize		(width, height);
	glutInitWindowPosition	(0, 0);
	glutCreateWindow		("Tutorial 5b - Dealing with rotations.exe");
	
	glPointSize(3.0f);
	glEnable				(GL_BLEND);
	glBlendFunc				(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable				(GL_DEPTH_TEST);
	glDisable				(GL_LIGHTING);
	
	glutDisplayFunc			(Display);
	glutReshapeFunc			(Reshape);
//	glutIdleFunc			(Idle);
	glutTimerFunc			(0, Timer, (int) 100.0f / 60.0f);
	glutPassiveMotionFunc	(PassiveMotion);
	glutMouseFunc			(Mouse);
	glutMotionFunc			(Motion);
	glutKeyboardFunc		(Keyboard);
	glutSpecialFunc			(ExtKeyboard);

	Init					();
	glutMainLoop			();
}


