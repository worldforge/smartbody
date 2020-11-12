/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#ifndef _SBTYPES_H_
#define _SBTYPES_H_

#ifdef WIN32

	#ifdef SB_EXPORTS
		#define SBAPI __declspec(dllexport)
	#else
		#define SBAPI __declspec(dllimport)
	#endif

#else

	#if __GNUC__ >= 4
		#ifdef SB_EXPORTS
			#define SBAPI __attribute__ ((visibility ("default")))
		#else
			#define SBAPI 
		#endif
	#else
		#define SBAPI 
	#endif
#endif

#ifdef __ANDROID__
#define LINK_VHMSG_CLIENT		(1)
#else
#define LINK_VHMSG_CLIENT		(1)
#endif

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #ifndef SB_IPHONE
    #define SB_IPHONE
    #endif
#else
    #ifndef SB_MAC
    #define SB_MAC
    #endif
#endif
#endif


#endif

