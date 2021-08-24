//***************************************************************************************
//	
//	Author:			Tom Franz
//	Date Created:	November 16, 2006
//	Last Modified:	May 28, 2007
//	File:			trisboard.h
//	Project:		Blue Tetris
//
//	Purpose:		Class definition for Blue Tetris playing board.
//					Each instance represents one player's board.
//					Renders in openGL.
//
//***************************************************************************************

#pragma once

// Local defines
#define BOARDWIDTH	10
#define BOARDDEPTH	22
#define XORIGIN		0
#define YORIGIN		0
#define ZORIGIN		100
#define UNITSIZE	5

#define FRAME_R		0.3		// Color settings for board attributes
#define FRAME_G		0.3
#define FRAME_B		0.3
#define BAR_R		0.7
#define BAR_G		0.6
#define BAR_B		0.6
#define GRID_R		0.0
#define GRID_G		0.2
#define GRID_B		0.2

// Include
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <vector>
#include "tetrad.h"
#include "trisunit.h"
#include "texture.h"
#include "resource.h"

using std::vector;

//***************************************************************************************
//
//	Class:		cTrisBoard
//	Purpose:	Represents playing board for Blue Tetris
//
//***************************************************************************************
class cTrisBoard
{
public:
	cTrisBoard();								// Constructors
	cTrisBoard(int columns, int rows, int xOrigin, int yOrigin, int zOrigin,
		int unitSize, cTexture texture, bool grid, bool frame, int face, bool permute, 
		int level, bool displayNext);
	~cTrisBoard();								// Destructor

	void clear();								// Clears the board
	bool remove(int rx, int ry);				// Removes unit in given coord

	bool add(int x, int y, int face);			// Single unit insertion/overwrite
	bool erase(int x, int y);					// Single unit deletion

	void display();								// Displays all units in board

	void start();								// Puts playing board in active state
	int clearLines();							// Clears any full lines
	void nextTetrad();							// Advances to next tetrad

	int sonicLock(int &cleared);				// Tetrad movement functions
	int sonicLock(int &cleared, int units[]);
	int moveDown(int &cleared);
	int moveDown(int &cleared, int units[]);
	int forceDown(int &cleared);
	int forceDown(int &cleared, int units[]);
	int moveLeft();
	int moveRight();

	int rotateLeft();							// Rotation function
	int rotateRight();

	void redraw();

	// Setters
	bool setWidth(int width);
	bool setHeight(int height);
	void setXOrigin(float x) { mxOrigin = x; }
	void setYOrigin(float y) { myOrigin = y; }
	void setZOrigin(float z) { mzOrigin = z; }
	void setUnitSize(float size) { mUnitSize = size; }
	void setFrame(bool state) { mFrame = state; }
	void setGrid(bool state) { mGrid = state; }
	bool setSkin(int skin) { if(skin >= 0 && skin <= 3) { mScheme = skin; return false; } else return true; }
	bool setNextDisplay(bool state) { mNextDisplay = state; }
	void setPermutation(bool state) { mPermute = state; }
	void setNextX(float x) { mNextx = x; }
	void setNextY(float y) { mNexty = y; }
	void setNextZ(float z) { mNextz = z; }
	void setNextSize(float size) { mNextSize = size; }
	void setNext(int list[]);
	void setAutonomy(bool state) { mAutonomous = state; }
	void setTexture(cTexture texture) { mTexture = texture; }

	// Getters
	cTetrad getTetrad() { return (*mActiveTetrad); }
	cTetrad* getTetradPtr() { return mActiveTetrad; }
	cTrisUnit* unit(int x, int y) { if(check(x, y)) return mBoard[x][y]; else return NULL; }
	int getNext(int n) { if(n >=0 && n <= 7) return mTetradList[n]; else return -1; }
	int width() { return mxSize; }
	int height() { return mySize; }
	int level() { return mLevel; }
	int singles() { return mClears[0]; }
	int doubles() { return mClears[1]; }
	int triples() { return mClears[2]; }
	int tetrises() { return mClears[3]; }
	int remaining() { return mRemaining; }
	unsigned long double score() { return mScore; }
	bool gameOver() { overflowCheck(); return mGameOver; }
	bool nextDisplay() { return mNextDisplay; }

	bool check(int x, int y);					// Checks if location is occupied/OB

private:

	void displayBar();							// Displays overflow bar on top row
	void displayFrame();						// Draws frame around board
	void displayGrid();							// Draws grid within board
	void displayUnits();						// Display Units in grid
	void displayUnit(cTrisUnit* unit);			// Displays individual tetris unit
	void displayActiveTetrad();					// Displays active tetrad
	void displayNextTetrad();					// Displays next tetrad

	// Displays Unit with origin at given coordinate
	void displayUnitAbsolute(cTrisUnit* unit, float x, float y, float z, float unitSize);

	bool overflowCheck();						// Checks for board overflow
	void shiftDown(int y, int lines);			// Shifts units in given line down

