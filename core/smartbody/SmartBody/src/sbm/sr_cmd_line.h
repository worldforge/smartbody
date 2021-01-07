/*
 *  sr_cmd_line.h - part of SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcus Thiebaux, USC
 *      Ari Shapiro, USC
 */

#ifndef SR_CMD_LINE_H
#define SR_CMD_LINE_H

#include	<sys/types.h>
#include	<cstdio>
#include    <cstdlib>
#include	<cstring>
#include	<iostream>
#include	<list>

////////////////////////////////////////////////////////////////////

class srCmdLine	{
	
	public:
		srCmdLine();
		virtual ~srCmdLine();
		
		int pending_cmd( bool );
		
		char *peek_cmd();
		char *read_cmd();
		
		void test_prompt();
	
	private:
		int realloc_buffer( int len );

		char *cmd_buffer;
		int buffer_len;
		int buffer_use;
		int buffer_pos;
	
		// storing the cmd lines
		unsigned int max_cmdlines;
		std::list<std::string>					*cmds;
		std::list<std::string>::iterator		iter;
};

////////////////////////////////////////////////////////////////////
#endif
