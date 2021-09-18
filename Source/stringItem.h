//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	March 8, 2007
//	Last Modified:	March 8, 2007
//	File:			stringItem.h
//	Project:		Blue Tetris
//
//	Purpose:		Menu item pointing to a string that can be edited by the user
//
//***************************************************************************************

#pragma once

#include <string>
using std::string;

#include "menuObject.h"
#include "resource.h"

//***************************************************************************************
//
//	Class:		cStringItem
//	Purpose:	Menu item type which points to a string and allows user to edit it
//
//***************************************************************************************
class cStringItem: public cMenuItem
{
public:

	cStringItem(): mString(NULL), mDisplayState(0), mStringReturnSet(false), 
		mStringReturn(0), mStringLength(0), mStringLengthSet(false) {}
	~cStringItem() {}

	virtual void display(int id);

	virtual int confirm();			// Response to confirm button
	virtual int select(int subid);	// Response to selection (mouse click)
	virtual int keystroke(int key);	// Response to direct input

	virtual void enterInputState() { mDisplayState = 1; }
	virtual void dehighlight() {mHighlight = false; mDisplayState = 0; }

	// Setters
	void setString(string* str) { mString = str; updateObject(); }	// Sets keymap pointer
	void setStringLength(int length) { mStringLength = length; mStringLengthSet = true; }
	void setStringReturn(int value) { mStringReturn = value; mStringReturnSet = true; }
	void clearText() { *mString = ""; }	// Clears member string

	// Getters
	virtual cMenuObject* object(int i) { return &mMainObject; }
	virtual int size() { return 1; }

private:

	void updateObject();			// Updates text on variable object
	bool checkValidity(int key);	// Checks validity of key in context to text

	string* mString;				// Pointer to editable string

	int mAction;					// Key's action
	int mDisplayState;				// State of button's display (Normal/Input)
	int mStringLength;				// Limitation on string length
	bool mStringLengthSet;			// Flags when length limit is applied
	int mStringReturn;				// Special value returned upon end of string input
	bool mStringReturnSet;			// Flags when special return is set
};

//***************************************************************************************
//
//	Function:	select
//	Purpose:	Response to mouse click
//
//***************************************************************************************
int cStringItem::select(int subid)
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
int cStringItem::confirm()
{
	if(mConfirm)
		return mConfirm;
	else
		mDisplayState = 1;
	return INPUT_MODE_SWITCH;
}

//***************************************************************************************
//
//	Function:	keystroke
//	Purpose:	Maps keystroke to action
//
//***************************************************************************************
int cStringItem::keystroke(int key)
{
	int returnVal = 0;

	if(key == 13 || key == 27)					// Exit upon enter or esc
	{
		mDisplayState = 0;
		updateObject();

		if(mStringReturnSet)
			returnVal = mStringReturn;
		else
			returnVal =  INPUT_MODE_SWITCH;

		updateObject();
	}
	else if (key == 8 && mString)				// Backspace
	{
		if(mString->length())
		{
			mString->resize(mString->length() - 1);
			updateObject();
		}
	}
	else if(mString)
	{
		if(!mStringLengthSet || mString->length() < mStringLength)
		{
			if(!checkValidity(key))
			{
				*mString += key;
				updateObject();
			}
		}
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	updateObject
//	Purpose:	Updates text in the main object
//
//***************************************************************************************
void cStringItem::updateObject()
{
	mMainObject.setText(*mString, false);		// Update text
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays menu item
//
//***************************************************************************************
void cStringItem::display(int id)
{
	static int displayCtr(0);

	if(mHighlight)
	{
		if(mDisplayState == 0)					// Normal highlighted display
		{
			glLoadName(id);
			mMainObject.highlight();
		}
		else if(mDisplayState == 1)				// Active display
		{
			glLoadName(id);
			glColor3f(0.5, 0.1, 0.5);
			mMainObject.colorlessDisplay();
			glLoadName(id);

			displayCtr++;

			if(displayCtr == 30)
				mMainObject.setText(*mString + "|", false);

			if(displayCtr == 60)
			{
				updateObject();
				displayCtr = 0;
			}
		}
	}
	else										// Normal idle display
	{
		glLoadName(id);
		mMainObject.display();
	}
}

//***************************************************************************************
//
//	Function:	checkValidity
//	Purpose:	Checks whether key is valid text character
//	Return:		True if key is invalid
//
//***************************************************************************************
bool cStringItem::checkValidity(int key)
{
	bool invalid(false);

	if(key < 32 || key > 126)
		invalid = true;

	return invalid;
}