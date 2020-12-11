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

#ifndef _MISC_COMMANDS_H
#define _MISC_COMMANDS_H

#include <string>
#include <sbm/sr_arg_buff.h>
#include <sb/SBCommandManager.h>


class SbmPawn;
class SbmCharacter;

int pawn_set_cmd_funcx( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);
int pawn_cmd_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);
int pawn_parse_pawn_command( SbmPawn* pawn, const std::string& cmd, srArgBuffer& args);

int character_cmd_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);
int create_remote_pawn_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);
int character_set_cmd_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);
int character_cmd_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);
int character_parse_character_command( SbmCharacter* character, std::string cmd, srArgBuffer& args, bool all_characters );


int set_world_offset_cmd( SbmPawn* pawn, srArgBuffer& args );


#endif