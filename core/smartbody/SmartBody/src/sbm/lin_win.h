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

#ifndef LIN_WIN_PORTABILITY_H
#define LIN_WIN_PORTABILITY_H

#ifdef WIN32
#else

#include <stdio.h>
#include <string.h>

#define _snprintf snprintf

#define _stricmp strcasecmp
#define _strncmp strncasecmp
#define _strdup strdup

#define _getcwd getcwd

//#ifndef abs
//#define abs fabs
//#endif

#endif

int LinWin_strcmp( const char *s1, const char *s2 );
int LinWin_strncmp( const char *s1, const char *s2, int n );

#endif
