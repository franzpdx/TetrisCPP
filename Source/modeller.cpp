//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 6, 2007
//	Last Modified:	February 6, 2007
//	File:			modeller.cpp
//	Project:		Blue Tetris
//
//	Purpose:		This is where I make my game not look bad.
//
//***************************************************************************************

// Includes
#define INIT_WIN_WIDTH 800
#define INIT_WIN_HEIGHT 600
#define INIT_WIN_X 3
#define INIT_WIN_Y 23

#include "trisboard.h"
#include "camera.h"
#include "resource.h"

#include <GL/glut.h>		// Glut utilities
#include <stdio.h>			// For printf()
#include <vector>

#define LIGHTINCR		50	// TEMPORARY - light movement increment
#define CAMERA_X		-118
#define CAMERA_Y		0
#define CAMERA_Z		0
#define CAMERA_ANGLE	0
#define LIGHT_X		-200
#define LIGHT_Y		-700
#define LIGHT_Z		850

// Function Prototypes
void HandleDirections(int key, int x, int y);
void HandleKeyboard(unsigned char key, int x, int y);
void Render();
void ChangeSize(GLsizei w, GLsizei h);
void initBoards();

void SpecialHandler(int key, int x, int y);
void KeyHandler(unsigned char key, int x, int y);

cTrisBoard mBoard[4];
int keys[3] = { 0, 0, 0 };

//***************************************************************************************
// Global variables

// Camera
cCamera camera(CAMERA_X, CAMERA_Y, CAMERA_Z, CAMERA_ANGLE);

// Light Position
GLfloat lightPos[] = { LIGHT_X, LIGHT_Y, LIGHT_Z, 1.0f };

//***************************************************************************************
//
//	Function:	initBoardMetrics
//	Purpose:	Initializes board metrics
//
//***************************************************************************************
void initBoardMetrics()
{
	int opponentZ = ENEMYZ;
	int nextZ = ENEMYNEXTZ;

	int n(0);

	for(int i(0); i < 4; i++)
	{
		if(i == 0)
		{
			mBoard[i].setXOrigin(MAINBOARDX);
			mBoard[i].setYOrigin(MAINBOARDY);
			mBoard[i].setZOrigin(MAINBOARDZ);
			mBoard[i].setUnitSize(MAINUNITSIZE);
			mBoard[i].setNextX(MAINNEXTX);
			mBoard[i].setNextY(MAINNEXTY);
			mBoard[i].setNextZ(MAINNEXTZ);
		}
		else
		{
			mBoard[i].setXOrigin(ENEMYX);
			mBoard[i].setYOrigin(ENEMYY);
			mBoard[i].setZOrigin(opponentZ);
			mBoard[i].setUnitSize(ENEMYUNITSIZE);
			mBoard[i].setNextX(ENEMYNEXTX);
			mBoard[i].setNextY(ENEMYNEXTY);
			mBoard[i].setNextZ(nextZ);

			opponentZ += BOARDOFFSET;
			nextZ += BOARDOFFSET;

			n++;
		}

		mBoard[i].setGrid(true);
		mBoard[i].start();
	}
}

//***************************************************************************************
//
//	Function:	SetupRC
//	Purpose:	This function does any needed initialization on the rendering
//				context.  Here it sets up and initializes the lighting for
//				the scene.
//
//***************************************************************************************
void SetupRC()
{
	// Light values and coordinates
    GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat  specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate hiddens surfaces

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set up and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set coloring method
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Make stuff shiney
    glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
    glMateriali(GL_FRONT, GL_SHININESS, 50);

	// Background color
	glClearColor(0, 0, 0, 0);
}

//***************************************************************************************
//
//	Function:	SpecialHandler
//	Purpose:	Moves special key press
//
//***************************************************************************************
void SpecialHandler(int key, int x, int y)
{
	printf("%i ", key);
	int blah;

	if(key == GLUT_KEY_DOWN)
	{
		for(int i(0); i < 4; i++)
			mBoard[i].moveDown(blah);
	}

	if(key == GLUT_KEY_RIGHT)
	{
		for(int i(0); i < 4; i++)
			mBoard[i].moveRight();
	}

	if(key == GLUT_KEY_LEFT)
	{
		for(int i(0); i < 4; i++)
			mBoard[i].moveLeft();
	}
}

//***************************************************************************************
//
//	Function:	KeyHandler
//	Purpose:	Handles alphanumeric key press
//
//***************************************************************************************
void KeyHandler(unsigned char key, int x, int y)
{
	printf("%i ", key);

	if(key == 'd' || key == 'D')
	{
		for(int i(0); i < 4; i++)
			mBoard[i].rotateLeft();
	}

	if(key == 'f' || key == 'F')
	{
		for(int i(0); i < 4; i++)
			mBoard[i].rotateRight();
	}

	if(key == 'q' || key == 'Q')
		glutFullScreen();

	if(key == 'w' || key == 'W')
	{
		glutReshapeWindow(INIT_WIN_WIDTH, INIT_WIN_HEIGHT);
		glutPositionWindow(INIT_WIN_X, INIT_WIN_Y);
	}
}


//***************************************************************************************
//
//	Function:	Render
//	Purpose:	Draws world
//
//***************************************************************************************
void Render()
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();

	gluLookAt(camera.x(), camera.y(), camera.z(), camera.xf(), camera.yf(), camera.zf(),
		0.0, 1.0, 0.0);

	for(int i(0); i < 4; i++)
		mBoard[i].display();

	// Restore the matrix state
	glPopMatrix();
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	// Buffer swap
	glutSwapBuffers();
}

//***************************************************************************************
//
//	Function:	ChangeSize
//	Purpose:	Handles resize of graphic elements when window is resized.
//
//***************************************************************************************
void ChangeSize(GLsizei w, GLsizei h)
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
//	Function:	TimerFunction
//	Purpose:	Posts redisplay message
//
//***************************************************************************************
void TimerFunction(int value)
{
	// Refresh the Window
	glutPostRedisplay();
	// Set timer to call self
	glutTimerFunc(40, TimerFunction, 1);
}

//***************************************************************************************
//
//	Function:	main
//	Purpose:	Initialzes and calls glut main loop
//
//***************************************************************************************
void main(int argc, char *argv[])
{
	initBoardMetrics();

	// Glut initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(INIT_WIN_WIDTH, INIT_WIN_HEIGHT);

	// Create game window
	glutCreateWindow("Blue Tetris");

	// Timer function association
	glutTimerFunc(33, TimerFunction, 1);
	
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyHandler);			// Key readers
	glutSpecialFunc(SpecialHandler);
	glutDisplayFunc(Render);
	SetupRC();
	
	glutMainLoop();						// Enter main loop
}