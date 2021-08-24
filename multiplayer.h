//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 30, 2007
//	Last Modified:	May 28, 2007
//	File:			multiplayer.h
//	Project:		Blue Tetris
//
//	Purpose:		Class defintion for client-side multiplayer game algorithm.
//					Interprets input from client and communicates with server.
//
//***************************************************************************************

#pragma once

#include <queue>
#include <vector>
#include <atltime.h>			// For time manipulation

#include "game.h"
#include "trisboard.h"
#include "afx.h"
#include "resource.h"
#include "keymap.h"
#include "socketConnection.h"
using std::queue;
using std::vector;

//***************************************************************************************
//
//	Class:		cMultiplayer
//	Purpose:	Handles game progression in multiplayer mode
//
//***************************************************************************************
class cMultiplayer: public cGame
{
public:

	// Value Constructor
	cMultiplayer(bool* present, int playerID, cKeymap keymap, cConnection* connection,
		cTexture texture, bool* frame, bool* grid, int* face);
	~cMultiplayer() {}					// Default Destructor

	virtual int advance(vector<int> keys);	// Frame advancement
	virtual void display();				// Display function

	// Enqueue incoming message
	void enqueue(string msg) { mConnection->enqueue(msg); }

	// Dequeue outgoing message
	bool dequeue(string &msg);

	virtual void setTexture(cTexture texture){ mTexture = texture; mBoard[mPlayerID].setTexture(mTexture); }

	virtual long double score() { return mBoard[mPlayerID].score(); }
	virtual int level() { return mBoard[mPlayerID].level(); }

private:

	int processInput(vector<int> keys);	// Keystroke interpretor
	int processPostgame(vector<int> keys);	// Keystroke inter. for postgame
	int executeCommand();				// Executes user commands
	bool initConnection();				// Initiate connection
	void sendMessage(int state, int message);	// Translate into message and enqueue
	int readMessage();					// Grab message from incoming queue and execute
	void sendLockMessage(int units[]);	// Reports locking of units
	void reportTetrad();				// Reports the location of the falling tetrad

	void requestFix(int id);			// Requests server's state of specified board

	bool lockdown(string message);		// Handles lockdown message
	void fixBoard(string message);		// Specifies board contents
	void setNext(string message);		// Updates list of upcoming tetrads
	void updateTetrad(string message);	// Updates location of active tetrad on given board
	bool handleGlobal();				// Handles any messages with global context
	void executeGlobal();				// Executes global messages

	void forceCheck();					// Performs forced tetrad drops

	void initBoardMetrics();			// Sets board metrics based on player ID

	// Display Message Handlers
	void clearMessage(int lines) {}		// Posts line clear message to event field

	// Message decoder
	bool decode(int &gamestate, int &id, string &instruction, string message); 
	string encode(const int state, const int message);	// Message encoder

	cConnection* mConnection;			// Socket connection
	cKeymap mKeymap;					// Keymapping
	int mLocalState;					// Room state
	cTexture mTexture;					// Texture
	int mGameState;
	int mBoardState;

	CFileTime mStartTime;				// Stores time at which game began
	CFileTime mLastDropTime;			// Stores timestamp of last forced drop
	CFileTimeSpan mTotalTime;			// Logs total time for postgame

	cTrisBoard mBoard[4];				// Player and opponent boards
	bool mPresent[4];					// Flags for player presence
	char mServerName[255];				// IP of server
	SOCKET mConn;						// Socket for server communication
	int mPlayerID;						// ID of this player
	int mPlayerCount;					// Number of players in this game

	bool mKeylist[7];					// Array for keydown tracking
	int mRepeat[7];						// Array for key repeat tracking
};

//***************************************************************************************
//
//	Function:		Value Constructor
//	Purpose:		Handles value initialization
//
//***************************************************************************************
cMultiplayer::cMultiplayer(bool* present, int playerID, cKeymap keymap, 
						   cConnection* connection, cTexture texture, bool* frame, bool* grid, int* face):
