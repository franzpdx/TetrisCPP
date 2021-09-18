//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 13, 2007
//	Last Modified:	May 26, 2007
//	File:			menu.h
//	Project:		Blue Tetris
//
//	Purpose:		Class definition for menu class.
//					Responsible for navigation and communication with user when outside
//					of active game state.
//
//***************************************************************************************

#pragma once

// Includes
#include <gl\gl.h>								// Header File For The OpenGL32 Library

#include "menuItem.h"
#include "listItem.h"
#include "iterativeItem.h"
#include "mapItem.h"
#include "stringItem.h"

#include "resource.h"
#include "camera.h"

// Local Defines
#define AUTO_X			-100		// Location of automatic menu objects
#define AUTO_Y			7.25
#define AUTO_Y_SPACING	1.5
#define AUTO_Z			-11
#define AUTO_Z_COL_WIDTH 8
#define AUTO_Z_SPACING	3

#define AUTO_NAV_X	-100			// Auto menu navigation button locations
#define AUTO_NAV_Y	-9
#define AUTO_NAV_Z	-9.5
	
//***************************************************************************************
//
//	Class:		cMenu
//	Purpose:	Contains a set of buttons/options and handles user interaction with
//				such.
//
//***************************************************************************************
class cMenu
{
public:

	cMenu(cKeymap* keymap): mCursor(0),mCancelMap(NULL),mInputMode(false), 
		mTopCoord(AUTO_Y), mKeymap(keymap) {}	// Constructor
	~cMenu();							// Destructor

	void display();						// Displays all menu items
	int advance(vector<int> keys);		// Advances to next frame (keyboard)
	int advance(int coords[]);			// Advances to next frame (mouse)

	void addItem(cMenuItem* item);		// Adds item to menu
	bool setCancelMapping(int index);	// Sets cancel mapping to item of given index
	bool setCursorPosition(int index);	// Sets cursor position
	bool enterInputMode(int index);		// Relocates cursor to given element and begins input mode

	float targetX() { return mItems[mCursor]->x(); }
	float targetY() { return mItems[mCursor]->y(); }
	float targetZ() { return mItems[mCursor]->z(); }

	void refresh();						// Refreshes values in all menu items

	void setAutoTop(float coord) { mTopCoord = coord; }	// Sets coordinate of menu's top for autoformat
	void autoFormat(int navButtons);	// Performs automatic positioning of options

private:

	int click(int x, int y);			// Handles mouseclicks
	void cursorUp();					// Cursor decrement
	void cursorDown();					// Cursor increment
	int cursorRight();
	int cursorLeft();
	int confirm();
	int cancel();
	int keystroke(int key) { return mItems[mCursor]->keystroke(key); }	// Sends untranslated key

	// Member data
	int mCursor;						// Tracks menu item player is hovering over
	cMenuItem* mCancelMap;				// Equates cancel command to menu item selection
	bool mInputMode;					// Flags input mode
	float mTopCoord;					// Coordinate of menu's top for autoformat

	vector<cMenuItem*> mItems;			// Set of items within menu
	cKeymap* mKeymap;					// Keymapping
};

//***************************************************************************************
//
//	Function:	destructor
//	Purpose:	Deallocates memory
//
//***************************************************************************************
cMenu::~cMenu()
{
	for(int i(0); i < mItems.size(); i++)
		delete mItems[i];
}

//***************************************************************************************
//
//	Function:	refesh
//	Purpose:	Refreshes values in all menu items
//
//***************************************************************************************
void cMenu::refresh()
{
	for(int i(0); i < mItems.size(); i++)
		mItems[i]->refresh();
}

//***************************************************************************************
//
//	Function:	setCursorPosition
//	Purpose:	Sets cursor position
//	Return:		True if index is out of bounds
//
//***************************************************************************************
bool cMenu::setCursorPosition(int index)
{
	bool error(false);

	if(index < 0 || index > mItems.size())
		error = true;
	else
	{
		mItems[mCursor]->dehighlight();
		mCursor = index;
		mItems[mCursor]->highlight();
	}

	return error;
}

