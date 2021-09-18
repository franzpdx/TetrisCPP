//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 30, 2007
//	Last Modified:	May 27, 2007
//	File:			singlePlayer.h
//	Project:		Blue Tetris
//
//	Purpose:		Class definition for single player instance. Handles game 
//					logic, data, progression in single-player mode.
//
//***************************************************************************************

#pragma once

#include "game.h"
#include "trisboard.h"
#include "keymap.h"
#include "sound.h"
#include <vector>
#include <atltime.h>			// For time manipulation
#include <string>
using std::string;
using std::vector;

// Local definitions used in command interpretation
#define ARRAY_SONIC_LOCK	0
#define ARRAY_DOWN			1
#define ARRAY_RIGHT			2
#define ARRAY_LEFT			3
#define ARRAY_ROTATE_LEFT	4
#define ARRAY_ROTATE_RIGHT	5
#define ARRAY_PAUSE			6

//***************************************************************************************
//
//	Class:		cSinglePlayer
//	Purpose:	Handles game progression in single-player mode
//
//***************************************************************************************
class cSinglePlayer : public cGame
{
public:

	cSinglePlayer(int bx, int by, float bxo, float byo, float bzo, float unitSize,
		bool frame, bool grid, int scheme, int level, bool permute, bool nextDisplay,
		int background, cKeymap keymap);
	~cSinglePlayer() {}							// Default destructor

	virtual int advance(vector<int> keys);		// Interpret keys
	int advanceGame();							// Advance game action

	virtual void display();						// Display function
	void displayStateMessages();				// Displays text specific to board states

	virtual void setTexture(cTexture texture) { mTexture = texture; mBoard.setTexture(mTexture); }

	// Getters
	virtual long double score() { return mBoard.score(); }
	virtual int level() { return mBoard.level(); }

private:

	bool executeCommand();				// Executes input
	void clearMessage(int lines);		// Posts line clear message to event field
	virtual void resetCountdown();		// Resets active tetrad drop counter

	void forceCheck();					// Forces tetrad downward at timed intervals

	cTrisBoard mBoard;					// Member board
	cKeymap mKeymap;					// Keymapping
	int mBackground;					// Background ID

	CFileTime mStartTime;				// Stores time at which game began
	CFileTime mPauseTime;				// Stores pause periods
	CFileTime mLastDropTime;			// Stores timestamp of last forced drop
	CFileTimeSpan mTotalTime;			// Logs total time for postgame

	int mBoardState;					// Pregame / Active / Pause / Postgame

	bool mKeylist[7];					// Array for keydown tracking
	int mRepeat[7];						// Array for key repeat tracking
};

//***************************************************************************************
//
//	Function:	Value Constructor
//	Purpose:	Handles value initialization
//
//***************************************************************************************
cSinglePlayer::cSinglePlayer(int bx, int by, float bxo, float byo, float bzo, 
							 float unitSize, bool frame, bool grid, int scheme, int level,
							 bool permute, bool nextDisplay, int background,
							 cKeymap keymap): mKeymap(keymap), mBoardState(BS_PREGAME),
							 mBackground(background), 
							 mBoard(bx, by, bxo, byo, bzo, unitSize, mTexture, grid, 
							 frame, scheme, permute, level, nextDisplay)
{
	for(int i(0); i < 7; i++)
	{
		mKeylist[i] = true;
		mRepeat[i] = 1;
	}
}