mKeymap(keymap), mConnection(connection), mLocalState(0),
mPlayerID(playerID), mTexture(texture), mGameState(S_ROOM)
{
	mPlayerCount = 0;

	for(int i(0); i < 4; i++)
	{
		mPresent[i] = present[i];
		if(present[i])
			mPlayerCount++;

		mBoard[i].setTexture(texture);
		mBoard[i].setFrame(frame[i]);
		mBoard[i].setGrid(grid[i]);
		mBoard[i].setSkin(face[i]);
	}
}

//***************************************************************************************
//
//	Function:	advance
//	Purpose:	Advances game progression one frame
//
//***************************************************************************************
int cMultiplayer::advance(vector<int> keys)
{
	bool error(false);
	int returnVal(0);
	string message;
	static int reportDelay(0);

	if(mLocalState == 0)			// 1. Initiate and Report
	{
		initBoardMetrics();

		sendMessage(S_ROOM, M_ENTER_GAME_STATE);	// Report ready state
		mLocalState++;
		mGameState = S_GAME;
	}

	else if(mLocalState == 1)		// 2. Wait for Start Signal
	{
		while(handleGlobal());

		if(!dequeue(message))
		{
			if(message[1] == M_START_GAME)
				mLocalState++;
		}
	}

	else if(mLocalState == 2)		// 3. Start Game
	{
		resetCountdown();
		mStartTime = CFileTime::GetCurrentTime();	// Store starting time
		mLastDropTime = CFileTime::GetCurrentTime();
		mBoard[mPlayerID].start();
		mLocalState++;
	}

	else if(mLocalState == 3)		// 4. Command Processing Loop
	{
		int temp(0);

		while(mIncoming.size())
		{
			if(!handleGlobal())
			{
				temp = readMessage();
				if(temp != 0)
					returnVal = temp;
			}
		}

		if(mBoardState == BS_ACTIVE)
			temp = processInput(keys);
		if(mBoardState == BS_POSTGAME)
			temp = processPostgame(keys);

		if(temp != 0)
			returnVal = temp;
	}

	reportDelay++;
	if(reportDelay == 20)		// When counter expires
	{
		reportTetrad();			// Report active tetrad's location
		reportDelay = 0;
	}

	return returnVal;
}

