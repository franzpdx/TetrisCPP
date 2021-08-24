//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 30, 2007
//	Last Modified:	May 27, 2007
//	File:			blueTetris.h
//	Project:		Blue Tetris
//
//	Purpose:		Class definition for Blue Tetris game object. Class contains
//					game data and handles logical progression of game and menus.
//
//***************************************************************************************

#pragma once

#define BT_DATAFILE		"Data/settings.xml"
#define BT_KEYMAPFILE	"Data/keymap.dat"
#define BT_SCOREFILE	"Data/scores.dat"

#include "singlePlayer.h"
#include "multiplayer.h"
#include "game.h"
#include "keymap.h"
#include "menu.h"
#include "cursor.h"
#include "texture.h"
#include "font.h"
#include "object.h"
#include "socketConnection.h"
#include "sound.h"
#include "timer.h"
#include "XMLVarLibrary.h"

#include <vector>
using std::vector;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <time.h>

//***************************************************************************************
//
//	Class:		cBlueTetris
//	Purpose:	Accepts array representing user input and handles game progression
//
//***************************************************************************************
class cBlueTetris
{
public:

	cBlueTetris();			// Default Constructor
	~cBlueTetris();			// Default Destructor

	bool advance(vector<int> keys, int coords[], bool click);	// Advances game progression
	void display();			// Display function
	void setTexture(cTexture texture) { mTexture = texture; if(mGame) mGame->setTexture(texture); }
	void init();			// Initialization

private:
	
	void initGameSounds();	// Initializes sound system and loads sound files

	int handleServerMessages();	// Handles messages on incoming queue
	void stateSwitch(int state);	// State machine for navigation between menus + states
	void submitScore();				// Submits score and name pair in member vars to local highs

	// Screen construction functions
	void titleScreen();				// Creates title screen objects
	void optionScreen();			// Creates game option screen objects
	void systemScreen(bool pregame);	// Creates system setting screen
	void roomScreen();				// Creates game room screen for multiplay
	void keyConfigScreen();			// Creates menu for key configuration
	void scoreScreen();				// Displays high score screen
	void nameEntryScreen(bool server);	// Creates name entry screen for high scores
	void connectionScreen(string message);	// Creates screen for connecting with server
	void startSinglePlayer();		// Switches to single-player game state
	void startMultiplayer();		// Switches to multiplayer game state
	void splashScreen();			// Constructs splash credit screen
	void fakeScreen();				// Creates nonfunctional menu for website

	void initGameBackground();			// Creates game background objects
	void createAstronomicBackground();	// Creates "Astronomic" background

	void createLogo();

	// Menu item creation functions
	cListItem* createGraphicItem();
	cListItem* createMusicItem();
	cListItem* createBackgroundItem();

	// Memory freeing functions. Nullify associated pointers.
	void deleteMenu();
	void deleteGame();
	void deleteObjects();
	void deleteConnection();

	void createCursor();	// Creates or reassociates cursor with current menu
	void deleteCursor();	// Deletes cursor object

	// Music and sound functions
	void updateMusicMute();
	void updateSoundMute();
	void playGameTrack();

	// Data storage to files
	bool saveSettings();	// Saves options to data file
	bool loadSettings();	// Loads options from data file

	long double charToLongDouble(char target[]);
	void longDoubleToChar(char result[], long double value);

	void defaultSettings();			// Restores settings to game defaults
	void defaultScorelist();		// Defaults scorelist data
	void defaultGameOptions();		// Restores game options
	void defaultSystemSettings();	// Restores system settings
	void verifySettings();			// Verifies all settings are within allowable bounds
	void clearMultiplayerSettings();		// Sets mPresent and mReady arrays to 0

	// Server communication functions
	void sendSettingReport();		// Reports mulitplayer user settings to server

	// Server score update functions
	void sendScore();		// Reports end-game score to server
	void sendScoreSubmit(); // Submits high score pair to the server
	void sendScoreRequest(); // Requests score list from server
	bool readScoreMessage(string message); // Interprets message from server containing score list
	string parseName(string message, int &n);
	long double parseScore(string message, int &n);

	int mState;				// Global gamestate

	// Settings: board metrics
	int mbx;				// Board X
	int mby;				// Board Y
	int mbxo;				// Board X Origin
	int mbyo;				// Board Y Origin
	int mbzo;				// Board Z Origin
	int mUnitSize;			// Unit Size

	// Settings: graphic options
	bool mFrame;			// Flag for graphic frame toggle
	bool mGrid;				// Flag for graphic grid toggle
	int mAppearance;		// Board appearance selection
	bool mFullscreen;		// Flags window state
	bool mNextDisplay;		// Toggles next tetrad display

	// Settings: functional
	int mLevel;				// Game level
	bool mPermute;			// Toggles tetrad draw method
	int mBackground;		// Selected background option
	int mMusic;				// Selected music track
	bool mPlayMusic;		// Music toggle
	bool mPlaySound;		// Sound toggle

	// Game components
	cGame* mGame;					// Pointer to game instance
	cMenu* mMenu;					// Pointer to menu instance
	cCursor* mCursor;				// Cursor to menu
	cTimer* mTimer;					// Pointer to timer object
	cKeymap mKeymap;				// Keymapping
	cConnection* mConnection;		// Connection to server
	vector<cObject*> mObjects;		// Idle world objects

	bool mPresent[4];				// Flags for multiplayer mode
	bool mReady[4];
	bool mPlaying[4];
	int mPlayerID;

	bool mMultiFrame[4];			// Graphic settings for multiplayer
	bool mMultiGrid[4];
	int mMultiFace[4];

	cTexture mTexture;				// Texture library
	long double mLocalScores[10];	// Local high score list
	string mLocalNames[10];			// Names for local score list
	long double mServerScores[10];	// Server-wide high score storage
	string mServerNames[10];		// Names for server score list
	bool mServerScoresRetrieved;	// Flags when server scores are stored

	long double mScore;				// Post-game score storage
	string mName;					// Name entry

	string mIP;						// IP address for server
	string mLastValidIP;			// IP address of last successful connection
	int mPort;						// Port for server

	bool mExit;						// Flags end of game execution
};

//***************************************************************************************
//
//	Function:	Constructor
//	Purpose:	Initializes game settings
//
//***************************************************************************************
cBlueTetris::cBlueTetris(): mbxo(0), mbyo(-55), mbzo(-25), mUnitSize(5), 
	mServerScoresRetrieved(false), mExit(false), mGame(NULL), mMenu(NULL), 
	mTimer(NULL), mCursor(NULL)
{
	srand((unsigned)time( NULL ));

	clearMultiplayerSettings();	// Initialize present and ready

	defaultSettings();		// Default all values in case loading fails
	defaultScorelist();

	loadSettings();			// Load user's settings
	verifySettings();		// Verify settings are all in valid range

	initGameSounds();
}

//***************************************************************************************
//
//	Function:	Destructor
//	Purpose:	Deallocates memory
//
//***************************************************************************************
cBlueTetris::~cBlueTetris()
{
	saveSettings();				// Save user's settings
	deleteFont();				// Deallocate font graphic info

	deleteGame();
	deleteMenu();

	freeSound();
}

//***************************************************************************************
//
//	Function:	init
//	Purpose:	Initializes Blue Tetris
//
//***************************************************************************************
void cBlueTetris::init()
{
	if(mFullscreen)
		glutFullScreen();

	if(!mPlayMusic)
		muteMusic();

	if(!mPlaySound)
		muteSound();

	stateSwitch(SPLASH_SCREEN);
}

