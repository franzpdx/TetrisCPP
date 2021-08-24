//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	October 30, 2006
//	Last Modified:	November 17, 2006
//	File:			camera.h
//	
//	Purpose:		Class representing a camera for use with Glut
//
//***************************************************************************************

#pragma once

#define	pi	3.141592653589793		// Pi. This is kinda important for angles.

#define MOVEINCREMENT 7.5			// Default increment for forward/back movement
#define STRAFEINCREMENT 5			// Default increment for left/right strafe
#define ROTATEINCREMENT (pi / 30)	// Default increment for rotations

#include <math.h>

class cCamera
{
public:

	// Constructors
	cCamera(): mx(0), my(50), mz(0), mAngle(0) { focus(); }		// Default
	cCamera(float x, float y, float z, float angle);			// Value

	~cCamera() {}						// Default destructor

	void moveBackward(float increment);
	void moveForward(float increment);
	void moveLeft(float increment);
	void moveRight(float increment);
	void rotateLeft(float increment);
	void rotateRight(float increment);

	// Getters
	float x() { return mx; }
	float y() { return my; }
	float z() { return mz; }
	float xf() { return mxf; }
	float yf() { return myf; }
	float zf() { return mzf; }

private:
	void focus();						// Calculates and sets focus location

	float mx;							// Eye position components
	float my;
	float mz;

	float mxf;							// Focus location (derived, not set)
	float myf;
	float mzf;

	float mAngle;						// Rotation angle
};

//***************************************************************************************
//
//	Function:	Constructor
//	Purpose:	Initializes values.
//				Accepts x and z values; y position and angle optional.
//
//***************************************************************************************
cCamera::cCamera(float x, float y, float z, float angle = 0): mx(x), my(y), mz(z),
	mAngle(angle)
{
	focus();
}

//***************************************************************************************
//
//	Function:	moveForward
//	Purpose:	Moves camera forward
//
//***************************************************************************************
void cCamera::moveForward(float increment = MOVEINCREMENT)
{
	mx += increment * cos(mAngle);
	mz += increment * sin(mAngle);
	focus();
}

//***************************************************************************************
//
//	Function:	moveBackward
//	Purpose:	Moves camera backward
//
//***************************************************************************************
void cCamera::moveBackward(float increment = MOVEINCREMENT)
{
	mx -= increment * cos(mAngle);
	mz -= increment * sin(mAngle);
	focus();
}

//***************************************************************************************
//
//	Function:	moveLeft
//	Purpose:	Moves camera left
//
//***************************************************************************************
void cCamera::moveLeft(float increment = STRAFEINCREMENT)
{
	mx -= increment * cos(mAngle + pi / 2);
	mz -= increment * sin(mAngle + pi / 2);
	focus();
}

//***************************************************************************************
//
//	Function:	moveRight
//	Purpose:	Moves camera right
//
//***************************************************************************************
void cCamera::moveRight(float increment = STRAFEINCREMENT)
{
	mx -= increment * cos(mAngle - pi / 2);
	mz -= increment * sin(mAngle - pi / 2);
	focus();
}

//***************************************************************************************
//
//	Function:	rotateLeft
//	Purpose:	Rotates camera left about eye position
//
//***************************************************************************************
void cCamera::rotateLeft(float increment = ROTATEINCREMENT)
{
	mAngle -= increment;
	
	if(mAngle < 0)
		mAngle += 2 * pi;

	focus();
}

//***************************************************************************************
//
//	Function:	rotateRotate
//	Purpose:	Rotates camera right about eye position
//
//***************************************************************************************
void cCamera::rotateRight(float increment = ROTATEINCREMENT)
{
	mAngle += increment;

	if(mAngle > 2 * pi)
		mAngle -= 2 * pi;

	focus();
}

//***************************************************************************************
//
//	Function:	focus
//	Purpose:	Calculates focus location
//
//***************************************************************************************
void cCamera::focus()
{
	myf = my;

	mxf = mx + cos(mAngle);
	mzf = mz + sin(mAngle);
}