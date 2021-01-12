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

#ifndef REMOTE_SPEECH_H
#define REMOTE_SPEECH_H


// Predeclare class
class remote_speech;


#include <string>
#include <sb/SBCommandManager.h>
#include "sbm/sbm_speech.hpp"
#include "sr_hash_map.h"
#include "sr_arg_buff.h"
#include "sb/sbm_character.hpp"
// Predeclare class

namespace SmartBody {
	class SBSpeechManager;
}

int remoteSpeechResult_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBSpeechManager& speechManager );
int set_char_voice(char* char_name, char* voiceCode);
int remoteSpeechTimeOut_func(srArgBuffer& args, SmartBody::SBSpeechManager& speechManager );

// Test functions
// TODO: move to "test ..."
int remote_speech_test( srArgBuffer& args, SmartBody::SBScene& scene);
int remoteSpeechReady_func( srArgBuffer& args, SmartBody::SBScene& scene);

class remote_speech: public SmartBody::SpeechInterface, public SmartBody::SBSceneOwned {
    public:
        /**
         *  Requests audio for a speech by agentName as specified in XML node.
         *  If node is a DOMElement, use the child nodes.
         *  If node is a text node, is the value a string.
         *  If node is an attribute, use the attribute value as a string.
         *  Returns a unique request identifier.
         */

		// Default Constructor/Destructor
		explicit remote_speech(SmartBody::SBScene& scene, float timeOutInSeconds = 20 );
		virtual ~remote_speech();

		// Methods
		virtual SmartBody::RequestId requestSpeechAudio( const char* agentName, std::string voiceCode, const DOMNode* node, const char* callbackCmd ); //accepts dom document of which sound will be created from, returns Request ID
		virtual SmartBody::RequestId requestSpeechAudio( const char* agentName, std::string voiceCode, std::string text, const char* callbackCmd ); //accepts char* of above and returns request ID
		virtual std::vector<SmartBody::VisemeData *>* getVisemes( SmartBody::RequestId requestId,  SbmCharacter* character ); //returns visemes  for given request
		virtual char* getSpeechPlayCommand( SmartBody::RequestId requestId, SbmCharacter* character = nullptr ); //returns the command to play speech
		virtual char* getSpeechStopCommand( SmartBody::RequestId requestId, SbmCharacter* character = nullptr ); //''                     stop
		virtual char* getSpeechAudioFilename( SmartBody::RequestId requestId ); // gets the fileName of speech
		virtual float getMarkTime( SmartBody::RequestId requestId, const XMLCh* markId ); //gets time value for a given marker
		virtual std::vector<float> getEmotionCurve(SmartBody::RequestId requestId, const std::string& emotionType, SbmCharacter* character = nullptr);
		virtual std::vector<std::string> getEmotionNames(SmartBody::RequestId requestId, SbmCharacter* character = nullptr);

		virtual void requestComplete( SmartBody::RequestId requestId );

		// RemoteSpeech specific methods
		int handleRemoteSpeechResult( SbmCharacter* character, char* msgID, char* status, char* result);
		int testRemoteSpeechTimeOut( const char* request_id_str);
	protected:
		virtual void sendSpeechCommand(const char* cmd);
		virtual void sendSpeechTimeout(std::ostringstream& outStream);

		
	protected:
		std::vector<SmartBody::VisemeData *>* extractVisemes(DOMNode* node, std::vector<SmartBody::VisemeData*>* visemes, SbmCharacter* character);
		std::string forPlaysound;
		srHashMap<DOMNode>     uttLookUp; 
		srHashMap<std::string> soundLookUp;
		srHashMap<const char>  commandLookUp;
//		srHashMap<std::string> remote_speech::charLookUp;
		srHashMap<std::string> charLookUp;
		unsigned int           msgNumber;
		float                  timeOut; //seconds for timeout between sbm's RvoiceRequest and Remote speech process's RmoteSpeechReply
};



// included after class definition b/c dependency
//



#endif // REMOTE_SPEECH_H
