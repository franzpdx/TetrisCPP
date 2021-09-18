//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	March 10, 2007
//	Last Modified:	March 12, 2007
//	File:			socketConnection.h
//	Project:		Blue Tetris
//
//	Purpose:		Class definition for object which handles a connection with the
//					Blue Tetris server.
//
//***************************************************************************************

#pragma once

#include "afx.h"
#include "resource.h"

#include <queue>
#include <string>
using std::queue;
using std::string;

// Message pump function declaration
UINT BTCRead(LPVOID pParam);
UINT BTCSend(LPVOID pParam);

queue<string> mOutgoing;			// Outgoing message queue
queue<string> mIncoming;			// Incoming message queue
bool readThreadError(false);
bool sendThreadError(false);

//***************************************************************************************
//
//	Class:		cConnection
//	Purpose:	Handles a connection with the Blue Tetris server
//
//***************************************************************************************
class cConnection
{
public:
	cConnection(const char* address);			// Constructor
	~cConnection() { disconnect(); }			// Destructor

	bool initConnection(const char* address);
	void disconnect();							// Disconnects if connected

	bool connected();							// Returns true if connected

	void enqueue(string msg);					// Places message in send queue
	void sendMessage(int state, int message);	// Forms and enqueues message
	void sendMessage(int state, int message, int value);	// Sends message with single-char value
	bool dequeue(string &msg);					// Grabs next item from incoming queue

private:

	string encode(const int state, const int message);

	bool mConnected;							// Flags connection status
	SOCKET mConn;								// Socket for server communication

};

//***************************************************************************************
//
//	Function:	constructor
//	Purpose:	Initializes member data and attempts to initialize connection
//
//***************************************************************************************
cConnection::cConnection(const char* address): mConnected(false)
{
	initConnection(address);
}

//***************************************************************************************
//
//	Function:	connected
//	Purpose:	Returns state of connection
//	Return:		True if connected
//
//***************************************************************************************
bool cConnection::connected()
{
	if(mConnected)
	{
		if(readThreadError || sendThreadError)		// Upon thread error
		{
			mConnected = false;						// Flag disconnection
			disconnect();							// Clean up threads
		}
	}

	return mConnected;							// Return connection state
}

//**************************************************************************************
//
//	Function:	disconnect
//	Purpose:	Closes connection with server
//
//**************************************************************************************
void cConnection::disconnect()
{
	if(mConnected)
	{
		readThreadError = true;
		sendThreadError = true;
	}

	mConnected = false;
}

//**************************************************************************************
//
//	Function:		initConnection
//	Purpose:		Attempts to connect to server;
//					If successful, launches read and send threads
//	Return:			True if error occurs
//
//**************************************************************************************
bool cConnection::initConnection(const char* address)
{
	bool error(false);								// Error flag

	mIncoming.empty();								// Initialize queues
	mOutgoing.empty();

	readThreadError = false;						// Initialize error flags
	sendThreadError = false;

	hostent *hp;
	sockaddr_in server;
	unsigned int addr;
	int r;											// Message return value
	char buff[MESSAGE_BUFFSIZE];					// Message buffer
	WSADATA wsaData;

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
			if(inet_addr(address)==INADDR_NONE)			// Locate server
			{
				hp=gethostbyname(address);
			}
			else
			{
				addr=inet_addr(address);
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

					if(r==SOCKET_ERROR)
					{
						mConnected = false;
					}
					else									// Connection Success
					{
						buff[r] = '\0';						// Add null terminator
						mIncoming.push(string(buff));		// Push message on queue

						mConnected = true;

						AfxBeginThread(BTCRead, (LPVOID)mConn);	// Start reading thread
						AfxBeginThread(BTCSend, (LPVOID)mConn);	// Start sending thread
					}
				}
			}
		}
	}

	return error;
}

//***************************************************************************************
//
//	Function:	enqueue
//	Purpose:	Places message in outgoing queue to be delivered
//
//***************************************************************************************
void cConnection::enqueue(string msg)
{
	mOutgoing.push(msg);
}