//***************************************************************************************
//
//	Function:	advance
//	Purpose:	Handles single frame advancement of game progression
//	Return:		True upon game exit
//
//***************************************************************************************
bool cBlueTetris::advance(vector<int> keys, int coords[], bool click)
{
	int stateVal(0);					// Stores return value of menu/game advancement

	updateSound();

	if(mTimer)							// Timers take top priority
	{
		if(mTimer->check())				// If timer expires
		{
			int val = mTimer->getReturn();	// Get return value

			if(val != 0)				// If nonzero
				stateSwitch(val);		// Send to state machine

			delete mTimer;				// Delete
			mTimer = NULL;				// Nullify
		}
	}
	else if(mMenu)						// Menu processing
	{
		if(click)
			stateVal = mMenu->advance(coords);
		else
			stateVal = mMenu->advance(keys);

		if(mConnection)
		{
			stateSwitch(stateVal);
			stateVal = handleServerMessages();
		}
	}
	else if(mGame)						// Game processing
	{
		stateVal = mGame->advance(keys);

		if(mConnection)
			if(mConnection->connected() == false)
				stateVal = HANDLE_DISCONNECT;
	}

	if(stateVal)						// If nonzero return caught
		stateSwitch(stateVal);			// Process through state machine

	return mExit;
}

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Renders active game interface
//
//***************************************************************************************
void cBlueTetris::display()
{
	if(mMenu)
		mMenu->display();

	if(mCursor)
		mCursor->display();

	else if(mGame)
		mGame->display();

	glColor3f(1.0, 1.0, 1.0);
	int size = mObjects.size();
	for(int i(0); i < size; i++)
		mObjects[i]->display();
}

//***************************************************************************************
//
//	Function:	saveSettings
//	Purpose:	Saves user option settings to data file
//	Return:		True if error occurred in accessing file
//
//***************************************************************************************
bool cBlueTetris::saveSettings()
{
	bool error(false);
	char buff[256];						// Buffer for output
	long double score;
	string name;

	ofstream fout;

	// Save User Settings

	cXMLLib settings;					// Create XML variable library

	settings.insert("width", mbx);		// Insert all settings
	settings.insert("height", mby);
	settings.insert("level", mLevel);
	settings.insert("appearance", mAppearance);
	settings.insert("background", mBackground);
	settings.insert("track", mMusic);
	settings.insert("music", mPlayMusic);
	settings.insert("sound", mPlaySound);
	settings.insert("permutation", mPermute);
	settings.insert("next", mNextDisplay);
	settings.insert("frame", mFrame);
	settings.insert("grid", mGrid);
	settings.insert("fullscreen", mFullscreen);
	settings.insert("IP", mIP);
	settings.insert("LastIP", mLastValidIP);

	if(settings.write(BT_DATAFILE))		// Save to file
		error = true;

	// Save High Score List
	fout.open(BT_SCOREFILE);

	if(fout)
	{
		for(int i(0); i < 10; i++)
		{
			score = mLocalScores[i];
			name = mLocalNames[i];

			sprintf(buff, "%.0Lf", score);
			fout << name << '\n'
				 << buff << '\n'
				 << BTChecksum(name, score) << '\n';
		}

		fout.close();
	}

	// Save Key Configuration
	fout.open(BT_KEYMAPFILE);			// Open keymap file
	if(fout)							// If opened
	{
		mKeymap.save(fout);				// Write keymapping
		fout.close();
	}

	return error;
}

//***************************************************************************************
//
//	Function:	loadSettings
//	Purpose:	Loads user option settings from data file
//	Return:		True if error occurred in accessing file
//
//***************************************************************************************
bool cBlueTetris::loadSettings()
{
	bool error(false);
	char buff[256];
	string name;
	long double score;
	
	ifstream keyfile, scorefile;

	cXMLLib settings;
	if(settings.read(BT_DATAFILE))
		error = true;
	else
	{
		settings.find("width", mbx);				// find settings from library
		settings.find("height", mby);
		settings.find("level", mLevel);
		settings.find("permutation", mPermute);
		settings.find("next", mNextDisplay);
		settings.find("frame", mFrame);
		settings.find("grid", mGrid);
		settings.find("appearance", mAppearance);
		settings.find("fullscreen", mFullscreen);
		settings.find("IP", mIP);
		settings.find("LastIP", mLastValidIP);
		settings.find("music", mPlayMusic);
		settings.find("sound", mPlaySound);
		settings.find("track", mMusic);
		settings.find("background", mBackground);
	}

	// Load High Score List
	scorefile.open(BT_SCOREFILE);
	if(scorefile)
	{
		int n(0);

		for(int i(0); i < 10; i++)
		{
			scorefile.getline(buff, 256);
			name = buff;
			scorefile.getline(buff, 256);
			score = charToLongDouble(buff);
			scorefile.getline(buff, 256);
			int l = BTChecksum(name, score);
			int m = atoi(buff);
			if(!(name == "No Entry" && score == 0))
			{
				if(BTChecksum(name, score) == atoi(buff))
				{
					mLocalNames[n] = name;
					mLocalScores[n] = score;
					n++;
				}
			}
		}
	}
	scorefile.close();

	// Load Key Configuration
	keyfile.open(BT_KEYMAPFILE);			// Open keymap file
	if(keyfile)								// If opened
		mKeymap.load(keyfile);				// Read keymap

	keyfile.close();

	return error;
}

//***************************************************************************************
//
//	Function:	charToLongDouble
//	Purpose:	Converts number within character array to long double
//
//***************************************************************************************
long double cBlueTetris::charToLongDouble(char target[])
{
	long double num(0);
	long double digit;
	int i(0);

	for(i = 0; target[i] >= 48 && target[i] <= 57; i++)
	{
		num = num * 10;
		digit = target[i] - 48;
		num += digit;
	}

	return num;
}

//***************************************************************************************
//
//	Function:	longDoubleToChar
//	Purpose:	Converts a long double to a character array of size SCORE_LENGTH
//
//***************************************************************************************
void cBlueTetris::longDoubleToChar(char result[], long double value)
{
	char buff[SCORE_LENGTH + 1];
	long double divisor(1);
	int i;

	for(i = 0; i < SCORE_LENGTH; i++)
		divisor = divisor * 10;

	for(i = 0; i < SCORE_LENGTH; i++)
	{
		buff[i] = ((int)(value / divisor)) + 48;	// Shave off digit, convert to ascii
		value = value - (value / divisor);			// Remove digit from value
		divisor = divisor / 10;						// Shift divisor to next digit
	}

	buff[SCORE_LENGTH] = '\0';						// Null terminate string
	sprintf(result, buff);							// Return result
}

//***************************************************************************************
//
//	Function:	defaultGameOptions
//	Purpose:	Restores game options to default
//
//***************************************************************************************
void cBlueTetris::defaultGameOptions()
{
	mLevel = DEFAULT_LEVEL;
	mAppearance = DEFAULT_FACE;
	mMusic = DEFAULT_MUSIC;

	mbx = DEFAULT_WIDTH;
	mby = DEFAULT_HEIGHT;
	mBackground = DEFAULT_BACKGROUND;
	mNextDisplay = DEFAULT_NEXT_DISPLAY;
}

//***************************************************************************************
//
//	Function:	defaultSystemSettings
//	Purpose:	Restores system settings
//
//***************************************************************************************
void cBlueTetris::defaultSystemSettings()
{
	mFrame = DEFAULT_FRAME;
	mGrid = DEFAULT_GRID;
	mPermute = DEFAULT_PERMUTE;
	mPlayMusic = DEFAULT_PLAY_MUSIC;
	mPlaySound = DEFAULT_PLAY_SOUND;
}

