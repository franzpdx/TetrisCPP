//***************************************************************************************
//
//	Author:			Tom Franz
//					Developed from Lesson 6 at nehe.gamedev.net
//	Date Created:	February 19, 2007
//	Last Modified:	February 20, 2007
//	File:			texture.h
//
//	Purpose:		Class definition for texture class, which loads and stores
//					textures.
//
//***************************************************************************************

#pragma once

#include "afx.h"
#include <GL\gl.h>
#include <GL\glaux.h>
#include <vector>
using std::vector;

//***************************************************************************************
//
//	Class:		cTexture
//	Purpose:	Class for OpenGL texture management
//
//***************************************************************************************
class cTexture
{
public:

	cTexture(): mSize(0) {}			// Constructor
	cTexture(const cTexture &texture);		// Copy constructor
	~cTexture() {}

	bool loadTexture(char* filename);	// Loads and stores texture from given file

	bool bind(int id);				// Binds texture of given ID
	void unbind();					// Unbinds texture

private:

	AUX_RGBImageRec *loadBMP(char *filename);	// Loads file

	vector<GLuint> mTexture;		// Stores loaded textures
	vector<bool> mError;			// Flags whether each texture is available

	int mSize;						// Stores number of loaded textures
};

//***************************************************************************************
//
//	Function:	copy constructor
//	Purpose:	Copys data of texturing class
//
//***************************************************************************************
cTexture::cTexture(const cTexture &texture)
{
	mSize = texture.mSize;
	mTexture = texture.mTexture;
	mError = texture.mError;
}

//***************************************************************************************
//
//	Function:	loadBMP
//	Purpose:	Loads the bitmap image
//	Returns:	Pointer to image
//
//***************************************************************************************
AUX_RGBImageRec *cTexture::loadBMP(char *filename)
{
	FILE *File=NULL;							// File handle

	if (!filename)
		return NULL;

	File=fopen(filename,"r");					// Check to see if file exists

	if (File)
	{
		fclose(File);							// Close handle to file
		return auxDIBImageLoad(filename);		// Load bitmap; return pointer
	}

	return NULL;
}

//***************************************************************************************
//
//	Function:	loadTexture
//	Purpose:	Loads the texture from a given file
//	Returns:	True if error occurs
//
//***************************************************************************************
bool cTexture::loadTexture(char *filename)
{
	bool error(false);

	AUX_RGBImageRec *TextureImage[1];				// Create storage struct for texture
	GLuint texture;

	memset(TextureImage,0,sizeof(void *)*1);		// Set pointer to null

	if (TextureImage[0]=loadBMP(filename))			// Attempt to load bitmap
	{
		glGenTextures(1, &texture);					// Create texture

		// Texture generation
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	else
		error = true;

	if (TextureImage[0])							// Free memory
	{
		if (TextureImage[0]->data)
		{
			free(TextureImage[0]->data);
		}

		free(TextureImage[0]);
	}

	mTexture.push_back(texture);
	mError.push_back(error);
	mSize++;

	return error;
}

//***************************************************************************************
//
//	Function:	bind
//	Purpose:	Binds texture of given ID
//	Return:		True if texture is not loaded or ID is invalid
//
//***************************************************************************************
bool cTexture::bind(int id)
{
	bool invalid(false);

	if(id < 0 || id >= mSize)
		invalid = true;
	else if(mError[id])
		invalid = true;
	else
		glBindTexture(GL_TEXTURE_2D, mTexture[id]);

	return invalid;
}

//***************************************************************************************
//
//	Function:	unbind
//	Purpose:	Removes any texture binding
//
//***************************************************************************************
void cTexture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, NULL);
}