//***************************************************************************************
//
//	Function:	processInput
//	Purpose:	Translates user input to game commands
//
//***************************************************************************************
int cMultiplayer::processInput(vector<int> keys)
{
	int i;
	int returnVal(0);
	int size = keys.size();
	int command;
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

	returnVal = executeCommand();				// Execute command array

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
//	Function:	processPostgame
//	Purpose:	Processes user input for post-game state
//
//***************************************************************************************
int cMultiplayer::processPostgame(vector<int> keys)
{
	int returnVal(0);
	int command;
	bool toggle = false;
	bool pause = false;
	static bool toggleDown(false);
	const int size = keys.size();

	for(int i(0); i < size; i++)	// Flag keys that are being pressed
	{
		command = mKeymap.map(keys[i], S_GAME);			// Map key to command	
		if(command == GAME_PAUSE)
			mKeylist[ARRAY_PAUSE] = true;
		else if(command == FULLSCREEN_TOGGLE)
			toggle = true;
	}

	if(!returnVal && toggle && !toggleDown)
		returnVal = EXECUTE_FULLSCREEN_TOGGLE;

	if(toggle)
		toggleDown = true;
	else
		toggleDown = false;

	if(pause)
		returnVal = END_MULTIPLAYER_GAME;

	return returnVal;
}

//***************************************************************************************
//
//	Function:	executeCommand
//	Purpose:	Carrys out user commands for this frame
//
//***************************************************************************************
int cMultiplayer::executeCommand()
{
	int rvalue(0);

	if(mKeylist[ARRAY_SONIC_LOCK] && mRepeat[ARRAY_SONIC_LOCK] < 1)
	{
		int units[12];
		int cleared;
		if(mBoard[mPlayerID].sonicLock(cleared, units))
		{
			playSound(SOUND_LOCK);
			sendLockMessage(units);
		}
	}

	if(mKeylist[ARRAY_DOWN] && (mRepeat[ARRAY_DOWN] == 0 || mRepeat[ARRAY_DOWN] > 3) )
	{
		int units[12];
		int cleared;
		if(mBoard[mPlayerID].moveDown(cleared, units))
		{
			playSound(SOUND_LOCK);
			sendLockMessage(units);
		}
	}

	if(mKeylist[ARRAY_RIGHT] && (mRepeat[ARRAY_RIGHT] == 0 || mRepeat[ARRAY_RIGHT] > 3) )
		mBoard[mPlayerID].moveRight();

	if(mKeylist[ARRAY_LEFT] && (mRepeat[ARRAY_LEFT] == 0 || mRepeat[ARRAY_LEFT] > 3) )
		mBoard[mPlayerID].moveLeft();

	if(mKeylist[ARRAY_ROTATE_LEFT] && !mRepeat[ARRAY_ROTATE_LEFT])
	{
		rvalue = mBoard[mPlayerID].rotateLeft();

		if(rvalue == 1)
			playSound(SOUND_COLLISION);
		else if(rvalue == 0)
			playSound(SOUND_ROTATE);
	}

	if(mKeylist[ARRAY_ROTATE_RIGHT] && !mRepeat[ARRAY_ROTATE_RIGHT])
	{
		rvalue = mBoard[mPlayerID].rotateRight();

		if(rvalue == 1)
			playSound(SOUND_COLLISION);
		else if(rvalue == 0)
			playSound(SOUND_ROTATE);
	}

	forceCheck();		// Force tetrad downward at fixed interval

	if(mBoard[mPlayerID].gameOver())
	{
		mTotalTime = CFileTime::GetTickCount() - mStartTime;
		mBoardState = BS_POSTGAME;
	}

	return rvalue;
}

//**************************************************************************************
//
//	Function:	forceCheck
//	Purpose:	Forces active tetrad downward at timed intervals
//
//**************************************************************************************
void cMultiplayer::forceCheck()
{
	CFileTimeSpan timeDiff = CFileTime::GetTickCount() - mLastDropTime;

	while((timeDiff.GetTimeSpan() / 10000) > BTDropInterval(mBoard[mPlayerID].level()))
	{
		int units[12];
		int cleared;
		mLastDropTime += BTDropInterval(mBoard[mPlayerID].level()) * 10000;
		timeDiff = CFileTime::GetCurrentTime() - mLastDropTime;

		if(mBoard[mPlayerID].forceDown(cleared, units))			// Force tetrad downward
		{
			playSound(SOUND_LOCK);
			sendLockMessage(units);
		}

		if(cleared > 0)							// Report any line clears
			clearMessage(cleared);
	}
}

//**************************************************************************************
//
//	Function:		initConnection
//	Purpose:		Attempts to connect to server;
//					If successful, launches read and send threads
//	Return:			True if error occurs
//
//**************************************************************************************
bool cMultiplayer::initConnection()
{
	bool error(false);								// Error flag

	hostent *hp;
	sockaddr_in server;
	unsigned int addr;
	int r;											// Message return value
	char buff[MESSAGE_BUFFSIZE];					// Message buffer
	WSADATA wsaData;

	sprintf(mServerName, DEFAULT_IP);				// Default servername

	int wsaret=WSAStartup(0x101,&wsaData);
	if(wsaret)	
		error = true;

	if(!error)
	{
		mConn=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	// Create connection

		if(mConn==INVALID_SOCKET)						// Check for socket validity
			error = true;

		if(!error)
		{
			if(inet_addr(mServerName)==INADDR_NONE)			// Locate server
			{
				hp=gethostbyname(mServerName);
			}
			else
			{
				addr=inet_addr(mServerName);
				hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
			}

			if(hp==NULL)									// Check for valid host loc
			{
				closesocket(mConn);
				error = true;
			}

			if(!error)
			{
				server.sin_addr.s_addr=*((unsigned long*)hp->h_addr);
				server.sin_family=AF_INET;
				server.sin_port=htons(BT_PORT);

				if(connect(mConn,(struct sockaddr*)&server,sizeof(server)))
				{
					closesocket(mConn);
					error = true;
				}

				if(!error)
				{
					r=recv(mConn,buff,MESSAGE_BUFFSIZE,0);	// Get response from server
					buff[r]=0;

					if(r != 3)								// Check for correct message
						error = true;
					else
					{
						buff[r] = '\0';						// Add null terminator
						ASSERT(mIncoming.empty());
						mIncoming.push(string(buff));		// Push message on queue
						handleGlobal();
					}

					if(r==SOCKET_ERROR)
						printf(" -Socket Error\n");
					else
					{
						printf(" -Connection established.\n");

						AfxBeginThread(BTCRead, (LPVOID)mConn);	// Start reading thread
						AfxBeginThread(BTCSend, (LPVOID)mConn);	// Start sending thread
					}
				}
			}
		}
	}

	return error;
}

//**************************************************************************************
//
//	Function:	sendMessage
//	Purpose:	Encodes command into message and places message on queue
//
//**************************************************************************************
void cMultiplayer::sendMessage(int state, int message)
{
	string newMsg = encode(state, message);
	enqueue(newMsg);
}

//**************************************************************************************
//
//	Function:	sendLockMessage
//	Purpose:	Reports lockdown of a tetrad
//
//**************************************************************************************
void cMultiplayer::sendLockMessage(int units[])
{
	int n(0);
	string message;
	message += S_GAME * 8 + BT_CODE * 32;
	message += M_LOCKDOWN;

	for(int i(0); i < 4; i++)
	{
		message += units[n++] + NUMERAL_OFFSET;
		message += units[n++] + NUMERAL_OFFSET;
		message += units[n++] + NUMERAL_OFFSET;
	}
	message += '\0';

	enqueue(message);
}

//**************************************************************************************
//
//	Function:	readMessage
//	Purpose:	Grabs message from queue and executes command
//	Returns:	State machine return value
//
//**************************************************************************************
int cMultiplayer::readMessage()
{
	int returnValue(0);				// Value returned from this execution

	while(handleGlobal());			// Handle all pending global messages

	string message;
	bool invalid = dequeue(message);
	int gamestate, id;
	string instruction;

	if(!invalid)
		invalid = decode(gamestate, id, instruction, message);

	if(!invalid)
		invalid = (gamestate != mGameState);

	if(!invalid)
	{
		if(gamestate == S_GAME)					// Handle game instructions
		{
			switch(instruction[0])
			{
			case M_LOCKDOWN:
				if(lockdown(message))
					requestFix(id);
				break;

			case M_BOARD:
				fixBoard(message);
				break;

			case M_TETRAD:
				updateTetrad(message);
				break;

			case M_GAME_END:
				returnValue = END_MULTIPLAYER_GAME;
				break;
			};
		}
	}

	return returnValue;
}

//***************************************************************************************
//
//	Function:	lockdown
//	Purpose:	Executes lockdown message
//
//***************************************************************************************
bool cMultiplayer::lockdown(string message)
{
	int id = message[0] & 7;
	bool occupied(false);

	if(id != mPlayerID && id < 4 && id >= 0)
	{
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

				occupied = mBoard[id].check(x[i], y[i]); // Check for data inconsistancies
			}
		}

		if(!occupied)								// Add units if no problem
		{
			for(i = 0; i < 4; i++)
			{
				mBoard[id].add(x[i], y[i], type[i]);
				mBoard[id].nextTetrad();
			}
			mBoard[id].clearLines();
		}

	}

	return occupied;
}

