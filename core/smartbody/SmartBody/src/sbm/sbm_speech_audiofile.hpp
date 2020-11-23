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

#ifndef SBM_SPEECH_AUDIOFILE_HPP
#define SBM_SPEECH_AUDIOFILE_HPP

#include <sb/SBTypes.h>
//#include <hash_map>
#include <map>

#include "sbm/sbm_speech.hpp"
#include "rapidxml.hpp"

namespace SmartBody
{

class AudioFileSpeech : public SpeechInterface
{
   public:
      struct SpeechRequestInfo
      {
         std::vector< VisemeData > visemeData;
//         stdext::hash_map< std::string, float > timeMarkers;
         std::map< std::string, float > timeMarkers;
         std::string id;
         std::string audioFilename;
         std::string playCommand;
         std::string stopCommand;
		 std::map<std::string, std::vector<float> >	emotionData;
      };

   private:
      XercesDOMParser * m_xmlParser;
      HandlerBase * m_xmlHandler;
      int m_requestIdCounter;
	  bool visemeCurveMode;
	  bool useMotionByDefault;
	  bool useMotion;

//      stdext::hash_map< RequestId, SpeechRequestInfo > m_speechRequestInfo;
      std::map< RequestId, SpeechRequestInfo > m_speechRequestInfo;

   public:
      AudioFileSpeech();
      virtual ~AudioFileSpeech();

      virtual RequestId requestSpeechAudio( const char * agentName, const std::string voiceCode, const DOMNode * node, const char * callbackCmd );
	  virtual RequestId requestSpeechAudio( const char * agentName, const std::string voiceCode, std::string text, const char * callbackCmd );
	  virtual RequestId requestSpeechAudioFast( const char * agentName, const std::string voiceCode, std::string text, const char * callbackCmd );
      virtual std::vector<VisemeData *> * getVisemes( RequestId requestId, SbmCharacter* character);
	  virtual std::vector<float> getEmotionCurve(RequestId requestId, const std::string& emotionType, SbmCharacter* character = nullptr);
	  virtual std::vector<std::string> getEmotionNames(RequestId requestId, SbmCharacter* character = nullptr);
      virtual char * getSpeechPlayCommand( RequestId requestId, SbmCharacter * character = nullptr );
      virtual char * getSpeechStopCommand( RequestId requestId, SbmCharacter * character = nullptr );
      virtual char * getSpeechAudioFilename( RequestId requestId );
      virtual float getMarkTime( RequestId requestId, const XMLCh * markId );
      virtual void requestComplete( RequestId requestId );
//	  stdext::hash_map< RequestId, SpeechRequestInfo >& getSpeechRequestInfo();
	  SBAPI std::map< RequestId, SpeechRequestInfo >& getSpeechRequestInfo();

	  void setVisemeMode(bool mode) {visemeCurveMode = mode;}
	  void setMotionMode(bool mode) {useMotionByDefault = mode;}

   protected:
      virtual void ReadVisemeDataLTF( const char * filename, std::vector< VisemeData > & visemeData );
      virtual void ReadVisemeDataBML( const char * filename, std::vector< VisemeData > & visemeData, const SbmCharacter* character );
	  virtual void ReadMotionDataBML( const char * filename, std::vector< VisemeData > & visemeData);
	  virtual void ReadEmotionData(const char* filename, std::map<std::string, std::vector<float> >& emotionData);
//      virtual void ReadSpeechTiming( const char * filename, stdext::hash_map< std::string, float > & timeMarkers );
      virtual void ReadSpeechTiming( const char * filename, std::map< std::string, float > & timeMarkers );
	  virtual void ReadSpeechTimingFast( const char * filename, std::map< std::string, float > & timeMarkers, rapidxml::xml_document<>& bmlDoc);
	  virtual void ReadVisemeDataBMLFast( const char * filename, std::vector< VisemeData > & visemeData, const SbmCharacter* character, rapidxml::xml_document<>& bmldoc);

	  std::map<std::string, DOMDocument*> xmlCache;
};

};


#endif // SBM_SPEECH_AUDIOFILE_HPP
