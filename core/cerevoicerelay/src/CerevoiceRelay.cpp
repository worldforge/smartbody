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
#include "cerevoice_tts.h"

/// Debug support code - enable to get a dump of all incoming and outgoing messages from this code
/// Enable this define to get dumps
//#define _DUMP_COMM_TO_DISK

#ifdef _DUMP_COMM_TO_DISK
FILE *_infile = fopen("cere_incoming.txt","w");
FILE *_outfile = fopen("cere_outgoing.txt","w");

FILE *_inXML = fopen("cere_incoming_xml.txt","w");
FILE *_outXML = fopen("cere_outgoing_xml.txt","w");
int _dumpCounter = 0;
#endif



const int NETWORK_PORT_TCP = 1314;
const char * host_name;
cerevoice_tts * tts;
SOCKET sock_tcp = NULL;
sockaddr_in toAddr_tcp;
int toSize_tcp = sizeof(toAddr_tcp);
bool bServerMode;


int InitWinsock( const char * network_host )
{
	fprintf(stderr, "Notify: Opening TCP/IP connection to network host: %s\n",network_host);
   WSADATA WSAData;
   INT Code = WSAStartup( MAKEWORD(2,2), &WSAData );

   if ( Code != 0 )
   {
      return -1;
   }

   sock_tcp = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

   if ( sock_tcp == INVALID_SOCKET )
   {
      printf( "Couldn't create socket2.\n" );

      int errnum = WSAGetLastError();
      printf( "socket error: %d\n", errnum );

      sock_tcp = NULL;
      WSACleanup();

      return -1;
   }

   // see if we're specifying a host by name or by number
   if ( isalpha( network_host[ 0 ] ) )
   {
      hostent * host = gethostbyname( network_host );

      if ( host == NULL )
      {
         printf( "gethostbyname() failed.\n" );

         int errnum = WSAGetLastError();
         printf( "socket error: %d\n", errnum );

         closesocket( sock_tcp );
         sock_tcp = NULL;
         WSACleanup();

         return -1;
      }

      toAddr_tcp.sin_family = AF_INET;
      toAddr_tcp.sin_addr = *( ( in_addr* )host->h_addr );
      toAddr_tcp.sin_port = htons( NETWORK_PORT_TCP );
   }
   else
   {
      toAddr_tcp.sin_family = AF_INET;
      toAddr_tcp.sin_addr.s_addr = inet_addr( network_host );
      toAddr_tcp.sin_port = htons( NETWORK_PORT_TCP );
   }

   {
      int ret;
      ret = connect( sock_tcp, (SOCKADDR*)&toAddr_tcp, toSize_tcp );

      if ( ret < 0 )
      {
         printf( "connect() failed.\n" );

         int errnum = WSAGetLastError();
         printf( "socket error: %d\n", errnum );

         closesocket( sock_tcp );
         sock_tcp = NULL;
         WSACleanup();

         return -1;
      }
   }

   return 0;
}


void CloseWinsock()
{
   if ( sock_tcp )
   {
      closesocket( sock_tcp );
      sock_tcp = NULL;
   }

   WSACleanup();
}