//***************************************************************************************
//
//	Function:	defaultSettings
//	Purpose:	Restores all settings to game defaults
//
//***************************************************************************************
void cBlueTetris::defaultSettings()
{
	defaultGameOptions();
	defaultSystemSettings();

	mFullscreen = false;
	mIP = DEFAULT_IP;
}

//***************************************************************************************
//
//	Function:	defaultScorelist
//	Purpose:	Fills scorelists with default values to prevent bad things
//
//***************************************************************************************
void cBlueTetris::defaultScorelist()
{
	for(int i(0); i < 10; i++)
	{
		mLocalScores[i] = 0;
		mServerScores[i] = 0;
		mLocalNames[i] = "No Entry";
		mServerNames[i] = "No Entry";
	}

	mServerScoresRetrieved = false;
}

//***************************************************************************************
//
//	Function:	verifySettings
//	Purpose:	Verifies that no settings are outside acceptable range
//
//***************************************************************************************
void cBlueTetris::verifySettings()
{
	if(mbx < 4)
		mbx = 4;

	if(mbx > 20)
		mbx = 20;

	if(mby < 6)
		mby = 6;

	if(mby > 30)
		mby = 30;

	if(mLevel < 0)
		mLevel = 0;

	if(mLevel > 20)
		mLevel = 20;

	if(mAppearance < 0)
		mAppearance = 0;

	if(mAppearance > 2)
		mAppearance = 2;

	if(mMusic < 0 || mMusic > 1)
		mMusic = 0;

	if(mBackground < 0 || mBackground > 1)
		mBackground = 0;
}

//***************************************************************************************
//
//	Function:	handleServerMessages
//	Purpose:	Handles messages recieved from server
//	Return:		State machine code
//
//***************************************************************************************
int cBlueTetris::handleServerMessages()
{
	string message;
	int returnVal(0);

	if(!mConnection)
		return 0;

	if(!mConnection->dequeue(message))	// Dequeue; execute if queue is not empty
	{
		int state = (message[0]>>3 & 0x3);			// Mask off gamestate
		int id = (message[0] & 0x7);				// Mask off client ID
		int code = message[1];

		switch(code)
		{
		case M_READY:								// Ready messages
			mReady[id] = true;
			break;

		case M_NOT_READY:
			mReady[id] = false;
			break;

		case M_CONNECT:								// Connection messages
			mPresent[id] = true;
			break;

		case M_DISCONNECT:
			mPresent[id] = false;
			if(id == mPlayerID)
				returnVal = HANDLE_DISCONNECT;
			break;

		case M_PLAYING:
			mPlaying[id] = true;
			break;
		
		case M_IDLE:
			mPlaying[id] = false;
			break;

		case M_APPEARANCE:							// Setting reports
			if(message.length() >= 2)
				mMultiFace[id] = message[2];
			break;

		case M_GRID:
			if(message.length() >= 2)
				mMultiGrid[id] = message[2];
			break;

		case M_FRAME:
			if(message.length() >= 2)
				mMultiFrame[id] = message[2];
			break;

		case M_ASSIGN_ID:					// ID assigment
			if(message.length() > 2)
			{
				mPlayerID = message[2] - NUMERAL_OFFSET;
				mPresent[mPlayerID] = true;
			}
			break;

		case M_ENTER_GAME_STATE:			// Enter Game State message
			mPresent[0] = (message[2] & 1);			// Mask off 'present' bits
			mPresent[1] = (message[2]>>1 & 1);
			mPresent[2] = (message[2]>>2 & 1);
			mPresent[3] = (message[2]>>3 & 1);
			
			startMultiplayer();
			break;

		case M_SCORE_LIST:					// Score List Update
			readScoreMessage(message);
			break;

		case M_HIGH_SCORE_ACHIEVED:
			returnVal = SERVER_NAME_ENTRY;
			break;

		case M_NO_HIGH_SCORE:
			returnVal = RETURN_TO_ROOM;
			break;
		};
	}
	else if(mConnection->connected() == false)
		returnVal = HANDLE_DISCONNECT;

	return returnVal;
}

//***************************************************************************************
//
//	Function:	sendSettingReport
//	Purpose:	Reports user settings to server (appearance, frame, grid)
//
//***************************************************************************************
void cBlueTetris::sendSettingReport()
{
	mConnection->sendMessage(S_ROOM, M_APPEARANCE, mAppearance);
	mConnection->sendMessage(S_ROOM, M_FRAME, mFrame);
	mConnection->sendMessage(S_ROOM, M_GRID, mGrid);
}

//***************************************************************************************
//
//	Function:	clearMultiplayerSettings
//	Purpose:	Sets all values within present and ready arrays to false
//
//***************************************************************************************
void cBlueTetris::clearMultiplayerSettings()
{
	for(int i(0); i > 4; i++)
	{
		mPresent[i] = false;
		mReady[i] = false;
		mPlaying[i] = false;

		mMultiGrid[i] = DEFAULT_GRID;
		mMultiFace[i] = DEFAULT_FACE;
		mMultiFrame[i] = DEFAULT_FRAME;
	}
}

//***************************************************************************************
//
//	Function:	stateSwitch
//	Purpose:	Blue Tetris's navigational state machine
//				Accepts return value from menu/game and handles navigation
//
//***************************************************************************************
void cBlueTetris::stateSwitch(int state)
{
	if(state == 0)						// Ignore null values
		return;

	switch (state)
	{
	case SPLASH_SCREEN:
		splashScreen();
		break;

	case UPDATE_MUSIC_MUTE:				// Music Functions -----------------------------
		updateMusicMute();
		break;

	case UPDATE_SOUND_MUTE:
		updateSoundMute();
		break;

	case PLAY_GAME_TRACK:
		playGameTrack();
		break;

	case PAUSE_MUSIC:
		pauseMusic();
		break;

	case UNPAUSE_MUSIC:
		unpauseMusic();
		break;

	case TITLE_SCREEN:					// Screen Navigation ---------------------------
		saveSettings();
		playMusic(MUSIC_TITLE);
		titleScreen();
		break;

	case OPTION_SCREEN:
		optionScreen();
		break;

	case MULTIPLAYER:
		connectionScreen("Enter Address and click Connect");
		break;

	case CONFIGURE_KEYS:
		keyConfigScreen();
		break;

	case SYSTEM_SCREEN:
		systemScreen(false);
		break;

	case VIEW_SCORES:
		scoreScreen();
		break;

	case EXIT:
		mExit = true;
		break;

	case SELECT_START_SINGLE_PLAYER:	// Single Player State Switching ---------------
		if(mCursor)
			mCursor->setSpin(2);

		if(mTimer)
			delete mTimer;

		mTimer = new cTimer(1, START_SINGLE_PLAYER);
		break;

	case START_SINGLE_PLAYER:
		playGameTrack();
		startSinglePlayer();
		break;

	case SWITCH_TO_SYSTEM_SCREEN:		// System setting screen (returns to SP options)
		systemScreen(true);
		break;

	case EXIT_SINGLE_PLAYER:
		playMusic(MUSIC_GAME_OVER);
		mScore = mGame->score();		// Store final game score
		deleteGame();

		if(mScore > mLocalScores[9])	// If greater than lowest high score
			nameEntryScreen(false);		// Go to name entry screen, local
		else
			scoreScreen();				// Otherwise go to high score list
		break;

	case SUBMIT_LOCAL:
		submitScore();
		scoreScreen();
		break;

	case DEFAULT_GAME_OPTIONS:
		defaultGameOptions();
		playGameTrack();
		if(mMenu)
			mMenu->refresh();
		break;

	case DEFAULT_SYSTEM_SETTINGS:
		defaultSystemSettings();
		updateMusicMute();
		updateSoundMute();
		if(mMenu)
			mMenu->refresh();
		break;

	case READY:							// Multiplayer State Switching -----------------
		sendSettingReport();
		mConnection->sendMessage(S_ROOM, M_READY);
		break;

	case END_MULTIPLAYER_GAME:
		playMusic(MUSIC_TITLE);
		deleteGame();
		createCursor();
		mScore = mGame->score();		// Store final game score
		sendScore();
		break;

	case SERVER_NAME_ENTRY:
		nameEntryScreen(true);
		break;

	case SUBMIT_SERVER:
		sendScoreSubmit();
		roomScreen();
		break;

	case RETURN_TO_ROOM:
		roomScreen();
		break;

	case HANDLE_DISCONNECT:
		deleteGame();
		deleteConnection();
		connectionScreen("Connection Lost");
		break;

	case EXIT_MULTIPLAYER:
		mConnection->sendMessage(S_GLOBAL, M_DISCONNECT);
		deleteConnection();				// Disconnection handled automatically

		deleteGame();
		titleScreen();
		playMusic(MUSIC_TITLE);
		break;

	case EXECUTE_FULLSCREEN_TOGGLE:		// Fullscreen Toggle ---------------------------	
		mFullscreen = !mFullscreen;
		if(mFullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(INIT_WIN_WIDTH, INIT_WIN_HEIGHT);
			glutPositionWindow(INIT_WIN_X, INIT_WIN_Y);
		}
		break;
	};

	if(state == INIT_CONNECTION)		// Connect to server ---------------------------
	{
		mObjects.pop_back();
		cTextObject* t = new cTextObject("Connecting...", -100, -5, 0, 1, 1, 1, true);
		mObjects.push_back(t);

		display();

		mConnection = new cConnection(mIP.c_str());		// Create connection

		if(mConnection->connected() == false)	// If connection failed
		{
			mObjects.pop_back();
			cTextObject* t = new cTextObject("Connection failed", -100, -5, 0, 1, 1, 1, true);
			mObjects.push_back(t);				// Post failure message

			delete mConnection;					// Delete object
			mConnection = NULL;					// Nullify pointer
		}
		else									// If connecton succeeds
		{
			mLastValidIP = mIP;					// Store IP as last valid
			roomScreen();						// Load room screen
			sendScoreRequest();					// Request score list from server
		}
	}
}

