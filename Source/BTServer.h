//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 31, 2007
//	Last Modified:	May 28, 2007
//	File:			BTServer.h
//	Project:		Blue Tetris
//
//	Purpose:		Functions for cBTServer: server for Blue Tetris.
//					Handles client connect and disconnect and game logic.
//
//***************************************************************************************

#pragma once

#define SERVER_MAXCLIENTS 4
#define BT_SERVER_SCOREFILE		"Data/serverscores.dat"

#include <queue>
#include <string.h>
#include <fstream>
#include "afx.h"
#include "trisBoard.h"
#include "resource.h"
#include "SQLConnection.h"
using std::ifstream;
using std::ofstream;
using std::queue;
using std::string;

// Server Function Definitions
void BTSInit();							// Default Constructor
UINT BTSRun(LPVOID pParam); 			// Runs server
UINT BTSClientRead(LPVOID pParam);		// Thread for reading client messages
UINT BTSClientSend(LPVOID pParam);		// Thread for sending client messages
UINT BTSMessageReader(LPVOID pParam);	// Thread for executing messages
bool BTSCheckValidity(string message);	// Checks if recieved message is valid
void BTSEnqueue(string message, int id); // Encodes client id, adds to incoming queue

void BTSExecute(string message);		// Executes command
void BTSHandleGlobal(string message);	// Handles global messages
void BTSHandleRoom(string message);		// Handles room messages
void BTSHandleGame(string message);		// Handles game messages

void BTSMessage(int state, int message, int id, int target);
void BTSMessage(string message);
void BTSMessageAll(const string message);
void BTSMessageAll(int state, int message, int id);
void BTSMessageAll(int state, string message, int id);
void BTSMessageOthers(const string message);

bool BTSLock(string message);			// Locks down specified tetris units
void BTSFixBoard(int id, int target);	// Dictates what the client's board looks like
void BTSReportNextList(int id);			// Dictates client's list of upcoming tetrads
void BTSReportClientStates(int id);		// Reports client states to given client

void BTSOverflowCheck(int id);			// Performs game over check
void BTSEndGame();						// Handles end of multiplayer game

void BTSSendScoreList(int client);		// Score communication functions
void BTSSubmitScore(string message);
void BTSSubmitLocalScore(string name, long double score);
bool BTSRetrieveScoreList();
void BTSSaveLocalScores();

long double BTSCharToLongDouble(char target[]);
void BTSLongDoubleToChar(char buff[], long double value);
string BTSParseName(string message, int &n);
long double BTSParseScore(string message, int &n);

bool BTSCheckID(int id);				// Checks whether ID is in valid range

// Global Variables
SOCKET mServer;							// Server socket
queue<string> mIncoming;				// Queue of incoming messages
int mInvalidMessages;					// Tracks number of ignored messages
bool mEndExecution;						// Flag server execution stop
long double mScores[10];
string mNames[10];
cSQLConnection mDatabase;
bool mDBMode;							// Database mode (off for local score storage)

// Room-specific members:
cTrisBoard mBoard[4];					// Players' boards
int mClientCount;						// Number of clients in room
queue<string> mMessages[SERVER_MAXCLIENTS]; // Message queue for all players
bool mPresent[4];						// Flags for occupied client IDs
UINT mClientMap[4];						// Maps occupied client IDs to socket id
bool mReady[4];							// Flags for players in ready state
bool mPlaying[4];						// Flags for players who are currently in game
int mState;								// Room's game state
int mLocalState;						// Substate local to server
int mDrawIndex[4];						// Tetrad drawing index for each player


//***************************************************************************************
//
//	Function:	BTSInit
//	Purpose:	Initializes client info
//
//***************************************************************************************
void BTSInit()
{
	mDBMode = true;

	for(int i(0); i < 10; i++)
	{
		mNames[i] = "No Entry";
		mScores[i] = 0;
	}

	mClientCount = 0;
	mInvalidMessages = 0;
	mEndExecution = false;

	mLocalState = 0;
	mState = 1;
	for(int i(0); i < 4; i++)
	{
		mPresent[i] = false;
		mReady[i] = false;
		mPlaying[i] = false;
	}
}

