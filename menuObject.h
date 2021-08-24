//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 14, 2007
//	Last Modified:	May 15, 2007
//	File:			menuObject.h
//	Project:		Blue Tetris
//
//	Purpose:		Represents one graphic component of a menu item.
//					Consists of text element and optional surrounding button.
//
//***************************************************************************************

#pragma once

// Includes
#include "font.h"
#include <string>
#include <GL/glut.h>
using std::string;

// Local defines: Colors
#define BUTTON_BORDER_R		0.3		// Standard border color
#define BUTTON_BORDER_G		0.3
#define BUTTON_BORDER_B		0.3

#define BUTTON_HBORDER_R	0.3		// Highlighted border color
#define BUTTON_HBORDER_G	0.3
#define BUTTON_HBORDER_B	0.9

#define BUTTON_TEXT_R		0.6		// Standard text color
#define BUTTON_TEXT_G		0.6
#define BUTTON_TEXT_B		0.6

#define BUTTON_HTEXT_R		1.0		// Highlighted text color
#define BUTTON_HTEXT_G		1.0
#define BUTTON_HTEXT_B		0.9

// Local defines: Metric
#define TEXT_X_OFFSET		-.1		// Position of text relative to button origin
#define TEXT_Y_OFFSET		.3
#define TEXT_Z_OFFSET		.2

#define BUTTON_DEPTH		.1		// Depth of button's border
#define B_BORDER_WIDTH		.1		// Width of button's border

#define NORM_WIDTH			.6
#define NORM_HEIGHT			1

//***************************************************************************************
//
//	Class:		cMenuObject
//	Purpose:	Represents one graphic object within a menu item
//
//***************************************************************************************
class cMenuObject
{
public:

	// Constructors
	cMenuObject(): mxOrigin(0), myOrigin(0), mzOrigin(0), mWidth(NORM_WIDTH), mHeight(NORM_HEIGHT), mBorder(true) {}
	cMenuObject(float x, float y, float z, float width, float height, string text, bool border);
	cMenuObject(float x, float y, float z, string text, bool border);
	cMenuObject(string text, bool border);
	~cMenuObject() {}		// Default destructor

	void display();			// Renders object in OpenGL
	void highlight();		// Special display for highlighted item
	void colorlessDisplay(); // Renders object without changing color

	// Setters
	void setX(float x) { mxOrigin = x; }
	void setY(float y) { myOrigin = y; }
	void setZ(float z) { mzOrigin = z; }
	bool setWidth(float width);
	bool setHeight(float height);
	bool setBorder(bool state) { mBorder = state; }
	void setCoords(float x, float y, float z) { mxOrigin=x; myOrigin=y; mzOrigin=z; }
	void setText(string text, bool resize);
	void setText(char character, bool resize);

	//Getters
	float width() { return mWidth + B_BORDER_WIDTH * 2; }
	float height() { return mHeight + B_BORDER_WIDTH * 2; }
	float x() { return mxOrigin; }
	float y() { return myOrigin; }
	float z() { return mzOrigin; }

private:

	void drawButton();		// Renders button
	void autoSize();		// Automatically resizes button to fit text

	string mText;			// Text displayed on button's face

	float mxOrigin;			// Origin coordinates
	float myOrigin;
	float mzOrigin;

	float mWidth;			// Button metrics
	float mHeight;

	bool mBorder;			// Flag for border display
};

//***************************************************************************************
//
//	Function:	constructor: all variables
//	Purpose:	Sets member data to sent data. Border defaults on.
//
//***************************************************************************************
cMenuObject::cMenuObject(float x, float y, float z, float width, float height, string text, 
						 bool border = true): mxOrigin(x), myOrigin(y), mzOrigin(z), mWidth(width),
						 mHeight(height), mText(text), mBorder(border)
{
}

//***************************************************************************************
//
//	Function:	constructor: automatic sizing
//	Purpose:	Sets member data to sent data. Border defaults on.
//				Automatic sizing of width and height.
//
//***************************************************************************************
cMenuObject::cMenuObject(float x, float y, float z, string text, bool border = true): mxOrigin(x),
myOrigin(y), mzOrigin(z), mText(text), mBorder(border)
{
	autoSize();
}

//***************************************************************************************
//
//	Function:	constructor: text only
//	Purpose:	Sets text of button and optionally border. Border defaults on.
//				Automatic sizing of width and height.
//				Coordinates all default to zero. Intended for use with autoformat.
//
//***************************************************************************************
cMenuObject::cMenuObject(string text, bool border = true): mxOrigin(0),
myOrigin(0), mzOrigin(0), mText(text), mBorder(border)
{
	autoSize();
}

//***************************************************************************************
//
//	Function:	setText
//	Purpose:	Sets button's text. Optionally resizes button (resize by default)
//
//***************************************************************************************
void cMenuObject::setText(string text, bool resize = true)
{
	mText = text;

	if(resize)
		autoSize();
}

void cMenuObject::setText(char character, bool resize = true)
{
	mText = char(character);

	if(resize)
		autoSize();
}

//***************************************************************************************
//
//	Function:	setWidth
//	Purpose:	Sets width of menu item
//	Return:		True if width is out of bounds
//
//***************************************************************************************
bool cMenuObject::setWidth(float width)
{
	bool invalid(false);

	if(width < 0)
		invalid = true;
	else
		mWidth = width;

	return invalid;
}