//***************************************************************************************
//
//	Function:	fixBoard
//	Purpose:	Specifies contents of player's board
//
//***************************************************************************************
void cMultiplayer::fixBoard(string message)
{
	int id = message[0] & 7;
	int n(2);

	int mxSize = mBoard[id].width();
	int mySize = mBoard[id].height();

	if(id >= 0 && id <= 4)
	{
		for(int y(0); y < mySize; y++)
		{
			for(int x(0); x < mxSize; x++)
			{
				if(message[n] == -1)
					mBoard[id].erase(x, y);
				else
					mBoard[id].add(x, y, message[n] - NUMERAL_OFFSET);

				n++;
			}
		}
	}
}

//***************************************************************************************
//
//	Function:	setNext
//	Purpose:	Handles message containing upcoming tetrad list
//
//***************************************************************************************
void cMultiplayer::setNext(string message)
{
	int next[7];
	int id = message[0] & 7;

	if(id >= 0 && id <= 4)
	{
		for(int i(0); i < 7; i++)
			next[i] = message[i + 2] - NUMERAL_OFFSET;

		mBoard[id].setNext(next);
	}
}

//***************************************************************************************
//
//	Function:	requestFix
//	Purpose:	Requests fixBoard message for specified player
//				Used to fix client-side inconsistancies for opponent boards
//
//***************************************************************************************
void cMultiplayer::requestFix(int id)
{
	string message;
	message += mGameState * 8 + BT_CODE * 32;
	message += M_REQUEST_FIX;
	message += id + NUMERAL_OFFSET;
	message += '\0';

	enqueue(message);
}