//***************************************************************************************
//
//	Function:	advance
//	Purpose:	Interprets input and advances game progression single frame
//	Return:		Value for state switching if applicable
//
//***************************************************************************************
int cSinglePlayer::advance(vector<int> keys)
{
	int returnVal(0);
	int size = keys.size();
	int command;
	int i;
	bool toggle(false);
	static bool toggleDown(false);

	mKeylist[0] = false;		// Key flags default to off
	mKeylist[1] = false;
	mKeylist[2] = false;
	mKeylist[3] = false;
	mKeylist[4] = false;
	mKeylist[5] = false;
	mKeylist[6] = false;

	for(i = 0; i < size; i++)	// Flag keys that are being pressed
	{
		command = mKeymap.map(keys[i], S_GAME);			// Map key to command
		if(command == GAME_SONIC_LOCK)					// Flag in command array
			mKeylist[ARRAY_SONIC_LOCK] = true;
		else if(command == GAME_DOWN)
			mKeylist[ARRAY_DOWN] = true;
		else if(command == GAME_RIGHT)
			mKeylist[ARRAY_RIGHT] = true;
		else if(command == GAME_LEFT)
			mKeylist[ARRAY_LEFT] = true;
		else if(command == GAME_ROTATE_LEFT)
			mKeylist[ARRAY_ROTATE_LEFT] = true;
		else if(command == GAME_ROTATE_RIGHT)
			mKeylist[ARRAY_ROTATE_RIGHT] = true;		
		else if(command == GAME_PAUSE)
			mKeylist[ARRAY_PAUSE] = true;
		else if(command == FULLSCREEN_TOGGLE)
			toggle = true;
	}

	returnVal = advanceGame();							// Execute commands

	if(!returnVal && toggle && !toggleDown)
		returnVal = EXECUTE_FULLSCREEN_TOGGLE;

	if(toggle)
		toggleDown = true;
	else
		toggleDown = false;

	for(i = 0; i < 7; i++)		// Track key repeats
	{
		if(mKeylist[i])
			mRepeat[i]++;
		else
			mRepeat[i] = 0;
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	advanceGame
//	Purpose:	Advances game progression
//
//***************************************************************************************
int cSinglePlayer::advanceGame()
{
	int returnVal(0);

	if(mBoardState == BS_ACTIVE) // If game is active ----------------------------------
	{
		if(mKeylist[ARRAY_PAUSE] && !mRepeat[ARRAY_PAUSE])
		{
			mPauseTime = CFileTime::GetCurrentTime();
			mBoardState = BS_PAUSED;
			returnVal = PAUSE_MUSIC;
		}
		else
			returnVal = executeCommand();	// Relay input to active advance function
	}

	else if(mBoardState == BS_PREGAME) // If game is in pregame state ------------------
	{
		bool begin(false);

		if( ( mKeylist[ARRAY_ROTATE_LEFT] && !mRepeat[ARRAY_ROTATE_LEFT]  ) ||
			( mKeylist[ARRAY_ROTATE_RIGHT] && !mRepeat[ARRAY_ROTATE_RIGHT]) ||
			( mKeylist[ARRAY_PAUSE] && !mRepeat[ARRAY_PAUSE]) )
		{
			resetCountdown();
			mStartTime = CFileTime::GetCurrentTime();	// Store starting time
			mBoardState = BS_ACTIVE;					// Set state to active
			mBoard.start();
		}
	}


	else if(mBoardState == BS_POSTGAME) // If game is in postgame state ----------------
	{
		bool exit(false);

		if( ( mKeylist[ARRAY_ROTATE_LEFT] && !mRepeat[ARRAY_ROTATE_LEFT]  ) ||
			( mKeylist[ARRAY_ROTATE_RIGHT] && !mRepeat[ARRAY_ROTATE_RIGHT]) ||
			( mKeylist[ARRAY_PAUSE] && !mRepeat[ARRAY_PAUSE]) )
			returnVal = EXIT_SINGLE_PLAYER;
	}


	else if(mBoardState == BS_PAUSED) // If game is paused -----------------------------
	{
		if(mKeylist[ARRAY_PAUSE] && !mRepeat[ARRAY_PAUSE])
		{
			mBoardState = BS_ACTIVE;
			CFileTimeSpan span = CFileTime::GetCurrentTime() - mPauseTime;
			mStartTime += span;
			mLastDropTime += span;
			returnVal = UNPAUSE_MUSIC;
		}
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	executeCommand
//	Purpose:	Carrys out user commands for this frame
//
//***************************************************************************************
bool cSinglePlayer::executeCommand()
{
	bool error(false);
	int rvalue(0);

	if(mKeylist[ARRAY_SONIC_LOCK] && mRepeat[ARRAY_SONIC_LOCK] < 1)
	{
		int cleared;
		
		if(mBoard.sonicLock(cleared))
			playSound(SOUND_LOCK);

		if(cleared > 0)
			clearMessage(cleared);
	}

	if(mKeylist[ARRAY_DOWN] && (mRepeat[ARRAY_DOWN] == 0 || mRepeat[ARRAY_DOWN] > 3) )
	{
		int cleared;
		
		if(mBoard.moveDown(cleared))
			playSound(SOUND_LOCK);

		if(cleared > 0)
			clearMessage(cleared);

		resetCountdown();
	}

	if(mKeylist[ARRAY_RIGHT] && (mRepeat[ARRAY_RIGHT] == 0 || mRepeat[ARRAY_RIGHT] > 3) )
		mBoard.moveRight();

	if(mKeylist[ARRAY_LEFT] && (mRepeat[ARRAY_LEFT] == 0 || mRepeat[ARRAY_LEFT] > 3) )
		mBoard.moveLeft();

	if(mKeylist[ARRAY_ROTATE_LEFT] && !mRepeat[ARRAY_ROTATE_LEFT])
	{
		rvalue = mBoard.rotateLeft();

		if(rvalue == 1)
			playSound(SOUND_COLLISION);
		else if(rvalue == 0)
			playSound(SOUND_ROTATE);
	}

	if(mKeylist[ARRAY_ROTATE_RIGHT] && !mRepeat[ARRAY_ROTATE_RIGHT])
	{
		rvalue =  mBoard.rotateRight();
		if(rvalue == 1)
			playSound(SOUND_COLLISION);
		else if(rvalue == 0)
			playSound(SOUND_ROTATE);
	}

	forceCheck();		// Force tetrad downward at fixed interval

	if(mBoard.gameOver())
	{
		mTotalTime = CFileTime::GetTickCount() - mStartTime;
		mBoardState = BS_POSTGAME;
	}

	return error;
}

//**************************************************************************************
//
//	Function:	forceCheck
//	Purpose:	Forces active tetrad downward at timed intervals
//
//**************************************************************************************
void cSinglePlayer::forceCheck()
{
	CFileTimeSpan timeDiff = CFileTime::GetTickCount() - mLastDropTime;
	
	while((timeDiff.GetTimeSpan() / 10000) > BTDropInterval(mBoard.level()))
	{
		int cleared;
		mLastDropTime += BTDropInterval(mBoard.level()) * 10000;
		timeDiff = CFileTime::GetCurrentTime() - mLastDropTime;
		
		if(mBoard.forceDown(cleared))			// Force tetrad downward
			playSound(SOUND_LOCK);

		if(cleared > 0)							// Report any line clears
			clearMessage(cleared);
	}
}

//**************************************************************************************
//
//	Function:	resetCountdown
//	Purpose:	Resets active tetrad drop counter
//
//**************************************************************************************
void cSinglePlayer::resetCountdown()
{
	mLastDropTime = CFileTime::GetCurrentTime();
}

//**************************************************************************************
//
//	Function:	clearMessage
//	Purpose:	Posts line clear message corresponding to number of lines sent
//
//**************************************************************************************
void cSinglePlayer::clearMessage(int lines)
{
	string message;
	float rotation(0);

	switch(lines)
	{
	case 1:
		message = "SINGLE";
		playSound(SOUND_LINE_DROP);
		break;

	case 2:
		message = "DOUBLE";
		playSound(SOUND_LINE_DROP);
		break;

	case 3:
		message = "TRIPLE";
		playSound(SOUND_LINE_DROP);
		break;

	case 4:
		message = "TETRIS";
		rotation = 360;
		playSound(SOUND_TETRIS);
		break;

	default:
		return;
	};

	post(message, 1.5, rotation);
}

//**************************************************************************************
//
//	Function:	display
//	Purpose:	Displays playing board and interface
//
//***************************************************************************************
void cSinglePlayer::display()
{
	int i;
	int seconds;
	CFileTimeSpan time;
	char buff[255];

	mBoard.display();

	glColor3f(0.7, 1.0, 0.7);
	glDisable(GL_TEXTURE_2D);

	displayEvent();

	// "Next" display
	displayText(-100, 8, 7.2, 0, 0, 0, true, "Next");

	// Score display
	displayText(-100, 8, -13, 0, 0, 0, false, "Score  %.0Lf", mBoard.score());

	if(mBackground == 0)
	{
		// Level display
		displayText(-100, 3.5, 6, 0, 0, 0, false, "Level %i", mBoard.level());

		// Remaining line display
		displayText(-100, 2, 6, 0, 0, 0, false, "Rest %i", mBoard.remaining());

		// Line counts
		displayText(-100, -5, -12.5, 0, 0, 0, false, "Singles");
		displayText(-100, -5, -8.5, 0, 0, 0, false, "%i", mBoard.singles());
		displayText(-100, -6.3, -12.5, 0, 0, 0, false, "Doubles");
		displayText(-100, -6.3, -8.5, 0, 0, 0, false, "%i", mBoard.doubles());
		displayText(-100, -7.6, -12.5, 0, 0, 0, false, "Triples");
		displayText(-100, -7.6, -8.5, 0, 0, 0, false, "%i", mBoard.triples());
		displayText(-100, -8.9, -12.5, 0, 0, 0, false, "Tetrises");
		displayText(-100, -8.9, -8.5, 0, 0, 0, false, "%i", mBoard.tetrises());
	}
	else
	{
		// Level display
		displayText(-100, 3, 6, 0, 0, 0, false, "Level %i", mBoard.level());

		// Remaining line display
		displayText(-100, 1, 6, 0, 0, 0, false, "Rest %i", mBoard.remaining());

		// Line counts
		displayText(-100, 3, -11.5, 0, 0, 0, false, "Singles");
		displayText(-100, 3, -7.5, 0, 0, 0, false, "%i", mBoard.singles());
		displayText(-100, 1, -11.5, 0, 0, 0, false, "Doubles");
		displayText(-100, 1, -7.5, 0, 0, 0, false, "%i", mBoard.doubles());
		displayText(-100, -1, -11.5, 0, 0, 0, false, "Triples");
		displayText(-100, -1, -7.5, 0, 0, 0, false, "%i", mBoard.triples());
		displayText(-100, -3, -11.5, 0, 0, 0, false, "Tetrises");
		displayText(-100, -3, -7.5, 0, 0, 0, false, "%i", mBoard.tetrises());
	}

	// Time Display
	if(mBoardState == BS_ACTIVE)
	{
		time = CFileTime::GetTickCount() - mStartTime;
		seconds = time.GetTimeSpan() / 10000000;
		sprintf(buff, "%.2i:%.2i", seconds/60, seconds % 60);
		
	}
	else if(mBoardState == BS_POSTGAME)
	{
		seconds = mTotalTime.GetTimeSpan() / 10000000;
		sprintf(buff, "%.2i:%.2i", seconds/60, seconds % 60);
	}
	else
		sprintf(buff, "-- : --");

	displayText(-100, -6.3, 6.35, 0, 0, 0, false, "Time"); // Display label
	displayText(-100, -7.5, 6.2, 0, 0, 0, false, buff);		// Display timer

	displayStateMessages();
}

//***************************************************************************************
//
//	Function:	displayStateMessages
//	Purpose:	Displays messages specific to board state
//
//***************************************************************************************
void cSinglePlayer::displayStateMessages()
{
	if(mBoardState != BS_ACTIVE)
	{
		string text;
		static float rotation(0);
		static float step(-.2);

		switch(mBoardState)
		{
		case BS_PAUSED:
			text = "PAUSE";
			break;

		case BS_PREGAME:
			text = "READY";
			break;

		case BS_POSTGAME:
			text = "GAME OVER";
			break;
		};

		rotation += step;						// Neato animation stuff
		if(rotation > 15 || rotation < -15)
			step = -step;

		glColor3f(1, 1, 1);
		displayText(-105, 0, 0, 0, rotation, 0, true, text.c_str());
	}
}