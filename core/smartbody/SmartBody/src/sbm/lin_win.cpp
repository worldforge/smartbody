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

#include "sbm/lin_win.h"

#ifdef WIN32

#else

#endif

#include <string.h>

/////////////////////////////////////////////////////////////////

#ifdef WIN32

int LinWin_strcmp( const char *s1, const char *s2 ) {
	return( _stricmp( s1, s2 ) );
}
int LinWin_strncmp( const char *s1, const char *s2, int n )	{
	return( _strnicmp( s1, s2, n ) );
}

#else

int LinWin_strcmp( const char *s1, const char *s2 ) {
	return( strcasecmp( s1, s2 ) );
}
int LinWin_strncmp( const char *s1, const char *s2, int n )	{
	return( strncasecmp( s1, s2, n ) );
}

#endif

/////////////////////////////////////////////////////////////////
