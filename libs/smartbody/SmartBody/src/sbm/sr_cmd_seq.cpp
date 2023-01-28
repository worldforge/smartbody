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

#include "sr_cmd_seq.h"
#include "sr_arg_buff.h"

#include <cstring>

#include <utility>
#include "SBUtilities.h"

#define MAX_CMD_ARGL 8192


//////////////////////////////////////////////////////////////////////////////////

srCmdSeq::srCmdSeq()	{
		
	event_offset = 0.0;
	_valid = true;
	current = 0;
}

srCmdSeq::~srCmdSeq() = default;

void srCmdSeq::print( FILE *fp )	{
	for (auto& event : command_events) {
		fprintf( fp, "%f  %s\n", event.time, event.cmd.c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////////////

int srCmdSeq::write_file( char *seq_file )	{
	FILE *out_fp;

	if( seq_file == nullptr )	{
	    SmartBody::util::log("srCmdSeq::write_file ERR: nullptr filename\n" );
		return( CMD_FAILURE );
	}

	if( ( out_fp = fopen( seq_file, "w" ) ) == nullptr ) {
	    SmartBody::util::log("srCmdSeq::write_file ERR: file '%s' not opened\n", seq_file ); 
		return( CMD_FAILURE );
	}
	
	fprintf( out_fp, "# srCmdSeq::write_file '%s'\n", seq_file );
	fprintf( out_fp, "\n" );
	print( out_fp );
	fclose( out_fp );
	return( CMD_SUCCESS );
}

#if 1
int srCmdSeq::read_file( FILE *seq_fp )	{

	if( seq_fp == nullptr )	{
	    SmartBody::util::log("srCmdSeq::read_file ERR: nullptr fp\n" );
		return( CMD_FAILURE );
	}
	
	char fill_buff[ MAX_CMD_ARGL ] = "";
	char line_buff[ MAX_CMD_ARGL ] = "";
	char ext_line_buff[ MAX_CMD_ARGL * 10 ] = "";
	int line_count = 0;
	float t = 0.0;
	char *cmd_buff = nullptr;
	bool extended_cmd = false; // <t> begin <cmd> \n end
	
	int done = FALSE;
	while( !done )	{

		if( fscanf( seq_fp, "%[ \t\r\n]", fill_buff ) == EOF )	{
			done = TRUE;
		}
		else	{
			if( fscanf( seq_fp, "%[^\n]", line_buff ) == EOF )	{
				done = TRUE;
			}
			else
			if( line_buff[ 0 ] != '#' )	{
				srArgBuffer args( line_buff );
//printf( ">>'%s'<<\n", line_buff );
				
				if( extended_cmd ) {
				
					srArgBuffer cmd_args( line_buff );
					char *tok = cmd_args.read_token();
					
					if( strcmp( tok, "end" ) == 0 )	{
						extended_cmd = false;
//	SmartBody::util::log( "srCmdSeq::read_file EXTENDED: '%s'\n", ext_line_buff );
						
						srArgBuffer ext_args( ext_line_buff );
						int token_count = ext_args.calc_num_tokens();
						if( token_count > 0 )	{

							if( insert( t, ext_line_buff ) == CMD_FAILURE )	{
								SmartBody::util::log("srCmdSeq::read_file ERR: insert FAILED: line # %d\n", line_count + 1 );
								done = TRUE;
							}
						}
						else	{
							SmartBody::util::log("srCmdSeq::read_file ERR: line # %d\n", line_count + 1 );
						}
					}
					else	{
						char *cmd_part = args.read_remainder_raw();
						char *cp_ext_line_buff = _strdup( ext_line_buff );
						// insert single space to minimize buffer usage:
//						sprintf( ext_line_buff, "%s %s", cp_ext_line_buff, cmd_part );
						// retain multi-line:
//						sprintf( ext_line_buff, "%s\n%s", cp_ext_line_buff, cmd_part );
						// restore fill_buff to preserve formatting:
						sprintf( ext_line_buff, "%s%s%s", cp_ext_line_buff, fill_buff, cmd_part );
						free( cp_ext_line_buff );
					}
				}
				else {
					int token_count = args.calc_num_tokens();
					if( token_count > 1 )	{

						t = args.read_float();
						cmd_buff = args.read_remainder_raw();
						srArgBuffer cmd_args( cmd_buff );
						char *tok = cmd_args.read_token();
						
						if( strcmp( tok, "begin" ) == 0 )	{
							extended_cmd = true;
							char *cmd_part = cmd_args.read_remainder_raw();
							sprintf( ext_line_buff, "%s", cmd_part );
						}
						else
						if( insert( t, cmd_buff ) == CMD_FAILURE )	{
							SmartBody::util::log("srCmdSeq::read_file ERR: insert FAILED: line # %d\n", line_count + 1 );
							done = TRUE;
						}
					}
					else	{
						SmartBody::util::log("srCmdSeq::read_file ERR: line # %d\n", line_count + 1 );
					}
				}
			}
		}
		line_count++;
	}

	if( extended_cmd ) {
		SmartBody::util::log("srCmdSeq::read_file ERR: UNMATCHED begin/end\n" );
		return( CMD_FAILURE );
	}
	return( CMD_SUCCESS );
}
#else
int srCmdSeq::read_file( FILE *seq_fp )	{

	if( seq_fp == nullptr )	{
	    SmartBody::util::log("srCmdSeq::read_file ERR: nullptr fp\n" );
		return( CMD_FAILURE );
	}
	char fill_buff[ MAX_CMD_ARGL ] = "";
	char line_buff[ MAX_CMD_ARGL ] = "";
	int line_count = 0;
	int done = FALSE;
	while( !done )	{

		if( fscanf( seq_fp, "%[ \t\n]", fill_buff ) == EOF )	{
			done = TRUE;
		}
		else	{
			if( fscanf( seq_fp, "%[^\n]", line_buff ) == EOF )	{
				done = TRUE;
			}
			else	{
				if( line_buff[ 0 ] != '#' )	{
					
					srArgBuffer args( line_buff );
					int token_count = args.calc_num_tokens();
					if( token_count >= 2 )	{
						
						float t = args.read_float();
						char *cmd = args.read_remainder_raw();
						if( insert( t, cmd ) == CMD_FAILURE )	{
							SmartBody::util::log("srCmdSeq::read_file ERR: insert FAILED: line # %d\n", line_count + 1 );
							done = TRUE;
						}
					}
					else	{
						SmartBody::util::log("srCmdSeq::read_file ERR: line # %d\n", line_count + 1 );
					}
				}
			}
		}
		line_count++;
	}
	return( CMD_SUCCESS );
}
#endif

int srCmdSeq::read_file( char *seq_file, int report_open_fail )	{
	FILE *in_fp;
	
	if( seq_file == nullptr )	{
	    SmartBody::util::log("srCmdSeq::read_file ERR: nullptr filename\n" );
		return( CMD_FAILURE );
	}
	
	if( ( in_fp = fopen( seq_file, "r" ) ) == nullptr ) {
		if( report_open_fail )	{
		    SmartBody::util::log("srCmdSeq::read_file ERR: file '%s' not found\n", seq_file ); 
		}
		return( CMD_FAILURE );
	}

	int err = read_file( in_fp );
	if( err != CMD_SUCCESS )	{
	    SmartBody::util::log("srCmdSeq::read_file ERR: '%s' FAILED\n", seq_file ); 
		return( err );
	}
	fclose( in_fp );
	return( CMD_SUCCESS );
}

//////////////////////////////////////////////////////////////////////////////////

int srCmdSeq::insert( float time, std::string cmd )	{
	return insert(command_event{time, std::move(cmd)});
}

std::string srCmdSeq::pop( float time )	{
	if (!command_events.empty()) {
		float offset_time = time - event_offset;
		if (command_events.front().time <= offset_time) {
			auto first = command_events.front().cmd;
			command_events.pop_front();
			return first;
		}
	}
	return( "" );
}

//////////////////////////////////////////////////////////////////////////////////

float srCmdSeq::duration() {
	if (!command_events.empty()) {
		return command_events.back().time;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

//srCmdSeq::command_event* srCmdSeq::next( )	{
//	if (current + 1 <)
//
//	if( iterator )	{
//		iterator = iterator->next;
//		if( iterator )	{
//			if( t )	{
//				*t = iterator->time;
//			}
//			return( iterator->cmd );
//		}
//	}
//	return( nullptr );
//}

//////////////////////////////////////////////////////////////////////////////////

int srCmdSeq::insert( command_event event )	{

	for (auto I = command_events.begin(); I != command_events.end(); ++I) {
		if (I->time > event.time) {
			command_events.insert(I, std::move(event));
			return CMD_SUCCESS;
		}
	}
	command_events.emplace_back(std::move(event));
	return( CMD_SUCCESS );
}

srCmdSeq::command_event srCmdSeq::remove()	{
	if (command_events.empty()) {
		return {};
	}
	auto front = std::move(command_events.front());
	command_events.pop_front();
	return front;
}

//////////////////////////////////////////////////////////////////////////////////
