/*
 *  sr_cmd_seq.h - part of SmartBody-lib
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
 */

#ifndef SR_COMMAND_SEQUENCE_H
#define SR_COMMAND_SEQUENCE_H


#include <sb/SBTypes.h>
#include <cstdio>
#include "sbm_constants.h"
#include <string>
#include <list>

//////////////////////////////////////////////////////////////////////////////////

class srCmdSeq	{

	struct command_event {
		float time;
		std::string cmd;
	};

	public:
		SBAPI srCmdSeq();
		virtual ~srCmdSeq();

		void	print( FILE *fp = stdout );						/* print events */
		int		get_count()	{ return( command_events.size() ); }
		
		int		write_file( char *seq_file );
		int		read_file( FILE *seq_fp );
		int		read_file( char *seq_file, int report_open_fail = TRUE );

		SBAPI int		insert( float time, std::string cmd);

		/* HACK: Query duration of seq (i.e., time of last command) */
		float   duration();

		/** Query sequence offset */
		float	offset() const { return event_offset; }
		/** Set pop by event.time + offset */
		void	offset( float t ) { event_offset = t; }
		/** remove first event passed */
		std::string pop( float time );

		void	reset() { current = 0; }
	   // srCmdSeq::command_event	*next();	// return pointer, time

		bool	isValid() const { return _valid; }
		void	setValid(bool val) { _valid = val; }
		command_event remove();

	protected:
		int		insert( command_event event );	/* sort by event.time, add after same time */

	private:
		float	event_offset;
		std::list<command_event> command_events;
		size_t current;
		bool	_valid;
};

//////////////////////////////////////////////////////////////////////////////////
#endif
