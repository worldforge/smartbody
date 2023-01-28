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

#ifndef SBM_SPEECH_IMPL_SKELETON_HPP
#define SBM_SPEECH_IMPL_SKELETON_HPP

#include "sbm_speech.hpp"


// TODO - replace SpeechImplementation with actual class name
class SpeechImplementation : public SmartBody::SpeechInterface {
public:
	// Constructor / Destructor
	SpeechImplementation();
	virtual ~SpeechImplementation();

	//  Override SpeechInterface methods (see sbm_speech.hpp)
    virtual SmartBody::RequestId requestSpeechAudio( const char* agentName, const DOMNode* node, const char* callbackCmd );
    virtual SmartBody::RequestId requestSpeechAudio( const char* agentName, const char* text, const char* callbackCmd );
//    virtual const std::vector<SmartBody::VisemeData *>* getVisemes( RequestId requestId );
    virtual char* getSpeechPlayCommand( SmartBody::RequestId requestId );
    virtual char* getSpeechStopCommand( SmartBody::RequestId requestId );
    virtual char* getSpeechAudioFilename( SmartBody::RequestId requestId );
    virtual float getMarkTime( SmartBody::RequestId requestId, const XMLCh* markId );
    virtual void requestComplete( SmartBody::RequestId requestId );
};


#endif // SBM_SPEECH_IMPL_SKELETON_HPP