//***************************************************************************************
//
//	Function:	run
//	Purpose:	Runs Blue Tetris server
//	Return:		True if error occurs in server execution
//
//***************************************************************************************
UINT BTSRun(LPVOID pParam)
{
	mDBMode = (int)pParam;

	BTSInit();

	bool error(false);

	WSADATA wsaData;							// WSA data
	sockaddr_in local;							// Socket address
	int wsaret=WSAStartup(0x101,&wsaData);

	if(wsaret!=0)								// Check for valid WSA
		error = true;

	if(!error)
	{
		local.sin_family=AF_INET;				// Set socket data
		local.sin_addr.s_addr=INADDR_ANY;
		local.sin_port=htons((u_short)BT_PORT);
		mServer=socket(AF_INET,SOCK_STREAM,0);	// Setup socket

		if(mServer==INVALID_SOCKET)
			error = true;

		if(!error)
		{
			if(bind(mServer,(sockaddr*)&local,sizeof(local))!=0)
				error = true;

			if(!error)
			{
				if(listen(mServer,10)!=0)
					error = true;

				if(!error)
				{
					SOCKET client;				// Client socket
					sockaddr_in from;
					int fromlen=sizeof(from);

					AfxBeginThread(BTSMessageReader, 0);	// Start the message executer

					while(!mEndExecution)					// As long as the server runs
					{
						// Wait for client connection
						client=accept(mServer, (struct sockaddr*)&from,&fromlen);

						if(!mEndExecution)				// Launch thread for this client
							AfxBeginThread(BTSClientRead,(LPVOID)client);
					}
				}
			}
		}
	}

	return error;
}

//***************************************************************************************
//
//	Function:	BTSMessageReader
//	Purpose:	Grabs messages from incoming message queue and executes valid commands
//
//***************************************************************************************
UINT BTSMessageReader(LPVOID pParam)
{
	string message;

	while(true)
	{
		if(!mIncoming.empty())
		{
			message = mIncoming.front();
			mIncoming.pop();

			if(BTSCheckValidity(message))
				mInvalidMessages++;
			else
				BTSExecute(message);
		}
	}
}

//***************************************************************************************
//
//	Function:	BTSCheckValidity
//	Purpose:	Checks if recieved message is a valid Blue Tetris instruction
//	Return:		True if message is invalid
//
//***************************************************************************************
bool BTSCheckValidity(string message)
{
	bool invalid(false);
	int state = (message[0]>>3 & 0x3);

	invalid = !((message[0]>>5 & 0x7) == BT_CODE);	// Check for signature BT bits

	if(!invalid)
		invalid = !(state == mState || state == 0);	// Check for valid gamestate

	return invalid;
}

//***************************************************************************************
//
//	Function:	BTSExecute
//	Purpose:	Interprets and executes message
//
//***************************************************************************************
void BTSExecute(string message)
{
	int state = (message[0]>>3 & 0x3);

	switch(state)
	{
	case S_GLOBAL:
		BTSHandleGlobal(message);
		break;

	case S_ROOM:
		BTSHandleRoom(message);
		break;

	case S_GAME:
		BTSHandleGame(message);
		break;
	};
}

//***************************************************************************************
//
//	Function:	BTSHandleGlobal
//	Purpose:	Handles messages with global state code
//
//***************************************************************************************
void BTSHandleGlobal(string message)
{
	int id = (message[0] & 0x7);					// Mask off client ID
	int code = message[1];

	switch(code)
	{
	case M_DISCONNECT:
		mPresent[id] = false;		// Note: disconnect message broadcast by send thread
		mReady[id] = false;
		mPlaying[id] = false;

	case M_REQUEST_SCORE:				// Answer score list requests
		BTSSendScoreList(id);
		break;

	case M_HIGH_SCORE_SUBMIT:			// Submit high score to database
		BTSSubmitScore(message);
		break;

	case M_APPEARANCE:					// Setting reports: Echo to other clients
	case M_FRAME:
	case M_GRID:
		BTSMessageOthers(message);
		break;
	};
}

