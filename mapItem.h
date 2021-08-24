//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	March 2, 2007
//	Last Modified:	March 5, 2007
//	File:			mapItem.h
//	Project:		Blue Tetris
//	
//	Purpose:		Class definition for mapItem class, which allows configuration of
//					keymap elements through menu items
//
//***************************************************************************************

#pragma once

#include "menuObject.h"
#include "keymap.h"
#include "resource.h"

//***************************************************************************************
//
//	Class:		cMapItem
//	Purpose:	Menu item type which allows user to configure the keymap class settings
//
//***************************************************************************************
class cMapItem: public cMenuItem
{
public:

	cMapItem(): mKeymap(NULL), mDisplayState(0), mAction(-1) {}	// Default constructor
	~cMapItem();												// Default destructor

	virtual void display(int id);

	void setVariableObject(cMenuObject object) { mVariableObject = object; }

	virtual int confirm();			// Response to confirm button
	virtual int select(int subid);	// Response to selection (mouse click)
	virtual int keystroke(int key);	// Response to direct input

	virtual void dehighlight() {mHighlight = false; mDisplayState = 0; }

	// Setters
	void setKeymap(cKeymap* map) { mKeymap = map; }	// Sets keymap pointer
	void setAction(int action) { mAction = action; updateItem(); } // Sets associated action

	// Getters
	virtual cMenuObject* object(int i) { if(i < 1) return &mMainObject; else return &mVariableObject; }
	virtual int size() { return 2; }

private:

	void updateItem();				// Automatically configures both objects
	void updateObject();			// Updates text on variable object
	
	string keyToString(int key);	// Converts key to string

	cMenuObject mVariableObject;	// Variable menu object (dispays selected key)

	cKeymap* mKeymap;				// Pointer to keymap

	int mAction;					// Key's action
	int mDisplayState;				// State of button's display (Normal/Input/Error)
};

//***************************************************************************************
//
//	Function:	select
//	Purpose:	Response to mouse click
//
//***************************************************************************************
int cMapItem::select(int subid)
{
	mDisplayState = 1;
	return INPUT_MODE_SWITCH;
}

//***************************************************************************************
//
//	Function:	confirm
//	Purpose:	Response to confirm keystroke
//
//***************************************************************************************
int cMapItem::confirm()
{
	mDisplayState = 1;
	return INPUT_MODE_SWITCH;
}

//***************************************************************************************
//
//	Function:	keystroke
//	Purpose:	Maps keystroke to action
//
//***************************************************************************************
int cMapItem::keystroke(int key)
{
	if(mKeymap)
	{
		if(mKeymap->set(key, mAction))
		{
			mDisplayState = 2;
		}
		else
		{
			updateObject();
			mDisplayState = 0;
		}
	}

	return INPUT_MODE_SWITCH;
}

//***************************************************************************************
//
//	Function:	updateItem
//	Purpose:	Updates text in main object and variable object
//
//***************************************************************************************
void cMapItem::updateItem()
{
	string text;

	switch(mAction)
	{
	case FULLSCREEN_TOGGLE:
		text = "Fullscreen Toggle";
		break;

	case MENU_UP:
		text = "Up";
		break;

	case MENU_DOWN:
		text = "Down";
		break;

	case MENU_LEFT:
		text = "Left";
		break;

	case MENU_RIGHT:
		text = "Right";
		break;

	case MENU_CONFIRM:
		text = "Confirm";
		break;

	case MENU_CANCEL:
		text = "Cancel";
		break;

	case GAME_SONIC_LOCK:
		text = "Sonic Drop";
		break;

	case GAME_DOWN:
		text = "Drop";
		break;

	case GAME_RIGHT:
		text = "Move Right";
		break;

	case GAME_LEFT:
		text = "Move Left";
		break;

	case GAME_ROTATE_RIGHT:
		text = "Rotate Right";
		break;

	case GAME_ROTATE_LEFT:
		text = "Rotate Left";
		break;

	case GAME_PAUSE:
		text = "Pause";
		break;
	};

	mMainObject.setText(text, false);
	updateObject();
}

//***************************************************************************************
//
//	Function:	updateObject
//	Purpose:	Updates text in the variable object
//
//***************************************************************************************
void cMapItem::updateObject()
{
	if(mKeymap)
	{
		int key = mKeymap->retrieve(mAction);			// Retrieve mapped key
		if(key != -1)									// Unless invalid command code
		{
			string text = keyToString(key);				// Make ledgible
			mVariableObject.setText(text, false);		// Update text
		}
	}
}

//***************************************************************************************
//
//	Function:	keyToString
//	Purpose:	Converts key that is good for computers
//				to string that is good for humans
//	Returns:	String explaining key
//
//***************************************************************************************
string cMapItem::keyToString(int key)
{
	string text;						// Return string
	char buff[255];						// For sprintf shortcuts

	if(key > 32 && key <= 126)			// Printable characters
		text = (char)key;
	else if(key >= 1 + SPECIALKEY_OFFSET && key <= 12 + SPECIALKEY_OFFSET)	// F1 - F12
	{
		sprintf(buff, "F%i", key - SPECIALKEY_OFFSET);
		text = buff;
	}
	else
	{
		switch (key)
		{
		case 103 + SPECIALKEY_OFFSET:			// Arrow keys
			text = "Down";
			break;

		case 101 + SPECIALKEY_OFFSET:
			text = "Up";
			break;

		case 100 + SPECIALKEY_OFFSET:
			text = "Left";
			break;

		case 102 + SPECIALKEY_OFFSET:
			text = "Right";
			break;

		case 32:								// Noncharacter keys
			text = "Space";
			break;

		case 27:
			text = "Esc";
			break;

		case 13:
			text = "Enter";
			break;

	
		case 127:
			text = "Del";
			break;

		case 107 + SPECIALKEY_OFFSET:
			text = "End";
			break;

		case 106 + SPECIALKEY_OFFSET:
			text = "Home";
			break;
			
		case 104 + SPECIALKEY_OFFSET:
			text = "Pg Up";
			break;

		case 105 + SPECIALKEY_OFFSET:
			text = "Pg Down";
			break;

		case 108 + SPECIALKEY_OFFSET:
			text = "Ins";
			break;

		default:
			sprintf(buff, "[%i]", key % SPECIALKEY_OFFSET);
			text = buff;
		};
	}

	return text;
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays menu item
//
//***************************************************************************************
void cMapItem::display(int id)
{
	if(mHighlight)
	{
		if(mDisplayState == 0)					// Normal highlighted display
		{
			glLoadName(id);
			mMainObject.highlight();
			glLoadName(id + 1);
			mVariableObject.highlight();
		}
		else if(mDisplayState == 1)				// Active display
		{
			glLoadName(id);
			mMainObject.highlight();
			glLoadName(id);
			glColor3f(0.5, 0.1, 0.5);
			mVariableObject.colorlessDisplay();
		}
		else if(mDisplayState == 2)				// Error display
		{
			glLoadName(id);
			mMainObject.highlight();
			glLoadName(id);
			glColor3f(1.0, 0.1, 0.1);
			mVariableObject.colorlessDisplay();
		}
	}
	else										// Normal idle display
	{
		glLoadName(id);
		mMainObject.display();
		glLoadName(id);
		mVariableObject.display();
	}
}