/// Processes RemoteSpeechCmd messages to generate audio
void process_message( const char * message )
{
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
      std::vector< std::string > tokens;
      const std::string delimiters = "\\/";
     
      vhcl::Tokenize( tts->temp_audio_dir_cereproc, tokens, delimiters );
      
      printf( "Warning, audio cache directory, %s, does not exist. Creating directory...\n", tts->temp_audio_dir_cereproc );
      for (unsigned int i = 0; i < tokens.size(); i++)
      {
         temp += tokens.at( i ) + "\\";
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
	*			cereproc_file_name refers to the path that cereproc needs to write to, relative to the path from where this program is running
	*			player_file_name refers to the path that Unreal or some other renderer will play the file from, i.e. relative to the path where it is running
	*/
   std::string cereproc_file_name = tts->temp_audio_dir_cereproc + file_name;
   std::string player_file_name = tts->temp_audio_dir_player + file_name;

   /// Generate the audio
   std::string xml = tts->tts( utterance.c_str(), cereproc_file_name.c_str(), player_file_name.c_str(), voice_id );

   // Only send out a reply when result is not empty, ignore otherwise as a nother voice relay might pick up the request
   if ( xml.compare("") != 0 )
   {
      std::string reply = agent_name;
      reply += " ";
      reply += message_id;
      reply += " OK: <?xml version=\"1.0\" encoding=\"UTF-8\"?>";
      reply += xml;
   
      printf( "REPLY: %s\n", reply.c_str() );

#ifdef _DUMP_COMM_TO_DISK
	  fprintf(_outfile, "%d: %s\n\n",_dumpCounter - 1,reply.c_str());
	  std::string dumpXMLstring = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + xml;
	  fprintf(_outXML, "%d: %s\n\n", _dumpCounter - 1, dumpXMLstring.c_str() );
#endif
      vhmsg::ttu_notify2( "RemoteSpeechReply", reply.c_str() );
   }
}


std::string remove_spaces_and_double_quotes ( const char * c )
{
   std::string s = c;

   while(s.find(" ") != std::string::npos)
   {
      s.replace(s.find(" "), 1, "");
   }
  
   while(s.find("\"") != std::string::npos)
   {
      s.replace(s.find("\""), 1, "");
   }

   return s;
}

/// Process all VHMsg messages
void vhmsg_callback( const char * op, const char * args, void * userData )
{
#ifdef _DUMP_COMM_TO_DISK
	fprintf(_infile,"%d: OP: %s :ARGS: %s \n\n",_dumpCounter++, op,args);
#endif
   printf( "received -- op: %s args: %s\n", op, args );

   if ( strcmp( op, "RemoteSpeechCmd" ) == 0 )
   {
      if ( bServerMode )
      {
         std::string query = args;

         if ( InitWinsock( host_name ) != 0 )
         {
            fprintf( stderr, "\n InitWinsock Failed!\n" );
         }

         int bytesSent = send( sock_tcp, query.c_str(), (int)query.size(), 0 );
         //printf( "SENT %d bytes\n", bytesSent );
         if ( bytesSent < 0 )
         {
            int errnum = WSAGetLastError();
            fprintf( stderr, "socket error: %d\n", errnum );
         }

         char recv_b[ 4096 ];
         int bytesReceived = recv( sock_tcp, recv_b, 4096, 0 );

         //close TCP connection
         CloseWinsock();

         if ( bytesReceived == 0 )
         {
            printf( "Connection closed\n" );
         }
         else if ( bytesReceived < 0 )
         {
            fprintf( stderr, "recv failed: %d\n", WSAGetLastError() );
         }
         else
         {
            recv_b[ bytesReceived ] = '\0';
            //printf("Bytes received: %d\n%s\n", bytesReceived, recv_b);

            //initializing vector value
            char * hit_actor;
            hit_actor = strtok( recv_b, "," );
            hit_actor = strtok( NULL, "," );
         }
      }
	  else
      {
         process_message( args );
      }
   }
   else if ( strcmp( op, "vrAllCall" ) == 0 )
   {
      vhmsg::ttu_notify2( "vrComponent", "tts cerevoicerelay" );
   }
   else if ( strcmp( op, "vrKillComponent" ) == 0 )
   {
      std::string strArgs = remove_spaces_and_double_quotes(args);
      
      if ( _stricmp( strArgs.c_str(), "tts" ) == 0 || _stricmp( strArgs.c_str(), "all" ) == 0 )
      {
         printf( "Kill message received." );
         vhmsg::ttu_notify2( "vrProcEnd", "tts cerevoicerelay" );
#ifdef _DUMP_COMM_TO_DISK
		 fclose(_infile);
		 fclose(_outfile);
		 fclose(_inXML);
		 fclose(_outXML);
#endif
         exit(0);
	  }
   }
}


void register_messages ()
{
	vhmsg::ttu_register( "RemoteSpeechCmd" );
	vhmsg::ttu_register( "vrKillComponent" );
   vhmsg::ttu_register( "vrAllCall" );
}


int main( int argc, char * argv[] )
{
   std::vector<char *> voices;
   bServerMode = true;

   vhmsg::ttu_set_client_callback( vhmsg_callback );
   int err = vhmsg::ttu_open();
   if ( err != vhmsg::TTU_SUCCESS )
   {
      printf( "Unable to connect to message server.\n\nPress any key to exit.\n");
      _getch();
      exit(1);
   }

   register_messages();

   // Parse parameters
   for (int i = 1; i < argc; i += 2)
   {
      if ( argc <= i+1)
      {
         printf( "Missing argument for parameter %s.\n\nPress any key to exit.\n", argv[ i ] );
         _getch();
         exit(1);
      } 
      else if ( strcmp( argv[ i ], "-host" ) == 0 )
      {
         bServerMode = true;
         host_name = argv[ i+1 ];
         printf( "Host parameter found. The server / client mode has currently not been implemented. " 
            "Please restart without host parameter to run as stand-alone application.\n\n" 
            "Press any key to exit.\n");
         _getch();
         exit(1);
      } 
      else if ( strcmp( argv[ i ], "-voice" ) == 0 )
      {
         printf( "Voice paramater: %s\n", argv[ i+1 ] );
         bServerMode = false;     
         voices.push_back( argv[ i+1 ] );
      } 
      else 
      {
         printf( "Unknown argument '%s'. \n\n"
            "Known arguments:\n"
            "-host <server_name>, for running CereVoiceRelay in client mode,\n   connecting to <server_name>\n"
            "-voice <voice_id>, for loading voice <voice_id>;\n   multiple '-voice <voice_id>' pairs can be given.\n\n"
            "Press any key to exit.\n", argv[ i ] );
         _getch();
         exit(1);
      }
   }

   // Check if we have any voices defined
   if ( voices.size() < 1 )
   {
      printf( "No voices have been defined. Please specify one or more voices \n"
         "using one or more '-voice <voice_id>' parameters.\n\nPress any key to exit.\n" );
      _getch();
      exit(1);
   }

   // Initialize cerevoice if running as stand-alone application
   if ( !bServerMode )
   {
      tts = new cerevoice_tts();
      tts->init( voices );

      // Notify that we're online
      vhmsg::ttu_notify2( "vrComponent", "tts cerevoicerelay" );
   }

   // Main loop
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