//***************************************************************************************
//
//	Function:	BTSHandleRoom
//	Purpose:	Handles messages with room state code
//
//***************************************************************************************
void BTSHandleRoom(string message)
{
	int id = (message[0] & 0x7);					// Mask off client ID
	int code = message[1];

	if(mLocalState == 0)					// Room Messages: Waiting ------------------
	{
		if(code == M_READY)					// Client enters ready state
		{
			mReady[id] = true;
			BTSMessageAll(message);

			bool allReady(true);
			for(int i(0); i < 4 && allReady; i++)
				if(mPresent[i])
					allReady = mReady[i];

			if(allReady)					// Broadcast Enter Game State message
			{
				string enterMsg;
				enterMsg += M_ENTER_GAME_STATE;
				enterMsg += (mPresent[0] + mPresent[1] * 2 + mPresent[2] * 4 + mPresent[3] * 8);
				BTSMessageAll(S_ROOM, enterMsg, C_GLOBAL);
				mLocalState++;
			}
		}
		if(code == M_NOT_READY)				// Client leaves ready state
		{
			mReady[id] = false;
			BTSMessageAll(message);
		}
	}

	else if(mLocalState == 1) // Room Messages: Starting Game -------
	{
		if(code == M_ENTER_GAME_STATE)		// Client enters game state
		{
			mReady[id] = false;				// Update states
			mPlaying[id] = true;
		}

		bool allReady(true);				// Check if all are ready
		for(int i(0); i < 4 && allReady; i++)
			if(mPresent[i])
				allReady = !mReady[i];

		if(allReady)					// If all are ready
		{
			for(int i(0); i < 4; i++)	// Send out initial tetrad lists
			{
				if(mPresent[i])
				{
					mBoard[i].start();
					mDrawIndex[i] = 2;
					BTSReportNextList(i);
				}
			}

			BTSMessageAll(S_ROOM, M_START_GAME, C_GLOBAL);
			mLocalState++;
			mState = S_GAME;
		}
	}
}

//***************************************************************************************
//
//	Function:	BTSHandleGame
//	Purpose:	Handles messages with game state code
//
//***************************************************************************************
void BTSHandleGame(string message)
{
	int id = (message[0] & 0x7);					// Mask off client ID
	int code = message[1];

	bool invalid(true);					// Flags whether client's action is valid

	switch(code)
	{
	case M_LOCKDOWN:
		if(BTSLock(message))
			BTSFixBoard(id, id);
		else
			BTSMessageOthers(message);

		BTSOverflowCheck(id);			// Perform check for game over
		break;

	case M_REQUEST_FIX:					// Respond to board fix requests
		BTSFixBoard(message[2] - NUMERAL_OFFSET, id);

	case M_TETRAD:
		BTSMessageOthers(message);
	};

	if(invalid)
		BTSMessage(S_GAME, M_INVALID_ACT, C_GLOBAL, id);
	else
		BTSMessageAll(message);
}

//***************************************************************************************
//
//	Function:	BTSEnqueue
//	Purpose:	Encodes client ID into message and places in incoming queue
//
//***************************************************************************************
void BTSEnqueue(string message, int id)
{
	message[0] += id;
	mIncoming.push(message);
}

void BTSMessage(int state, int message, int id, int target)
{
	char code = id + state * 8 + BT_CODE * 32;
	string newMsg;
	newMsg = code + message;

	mMessages[target].push(newMsg);
}

void BTSMessage(string message)
{
	int client = message[0] & 7;

	if(client >= 0 && client <= 4)
		mMessages[client].push(message);
}

//***************************************************************************************
//
//	Function:	BTSMessageAll
//	Purpose:	Places message in all present clients' send queues
//
//***************************************************************************************
void BTSMessageAll(int state, int message, int id)
{
	char code = id + state * 8 + BT_CODE * 32;
	string newMsg;
	newMsg = code;
	newMsg += message;
	newMsg += '\0';

	BTSMessageAll(newMsg);
}

