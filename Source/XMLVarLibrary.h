//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	May 18, 2007
//	Last Modified:	May 18, 2007
//	File:			XMLVarLibrary.h
//
//	Purpose:		Class definition for XMLLib, which acts as an intermediary for
//					translating between C++ variables and XML representations
//
//***************************************************************************************

#pragma once

#include "XMLVarConversion.h"

#include <vector>
using std::vector;

#include <string.h>
using std::string;

#include <fstream>
using std::ifstream;
using std::ofstream;

//***************************************************************************************
//
//	Class:		cXMLLib
//	Purpose:	Variable library. Reads and writes as C++ or XML
//
//***************************************************************************************
class cXMLLib
{
public:

	cXMLLib() {}
	~cXMLLib() {}

	// Insert: Adds a variable to the library
	void insert(string name, string value)	{ mVariables.push_back(cXMLvc(name, value)); }
	void insert(string name, int value)		{ mVariables.push_back(cXMLvc(name, value)); }
	void insert(string name, bool value)	{ mVariables.push_back(cXMLvc(name, value)); }

	bool write(const char* filename);		// writes contents to given file
	bool read(const char* filename);		// reads contents of given file

	bool find(string name, string& value);	// If name is found, value is populated w/ data
	bool find(string name, int& value);	
	bool find(string name, bool& value);

private:

	bool search(const char* buffer);		// Searches for beginning tag
	bool readVariables(const char* buffer);	// Reads variable data until end tag
	string parseToken(const char* buffer, int& i);	// Parses token from string
	bool labelChar(const char c);			// Returns true if c is label-type

	vector<cXMLvc> mVariables;

};

//***************************************************************************************
//
//	Function:	write
//	Purpose:	Writes contents to given file
//	Returns:	True if error occurs
//
//***************************************************************************************
bool cXMLLib::write(const char* filename)
{
	ofstream fout(filename);
	bool failure(false);
	vector<cXMLvc>::iterator it = mVariables.begin();

	if(!fout.fail())
	{
		fout << "<settings>\n";				// Display beginning tag

		while(it != mVariables.end())		// For each variable
		{
			fout << "\t" << *it;			// Display tag for this variable
			it++;							// Move to next
		}

		fout << "</settings>";				// Display end tag

		fout.close();						// Close file
	}
	else									// If file cannot be opened
		failure = true;						// Report failure

	return failure;
}

//***************************************************************************************
//
//	Function:	read
//	Purpose:	Reads contents of given file
//	Returns:	True if error occurs
//
//***************************************************************************************
bool cXMLLib::read(const char* filename)
{
	ifstream fin(filename);
	bool failure(false);			// Flags file read failure
	bool begin(false);				// Flags beginning of XML
	bool end(false);				// Flags end of XML

	const int bufferlen(512);
	char buffer[bufferlen];

	if(!fin.fail())
	{
		while(!fin.eof() && !end)				// Until end of file
		{
			fin.getline(buffer, bufferlen);		// Read line
			
			if(begin)							// If in XML section
				end = readVariables(buffer);	// Read data
			else
				begin = search(buffer);			// Otherwise search for XML beginning
		}
	}
	else							// If file cannot be opened
		failure = true;				// Report failure

	if(!begin)						// If settings section not found
		failure = true;				// Report error

	return failure;
}

//***************************************************************************************
//
//	Function:	search
//	Purpose:	Seaches buffer for beginning tag
//	Returns:	True if beginning tag found
//
//***************************************************************************************
bool cXMLLib::search(const char* buffer)
{
	bool begin = false;

	if (!string(buffer).find("<settings>"))		// Seach for tag within buffer
		begin = true;								// If found, return true

	return begin;
}

//***************************************************************************************
//
//	Function:	readVariables
//	Purpose:	Reads variable data from file until end tag is found
//	Returns:	True if end tag found
//
//***************************************************************************************
bool cXMLLib::readVariables(const char *buffer)
{
	bool end = false;

	if (!string(buffer).find("</settings>"))		// Search for tag within buffer
		end = true;									// If found, return true
	else
	{
		int i(0);
		string name;
		string value;
		string temp;

		while(buffer[i] != '\0' && buffer[i] != '<')	// <
			i++;

		name = parseToken(buffer, i);					// (name)

		while(buffer[i] != '\0' && temp != "value")		// value
			temp = parseToken(buffer, i);

		while(buffer[i] != '\0' && buffer[i] != '=')	// =
			i++;
		
		while(buffer[i] != '\0' && buffer[i] != '\"')	// "
			i++;

		i++;

		while(buffer[i] != '\0' && buffer[i] != '\"')	// "
		{ 
			value += buffer[i];
			i++;
		}

		while(buffer[i] != '\0' && buffer[i] != '>')	// >
			i++;

		if(buffer[i] != '\0')							// If all components found before end tag
			mVariables.push_back(cXMLvc(name, value));	// Add value to library
	}

	return end;
}

//***************************************************************************************
//
//	Function:	find
//	Purpose:	Searches library for given variable name.
//				If found, value is populated with data from library.
//	Returns:	True if value found
//
//***************************************************************************************
bool cXMLLib::find(string name, string& value)
{
	vector<cXMLvc>::iterator it = mVariables.begin();
	bool found(false);

	while(!found && it != mVariables.end())
	{
		if(it->name() == name)
		{
			value = it->getString();
			found = true;
		}
		it++;
	}

	return found;
}

bool cXMLLib::find(string name, int& value)
{
	vector<cXMLvc>::iterator it = mVariables.begin();
	bool found(false);

	while(!found && it != mVariables.end())
	{
		if(it->name() == name)
		{
			value = it->getInt();
			found = true;
		}
		it++;
	}

	return found;
}

bool cXMLLib::find(string name, bool& value)
{
	vector<cXMLvc>::iterator it = mVariables.begin();
	bool found(false);

	while(!found && it != mVariables.end())
	{
		if(it->name() == name)
		{
			value = it->getBool();
			found = true;
		}
		it++;
	}

	return found;
}

//***************************************************************************************
//
//	Function:	parseToken
//	Purpose:	Parses a token from the buffer
//	Returns:	String containing token
//
//***************************************************************************************
string cXMLLib::parseToken(const char *buffer, int& i)
{
	string token;
	while(!labelChar(buffer[i]) && buffer[i] != '\0')
		i++;

	while(labelChar(buffer[i]) && buffer[i] != '\0')
	{
		token += buffer[i];
		i++;
	}

	return token;
}

//***************************************************************************************
//
//	Function:	labelChar
//	Purpose:	Checks whether character is appropriate for an XML label
//	Returns:	True if character is label-type
//
//***************************************************************************************
bool cXMLLib::labelChar(const char c)
{
	bool isLabel(false);

	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
		c == '-' || c == '_')
		isLabel = true;

	return isLabel;
}