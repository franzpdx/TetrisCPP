//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	May 26, 2007
//	Last Modified:	May 27, 2007
//	File:			cursor.h
//
//	Purpose:		Class which defines a "cursor" object, which indicates the current
//					targeted item on a menu.
//
//***************************************************************************************

#pragma once

#include "object.h"
#include "menu.h"

//***************************************************************************************
//
//	Class:		cCursor
//	Purpose:	Object which localizes to selected menu item
//
//***************************************************************************************
class cCursor
{
public:
	cCursor(cTexture* texture, int* scheme, cMenu* menu, float x, float y, float z, float size):
		mMenu(menu), mCube(x, y, z, size, 0, true, texture, rand() % 7, scheme){}

	~cCursor() {}

	void display();

	void orphan() { mMenu = NULL; }				// Disassociates from menu
	void assign(cMenu* menu) { mMenu = menu; }	// Associates with menu
	void setSpin(int spin) { mCube.setSpin(spin); }	// Alters spinning behavior

private:

	cCubeObject mCube;		// Pointer to cube object
	cMenu* mMenu;			// Pointer to menu

};

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays cursor and updates position
//
//***************************************************************************************
void cCursor::display()
{
	const float zoffset(0.7);
	const float yoffset(-.5);
	const float delay(2.7);

	float xdistance = mMenu->targetX() - mCube.x();
	float ydistance = mMenu->targetY() - yoffset - mCube.y();
	float zdistance = mMenu->targetZ() - zoffset - mCube.z();

	float distance = abs(xdistance) + abs(ydistance) + abs(zdistance);

	mCube.display();

	if(distance < .3)			// Location adjustment for small distances
	{
		if(xdistance > .1)
			xdistance = .1;
		if(ydistance > .1)
			ydistance = .1;
		if(zdistance > .1)
			zdistance = .1;

		mCube.move(xdistance, ydistance, zdistance);
	}
	else						// Location adjustment for larger distances
	{
		mCube.move(xdistance / delay, ydistance / delay, zdistance / delay);
	}
}