void BTSMessageAll(int state, string message, int id)
{
	char code = id + state * 8 + BT_CODE * 32;
	string newMsg;
	newMsg = code;
	newMsg += message;

	BTSMessageAll(newMsg);
}

void BTSMessageAll(const string message)
{
	for(int i(0); i < 4; i++)
		if(mPresent[i])
			mMessages[i].push(message);
}

//***************************************************************************************
//
//	Function:	MessageOthers
//	Purpose:	Sends message to all but the client identified in the message
//
//***************************************************************************************
void BTSMessageOthers(const string message)
{
	int id = message[0] & 7;
	for(int i(0); i < 4; i++)
		if(mPresent[i] && i != id)
			mMessages[i].push(message);
}

//***************************************************************************************
//
//	Function:	BTSFixBoard
//	Purpose:	Defines contents of client's board
//				Used to clarify data inconsistancies
//
//***************************************************************************************
void BTSFixBoard(int id, int target)
{
	printf("Inconsistancy\n");
	string message;
	message += id + mState * 8 + BT_CODE * 32;
	message += M_BOARD;

	int mxSize = mBoard[id].width();
	int mySize = mBoard[id].height();

	for(int y(0); y < mySize; y++)
	{
		for(int x(0); x < mxSize; x++)
		{
			if(mBoard[id].check(x, y))
				message += mBoard[id].unit(x, y)->face() + NUMERAL_OFFSET;
			else
				message += -1;
		}
	}

	message += '\0';

	if(target = C_GLOBAL)
		BTSMessageAll(message);
	else if(target >= 0 && target < 4)
		mMessages[target].push(message);
}

//***************************************************************************************
//
//	Function:	BTSReportNextList
//	Purpose:	Reports list of upcoming tetrad types to client
//
//***************************************************************************************
void BTSReportNextList(int id)
{
	if(BTSCheckID(id))
	{
		string message;

		message += id + S_GLOBAL * 8 + BT_CODE * 32;
		message += M_NEXT;

		for(int i(0); i < 7; i++)
		{
			message += mBoard[id].getNext(i) + NUMERAL_OFFSET;
		}
		message += '\0';

		BTSMessage(message);
	}
}

//***************************************************************************************
//
//	Function:	BTSReportClientStates
//	Purpose:	Reports state of all clients to given client (connected, ready)
//
//***************************************************************************************
void BTSReportClientStates(int id)
{
	if(BTSCheckID(id))
	{
		for(int i(0); i < 4; i++)
		{
			if(mPresent[i])
				BTSMessage(S_GLOBAL, M_CONNECT, i, id);
			else
				BTSMessage(S_GLOBAL, M_DISCONNECT, i, id);

			if(mReady[i])
				BTSMessage(S_GLOBAL, M_READY, i, id);
			else
				BTSMessage(S_GLOBAL, M_NOT_READY, i, id);

			if(mPlaying[i])
				BTSMessage(S_GLOBAL, M_PLAYING, i, id);
			else
				BTSMessage(S_GLOBAL, M_IDLE, i, id);
		}
	}
}

//***************************************************************************************
//
//	Function:	BTSCheckID
//	Purpose:	Checks if ID is in valid bounds
//	Returns:	True if valid ID
//
//***************************************************************************************
bool BTSCheckID(int id)
{
	if(id >= 0 && id < 4)
		return true;
	else
		return false;
}

//***************************************************************************************
//
//	Function:	BTSLock
//	Purpose:	Locks down specified tetris units
//	Return:		True if any specified locations are occupied (data inconsistancy)
//
//***************************************************************************************
bool BTSLock(string message)
{
	bool occupied(false);
	int id = message[0] & 7;
	int type[4];
	int x[4];
	int y[4];
	int n(2);
	int i;

	if(message.length() >= 14)					// Read data from string
	{
		for(i = 0; i < 4 && !occupied; i++)
		{
			type[i] = message[n++] - NUMERAL_OFFSET;
			x[i] = message[n++] - NUMERAL_OFFSET;
			y[i] = message[n++] - NUMERAL_OFFSET;

			occupied = mBoard[id].check(x[i], y[i]); // Check for data inconsistancie
		}
	}

	if(!occupied)								// Add units if no problem
	{
		for(i = 0; i < 4; i++)
		{
			mBoard[id].add(x[i], y[i], type[i]);
		}
		mBoard[id].clearLines();
	}

	mDrawIndex[id]++;
	if(mDrawIndex[id] == 7)
	{
		mBoard[id].redraw();
		BTSReportNextList(id);
		mDrawIndex[id] = 0;
	}

	return occupied;
}

