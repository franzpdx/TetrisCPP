//***************************************************************************************
//
//	Authors:		Tom Franz
//	Date Created:	November 6, 2006
//	Last Modified:	May 25, 2007
//	File:			main.cpp
//	Project:		Blue Tetris
//
//	Purpose:		Runs Blue Tetris
//
//***************************************************************************************

// Eliminiate console window
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

// Includes
#include "blueTetris.h"
#include "camera.h"
#include "resource.h"
#include "texture.h"

// Function Prototypes
void changeSize(GLsizei w, GLsizei h);
void draw();
bool init();
void loadTextures();

void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void click(int button, int state, int x, int y);

//***************************************************************************************
// Global variables

// Camera
cCamera camera(CAMERA_X, CAMERA_Y, CAMERA_Z, CAMERA_ANGLE);

// Light Position
GLfloat lightPos[] = { LIGHT_X, LIGHT_Y, LIGHT_Z, 1.0f };

// Game object
cBlueTetris tetris;

// Global variables
vector<int> keys;				// List of keys being pressed
int coords[2];					// x, y coords of mouse click
bool mouseClick(false);			// Flags when mouse click is recieved
cTexture texture;				// Textures

//***************************************************************************************
//
//	Function:	init
//	Purpose:	Initializes glut settings
//
//***************************************************************************************
bool init()
{
	loadTextures();

	// Light values and coordinates
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat  specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Enables
	glEnable(GL_TEXTURE_2D);							// Texture mapping
	glEnable(GL_DEPTH_TEST);							// Depth testing
	glEnable(GL_LIGHTING);								// Lighting
	glEnable(GL_COLOR_MATERIAL);						// Coloring
	glEnable(GL_DEPTH_TEST);							// Hidden surface removal
	glFrontFace(GL_CCW);								// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);								// Do not calculate hiddens surfaces
	
	glShadeModel(GL_SMOOTH);							// Smooth shading
	glClearDepth(1.0f);									// Depth buffer setup
	glDepthFunc(GL_LEQUAL);								// Depth testing method
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Perspective calculation settings

	// Apperance properties
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);		// Make stuff shiny
	glMateriali(GL_FRONT, GL_SHININESS, 50);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	// Set coloring method

	// Set up and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);

	// Backgound color
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

	return TRUE;										// Initialization Went OK
}

//***************************************************************************************
//
//	Function:	specialDown
//	Purpose:	Moves special key press
//
//***************************************************************************************
void specialDown(int key, int x, int y)
{
	key += SPECIALKEY_OFFSET;	// Signify special key
	int size = keys.size();
	bool present(false);

	for(int i(0); i < size && !present; i++)
		if(keys[i] == key)
			present = true;

	if(!present)
		keys.push_back(key);
}

//***************************************************************************************
//
//	Function:	specialUp
//	Purpose:	Handles special key release
//
//***************************************************************************************
void specialUp(int key, int x, int y)
{
	key += SPECIALKEY_OFFSET;	// Signify special key
	int size = keys.size();
	int i, n;
	bool present(false);

	for(i = 0; i < size && !present; i++)
	{
		if(keys[i] == key)
		{
			n = i;
			present = true;
		}
	}

	if(present)
	{
		keys[n] = keys[size - 1];
		keys.pop_back();
	}
}

//***************************************************************************************
//
//	Function:	keyDown
//	Purpose:	Handles alphanumeric key press
//
//***************************************************************************************
void keyDown(unsigned char key, int x, int y)
{
	int size = keys.size();
	bool present(false);

	for(int i(0); i < size && !present; i++)
		if(keys[i] == key)
			present = true;

	if(!present)
		keys.push_back(key);
}

//***************************************************************************************
//
//	Function:	keyUp
//	Function:	Handles alphanumeric key release
//
//***************************************************************************************
void keyUp(unsigned char key, int x, int y)
{
	int size = keys.size();
	int pressed;
	int i, n;
	bool present(false);

	for(i = 0; i < size && !present; i++)
	{
		pressed = keys[i];

		if(pressed == key			// Check for key, upper and lower cases
			|| ((pressed >= 65 && pressed <= 90) && key == pressed + 32)
			|| ((pressed >= 97 && pressed <= 122) && key == pressed - 32) )
		{
			n = i;
			present = true;
		}
	}

	if(present)
	{
		keys[n] = keys[size - 1];
		keys.pop_back();
	}
}