	void lineScore(const int lines);			// Increases score upon line clear
	void dropScore();							// Increases score upon manual drop

	bool checkLine(int y);						// Checks if given line is full
	void clearLine(int y);						// Clears units from given line
	void levelCheck(int lines);					// Handles levelling

	void primeTetrads();						// Initializes tetrads
	void drawTetrads();							// Draws tetrad sequence
	void lockTetrad();							// Locks tetrad in place
	void drawRandomTetrads();					// Draws next tetrads randomly

	void initBoard();							// Initializes playing board data container
	void updateMetrics();						// Sets unit size to enforce aspect ratio of board on screen

	bool checkWidth();							// Verifies that width is in valid range
	bool checkHeight();							// Verifies that height is in valid range

	cTetrad* mActiveTetrad;						// Pointer to active tetrad
	cTetrad* mNextTetrad;						// Pointer to next tetrad

	int mTetradList[7];							// List of next tetrad pieces
	int mIndex;									// Current location in list

	vector< vector<cTrisUnit*> > mBoard;		// Playing board
	int	mxSize;									// Column count
	int mySize;									// Row count

	// Game options: graphic
	bool mFrame;								// Toggles frame display
	bool mGrid;									// Toggles grid display
	int mScheme;								// Graphic scheme
	bool mNextDisplay;							// Toggles display of next tetrad

	// Game options: functional
	bool mPermute;								// Toggles tetrad generation method

	float mxOrigin;								// Board origin
	float myOrigin;
	float mzOrigin;
	float mUnitSize;							// Size of each unit on grid

	float mNextx;								// Location of next tetrad display
	float mNexty;
	float mNextz;
	float mNextSize;

	bool mAutonomous;							// Flags reliance on server

	cTexture mTexture;							// Texture library

	// Game info
	int mLevel;						// Playing level
	int mClears[4];					// Clear count for each clearing magnitude
	unsigned long double mScore;	// Player's score
	int mRemaining;					// Remaining lines for this level
	bool mGameOver;					// Flags game end (board goes idle)
};

//***************************************************************************************
//
//	Function:	default constructor
//	Purpose:	Intializes board with defaulted dimensions
//
//***************************************************************************************
cTrisBoard::cTrisBoard(): mxSize(BOARDWIDTH), mySize(BOARDDEPTH), mxOrigin(XORIGIN),
myOrigin(YORIGIN), mzOrigin(ZORIGIN), mUnitSize(UNITSIZE), mFrame(true),
mGrid(false), mScheme(0), mActiveTetrad(NULL), mNextTetrad(NULL), mPermute(true),
mAutonomous(true), mIndex(7), mLevel(0), mGameOver(false), mNextDisplay(true)
{ 
	mActiveTetrad = NULL;
	mNextTetrad = NULL;
	initBoard();
}

//***************************************************************************************
//
//	Function:	value constructor
//	Purpose:	Initializes board with given dimensions
//
//***************************************************************************************
cTrisBoard::cTrisBoard(int columns, int rows, int xOrigin, int yOrigin, int zOrigin,
					   int unitSize, cTexture texture, bool grid = false, bool frame = true, 
					   int face = 0, bool permute = true, int level = 0, bool displayNext = true):
mxSize(columns), mySize(rows), mxOrigin(xOrigin), myOrigin(yOrigin),
mzOrigin(zOrigin), mUnitSize(unitSize), mGrid(grid), mFrame(frame),
mScheme(face), mActiveTetrad(NULL), mNextTetrad(NULL), mLevel(level),
mPermute(permute), mAutonomous(true), mIndex(7), mGameOver(false), mNextDisplay(displayNext)
{
	mTexture = texture;
	mActiveTetrad = NULL;
	mNextTetrad = NULL;
	mNextx = 0;
	mNexty = 37;
	mNextz = 45;
	mNextSize = UNITSIZE;
	updateMetrics();
	initBoard();
}	

//***************************************************************************************
//
//	Function:	destructor
//	Purpose:	Frees memory upon board destruction
//
//***************************************************************************************
cTrisBoard::~cTrisBoard()
{
	if(mActiveTetrad)
		delete mActiveTetrad;

	if(mNextTetrad)
		delete mNextTetrad;

	clear();
}

//***************************************************************************************
//
//	Function:	start
//	Purpose:	Puts playing board in active state
//
//***************************************************************************************
void cTrisBoard::start()
{
	mGameOver = false;
	primeTetrads();
}

//***************************************************************************************
//
//	Function:	setNext
//	Purpose:	Populates list of upcoming tetrads
//
//***************************************************************************************
void cTrisBoard::setNext(int list[])
{
	for(int i(0); i < 7; i++)
		mTetradList[i] = list[i];

	mIndex = 0;
}

