//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	May 16, 2007
//	Last Modified:	May 16, 2007
//	File:			XMLVarConversion.h
//
//	Purpose:		Class defintion for cXMLvc which converts between variables and
//					XML expressions of variables.
//					Overloaded for:
//					Int
//					Bool
//					String
//
//***************************************************************************************

#pragma once

#include <fstream>
using std::ifstream;

#include <iostream>
using std::ostream;

//***************************************************************************************
//
//	Class:		cXMLvc
//	Purpose:	Universal Modelling Language Variable Converter
//				Contains information expressing a variable's name and data.
//				Can print to and read from file streams
//
//***************************************************************************************
class cXMLvc
{
public:

	cXMLvc() {}		// Default constructor
	cXMLvc(string name, string data): mName(name), mData(data) {}			// Value Ctors
	cXMLvc(string name, int data): mName(name), mData(toString(data)) {}
	cXMLvc(string name, bool data): mName(name), mData(toString(data)) {}

	~cXMLvc() {}						// Default destructor

	// Typecasting getters
	bool getBool() { return toBool(mData); }
	int getInt() { return toInt(mData); }
	string getString() { return mData; }

	//bool read(ifstream* fin);				// Reader

	void setName(string name) { mName = name; }
	string name() { return mName; }

	bool toBool(string value);			// Type conversions
	int toInt(string value);
	string toString(int value);
	string toString(bool value);

	friend ostream& operator<<(ostream& outstream, cXMLvc& variable);

private:

	// Member variables
	string mName;						// Variable name
	string mData;						// Variable data

};

//***************************************************************************************
//
//	Function:	toBool
//	Purpose:	Returns value as bool
//	Returns:	(bool)data
//
//***************************************************************************************
bool cXMLvc::toBool(string data)
{
	bool value(false);

	if(data.length() != 0 && data[0] == '1')
		value = true;
	
	return value;
}

//***************************************************************************************
//
//	Function:	toInt
//	Purpose:	Returns value as integer
//	Returns:	(int)data
//
//***************************************************************************************
int cXMLvc::toInt(string data)
{
	int i(0);
	int value(0);
	while(i < data.length() && data[i] >= '0' && data[i] <= '9')
	{
		value = value * 10;
		value += data[i] - '0';
		i++;
	}

	return value;
}

//***************************************************************************************
//
//	Function:	toString
//	Purpose:	Converts int & bool to string
//
//***************************************************************************************
string cXMLvc::toString(int value)
{
	string data;

	if(value == 0)
		data = "0";
	else
	{
		while(value)
		{
			data.insert(data.begin(), (value % 10) + 48);
			value /= 10;
		}
	}

	return data;
}

string cXMLvc::toString(bool value)
{
	if(value)
		return "1";
	else
		return "0";
}

//***************************************************************************************
//
//	Operator:	Shift Left
//	Purpose:	Allows streamed output of XML
//
//***************************************************************************************
ostream& operator<<(ostream& outstream, cXMLvc& variable)
{
	outstream << "<"
			<< variable.name()
			<< " value=\""
			<< variable.getString()
			<< "\" />\n";

	return outstream;
}
