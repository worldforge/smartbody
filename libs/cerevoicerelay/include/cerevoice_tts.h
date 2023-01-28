/*
   Part of SBM: SmartBody Module
   Copyright (C) 2008  University of Southern California

   SBM is free software: you can redistribute it and/or
   modify it under the terms of the Lesser GNU General Public License
   as published by the Free Software Foundation, version 3 of the
   license.

   SBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   Lesser GNU General Public License for more details.

   You should have received a copy of the Lesser GNU General Public
   License along with SBM.  If not, see:
       http://www.gnu.org/licenses/lgpl-3.0.txt

   CONTRIBUTORS:
      Edward Fast, USC
      Thomas Amundsen, USC
      Arno Hartholt, USC
	  Abhinav Golas, USC
*/

#ifndef CEREVOICE_TTS_H
#define CEREVOICE_TTS_H

#include <string>

struct CPRCEN_engine; 

class cerevoice_tts
{
   public:
      /* Path where the voice and license files can be found*/
      char * voice_path;

      /* Extension of voice file (usually .voice) */
      char * voice_file_extension;

      /* Extension of license file (usually .lic) */
      char * license_file_extension;

      /* Directory where audio files get saved, relative to CereProc. Can be absolute */
      char * temp_audio_dir_cereproc;

      /* Directory where audio files get saved, relative to module that plays the sound 
      (like Unreal Tournament). Can be absolute */
      char * temp_audio_dir_player;

	  CPRCEN_engine* voiceEngine;

      /* Initializes CereVoiceRelay, given a vector of voice_id's */
      void init( std::vector<char *> );

      /* Constructs an audio file based on the text and sends out a reply message */
      std::string tts( const char * text, const char * cereproc_file_name, const char * player_file_name, std::string voice_id );

protected:
      void load_voice( char * voice_id );

	  std::string addUselTag(std::string);
         
};


#endif // CEREVOICE_TTS_H