//***************************************************************************************
//
//	Function:	primeTetrads
//	Purpose:	Draws initial tetrad sequence and assigns active and next tetrad members
//
//***************************************************************************************
void cTrisBoard::primeTetrads()
{
	if(mAutonomous)
	{
		srand( (unsigned)time( NULL ) );		// Seed randomizer

		drawTetrads();
		mActiveTetrad = new cTetrad(mTetradList[0], mxSize, mySize);
		mNextTetrad = new cTetrad(mTetradList[1], mxSize, mySize);
		mIndex = 2;
	}
}

//***************************************************************************************
//
//	Function:	nextTetrad
//	Purpose:	Advances to next tetrad:
//				Active Tetrad = Next Tetrad
//				Next Tetrad = new tetrad from list
//
//***************************************************************************************
void cTrisBoard::nextTetrad()
{
	if(!mGameOver)
	{
		mActiveTetrad = mNextTetrad;	// Next becomes active

		if(mIndex > 6)					// If end of list reached
		{
			if(mAutonomous)
			{
				if(mPermute)
					drawTetrads();				// Permute new list
				else
					drawRandomTetrads();		// Draw new random list
			}
		}

		if(mIndex < 7)
		{
			mNextTetrad = new cTetrad(mTetradList[mIndex], mxSize, mySize);	// Draw new next
			mIndex++;						// Increase index in list of next tetrads
		}
		else
			mNextTetrad = NULL;
	}
}

//***************************************************************************************
//
//	Function:	redraw
//	Purpose:	Forces redraw of next tetrad set
//
//***************************************************************************************
void cTrisBoard::redraw()
{
	if(mPermute)
		drawTetrads();
	else
		drawRandomTetrads();
}

//***************************************************************************************
//
//	Function:	drawTetrad
//	Purpose:	Draws individual tetrad randomly
//
//***************************************************************************************
void cTrisBoard::drawRandomTetrads()
{
	int temp, a, b;
	srand((unsigned)time( NULL ));	// Seed randomizer

	for(int i(0); i < 7; i++)		// Populate list
		mTetradList[i] = rand() % 7;

	for(int i(0); i < 7; i++)		// Shuffle list
	{
		a = rand() % 7;
		temp = mTetradList[i];
		
		if(mTetradList[i] == mTetradList[a])
			mTetradList[i] = rand() % 7;
		else
			mTetradList[i] = mTetradList[a];

		mTetradList[a] = temp;
	}

	mIndex = 0;
}

//***************************************************************************************
//
//	Function:	drawTetrads
//	Purpose:	Fills tetrad list with a set of all seven tetrads, permuted
//				Behavior reflects The Tetris Company's "Random Generator" algorithm
//
//***************************************************************************************
void cTrisBoard::drawTetrads()
{
	int temp, a, b;
	srand((unsigned)time( NULL ));	// Seed randomizer

	for(int i(0); i < 7; i++)		// Populate list
		mTetradList[i] = i;

	for(int i(0); i < 7; i++)		// Shuffle list
	{
		a = rand() % 7;
		temp = mTetradList[i];
		mTetradList[i] = mTetradList[a];
		mTetradList[a] = temp;
	}

	mIndex = 0;						// Reset list index
}

//***************************************************************************************
//
//	Function:	initBoard
//	Purpose:	Initializes playing board
//
//***************************************************************************************
void cTrisBoard::initBoard()
{
	int i;

	// Initialize mBoard data
	mBoard.resize(mxSize);

	for(i = 0; i < mxSize; i++)
	{
		mBoard[i].resize(mySize);
	}

	// Initialize statistic data
	mScore = 0;

	mRemaining = mLevel * 10;
	if(mRemaining < 10)
		mRemaining = 10;
	if(mRemaining > 100)
		mRemaining = 100;

	for(i = 0; i < 4; i++)
		mClears[i] = 0;
}

//***************************************************************************************
//
//	Function:	clear
//	Purpose:	Clears all units from the board
//
//***************************************************************************************
void cTrisBoard::clear()
{
	for(int i = 0; i < mxSize; i++)				// For each row
	{
		for(int j = 0; j < mySize; j++)			// For each column
			remove(i, j);
	}
}

//***************************************************************************************
//
//	Function:	remove
//	Purpose:	Removes the tetris unit in the specified row and column
//	Return:		True if block is not present
//
//***************************************************************************************
bool cTrisBoard::remove(int rx, int ry)
{
	if(mBoard[rx][ry])
	{
		delete mBoard[rx][ry];
		return false;
	}
	else
		return true;
}

//***************************************************************************************
//
//	Function:	add
//	Purpose:	Introduces a new unit to the grid
//	Return:		True if invalid location for current board metrics
//
//***************************************************************************************
bool cTrisBoard::add(int x, int y, int face)
{
	bool invalid(false);

	if(x < 0 || x >= mxSize || y < 0 || y >= mySize)
		invalid = true;
	else
	{
		if(mBoard[x][y])
			delete mBoard[x][y];

		mBoard[x][y] = new cTrisUnit(x, y, face);
	}

	return invalid;
}

