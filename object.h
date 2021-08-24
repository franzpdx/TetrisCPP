//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	March 5, 2007
//	Last Modified:	May 26, 2007
//	File:			object.h
//	Project:		Blue Tetris
//
//	Purpose:		cObject class which represents world objects for Blue Tetris,
//					rendered in OpenGL.
//
//					File contains definitions for objects:
//					- cTextObject		(floating text)
//					- cBoolObject		(check mark reflecting a bool)
//					- cImageObject		(static image; unaffected by lighting)
//					- cCubeObject		(Tetris unit)
//
//***************************************************************************************

#pragma once

#include <gl/gl.h>
#include <string.h>
using std::string;

#include "texture.h"
#include "font.h"

//***************************************************************************************
//
//	Class:		cObject
//	Purpose:	Represents an object in OpenGL. Interface class.
//
//***************************************************************************************
class cObject
{
public:
	cObject() {}
	~cObject() {}

	virtual void display() {}
};

//***************************************************************************************
//
//	Class:		cBoolObject
//	Purpose:	Displays character based on state of given boolean value
//
//***************************************************************************************
class cBoolObject : public cObject
{
public:
	cBoolObject(): mRed(1), mGreen(1), mBlue(1), mx(0), my(0), mz(0),
		mxRot(0), myRot(0), mzRot(0), mCenter(false), mValue(NULL) {}
	cBoolObject(bool* value, float x, float y, float z, float red, float green, float blue, bool center);
	~cBoolObject() {}

	void setColor(float red, float blue, float green) { mRed = red; mGreen = green; mBlue = blue; }
	void setXRotation(float rot) { mxRot = rot; }
	void setYRotation(float rot) { myRot = rot; }
	void setZRotation(float rot) { mzRot = rot; }
	void setPosition(float x, float y, float z) { mx = x; my = y; mz = z; }
	void setCentering(bool state) { mCenter = state; }

	virtual void display();

private:

	bool* mValue;					// Pointer to bool
	float mRed, mGreen, mBlue;		// Text color
	float mx, my, mz;				// Text position in world coordinates
	float mxRot, myRot, mzRot;		// Text rotation
	bool mCenter;					// Flags whether text is centered
};