//***************************************************************************************
//
//	Function:	setHeight
//	Purpose:	Sets height of menu item
//	Return:		True if height is out of bounds
//
//***************************************************************************************
bool cMenuObject::setHeight(float height)
{
	bool invalid(false);

	if(height < 0)
		invalid = true;
	else
		mHeight = height;

	return invalid;
}

//***************************************************************************************
//
//	Function:	autoSize
//	Purpose:	Automatically resizes button width to fit text
//
//***************************************************************************************
void cMenuObject::autoSize()
{
	mHeight = NORM_HEIGHT;
	mWidth = NORM_WIDTH + textLength(mText.c_str());
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Standard display routine for this object
//
//***************************************************************************************
void cMenuObject::display()
{
	glColor3f(BUTTON_TEXT_R, BUTTON_TEXT_G, BUTTON_TEXT_B);
	displayText(mxOrigin + TEXT_X_OFFSET, myOrigin + TEXT_Y_OFFSET, 
		mzOrigin + TEXT_Z_OFFSET, 0, 0, 0, false, mText.c_str());

	if(mBorder)								// Border display
	{
		glColor3f(BUTTON_BORDER_R, BUTTON_BORDER_G, BUTTON_BORDER_B);
		drawButton();
	}
}

//***************************************************************************************
//
//	Function:	colorlessDisplay
//	Purpose:	Normal display routine; draws without changing color settings
//
//***************************************************************************************
void cMenuObject::colorlessDisplay()
{
	glDisable(GL_TEXTURE_2D);

	if(mBorder)								// Border display
	{
		drawButton();
	}

	glColor3f(BUTTON_HTEXT_R, BUTTON_HTEXT_G, BUTTON_HTEXT_B);
	displayText(mxOrigin + TEXT_X_OFFSET, myOrigin + TEXT_Y_OFFSET, 
		mzOrigin + TEXT_Z_OFFSET, 0, 0, 0, false, mText.c_str());
}

//***************************************************************************************
//
//	Function:	highlight
//	Purpose:	Special display routine for highlighted object
//
//***************************************************************************************
void cMenuObject::highlight()
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(BUTTON_HTEXT_R, BUTTON_HTEXT_G, BUTTON_HTEXT_B);	

	displayText(mxOrigin + TEXT_X_OFFSET, myOrigin + TEXT_Y_OFFSET, 
				mzOrigin + TEXT_Z_OFFSET, 0, 0, 0, false, mText.c_str());

	if(mBorder)								// Border display
	{
		glColor3f(BUTTON_HBORDER_R, BUTTON_HBORDER_G, BUTTON_HBORDER_B);

		drawButton();
	}
}

//***************************************************************************************
//
//	Function:	drawButton
//	Purpose:	Draws button. Surprise.
//
//***************************************************************************************
void cMenuObject::drawButton()
{
	glBegin(GL_QUADS);

	glNormal3f(-1, 0, 0);	// Surface facing toward -x
	glVertex3f(mxOrigin, myOrigin + B_BORDER_WIDTH, mzOrigin + mWidth - B_BORDER_WIDTH);
	glVertex3f(mxOrigin, myOrigin + mHeight - B_BORDER_WIDTH, mzOrigin + mWidth - B_BORDER_WIDTH);
	glVertex3f(mxOrigin, myOrigin + mHeight - B_BORDER_WIDTH, mzOrigin + B_BORDER_WIDTH);
	glVertex3f(mxOrigin, myOrigin + B_BORDER_WIDTH, mzOrigin + B_BORDER_WIDTH);

	glNormal3f(0, 0, 1);	// Left border
	glVertex3f(mxOrigin, myOrigin + B_BORDER_WIDTH, mzOrigin + B_BORDER_WIDTH);
	glVertex3f(mxOrigin, myOrigin + mHeight - B_BORDER_WIDTH, mzOrigin + B_BORDER_WIDTH);
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin + mHeight, mzOrigin);
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin, mzOrigin);

	glNormal3f(0, 0, -1);	// Right border
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin + mHeight, mzOrigin + mWidth);
	glVertex3f(mxOrigin, myOrigin + mHeight - B_BORDER_WIDTH, mzOrigin + mWidth - B_BORDER_WIDTH);
	glVertex3f(mxOrigin, myOrigin + B_BORDER_WIDTH, mzOrigin + mWidth - B_BORDER_WIDTH);
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin, mzOrigin + mWidth);

	glNormal3f(0, 1, 0);	// Bottom border
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin, mzOrigin);
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin, mzOrigin + mWidth);
	glVertex3f(mxOrigin, myOrigin + B_BORDER_WIDTH, mzOrigin + mWidth - B_BORDER_WIDTH);
	glVertex3f(mxOrigin, myOrigin + B_BORDER_WIDTH, mzOrigin + B_BORDER_WIDTH);

	glNormal3f(0, -1, 0);	// Top border
	glVertex3f(mxOrigin, myOrigin + mHeight - B_BORDER_WIDTH, mzOrigin + mWidth - B_BORDER_WIDTH);
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin + mHeight, mzOrigin + mWidth);
	glVertex3f(mxOrigin + BUTTON_DEPTH, myOrigin + mHeight, mzOrigin);
	glVertex3f(mxOrigin, myOrigin + mHeight - B_BORDER_WIDTH, mzOrigin + B_BORDER_WIDTH);
	
	glEnd();
}