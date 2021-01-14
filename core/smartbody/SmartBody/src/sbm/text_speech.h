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

#ifndef TEXT_SPEECH_H
#define TEXT_SPEECH_H


// Predeclare class
class text_speech;


#include <string>
#include <sb/SBCommandManager.h>
#include "sbm/sbm_speech.hpp"
#include "sr_hash_map.h"
#include "sr_arg_buff.h"
#include "sb/sbm_character.hpp"
// Predeclare class

class srCmdSeq;


class text_speech: public SmartBody::SpeechInterface, public SmartBody::SBSceneOwned {
    public:
		// Default Constructor/Destructor
		text_speech(SmartBody::SBScene& scene);
		virtual ~text_speech();

		// Methods
		SmartBody::RequestId requestSpeechAudio( const char* agentName, const std::string voiceCode, const DOMNode* node, const char* callbackCmd ) override; //accepts dom document of which sound will be created from, returns Request ID
		SmartBody::RequestId requestSpeechAudio( const char* agentName, const std::string voiceCode, std::string text, const char* callbackCmd ) override; //accepts char* of above and returns request ID
		std::vector<SmartBody::VisemeData *>* getVisemes( SmartBody::RequestId requestId, SbmCharacter* character = nullptr ) override; //returns visemes  for given request
		char* getSpeechPlayCommand( SmartBody::RequestId requestId, SbmCharacter* character = nullptr ) override; //returns the command to play speech
		char* getSpeechStopCommand( SmartBody::RequestId requestId, SbmCharacter* character = nullptr ) override; //''                     stop
		char* getSpeechAudioFilename( SmartBody::RequestId requestId ) override; // gets the fileName of speech
		float getMarkTime( SmartBody::RequestId requestId, const XMLCh* markId ) override; //gets time value for a given marker
		void requestComplete( SmartBody::RequestId requestId ) override;

		std::vector<float> getEmotionCurve(SmartBody::RequestId requestId, const std::string& emotionType, SbmCharacter* character = nullptr) override;
		std::vector<std::string> getEmotionNames(SmartBody::RequestId requestId, SbmCharacter* character = nullptr) override;

		void startSchedule( SmartBody::RequestId requestId );
	private:
		std::vector<SmartBody::VisemeData *>* extractVisemes(DOMNode* node, std::vector<SmartBody::VisemeData*>* visemes, const SbmCharacter* character);
		std::string forPlaysound;
		std::map<std::string, std::unique_ptr<DOMNode>>     uttLookUp;
		srHashMap<srCmdSeq>    scheduleLookUp;
		srHashMap<std::string> charLookUp;
		unsigned int           msgNumber;

};



// included after class definition b/c dependency


#endif