//***************************************************************************************
//
//	Function:	erase
//	Purpose:	Removes tetris unit in given location
//	Return:		True if invalid location for current board metrics
//
//***************************************************************************************
bool cTrisBoard::erase(int x, int y)
{
	bool invalid(false);

	if(x < 0 || x >= mxSize || y < 0 || y >= mySize)
		invalid = true;
	else
	{
		if(mBoard[x][y])
			delete mBoard[x][y];

		mBoard[x][y] = NULL;
	}

	return invalid;
}

//***************************************************************************************
//
//	Function:	sonicLock
//	Purpose:	Instantaneously drops and locks down tetrad
//	Return:		True if collision occurs (which it really should)
//				Cleared line count passed by reference
//
//***************************************************************************************
int cTrisBoard::sonicLock(int &cleared)
{
	bool collision(false);
	cleared = 0;

	if(mActiveTetrad)
	{
		while(!moveDown(cleared))			// Move down until no longer possible
			dropScore();					// Drop score is doubled for sonic lock
		collision = true;					// Collision has occurred
		dropScore();
	}
	else
		collision = -1;

	return collision;
}

int cTrisBoard::sonicLock(int &cleared, int units[])
{
	bool collision(false);
	cleared = 0;

	if(mActiveTetrad)
	{
		while(!moveDown(cleared, units))	// Move down until no longer possible
			dropScore();					// Drop score is doubled for sonic lock
		collision = true;					// Collision has occurred
		dropScore();
	}
	else
		collision = -1;

	return collision;
}

//***************************************************************************************
//
//	Function:	moveRight
//	Purpose:	Attempts to move active tetrad right one unit
//	Return:		Number of lines cleared
//
//***************************************************************************************
int cTrisBoard::moveRight()
{
	int collision(false);

	if(mActiveTetrad)
	{
		//	Collision check
		for(int i(0); i < 4 && !collision; i++)
			collision = check(mActiveTetrad->unit(i)->x() + 1, mActiveTetrad->unit(i)->y());

		if(!collision)
			mActiveTetrad->moveRight();
	}
	else
		collision = -1;

	return collision;
}

//***************************************************************************************
//
//	Function:	moveLeft
//	Purpose:	Attempts to move active tetrad left one unit
//	Return:		True if unit is unable to move due to collision
//
//***************************************************************************************
int cTrisBoard::moveLeft()
{
	int collision(false);

	if(mActiveTetrad)
	{
		// Collision Check
		for(int i(0); i < 4 && !collision; i++)
			collision = check(mActiveTetrad->unit(i)->x() - 1, mActiveTetrad->unit(i)->y());

		if(!collision)
			mActiveTetrad->moveLeft();
	}
	else
		collision = -1;

	return collision;
}

//***************************************************************************************
//
//	Function:	moveDown
//	Purpose:	Attempts to move active tetrad down one unit
//				Increases score per unit drop
//	Return:		Number of cleared lines
//				Cleared line count passed by reference
//				Overload returns array of size 12 containing unit data
//
//***************************************************************************************
int cTrisBoard::moveDown(int &cleared)
{
	if(mActiveTetrad)
		dropScore();
	return forceDown(cleared);
}

int cTrisBoard::moveDown(int &cleared, int units[])
{
	if(mActiveTetrad)
		dropScore();
	return forceDown(cleared, units);
}

//***************************************************************************************
//
//	Function:	forceDown
//	Purpose:	Attempts to move active tetrad down one unit
//	Return:		Number of cleared lines
//				Cleared line count passed by reference
//				Overload returns array of size 12 containing unit data
//
//***************************************************************************************
int cTrisBoard::forceDown(int &cleared)
{
	int collision(false);
	cleared = 0;

	if(mActiveTetrad)
	{
		// Collision Check
		for(int i(0); i < 4 && !collision; i++)
			collision = check(mActiveTetrad->unit(i)->x(), mActiveTetrad->unit(i)->y() - 1);

		if(!collision)
			mActiveTetrad->moveDown();
	}
	else
	{
		collision = -1;
		nextTetrad();
		if(!mActiveTetrad)
			nextTetrad();
	}

	if(collision > 0)				// If collision occurs, tetrad becomes locked
	{
		lockTetrad();				// Lock down this tetrad
		cleared = clearLines();		// Clear any full lines
		if(!overflowCheck())		// Check for board overflow
		{
			nextTetrad();			// Fire next tetrad
		}
		else
			mGameOver = true;
	}

	return collision;
}

