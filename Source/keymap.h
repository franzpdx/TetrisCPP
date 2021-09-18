//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	February 9, 2007
//	Last Modified:	March 4, 2007
//	File:			keymap.h
//	Project:		Blue Tetris
//
//	Purpose:		Class definition for keymap class, which is used by Blue Tetris
//					to map keystrokes to commands dynamicly.
//
//***************************************************************************************

#pragma once

#include <vector>
#include "resource.h"

#include <fstream>
using std::ifstream;
using std::ofstream;

using std::vector;

//***************************************************************************************
//
//	Class:			cKeymap
//	Purpose:		Maps keystrokes to Blue Tetris commands
//
//***************************************************************************************
class cKeymap
{
public:

	cKeymap() { defaultKeymap(); }		// Default Constructor
	~cKeymap() {}						// Default Destructor

	int map(int key, int context);		// Accepts keystroke, returns command ID
	bool set(int key, int command);		// Attempts to map key to command
	int retrieve(int command);			// Retrieves key tied to command

	bool save(ofstream &fout);			// Writes keymap to file
	bool load(ifstream &fin);			// Reads keymapping from file

private:

	void defaultKeymap();				// Sets keymap to default

	void assign(int key, int command);	// Maps key to command
	int getContext(int command);		// Determines context of given command
	bool checkGlobalKeys(int key);		// Checks if key is used globally
	bool checkMenuKeys(int key);		// Checks if key is used in menu navigation
	bool checkGameKeys(int key);		// Checks if key is used during game
	int upperCase(int key);				// toUpper

	// Keymappings
	int mFullscreenToggle;

	int mMenuDown;
	int mMenuUp;
	int mMenuLeft;
	int mMenuRight;
	int mMenuConfirm;
	int mMenuCancel;

	int mGameSonicDrop;
	int mGameDown;
	int mGameRight;
	int mGameLeft;
	int mGameRotateLeft;
	int mGameRotateRight;
	int mGamePause;
};

//***************************************************************************************
//
//	Function:	map
//	Purpose:	Maps keystroke to command given context
//	Return:		0 if no mapping found; ID of command if mapping is found
//
//***************************************************************************************
int cKeymap::map(int key, int context)
{
	int command(0);						// Stores translated command

	key = upperCase(key);

	if(key == mFullscreenToggle)		// Global context
		command = FULLSCREEN_TOGGLE;

	if(context == S_MENU)				// Menu context
	{
		if(key == mMenuDown)
			command = MENU_DOWN;
		else if(key == mMenuUp)
			command = MENU_UP;
		else if(key == mMenuLeft)
			command = MENU_LEFT;
		else if(key == mMenuRight)
			command = MENU_RIGHT;
		else if(key == mMenuConfirm)
			command = MENU_CONFIRM;
		else if(key == mMenuCancel)
			command = MENU_CANCEL;
	}
	else if(context == S_GAME)			// Game context
	{
		if(key == mGameSonicDrop)
			command = GAME_SONIC_LOCK;
		else if(key == mGameDown)
			command = GAME_DOWN;
		else if(key == mGameLeft)
			command = GAME_LEFT;
		else if(key == mGameRight)
			command = GAME_RIGHT;
		else if(key == mGameRotateLeft)
			command = GAME_ROTATE_LEFT;
		else if(key == mGameRotateRight)
			command = GAME_ROTATE_RIGHT;
		else if(key == mGamePause)
			command = GAME_PAUSE;
	}

	return command;
}