//***************************************************************************************
//
//	Function:	reportTetrad
//	Purpose:	Sends a report of the location of the active tetrad
//
//***************************************************************************************
void cMultiplayer::reportTetrad()
{
	string message;
	cTetrad* tetrad = mBoard[mPlayerID].getTetradPtr();

	if(tetrad)
	{
		message += mGameState * 8 + BT_CODE * 32;
		message += M_TETRAD;
		message += tetrad->type();
		
		for(int i(0); i < 4; i++)
		{
			message += tetrad->unit(i)->x() + NUMERAL_OFFSET;
			message += tetrad->unit(i)->y() + NUMERAL_OFFSET;
		}
	}
}

//***************************************************************************************
//
//	Function:	updateTetrad
//	Purpose:	Updates location of the active tetrad on the specified board
//
//***************************************************************************************
void cMultiplayer::updateTetrad(string message)
{
	if(message.length() >= 11)
	{
		int id = message[0] & 7;

		int x[4];		// Arrays for storing coordinates
		int y[4];
		int n(3);		// Index for message
		int type = message[2];

		for(int i(0); i < 4; i++)	// For each coordinate
		{
			x[i] = message[n++] - NUMERAL_OFFSET;	// Decode coordinates
			y[i] = message[n++] - NUMERAL_OFFSET;
		}

		cTetrad* tetrad = mBoard[id].getTetradPtr();
		if(!tetrad)
			tetrad = new cTetrad(type, mBoard[id].width(), mBoard[id].height());
		tetrad->setUnits(x, y);	// Set coordinates
	}
}

//***************************************************************************************
//
//	Function:	handleGlobal
//	Purpose:	Determines whether next message has global context. If so,
//				message is executed.
//	Return:		True if global message found
//
//***************************************************************************************
bool cMultiplayer::handleGlobal()
{
	bool global(false);

	if(!mIncoming.empty())
	{
		if((mIncoming.front()[0]>>3 & 3) == S_GLOBAL)
		{
			global = true;
			executeGlobal();
		}
	}

	return global;
}

//***************************************************************************************
//
//	Function:		executeGlobal
//	Purpose:		Executes a global command
//
//***************************************************************************************
void cMultiplayer::executeGlobal()
{
	string buff = mIncoming.front();
	mIncoming.pop();

	int client = buff[0] & 7;
	int message = buff[1];

	if(client == C_GLOBAL)
	{
		if(message == M_ASSIGN_ID && buff.length() > 2)
			mPlayerID = buff[2] - NUMERAL_OFFSET;
	}
	else
	{
		if(message == M_DISCONNECT)
		{
			if(client == mPlayerID)
				mConnection->disconnect();

			mPresent[client] = false;
		}

		if(message == M_CONNECT)
			mPresent[client] = true;

		if(message == M_NEXT)
			setNext(buff);
	}
}

