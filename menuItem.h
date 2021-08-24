//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 13, 2007
//	Last Modifed:	March 15, 2007
//	File:			menuItem.h
//
//	Purpose:		Class definition for cMenuItem, representing a selectable
//					menu item.
//
//***************************************************************************************

#pragma once

#include "menuObject.h"

#define MENU_SUBID 1000

//***************************************************************************************
//
//	Class:		cMenuItem
//	Purpose:	Represents an item within a menu
//
//***************************************************************************************
class cMenuItem
{
public:

	cMenuItem(): mHighlight(false), mConfirm(0), mCancel(0), mNavReturn(0) {}	// Constructor
	~cMenuItem() {}					// Destructor

	virtual void display(int id);	// Displays menu item
	void setMainObject(cMenuObject object);

	virtual bool cursorLeft();		// Moves cursor left within this item (decrement)
	virtual bool cursorRight();		// Moves cursor right within this item (increment)
	virtual int confirm() { return mConfirm; }
	virtual int cancel() { return mCancel; }
	virtual int select(int subid);	// Response to selection (mouse click)
	virtual int keystroke(int key) { return 0; } // Accepts untranslated keystroke

	virtual void highlight() { mHighlight = true; }
	virtual void dehighlight() { mHighlight = false; }

	virtual void refresh() {}

	// Setters
	void setConfirm(int code) { mConfirm = code; }
	void setCancel(int code) { mCancel = code; }
	void setNavReturn(int code) { mNavReturn = code; }
	virtual void enterInputState() { }

	// Getters
	virtual cMenuObject* object(int i) { return &mMainObject; }
	virtual int size() { return 1; }
	int navReturn() { return mNavReturn; }
	float x() { return mMainObject.x(); }
	float y() { return mMainObject.y(); }
	float z() { return mMainObject.z(); }

protected:

	cMenuObject mMainObject;		// Menu's main object

	bool mHighlight;				// Flags whether menu item is highlighted
	int mConfirm;					// Confirm return value
	int mCancel;					// Cancel return value
	int mNavReturn;					// Return value sent upon navigation to this object

};

//***************************************************************************************
//
//	Function:	cursorLeft
//	Purpose:	Moves cursor left within this item (decrement)
//	Returns:	False if cursor cannot be moved
//
//***************************************************************************************
bool cMenuItem::cursorLeft()
{
	return false;
}

//***************************************************************************************
//
//	Function:	cursorRight
//	Purpose:	Moves cursor right within this item (increment)
//	Returns:	False if cursor cannot be moved
//
//***************************************************************************************
bool cMenuItem::cursorRight()
{
	return false;
}

//***************************************************************************************
//
//	Function:	select
//	Purpose:	Response to selection (mouse click)
//
//***************************************************************************************
int cMenuItem::select(int subid)
{
	int returnVal = 0;
	
	if(subid == 0)
	{
		returnVal = mConfirm;
		if(returnVal == 0)
			returnVal = mNavReturn;
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	setMainObject
//	Purpose:	Sets the menu's main object
//
//***************************************************************************************
void cMenuItem::setMainObject(cMenuObject object)
{
	mMainObject = object;
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays all objects within menu item
//
//***************************************************************************************
void cMenuItem::display(int id)
{
	glLoadName(id);
	if(mHighlight)
		mMainObject.highlight();
	else
		mMainObject.display();
}
