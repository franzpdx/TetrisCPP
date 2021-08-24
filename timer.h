//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	April 30, 2007
//	Last Modified:	April 30, 2007
//	File:			timer.h
//
//	Purpose:		Class declaration for timer object, which represents a time span
//					with an optional return value upon timeout.
//
//***************************************************************************************

#pragma once

#include <atltime.h>			// For time manipulation

//***************************************************************************************
//
//	Class:		cTimer
//	Purpose:	Represents a delay with a timeout, optional return value for
//				state switching
//
//***************************************************************************************
class cTimer
{
public:

	cTimer(float seconds): mDuration(seconds), mReturn(0) { reset(); }
	cTimer(float seconds, int returnValue): mDuration(seconds), mReturn(returnValue) { reset(); }
	~cTimer() {}

	void reset();								// Reset timer
	bool check();								// Check for expiration

	int getReturn() { return mReturn; }			// Grab return value

private:

	float mDuration;				// Delay
	int mReturn;					// Return value

	CFileTime mStartTime;			// Start time

};

//***************************************************************************************
//
//	Function:	reset
//	Purpose:	Sets initialization time
//
//***************************************************************************************
void cTimer::reset()
{
	mStartTime = CFileTime::GetCurrentTime();
}

//***************************************************************************************
//
//	Function:	check
//	Purpose:	Checks whether timer has expired
//	Return:		True if timer exceeded
//
//***************************************************************************************
bool cTimer::check()
{
	bool timeout(false);

	CFileTimeSpan timeSpan = CFileTime::GetTickCount() - mStartTime;

	if(timeSpan > mDuration * 10000000)
		timeout = true;

	return timeout;
}