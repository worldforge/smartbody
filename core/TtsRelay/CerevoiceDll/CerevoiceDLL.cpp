
#include "vhcl.h"

#include "CerevoiceDLL.h"

#include <vector>

#include <windows.h>
#include <io.h>
#include <direct.h>

#include "cerevoice_tts.h"

using std::string;
using std::vector;


cerevoice_tts * tts = NULL;

string g_fileName;
vector<double> g_wordBreakListStart;
vector<double> g_wordBreakListEnd;
vector<string> g_markListName;
vector<double> g_markListTime;
vector<string> g_visemeListType;
vector<double> g_visemeListStart;
vector<double> g_visemeListArticulation;


void process_message( const char * message );


BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
   switch ( fdwReason )
   {
      case DLL_PROCESS_ATTACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
      default:
         break;
   }

   return TRUE;
}



CEREVOICE_DLL_API bool CEREVOICE_DLL_Init(const char * visemeMapping)
{
   printf( "CerevoiceDLL - Init()\n" );

   vector<char *> voices;
   voices.push_back("katherine");
   voices.push_back("star");

   tts = new cerevoice_tts();
   tts->init( voices, visemeMapping );

   return true;
}


CEREVOICE_DLL_API void CEREVOICE_DLL_SetVisemeMapping(const char * visemeMapping)
{
   tts->setVisemeMapping(visemeMapping);
}


CEREVOICE_DLL_API bool CEREVOICE_DLL_GenerateAudio(const char * message, const char * outputFilename, const char * messageOutputFileName, const char * voice, char * xmlReplyReturn, CEREVOICE_DLL_GenerateAudioReplyInterop * generateAudioReplyInterop)
{
   g_wordBreakListStart.clear();
   g_wordBreakListEnd.clear();
   g_markListName.clear();
   g_markListTime.clear();
   g_visemeListType.clear();
   g_visemeListStart.clear();
   g_visemeListArticulation.clear();


   string cerevoiceMessage = vhcl::Format( "speak %s %s %s %s %s", "unused", "1", voice, outputFilename, message );

   process_message( cerevoiceMessage.c_str() );

   strcpy( xmlReplyReturn, "" );

   generateAudioReplyInterop->soundFile = NULL;
   generateAudioReplyInterop->workBreakListNum = 0;
   generateAudioReplyInterop->wordBreakListStart = NULL;
   generateAudioReplyInterop->wordBreakListEnd = NULL;
   generateAudioReplyInterop->markListNum = 0;
   generateAudioReplyInterop->markListName = NULL;
   generateAudioReplyInterop->markListTime = NULL;
   generateAudioReplyInterop->visemeListNum = 0;
   generateAudioReplyInterop->visemeListType = NULL;
   generateAudioReplyInterop->visemeListStart = NULL;
   generateAudioReplyInterop->visemeListArticulation = NULL;

   generateAudioReplyInterop->soundFile = new char [g_fileName.size() + 1];
   strcpy(generateAudioReplyInterop->soundFile, g_fileName.c_str());


   generateAudioReplyInterop->workBreakListNum = g_wordBreakListStart.size();
   generateAudioReplyInterop->wordBreakListStart = new double [g_wordBreakListStart.size()];
   if (!g_wordBreakListStart.empty())
      memcpy(generateAudioReplyInterop->wordBreakListStart, &g_wordBreakListStart[0], sizeof(double) * g_wordBreakListStart.size());

   generateAudioReplyInterop->wordBreakListEnd = new double [g_wordBreakListEnd.size()];
   if (!g_wordBreakListEnd.empty())
      memcpy(generateAudioReplyInterop->wordBreakListEnd, &g_wordBreakListEnd[0], sizeof(double) * g_wordBreakListEnd.size());


   generateAudioReplyInterop->markListNum = g_markListName.size();
   generateAudioReplyInterop->markListName = new char * [g_markListName.size()];
   for (size_t i = 0; i < g_markListName.size(); i++)
   {
      generateAudioReplyInterop->markListName[i] = new char [g_markListName[i].size() + 1];
      strcpy(generateAudioReplyInterop->markListName[i], g_markListName[i].c_str());
   }

   generateAudioReplyInterop->markListTime = new double [g_markListTime.size()];
   if (!g_markListTime.empty())
      memcpy(generateAudioReplyInterop->markListTime, &g_markListTime[0], sizeof(double) * g_markListTime.size());


   generateAudioReplyInterop->visemeListNum = g_visemeListType.size();
   generateAudioReplyInterop->visemeListType = new char * [g_visemeListType.size()];
   for (size_t i = 0; i < g_visemeListType.size(); i++)
   {
      generateAudioReplyInterop->visemeListType[i] = new char [g_visemeListType[i].size() + 1];
      strcpy(generateAudioReplyInterop->visemeListType[i], g_visemeListType[i].c_str());
   }

   generateAudioReplyInterop->visemeListStart = new double [g_visemeListStart.size()];
   if (!g_visemeListStart.empty())
      memcpy(generateAudioReplyInterop->visemeListStart, &g_visemeListStart[0], sizeof(double) * g_visemeListStart.size());

   generateAudioReplyInterop->visemeListArticulation = new double [g_visemeListArticulation.size()];
   if (!g_visemeListArticulation.empty())
      memcpy(generateAudioReplyInterop->visemeListArticulation, &g_visemeListArticulation[0], sizeof(double) * g_visemeListArticulation.size());

   return true;
}


