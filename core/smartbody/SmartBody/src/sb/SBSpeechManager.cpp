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

SBSpeechManager::SBSpeechManager() : SBObject()
{
	_speech_rvoice = new remote_speech();
	_speech_localvoice = new local_speech();
	_speech_audiofile = new SmartBody::AudioFileSpeech();
	_speech_text = new text_speech();
	_festivalRelayLocal = new FestivalSpeechRelayLocal();
	_cereprocRelayLocal = new CereprocSpeechRelayLocal();
}

SBSpeechManager::~SBSpeechManager()
{
	delete _speech_rvoice;
	delete _speech_localvoice;
	delete _speech_audiofile;
	delete _speech_text;
	delete _festivalRelayLocal;
	delete _cereprocRelayLocal;
}

FestivalSpeechRelayLocal* SBSpeechManager::festivalRelay()
{ 
	return _festivalRelayLocal;
}

CereprocSpeechRelayLocal* SBSpeechManager::cereprocRelay()
{ 
	return _cereprocRelayLocal;
}

remote_speech* SBSpeechManager::speech_rvoice()
{ 
	return _speech_rvoice;
}

local_speech* SBSpeechManager::speech_localvoice()
{
	return _speech_localvoice;
}

SmartBody::AudioFileSpeech* SBSpeechManager::speech_audiofile()
{ 
	return _speech_audiofile;
}

text_speech* SBSpeechManager::speech_text()
{ 
	return _speech_text;
} 

}
