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

#ifndef _SBSPEECHMANAGER_H_
#define _SBSPEECHMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBObject.h>

class remote_speech;
class local_speech;
class text_speech;
class FestivalSpeechRelayLocal;
class CereprocSpeechRelayLocal;

namespace  SmartBody 
{
	class AudioFileSpeech;
}

namespace SmartBody {

//TODO: move out into separate module

class SBSpeechManager : public SBObject
{
	public:
		SBAPI explicit SBSpeechManager(SBScene& scene);
		SBAPI ~SBSpeechManager();

		SBAPI FestivalSpeechRelayLocal* festivalRelay();
		SBAPI CereprocSpeechRelayLocal* cereprocRelay();
		SBAPI remote_speech* speech_rvoice();
		SBAPI local_speech* speech_localvoice();
		SBAPI SmartBody::AudioFileSpeech* speech_audiofile();
		SBAPI text_speech* speech_text();

	protected:
		std::unique_ptr<remote_speech> _speech_rvoice;
		std::unique_ptr<local_speech> _speech_localvoice;
		std::unique_ptr<SmartBody::AudioFileSpeech> _speech_audiofile;
		std::unique_ptr<text_speech> _speech_text;
		std::unique_ptr<FestivalSpeechRelayLocal> _festivalRelayLocal;
		std::unique_ptr<CereprocSpeechRelayLocal> _cereprocRelayLocal;
};

}

#endif