CEREVOICE_DLL_API bool CEREVOICE_DLL_Close()
{
   delete tts;
   tts = NULL;
   return true;
}


void process_message( const char * message )
{
   /// Processes RemoteSpeechCmd messages to generate audio

   std::string message_c = message;

   // parse the string
   std::vector< std::string > tokens;
   const std::string delimiters = " ";
   vhcl::Tokenize( message_c, tokens, delimiters );

   /// Get non-XML components of the message
   std::string command = tokens.at( 0 );

   std::string agent_name = tokens.at( 1 );
   std::string message_id = tokens.at( 2 );
   std::string voice_id = tokens.at( 3 );
   std::string file_name = tokens.at( 4 );
   size_t prefix_length = message_c.find( file_name, 0 ) + file_name.length() + 1;
   std::string utterance = message_c.substr( prefix_length );  // strip off the prefix, only using the xml

#ifdef _DUMP_COMM_TO_DISK
   fprintf(_inXML,"%d: %s\n\n",_dumpCounter - 1, utterance.c_str());
#endif

   // remove anything after </speech> tag
   size_t postfix_pos = utterance.rfind( "</speech>" );
   if ( postfix_pos != std::string::npos )
      utterance = utterance.substr( 0, postfix_pos + 9 );

   // parse out just the sound file name and give it a .wav file type
   int pos = file_name.find( ".aiff" );
   int pos2 = file_name.find( "utt" );

   // obtaining the directory path where the output sound file is to be written

   char directory[_MAX_PATH];

   strcpy(directory,((std::string)file_name.substr(0, pos2)).c_str());
   tts->temp_audio_dir_cereproc = directory;
   // converting the directory path to an absolute path
   char full[ _MAX_PATH ];
   if ( _fullpath( full, tts->temp_audio_dir_cereproc, _MAX_PATH ) == NULL )
   {
      printf("/nError converting path sent from SBM to absolute path/n");
   }

   char absolute_directory[_MAX_PATH];
   strcpy(absolute_directory,full);
   tts->temp_audio_dir_player = absolute_directory;

   file_name = file_name.substr( pos2, pos - pos2 ) + ".wav";


   // Make sure the audio temp directory exists and create if not
   // CPRC_riff_save will not create the directory if non-existent
   if( !(_access( tts->temp_audio_dir_cereproc, 0 ) == 0 ) )
   {
      std::string temp = "";
      std::vector< std::string > tokens2;
      const std::string delimiters2 = "\\/";

      vhcl::Tokenize( tts->temp_audio_dir_cereproc, tokens2, delimiters2 );

      printf( "Warning, audio cache directory, %s, does not exist. Creating directory...\n", tts->temp_audio_dir_cereproc );
      for (unsigned int i = 0; i < tokens2.size(); i++)
      {
         temp += tokens2.at( i ) + "\\";
         _mkdir( temp.c_str() );
      }

      // Check if directory has been created. Should be done more elegantly, including sending message or timing out.
      if( !(_access( tts->temp_audio_dir_cereproc, 0 ) == 0 ) )
      {
         printf( "ERROR: audio cache directory, %s, could not be created. This will likely lead to errors down the line.\\n", tts->temp_audio_dir_cereproc );
      }
   }


   // Create file name relative to cerevoice relay
   /**
    * Clarification:
    *   cereproc_file_name refers to the path that cereproc needs to write to, relative to the path from where this program is running
    *   player_file_name refers to the path that Unreal or some other renderer will play the file from, i.e. relative to the path where it is running
   */
   std::string cereproc_file_name = tts->temp_audio_dir_cereproc + file_name;
   std::string player_file_name = tts->temp_audio_dir_player + file_name;

   g_fileName = player_file_name;

   /// Generate the audio
   tts->tts( utterance.c_str(), cereproc_file_name.c_str(), player_file_name.c_str(), voice_id );

#ifdef _DUMP_COMM_TO_DISK
   {
      fprintf(_outfile, "%d: %s\n\n",_dumpCounter - 1,reply.c_str());
      std::string dumpXMLstring = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + xml;
      fprintf(_outXML, "%d: %s\n\n", _dumpCounter - 1, dumpXMLstring.c_str() );
   }
#endif
}
