//***************************************************************************************
//
//	Author:			Tom Franz
//	Date Created:	April 3, 2007
//	Last Modified:	April 3, 2007
//	File:			sound.h
//
//	Purpose:		Function library for game sounds.
//					Can load, store, and play music and sound effects.
//					Operates on FMOD.
//
//***************************************************************************************

#pragma once

#include "fmod.hpp"
#include <vector>
using std::vector;

//***************************************************************************************
//
//	Structure:	cSoundFile
//	Purpose:	Pairs a loaded sound with a key value
//
//***************************************************************************************
struct cSoundFile
{
	FMOD::Sound *sound;
	unsigned int key;
};

//--------------------------------------------------------------------------------------O

// Forward Declarations
bool initSound();
void updateSound();
void freeSound();

// PUBLIC LIBRARY FUNCTIONS -------------------------------
bool loadMusic(const char* filename, unsigned int id);
bool playMusic(unsigned int id);
bool loadSound(const char* filename, unsigned int id);
bool playSound(unsigned int id);

void pauseMusic();
void unpauseMusic();

void muteMusic();
void unmuteMusic();
void muteSound();
void unmuteSound();
// END PUBLIC FUNCTIONS -----------------------------------


// Global sound data
FMOD::System *slSystem(NULL);	// Pointer to FMOD system

bool slLoaded(false);			// Flags successful initialization
bool slSilence(true);			// Flags when no music is playing

unsigned int slCurrentTrack;	// Saves key of current track
bool slMuteSound(false);		// Toggles sound
bool slMuteMusic(false);		// Toggles music

vector<cSoundFile> slMusic;		// Stores music tracks
vector<cSoundFile> slSound;		// Stores sound effects
FMOD::Channel *slMusicChannel;	// Sound channel that music plays in



//***************************************************************************************
//
//	Function:	initSound
//	Purpose:	Initializes FMOD
//
//***************************************************************************************
bool initSound()
{
	bool fmodError(false);

	FMOD_RESULT result;

	result = FMOD::System_Create(&slSystem);		// Create the main system object.
	if (result != FMOD_OK)
	{
		fmodError = true;
		exit(-1);
	}

	result = slSystem->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
	if (result != FMOD_OK)
	{
		fmodError = true;
		exit(-1);
	}

	if(!fmodError)
		slLoaded = true;

	return fmodError;
}

//***************************************************************************************
//
//	Function:	updateSound
//	Purpose:	Updates sound system data
//
//***************************************************************************************
void updateSound()
{
	slSystem->update();
}

//***************************************************************************************
//
//	Function:	freeSound
//	Purpose:	Frees sound data
//
//***************************************************************************************
void freeSound()
{
	slSystem->release();
}

//***************************************************************************************
//
//	Function:	loadMusic
//	Purpose:	Loads a sound file into the music library, associates with key
//
//***************************************************************************************
bool loadMusic(const char* filename, unsigned int id)
{
	bool error = false;
	FMOD_RESULT result;

	if(slLoaded)
	{
		for(int i(0); i < slMusic.size(); i++)		// Check against existant keys
		{
			if(id == slMusic[i].key)
				error = true;
		}

		if(!error)									// If new key, load file
		{
			FMOD::Sound *music;
			result = slSystem->createSound(filename, FMOD_LOOP_BIDI, 0, &music);

			if(result == FMOD_OK)
			{
				cSoundFile file;
				file.sound = music;
				file.key = id;
				slMusic.push_back(file);
			}
			else
				error = true;
		}
	}
	else
		error = true;


	return error;
}

//***************************************************************************************
//
//	Function:	playMusic
//	Purpose:	Plays music associated with given key
//
//***************************************************************************************
bool playMusic(unsigned int id)
{
	bool error = true;

	if(slLoaded)
	{
		if(slMuteMusic)
		{
			error = false;
			slCurrentTrack = id;
		}
		else if(slSilence || id != slCurrentTrack)
		{
			FMOD_RESULT result;
			bool found(false);
			int i;

			for(i = 0; i < slMusic.size() && !found; i++)
			{
				if(slMusic[i].key == id)
					found = true;
			}

			if(found)
			{
				result = slSystem->playSound(FMOD_CHANNEL_REUSE, slMusic[i - 1].sound, false, &slMusicChannel);
				slCurrentTrack = id;
				slSilence = false;
				error = false;
			}
		}
	}

	return error;
}

//***************************************************************************************
//
//	Function:	loadSound
//	Purpose:	Loads a sound file into the sound effect library, associates with key
//
//***************************************************************************************
bool loadSound(const char* filename, unsigned int id)
{
	bool error = false;
	FMOD_RESULT result;

	if(slLoaded)
	{
		for(int i(0); i < slSound.size(); i++)		// Check against existant keys
		{
			if(id == slSound[i].key)
				error = true;
		}

		if(!error)									// If new key, load file
		{
			FMOD::Sound *sound;
			result = slSystem->createSound(filename, FMOD_DEFAULT, 0, &sound);

			if(result == FMOD_OK)
			{
				cSoundFile file;
				file.sound = sound;
				file.key = id;
				slSound.push_back(file);
			}
			else
				error = true;
		}
	}
	else
		error = true;


	return error;
}

//***************************************************************************************
//
//	Function:	playSound
//	Purpose:	Plays sound associated with given key
//
//***************************************************************************************
bool playSound(unsigned int id)
{
	bool error = true;

	if(slLoaded)
	{
		if(slMuteSound)
			error = false;
		else
		{
			FMOD_RESULT result;
			bool found(false);
			int i;

			for(i = 0; i < slSound.size() && !found; i++)
			{
				if(slSound[i].key == id)
					found = true;
			}

			if(found)
			{
				result = slSystem->playSound(FMOD_CHANNEL_FREE, slSound[i - 1].sound, false, NULL);
				error = false;
			}
		}
	}

	return error;
}

//***************************************************************************************
//
//	Function:	muteMusic
//	Purpose:	Mutes music system
//
//***************************************************************************************
void muteMusic()
{
	slMuteMusic = true;
	slMusicChannel->stop();
	slSilence = true;
}

//***************************************************************************************
//
//	Function:	unmuteMusic
//	Purpose:	Unmutes music system
//
//***************************************************************************************
void unmuteMusic()
{
	slMuteMusic = false;
	playMusic(slCurrentTrack);
}

//***************************************************************************************
//
//	Function:	muteSound
//	Purpose:	Mutes system sound
//
//***************************************************************************************
void muteSound()
{
	slMuteSound = true;
}

//***************************************************************************************
//
//	Function:	unmuteSound
//	Purpose:	Unmutes system sound
//
//***************************************************************************************
void unmuteSound()
{
	slMuteSound = false;
}

//***************************************************************************************
//
//	Function:	pauseMusic
//	Purpose:	Pauses current track
//
//***************************************************************************************
void pauseMusic()
{
	slMusicChannel->setPaused(true);
}

//***************************************************************************************
//
//	Function:	unpauseMusic
//	Purpose:	Unpauses current track
//
//***************************************************************************************
void unpauseMusic()
{
	slMusicChannel->setPaused(false);
}