//***************************************************************************************
//
//	Function:	Constructor
//	Purpose:	Initializes values. Font color defaults to 100% white, center to false
//
//***************************************************************************************
cBoolObject::cBoolObject(bool* value, float x, float y, float z, float red = 1.0,
	float green = 1.0, float blue = 1.0, bool center = false): mValue(value),
	mRed(red), mGreen(green), mBlue(blue), mx(x), my(y), mz(z), mxRot(0), myRot(0),
	mzRot(0), mCenter(center)
{
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Renders character if boolean value is true
//
//***************************************************************************************
void cBoolObject::display()
{
	if(mValue)
	{
		if(*mValue == true)
		{
			glDisable(GL_TEXTURE_2D);
			glColor3f(mRed, mGreen, mBlue);
			displayText(mx, my, mz, mxRot, myRot, mzRot, mCenter, "x");
		}
	}
}

//***************************************************************************************
//
//	Class:		cTextObject
//	Purpose:	Renders a block of text
//
//***************************************************************************************
class cTextObject : public cObject
{
public:
	cTextObject(): mRed(1), mGreen(1), mBlue(1), mx(0), my(0), mz(0), mxRot(0), myRot(0), 
		mzRot(0), mCenter(false) {}
	cTextObject(string text, float x, float y, float z, float red, float green, float blue, bool center);
	~cTextObject() {}

	// Setters
	void setText(string text) { mText = text; }
	void setColor(float red, float blue, float green) { mRed = red; mGreen = green; mBlue = blue; }
	void setXRotation(float rot) { mxRot = rot; }
	void setYRotation(float rot) { myRot = rot; }
	void setZRotation(float rot) { mzRot = rot; }
	void setPosition(float x, float y, float z) { mx = x; my = y; mz = z; }
	void setCentering(bool state) { mCenter = state; }

	virtual void display();

protected:

	string mText;					// Text
	float mRed, mGreen, mBlue;		// Text color
	float mx, my, mz;				// Text position in world coordinates
	float mxRot, myRot, mzRot;		// Text rotation
	bool mCenter;					// Flags whether text is centered
};

//***************************************************************************************
//
//	Function:	Constructor
//	Purpose:	Initializes values. Font color defaults to 100% white, center to false
//
//***************************************************************************************
cTextObject::cTextObject(string text, float x, float y, float z, float red = 1.0,
	float green = 1.0, float blue = 1.0, bool center = false): mText(text),
	mRed(red), mGreen(green), mBlue(blue), mx(x), my(y), mz(z), mxRot(0), myRot(0),
	mzRot(0), mCenter(center)
{
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Renders text
//
//***************************************************************************************
void cTextObject::display()
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(mRed, mGreen, mBlue);
	displayText(mx, my, mz, mxRot, myRot, mzRot, mCenter, mText.c_str());
}

//***************************************************************************************
//
//	Class:		cImageObject
//	Purpose:	Renders an object textured with an image
//
//***************************************************************************************
class cImageObject : public cObject
{
public:
	cImageObject(float x1, float y1, float x2, float y2, float z,
		cTexture* textures, int texID): mTexture(textures), mID(texID), mx1(x1),
		my1(y1), mx2(x2), my2(y2), mz(z) {}
	~cImageObject() {}

	virtual void display();

private:

	float mx1, my1, mx2, my2, mz;	// Coordinates of object's corners
	cTexture* mTexture;				// Pointer to texture library
	int mID;						// ID of texture within library

};

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays textured object
//
//***************************************************************************************
void cImageObject::display()
{
	mTexture->bind(mID);

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0); glVertex3f(mz, my1, mx1);		// Awesome methodology
	glTexCoord2f(1, 0); glVertex3f(mz, my1, mx2);
	glTexCoord2f(1, 1); glVertex3f(mz, my2, mx2);
	glTexCoord2f(0, 1); glVertex3f(mz, my2, mx1);

	mTexture->unbind();

	glEnd();

	glEnable(GL_LIGHTING);
}

//***************************************************************************************
//
//	Class:		cCubeObject
//	Purpose:	Renders a tetris unit
//
//***************************************************************************************
class cCubeObject : public cObject
{
public:

	cCubeObject(float x, float y, float z, float size, float rotation, int spinning,
		cTexture* texture, int face, int scheme): mx(x), my(y), mz(z), mSize(size),
		mRot(rotation),	mRot2(0), mSpin(spinning), mTexture(texture), mFace(face), 
		mStaticScheme(scheme), mDynamicScheme(0), mDynamic(false) {}

	cCubeObject(float x, float y, float z, float size, float rotation, int spinning,
		cTexture* texture, int face, int* scheme): mx(x), my(y), mz(z), mSize(size),
		mRot(rotation), mRot2(0), mSpin(spinning), mTexture(texture), mFace(face),
		mDynamicScheme(scheme), mStaticScheme(0), mDynamic(true) {}

	~cCubeObject() {}

	void display();			// Displays cube

	void move(float x, float y, float z)	// Moves cube's location
	{ mx += x; my += y; mz += z; }

	void setSpin(int spinType) { mSpin = spinType; }

	// getters
	float x() { return mx; }
	float y() { return my; }
	float z() { return mz; }

private:

	float mx, my, mz;		// Origin coordinates
	float mSize;			// Size of cube (length of each edge)
	float mRot;				// Rotation (method 1)
	float mRot2;			// Rotation (method 2)

	int mSpin;				// 0 for idle; 1, 2 represent spinning behaviors

	int mFace;				// Cube's appearance/color

	bool mDynamic;			// Flags which scheme is used
	int mStaticScheme;		// Static appearance
	int* mDynamicScheme;	// Dynamic appearance (changes with settings)
	