//**************************************************************************************
//
//	Function:	sendMessage
//	Purpose:	Encodes command into message and places message on queue
//
//**************************************************************************************
void cConnection::sendMessage(int state, int message)
{
	string newMsg = encode(state, message);
	enqueue(newMsg);
}

//***************************************************************************************
//
//	Function:	sendMessage (3 param)
//	Purpose:	Encodes command into message, appends given value, sends message
//
//***************************************************************************************
void cConnection::sendMessage(int state, int message, int value)
{
	string newMsg = encode(state, message);
	newMsg += char(value);
	enqueue(newMsg);
}

//***************************************************************************************
//
//	Function:		encode
//	Purpose:		Encodes message string
//	Returns:		Encoded message string
//
//**************************************************************************************
string cConnection::encode(const int state, const int message)
{
	string newMsg;
	newMsg += state * 8 + BT_CODE * 32;
	newMsg += message;

	return newMsg;
}

//***************************************************************************************
//
//	Function:		dequeue
//	Purpose:		Grabs next item from the queue
//	Return:			True if nothing on queue; message returned by reference
//
//***************************************************************************************
bool cConnection::dequeue(string &msg)
{
	bool isEmpty(false);

	if(!mIncoming.empty())
	{	
		msg = mIncoming.front();
		mIncoming.pop();
	}
	else
		isEmpty = true;

	return isEmpty;
}

//--------------------------------------------------------------------------------------O
//
//	Global functions
//	The following functions constitute the client-side message pump and are called
//	by the afx thread launcher. For this reason they are declared globally.

//***************************************************************************************
//
//	Function:	BTCRead
//	Purpose:	Thread that reads and stores messages from the server
//
//***************************************************************************************
UINT BTCRead(LPVOID pParam)
{
	readThreadError = false;

	SOCKET socket=(SOCKET)pParam;				// Cast socket from sent parameter
	char buff[MESSAGE_BUFFSIZE];				// Buffer for reading from server
	char message[MESSAGE_BUFFSIZE];				// Second buffer for parsing
	bool sockError(false);						// Error flag
	int r;										// Return value for message pump
	int i, j;									// Index variables
	string temp;

	while(!sockError && !readThreadError)		// Until error occurs
	{
		r=recv(socket,buff,MESSAGE_BUFFSIZE,0);	// Read message from client

		if(r == SOCKET_ERROR)					// Check for error/disconnection
			sockError = true;
		else
		{
			for(i = 0, j = 0; i < r; i++)				// For each character in the buffer
			{
				if((unsigned char)buff[i] == MESSAGE_TERMINATOR)		// If end of message
				{
					message[j] = '\0';					// Null terminate message
					mIncoming.push(string(message));	// Push on message queue
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

	closesocket(socket);						// Close the socket

	readThreadError = true;
	printf("Read thread disconnected\n");		// Debug code

	return 0;
}

//***************************************************************************************
//
//	Function:	BTCSend
//	Purpose:	Thread that sends messages to the server
//
//***************************************************************************************
UINT BTCSend(LPVOID pParam)
{
	readThreadError = false;

	SOCKET socket = (SOCKET)pParam;				// Cast parameter to get the socket
	int r;										// Return value for message pump
	int length;									// Stores buffer length
	char buff[MESSAGE_BUFFSIZE];				// Message buffer
	bool sockError(false);						// Error flag

	while(!sockError && !readThreadError)		// Until error occurs
	{
		if(!mOutgoing.empty())					// If message in queue, send message
		{
			sprintf(buff, mOutgoing.front().c_str());
			mOutgoing.pop();

			length = strlen(buff);				// Append message terminator
			buff[length] = MESSAGE_TERMINATOR;
			buff[length + 1] = '\0';
			length++;

			r=send(socket, buff, strlen(buff), 0);

			if(r==SOCKET_ERROR)					// Check for socket errors
				sockError = true;
		}
	}

	sendThreadError = true;
	printf("Send thread disconnected\n");		// Debug code

	return 0;
}