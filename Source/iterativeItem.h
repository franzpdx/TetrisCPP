//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 14, 2007
//	Last Modifed:	March 15, 2007
//	File:			iterativeItem.h
//
//	Purpose:		Class definition for cIterativeItem, representing a menu item
//					with a single variable item.
//
//***************************************************************************************

#pragma once

#include "menuItem.h"
#include "menuObject.h"

//***************************************************************************************
//
//	Class:		cIterativeItem
//	Purpose:	Represents an item within a menu with a variable object
//
//***************************************************************************************
class cIterativeItem: public cMenuItem
{
public:

	cIterativeItem(): mIndex(NULL), mUpperBound(0), mLowerBound(0) {}	// Constructor
	~cIterativeItem() {}			// Destructor

	virtual void display(int id);	// Displays menu item

	void setVariableObject(cMenuObject object);	// Sets dimensions of variable object

	virtual bool cursorLeft();		// Moves cursor left within this item (decrement)
	virtual bool cursorRight();		// Moves cursor right within this item (increment)
	virtual int select(int subid);	// Response to selection (mouse click)

	virtual void refresh() { updateObject(); }

	// Setters
	void setIndex(int* index);
	void setUpperBound(int bound) { mUpperBound = bound; }
	void setLowerBound(int bound) { mLowerBound = bound; }

	// Getters
	virtual cMenuObject* object(int i) { if(i < 1) return &mMainObject; else return &mVariableObject; }
	virtual int size() { return 2; }

private:

	void updateObject();			// Updates button text to reflect index

	cMenuObject mVariableObject;	// Variable menu object

	int* mIndex;					// Target value of this item
	int mUpperBound;				// Highest value index can hold
	int mLowerBound;				// Lowest value index can hold

};

//***************************************************************************************
//
//	Function:	cursorLeft
//	Purpose:	Decrments index of variable object
//	Returns:	False if cursor cannot be moved
//
//***************************************************************************************
bool cIterativeItem::cursorLeft()
{
	bool bound(false);

	if(*mIndex == mLowerBound)
		bound = true;
	else
		(*mIndex)--;

	updateObject();

	return bound;
}

//***************************************************************************************
//
//	Function:	cursorRight
//	Purpose:	Increments index of variable object
//	Returns:	False if cursor cannot be moved
//
//***************************************************************************************
bool cIterativeItem::cursorRight()
{
	bool bound(false);

	if(*mIndex == mUpperBound)
		bound = true;
	else
		(*mIndex)++;

	updateObject();

	return bound;
}

//***************************************************************************************
//
//	Function:	select
//	Purpose:	Response to selection (mouse click)
//
//***************************************************************************************
int cIterativeItem::select(int subid)
{
	int returnVal(0);

	if(subid == 0)
		returnVal = mConfirm;
	else if (subid == 1)
	{
		if(cursorRight())
		{
			*mIndex = mLowerBound;
			updateObject();
		}
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	setVariableObject
//	Purpose:	Sets the variable object metrics
//
//***************************************************************************************
void cIterativeItem::setVariableObject(cMenuObject object)
{
	mVariableObject = object;
	updateObject();
}

//***************************************************************************************
//
//	Function:	setIndex
//	Purpose:	Sets the index and updates object text
//
//***************************************************************************************
void cIterativeItem::setIndex(int* index)
{
	mIndex = index;
	updateObject();
}

//***************************************************************************************
//
//	Function:	updateObject
//	Purpose:	Updates text on iterative object to reflect its index
//
//***************************************************************************************
void cIterativeItem::updateObject()
{
	if(mIndex != NULL)
	{
		char buff[10];
		sprintf(buff, "%i", *mIndex);
		mVariableObject.setText(buff, false);
	}
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays all objects within menu item
//
//***************************************************************************************
void cIterativeItem::display(int id)
{
	glLoadName(id);

	if(mHighlight)
	{
		mMainObject.highlight();
		
		glLoadName(id + MENU_SUBID);
		mVariableObject.highlight();
	}
	else
	{
		mMainObject.display();

		glLoadName(id + MENU_SUBID);
		mVariableObject.display();
	}
}