/*
    This file is part of VHMsg written by Edward Fast at 
    University of Southern California's Institute for Creative Technologies.
    http://www.ict.usc.edu
    Copyright 2008 Edward Fast, University of Southern California

    VHMsg is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VHMsg is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with VHMsg.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "vhcl.h"

#include "elvish.h"

#ifdef WIN32_BUILD
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <string>

#include "vhmsg-tt.h"


// choose to hide/show debug messages
#ifdef DEBUG
//#define Log  OutputDebug
#define Log
#else
#define Log
#endif


#define ELVISH_MAINLOOP_DELAY       50   // In ms
#define ELVISH_VERSION     "2.2"


using std::string;


static void elvish_callback( const char * op, const char * args, void * user_data );


// Global Variables
bool elvish_do_loop;
Tcl_Interp * GlobalInterp;


int Elvish_Init( Tcl_Interp * interp )
{
   elvish_do_loop = false;

   // we need at least TCL 8.1
   if ( Tcl_InitStubs( interp, "8.1", 0 ) == NULL )
   {
      return TCL_ERROR;
   }

   GlobalInterp = interp;

   vhmsg::ttu_set_client_callback( elvish_callback );

   Tcl_CreateCommand( interp, "ttOpen",     elvish_ttOpen,      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
   Tcl_CreateCommand( interp, "ttClose",    elvish_ttClose,     (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL ); 
   Tcl_CreateCommand( interp, "ttRegister", elvish_ttRegister,  (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
   Tcl_CreateCommand( interp, "ttNotice",   elvish_ttNotice,    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
   Tcl_CreateCommand( interp, "ttNoticeNQ", elvish_ttNoticeNQ,  (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
   Tcl_CreateCommand( interp, "ttVersion",  elvish_ttVersion,   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
   Tcl_CreateCommand( interp, "elvish_version", elvish_version, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

   Tcl_PkgProvide( interp, "elvish", ELVISH_VERSION );
   return TCL_OK;
}


int elvish_ttOpen( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   if ( argc < 2 )
   {
      Tcl_AppendResult( interp, "Wrong # of args:  Should be \"", argv[ 0 ], " notices [<host>] [<scope>]\"", (char *)NULL );
      return TCL_ERROR;
   }

#ifdef DEBUG_BUILD
   if ( strcmp( argv[ 1 ], "notices" ) != 0 )
   {
      Log( "Warning: In ttOpen first arg %s is not notices", argv[ 1 ] );
   }
#endif  // DEBUG

   string my_host;
   string elvish_scope;

   if ( argc >= 3 )
   {
      my_host = argv[ 2 ];
   }

   if ( argc >= 4 )
   {
      elvish_scope = argv[ 3 ];

#ifdef DEBUG_BUILD
      Log( "Setting scope to %s", elvish_scope.c_str() );
#endif
   }

   int ret = vhmsg::ttu_open( my_host.c_str(), elvish_scope.c_str() );
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "Error trying to open connection! - '%s'", my_host.c_str() );
   }

   return TCL_OK;
}


int elvish_ttClose( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   elvish_do_loop = false;

   int ret;
   ret = vhmsg::ttu_close();
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "Error closing connection!" );
   }

   return TCL_OK;
}


// elvish_ttRegister
// * Single argument version.  Called from TCL as ttRegiser handle <tScript command>
// * Will set up a subscription as "register(<tScript command>)"
int elvish_ttRegister( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   string subscription_string;  // used to form an elvin subscription string

   // Note we assume that handle and observe are one and the same
   if ( argc != 3 )
   {
      Tcl_AppendResult( interp, "Wrong # of args:  Should be \"", argv[ 0 ], " <observe/handle> <Tscript command>\"", (char *)NULL );
      return TCL_ERROR;
   }

#ifdef DEBUG_BUILD
   if ( ( strcmp( argv[ 1 ], "observe" ) != 0 ) && ( strcmp( argv[ 1 ], "handle" ) != 0 ) )
   {
      Log( "Warning: First argument %s not observe/handle", argv[ 1 ] );
   }
#endif

   // If we haven't started up the elvish mainloop by now, do so
   if ( !elvish_do_loop )
   {
#ifdef DEBUG_BUILD
      Log( "Starting elvish mainloop" );
#endif

      elvish_do_loop = true;
      Tcl_DoWhenIdle( elvish_mainloop, NULL );
   }

   int ret;
   ret = vhmsg::ttu_register( argv[ 2 ] );
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "Error trying to register for message! - '%s'", argv[ 2 ] );
   }

   return TCL_OK;
}


// elvish_ttNotice
// * This is the standard commish-compatable version of ttNotice and 
// * is called from TCL with ttNotice <tScript> ["<arg list>"]
// * Please note, when calling ttNotice you must pack all the arguments
// * with some sort of quotes in TCL before passing it to this function
int elvish_ttNotice( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   int i;
   string notif_string;

   // Make sure that there exists something to send
   if ( argc < 2 )
   {
      Tcl_AppendResult( interp, "Wrong # of args:  Should be \"", argv[ 0 ], " <Tscript command> [<Arg>] [<Arg>]...\"", (char *)NULL );
      return TCL_ERROR;
   }

   // Okay, start adding the subscription fields
   if ( argv[ 2 ] )
   {
      // put stuff into the string
      i = 2;
      while ( argc > i )
      {
         notif_string += "\"";
         notif_string += argv[ i ];
         notif_string += "\" ";
         i++;
      }
   }

   int ret;
   ret = vhmsg::ttu_notify2( argv[ 1 ], notif_string.c_str() );
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "ttNotice failed! - '%s %s'", argv[ 1 ], notif_string.c_str() );
   }

   return TCL_OK;
}


// elvish_ttNoticeNQ
// * This is the alternate version of ttNotice that doesn't surround the parameters with quotes
int elvish_ttNoticeNQ( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   string notif_string;

   // Make sure that there exists something to send
   if ( argc < 2 )
   {
      Tcl_AppendResult( interp, "Wrong # of args:  Should be \"", argv[ 0 ], " <Tscript command> [<Arg>] [<Arg>]...\"", (char *)NULL );
      return TCL_ERROR;
   }

   string message;
   int i = 1;
   while ( argc > i )
   {
      if ( i != 1 )
         message += " ";

      message += argv[ i ];
      i++;
   }

   int ret = vhmsg::ttu_notify1( message.c_str() );
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "ttNotice failed! - '%s'", message.c_str() );
   }

   return TCL_OK;
}


// elvish_ttVersion
// * Sends out a message with the version of the given component.  This is based on the given .ini file.
// * If file_name is NULL, it uses version.ini in the executable's directory.
int elvish_ttVersion( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   // Make sure that there exists something to send
   if ( argc < 4 )
   {
      Tcl_AppendResult( interp, "Wrong # of args:  Should be \"", argv[ 0 ], " <component> <subcomponent> <type> [<file_name>]\"", (char *)NULL );
      return TCL_ERROR;
   }

   const char * component    = argv[ 1 ];
   const char * subcomponent = argv[ 2 ];
   const char * type         = argv[ 3 ];
   const char * fileName     = NULL;

   if ( argc > 4 )
   {
      fileName = argv[ 4 ];
   }

   int ret = vhmsg::ttu_report_version( component, subcomponent, type, fileName );
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "ttVersion failed - '%s %s %s %s'", component, subcomponent, type, fileName ? fileName : "" );
   }

   return TCL_OK;
}


int elvish_version( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   Tcl_AppendResult( interp, "Elvish Version ", ELVISH_VERSION, (char *)NULL );

   return TCL_OK;
}


// elvish_mainloop
// * Single argument mainloop.  This function is not called directly by TCL, but
// * is designed to set itself up to be executed directly by TCL, but will
// * set itself to run itself while TCL is idle as long as
// * elvish_do_loop = 1.  mainloop will queue itself to be run after however long
// * ELVISH_MAINLOOP_DELAY is.  If any messages are in the queue, it will pop these
// * messages off until there are none remaining and call TCL with the command
// * <name> <value> for each message
static void elvish_mainloop( ClientData ptr )
{
   // if not supposed to be running
   if ( !elvish_do_loop )
   {
      return;
   }

   int ret;
   ret = vhmsg::ttu_poll();
   if ( ret != vhmsg::TTU_SUCCESS )
   {
      Log( "ttu_poll() failed!" );
   }

   // Run the handler again in whatever is defined as MAINLOOP DELAY
   Tcl_CreateTimerHandler( ELVISH_MAINLOOP_DELAY, elvish_mainloop, ptr );
}


static void elvish_callback( const char * op, const char * args, void * user_data )
{
   string obj_string;
   string args_string = args;

   // AH 10/21/05: added special case for vrSpeak in order to log the XML part correctly:
   // embrace arguments in curly brackets so TCL won't interpret it.
   // Size of temp_string is static (1024), might cause problems in the future
   // EDF - using strings instead of static char arrays
   // AH 10/17/06: added vrExpress to special case
   if ( (strcmp( op, "vrSpeak" ) == 0) || (strcmp( op, "vrExpress" ) == 0))
   {
      string temp_string;
      temp_string  = "{";
      temp_string += args;
      temp_string += "}";
      args_string = temp_string;
   }

   obj_string  = op;
   obj_string += " ";
   obj_string += args_string;

#ifdef DEBUG_BUILD
   //Log( "Attempt: '%s' - %i", obj_string.c_str(), obj_string.length() );
#endif

   Tcl_Eval( GlobalInterp, obj_string.c_str() );
}