int cTrisBoard::forceDown(int &cleared, int units[])
{
	int collision(false);
	cleared = 0;

	if(mActiveTetrad)
	{
		// Collision Check
		for(int i(0); i < 4 && !collision; i++)
			collision = check(mActiveTetrad->unit(i)->x(), mActiveTetrad->unit(i)->y() - 1);

		if(!collision)
			mActiveTetrad->moveDown();
	}
	else
	{
		collision = -1;
		nextTetrad();
		if(!mActiveTetrad)
			nextTetrad();
	}

	if(collision > 0)				// If collision occurs, tetrad becomes locked
	{
		int n(0);

		for(int i(0); i < 4; i++)
		{
			units[n++] = mActiveTetrad->unit(i)->face();
			units[n++] = mActiveTetrad->unit(i)->x();
			units[n++] = mActiveTetrad->unit(i)->y();
		}

		lockTetrad();				// Lock down this tetrad
		if(!overflowCheck())		// Check for board overflow
		{
			cleared = clearLines();	// Clear any full lines
			nextTetrad();			// Fire next tetrad
		}
	}

	return collision;
}

//***************************************************************************************
//
//	Function:	rotateRight
//	Purpose:	Attempts to rotate active tetrad left (counterclockwise)
//	Return:		True if rotation cannot be completed
//
//***************************************************************************************
int cTrisBoard::rotateRight()
{
	int collision(0);

	if(mActiveTetrad)
	{
		cTetrad* tempTetrad = new cTetrad(*mActiveTetrad);	// Copy current state

		mActiveTetrad->rotateRight();						// View rotated tetrad

		for(int i(0); i < 4 && !collision; i++)				// Check for collision
			collision = check(mActiveTetrad->unit(i)->x(), mActiveTetrad->unit(i)->y());

		if(collision)										// If collision occurs
		{
			delete mActiveTetrad;							// Delete rotated tetrad
			mActiveTetrad = tempTetrad;						// Revert state
		}
		else
			delete tempTetrad;								// Else delete old state
	}
	else
		collision = -1;

	return collision;
}

//***************************************************************************************
//
//	Function:	rotateLeft
//	Purpose:	Attempts to rotate active tetrad left (counterclockwise)
//	Return:		True if rotation cannot be completed
//
//***************************************************************************************
int cTrisBoard::rotateLeft()
{
	int collision(0);

	if(mActiveTetrad)
	{
		cTetrad* tempTetrad = new cTetrad(*mActiveTetrad);

		mActiveTetrad->rotateLeft();

		for(int i(0); i < 4 && !collision; i++)
			collision = check(mActiveTetrad->unit(i)->x(), mActiveTetrad->unit(i)->y());

		if(collision)
		{
			delete mActiveTetrad;
			mActiveTetrad = tempTetrad;
		}
		else
			delete tempTetrad;
	}
	else
		collision = -1;

	return collision;
}

//***************************************************************************************
//
//	Function:	check
//	Purpose:	Checks if given coordinate is occupied or off board
//	Return:		True if location is unavailable
//
//***************************************************************************************
bool cTrisBoard::check(int x, int y)
{
	bool collision(false);

	if(x < 0 || x >= mxSize || y < 0 || y >= mySize)
		collision = true;
	else if(mBoard[x][y])
		collision = true;

	return collision;
}

//***************************************************************************************
//
//	Function:	lockTetrad
//	Purpose:	Locks current tetrad in place;
//				places units from tetrad into playing grid
//
//***************************************************************************************
void cTrisBoard::lockTetrad()
{
	for(int i(0); i < 4; i++)	// Place each unit into playing board
	{
		mBoard[mActiveTetrad->unit(i)->x()][mActiveTetrad->unit(i)->y()] =
			mActiveTetrad->unit(i);
	}

	mActiveTetrad->freeUnits();	// Free units from tetrad (now in grid)
	delete mActiveTetrad;		// Delete tetrad instance
	mActiveTetrad = NULL;		// Nullify pointer
}

//***************************************************************************************
//
//	Function:	overflowCheck
//	Purpose:	Checks if board has overflowed
//	Return:		True if board has overflowed
//
//***************************************************************************************
bool cTrisBoard::overflowCheck()
{
	bool overflow(false);

	for(int y(mySize - 2); y < mySize && !overflow; y++)	// For top two rows
		for(int x(0); x < mxSize && !overflow; x++)			// For each unit in row
			if(mBoard[x][y])								// If occupied
				overflow = true;							// Flag overflow

	return overflow;
}

//***************************************************************************************
//
//	Function:	clearLines
//	Purpose:	Clears any full lines from playing board, shifts above units down
//	Return:		Number of lines cleared
//
//***************************************************************************************
int cTrisBoard::clearLines()
{
	int lines(0);

	for(int y(0); y < mySize; y++)				// For each line
	{
		if(checkLine(y))						// If line is full
		{
			clearLine(y);						// Clear line
			lines++;							// Increment counter
		}
		else
		{
			if(lines > 0)						// If cleared line(s) below
				shiftDown(y, lines);			// Shift units downward
		}
	}

	if(lines > 0)
	{
		mClears[lines - 1]++;					// Increment appropriate line counter

		lineScore(lines);						// Increase player's score
		levelCheck(lines);						// Process against rest
	}

	return lines;
}

