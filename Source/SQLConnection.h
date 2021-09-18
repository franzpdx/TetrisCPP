//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	March 10, 2007
//	Last Modified:	March 14, 2007
//	File:			SQLConnection.h
//	Project:		Blue Tetris
//
//	Purpose:		Definition for class that handles an SQL connection and contains
//					function calls for sending and retrieving data from the Blue Tetris
//					high score database.
//
//***************************************************************************************

#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
using std::string;

#define DATABASE_NAME "bluetetris"
#define USER_NAME "btserver"
#define PASSWORD "bts"

//***************************************************************************************
//
//	Class:			cSQLConnection
//	Purpose:		Propogates a connection between the Blue Tetris Server and the
//					Blue Tetris Database.
//
//***************************************************************************************
class cSQLConnection
{
public:
	cSQLConnection(): mConnected(false) {}				// Constructor
	~cSQLConnection() { disconnect(); }					// Destructor

	bool connect();			// Initiates connection
	void disconnect();		// Closes connection

	bool connected() { return mConnected; }

	bool submitScore(const char* name, long double score);		// Submits score to database
	bool retrieveTable(string names[], long double scores[]);	// Retrieves lists from db

private:

	SQLHANDLE henv;
	SQLHANDLE hdbc;
	SQLHANDLE hstmt;

	bool mConnected;
};

//***************************************************************************************
//
//	Function:	retrieveTable
//	Purpose:	Retrieves high score table
//
//***************************************************************************************
bool cSQLConnection::retrieveTable(string names[], long double scores[])
{
	char name[NAME_LENGTH + 1];
	unsigned long score;
	char query[512];
	bool inserted;
	bool terminated;
	bool error(false);
	int i, n;
	int errorCounter(0);
	SDWORD cbQual;

	if(!mConnected)
		connect();

	if(mConnected)
	{
		for(n = 0; n < 10; n++)			// Clear values in arrays
		{
			names[n] = "No Entry";
			scores[n] = 0;
		}

		for(n = 0; n < 10; n++)			// Draw 10 values from database
		{
			SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);						 // Allocate handle
			sprintf(query, "select player, score from scores where id = %i", n); // Form query
			SQLExecDirect(hstmt, (unsigned char *)query, SQL_NTS);				 // Execute query
			SQLBindCol(hstmt, 1, SQL_CHAR, name, NAME_LENGTH + 1, &cbQual);					 // Bind Data
			SQLBindCol(hstmt, 2, SQL_INTEGER, &score, SCORE_LENGTH, &cbQual);

			if(SQLFetch(hstmt) != SQL_ERROR)									 // Retrieve Data
			{
				inserted = false;
				terminated = false;
				for(i = NAME_LENGTH; i > 0 && !terminated; i--)
				{
					if(name[i-1] != ' ')
					{
						name[i] = '\0';
						terminated = true;
					}
				}

				for(i = 9; i > 0 && !inserted; i--)
				{
					if(score <= scores[i - 1])		// If position found
					{
						scores[i] = score;			// Insert score
						names[i] = name;
						inserted = true;
					}
					else							// Else
					{
						scores[i] = scores[i - 1];	// Shift next score down
						names[i] = names[i - 1];
					}
				}

				if(!inserted)						// If loop is exited, score is first
				{
					scores[0] = score;
					names[0] = name;
				}
			}
			else
				errorCounter++;

			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		}
	}
	else
		error = true;

	if(errorCounter == 10)			// If no entries found successfully
		error = true;				// Query failed =(

	return error;
}

//***************************************************************************************
//
//	Function:	submitScore
//	Purpose:	Submits name and score pair to the database
//
//***************************************************************************************
bool cSQLConnection::submitScore(const char* newName, long double newScore)
{
	static bool mutex(false);

	bool error(false);
	int n(0);
	char name[NAME_LENGTH + 1];
	unsigned long score;
	int index;
	char query[255];
	SDWORD cbQual;
	unsigned int timeout(0);

	int ids[10];
	string names[10];
	unsigned long scores[10];

	if(!mConnected)			// If not connected
		connect();			// Attempt to connect

	while(mutex);			// Spinlock

	if(mConnected)			// If connected, submit query
	{
		mutex = true;

		for(int i(0); i < 10; i++)	// Retrieve current contents
		{
			SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);						 // Allocate handle
			sprintf(query, "select player, score from scores where id = %i", i); // Form Query
			SQLExecDirect(hstmt, (unsigned char *)query, SQL_NTS);				 // Execute Query
			SQLBindCol(hstmt, 1, SQL_CHAR, name, 10, &cbQual);					 // Bind Data
			SQLBindCol(hstmt, 2, SQL_INTEGER, &score, 10, &cbQual);

			if(SQLFetch(hstmt) != SQL_ERROR)
			{
				ids[n] = i;
				names[n] = name;
				scores[n] = score;
				n++;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		}

		index = 0;

		for(int i(1); i < n; i++)		// Find lowest value
		{
			if(scores[i] < scores[index])
				index = i;
		}

		if(newScore > scores[index] || n == 0)
		{
			if(n > 0)					// Remove old score entry
			{
				SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);						 // Allocate handle
				sprintf(query, "delete from scores where id = %i", ids[index]);
				SQLExecDirect(hstmt, (unsigned char *)query, SQL_NTS);
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			}
			else
				ids[index] = 0;
										// Insert new score entry with same index
			SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);						 // Allocate handle
			sprintf(query, "insert into scores values ( %i, '%s', %.0Lf );", ids[index], newName, newScore);
			SQLExecDirect(hstmt, (unsigned char *)query, SQL_NTS);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		}

		mutex = false;
	}
	else
		error = true;

	return error;
}

//***************************************************************************************
//
//	Function:	connect
//	Purpose:	Initiates connection with database
//
//***************************************************************************************
bool cSQLConnection::connect()
{
	if(!mConnected)
	{
		SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
		SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		SQLConnect(hdbc, (unsigned char *)DATABASE_NAME, SQL_NTS,	// Database name
						 (unsigned char *)USER_NAME, SQL_NTS,		// User Name
						 (unsigned char *)PASSWORD, SQL_NTS);		// Password

		mConnected = true;			// Set flag for active connection
	}

	return mConnected;
}	

//***************************************************************************************
//
//	Function:	disconnect
//	Purpose:	Closes connection with database
//
//***************************************************************************************
void cSQLConnection::disconnect()
{
	if(mConnected)
	{
		if(hdbc != NULL)
			SQLDisconnect(hdbc);							// Disconnect from database

		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);			// More handle freeing
		SQLFreeHandle(SQL_NULL_HANDLE, henv);

		mConnected = false;			// Null connection flag
	}
}