//***************************************************************************************
//
//	Function:	initGameSounds
//	Purpose:	Initializes sound system and loads sound files
//
//***************************************************************************************
void cBlueTetris::initGameSounds()
{
	initSound();

	loadMusic("./Music/Dubmood - ST-Style.xm", MUSIC_TITLE);
	loadMusic("./Music/Floppi - Jttimarsu Edit.xm", MUSIC_GAME_OVER);
	loadMusic("./Music/Gammis - Saturn.xm", MUSIC_TRACK1);
	loadMusic("./Music/Tetris - Korobeiniki.mp3", MUSIC_TRACK2);

	loadSound("./Sound/Lock.wav", SOUND_LOCK);
	loadSound("./Sound/Bump.wav", SOUND_COLLISION);
	loadSound("./Sound/Rotate.wav", SOUND_ROTATE);
	loadSound("./Sound/Impact.wav", SOUND_LINE_DROP);
	loadSound("./Sound/Impact.wav", SOUND_TETRIS);
}

//-------------------------------------------------------------------------------------O
//
//	Score handling functions
//	The following functions handle local and server-wide score management
//

//***************************************************************************************
//
//	Function:	sendScoreRequest
//	Purpose:	Sends server a request for the high score list
//
//***************************************************************************************
void cBlueTetris::sendScoreRequest()
{
	if(mConnection)
	{
		mConnection->sendMessage(S_GLOBAL, M_REQUEST_SCORE);	// Send a score request
	}
}

//***************************************************************************************
//
//	Function:	readScoreMessage
//	Purpose:	Interpets message from server containing high score list
//
//***************************************************************************************
bool cBlueTetris::readScoreMessage(string message)
{
	bool success(false);
	long double score;
	string name;
	int n(3);					// Message index; begins at first non-header char

	if(message[2] == M_SCORE_LIST_FAILURE)
		mServerScoresRetrieved = false;

	if(message[2] == M_SCORE_LIST_SUCCESS)
	{
		for(int i(0); i < 10 && n < message.length(); i++)
		{
			name = parseName(message, n);
			score = parseScore(message, n);

			mServerNames[i] = name;
			mServerScores[i] = score;
		}

		mServerScoresRetrieved = true;
		success = true;
	}

	return success;
}

//***************************************************************************************
//
//	Function:	parseName
//	Purpose:	Parses a name from the server score list message
//	Return:		String containing player name
//
//***************************************************************************************
string cBlueTetris::parseName(string message, int &n)
{
	string name;

	for(int i(0); i < NAME_LENGTH && n < message.length(); i++, n++)
	{
		if(message[n] != EMPTY_CHARACTER)
			name += message[n];
	}

	return name;
}

//***************************************************************************************
//
//	Function:	parseScore
//	Purpose:	Parses a score from the server score list message
//	Return:		long double containing high score
//
//***************************************************************************************
long double cBlueTetris::parseScore(string message, int &n)
{
	char score[SCORE_LENGTH + 1];
	
	for(int i(0); i < SCORE_LENGTH && n < message.length(); i++, n++)
		score[i] = message[n];

	return charToLongDouble(score);
}

//***************************************************************************************
//
//	Function:	sendScore
//	Purpose:	Reports end game score to server (for check; not submit)
//
//***************************************************************************************
void cBlueTetris::sendScore()
{
	string message;
	char buff[255];

	message += S_GLOBAL * 8 + BT_CODE * 32;		// Identifier byte
	message += M_REPORT_SCORE;				// Message byte

	longDoubleToChar(buff, mScore);
	message += buff;

	mConnection->enqueue(message);
}

//***************************************************************************************
//
//	Function:	sendScoreSubmit
//	Purpose:	Submits a score pair to the server
//
//***************************************************************************************
void cBlueTetris::sendScoreSubmit()
{
	string message;
	char buff[255];

	message += S_GLOBAL * 8 + BT_CODE * 32;		// Identifier byte
	message += M_HIGH_SCORE_SUBMIT;				// Message byte

	message += mName;

	while(message.length() < 2 + NAME_LENGTH)
	{
		message += EMPTY_CHARACTER;
	}

	longDoubleToChar(buff, mScore);
	message += buff;

	mConnection->enqueue(message);
}

//***************************************************************************************
//
//	Function:	submitScore
//	Purpose:	Submits score and name pair stored in member variables to
//				local high score list
//
//***************************************************************************************
void cBlueTetris::submitScore()
{
	bool inserted(false);

	if(mScore > mLocalScores[9])
	{
		for(int i = 9; i > 0 && !inserted; i--)
		{
			if(mScore <= mLocalScores[i - 1])	// If position found
			{
				mLocalScores[i] = mScore;		// Insert score
				mLocalNames[i] = mName;
				inserted = true;
			}
			else								// Else
			{
				mLocalScores[i] = mLocalScores[i - 1];	// Shift next score down
				mLocalNames[i] = mLocalNames[i - 1];
			}
		}

		if(!inserted)							// If loop is exited, score is first
		{
			mLocalScores[0] = mScore;
			mLocalNames[0] = mName;
		}
	}
}


//--------------------------------------------------------------------------------------O
//
//	Menu creation functions
//
//	The following functions will create thier corresponding menus and set the menu
//	pointer to the created menu.
//