//***************************************************************************************
//
//	Function:		encode
//	Purpose:		Encodes message string
//	Returns:		Encoded message string
//
//**************************************************************************************
string cMultiplayer::encode(const int state, const int message)
{
	string newMsg;
	newMsg += state * 8 + BT_CODE * 32;
	newMsg += message;

	return newMsg;
}

//**************************************************************************************
//
//	Function:		decode
//	Purpose:		Decodes message string
//	Returns:		True if recieved message does not match Blue Tetris formatting
//					Return by reference: game state, target client id, instruction
//
//**************************************************************************************
bool cMultiplayer::decode(int &gamestate, int &id, string &instruction, string message)
{
	bool invalid(false);

	if(message.length() < 2)
		invalid = true;
	else
	{
		invalid = ((message[0]>>5 & 0x7) != BT_CODE);			// Check for signature BT bits

		if(!invalid)
		{
			gamestate = (message[0]>>3 & 0x3);			// Mask off gamestate
			id = (message[0] & 0x7);					// Mask off client ID

			instruction = message[1];
		}
	}

	return invalid;
}

//***************************************************************************************
//
//	Function:		dequeue
//	Purpose:		Grabs next item from the queue
//	Return:			True if nothing on queue; message returned by reference
//
//***************************************************************************************
bool cMultiplayer::dequeue(string &msg)
{
	return mConnection->dequeue(msg);
}

//**************************************************************************************
//
//	Function:		initBoardMetrics
//	Purpose:		Initializes board positions, placing the player on the left
//					and the competetors numerically ordered on the right
//
//**************************************************************************************
void cMultiplayer::initBoardMetrics()
{
	int opponentZ = ENEMYZ;
	int nextZ = ENEMYNEXTZ;

	int n(0);

	for(int i(0); i < 4; i++)
	{
		mBoard[i].setAutonomy(false);

		if(i == mPlayerID)
		{
			mBoard[i].setXOrigin(MAINBOARDX);
			mBoard[i].setYOrigin(MAINBOARDY);
			mBoard[i].setZOrigin(MAINBOARDZ);
			mBoard[i].setUnitSize(MAINUNITSIZE);
			mBoard[i].setNextX(MAINNEXTX);
			mBoard[i].setNextY(MAINNEXTY);
			mBoard[i].setNextZ(MAINNEXTZ);
		}
		else if(mPresent[i] && n != 4)
		{
			mBoard[i].setXOrigin(ENEMYX);
			mBoard[i].setYOrigin(ENEMYY);
			mBoard[i].setZOrigin(opponentZ);
			mBoard[i].setUnitSize(ENEMYUNITSIZE);
			mBoard[i].setNextX(ENEMYNEXTX);
			mBoard[i].setNextY(ENEMYNEXTY);
			mBoard[i].setNextZ(nextZ);

			opponentZ += BOARDOFFSET;
			nextZ += BOARDOFFSET;

			n++;
		}
	}
}

//**************************************************************************************
//
//	Function:		display
//	Purpose:		Displays playing board and interface
//
//***************************************************************************************
void cMultiplayer::display()
{
	const int PLvX(-5);
	const int PLvY(9);

	const int ELvX(-3);
	const int ELvY(7);
	const int ELvOffset(4);

	int enemyLvX(ELvX);

	for(int i(0); i < 4; i++)
	{
		if(mPresent[i])
		{
			mBoard[i].display();

			if(i == mPlayerID)
			{
				glColor3f(0.7, 1.0, 0.7);
				displayText(-100, PLvY, PLvX, 0, 0, 0, false, "Lv %i", mBoard[i].level());
				displayText(-100, 5, 8, 0, 0, 0, false, "Score: %i", mBoard[i].score());
			}
			/*else
			{
				displayText(-100, ELvY, enemyLvX, 0, 0, 0, false, "Lv %i", mBoard[i].level());
				enemyLvX += ELvOffset;
			}*/
		}
	}
}