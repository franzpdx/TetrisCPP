//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 30, 2007
//	Last Modified:	March 7, 2007
//	File:			game.h
//	Project:		Blue Tetris
//
//	Purpose:		Definition for parent game class. Single Player and client-side
//					multiplayer game classes inherit from this class; contains
//					functionality mutual to game modes.
//
//***************************************************************************************

#pragma once

#define DEFAULT_DURATION 40

#include "texture.h"
#include "font.h"
#include <string>
using std::string;

#include <vector>
using std::vector;

//***************************************************************************************
//
//	Class:		cGame
//	Purpose:	Basic game class, containing functionality mutual to single player
//				and multiplayer
//
//***************************************************************************************
class cGame
{
public:

	cGame(): mDuration(0), mEventField(true) {}		// Default Constructor
	~cGame() {}										// Default Destructor

	virtual int advance(vector<int> keys) { return true; }// Frame advancement
	virtual void display() {}			// Display function
	virtual void setTexture(cTexture texture) { mTexture = texture; }

	// Getters
	virtual long double score() { return 0; }
	virtual int level() { return 0; }

protected:

	void post(string message, float magnification, float rotation, float duration);
	void displayEvent();
	virtual void resetCountdown() {}			// Resets coundown timer

	int mCountdown;						// Countdown until tetrad forced downward

	cTexture mTexture;					// Texture library

	string mEventMessage;				// Message in event field
	float mDuration;					// Duration of message display
	float mEventRotation;				// Rotation amount for event field
	float mEventMagnification;			// Magnification amount for event field
	bool mEventField;					// Flag for toggling event field
};

//**************************************************************************************
//
//	Function:	post
//	Purpose:	Posts message to event field with given attributes
//
//**************************************************************************************
void cGame::post(string message, float magnification = 0, float rotation = 0, float duration = DEFAULT_DURATION)
{
	mEventMagnification = magnification;
	mEventRotation = rotation;
	mEventMessage = message;
	mDuration = duration;
}

//***************************************************************************************
//
//	Function:	displayEvent
//	Purpose:	Displays event field
//
//***************************************************************************************
void cGame::displayEvent()
{
	// Display event field
	if(mEventField && mDuration)
	{
		displayText(-100 - mEventMagnification, -9.5 + mEventMagnification/2, 0, mEventRotation, 0, 0, true, mEventMessage.c_str());
		mDuration--;
		
		if(mEventMagnification < 0.4 && mEventMagnification > -0.4)
			mEventMagnification = 0;

		if(mEventMagnification > 0)
			mEventMagnification -= 0.4;
		if(mEventMagnification < 0)
			mEventMagnification += 0.4;
		
		if(mEventRotation < 25 && mEventRotation > -25)
			mEventRotation = 0;

		if(mEventRotation > 0)
			mEventRotation -= 25;
		if(mEventRotation < 0)
			mEventRotation += 25;
	}
}