//***************************************************************************************
//
//	Function:	connectionScreen
//	Purpose:	Creates objects for connection screen which prompts user for server addr
//
//***************************************************************************************
void cBlueTetris::connectionScreen(string message)
{
	deleteMenu();

	mMenu = new cMenu(&mKeymap);

	cTextObject* t = new cTextObject("Server Address:", -100, 5, 0, 1, 1, 1, true);
	mObjects.push_back(t);

	if(mLastValidIP.length())
	{
		string addr = "Last Valid Address: " + mLastValidIP;
		t = new cTextObject(addr, -100, 1, 0, .6, .6, .6, true);
		mObjects.push_back(t);
	}

	cStringItem* s = new cStringItem();
	s->setMainObject(cMenuObject(-100, 3, -5, 10, 1, ""));
	s->setString(&mIP);
	s->setStringLength(15);
	mMenu->addItem(s);

	cMenuItem* m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, -7.5, -9.5, "Connect"));
	m->setConfirm(INIT_CONNECTION);
	mMenu->addItem(m);

	m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, -9, -9.5, "Exit"));
	m->setConfirm(TITLE_SCREEN);
	mMenu->addItem(m);

	mMenu->setCancelMapping(2);
	mMenu->setCursorPosition(1);

	// State machine relies on "buffer" message being last in object vector
	t = new cTextObject(message, -100, -5, 0, .6, .6, .6, true);
	mObjects.push_back(t);

	createCursor();
}

//***************************************************************************************
//
//	Function:	titleScreen
//	Purpose:	Creates title screen objects and sets menu pointer
//
//***************************************************************************************
void cBlueTetris::titleScreen()
{
	const float top(0);
	const float spacing(1.8);

	deleteMenu();
	createLogo();

	mMenu = new cMenu(&mKeymap);

	cMenuItem* m = new cMenuItem();						// Single Player button
	m->setMainObject(cMenuObject(-100, top, 4, 7.5, 1, "Single Player"));
	m->setConfirm(OPTION_SCREEN);
	mMenu->addItem(m);

	m = new cMenuItem();								// Multiplayer button
	m->setMainObject(cMenuObject(-100, top - spacing, 4, 7.5, 1, "Multiplayer"));
	m->setConfirm(MULTIPLAYER);
	mMenu->addItem(m);

	m = new cMenuItem();								// View high scores button
	m->setMainObject(cMenuObject(-100, top - spacing*2, 4, 7.5, 1, "High Scores"));
	m->setConfirm(VIEW_SCORES);
	mMenu->addItem(m);

	m = new cMenuItem();								// Configure keys
	m->setMainObject(cMenuObject(-100, top - spacing*3, 4, 7.5, 1, "Configure Keys"));
	m->setConfirm(CONFIGURE_KEYS);
	mMenu->addItem(m);

	m = new cMenuItem();								// System Settings
	m->setMainObject(cMenuObject(-100, top - spacing*4, 4, 7.5, 1, "System Settings"));
	m->setConfirm(SYSTEM_SCREEN);
	mMenu->addItem(m);

	m = new cMenuItem();								// Exit program
	m->setMainObject(cMenuObject(-100, top - spacing*5, 4, 7.5, 1, "Exit Game"));
	m->setConfirm(EXIT);
	mMenu->addItem(m);

	createCursor();
}

//***************************************************************************************
//
//	Function:	createLogo
//	Purpose:	Creates Blue Tetris logo for title screen
//
//***************************************************************************************
void cBlueTetris::createLogo()
{
	const float size(.2);
	const float x(-113.4);
	const float y(1.62);
	const float z(-1.8);

	mObjects.push_back(new cTextObject("B  ue Tetris", -114, 1, 0, 1, 1, 1, true));

	mObjects.push_back(new cCubeObject(x, y, z, size, 0, false, &mTexture, 2, &mAppearance));
	mObjects.push_back(new cCubeObject(x, y - size, z, size, 0, false, &mTexture, 2, &mAppearance));
	mObjects.push_back(new cCubeObject(x, y - size*2, z, size, 0, false, &mTexture, 2, &mAppearance));
	mObjects.push_back(new cCubeObject(x, y - size*2, z + size, size, 0, false, &mTexture, 2, &mAppearance));
}

//***************************************************************************************
//
//	Function:	optionScreen
//	Purpose:	Creates menu screen objects and sets menu pointer
//
//***************************************************************************************
void cBlueTetris::optionScreen()
{
	deleteMenu();

	cTextObject* t = new cTextObject("Game Options", -100, 9, 0, .7, 1.0, .7, true);
	mObjects.push_back(t);

	mMenu = new cMenu(&mKeymap);

	cIterativeItem* i = new cIterativeItem();			// Iterator: Level selection
	i->setMainObject(cMenuObject("Level", false));
	i->setVariableObject(cMenuObject("00"));
	i->setIndex(&mLevel);
	i->setUpperBound(20);
	i->setLowerBound(0);
	mMenu->addItem(i);

	mMenu->addItem(createGraphicItem());
	mMenu->addItem(createMusicItem());
	mMenu->addItem(createBackgroundItem());

	cListItem* m = new cListItem();						// List: Next Display
	m->setMainObject(cMenuObject("Next Display", false));
	m->addObject(cMenuObject("On"));
	m->addObject(cMenuObject("Off"));
	m->setIndex(&mNextDisplay);
	mMenu->addItem(m);

	i = new cIterativeItem();							// Iterator: Board height
	i->setMainObject(cMenuObject("Board Height", false));
	i->setVariableObject(cMenuObject("00"));
	i->setLowerBound(6);
	i->setUpperBound(30);
	i->setIndex(&mby);
	mMenu->addItem(i);

	i = new cIterativeItem();							// Iterator: Board width
	i->setMainObject(cMenuObject("Board Width", false));
	i->setVariableObject(cMenuObject("00"));
	i->setLowerBound(4);
	i->setUpperBound(20);
	i->setIndex(&mbx);
	mMenu->addItem(i);

	cMenuItem* e = new cMenuItem();						// Button: Default Settings
	e->setMainObject(cMenuObject("Default Settings"));
	e->setConfirm(DEFAULT_GAME_OPTIONS);
	mMenu->addItem(e);

	e = new cMenuItem();								// Button: Start
	e->setMainObject(cMenuObject("Start"));
	e->setConfirm(SELECT_START_SINGLE_PLAYER);
	mMenu->addItem(e);

	e = new cMenuItem();								// Button: Cancel
	e->setMainObject(cMenuObject("Cancel"));
	e->setConfirm(TITLE_SCREEN);
	mMenu->addItem(e);

	e = new cMenuItem();								// Button: System Settings
	e->setMainObject(cMenuObject("System Settings"));
	e->setConfirm(SWITCH_TO_SYSTEM_SCREEN);
	mMenu->addItem(e);

	mMenu->setCancelMapping(9);						// Set cancel mapping
	mMenu->setCursorPosition(8);
	mMenu->autoFormat(3);

	createCursor();
}

