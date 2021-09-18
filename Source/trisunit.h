//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	November 16, 2006
//	Last Modified:	January 17, 2007
//	File:			trisunit.h
//
//	Purpose:		Models a tetris unit
//
//***************************************************************************************

#pragma once

//***************************************************************************************
//
//	Class:		cTrisUnit
//	Purpose:	Represents a tetris unit
//
//***************************************************************************************
class cTrisUnit
{
public:
	// Default Constructor
	cTrisUnit(): mFace(0), mx(-1), my(-1) {}

	// Value Constructor
	cTrisUnit(int x, int y, int face): mx(x), my(y), mFace(face) {}

	// Default Destructor
	~cTrisUnit() {}

	void moveLeft() { mx--; }		// Movement functions
	void moveRight() { mx++; }
	void moveDown() { my--; }
	void drop(int y) { my -= y; }

	void setCoord(int x, int y) { mx = x; my = y; }	// Setter

	int x() { return mx; }
	int y() { return my; }
	int face() { return mFace; }

protected:

	int mx;					// Relative coordinates in Tetris board
	int my;

	int mFace;				// Unit's appearance
};