//***************************************************************************************
//
//	Function:	levelCheck
//	Purpose:	Processes lines against remaining line counter; handles levelup
//
//***************************************************************************************
void cTrisBoard::levelCheck(int lines)
{
	mRemaining -= lines;

	if(mRemaining < 1)
	{
		mRemaining += 10;
		mLevel++;
	}
}

//***************************************************************************************
//
//	Function:	checkLine
//	Purpose:	Checks if given line is full
//	Return:		True if line is full
//
//***************************************************************************************
bool cTrisBoard::checkLine(int y)
{
	bool full(true);

	for(int i(0); i < mxSize; i++)
	{
		if(!mBoard[i][y])
			full = false;
	}

	return full;
}

//***************************************************************************************
//
//	Function:	clearLine
//	Purpose:	Clears units from given line
//
//***************************************************************************************
void cTrisBoard::clearLine(int y)
{
	for(int i(0); i < mxSize; i++)		// for each location in row
		if(mBoard[i][y])				// If occupied
		{
			delete mBoard[i][y];		// Delete unit
			mBoard[i][y] = NULL;		// Nullify pointer
		}
}

//***************************************************************************************
//
//	Function:	shiftDown
//	Purpose:	Shifts units in given line down by given number of lines
//
//***************************************************************************************
void cTrisBoard::shiftDown(int y, int lines)
{
	if(y < mySize && y >= 0 && (y - lines) >= 0)	// If target lines are valid
	{
		for(int i(0); i < mxSize; i++)				// For each unit in line
		{
			if(mBoard[i][y])
			{
				mBoard[i][y]->drop(lines);				// Change unit's 
				mBoard[i][y - lines] = mBoard[i][y];	// Move unit down
				mBoard[i][y] = NULL;					// Nullify pointer
			}
		}
	}
}

//***************************************************************************************
//
//	Function:	lineScore
//	Purpose:	Increases player's score based on clear type and current level
//				Blue Tetris utilizes the original Tetris scoring functions:
//				http://www.tetrisconcept.com/wiki/index.php/Scoring
//
//				Formulae for score, where n = current game level
//				1 line:    40 * (n + 1)
//				2 lines:  100 * (n + 1)
//				3 lines:  300 * (n + 1)
//				4 lines: 1200 * (n + 1)
//
//***************************************************************************************
void cTrisBoard::lineScore(const int lines)
{
	int base(0);				// Base score (function of clear type)

	if(lines == 1)
		base = 40;
	else if(lines == 2)
		base = 100;
	else if(lines == 3)
		base = 300;
	else if(lines == 4)
		base = 1200;

	mScore += base * (mLevel + 1);
}

//***************************************************************************************
//
//	Function:	dropScore
//	Purpose:	Increases player's score per unit of soft drop
//
//***************************************************************************************
void cTrisBoard::dropScore()
{
	mScore++;
}

//	Display Functions ------------------------------------------------------------------O
//
//	cTrisBoard handles drawing itsself and all its inhabitants
//	Drawing functions utilize GLUT functions; render in OpenGL
//

//***************************************************************************************
//
//	Function:	display
//	Purpose:	Displays tetris board
//
//***************************************************************************************
void cTrisBoard::display()
{
	if(mFrame)					// Optional Frame Display
		displayFrame();

	if(mGrid)					// Optional Grid Display
		displayGrid();

	displayBar();				// Display overflow line

	displayUnits();				// Display board inhabitants
	displayActiveTetrad();

	if(mNextDisplay)
		displayNextTetrad();
}

//***************************************************************************************
//
//	Function:	displayBar
//	Purpose:	draws line below top two rows, signifying overflow limit
//
//***************************************************************************************
void cTrisBoard::displayBar()
{
	glColor3f(BAR_R, BAR_G, BAR_B);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + (mySize-2) * mUnitSize, mzOrigin);
	glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + (mySize-2) * mUnitSize,
		mzOrigin + mxSize * mUnitSize);

	glEnd();
}

