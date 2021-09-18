//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 14, 2007
//	Last Modifed:	February 25, 2007
//	File:			listItem.h
//
//	Purpose:		Class definition for cListItem, which represents a menu item
//					with a list of selectable options
//
//***************************************************************************************

#pragma once

#include "menuItem.h"
#include "menuObject.h"

#include <vector>
using std::vector;

//***************************************************************************************
//
//	Class:		cListItem
//	Purpose:	Represents an item within a menu with a list of options
//
//***************************************************************************************
class cListItem: public cMenuItem
{
public:

	cListItem(): mIndex(NULL), mBoolIndex(NULL), mBoolean(false), mUpperBound(-1)	 {}
	~cListItem() {}					// Destructor

	virtual void display(int id);	// Displays menu item

	void addObject(cMenuObject object, bool increaseBounds);

	virtual bool cursorLeft();		// Moves cursor left within this item (decrement)
	virtual bool cursorRight();		// Moves cursor right within this item (increment)
	virtual int select(int subid);	// Selects subitem

	// Setters
	void setIndex(int* index) { mIndex = index; mBoolean = false; }
	void setIndex(bool* index) { mBoolIndex = index; mBoolean = true; }
	void setUpperBound(int bound) { mUpperBound = bound; }

	// Getters
	virtual cMenuObject* object(int i);
	virtual int size() { return (1 + mObjects.size()); }

private:

	vector<cMenuObject> mObjects;	// Set of graphic objects within item

	bool *mBoolIndex;				// Boolean index
	int *mIndex;					// Target value of this item
	bool mBoolean;					// Flags index type

	int mUpperBound;				// Highest value index can hold

};

//***************************************************************************************
//
//	Function:	cursorLeft
//	Purpose:	Moves cursor left within this item (decrement)
//	Returns:	False if cursor cannot be moved
//
//***************************************************************************************
bool cListItem::cursorLeft()
{
	bool bound(false);

	if(mBoolean)
	{
		if(*mBoolIndex == true)
			bound = true;
		else
			*mBoolIndex = true;
	}
	else
	{
		if(*mIndex <= 0)
			bound = true;
		else
			(*mIndex)--;
	}

	return bound;
}

//***************************************************************************************
//
//	Function:	cursorRight
//	Purpose:	Moves cursor right within this item (right)
//	Returns:	False if cursor cannot be moved
//
//***************************************************************************************
bool cListItem::cursorRight()
{
	bool bound(false);

	if(mBoolean)
	{
		if(*mBoolIndex == false)
			bound = true;
		else
			*mBoolIndex = false;
	}
	else
	{
		if(*mIndex >= mUpperBound)
			bound = true;
		else
			(*mIndex)++;
	}

	return bound;
}

//***************************************************************************************
//
//	Function:	select
//	Purpose:	Selects subitem (response to mouse click)
//
//***************************************************************************************
int cListItem::select(int subid)
{
	int returnVal(0);

	if(subid == 0)
		returnVal = mConfirm;
	else
	{
		if(mBoolean)
		{
			if(subid == 1 || subid == 2)
				(*mBoolIndex) = !(subid - 1);
		}
		else
		{
			if(subid > 0 && subid <= mUpperBound + 1)
				(*mIndex) = subid - 1;
		}
	}

	 return returnVal;
}

//***************************************************************************************
//
//	Function:	addObject
//	Purpose:	Adds an object to this item's object set
//				Optionally expands index's upper bound by one
//
//***************************************************************************************
void cListItem::addObject(cMenuObject object, bool increaseBounds = true)
{
	mObjects.push_back(object);

	if(increaseBounds)
		mUpperBound++;
}

//***************************************************************************************
//
//	Function:	object
//	Purpose:	Returns pointer to object; references objects left to right
//	Return:		Pointer to object of given index
//
//***************************************************************************************
cMenuObject* cListItem::object(int i)
{
	if(i < 1)
		return &mMainObject;
	else if(i < mObjects.size())
		return &mObjects[i - 1];
	else
		return &mObjects[mObjects.size() - 1];
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays all objects within menu item
//
//***************************************************************************************
void cListItem::display(int id)
{
	glLoadName(id);
	if(mHighlight)
		mMainObject.highlight();
	else
		mMainObject.display();

	int size = mObjects.size();

	if(mBoolean)
	{
		if(mBoolIndex != NULL)
		{
			for(int i(0); i < size; i++)
			{
				glLoadName(id + (i + 1) * MENU_SUBID);
				if(*mBoolIndex != i)
					mObjects[i].highlight();
				else
					mObjects[i].display();
			}
		}
		else
			for(int i(0); i < size; i++)
				mObjects[i].display(); 
	}
	else
	{
		if(mIndex != NULL)
		{
			for(int i(0); i < size; i++)
			{
				glLoadName(id + (i + 1) * MENU_SUBID);
				if(*mIndex == i)
					mObjects[i].highlight();		// Display highlighted menu item
				else
					mObjects[i].display();			// Display normal menu items
			}
		}
		else
			for(int i(0); i < size; i++)
				mObjects[i].display();
	}
}