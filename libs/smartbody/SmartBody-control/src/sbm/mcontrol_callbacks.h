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

#ifndef MCONTROL_CALLBACKS_H
#define MCONTROL_CALLBACKS_H

#include <sb/SBTypes.h>
#include <sbm/sr_arg_buff.h>
#include <sb/SBCommandManager.h>
#include "CommandContext.h"

namespace SmartBody {
	class SBVHMsgManager;
	class SBBoneBusManager;
	class SBSteerManager;
	class SBSimulationManager;
}

int mcu_sequence_func( srArgBuffer& args, SmartBody::SBCommandManager& cmdMgr, SmartBody::SBSimulationManager& simMgr );
int mcu_sequence_chain_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
int mcu_terrain_func( srArgBuffer& args, SmartBody::SBScene& scene );
int mcu_time_func( srArgBuffer& args, SmartBody::SBScene& scene );
int mcu_time_ival_prof_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );

// panimation commands
int mcu_panim_cmd_func( srArgBuffer& args, SmartBody::SBScene& scene );
int mcu_panim_test_cmd_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
int mcu_motion_player_func(srArgBuffer& args, SmartBody::SBScene& scene );
int mcu_panim_schedule_func(std::string motion1, std::string charName, bool loop, SmartBody::SBCommandManager* cmdMgr);
int mcu_panim_schedule_func(std::string motion1, std::string motion2, std::string characterName, float weight, bool loop, SmartBody::SBCommandManager* cmdMgr);

//int mcu_character_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );  // Old version... See SbmCharacter::character_cmd_func

int mcu_character_breathing( const char* name, srArgBuffer& args, SmartBody::SBScene& scene) ;

int mcu_controller_func( srArgBuffer& args, SmartBody::SBScene& scene );

int mcu_net_func( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager ) ;
int mcu_net_reset( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager );
int mcu_net_check( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager ) ;

int mcu_play_sound_func( srArgBuffer& args, CommandContext commandContext );
int mcu_stop_sound_func( srArgBuffer& args, CommandContext commandContext );

int mcu_uscriptexec_func( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager);

int mcu_commapi_func( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager );

int mcu_vrKillComponent_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBVHMsgManager& vhmsgManager );
SBAPI int mcu_vrAllCall_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager, SmartBody::SBScene& scene );
int mcu_vrQuery_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr);

//perception
int mcu_vrPerception_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBVHMsgManager& vhmsgManager );

// listening feedback
int mcu_vrBCFeedback_func( srArgBuffer& args, SmartBody::SBScene& scene );

//speech
int mcu_vrSpeech_func( srArgBuffer& args, SmartBody::SBScene& scene );

// examine the motion for specific skeleton
int mcu_check_func( srArgBuffer& args, SmartBody::SBScene& scene);

int mcu_vhmsg_connect_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager);
int mcu_vhmsg_disconnect_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager );

int mcu_python_func( srArgBuffer& args, SmartBody::SBScene& scene);
int mcu_pythonscript_func( srArgBuffer& args, SmartBody::SBScene& scene );

int mcu_steer_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBSteerManager& steerManager );
int syncpoint_func( srArgBuffer& args, SmartBody::SBScene& scene);
#ifdef USE_GOOGLE_PROFILER
int startprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
int stopprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
int startheapprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
int stopheapprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
#endif

int mcu_joint_datareceiver_func( srArgBuffer& args, SmartBody::SBScene& scene);
int mcu_vrExpress_func( srArgBuffer& args, SmartBody::SBScene& scene);
int vhmsglog_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager);

int xmlcachedir_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
int xmlcache_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );

void mcu_vhmsg_callback( const char *op, const char *args, void * user_data );
int mcuFestivalRemoteSpeechCmd_func( srArgBuffer& args, SmartBody::SBScene& scene);

int mcu_echo_func( srArgBuffer& args, SmartBody::SBSimulationManager& simMgr);
int sb_main_func( srArgBuffer & args, SmartBody::SBScene& scene);
int sbm_main_func( srArgBuffer & args, CommandContext commandContext);
int sbm_vhmsg_send_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager);
int mcu_triggerevent_func(srArgBuffer& args, SmartBody::SBScene& scene);
int deprecatedMessage( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr  );

//////////////////////////////////////////////////////////////////
#endif
