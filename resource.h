//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 31, 2007
//	Last Modified:	May 27, 2007
//	File:			resource.h
//	Project:		Blue Tetris
//	
//	Purpose:		Contains definitions common to Blue Tetris files
//
//***************************************************************************************

#pragma once

#include <math.h>				// For exponential calculation
#include <string.h>
using std::string;

//-------------------------------------------------------------------------------------O
//
//	Global Functions
//

//***************************************************************************************
//
//	Function:	BTDropInterval
//	Purpose:	Returns time interval of tetrad drop for playing board's level
//
//***************************************************************************************
long BTDropInterval(const unsigned int level)
{
	return (2.2 - pow(level, 1/4.5)) * 1000;
}

//***************************************************************************************
//
//	Function:	checksum
//	Purpose:	Calculates checksum for given name and score pair
//	Return:		Checksum value
//
//***************************************************************************************
int BTChecksum(string name, long double score)
{
	int checksum(0);
	int upperScore = score / 100000;
	int lowerScore = score - upperScore * 100000;

	while(upperScore != 0)
	{
		checksum += upperScore % 1000;
		upperScore = upperScore / 1000;
	}

	while(lowerScore != 0)
	{
		checksum += lowerScore % 100;
		lowerScore = lowerScore / 100;
	}

	for(int n(0); n < name.length(); n++)
	{
		checksum += int(name[n]);
	}

	int divisor = checksum % 10;
	if(divisor == 0)
		divisor = 1;
	checksum = checksum / divisor;

	return checksum;
}

//-------------------------------------------------------------------------------------O
//
//	Global Defines
//

// Constants for OpenGL
#define CAMERA_X			-118
#define CAMERA_Y			0
#define CAMERA_Z			0
#define CAMERA_ANGLE		0
#define LIGHT_X				-200
#define LIGHT_Y				-700
#define LIGHT_Z				850
#define INIT_WIN_WIDTH		800
#define INIT_WIN_HEIGHT		600
#define INIT_WIN_X			3
#define INIT_WIN_Y			23
#define NAME_LENGTH			9				// Maximum length of player name
#define SCORE_LENGTH		12				// Number of digits in high scores

// Default Board Settings
#define DEFAULT_WIDTH		10
#define DEFAULT_HEIGHT		22
#define DEFAULT_LEVEL		0
#define DEFAULT_FRAME		true
#define DEFAULT_GRID		true
#define DEFAULT_FACE		true
#define DEFAULT_PERMUTE		true
#define DEFAULT_NEXT_DISPLAY true
#define DEFAULT_MUSIC		0
#define DEFAULT_PLAY_MUSIC	true
#define DEFAULT_PLAY_SOUND	true
#define DEFAULT_BACKGROUND	0

// Fixed texture values
#define T_SPLASH_SCREEN 21
#define T_BACKGROUND_1	22

// Multiplayer board coordinates
#define MAINBOARDX 0
#define MAINBOARDY -55
#define MAINBOARDZ -82
#define MAINUNITSIZE 5

#define MAINNEXTX 0
#define MAINNEXTY 40
#define MAINNEXTZ -17

#define ENEMYX 0
#define ENEMYY -55
#define ENEMYZ -21
#define ENEMYNEXTX 0
#define ENEMYNEXTY 17
#define ENEMYNEXTZ 1
#define BOARDOFFSET 37
#define ENEMYUNITSIZE 3

// Global fixed values
#define MESSAGE_BUFFSIZE 512
#define DEFAULT_IP ""
#define BT_PORT 58813
#define BT_CODE 5
#define SPECIALKEY_OFFSET	1000		// Offset used to differentiate special key input
#define EMPTY_CHARACTER		2			// Signifies placeholder for no character
#define NUMERAL_OFFSET		1			// Avoids "0" from being read as null terminator

// State Machine Codes
#define TITLE_SCREEN		1
#define OPTION_SCREEN		2
#define MULTIPLAYER			3
#define VIEW_SCORES			4
#define CONFIGURE_KEYS		5
#define SYSTEM_SCREEN		6
#define START_SINGLE_PLAYER	10
#define EXIT_SINGLE_PLAYER	11
#define LOCAL_NAME_ENTRY	12
#define SUBMIT_LOCAL		13
#define DEFAULT_GAME_OPTIONS	14
#define DEFAULT_SYSTEM_SETTINGS	15
#define SWITCH_TO_SYSTEM_SCREEN	16
#define SELECT_START_SINGLE_PLAYER 17
#define START_MULTIPLAYER	20
#define EXIT_MULTIPLAYER	21
#define READY				22
#define END_MULTIPLAYER_GAME 23
#define SERVER_NAME_ENTRY	24
#define SUBMIT_SERVER		25
#define REPORT_SCORE		26
#define RETURN_TO_ROOM		27
#define INIT_CONNECTION		30
#define HANDLE_DISCONNECT	31
#define UPDATE_MUSIC_MUTE	90
#define UPDATE_SOUND_MUTE	91
#define PLAY_GAME_TRACK		92
#define PAUSE_MUSIC			93
#define UNPAUSE_MUSIC		94
#define SPLASH_SCREEN		97
#define EXIT				98
#define EXECUTE_FULLSCREEN_TOGGLE 99

