/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

// sbm_audio.cpp

#include "sbm_audio.h"

#ifndef SB_NO_VHCL_AUDIO

#include "vhcl.h"
#include "vhcl_audio.h"
#include <sstream>
#include <cstdlib>
#include "SBUtilities.h"

#define USEAUDIODURATION 1

static vhcl::Audio * g_audio = nullptr;

bool AUDIO_Init()
{
	SmartBody::util::log("Initializing AUDIO...");
	if (g_audio)
	{
		SmartBody::util::log("Could not initialize AUDIO...");
		return false;
	}

	g_audio = new vhcl::Audio();
	bool ret = g_audio->Open();
	if (!ret)
		SmartBody::util::log("Could not perform AUDIO_Init()");
	return ret;
}

float AUDIO_Play( const char * audio_file )
{
	float duration = 0.0f;
	vhcl::Sound * sound = g_audio->CreateSoundLibSndFile( audio_file, audio_file );
	if ( sound )
	{
		sound->Play();
#ifdef USEAUDIODURATION
		// determine the length of the audio file
		float numSamples = (float) sound->getSampleSize();
		float sampleRate = (float) sound->getSampleRate();
		
		if (sampleRate != 0.0)
		{
			duration = numSamples / sampleRate;
		}
#endif
		return duration;
	}
	else
	{
		SmartBody::util::log("no sound available from %s", audio_file);
		return 0.0f;
	}

}

void AUDIO_Stop( const char * audio_file )
{
	if (g_audio)
		g_audio->DestroySound(audio_file);
}


void AUDIO_Close()
{
	if (!g_audio)
		return;
	g_audio->Close();
	delete g_audio;
	g_audio = nullptr;
}

#else

bool AUDIO_Init()
{
  return true;
}


float AUDIO_Play( const char * audio_file )
{
	return 0.0f;
}

void AUDIO_Stop( const char * audio_file )
{
}


void AUDIO_Close()
{
}

#endif