//***************************************************************************************
//
//	Function:	systemScreen
//	Purpose:	Creates system setting screen
//
//***************************************************************************************
void cBlueTetris::systemScreen(bool pregame)
{
	deleteMenu();

	int exitLocation;
	if(pregame)
		exitLocation = OPTION_SCREEN;
	else
		exitLocation = TITLE_SCREEN;

	cTextObject* t = new cTextObject("System Settings", -100, 9, 0, .7, 1.0, .7, true);
	mObjects.push_back(t);

	mMenu = new cMenu(&mKeymap);

	cListItem* m = new cListItem();						// List: Permutation toggle
	m->setMainObject(cMenuObject("Draw Method", false));
	m->addObject(cMenuObject("7 Bag"));
	m->addObject(cMenuObject("Random"));
	m->setIndex(&mPermute);
	mMenu->addItem(m);

	m = new cListItem();								// List: Frame toggle
	m->setMainObject(cMenuObject("Frame", false));
	m->addObject(cMenuObject("On"));
	m->addObject(cMenuObject("Off"));
	m->setIndex(&mFrame);
	mMenu->addItem(m);

	m = new cListItem();								// List: Grid toggle
	m->setMainObject(cMenuObject("Grid", false));
	m->addObject(cMenuObject("On"));
	m->addObject(cMenuObject("Off"));
	m->setIndex(&mGrid);
	mMenu->addItem(m);

	m = new cListItem();								// List: Music Toggle
	m->setMainObject(cMenuObject("Music", false));
	m->addObject(cMenuObject("On"));
	m->addObject(cMenuObject("Off"));
	m->setIndex(&mPlayMusic);
	m->setNavReturn(UPDATE_MUSIC_MUTE);
	mMenu->addItem(m);

	m = new cListItem();								// List: Sound Toggle
	m->setMainObject(cMenuObject("Sound", false));
	m->addObject(cMenuObject("On"));
	m->addObject(cMenuObject("Off"));
	m->setIndex(&mPlaySound);
	m->setNavReturn(UPDATE_SOUND_MUTE);
	mMenu->addItem(m);

	cMenuItem* e = new cMenuItem();						// Button: Default Settings
	e->setMainObject(cMenuObject("Default Settings"));
	e->setConfirm(DEFAULT_SYSTEM_SETTINGS);
	mMenu->addItem(e);

	e = new cMenuItem();								// Button: Cancel
	e->setMainObject(cMenuObject("Return"));
	e->setConfirm(exitLocation);
	mMenu->addItem(e);
	
	mMenu->setCancelMapping(6);							// Set cancel mapping
	mMenu->autoFormat(1);

	createCursor();
}

//***************************************************************************************
//
//	Function:	roomScreen
//	Purpose:	Creates screen objects for a game room screen
//
//***************************************************************************************
void cBlueTetris::roomScreen()
{
	deleteMenu();

	const int row0(8);
	const int row1(6.5);
	const int row2(5);
	const int row3(3.5);
	const int row4(2);

	const int col1(-11);
	const int col2(-3);
	const int col3(1);
	const int col4(4);

	//=====The following code makes this======
	//				Present	Ready	Playing
	//	Player 1	x		x
	//	Player 2
	//	Player 3
	//	Player 4	x		x
	//
	//========================================

	cTextObject* t;
	cBoolObject* b;

	t = new cTextObject("Present:", -100, row0, col2);		// Column Labels
	mObjects.push_back(t);

	t = new cTextObject("Ready:", -100, row0, col3);
	mObjects.push_back(t);

	t = new cTextObject("Playing:", -100, row0, col4);
	mObjects.push_back(t);

	t = new cTextObject("Player 1", -100, row1, col1);		// Row: Player 1
	mObjects.push_back(t);

	b = new cBoolObject(&mPresent[0], -100, row1, col2);
	mObjects.push_back(b);
	b = new cBoolObject(&mReady[0], -100, row1, col3);
	mObjects.push_back(b);
	b = new cBoolObject(&mPlaying[0], -100, row1, col4);
	mObjects.push_back(b);

	t = new cTextObject("Player 2", -100, row2, col1);		// Row: Player 2
	mObjects.push_back(t);

	b = new cBoolObject(&mPresent[1], -100, row2, col2);
	mObjects.push_back(b);
	b = new cBoolObject(&mReady[1], -100, row2, col3);
	mObjects.push_back(b);
	b = new cBoolObject(&mPlaying[1], -100, row1, col4);
	mObjects.push_back(b);

	t = new cTextObject("Player 3", -100, row3, col1);		// Row: Player 3
	mObjects.push_back(t);
	
	b = new cBoolObject(&mPresent[2], -100, row3, col2);
	mObjects.push_back(b);
	b = new cBoolObject(&mReady[2], -100, row3, col3);
	mObjects.push_back(b);
	b = new cBoolObject(&mPlaying[2], -100, row1, col4);
	mObjects.push_back(b);

	t = new cTextObject("Player 4", -100, row4, col1);		// Row: Player 4
	mObjects.push_back(t);

	b = new cBoolObject(&mPresent[3], -100, row4, col2);
	mObjects.push_back(b);
	b = new cBoolObject(&mReady[3], -100, row4, col3);
	mObjects.push_back(b);
	b = new cBoolObject(&mPlaying[3], -100, row1, col4);
	mObjects.push_back(b);

	mMenu = new cMenu(&mKeymap);						// Menu section

	mMenu->addItem(createGraphicItem());
	mMenu->addItem(createMusicItem());
	mMenu->addItem(createBackgroundItem());

	cMenuItem* e = new cMenuItem();						// Button: Start
	e->setMainObject(cMenuObject("Ready"));
	e->setConfirm(READY);
	mMenu->addItem(e);

	e = new cMenuItem();								// Button: Cancel
	e->setMainObject(cMenuObject("Leave"));
	e->setConfirm(EXIT_MULTIPLAYER);
	mMenu->addItem(e);

	mMenu->setAutoTop(-1.5);
	mMenu->setCursorPosition(3);
	mMenu->setCancelMapping(4);
	mMenu->autoFormat();
	
	createCursor();
}

