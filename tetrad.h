//***************************************************************************************
//
//	File:			tetrad.h
//	Author:			Tom Franz
//	Date Created:	January 14, 2007
//	Last Modified:	February 11, 2007
//	Project:		Blue Tetris
//
//	Purpose:		Class that represents a Tetrad (otherwise known as a Tetrominoe or
//					falling tetris piece.) Contains four tetris units and is capable of
//					moving and rotating.
//
//***************************************************************************************

#pragma once

#include "trisunit.h"

//***************************************************************************************
//
//	Class:			cTetrad
//	Purpose:		Represents a tetrad within Blue Tetris
//
//***************************************************************************************
class cTetrad
{
public:

	cTetrad();							// Default Constructor
	cTetrad(int type, int boardWidth, int boardHeight);			// Value Constructor
	cTetrad(const cTetrad &tetrad);		// Copy Constructor
	~cTetrad();							// Default Destructor

	bool generate(int type, int boardWidth, int boardHeight);	// Generates a tetrad

	void moveLeft();					// Movement functions
	void moveRight();
	void moveDown();

	void rotateLeft();					// Rotation functions
	void rotateRight();

	cTrisUnit* unit(int i);				// Returns member pointer to unit of given index
	void freeUnits();					// Nullifies member unit pointers
	void setUnits(int* x, int* y);		// Sets locations of each unit within tetrad

	int type() { return mType; }		// Type getter

private:

	// Tetrad creation functions (place units into tetrad)
	void generateI(int boardWidth, int boardHeight);					
	void generateO(int boardWidth, int boardHeight);
	void generateL(int boardWidth, int boardHeight);
	void generateJ(int boardWidth, int boardHeight);
	void generateS(int boardWidth, int boardHeight);
	void generateZ(int boardWidth, int boardHeight);
	void generateT(int boardWidth, int boardHeight);

	// Rotation calculation functions
	void counterclockwise(int &x, int &y);
	void clockwise(int &x, int &y);

	void cTranslateI();
	void ccTranslateI();

	int mType;							// Integer representation of tetrad shape

	cTrisUnit* mUnits[4];				// Array of units contained in tetrad
};

//***************************************************************************************
//
//	Function:		Default Constructor
//	Purpose:		Initializes member data
//
//***************************************************************************************
cTetrad::cTetrad()
{
	for(int i(0); i < 4; i++)
		mUnits[i] = 0;
}
//***************************************************************************************
//
//	Function:		Value Constructor
//	Purpose:		Initializes tetrad of given type
//
//***************************************************************************************
cTetrad::cTetrad(int type, int boardWidth, int boardHeight)
{
	generate(type, boardWidth, boardHeight);
}

//***************************************************************************************
//
//	Function:		Copy Constructor
//	Purpose:		Creates hard copy of given tetrad
//
//***************************************************************************************
cTetrad::cTetrad(const cTetrad &tetrad)
{
	mType = tetrad.mType;

	for(int i(0); i < 4; i++)
		mUnits[i] = new cTrisUnit(tetrad.mUnits[i]->x(), tetrad.mUnits[i]->y(),
		tetrad.mUnits[i]->face());
}

//***************************************************************************************
//
//	Function:		Destructor
//	Purpose:		Frees memory upon tetrad deletion
//
//***************************************************************************************
cTetrad::~cTetrad()
{
	for(int i(0); i < 4; i++)
	{
		if(mUnits[i])
			delete mUnits[i];
	}
}

//***************************************************************************************
//
//	Function:		moveLeft
//	Purpose:		Moves tetrad left one unit
//
//***************************************************************************************
void cTetrad::moveLeft()
{
	for(int i(0); i < 4; i++)
		mUnits[i]->moveLeft();
}

//***************************************************************************************
//
//	Function:		moveRight
//	Purpose:		Moves tetrad right one unit
//
//***************************************************************************************
void cTetrad::moveRight()
{
	for(int i(0); i < 4; i++)
		mUnits[i]->moveRight();
}

//***************************************************************************************
//
//	Function:		moveDown
//	Purpose:		Moves tetrad down one unit
//
//***************************************************************************************
void cTetrad::moveDown()
{
	for(int i(0); i < 4; i++)
		mUnits[i]->moveDown();
}

//	Note: Rotation Behavior ------------------------------------------------------------O
//
//	Except in special cases, Blue Tetris tetrads rotate through the following algorithm:
//
//	The first unit in the mUnits array acts as the center of rotation;
//		Unit[0] remains idle as other units rotate about it.
//	For each unit [1] thorugh [3] the offset of the rotating unit from the center unit
//		is calculated
//	Clockwise and Counterclockwise functions handle recalculation of offset for rotation
//	The unit is then reassigned its new position given Unit[0]'s position and its
//		new offset
//