//***************************************************************************************
//
//	Function:	BTSOverflowCheck
//	Purpose:	Checks given board for gameover state
//				If all clients are at game over state, calls function to end game
//
//***************************************************************************************
void BTSOverflowCheck(int id)
{
	bool gameEnd(false);

	if(mBoard[id].gameOver())			// Check if this player has topped out
	{
		gameEnd = true;
		for(int i(0); i < 4; i++)		// For each player
		{
			if(mPresent[id] && !mBoard[id].gameOver())	// If this player is present and still playing
				gameEnd = false;				// Game has not ended
		}
	}

	if(gameEnd)							// If game has ended
		BTSEndGame();					// Call end game function
}

//***************************************************************************************
//
//	Function:	BTSEndGame
//	Purpose:	Handles end of multiplayer game
//
//***************************************************************************************
void BTSEndGame()
{
	BTSRetrieveScoreList();				// Update score list
	BTSMessageAll(S_GAME, M_GAME_END, C_GLOBAL);	// Send game end message to all clients
}

//-------------------------------------------------------------------------------------O
//
//	Score Funtions
//	The following functions communicate high scores between tiers
//

//***************************************************************************************
//
//	Function:	BTSSendScoreList
//	Purpose:	Sends high score list to client
//
//***************************************************************************************
void BTSSendScoreList(int client)
{
	bool error(false);
	int x;
	char buff[255];

	if(mDBMode)
	{
		if(BTSRetrieveScoreList())
			error = true;
	}

	string message;
	message += S_GLOBAL * 8 + BT_CODE * 32;
	message += M_SCORE_LIST;

	if(error)
		message += M_SCORE_LIST_FAILURE;
	else
	{
		message += M_SCORE_LIST_SUCCESS;

		for(int i(0); i < 10; i++)
		{
			message += mNames[i];

			for(x = mNames[i].length(); x < NAME_LENGTH; x++)
			{
				message += EMPTY_CHARACTER;
			}

			BTSLongDoubleToChar(buff, mScores[i]);
			message += buff;
		}
	}

	mMessages[client].push(message);
}

//***************************************************************************************
//
//	Function:	BTSCheckScore
//	Purpose:	Checks a client score against server high scores.
//				Returns a message to the client stating the next action.
//
//***************************************************************************************
void BTSCheckScore(string message)
{
	int id = message[0] & 7;

	int n(2);
	long double score = BTSParseScore(message, n);

	if(score > mScores[9])			// If greater than lowest high score
		BTSMessage(S_GLOBAL, M_HIGH_SCORE_ACHIEVED, id, id);
	else
		BTSMessage(S_GLOBAL, M_NO_HIGH_SCORE, id, id);
}

//***************************************************************************************
//
//	Function:	BTSSubmitScore
//	Purpose:	Submits a high score pair to the database
//
//***************************************************************************************
void BTSSubmitScore(string message)
{
	int n(2);

	string name = BTSParseName(message, n);
	long double score = BTSParseScore(message, n);

	if(mDBMode)
		mDatabase.submitScore(name.c_str(), score);
	else
		BTSSubmitLocalScore(name, score);
}

//***************************************************************************************
//
//	Function:	BTSSubmitLocalScore
//	Purpose:	Submits score and name pair stored in member variables to
//				local high score list
//
//***************************************************************************************
void BTSSubmitLocalScore(string name, long double score)
{
	bool inserted(false);

	if(score > mScores[9])
	{
		for(int i = 9; i > 0 && !inserted; i--)
		{
			if(score <= mScores[i - 1])			// If position found
			{
				mScores[i] = score;				// Insert score
				mNames[i] = name;
				inserted = true;
			}
			else								// Else
			{
				mScores[i] = score;				// Shift next score down
				mNames[i] = name;
			}
		}

		if(!inserted)							// If loop is exited, score is first
		{
			mScores[0] = score;
			mNames[0] = name;
		}
	}

	BTSSaveLocalScores();
}

