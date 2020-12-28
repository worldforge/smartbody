
#include "vhcl.h"

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <map>

using std::string;
using std::map;

#include "vhmsg-tt.h"




int numMessagesReceived = 0;

void tt_client_callback( const char * op, const char * args, void * user_data )
{
   //printf( "received - '%s %s'\n", op, args );
}


int main( int argc, char * argv[] )
{
   int err;

   vhmsg::ttu_set_client_callback( tt_client_callback );
   err = vhmsg::ttu_open();
   if ( err != vhmsg::TTU_SUCCESS )
   {
      printf( "Connection error!\n" );
      return -1;
   }

   printf( "VHMSG_SERVER: %s\n", vhmsg::ttu_get_server() );
   printf( "VHMSG_SCOPE: %s\n", vhmsg::ttu_get_scope() );

   err = vhmsg::ttu_register( "*" );


   std::string s;
   int num = 2000000;

   while ( !_kbhit() )
   {
      err = vhmsg::ttu_poll();
      if( err == vhmsg::TTU_ERROR )
      {
         printf( "ttu_poll ERR\n" );
      }


      s.assign( num, 'a' );
      num += 10;
      printf( "Sending %d - %s\n", num, "" ); //s.c_str() );
      vhmsg::ttu_notify2( "elbench", s.c_str() );
   }


   printf( "Hit any key to exit..." );
   _getch();


   vhmsg::ttu_close();

   return 0;
}
