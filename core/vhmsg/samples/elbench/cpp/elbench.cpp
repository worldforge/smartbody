
#include "vhcl.h"

#if defined(WIN_BUILD)
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

using std::string;
using std::map;

#include "vhmsg-tt.h"


#define TEST_WAIT  0


bool testSpecialCases = false;

int numMessagesReceived = 0;

void tt_client_callback( const char * op, const char * args, void * user_data )
{
   if ( testSpecialCases )
   {
      printf( "received - '%s %s'\n", op, args );
   }
   else
   {
      //printf( "Received Message '%s' - '%s'\n", op, args );
      numMessagesReceived++;

      if ( numMessagesReceived % 2000 == 0 )
      {
         printf( "%d messages received - '%s %s'\n", numMessagesReceived, op, args );
      }
   }
}


void tt_client_callback_map( const char * op, const char * args, const map<string, string> * messageMap, void * userData )
{
   if ( testSpecialCases )
   {
      //printf( "Received Message '%s' - '%s'\n", op, args );
      
      map<string,string>::const_iterator it;
      for ( it = messageMap->begin(); it != messageMap->end(); it++ )
      {
         printf( "received - '%s %s'\n", it->first.c_str(), it->second.c_str() );
      }

      if ( messageMap->size() > 1 )
         printf( "--------\n" );
   }
   else
   {
      numMessagesReceived++;

      if ( numMessagesReceived % 2000 == 0 )
      {
         map<string,string>::const_iterator it;
         for ( it = messageMap->begin(); it != messageMap->end(); it++ )
         {
            printf( "%d messages received - '%s %s'\n", numMessagesReceived, it->first.c_str(), it->second.c_str() );
         }

         if ( messageMap->size() > 1 )
            printf( "--------\n" );
      }
   }
}


void ConnectionTester()
{
   testSpecialCases = true;
   vhmsg::ttu_set_client_callback( tt_client_callback );
   int sendNum = 0;

   while ( 1 )
   {
      if (vhcl::kbhit())
      {
         char c = vhcl::getch();
         if ( c == 'o' )
         {
            int err = vhmsg::ttu_open();
            printf( "ttu_open() - '%d'\n", err );
         }
         else if ( c == 'c' )
         {
            int err = vhmsg::ttu_close();
            printf( "ttu_close() - '%d'\n", err );
         }
         else if ( c == 'r' )
         {
            int err = vhmsg::ttu_register( "vhTest" );
            printf( "ttu_register() - '%d'\n", err );
         }
         else if ( c == 'u' )
         {
            int err = vhmsg::ttu_unregister( "vhTest" );
            printf( "ttu_unregister() - '%d'\n", err );
         }
         else if ( c == 's' )
         {
            int err = vhmsg::ttu_notify1( vhcl::Format( "vhTest Number #%d", sendNum++ ).c_str() );
            printf( "ttu_notify() - '%d'\n", err );
         }
         else if ( c == 'q' )
         {
            exit(0);
         }
      }

      vhmsg::ttu_poll();
      vhcl::Sleep(0.1);
   }
}

