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

NOTE : the mesh velocity is not owrking properly. like AT ALL for rotating meshes. 
       I think it's to do with the way the overlap are treated like normal collsions
	   They should be treated differently, like stored in a list, and processed
	   Need to look at FluidStudio's implementation of the swpet volume test, 
	   they can handle moving geometry properly.
------------------------------------------------------------------
$History: $
------------------------------------------------------------------
*/

#include "Vector.h"

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

//--------------------------------------------------------------------------
//
// Game interface
//
//--------------------------------------------------------------------------
extern float  dbg_world_size;

extern void GameInit		();
extern void GameUpdate		(float dt);
extern void GameRender		(void);
extern void GameSetAttractor(const Vector& xAttractor);

//-----------------------------------------------------
// the game data
//-----------------------------------------------------
Vector xAttractor(0, 0);		// attracts the objects

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
	//-----------------------------------------------------
	// compute attractor position under the mouse cursor
	//-----------------------------------------------------
	xAttractor.x = (mouse_x / (float) width) * dbg_world_size;
	xAttractor.y = dbg_world_size - ((mouse_y / (float) height) * dbg_world_size);

	//-----------------------------------------------------
	// set attractor on mouse button, else particles will 
	// fall under gravity
	//-----------------------------------------------------
	if (mouse_b)
		GameSetAttractor(xAttractor);

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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float borders = dbg_world_size /4;
	glOrtho(-borders, 
			 dbg_world_size + borders, 
			-borders, 
			(dbg_world_size + borders), 
			-100, 
			100);
	
	//-----------------------------------------------------------------
	// Setup the model view matrix
	//-----------------------------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	GameRender();
	
	glutSwapBuffers();
}


void PassiveMotion(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	mouse_b = 0;
}

void Motion(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	mouse_b = 1;
}

void Idle()
{
	Update(1.0f / 30.0f);

	Display();
}

void Timer(int t)
{
	Idle();

	glutTimerFunc(t, Timer, (int) 100.0f / 30.0f);
}
	
void Reshape(int w, int h)
{
	width  = w;
	height = h;
	glViewport(	0, 0, w, h);
}

void Keyboard(unsigned char k, int x, int y)
{
	if (k == 27)
		exit(0);

	if (k == ' ') 
		Init();
	else
		Update(1.0f / 30.0f);
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
	glutCreateWindow		("ping pong");
	
	glEnable				(GL_BLEND);
	glBlendFunc				(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable				(GL_DEPTH_TEST);
	glDisable				(GL_LIGHTING);
	
	glutDisplayFunc			(Display);
	glutReshapeFunc			(Reshape);
//	glutIdleFunc			(Idle);
	glutTimerFunc			(0, Timer, (int) 100.0f / 30.0f);
	glutPassiveMotionFunc	(PassiveMotion);
	glutMotionFunc			(Motion);
	glutKeyboardFunc		(Keyboard);

	
	printf("-----------------------------------\n");
	printf("- Soft bodies Verlet Physics Demo -\n");
	printf("-----------------------------------\n");
	printf("by olivier renault\n");
	printf("olivierrenault@hotmail.com\n");
	printf("\n");
	printf("Demonstrates soft body collison and physics\n");
	printf("using particle systems and linear constraints\n");
	printf("\n");
	printf("features\n");
	printf("--------\n");
	printf("Verlet integrator, and stiff linear constraints.\n");
	printf("particle collisions and mesh collisions.\n");
	printf("friction and collision response.\n");
	printf("soft bodies, hard bodies, chains.\n");
	printf("\n");
	printf("controls\n");
	printf("--------\n");
	printf("- keep mouse button pressed, and move mouse to\n");
	printf("  attract bodies\n");
	printf("- release button for normal gravity mode\n");
	printf("- press space bar to initialise\n");
	printf("- press esc to shutdown\n");
	
	Init					();
	glutMainLoop			();
}