//***************************************************************************************
//
//	Function:	advance
//	Purpose:	Advances menu one frame; interprets keystrokes
//
//***************************************************************************************
int cMenu::advance(vector<int> keys)
{
	static int last(-1);
	static int count(0);
	int returnVal(0);

	if(keys.size())
	{
		int key = keys[0];
		int command = mKeymap->map(key, S_MENU);

		if(mInputMode)							// If input mode is on
		{
			if(last != key || count > 10)		// Send key; delayed repeat
				returnVal = keystroke(key);		// Directly send key ID
		}
		else if(last != key || count > 10)		// Else (with delayed repeat)
		{
			switch(command)						// Translate and execute command
			{
			case MENU_UP:
				cursorUp();
				break;

			case MENU_DOWN:
				cursorDown();
				break;

			case MENU_RIGHT:
				returnVal = cursorRight();
				break;

			case MENU_LEFT:
				returnVal = cursorLeft();
				break;

			case MENU_CONFIRM:
				if(last != key)
					returnVal = confirm();
				break;

			case MENU_CANCEL:
				if(last != key)
					returnVal = cancel();
				break;

			case FULLSCREEN_TOGGLE:
				if(last != key)
					returnVal = EXECUTE_FULLSCREEN_TOGGLE;
				break;
			};
		}

		if(last == key)						// Repeat tracking
			count++;
		else
		{
			last = key;
			count = 1;
		}
	}
	else
	{
		last = -1;
	}

	if(returnVal == INPUT_MODE_SWITCH)
	{
		mInputMode = !mInputMode;
		returnVal = 0;
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	advance
//	Purpose:	Advances menu one frame; interprets mouseclicks
//
//***************************************************************************************
int cMenu::advance(int coords[])
{
	int returnVal = click(coords[0], coords[1]);

	if(returnVal == INPUT_MODE_SWITCH)
	{
		if(mInputMode)
		{
			mInputMode = false;
		}

		mInputMode = !mInputMode;
		returnVal = 0;
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	confirm
//	Purpose:	Relays confirm message to target menu item
//
//***************************************************************************************
int cMenu::confirm()
{
	return mItems[mCursor]->confirm();
}

//***************************************************************************************
//
//	Function:	cancel
//	Purpose:	Relays cancel message to target menu item
//				If not applicable, checks for cancel mapping
//
//***************************************************************************************
int cMenu::cancel()
{
	int value = 0;

	value = mItems[mCursor]->cancel();				// Send command to target button

	if(value == 0)									// If no local command
		if(mCancelMap)								// Check for mapping
			value = mCancelMap->confirm();			// Execute mapped command

	return value;
}

//***************************************************************************************
//
//	Function:	cursorUp
//	Purpose:	Moves cursor up once (decrement)
//
//***************************************************************************************
void cMenu::cursorUp()
{
	mItems[mCursor]->dehighlight();

	mCursor--;

	if(mCursor < 0)
		mCursor = mItems.size() - 1;

	mItems[mCursor]->highlight();
}

//***************************************************************************************
//
//	Function:	cursorDown
//	Purpose:	Moves cursor down once (increment)
//
//***************************************************************************************
void cMenu::cursorDown()
{
	mItems[mCursor]->dehighlight();

	mCursor++;						// Increment cursor index

	if(mCursor >= mItems.size())	// Rollover if applicable
		mCursor = 0;

	mItems[mCursor]->highlight();
}

//***************************************************************************************
//
//	Function:	cursorRight
//	Purpose:	Moves right within menu item
//
//***************************************************************************************
int cMenu::cursorRight()
{
	int returnval(0);

	if(!mItems[mCursor]->cursorRight())
		returnval = mItems[mCursor]->navReturn();

	return returnval;
}

//***************************************************************************************
//
//	Function:	cursorLeft
//	Purpose:	Moves left within menu item
//
//***************************************************************************************
int cMenu::cursorLeft()
{
	int returnval(0);

	if(!mItems[mCursor]->cursorLeft())
		returnval = mItems[mCursor]->navReturn();

	return returnval;;
}

//***************************************************************************************
//
//	Function:	addItem
//	Purpose:	Adds a menu item to item collection
//
//***************************************************************************************
void cMenu::addItem(cMenuItem* item)
{
	mItems.push_back(item);
	if(mItems.size() == 1)
		mItems[0]->highlight();				// Initialize cursor
}

//***************************************************************************************
//
//	Function:	setCancelMapping
//	Purpose:	Sets menu's global cancel command to specified menu item
//	Return:		True if index is invalid
//
//***************************************************************************************
bool cMenu::setCancelMapping(int index)
{
	bool invalid(false);

	if(index < 0 || index >= mItems.size())
		invalid = true;
	else
		mCancelMap = mItems[index];

	return invalid;
}

//***************************************************************************************
//
//	Function:	enterInputMode
//	Purpose:	Relocates cursor to given item and initiates input mode
//	Returns:	True if invalid index given
//
//***************************************************************************************
bool cMenu::enterInputMode(int index)
{
	bool invalid(false);

	if(index < 0 || index >= mItems.size())
		invalid = true;
	else
	{
		mCursor = index;
		mInputMode = true;
		mItems[index]->enterInputState();
	}

	return invalid;
}

//***************************************************************************************
//
//	Function:	autoFormat
//	Purpose:	Automatically spaces menu items
//				Assumes last two items on list are confirm and cancel
//
//***************************************************************************************
void cMenu::autoFormat(int navButtons = 2)
{
	int size = mItems.size();					// Number of items
	int i, n;									// Index variables
	float z;									// Variable for calculating Z positioning

	for(i = 0; i < size - (navButtons); i++)
	{
		z = AUTO_Z;
		for(n = 0; n < mItems[i]->size(); n++)
		{
			if(n == 1)
				z += AUTO_Z_COL_WIDTH;
			else if(n > 1)
			{
				if(mItems[i]->object(n-1)->width() > AUTO_Z_SPACING)
					z += mItems[i]->object(n-1)->width() + .5;
				else
					z += AUTO_Z_SPACING;
			}

			mItems[i]->object(n)->setCoords(AUTO_X,	mTopCoord - (i * AUTO_Y_SPACING), z);
		}
	}

	for(; i < size; i++)
	{
		mItems[i]->object(0)->setCoords(AUTO_NAV_X, 
			AUTO_NAV_Y + (size - (i + 1)) * AUTO_Y_SPACING, AUTO_NAV_Z);
	}
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays all menu items
//
//***************************************************************************************
void cMenu::display()
{
	int size = mItems.size();

	glDisable(GL_TEXTURE_2D);

	for(int i(0); i < size; i++)
		mItems[i]->display(i);
}

//***************************************************************************************
//
//	Function:	click
//	Purpose:	Handles mouse click event
//	Return:		Return value of clicked object's confirm command (defaults to 0)
//
//***************************************************************************************
int cMenu::click(int x, int y)
{
	cCamera camera(CAMERA_X, CAMERA_Y, CAMERA_Z, CAMERA_ANGLE);

	int returnValue(0);								// Return value
	GLuint	buffer[512];							// Set Up A Selection Buffer
	GLuint	hits;									// Number of intersections

	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);						// Tell OpenGL To Use Our Array For Selection

	// Puts OpenGL In Selection Mode. Nothing Will Be Drawn. Object ID's and Extents Are Stored In The Buffer.
	(void) glRenderMode(GL_SELECT);

	glInitNames();								// Initializes The Name Stack
	glPushName(0);								// Push 0 (At Least One Entry) Onto The Stack

	glMatrixMode(GL_PROJECTION);						// Selects The Projection Matrix
	glPushMatrix();								// Push The Projection Matrix
	glLoadIdentity();							// Resets The Matrix

	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
	gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3]-y), 1.0f, 1.0f, viewport);

	// Apply The Perspective Matrix
	gluPerspective(60.0f, (GLfloat) (viewport[2]-viewport[0])/(GLfloat) (viewport[3]-viewport[1]), 0.1f, 1500.0f);
	
	gluLookAt(camera.x(), camera.y(), camera.z(), camera.xf(), camera.yf(), camera.zf(), 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	display();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();								// Pop The Projection Matrix
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	hits=glRenderMode(GL_RENDER);						// Switch To Render Mode, Find Out How Many

	if (hits > 0)								// If There Were More Than 0 Hits
	{
		int	choose = buffer[3];					// Make Our Selection The First Object
		int	depth = buffer[1];					// Store How Far Away It Is

		for (int loop = 1; loop < hits; loop++)				// Loop Through All The Detected Hits
		{
			// If This Object Is Closer To Us Than The One We Have Selected
			if (buffer[loop*4+1] < GLuint(depth))
			{
				choose = buffer[loop*4+3];			// Select The Closer Object
				depth = buffer[loop*4+1];			// Store How Far Away It Is
			}       
		}

		int item = choose % MENU_SUBID;					// Decode name
		int subitem = choose / MENU_SUBID;

		mItems[mCursor]->dehighlight();
 		mCursor = item;									// Set focus to target
		mItems[mCursor]->highlight();

		returnValue = mItems[item]->select(subitem);	// Execute command on subobject
		if(returnValue == 0)
			returnValue = mItems[item]->navReturn();
	}

	return returnValue;
}