//***************************************************************************************
//
//	Function:	click
//	Purpose:	Handles mouseclick
//
//***************************************************************************************
void click(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		coords[0] = x;
		coords[1] = y;
		mouseClick = true;
	}
}

//***************************************************************************************
//
//	Function:	draw
//	Purpose:	Draws world
//
//***************************************************************************************
void draw()
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();							// Resets The Matrix

	gluLookAt(camera.x(), camera.y(), camera.z(), camera.xf(), camera.yf(), camera.zf(), 0.0, 1.0, 0.0);
	tetris.display();

	// Restore the matrix state
	glPopMatrix();

	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	// Buffer swap
	glutSwapBuffers();
}

//***************************************************************************************
//
//	Function:	changeSize
//	Purpose:	Handles resize of graphic elements when window is resized.
//
//***************************************************************************************
void changeSize(GLsizei w, GLsizei h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	fAspect = (GLfloat)w/(GLfloat)h;

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Produce the perspective projection
	gluPerspective(60.0f, fAspect, 1.0, 1500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
}

//***************************************************************************************
//
//	Function:	timerFunction
//	Purpose:	Posts redisplay message
//
//***************************************************************************************
void timerFunction(int value)
{
	if(tetris.advance(keys, coords, mouseClick))
	{
		exit(0);		// Force program exit (because glutMainLoop() is retarded)
	}
	else
	{
		// Refresh the Window
		glutPostRedisplay();
		// Set timer to call self
		glutTimerFunc(30, timerFunction, 1);

		mouseClick = false;
	}
}

//***************************************************************************************
//
//	Function:	main
//	Purpose:	Initialzes and calls glut main loop
//
//***************************************************************************************
void main(int argc, char *argv[])
{
	try
	{
		// Glut initialization
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(INIT_WIN_WIDTH, INIT_WIN_HEIGHT);

		// Create game window
		glutCreateWindow("Blue Tetris");

		glutTimerFunc(33, timerFunction, 1);

		glutReshapeFunc(changeSize);		// Rendering funcitons
		glutDisplayFunc(draw);

		glutKeyboardFunc(keyDown);			// Input functions
		glutKeyboardUpFunc(keyUp);
		glutSpecialFunc(specialDown);
		glutSpecialUpFunc(specialUp);
		glutMouseFunc(click);

		init();
		loadFont();

		tetris.setTexture(texture);
		tetris.init();

		glutMainLoop();						// Enter main loop
	}
	catch(...)
	{
		tetris.~cBlueTetris();
	}
}

//***************************************************************************************
//
//	Function:	loadTextures
//	Purpose:	Loads textures used by Blue Tetris
//
//***************************************************************************************
void loadTextures()
{
	// Load tetris unit skins
	texture.loadTexture("Bitmaps/classici.bmp");	// Classic Skin
	texture.loadTexture("Bitmaps/classico.bmp");
	texture.loadTexture("Bitmaps/classicl.bmp");
	texture.loadTexture("Bitmaps/classicj.bmp");
	texture.loadTexture("Bitmaps/classics.bmp");
	texture.loadTexture("Bitmaps/classicz.bmp");
	texture.loadTexture("Bitmaps/classict.bmp");
	texture.loadTexture("Bitmaps/moderni.bmp");		// Modern Skin
	texture.loadTexture("Bitmaps/moderno.bmp");
	texture.loadTexture("Bitmaps/modernl.bmp");
	texture.loadTexture("Bitmaps/modernj.bmp");
	texture.loadTexture("Bitmaps/moderns.bmp");
	texture.loadTexture("Bitmaps/modernz.bmp");
	texture.loadTexture("Bitmaps/modernt.bmp");
	texture.loadTexture("Bitmaps/gameboyi.bmp");	// Gameboy Skin
	texture.loadTexture("Bitmaps/gameboyo.bmp");
	texture.loadTexture("Bitmaps/gameboyl.bmp");
	texture.loadTexture("Bitmaps/gameboyj.bmp");
	texture.loadTexture("Bitmaps/gameboys.bmp");
	texture.loadTexture("Bitmaps/gameboyz.bmp");
	texture.loadTexture("Bitmaps/gameboyt.bmp");
	texture.loadTexture("Bitmaps/credits.bmp");		// Splash Screen
	texture.loadTexture("Bitmaps/mariner10.bmp");	// Background 1

	texture.unbind();
}