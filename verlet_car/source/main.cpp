//=========================================================
//
// Simple Physics Demo
//
//=========================================================
#include "Game.h"

//------------------------------------------------------------------
// cameras
//------------------------------------------------------------------
int screen_width  = 640;	// The width  of the screen in pixels
int screen_height = 480;	// The height of the screen in pixels
int screen_bpp    = 32;		// bits per pixels
int screen_hz     = 60;		// dispaly refresh 

//------------------------------------------------------------------
// input buffer
//------------------------------------------------------------------
int mouse_x		=0;
int mouse_y		=0;
int mouse_button=0;
int old_mouse_x	=0;
int old_mouse_y	=0;
unsigned char key[256];

//--------------------------------------------------------------------------
// the rate of updates (recommended 5fps -> 60 fps for updates). 
//--------------------------------------------------------------------------
float updatefps = 30.0f;

Vector CameraEye;
Vector CameraTarget;
Vector CameraUp;


void Init()
{	
	//--------------------------------------------------------------------------
	// clear key buffer
	//--------------------------------------------------------------------------
	memset(key, 0, sizeof(key));

	CameraEye	 = Vector(0.0f, 0.0f, 0.0f);
	CameraTarget = Vector(0.0f, 0.0f, -1.0f);
	CameraUp	 = Vector(0.0f, 1.0f, 0.0f);

	srand(clock());

	GameInit(Vector(-1000.0f, 0.0f, -1000.0f), Vector(1000.0f, 10.0f, 1000.0f));
}

void Shutdown()
{
	GameShutdown();
}

void UpdateControls()
{
//--------------------------------------------------------------------------
// dirty windows key management
//--------------------------------------------------------------------------
	GetKeyboardState(key);	

	GameSetControls(key, mouse_x, mouse_y, mouse_button);
}

void UpdateCamera()
{
	GameGetCamera(CameraEye, CameraTarget, CameraUp);
}

//--------------------------------------------------------------------------
// update all the spheres independently
//--------------------------------------------------------------------------
void Update(void)
{
	UpdateControls();

	GameUpdate(1.0f / updatefps);

	UpdateCamera();
}


//--------------------------------------------------------------------------
// render simulation
//--------------------------------------------------------------------------
void Render(void)
{
	//--------------------------------------------------------------------------
	// render stuff
	//--------------------------------------------------------------------------
	glClearColor(0.01f, 0.2f, 0.08f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(85.0f, screen_width / (float) screen_height, 0.5f, 20000.0f);

	//-----------------------------------------------------------------
	// Setup the model view matrix
	//-----------------------------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	CameraEye.x   , CameraEye.y   , CameraEye.z, 
				CameraTarget.x, CameraTarget.y, CameraTarget.z, 
				CameraUp.x    , CameraUp.y    , CameraUp.z);
	
	Update();

	GameRender();

	glutSwapBuffers();
}
 

//--------------------------------------------------------------------------
// Update & render simulation
//--------------------------------------------------------------------------
void Idle()
{
	Render();
}


//--------------------------------------------------------------------------
// Update & render simulation
//--------------------------------------------------------------------------
void Timer(int t)
{
	Render();

	glutTimerFunc(t, Timer, (int)(100.0f / updatefps));
}



void Keyboard(unsigned char k, int x, int y)
{
	//---------------------------------------------
	// Initialise the simulation
	//---------------------------------------------
	if (k == 'r' || k == 'R')
		Init();

	if (k == 27)
	{
		Shutdown();
		exit(0);
	}
}

void Mouse(int Button, int State, int x, int y)
{
	mouse_button = 0;

	if (Button == GLUT_LEFT_BUTTON)
	{
		if (State == GLUT_DOWN)
		{
			mouse_button = 1;
		}
	}
	
	if (Button == GLUT_RIGHT_BUTTON)
	{
		if (State == GLUT_DOWN)
		{
			mouse_button = 2;
		}
	}

	old_mouse_y = mouse_y;
	old_mouse_x = mouse_x;

	mouse_y = y;
	mouse_x = x;
}

void Motion(int x, int y)
{
	old_mouse_y = mouse_y;
	old_mouse_x = mouse_x;

	mouse_y = y;
	mouse_x = x;
}
void PassiveMotion(int x, int y)
{
	old_mouse_y = mouse_y;
	old_mouse_x = mouse_x;
	mouse_y = y;
	mouse_x = x;
}

//--------------------------------------------------------------------------
// window management
//--------------------------------------------------------------------------
void Reshape(int w, int h)
{
	screen_width  = w;
	screen_height = h;
	glViewport(	0, 0, screen_width, screen_height);
}

bool EnterWindow()
{
	glutInitWindowSize		(screen_width, screen_height);
	glutInitWindowPosition	(0, 0);
	glutCreateWindow		("car on heightmap demo");
	glutReshapeFunc			(Reshape);
	
	return true;
}

//--------------------------------------------------------------------------
// main loop
//--------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//--------------------------------------------------------------------------
	// OpenGL / GLUT init
	//--------------------------------------------------------------------------
    glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	EnterWindow();

	glShadeModel	(GL_SMOOTH);
	glEnable		(GL_NORMALIZE);
	glEnable		(GL_DEPTH_TEST);
	glDisable		(GL_CULL_FACE);
	glDisable		(GL_LIGHTING);
	glEnable		(GL_BLEND);
	glBlendFunc		(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float GlobalAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float LightAmbient [] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float LightSpecular[] = { 0.7f, 0.1f, 0.7f, 1.0f };
	float LightDiffuse [] = { 0.1f, 0.5f, 0.5f, 1.0f };
	float LightPos     [] = { 1.0f, 0.1f, 0.2f, 0.0f };
	float exp = 120.0f;
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, GlobalAmbient);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  LightSpecular);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, &exp);
	
	glutDisplayFunc			(Render);
	glutTimerFunc			(0, Timer, (int)(1000.0f / updatefps));
	glutKeyboardFunc		(Keyboard);
	glutMouseFunc			(Mouse);
	glutPassiveMotionFunc	(PassiveMotion);
	glutMotionFunc			(Motion);

	Init					();
	glutMainLoop			();
	Shutdown();
	
	return 0;
}
