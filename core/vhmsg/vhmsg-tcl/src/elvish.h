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

#ifndef ELVISH_H
#define ELVISH_H


#include <tcl.h>


// Elvish_Init
// * Initialization for TCL
int Elvish_Init( Tcl_Interp * interp );

// elvish_ttOpen
// * Allocates, creates, and connects for a new session
// * Inputs: (From TCL) ttOpen <unused_text> <host>
// * Output: TCL_OK or TCL_ERROR and error message depending on internal failures
int elvish_ttOpen( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_ttClose
// * Disconnect from the server and clean up connection
int elvish_ttClose( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_ttRegister
// * Registers interest in a Tscript message 
// * Inputs: (From TCL) ttRegister <handle/observe> <Tscript command>
// * Output: TCL_OK or TCL_ERROR and error message depending on internal failures.
int elvish_ttRegister( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_ttNotice
// * Sends a notification
// * Inputs: (From TCL) ttNotice <name> [<value>]
// * Output: TCL_OK or TCL_ERROR and error message depending on internal failures
int elvish_ttNotice( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_ttNoticeNQ
// * Sends a notification, alternate version that doesn't put quotes around parameters
// * Inputs: (From TCL) ttNoticeNQ <name> [<value>]
// * Output: TCL_OK or TCL_ERROR and error message depending on internal failures
int elvish_ttNoticeNQ( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_ttVersion
// * Sends out a message with the version of the given component.  This is based on the given .ini file.
// * If file_name is NULL, it uses version.ini in the executable's directory.
// * Inputs: (From TCL) ttVersion <component> <subcomponent> <type> [<file_name>]
// * Output: TCL_OK or TCL_ERROR and error message depending on internal failures
int elvish_ttVersion( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_version
// * Prints out the version of elvish
int elvish_version( ClientData notUsed, Tcl_Interp * interp, int argc, CONST char * argv[] );

// elvish_mainloop
// * Mainloop for elvish, requires TK, will run every ELVISH_MAINLOOP_DELAY and
// * will process pending messages
// * Inputs: Internal function
// * Outputs: Will execute TCL functions based upon "<name> <args> as recieved
// * over active subscriptions made with elvish_ttRegister
static void elvish_mainloop( ClientData ptr );


#endif
