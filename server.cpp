//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	January 31, 2007
//	Last Modified:	February 8, 2007
//	File:			server.cpp
//	Project:		Blue Tetris
//
//	Purpose:		Runs the Blue Tetris server
//
//***************************************************************************************

#include <conio.h>
#include "BTServer.h"

void main()
{
	try
	{
		char selection;
		int mode;

		printf("| Blue Tetris Server.\n| Version 2.0\n\n");

		printf("| Press 's' to start in sql server mode.\n| Press any other key to start in local data mode.\n\n");

		selection = getch();

		if(selection == 's' || selection == 'S')
		{
			mode = 1;
			printf("| SQL Database mode selected\n");
		}
		else
		{
			mode = 0;
			printf("| Local data mode selected\n");
		}

		AfxBeginThread(BTSRun,(LPVOID)mode);
		printf("| Now running\n| Press escape to end execution.\n\n");
		while(_getch()!=27);

		mEndExecution = true;
		closesocket(mServer);
		WSACleanup();
	}
	catch(...)
	{
		printf("\n\nUnhandled exception occurred...\n\n");
		WSACleanup();
	}

	printf("Server down.\n");
}