	cTexture* mTexture;		// Pointer to texture library

};

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Renders cube object; performs rotations when appropriate
//
//***************************************************************************************
void cCubeObject::display()
{

	int scheme;

	if(mDynamic)
		scheme = *mDynamicScheme;
	else
		scheme = mStaticScheme;

	if(mTexture->bind(mFace + scheme * 7))
		glColor3f(RED[scheme][mFace], GREEN[scheme][mFace], BLUE[scheme][mFace]);
	else
		glColor3f(1.0, 1.0, 1.0);

	glPushMatrix();
	
	glTranslatef(mx, my, mz);
	glRotatef(mRot2, 0.0, 1.0, 0.0);
	glRotatef(mRot, 0.3, 0.5, 0.2);
	
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glNormal3f(0, 0, -1);			// Left side
	glTexCoord2f(1, 1); glVertex3f(-mSize/2, mSize/2, -mSize/2);
	glTexCoord2f(0, 1); glVertex3f(mSize/2, mSize/2, -mSize/2);
	glTexCoord2f(0, 0); glVertex3f(mSize/2, -mSize/2, -mSize/2);
	glTexCoord2f(1, 0); glVertex3f(-mSize/2, -mSize/2, -mSize/2);

	glNormal3f(1, 0, 0);			// Back
	glTexCoord2f(1, 1); glVertex3f(mSize/2, mSize/2, -mSize/2);
	glTexCoord2f(0, 1); glVertex3f(mSize/2, mSize/2, mSize/2);
	glTexCoord2f(0, 0); glVertex3f(mSize/2, -mSize/2, mSize/2);
	glTexCoord2f(1, 0); glVertex3f(mSize/2, -mSize/2, -mSize/2);

	glNormal3f(0, 0, 1);			// Right side
	glTexCoord2f(0, 0); glVertex3f(-mSize/2, -mSize/2, mSize/2);
	glTexCoord2f(1, 0); glVertex3f(mSize/2, -mSize/2, mSize/2);
	glTexCoord2f(1, 1); glVertex3f(mSize/2, mSize/2, mSize/2);
	glTexCoord2f(0, 1); glVertex3f(-mSize/2, mSize/2, mSize/2);

	glNormal3f(-1, 0, 0);			// Front
	glTexCoord2f(0, 0); glVertex3f(-mSize/2, -mSize/2, -mSize/2);
	glTexCoord2f(1, 0); glVertex3f(-mSize/2, -mSize/2, mSize/2);
	glTexCoord2f(1, 1); glVertex3f(-mSize/2, mSize/2, mSize/2);
	glTexCoord2f(0, 1); glVertex3f(-mSize/2, mSize/2, -mSize/2);

	glNormal3f(0, 1, 0);			// Top
	glTexCoord2f(0, 0); glVertex3f(-mSize/2, mSize/2, -mSize/2);
	glTexCoord2f(1, 0); glVertex3f(-mSize/2, mSize/2, mSize/2);
	glTexCoord2f(1, 1); glVertex3f(mSize/2, mSize/2, mSize/2);
	glTexCoord2f(0, 1); glVertex3f(mSize/2, mSize/2, -mSize/2);

	glNormal3f(0, -1, 0);			// Bottom
	glTexCoord2f(0, 1); glVertex3f(mSize/2, -mSize/2, -mSize/2);
	glTexCoord2f(0, 0); glVertex3f(mSize/2, -mSize/2, mSize/2);
	glTexCoord2f(1, 0); glVertex3f(-mSize/2, -mSize/2, mSize/2);
	glTexCoord2f(1, 1); glVertex3f(-mSize/2, -mSize/2, -mSize/2);

	mTexture->unbind();

	glEnd();

	glPopMatrix();

	if(mSpin == 1)
	{
		mRot += 2;
		if(mRot >= 360)
			mRot -= 360;
	}
	else if(mSpin == 2)
	{
		mRot2 += 30;
		if(mRot2 >= 360)
			mRot -= 360;
	}
}