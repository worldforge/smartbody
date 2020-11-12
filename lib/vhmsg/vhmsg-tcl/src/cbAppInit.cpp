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

#include <tcl.h>
#include <tk.h>
#include <stdio.h>

#ifndef WIN32_BUILD
#include <unistd.h>
#endif

#include <string>

#include "elvish.h"


using std::string;


// * The following variable is a special hack that is needed in order for
// * Sun shared libraries to be used for Tcl.
#ifndef WIN32_BUILD
#ifdef __cplusplus
extern "C" {
#endif
   extern int matherr();
#ifdef __cplusplus
}
#endif
   int *tclDummyMathPtr = (int *) matherr;
#endif


// 08-21-03 EDF - win32 doesn't support send(), so use use a replacement.
//                Also, define a replacement send() function to use in its place.
#ifdef WIN32_BUILD
   #define USE_SEND_REPLACEMENT  1
   #define DDE_METHOD      0
   #define WINSEND_METHOD  0
   #define COMM_METHOD     1
#else
   #define USE_SEND_REPLACEMENT  0
#endif

#if USE_SEND_REPLACEMENT
   int elvish_send( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char* argv[] );
   int elvish_echo( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char* argv[] );
#endif


// *----------------------------------------------------------------------
// *
// * main --
// *
// * This is the main program for the application.
// *
// * Results:
// * None: Tk_Main never returns here, so this procedure never
// * returns either.
// *
// * Side effects:
// * Whatever the application does.
// *
// *----------------------------------------------------------------------
int main( int argc, char ** argv )
{
   vhcl::Memory::EnableDebugFlags();
   vhcl::Crash::EnableExceptionHandling( true );

   Tk_Main( argc, argv, Tcl_AppInit );

   return 0;
}


// *----------------------------------------------------------------------
// *
// * Tcl_AppInit --
// *
// * This procedure performs application-specific initialization.
// * Most applications, especially those that incorporate additional
// * packages, will have their own version of this procedure.
// *
// * Results:
// * Returns a standard Tcl completion code, and leaves an error
// * message in interp->result if an error occurs.
// *
// * Side effects:
// * Depends on the startup script.
// *
// *----------------------------------------------------------------------
int Tcl_AppInit( Tcl_Interp * interp )  // interp - Interpreter for application.
{
   if ( Tcl_Init( interp ) == TCL_ERROR )
   {
      return TCL_ERROR;
   }

   if ( Tk_Init( interp ) == TCL_ERROR )
   {
      return TCL_ERROR;
   }

   if ( Elvish_Init( interp ) == TCL_ERROR )
   {
      return TCL_ERROR;
   }


#if USE_SEND_REPLACEMENT

#if WINSEND_METHOD
   Tcl_Eval( interp, "package require winsend" );

   Tcl_Eval( interp, "tk appname" );

   string temp;
   temp = "winsend appname " + string( Tcl_GetStringResult( interp ) );
   Tcl_Eval( interp, temp.c_str() );
#endif  // WINSEND_METHOD

#if COMM_METHOD
   Tcl_Eval( interp, "package require comm" );

   {
      string self;
      string appName;
      string temp;

      Tcl_Eval( interp, "::comm::comm self" );
      self = Tcl_GetStringResult( interp );

      Tcl_Eval( interp, "tk appname" );
      appName = Tcl_GetStringResult( interp );

      temp = "::comm::comm send " + appName + " set simulator_name " + self;
      Tcl_Eval( interp, temp.c_str() );

      temp = "tk appname " + self;
      Tcl_Eval( interp, temp.c_str() );
   }
#endif  // COMM_METHOD

#if DDE_METHOD
   Tcl_Eval( interp, "package require dde" );

   Tcl_Eval( interp, "tk appname" );

   {
      string temp;
      temp = "dde servername " + string( Tcl_GetStringResult( interp ) );
      Tcl_Eval( interp, temp.c_str() );
   }
#endif  // DDE_METHOD


    Tcl_CreateCommand( interp, "send", elvish_send, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
    Tcl_CreateCommand( interp, "echo", elvish_echo, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

#endif  // USE_SEND_REPLACEMENT


    // * Call the init procedures for included packages.  Each call should
    // * look like this:
    // *
    // * if (Mod_Init(interp) == TCL_ERROR) {
    // *     return TCL_ERROR;
    // * }
    // *
    // * where "Mod" is the name of the module.

    // * Call Tcl_CreateCommand for application-specific commands, if
    // * they weren't already created by the init procedures called above.

    // * Specify a user-specific startup file to invoke if the application
    // * is run interactively.  Typically the startup file is "~/.apprc"
    // * where "app" is the name of the application.  If this line is deleted
    // * then no user-specific startup file will be run under any conditions.

    // tcl_RcFileName = "~/.wishrc";
    return TCL_OK;
}


#if USE_SEND_REPLACEMENT

int elvish_send( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   int i;
   string temp;

   // argv[ 0 ] is "send"

   // 06-21-03 EDF - I think one more set of {} is required around all of the arguments
   //                but it seems to be working now, so I won't disturb the apple cart.

   if ( strcmp( argv[ 1 ], "-async" ) == 0 )
   {
#if WINSEND_METHOD
      temp = "winsend send """;
      temp += argv[ 2 ];
#endif
#if COMM_METHOD
      temp = "::comm::comm send -async """;
      temp += argv[ 2 ];
#endif
#if DDE_METHOD
      temp = "dde execute -async TclEval """;
      temp += argv[ 2 ];
#endif
      for ( i = 3; i < argc; i++ )
      {
         temp += " {";
         temp += argv[ i ];
         temp += "}";
      }
      temp += """";

      Tcl_Eval( interp, temp.c_str() );
   }
   else
   {
#if WINSEND_METHOD
      temp = "winsend send ";
      temp += argv[ 1 ];
#endif
#if COMM_METHOD
      temp = "::comm::comm send ";
      temp += argv[ 1 ];
#endif
#if DDE_METHOD
      temp = "dde eval ";
      temp += argv[ 1 ];
#endif
      for ( i = 2; i < argc; i++ )
      {
         temp += " {";
         temp += argv[ i ];
         temp += "}";
      }

      Tcl_Eval( interp, temp.c_str() );
   }

   return TCL_OK;
}


int elvish_echo( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] )
{
   printf( "elvish echo: %s\n", argv[ 1 ] );
   return TCL_OK;
}

#endif // USE_SEND_REPLACEMENT