//***************************************************************************************
//
//	Function:	BTSSaveLocalScores
//	Purpose:	Saves scores to local data file
//
//***************************************************************************************
void BTSSaveLocalScores()
{
	string name;
	long double score;
	char buff[256];
	ofstream fout;

	fout.open(BT_SERVER_SCOREFILE);

	if(fout)
	{
		for(int i(0); i < 10; i++)
		{
			score = mScores[i];
			name = mNames[i];

			sprintf(buff, "%.0Lf", score);
			fout << name << '\n'
				 << buff << '\n'
				 << BTChecksum(name, score) << '\n';
		}

		fout.close();
	}
}

//***************************************************************************************
//
//	Function:	BTSRetrieveScoreList
//	Purpose:	Retrieves list of high scores from database
//	Return:		True if error occurs
//
//***************************************************************************************
bool BTSRetrieveScoreList()
{
	bool error(false);
	char buff[256];
	string name;
	long double score;

	if(mDBMode)
		error = mDatabase.retrieveTable(mNames, mScores);
	else
	{
		ifstream scorefile;
		scorefile.open(BT_SERVER_SCOREFILE);
		if(scorefile)
		{
			int n(0);

			for(int i(0); i < 10; i++)
			{
				scorefile.getline(buff, 256);
				name = buff;
				scorefile.getline(buff, 256);
				score = BTSCharToLongDouble(buff);
				scorefile.getline(buff, 256);
				int l = BTChecksum(name, score);
				int m = atoi(buff);
				if(!(name == "No Entry" && score == 0))
				{
					if(BTChecksum(name, score) == atoi(buff))
					{
						mNames[n] = name;
						mScores[n] = score;
						n++;
					}
				}
			}
		}
		else
			error = true;
		scorefile.close();
	}

	return error;
}

//***************************************************************************************
//
//	Function:	BTSParseName
//	Purpose:	Parses a name from the server score list message
//	Return:		String containing player name
//
//***************************************************************************************
string BTSParseName(string message, int &n)
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
//	Function:	BTSParseScore
//	Purpose:	Parses a score from the server score list message
//	Return:		long double containing high score
//
//***************************************************************************************
long double BTSParseScore(string message, int &n)
{
	char score[SCORE_LENGTH + 1];

	for(int i(0); i < SCORE_LENGTH && n < message.length(); i++, n++)
		score[i] = message[n];

	return BTSCharToLongDouble(score);
}

//***************************************************************************************
//
//	Function:	BTSCharToLongDouble
//	Purpose:	Converts number within character array to long double
//
//***************************************************************************************
long double BTSCharToLongDouble(char target[])
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
//	Function:	BTSLongDoubleToChar
//	Purpose:	Converts a long double to a character array of size SCORE_LENGTH
//
//***************************************************************************************
void BTSLongDoubleToChar(char result[], long double value)
{
	char buff[SCORE_LENGTH + 1];
	long double divisor(1);
	int i;

	for(i = 0; i < SCORE_LENGTH - 1; i++)
		divisor = divisor * 10;

	for(i = 0; i < SCORE_LENGTH; i++)
	{
		buff[i] = ((int)(value / divisor)) + 48;	// Shave off digit, convert to ascii
		value = value - ((buff[i] - 48) * divisor);	// Remove digit from value
		divisor = divisor / 10;				// Shift divisor to next digit
	}

	buff[SCORE_LENGTH] = '\0';				// Null terminate string
	sprintf(result, buff);							// Return result
}

//--------------------------------------------------------------------------------------O
//
//	Thread Functions
//	The following functions are used by threads to communicate with clients
//

