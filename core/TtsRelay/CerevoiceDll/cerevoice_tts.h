
#ifndef CEREVOICE_TTS_H
#define CEREVOICE_TTS_H

#include <string>


class cerevoice_tts
{
   public:
      // Path where the voice and license files can be found
      char * voice_path;

      // Extension of voice file (usually .voice)
      char * voice_file_extension;

      // Extension of license file (usually .lic)
      char * license_file_extension;

      // Directory where audio files get saved, relative to CereProc. Can be absolute
      char * temp_audio_dir_cereproc;

      // Directory where audio files get saved, relative to module that plays the sound (like Unreal Tournament). Can be absolute
      char * temp_audio_dir_player;

      // Initializes CereVoiceRelay, given a vector of voice_id's
      void init( std::vector<char *> vctVoices, const std::string & visemeMapping );

      void setVisemeMapping( const std::string & visemeMapping );

      // Constructs an audio file based on the text and sends out a reply message
      void tts( const char * text, const char * cereproc_file_name, const char * player_file_name, const std::string & voice_id );

protected:
      void load_voice( const char * voice_id );

      std::string addUselTag(std::string text);
};


#endif // CEREVOICE_TTS_H
