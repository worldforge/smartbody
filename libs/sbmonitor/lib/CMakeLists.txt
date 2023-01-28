cmake_minimum_required (VERSION 2.6)
project (sbmonitorlib)


include_directories( "." )
include_directories( "../../../lib/vhcl/include" )
include_directories( "../../../lib/vhmsg/vhmsg-c/include" )


####

set( SBM_DEBUGGER_LIB_SRC

	SBDebuggerCommon.cpp
	SBDebuggerClient.cpp

)

####


add_library(
	sbmonitorlib
	STATIC

	${SBM_DEBUGGER_LIB_SRC}
)