//***************************************************************************************
//
//	Function:	BTSClientRead
//	Purpose:	Thread that reads and stores messages from a client
//
//***************************************************************************************
UINT BTSClientRead(LPVOID pParam)
{
	SOCKET client=(SOCKET)pParam;				// Cast client socket from sent parameter
	char buff[MESSAGE_BUFFSIZE];				// Buffer for reading from client
	char message[MESSAGE_BUFFSIZE];				// Buffer for parsing messages
	static bool IDAssign(false);				// Flag to prevent race condition
	bool sockError(false);						// Error flag
	int r;										// Return value for message pump
	int i, j;									// Index variables
	int clientID(-1);							// Client ID for this thread

	while(IDAssign)								// If assignment is being made, spinlock
	{}

	IDAssign = true;
	mClientCount++;								// Increment client count
	for(int i(0); i < mClientCount && clientID == -1; i++)
	{
		if(!mPresent[i])
		{
			mPresent[i] = true;					// Flag ID as taken
			clientID = i;						// Assign ID for this thread
			mClientMap[clientID] = client;		// Map client ID to socket ID
		}
	}
	IDAssign = false;

	sprintf(message, "");

	message[0] = (C_GLOBAL + S_GLOBAL * 8 + BT_CODE * 32); // form ID assignment msg
	message[1] = M_ASSIGN_ID;
	message[2] = clientID + NUMERAL_OFFSET;
	send(client, message , 3, 0); // Send message to client

	printf("Client connected\n");
	AfxBeginThread(BTSClientSend, (LPVOID)client);	// Launch thread for sending data

	BTSReportClientStates(clientID);				// Report client states to newcomer

	while(!sockError && mPresent[clientID])			// Loop continuously
	{
		r=recv(client,buff,MESSAGE_BUFFSIZE,0);	// Read message from client

		if(r==SOCKET_ERROR)						// Check for error/disconnection
			sockError = true;
		else
		{
			for(i = 0, j = 0; i < r; i++)				// For each character in the buffer
			{
				if((unsigned char)buff[i] == MESSAGE_TERMINATOR)		// If end of message
				{
					message[j] = '\0';					// Null terminate message
					BTSEnqueue(string(message), clientID);	// Push on message queue
					j = 0;								// Reset position in message buffer
				}
				else									// If not end of message
				{
					message[j] = buff[i];				// Append character to message buffer
					j++;								// Increment position in message buffer
				}
			}
		}
	}

	closesocket(client);						// Close the socket
	mPresent[clientID] = false;					// Free client ID
	mClientCount--;								// Decrement client count

	return 0;
}

//***************************************************************************************
//
//	Function:	BTSClientSend
//	Purpose:	Thread that sends messages to a client
//
//***************************************************************************************
UINT BTSClientSend(LPVOID pParam)
{
	SOCKET client = (SOCKET)pParam;				// Cast parameter to get client socket
	int r;										// Return value for message pump
	char buff[MESSAGE_BUFFSIZE];				// Message buffer
	int clientID;								// Client ID for this thread
	bool IDFound(false);						// Flags when client ID is acquired
	bool sockError(false);						// Error flag
	int length;									// Stores length of buffer

	for(int i(0); i < mClientCount && !IDFound; i++)	// Acquire client ID
	{
		if(mClientMap[i] == client)
		{
			clientID = i;
			IDFound = true;
		}
	}

	if(!IDFound)
		sockError = true;

	while(!sockError && mPresent[clientID])
	{
		if(mMessages[clientID].size() > 0)		// If message in queue, send message
		{
			strcpy(buff, mMessages[clientID].front().c_str());
			mMessages[clientID].pop();

			length = strlen(buff);				// Append message terminator
			buff[length] = MESSAGE_TERMINATOR;
			buff[length + 1] = '\0';
			length++;

			r=send(client, buff, length, 0);

			if(r==SOCKET_ERROR)					// Check for socket errors
				sockError = true;
		}
	}

	BTSMessageAll(S_GLOBAL, M_DISCONNECT, clientID);	// Broadcast disconnection
	printf("Client disconnected\n");

	return 0;
}