int main( int argc, char * argv[] )
{
   bool receiveMode = true;
   bool testMap = false;


   // enable this to run through simple diagnostics.
   // hit keys to test out vhmsg:
#if 0
   printf("'o' - open connection\n");
   printf("'c' - close connection\n");
   printf("'r' - register message\n");
   printf("'u' - unregister message\n");
   printf("'s' - send message\n");
   printf("'q' - quit\n");

   ConnectionTester();
#endif

   if ( argc > 1 )
   {
      // if you pass in an argument of 1, set the mode to 'send mode'
      // if you pass in a 2nd argument of 1, instead of testing speed, we'll test out special case messages (double-quote, braces, etc)
      if ( atoi( argv[ 1 ] ) == 1 )
      {
         receiveMode = false;
      }
   }

   if ( argc > 2 )
   {
      if ( atoi( argv[ 2 ] ) == 1 )
      {
         testSpecialCases = true;
      }
   }

   if ( argc > 3 )
   {
      if ( atoi( argv[ 3 ] ) == 1 )
      {
         printf( "Testing map messages\n" );
         testMap = true;
      }
   }

   int err;

   if ( testMap )
   {
      vhmsg::ttu_set_client_callback_map( tt_client_callback_map );
   }
   else
   {
      vhmsg::ttu_set_client_callback( tt_client_callback );
   }

   err = vhmsg::ttu_open();
   if ( err == vhmsg::TTU_SUCCESS )
   {
      printf( "VHMSG_SERVER: %s\n", vhmsg::ttu_get_server() );
      printf( "VHMSG_SCOPE: %s\n", vhmsg::ttu_get_scope() );

      if ( testMap )
      {
         err = vhmsg::ttu_register( "*" );
      }
      else
      {
         err = vhmsg::ttu_register( "elbench" );
      }

      vhmsg::ttu_report_version( "elbench", "all", "all" );
   }
   else
   {
      printf( "Connection error!\n" );
      return -1;
   }


   const int NUM_MESSAGES = 20000;

   if ( receiveMode )
   {
      printf( "Receive Mode\n" );

      if ( testSpecialCases )
      {
         printf( "Testing special case messages\n" );

#if TEST_WAIT
         while ( 1 )
         {
            err = vhmsg::ttu_wait(5);
            if( err == vhmsg::TTU_ERROR )
            {
               printf( "ttu_wait ERR\n" );
            }
         }
#else
         while ( !vhcl::kbhit() )
         {
            err = vhmsg::ttu_poll();
            if( err == vhmsg::TTU_ERROR )
            {
               printf( "ttu_poll ERR\n" );
            }
         }
#endif
      }
      else
      {
         unsigned int timeBefore = 0;
         unsigned int timeAfter;

#if TEST_WAIT
         while ( 1 )
         {
            err = vhmsg::ttu_wait(5);
            if( err == vhmsg::TTU_ERROR )
            {
               printf( "ttu_poll ERR\n" );
            }

            // we've received our first message
            if ( numMessagesReceived > 0 && timeBefore == 0 )
            {
               timeBefore = timeGetTime();
            }

            if ( numMessagesReceived >= NUM_MESSAGES )
            {
               timeAfter = timeGetTime();

               printf( "Time to receive %d messages: %d\n", NUM_MESSAGES, timeAfter - timeBefore );

               numMessagesReceived = 0;
               timeBefore = 0;
            }
         }
#else
         while ( !vhcl::kbhit() )
         {
            err = vhmsg::ttu_poll();
            if( err == vhmsg::TTU_ERROR )
            {
               printf( "ttu_poll ERR\n" );
            }

            // we've received our first message
            if ( numMessagesReceived > 0 && timeBefore == 0 )
            {
// TODO: linux
#if defined(WIN_BUILD)
               timeBefore = timeGetTime();
#else
               timeBefore = 0;
#endif
            }

            if ( numMessagesReceived >= NUM_MESSAGES )
            {
// TODO: linux
#if defined(WIN_BUILD)
               timeAfter = timeGetTime();
#else
               timeAfter = 0;
#endif

               printf( "Time to receive %d messages: %d\n", NUM_MESSAGES, timeAfter - timeBefore );

               numMessagesReceived = 0;
               timeBefore = 0;
            }
         }
#endif
      }
   }
   else
   {
      printf( "Send Mode\n" );

      if ( testSpecialCases )
      {
         printf( "Testing special case messages\n" );

         string s;

         s = "Test Message: 'Single-Quote'";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: \"Double-Quote\"";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: [Bracket]";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: {Brace}";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: \\Backslash\\";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: /Slash/";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: (Parenthesis)";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: <Angled Parens?>";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: `Dunno what these are called`";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );

         s = "Test Message: \"~!@#$%^&*()_+\" - top row special characters";
         printf( "Sending %s\n", s.c_str() );
         vhmsg::ttu_notify2( "elbench", s.c_str() );
      }
      else
      {
// TODO: linux
#if defined(WIN_BUILD)
         unsigned int timeBefore = timeGetTime();
#else
         unsigned int timeBefore = 0;
#endif

         for ( int i = 0; i < NUM_MESSAGES; i++ )
         {
            char s[ 512 ];
            sprintf( s, "%d Test Message", i );
            vhmsg::ttu_notify2( "elbench", s );

            if ( i % 2000 == 0 )
            {
               printf( "%d messages sent\n", i );
            }
         }

// TODO: linux
#if defined(WIN_BUILD)
         unsigned int timeAfter = timeGetTime();
#else
         unsigned int timeAfter = 0;
#endif

         printf( "Time to send %d messages: %d\n", NUM_MESSAGES, timeAfter - timeBefore );
      }

      printf( "Hit any key to exit..." );
// TODO: linux
#if defined(WIN_BUILD)
      _getch();
#endif
   }


   vhmsg::ttu_close();

   return 0;
}