//***************************************************************************************
//
//	Function:	displayFrame
//	Purpose:	draws frame about playing board
//
//***************************************************************************************
void cTrisBoard::displayFrame()
{
	glColor3f(FRAME_R, FRAME_G, FRAME_B);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glNormal3f(0, 0, 1);	// Left wall facing inward
	glVertex3f(mxOrigin + mUnitSize, myOrigin, mzOrigin);
	glVertex3f(mxOrigin + mUnitSize, myOrigin + mySize * mUnitSize, mzOrigin);
	glVertex3f(mxOrigin, myOrigin + mySize * mUnitSize, mzOrigin);
	glVertex3f(mxOrigin, myOrigin, mzOrigin);

	glNormal3f(0, 0, -1);	// Right wall facing inward
	glVertex3f(mxOrigin + mUnitSize, myOrigin, mzOrigin + mxSize * mUnitSize);
	glVertex3f(mxOrigin, myOrigin, mzOrigin + mxSize * mUnitSize);
	glVertex3f(mxOrigin, myOrigin + mySize * mUnitSize, mzOrigin + mxSize * mUnitSize);
	glVertex3f(mxOrigin + mUnitSize, myOrigin + mySize * mUnitSize, mzOrigin + 
		mxSize * mUnitSize);

	glNormal3f(0, 1, 0);	// Floor facing upward
	glVertex3f(mxOrigin, myOrigin, mzOrigin);
	glVertex3f(mxOrigin, myOrigin, mzOrigin + mxSize * mUnitSize);
	glVertex3f(mxOrigin + mUnitSize, myOrigin, mzOrigin + mxSize * mUnitSize);
	glVertex3f(mxOrigin + mUnitSize, myOrigin, mzOrigin);

	glNormal3f(0, -1, 0);	// Ceiling facing downward
	glVertex3f(mxOrigin, myOrigin + mySize * mUnitSize, mzOrigin);
	glVertex3f(mxOrigin + mUnitSize, myOrigin + mySize * mUnitSize, mzOrigin);
	glVertex3f(mxOrigin + mUnitSize, myOrigin + mySize * mUnitSize, mzOrigin + mUnitSize * mxSize);
	glVertex3f(mxOrigin, myOrigin + mySize * mUnitSize, mzOrigin + mUnitSize * mxSize);

	glEnd();
}

//***************************************************************************************
//
//	Function:	displayGrid
//	Purpose:	Displays grid defining each unit location within playing grid
//
//***************************************************************************************
void cTrisBoard::displayGrid()
{
	int i;				// Index variable used in drawing loops

	glColor3f(GRID_R, GRID_G, GRID_B);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	for(i = 1; i < mxSize; i++)	// Draw vertical lines
	{
		glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin, mzOrigin + mUnitSize * i);
		glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + mySize * mUnitSize,
			mzOrigin + mUnitSize * i);
	}

	for(i = 1; i < mySize - 2; i++)	// Draw horizontal lines up to flood zone
	{
		glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + mUnitSize * i, mzOrigin);
		glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + mUnitSize * i,
			mzOrigin + mxSize * mUnitSize);
	}

	// Draw horizontal line above flood line
	i++;
	glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + mUnitSize * i, mzOrigin);	
	glVertex3f(mxOrigin + (mUnitSize / 2), myOrigin + mUnitSize * i,
		mzOrigin + mxSize * mUnitSize);

	glEnd();
}

//***************************************************************************************
//
//	Function:	displayActiveTetrad
//	Purpose:	Displays active tetrad if pointer is not null
//
//***************************************************************************************
void cTrisBoard::displayActiveTetrad()
{
	if(mActiveTetrad)
	{
		for(int i(0); i < 4; i++)
			displayUnit(mActiveTetrad->unit(i));
	}
}

//***************************************************************************************
//
//	Function:	displayNextTetrad
//	Purpose:	Displays next tetrad if pointer is not null
//
//***************************************************************************************
void cTrisBoard::displayNextTetrad()
{
	if(mNextTetrad)
	{
		int x = mNextTetrad->unit(0)->x();
		int y = mNextTetrad->unit(0)->y();

		for(int i(0); i < 4; i++)
		{
			displayUnitAbsolute(mNextTetrad->unit(i), mNextx, 
				mNexty + (mNextTetrad->unit(i)->y() - y) * mNextSize,
				mNextz + (mNextTetrad->unit(i)->x() - x) * mNextSize,
				mNextSize);
		}
	}
}

//***************************************************************************************
//
//	Function:	displayUnits
//	Purpose:	Displays all units present within tetris grid
//
//***************************************************************************************
void cTrisBoard::displayUnits()
{
	// Display each Tetris Unit inhabiting the board
	for(int x(0); x < mxSize; x++)
	{
		for(int y(0); y < mySize; y++)
		{
			if(mBoard[x][y])
				displayUnit(mBoard[x][y]);
		}
	}
}

//***************************************************************************************
//
//	Function:	displayUnit
//	Purpose:	Displays a Tetris Unit.
//
//***************************************************************************************
void cTrisBoard::displayUnit(cTrisUnit* unit)
{
	int x = mxOrigin;
	int z = mzOrigin + unit->x() * mUnitSize;
	int y = myOrigin + unit->y() * mUnitSize;

	displayUnitAbsolute(unit, x, y, z, mUnitSize);
}