//***************************************************************************************
//
//	Function:	defaultKeymap
//	Purpose:	Sets keymappings to Blue Tetris's defaults
//
//***************************************************************************************
void cKeymap::defaultKeymap()
{
	mFullscreenToggle = 2 + SPECIALKEY_OFFSET;		// F2

	mMenuDown = 103 + SPECIALKEY_OFFSET;			// Arrow keys
	mMenuUp = 101 + SPECIALKEY_OFFSET;
	mMenuLeft = 100 + SPECIALKEY_OFFSET;
	mMenuRight = 102 + SPECIALKEY_OFFSET;
	mMenuConfirm = 70;			// F
	mMenuCancel = 68;			// D

	mGameSonicDrop = 101 + SPECIALKEY_OFFSET;		// Arrow keys
	mGameDown = 103 + SPECIALKEY_OFFSET;
	mGameRight = 102 + SPECIALKEY_OFFSET;
	mGameLeft = 100 + SPECIALKEY_OFFSET;
	mGameRotateLeft = 68;		// D
	mGameRotateRight = 70;		// F
	mGamePause = 27;			// Escape
}

//***************************************************************************************
//
//	Function:	set
//	Purpose:	Attempts to set key to command
//	Return:		True if key is already taken in given context
//
//***************************************************************************************
bool cKeymap::set(int key, int command)
{
	bool taken(false);					// Flags whether key was already taken
	int context = getContext(command);

	key = upperCase(key);

	if(context == S_GLOBAL)
		taken = checkGlobalKeys(key) || checkMenuKeys(key) || checkGameKeys(key);
	else if(context == S_GAME)
		taken = checkGlobalKeys(key) || checkGameKeys(key);
	else if(context == S_MENU)
		taken = checkGlobalKeys(key) || checkMenuKeys(key);

	if(!taken)
		assign(key, command);

	return taken;
}

//***************************************************************************************
//
//	Function:	checkGlobalKeys
//	Purpose:	Checks if key is mapped to a global command
//	Returns:	True if keymapping found
//
//***************************************************************************************
bool cKeymap::checkGlobalKeys(int key)
{
	return (key == mFullscreenToggle);
}

//***************************************************************************************
//
//	Function:	checkMenuKeys
//	Purpose:	Checks if key is mapped to a menu action
//	Returns:	True if keymapping found
//
//***************************************************************************************
bool cKeymap::checkMenuKeys(int key)
{
	return (key == mMenuUp || key == mMenuDown || key == mMenuRight || key == mMenuLeft 
		|| key == mMenuConfirm || key == mMenuCancel);
}

//***************************************************************************************
//
//	Function:	checkGameKeys
//	Purpose:	Checks if key is mapped to a game action
//	Returns:	True if keymapping found
//
//***************************************************************************************
bool cKeymap::checkGameKeys(int key)
{
	return (key == mGameSonicDrop || key == mGameDown || key == mGameRight || 
		key == mGameLeft || key == mGameRotateLeft || key == mGameRotateRight ||
		key == mGamePause);
}

//***************************************************************************************
//
//	Function:	getContext
//	Purpose:	Determines context of given command
//	Return:		Defined game state; -1 if command code is invalid
//
//***************************************************************************************
int cKeymap::getContext(int command)
{
	int context = -1;

	switch(command)
	{
	case FULLSCREEN_TOGGLE:
		context = S_GLOBAL;
		break;

	case MENU_UP:
	case MENU_DOWN:
	case MENU_LEFT:
	case MENU_RIGHT:
	case MENU_CONFIRM:
	case MENU_CANCEL:
		context = S_MENU;
		break;

	case GAME_SONIC_LOCK:
	case GAME_DOWN:
	case GAME_RIGHT:
	case GAME_LEFT:
	case GAME_ROTATE_RIGHT:
	case GAME_ROTATE_LEFT:
	case GAME_PAUSE:
		context = S_GAME;
		break;
	}

	return context;
}

