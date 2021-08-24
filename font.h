//***************************************************************************************
//
//	Author:			Tom Franz
//					Thanks to tutorial 14 at nehe.gamedev.com
//	Date Created:	February 24, 2007
//	Last Modifed:	February 24, 2007
//	File:			font.h
//	Project:		Blue Tetris
//
//	Purpose:		Library of functions to display 3D text for Blue Tetris.
//					Must be initialized and destructed through function calls.
//
//***************************************************************************************

#pragma once

// Global function prototypes
bool loadFont();						// Creates library
void deleteFont();						// Deletes library
void displayText(float x, float y, float z, float xRot, float yRot, float zRot, bool center, const char *fmt, ...);
float textLength(const char *fmt, ...);	// Returns physical length of text

// Global variables
GLYPHMETRICSFLOAT gmf[256];	// Font storage buffer
GLuint	base;

//***************************************************************************************
//
//	Function:	buildFont
//	Purpose:	Builds Blue Tetris's font bank
//
//***************************************************************************************
bool loadFont()
{
	bool failure(false);
	HFONT font;										// Windows font ID
	HDC hdc = GetDC(NULL);							// Get screen's DC

	base = glGenLists(256);							// Create bank of 256 characters

	font = CreateFont(	-12,						// Height
						0,							// Width
						0,							// Angle of escapement
						0,							// Angle of orientation
						FW_BOLD,					// Font weight
						FALSE,						// Italic flag
						FALSE,						// Underline flag
						FALSE,						// Strikeout flag
						ANSI_CHARSET,				// Character set identifier
						OUT_TT_PRECIS,				// Output preset
						CLIP_DEFAULT_PRECIS,		// Clipping precision
						ANTIALIASED_QUALITY,		// Output quality
						FF_DONTCARE|DEFAULT_PITCH,	// Family, pitch
						"Lucida Calligraphy");		// Font name

	if(!font)
		failure = true;

	if(!failure)
	{
		SelectObject(hdc, font);						// Select the font

		wglUseFontOutlines(	hdc,						// Select current DC
							0,							// Starting character
							255,						// Number of characters
							base,						// Starting list
							0.0f,						// Deviation from true outline
							0.1f,						// Depth
							WGL_FONT_POLYGONS,			// Use polygons
							gmf);						// Buffer address
	}

	return failure;
}

//***************************************************************************************
//
//	Function:	displayText
//	Purpose:	Prints given string in 3D font
//				Accepts coordinate, rotation in each direction, and string arg list
//
//***************************************************************************************
void displayText(float x, float y, float z, float xRot, float yRot, float zRot, bool center, const char *fmt, ...)
{
	bool error(false);								// Flags if text cannot be displayed
	float length(0);								// stores length of string
	char text[256];									// String buffer
	va_list	ap;										// Pointer to arg list

	if (fmt == NULL)								// Check if pointer is null
		error = true;

	if(!error)
	{
		glPushMatrix();
		glTranslatef(x, y, z);						// Perform translation
		glRotatef(yRot - 90,0.0f,1.0f,0.0f);		// Y rotation (normalizes along z)
		if(xRot) 
			glRotatef(xRot,1.0f,0.0f,0.0f);			// X rotation
		if(zRot) 
			glRotatef(zRot,0.0f,0.0f,1.0f);			// Z rotation

		va_start(ap, fmt);							// Parse string for variables
			vsprintf(text, fmt, ap);				// Convert symbols to numbers
		va_end(ap);									// Store results in text

		if(center)
		{
			for (int i(0); i < (strlen(text)); i++)		// Determine total width
				length+=gmf[text[i]].gmfCellIncX;		// Store in length

			glTranslatef(-length/2, 0, 0);			// Text centering
		}

		glPushAttrib(GL_LIST_BIT | GL_POLYGON_BIT);	// Store important gl attributes
		glListBase(base);							// Set base char to 0
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draw the display list
		glPopAttrib();								// Restore gl attributes
		glPopMatrix();
	}
}

//***************************************************************************************
//
//	Function:	textLength
//	Purpose:	Returns length of given text
//
//***************************************************************************************
float textLength(const char *fmt, ...)
{
	float length(0);
	va_list	ap;									// Pointer to arg list
	char text[256];								// String buffer

	if (fmt == NULL)							// Check if pointer is null
		return 0;

	va_start(ap, fmt);							// Parse string for variables
		vsprintf(text, fmt, ap);				// Convert symbols to numbers
	va_end(ap);		

	for (int i(0); i < (strlen(text)); i++)		// Determine total width
		length += gmf[text[i]].gmfCellIncX;		// Store in length

	return length;
}

//***************************************************************************************
//
//	Function:	deleteFont
//	Purpose:	Frees font list from memory
//
//***************************************************************************************
void deleteFont()
{
	glDeleteLists(base, 256);
}