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
#include "SBUtilities.h"
#include "sb/SBScene.h"
#include "sb/SBAttribute.h"

#include "vhcl.h"
#include "vhcl_audio.h"
#include <sstream>
#include <cstdlib>

#define USEAUDIODURATION 1

static vhcl::Audio* g_audio = nullptr;

void SmartBody::attachAudioToScene(SBScene& scene) {
	struct AudioObserver : public SBObserver {
		void notify(SBSubject* subject) override {
			auto* boolAttr = dynamic_cast<BoolAttribute*>(subject);

			if (boolAttr && boolAttr->getName() == "internalAudio") {
				bool val = boolAttr->getValue();
				if (!val) {
					SmartBody::util::log("Turning off audio...");
					AUDIO_Close();
				} else {
					SmartBody::util::log("Turning on audio...");
					AUDIO_Init();
				}
				return;
			}
		}

	};

	scene.registerObserver(new AudioObserver());
}


bool AUDIO_Init() {
	SmartBody::util::log("Initializing AUDIO...");
	if (g_audio) {
		SmartBody::util::log("Could not initialize AUDIO...");
		return false;
	}

	g_audio = new vhcl::Audio();
	bool ret = g_audio->Open();
	if (!ret)
		SmartBody::util::log("Could not perform AUDIO_Init()");
	return ret;
}

float AUDIO_Play(const char* audio_file) {
	float duration = 0.0f;
	vhcl::Sound* sound = g_audio->CreateSoundLibSndFile(audio_file, audio_file);
	if (sound) {
		sound->Play();
#ifdef USEAUDIODURATION
		// determine the length of the audio file
		float numSamples = (float) sound->getSampleSize();
		float sampleRate = (float) sound->getSampleRate();

		if (sampleRate != 0.0) {
			duration = numSamples / sampleRate;
		}
#endif
		return duration;
	} else {
		SmartBody::util::log("no sound available from %s", audio_file);
		return 0.0f;
	}

}

void AUDIO_Stop(const char* audio_file) {
	if (g_audio)
		g_audio->DestroySound(audio_file);
}


void AUDIO_Close() {
	if (!g_audio)
		return;
	g_audio->Close();
	delete g_audio;
	g_audio = nullptr;
}