//***************************************************************************************
//
//	Function:	keyConfigScreen
//	Purpose:	Constructs key configuration menu
//
//***************************************************************************************
void cBlueTetris::keyConfigScreen()
{
	deleteMenu();

	mMenu = new cMenu(&mKeymap);

	const float leftRow(-12.5);
	const float rightRow(1);
	const float spacing(1);
	const float nameWidth(6);
	const float keyWidth(4.5);
	const bool nameBorder(false);

	// Labels
	cTextObject* t = new cTextObject("Game Keys", -100, 5.5, -6.25, .6, .6, .6, true);
	mObjects.push_back(t);
	t = new cTextObject("Menu Keys", -100, 5.5, 6.25, .6, .6, .6, true);
	mObjects.push_back(t);
	t = new cTextObject("Key Configuration", -100, 9, 0, .7, 1.0, .7, true);
	mObjects.push_back(t);

	// Global Commands
	cMapItem* m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 7, -12.5, 8, 1, "Fullscreen Toggle", nameBorder));
	m->setVariableObject(cMenuObject(-100, 7, -3.5, 4.5, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(FULLSCREEN_TOGGLE);
	mMenu->addItem(m);

	// Game Keys
	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 4, leftRow, nameWidth, 1, "Drop", nameBorder));
	m->setVariableObject(cMenuObject(-100, 4, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_DOWN);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 2.5, leftRow, nameWidth, 1, "Left", nameBorder));
	m->setVariableObject(cMenuObject(-100, 2.5, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_LEFT);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 1, leftRow, nameWidth, 1, "Right", nameBorder));
	m->setVariableObject(cMenuObject(-100, 1, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_RIGHT);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -0.5, leftRow, nameWidth, 1, "Sonic Lock", nameBorder));
	m->setVariableObject(cMenuObject(-100, -0.5, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_SONIC_LOCK);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -2, leftRow, nameWidth, 1, "Rotate Left", nameBorder));
	m->setVariableObject(cMenuObject(-100, -2, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_ROTATE_LEFT);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -3.5, leftRow, nameWidth, 1, "Rotate Right", nameBorder));
	m->setVariableObject(cMenuObject(-100, -3.5, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_ROTATE_RIGHT);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -5, leftRow, nameWidth, 1, "Pause", nameBorder));
	m->setVariableObject(cMenuObject(-100, -5, leftRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(GAME_PAUSE);
	mMenu->addItem(m);


	// Menu Navigation Keys
	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 4, rightRow, nameWidth, 1, "Up", nameBorder));
	m->setVariableObject(cMenuObject(-100, 4, rightRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(MENU_UP);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 2.5, rightRow, nameWidth, 1, "Down", nameBorder));
	m->setVariableObject(cMenuObject(-100, 2.5, rightRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(MENU_DOWN);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, 1, rightRow, nameWidth, 1, "Left", nameBorder));
	m->setVariableObject(cMenuObject(-100, 1, rightRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(MENU_LEFT);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -0.5, rightRow, nameWidth, 1, "Right", nameBorder));
	m->setVariableObject(cMenuObject(-100, -0.5, rightRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(MENU_RIGHT);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -2, rightRow, nameWidth, 1, "Confirm", nameBorder));
	m->setVariableObject(cMenuObject(-100, -2, rightRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(MENU_CONFIRM);
	mMenu->addItem(m);

	m = new cMapItem();
	m->setMainObject(cMenuObject(-100, -3.5, rightRow, nameWidth, 1, "Cancel", nameBorder));
	m->setVariableObject(cMenuObject(-100, -3.5, rightRow + nameWidth + spacing, keyWidth, 1, ""));
	m->setKeymap(&mKeymap);
	m->setAction(MENU_CANCEL);
	mMenu->addItem(m);


	// Exit Button
	cMenuItem* i = new cMenuItem();
	i->setMainObject(cMenuObject(-100, -9, -9.5, "Exit"));
	i->setConfirm(TITLE_SCREEN);
	mMenu->addItem(i);
	mMenu->setCancelMapping(14);

	createCursor();
}

//***************************************************************************************
//
//	Function:	scoreScreen
//	Purpose:	Constructs high score display screen
//
//***************************************************************************************
void cBlueTetris::scoreScreen()
{
	deleteMenu();

	int i;				// Index variable
	float y, z;			// Variables for determining output location
	const int col1(1);	// Spacing values
	const int col2(6);
	const int col3(5.5);
	char buff[256];		// Buffer for sprintf operations
	string name;		// Buffer for name

	mMenu = new cMenu(&mKeymap);

	cMenuItem* m = new cMenuItem();
	m->setMainObject(cMenuObject("Exit"));
	m->setConfirm(TITLE_SCREEN);
	
	mMenu->addItem(m);
	mMenu->setCancelMapping(0);
	mMenu->autoFormat();

	cTextObject* t;
	y = 6.5;
	z = -12.7;

	t = new cTextObject("High Scores", -100, 8, -7.5, .7, 1, .7, true);
	mObjects.push_back(t);
	for(i = 0; i < 10; i++)
	{
		sprintf(buff, "%i", i + 1);
		t = new cTextObject(string(buff), -100, y, z);
		mObjects.push_back(t);

		name = mLocalNames[i];
		sprintf(buff, "%.0Lf", mLocalScores[i]);

		while(textLength(name.c_str()) + textLength(buff) > col2 + col3)
			name.resize(name.length() - 1);

		t = new cTextObject(name, -100, y, z + col1);
		mObjects.push_back(t);
		t = new cTextObject(string(buff), -100, y, z + col1 + col2 + col3 - textLength(buff));
		mObjects.push_back(t);
		y -= 1.35;
	}

	if(mServerScoresRetrieved)
	{
		y = 6.5;
		z = .3;
		t = new cTextObject("Server High Scores", -100, 8, 7.5, .7, 1, .7, true);
		mObjects.push_back(t);
		for(i = 0; i < 10; i++)
		{
			sprintf(buff, "%i", i + 1);
			t = new cTextObject(string(buff), -100, y, z);
			mObjects.push_back(t);
			
			name = mServerNames[i];
			sprintf(buff, "%.0Lf", mServerScores[i]);

			while(textLength(name.c_str()) + textLength(buff) > col2 + col3)
				name.resize(name.length() - 1);

			t = new cTextObject(name, -100, y, z + col1);
			mObjects.push_back(t);
			t = new cTextObject(string(buff), -100, y, z + col1 + col2 + col3 - textLength(buff));
			mObjects.push_back(t);
			y -= 1.35;
		}
	}

	createCursor();
}

//***************************************************************************************
//
//	Function:	nameEntryScreen
//	Purpose:	Creates screen for name entry; displays score
//
//***************************************************************************************
void cBlueTetris::nameEntryScreen(bool server)
{
	deleteMenu();
	mName.clear();

	int place(0);			// Int for calculating placement
	char buff[100];			// Buffer for forming messages
	float x(-104);

	float pRed(1);			// Color of placement text
	float pGreen(1);
	float pBlue(.9);

	float tRed(1);			// Color of normal text
	float tGreen(1);
	float tBlue(.9);

	for(int i = 9; i > 0 && place == 0; i--)		// Determine placement
	{
		if (mLocalScores[i - 1] > mScore)
			place = i;
	}

	place += 1;				// Convert from array index to placement number

	switch(place)
	{
	case 1:
		sprintf(buff, "First Place!");
		pRed = 0.72;
		pGreen = 0.85;
		pBlue = 1.0;
		break;

	case 2:
		sprintf(buff, "Second Place");
		pRed = 1.0;
		pGreen = 0.85;
		pBlue = 0.55;
		break;

	case 3:
		sprintf(buff, "3rd Place");
		break;

	default:
		sprintf(buff, "%ith Place", place);
	};

	cTextObject* t = new cTextObject("Congratulations!", x - 3, 4.2, 0, pRed, pGreen, pBlue, true);
	mObjects.push_back(t);

	t = new cTextObject(string(buff), x, 3.5, 0, pRed, pGreen, pBlue, true);
	mObjects.push_back(t);

	if(server == true)
	{
		if(place == 1)
			sprintf(buff, "New Server Record!");
		else
			sprintf(buff, "on the Server");
		t = new cTextObject(string(buff), x, 2.6, 0, pRed, pGreen, pBlue, true);
	}

	t = new cTextObject("Enter Your Name:", x, .3, 0, tRed, tGreen, tBlue, true);
	mObjects.push_back(t);

	sprintf(buff, "Score: %.0Lf", mScore);
	t = new cTextObject(string(buff), x, -5, 0, tRed, tGreen, tBlue, true);
	mObjects.push_back(t);

	mMenu = new cMenu(&mKeymap);
	cStringItem* i = new cStringItem();
	i->setMainObject(cMenuObject(x, -1.5, -2, "", false));
	i->setString(&mName);
	i->setStringLength(NAME_LENGTH);

	if(server)
		i->setStringReturn(SUBMIT_SERVER);
	else
		i->setStringReturn(SUBMIT_LOCAL);

	mMenu->addItem(i);
	mMenu->enterInputMode(0);
}

//***************************************************************************************
//
//	Function:	splashScreen
//	Purpose:	Creates splash credit screen and sets up timer
//
//***************************************************************************************
void cBlueTetris::splashScreen()
{
	deleteMenu();

	cImageObject* image = new cImageObject(-1, -1, 1, 1, -116.2, &mTexture, T_SPLASH_SCREEN);
	mObjects.push_back(image);

	mTimer = new cTimer(4, TITLE_SCREEN);
}

//***************************************************************************************
//
//	Function:	startSinglePlayer
//	Purpose:	Switches to single player active game state
//
//***************************************************************************************
void cBlueTetris::startSinglePlayer()
{
	deleteCursor();
	deleteMenu();

	initGameBackground();

	mGame = new cSinglePlayer(mbx, mby, mbxo, mbyo, mbzo, mUnitSize, mFrame, mGrid, 
		mAppearance, mLevel, mPermute, mNextDisplay, mBackground, mKeymap);
	mGame->setTexture(mTexture);
}

//***************************************************************************************
//
//	Function:	startMultiplayer
//	Purpose:	Switches to multiplayer active game state
//
//***************************************************************************************
void cBlueTetris::startMultiplayer()
{
	deleteCursor();
	deleteMenu();

	initGameBackground();

	mMultiFrame[mPlayerID] = mFrame;
	mMultiGrid[mPlayerID] = mGrid;
	mMultiFace[mPlayerID] = mAppearance;

	mGame = new cMultiplayer(mPresent, mPlayerID, mKeymap, mConnection, mTexture, 
		mMultiFrame, mMultiGrid, mMultiFace);
}

//***************************************************************************************
//
//	Function:	fakeScreen
//	Purpose:	Creates the menu used for the website
//
//***************************************************************************************
void cBlueTetris::fakeScreen()
{
	deleteCursor();
	deleteMenu();

	const float x(-3);
	const float width(6);

	mMenu = new cMenu(&mKeymap);

	cMenuItem* m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, 4, x, width, 1, "Home"));
	mMenu->addItem(m);

	m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, 2, x, width, 1, "Download"));
	mMenu->addItem(m);

	m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, 0, x, width, 1, "About"));
	mMenu->addItem(m);

	m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, -2, x, width, 1, "Version Info"));
	mMenu->addItem(m);

	m = new cMenuItem();
	m->setMainObject(cMenuObject(-100, -4, x, width, 1, "Links"));
	mMenu->addItem(m);
}