//***************************************************************************************
//
//	Function:	displayUnitAbsolute
//	Purpose:	Displays a tetris unit with given origin
//
//***************************************************************************************
void cTrisBoard::displayUnitAbsolute(cTrisUnit* unit, float x, float y, float z, float unitSize)
{
	if(mTexture.bind(unit->face() + mScheme * 7))
		glColor3f(RED[mScheme][unit->face()], GREEN[mScheme][unit->face()], BLUE[mScheme][unit->face()]);
	else
		glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glNormal3f(0, 0, -1);			// Left side
	glTexCoord2f(1, 1); glVertex3f(x, y + unitSize, z);
	glTexCoord2f(0, 1); glVertex3f(x + unitSize, y + unitSize, z);
	glTexCoord2f(0, 0); glVertex3f(x + unitSize, y, z);
	glTexCoord2f(1, 0); glVertex3f(x, y, z);

	glNormal3f(1, 0, 0);			// Back
	glTexCoord2f(1, 1); glVertex3f(x + unitSize, y + unitSize, z);
	glTexCoord2f(0, 1); glVertex3f(x + unitSize, y + unitSize, z + unitSize);
	glTexCoord2f(0, 0); glVertex3f(x + unitSize, y, z + unitSize);
	glTexCoord2f(1, 0); glVertex3f(x + unitSize, y, z);

	glNormal3f(0, 0, 1);			// Right side
	glTexCoord2f(0, 0); glVertex3f(x, y, z + unitSize);
	glTexCoord2f(1, 0); glVertex3f(x + unitSize, y, z + unitSize);
	glTexCoord2f(1, 1); glVertex3f(x + unitSize, y + unitSize, z + unitSize);
	glTexCoord2f(0, 1); glVertex3f(x, y + unitSize, z + unitSize);

	glNormal3f(-1, 0, 0);			// Front
	glTexCoord2f(0, 0); glVertex3f(x, y, z);
	glTexCoord2f(1, 0); glVertex3f(x, y, z + unitSize);
	glTexCoord2f(1, 1); glVertex3f(x, y + unitSize, z + unitSize);
	glTexCoord2f(0, 1); glVertex3f(x, y + unitSize, z);

	glNormal3f(0, 1, 0);			// Top
	glTexCoord2f(0, 0); glVertex3f(x, y + unitSize, z);
	glTexCoord2f(1, 0); glVertex3f(x, y + unitSize, z + unitSize);
	glTexCoord2f(1, 1); glVertex3f(x + unitSize, y + unitSize, z + unitSize);
	glTexCoord2f(0, 1); glVertex3f(x + unitSize, y + unitSize, z);

	glNormal3f(0, -1, 0);			// Bottom
	glTexCoord2f(0, 1); glVertex3f(x + unitSize, y, z);
	glTexCoord2f(0, 0); glVertex3f(x + unitSize, y, z + unitSize);
	glTexCoord2f(1, 0); glVertex3f(x, y, z + unitSize);
	glTexCoord2f(1, 1); glVertex3f(x, y, z);

	mTexture.unbind();

	glEnd();
}

//***************************************************************************************
//
//	Function:	checkWidth
//	Purpose:	Checks if width is within valid range
//				Any invalid values are rounded to acceptable values
//
//	Returns:	True if value was out of range
//
//***************************************************************************************
bool cTrisBoard::checkWidth()
{
	bool invalid(false);

	if(mxSize < 4)
	{
		mxSize = 4;
		invalid = true;
	}
	else if(mxSize > 20)
	{
		mxSize = 20;
		invalid = true;
	}

	return invalid;
}

//***************************************************************************************
//
//	Function:	checkHeight
//	Purpose:	Checks if height is within valid range
//				Any invalid values are rounded to acceptable values
//
//	Returns:	True if value was out of range
//
//***************************************************************************************
bool cTrisBoard::checkHeight()
{
	bool invalid(false);

	if(mySize < 6)
	{
		mySize = 6;
		invalid = true;
	}
	else if(mySize > 30)
	{
		mySize = 30;
		invalid = true;
	}

	return invalid;
}

//***************************************************************************************
//
//	Function:		setWidth
//	Purpose:		Sets board's width
//	Return:			True if size is out of bounds
//
//***************************************************************************************
bool cTrisBoard::setWidth(int width)
{
	bool valid;

	mxSize = width;
	valid = checkWidth();
	updateMetrics();
	initBoard();

	return valid;
}

//***************************************************************************************
//
//	Function:		setHeight
//	Purpose:		Sets board's height
//	Return:			True if size is out of bounds
//
//***************************************************************************************
bool cTrisBoard::setHeight(int height)
{
	bool valid;

	mySize = height;
	valid = checkHeight();
	updateMetrics();
	initBoard();

	return valid;
}

//***************************************************************************************
//
//	Function:		updateMetrics
//	Purpose:		Automatically adjusts board size and location
//					to maintain aspect and centering
//
//***************************************************************************************
void cTrisBoard::updateMetrics()
{
	int x;
	int y;

	x = 60 / mxSize;
	y = 110 / mySize;

	if(x <= y)				// Set unit size to the smaller of the two
		mUnitSize = x;
	else
		mUnitSize = y;

	myOrigin = -(mUnitSize * mySize) / 2;
	mzOrigin = -(mUnitSize * mxSize) / 2;
}