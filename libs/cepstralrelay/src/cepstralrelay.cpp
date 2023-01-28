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
*/

#include "vhcl.h"

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <winsock.h>
#include <direct.h>
#include <io.h>

#include <string>
#include <vector>
#include <iostream>

#include "vhmsg-tt.h"
#include "cepstral_tts.h"


cepstral_tts * tts = NULL;

std::string directory = "";


void process_message( const char * message )
{
   std::string message_c = message;

   // parse the string
   std::vector< std::string > tokens;
   const std::string delimiters = " ";
   vhcl::Tokenize( message_c, tokens, delimiters );

   std::string command = tokens.at( 0 );

   std::string agent_name = tokens.at( 1 );
   std::string message_id = tokens.at( 2 );
   std::string voice = tokens.at( 3 );
   std::string file_name = tokens.at( 4 );
   size_t prefix_length = message_c.find( file_name, 0 ) + file_name.length() + 1;
   std::string utterance = message_c.substr( prefix_length );  // strip off the prefix, only using the xml

   // remove anything after </speech> tag
   size_t postfix_pos = utterance.rfind( "</speech>" );
   if ( postfix_pos != std::string::npos )
      utterance = utterance.substr( 0, postfix_pos + 9 );

   //parse out just the sound file name and give it a .wav file type
   int pos = file_name.find( ".aiff" );
   int pos2 = file_name.find( "utt" );

   // obtaining the directory path where the output sound file is to be written
   directory = file_name.substr(0, pos2);
   char full[ _MAX_PATH ];
   if ( _fullpath( full, directory.c_str(), _MAX_PATH ) == NULL )
   {
		printf("/nError converting path sent from SBM to absolute path/n");
   }
   
   directory = std::string(full);

   // Make sure the audio temp directory exists and create if not
   // CPRC_riff_save will not create the directory if non-existent
   if( !(_access( directory.c_str(), 0 ) == 0 ) )
   {
      std::string temp = "";
      std::vector< std::string > tokens;
      const std::string delimiters = "\\\\";
      vhcl::Tokenize( directory, tokens, delimiters );

      printf( "Warning, audio temp directory, %s, does not exist. Creating directory...\n", directory );
      for (unsigned int i = 0; i < tokens.size(); i++)
      {
         temp += tokens.at( i ) + "\\";
         _mkdir( temp.c_str() );
      }
   }

   // obtaining only the filename from the entire path
   file_name = file_name.substr( pos2, pos - pos2 ) + ".wav";

   //set the full file name
   file_name = directory + file_name;

   std::string xml = tts->tts( utterance.c_str(), file_name.c_str() );

   std::string reply = agent_name;
   reply += " ";
   reply += message_id;
   reply += " OK: <?xml version=\"1.0\" encoding=\"UTF-8\"?>";
   reply += xml;

   printf( "REPLY: %s\n", reply.c_str() );

   vhmsg::ttu_notify2( "RemoteSpeechReply", reply.c_str() );
}


void vhmsg_callback( const char * op, const char * args, void * userData )
{
   printf( "received -- op: %s args: %s\n", op, args );

   if ( strcmp( op, "RemoteSpeechCmd" ) == 0 )
   {
      process_message( args );
   }
}


int main( int argc, char * argv[] )
{
   //get the saso root
   //char * saso_root = getenv( "SASO_ROOT" );


   // Commented by Shridhar on 10/09/09
   // Now the output sound-file path is no longer hardcoded, it is obtained from SBM through the RemoteSpeechCmd
   //if ( saso_root != NULL )
   //{
   //   std::string saso_root_string = saso_root;
   //   directory = saso_root_string + "\\dimr\\tmpaudio\\";
   //   std::cout << "Audio files will be saved to: " << directory << "\n";
   //}
   //else
   //{
   //   //if the saso_root is not set, output the audio to the c drive
   //   directory = "..\\..\\..\\dimr\\tmpaudio\\";
   //   std::cout << "SASO_ROOT not set, audio files will be saved to: " << directory << "\n";
   //}


   // the below path is only for display purposes. The actual path is obtained from SBM (currently happens to be same as below 10/09/09)
   directory = "..\\..\\data\\cache\\audio";
   std::cout << "Audio files will be saved to: " << directory << "\n";
   std::cout << "This is the default path. Actual path will be obtained from SBM through RemoteSpeechCmd Message.\n";

   vhmsg::ttu_set_client_callback( vhmsg_callback );
   int err = vhmsg::ttu_open();
   if ( err != vhmsg::TTU_SUCCESS )
   {
      printf( "unable to connect to message server, aborting\n" );
      return -1;
   }

   vhmsg::ttu_register( "RemoteSpeechCmd" );

   tts = new cepstral_tts();

   tts->init();

   for ( ;; )
   {
      if ( _kbhit() )
      {
         int c = _getch();
         if ( c == 'q' )
         {
            break;
         }
      }

      vhmsg::ttu_poll();
      Sleep( 10 );
   }
}