//***************************************************************************************
//
//	Function:	assign
//	Purpose:	Maps key to command
//
//***************************************************************************************
void cKeymap::assign(int key, int command)
{
	switch(command)
	{
	case FULLSCREEN_TOGGLE:
		mFullscreenToggle = key;
		break;

	case MENU_UP:
		mMenuUp = key;
		break;

	case MENU_DOWN:
		mMenuDown = key;
		break;

	case MENU_RIGHT:
		mMenuRight = key;
		break;

	case MENU_LEFT:
		mMenuLeft = key;
		break;

	case MENU_CONFIRM:
		mMenuConfirm = key;
		break;

	case MENU_CANCEL:
		mMenuCancel = key;
		break;

	case GAME_SONIC_LOCK:
		mGameSonicDrop = key;
		break;

	case GAME_DOWN:
		mGameDown = key;
		break;

	case GAME_RIGHT:
		mGameRight = key;
		break;

	case GAME_LEFT:
		mGameLeft = key;
		break;

	case GAME_ROTATE_RIGHT:
		mGameRotateRight = key;
		break;

	case GAME_ROTATE_LEFT:
		mGameRotateLeft = key;
		break;

	case GAME_PAUSE:
		mGamePause = key;
		break;
	};
}

//***************************************************************************************
//
//	Function:	retrieve
//	Purpose:	Returns key mapped to command
//
//***************************************************************************************
int cKeymap::retrieve(int command)
{
	int key(-1);

	switch(command)
	{
	case FULLSCREEN_TOGGLE:
		key = mFullscreenToggle;
		break;

	case MENU_UP:
		key = mMenuUp;
		break;

	case MENU_DOWN:
		key = mMenuDown;
		break;

	case MENU_RIGHT:
		key = mMenuRight;
		break;

	case MENU_LEFT:
		key = mMenuLeft;
		break;

	case MENU_CONFIRM:
		key = mMenuConfirm;
		break;

	case MENU_CANCEL:
		key = mMenuCancel;
		break;

	case GAME_SONIC_LOCK:
		key = mGameSonicDrop;
		break;

	case GAME_DOWN:
		key = mGameDown;
		break;

	case GAME_RIGHT:
		key = mGameRight;
		break;

	case GAME_LEFT:
		key = mGameLeft;
		break;

	case GAME_ROTATE_RIGHT:
		key = mGameRotateRight;
		break;

	case GAME_ROTATE_LEFT:
		key = mGameRotateLeft;
		break;

	case GAME_PAUSE:
		key = mGamePause;
		break;
	};

	return key;
}

//***************************************************************************************
//
//	Function:	save
//	Purpose:	Writes keymapping to file
//	Return:		True if error occurs in opening file
//
//***************************************************************************************
bool cKeymap::save(ofstream &fout)
{
	bool error(false);

	if(!fout)
		error = true;
	else
	{
		fout << mFullscreenToggle << " "

			<< mMenuDown << " "
			<< mMenuUp << " "
			<< mMenuLeft << " "
			<< mMenuRight << " "
			<< mMenuConfirm << " "
			<< mMenuCancel << " "

			<< mGameSonicDrop << " "
			<< mGameDown << " "
			<< mGameRight << " "
			<< mGameLeft << " "
			<< mGameRotateLeft << " "
			<< mGameRotateRight << " "
			<< mGamePause;
	}

	return error;
}

//***************************************************************************************
//
//	Function:	load
//	Purpose:	Loads keymapping from file
//	Return:		True if error occurs in reading file
//
//***************************************************************************************
bool cKeymap::load(ifstream &fin)
{
	bool error(false);

	if(!fin)
		error = true;
	else
	{
		fin >> mFullscreenToggle

			>> mMenuDown
			>> mMenuUp
			>> mMenuLeft
			>> mMenuRight
			>> mMenuConfirm
			>> mMenuCancel

			>> mGameSonicDrop
			>> mGameDown
			>> mGameRight
			>> mGameLeft
			>> mGameRotateLeft
			>> mGameRotateRight
			>> mGamePause;
	}

	return error;
}

//***************************************************************************************
//
//	Function:	upperCase
//	Purpose:	Converts any lower case character to upper case
//
//***************************************************************************************
int cKeymap::upperCase(int key)
{
	if(key >= 97 && key <= 122)
		key -= 32;

	return key;
}