//***************************************************************************************
//
//	Function:		rotateRight
//	Purpose:		Rotates tetrad right (clockwise) 90 degrees
//
//***************************************************************************************
void cTetrad::rotateRight()
{
	if(mType != 1)				// Don't rotate squares. That's just silly.
	{
		if(mType == 0)				// Special rotation for I
			cTranslateI();

		int xPrime = mUnits[0]->x();
		int yPrime = mUnits[0]->y();
		int x, y;

		for(int i(1); i < 4; i++)	// Rotate about center tetris unit
		{
			x = mUnits[i]->x() - xPrime;
			y = mUnits[i]->y() - yPrime;

			clockwise(x, y);

			mUnits[i]->setCoord(xPrime + x, yPrime + y);
		}
	}
}

//***************************************************************************************
//
//	Function:		rotateLeft
//	Purpose:		Rotates tetrad left (counterclockwise) 90 degrees
//
//***************************************************************************************
void cTetrad::rotateLeft()
{
	if(mType != 1)					// Don't rotate squares.
	{
		if(mType == 0)				// Special rotation for I
			ccTranslateI();

		int xPrime = mUnits[0]->x();	// x coordinate of center unit
		int yPrime = mUnits[0]->y();	// y coordinate of center unit
		int x, y;

		for(int i(1); i < 4; i++)	// Rotate about center tetris unit
		{
			x = mUnits[i]->x() - xPrime;
			y = mUnits[i]->y() - yPrime;

			counterclockwise(x, y);

			mUnits[i]->setCoord(xPrime + x, yPrime + y);
		}
	}
}

//***************************************************************************************
//
//	Function:	clockwise
//	Purpose:	Calculates new offset rotating clockwise about origin
//
//***************************************************************************************
void cTetrad::clockwise(int &x, int &y)
{
	int temp;		// Intermediate variable

	if(x == 0)
	{
		x = y;
		y = 0;
	}
	else if(y == 0)
	{	
		y = -x;
		x = 0;
	}
	else
	{
		temp = x;
		x = y;
		y = -temp;
	}
}

//***************************************************************************************
//
//	Function:	counterclockwise
//	Purpose:	Calculates new offset rotating counterclockwise about origin
//
//***************************************************************************************
void cTetrad::counterclockwise(int &x, int &y)
{
	int temp;		// Intermediate variable

	if(x == 0)
	{
		x = -y;
		y = 0;
	}
	else if(y == 0)
	{	
		y = x;
		x = 0;
	}
	else
	{
		temp = x;
		x = -y;
		y = temp;
	}
}

//***************************************************************************************
//
//	Function:		ccRotateI
//	Purpose:		Performs counterclockwise rotation of I tetrad
//
//***************************************************************************************
void cTetrad::cTranslateI()
{
	int xPrime = mUnits[0]->x();	// x coordinate of center unit
	int yPrime = mUnits[0]->y();	// y coordinate of center unit
	int x, y, i;

	x = mUnits[1]->x() - xPrime;
	y = mUnits[1]->y() - yPrime;

	if(y == 0)
		for(i = 0; i < 4; i++)
			mUnits[i]->setCoord(mUnits[i]->x() - x, mUnits[i]->y());
	else if(x == 0)
		for(i = 0; i < 4; i++)
			mUnits[i]->setCoord(mUnits[i]->x(), mUnits[i]->y() - y);
}

//***************************************************************************************
//
//	Function:		ccRotateI
//	Purpose:		Performs counterclockwise rotation of I tetrad
//
//***************************************************************************************
void cTetrad::ccTranslateI()
{
	int xPrime = mUnits[0]->x();	// x coordinate of center unit
	int yPrime = mUnits[0]->y();	// y coordinate of center unit
	int x, y, i;

	x = mUnits[1]->x() - xPrime;
	y = mUnits[1]->y() - yPrime;

	if(y == 0)
		for(i = 0; i < 4; i++)
			mUnits[i]->setCoord(mUnits[i]->x(), mUnits[i]->y() + x);
	else if(x == 0)
		for(i = 0; i < 4; i++)
			mUnits[i]->setCoord(mUnits[i]->x() - y, mUnits[i]->y());
}

//***************************************************************************************
//
//	Function:	unit
//	Purpose:	Getter for member units
//	Returns:	A pointer to the member unit of given index
//
//***************************************************************************************
cTrisUnit* cTetrad::unit(int i)
{
	if(i < 0)
		i = 0;
	if(i > 3)
		i = 3;

	if(mUnits)
		return mUnits[i];
	else
		return NULL;
}