#define INPUT_MODE_SWITCH	-1099

// Command ID: Global
#define FULLSCREEN_TOGGLE	101

// Command ID: Menu
#define MENU_UP				201
#define MENU_DOWN			202
#define MENU_RIGHT			203
#define MENU_LEFT			204
#define MENU_CONFIRM		205
#define MENU_CANCEL			206

// Command ID: Game
#define GAME_SONIC_LOCK		301
#define GAME_DOWN			302
#define GAME_LEFT			303
#define GAME_RIGHT			304
#define GAME_ROTATE_LEFT	305
#define GAME_ROTATE_RIGHT	306
#define GAME_PAUSE			307

// Message Codes
#define MESSAGE_TERMINATOR 240

// Message Codes: Game State
#define S_GLOBAL			0
#define S_ROOM				1
#define S_GAME				2
#define S_MENU				3

// Message Codes: Contextless Messages
#define M_DISCONNECT		1
#define M_CONNECT			2
#define M_NEXT				8
#define M_REQUEST_STATE_REPORT	9
#define M_REQUEST_SCORE		30
#define M_SCORE_LIST		31
#define M_REPORT_SCORE		32
#define M_HIGH_SCORE_SUBMIT	33
#define M_HIGH_SCORE_ACHIEVED 34
#define M_NO_HIGH_SCORE		35

#define M_SCORE_LIST_FAILURE 1
#define M_SCORE_LIST_SUCCESS 2

// Message Codes: Room Messages
#define M_JOIN				2
#define M_LEAVE				3
#define M_READY				4
#define M_NOT_READY			5
#define M_ENTER_GAME_STATE	6
#define M_START_GAME		7
#define M_MESSAGE			8
#define M_ASSIGN_ID			9
#define M_PLAYING			10
#define M_IDLE				11
#define M_APPEARANCE		12
#define M_FRAME				13
#define M_GRID				14

// Message Codes: Game Messages
#define M_INVALID_ACT		2
#define M_GAME_OVER			3
#define M_LOCKDOWN			4
#define M_BOARD				5
#define M_REQUEST_FIX		6
#define M_GAME_END			7
#define M_TETRAD			8

// Message Codes: Client
#define C_GLOBAL			7

// Board States
#define BS_ACTIVE			0
#define BS_PREGAME			1
#define BS_POSTGAME			2
#define BS_PAUSED			3

// Music Keys
#define MUSIC_TITLE			1
#define MUSIC_GAME_OVER		2
#define MUSIC_TRACK1		11
#define MUSIC_TRACK2		12
#define MUSIC_TRACK3		13

// Sound Keys
#define SOUND_COLLISION		1
#define SOUND_LOCK			2
#define SOUND_ROTATE		3
#define SOUND_LINE_DROP		4
#define SOUND_TETRIS		14

// Color Schemes -----------------------------------------------------------------------O
//
//	Scheme 0: Super Tetris 3
//	Scheme 1: 2007 Tetris Guideline
//	Scheme 2: Game Boy
//
//	These values serve as a default for when textures are not functioning.
//
// Index Value:				   0     1     2     3     4     5     6
// Piece Identity:			   I     O     L     J     S     Z     T
const float RED[3][7] =		{ .094, .471, .125, .816, .0  , .816, .878,
.4  , .7  , .8  , .0  , .0  , .9  , .8,
.1,   .2,   .3,   .4,   .5,   .6,  .7};

const float GREEN[3][7] =	{ .094, .0  , .439, .314, .722, .094, .753,
.5  , .9  , .3  , .0  , .7  , .0  , .0 ,
.1,   .2,   .3,   .4,   .5,   .6,  .7};

const float BLUE[3][7] =	{ .659, .471, .220, .0  , .784, .0  , .094,
.8  , .2  , .1  , .9  , .1,   0,  .7 ,
.1,   .2,   .3,   .4,   .5,   .6,  .7};