//-------------------------------------------------------------------------------------O
//
//	Menu Item Creation Functions
//	These functions create individual items for a menu
//	Used to standardize settings used in multiple menus
//

//***************************************************************************************
//
//	Function:	createGraphicItem
//	Purpose:	Creates an item for selecting graphic option
//	Returns:	Pointer to newed item
//
//***************************************************************************************
cListItem* cBlueTetris::createGraphicItem()
{
	cListItem* m = new cListItem();						// List: Graphic Scheme
	m->setMainObject(cMenuObject("Scheme", false));
	m->addObject(cMenuObject("Classic"));
	m->addObject(cMenuObject("Modern"));
	m->addObject(cMenuObject("Gameboy"));
	m->setIndex(&mAppearance);
	
	return m;
}

//***************************************************************************************
//
//	Function:	createMusicItem
//	Purpose:	Creates an item for selecting music track
//	Returns:	Pointer to newed item
//
//***************************************************************************************
cListItem* cBlueTetris::createMusicItem()
{
	cListItem* m = new cListItem();						// List: Music Selection
	m->setMainObject(cMenuObject("Music", false));
	m->addObject(cMenuObject("Saturn"));
	m->addObject(cMenuObject("Korobeiniki"));
	m->setIndex(&mMusic);
	m->setNavReturn(PLAY_GAME_TRACK);
	
	return m;
}

//***************************************************************************************
//
//	Function:	createBackgroundItem
//	Purpose:	Creates an item for selecting background option
//	Returns:	Pointer to newed item
//
//***************************************************************************************
cListItem* cBlueTetris::createBackgroundItem()
{
	cListItem* m = new cListItem();						// List: Background
	m->setMainObject(cMenuObject("Background", false));
	m->addObject(cMenuObject("Astronomic"));
	m->addObject(cMenuObject("None"));
	m->setIndex(&mBackground);
	
	return m;
}

//-------------------------------------------------------------------------------------O
//
//	Background Creation Functions
//	The following functions create the objects that comprise each background object
//

//***************************************************************************************
//
//	Function:	initGameBackground
//	Purpose:	Initializes background objects
//
//***************************************************************************************
void cBlueTetris::initGameBackground()
{
	switch(mBackground)
	{
	case 0:								// Case: Astronomic
		createAstronomicBackground();
		break;

	case 1:								// Case: None
		break;
	};
}

//***************************************************************************************
//
//	Function:	createAstronomicBackground
//	Purpose:	Creates objects for astronomic background option
//
//***************************************************************************************
void cBlueTetris::createAstronomicBackground()
{
	const int size(130);		// Size of image relative to center

	cImageObject* image = new cImageObject(-size, -size, size, size, 20,
		&mTexture, T_BACKGROUND_1);

	mObjects.push_back(image);
}

//-------------------------------------------------------------------------------------O
//
//	Delete Functions + Pointer Management
//	These functions handle deletion of common game objects.
//	Functions nullify pointers and ensure exceptions are not created.
//

//***************************************************************************************
//
//	Function:	deleteMenu
//	Purpose:	Deletes menu if pointer is not null; nullifies pointer
//
//***************************************************************************************
void cBlueTetris::deleteMenu()
{
	if(mCursor)
		mCursor->orphan();

	if(mMenu)
		delete mMenu;

	deleteObjects();

	mMenu = NULL;
}

//***************************************************************************************
//
//	Function:	deleteGame
//	Purpose:	Deletes game if pointer is not null; nullifies pointer
//
//***************************************************************************************
void cBlueTetris::deleteGame()
{
	if(mGame)
		delete mGame;

	deleteObjects();

	mGame = NULL;
}

//***************************************************************************************
//
//	Function:	deleteObjects
//	Purpose:	Clears array of objects
//
//***************************************************************************************
void cBlueTetris::deleteObjects()
{
	int size = mObjects.size();
	for(int i(0); i < size; i++)
		delete mObjects[i];

	mObjects.clear();
}

//***************************************************************************************
//
//	Function:	deleteConnection
//	Purpose:	Deletes connection object
//
//***************************************************************************************
void cBlueTetris::deleteConnection()
{
	if(mConnection)
		delete mConnection;

	mConnection = NULL;
}

//***************************************************************************************
//
//	Function:	createCursor
//	Purpose:	Creates cursor if pointer is null
//				If not, reassigns cursor to current menu
//
//***************************************************************************************
void cBlueTetris::createCursor()
{
	if(mCursor == NULL)
		mCursor = new cCursor(&mTexture, &mAppearance, mMenu, 0, 0, 0, .6);
	else
		mCursor->assign(mMenu);
}

//***************************************************************************************
//
//	Function:	deleteCursor
//	Purpose:	Deletes cursor, nullifies pointer
//
//***************************************************************************************
void cBlueTetris::deleteCursor()
{
	if(mCursor != NULL)
	{
		delete mCursor;
		mCursor = NULL;
	}
}

//-------------------------------------------------------------------------------------O
//
//	Music and Sound functions
//	Functions to toggle muting and update sound track
//

//***************************************************************************************
//
//	Function:	updateSoundMute
//	Purpose:	Updates sound muting according to member variable
//
//***************************************************************************************
void cBlueTetris::updateSoundMute()
{
	if(mPlaySound)
		unmuteSound();
	else
		muteSound();
}

//***************************************************************************************
//
//	Function:	updateMusicMute
//	Purpose:	Updates music muting according to member variable
//
//***************************************************************************************
void cBlueTetris::updateMusicMute()
{
	if(mPlayMusic)
		unmuteMusic();
	else
		muteMusic();
}

//***************************************************************************************
//
//	Function:	playGameTrack
//	Purpose:	Plays user-selected track
//
//***************************************************************************************
void cBlueTetris::playGameTrack()
{
	switch(mMusic)
	{
	case 0:
		playMusic(MUSIC_TRACK1);
		break;

	case 1:
		playMusic(MUSIC_TRACK2);
		break;

	case 2:
		playMusic(MUSIC_TRACK3);
		break;
	};
}