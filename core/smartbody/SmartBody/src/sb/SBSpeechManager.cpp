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

#include "SBSpeechManager.h"
#include <sbm/remote_speech.h>
#include <sbm/local_speech.h>
#include <sbm/text_speech.h>
#include <sbm/sbm_speech_audiofile.hpp>


namespace SmartBody {

SBSpeechManager::SBSpeechManager(SBScene& scene) : SBObject()
{
	_speech_rvoice = std::make_unique<remote_speech>(scene);
	_speech_localvoice = std::make_unique<local_speech>(scene);
	_speech_audiofile = std::make_unique<SmartBody::AudioFileSpeech>();
	_speech_text = std::make_unique<text_speech>();
	_festivalRelayLocal = std::make_unique<FestivalSpeechRelayLocal>();
	_cereprocRelayLocal = std::make_unique<CereprocSpeechRelayLocal>();
}

SBSpeechManager::~SBSpeechManager() = default;

FestivalSpeechRelayLocal* SBSpeechManager::festivalRelay()
{ 
	return _festivalRelayLocal.get();
}

CereprocSpeechRelayLocal* SBSpeechManager::cereprocRelay()
{ 
	return _cereprocRelayLocal.get();
}

remote_speech* SBSpeechManager::speech_rvoice()
{ 
	return _speech_rvoice.get();
}

local_speech* SBSpeechManager::speech_localvoice()
{
	return _speech_localvoice.get();
}

SmartBody::AudioFileSpeech* SBSpeechManager::speech_audiofile()
{ 
	return _speech_audiofile.get();
}

text_speech* SBSpeechManager::speech_text()
{ 
	return _speech_text.get();
} 

}