//***************************************************************************************
//
//	Function:	freeUnits
//	Purpose:	Sets member unit pointers to null, freeing unit data
//
//***************************************************************************************
void cTetrad::freeUnits()
{
	mUnits[0] = NULL;
	mUnits[1] = NULL;
	mUnits[2] = NULL;
	mUnits[3] = NULL;
}

//***************************************************************************************
//
//	Function:	setUnits
//	Purpose:	Sets location of each unit within tetrad
//
//***************************************************************************************
void cTetrad::setUnits(int* x, int* y)
{
	for(int i(0); i < 4; i++)
		mUnits[i]->setCoord(x[i], y[i]);
}

//	Tetrad Generation Functions --------------------------------------------------------O
//
//	Blue Tetris's initial locations for falling tetrads on playing board, and initial
//	tetrad orientations comply to The Tetris Company's Tetris Guideline, January 2007
//	See http://www.tetrisconcept.com/wiki/index.php/Tetromino
//

//***************************************************************************************
//
//	Function:		generate
//	Purpose:		Forms tetrad of shape corresponding to given value;
//					Initializes tetrad's location on board
//	Returns:		True error is encountered in tetrad generation
//
//***************************************************************************************
bool cTetrad::generate(int type, int boardWidth, int boardHeight)
{
	bool error(false);				// Flags error in tetrad creation

	if(type < 0 || type > 6)		// Catch invalid types
		type = 0;

	mType = type;					// Store type

	if     (type == 0)				// Generate tetrad according to type
		generateI(boardWidth, boardHeight);
	else if(type == 1)
		generateO(boardWidth, boardHeight);
	else if(type == 2)
		generateL(boardWidth, boardHeight);
	else if(type == 3)
		generateJ(boardWidth, boardHeight);
	else if(type == 4)
		generateS(boardWidth, boardHeight);
	else if(type == 5)
		generateZ(boardWidth, boardHeight);
	else if(type == 6)
		generateT(boardWidth, boardHeight);

	if(!mUnits[0] || !mUnits[1] || !mUnits[2] || !mUnits[3])
		error = true;

	return error;
}

//***************************************************************************************
//
//	Function:	generateI
//	Purpose:	Generates a I-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateI(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x - 1, y, mType);
	mUnits[2] = new cTrisUnit(x + 1, y, mType);
	mUnits[3] = new cTrisUnit(x + 2, y, mType);
}

//***************************************************************************************
//
//	Function:	generateO
//	Purpose:	Generates an O-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateO(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x + 1, y, mType);
	mUnits[2] = new cTrisUnit(x + 1, y + 1, mType);
	mUnits[3] = new cTrisUnit(x, y + 1, mType);
}

//***************************************************************************************
//
//	Function:	generateL
//	Purpose:	Generates a L-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateL(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x + 1, y, mType);
	mUnits[2] = new cTrisUnit(x + 1, y + 1, mType);
	mUnits[3] = new cTrisUnit(x - 1, y, mType);
}

//***************************************************************************************
//
//	Function:	generateJ
//	Purpose:	Generates an J-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateJ(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x + 1, y, mType);
	mUnits[2] = new cTrisUnit(x - 1, y, mType);
	mUnits[3] = new cTrisUnit(x - 1, y + 1, mType);
}

//***************************************************************************************
//
//	Function:	generateS
//	Purpose:	Generates an S-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateS(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x - 1, y, mType);
	mUnits[2] = new cTrisUnit(x , y + 1, mType);
	mUnits[3] = new cTrisUnit(x + 1, y + 1, mType);
}

//***************************************************************************************
//
//	Function:	generateZ
//	Purpose:	Generates a Z-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateZ(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x + 1, y, mType);
	mUnits[2] = new cTrisUnit(x, y + 1, mType);
	mUnits[3] = new cTrisUnit(x - 1, y + 1, mType);
}

//***************************************************************************************
//
//	Function:	generateT
//	Purpose:	Generates a T-shaped tetrad
//
//***************************************************************************************
void cTetrad::generateT(int boardWidth, int boardHeight)
{
	int x = boardWidth / 2 - 1;					// x coord of tetrad's "center" unit
	int y = boardHeight - 2;					// y coord of same

	mUnits[0] = new cTrisUnit(x, y, mType);
	mUnits[1] = new cTrisUnit(x - 1, y, mType);
	mUnits[2] = new cTrisUnit(x, y + 1, mType);
	mUnits[3] = new cTrisUnit(x + 1, y, mType);
}