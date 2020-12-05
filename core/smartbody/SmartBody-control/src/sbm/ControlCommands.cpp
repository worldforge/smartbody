/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>

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

#include "ControlCommands.h"

#include "mcontrol_callbacks.h"
#include "MiscCommands.h"

namespace SmartBody {

void registerControlCommands(SmartBody::SBCommandManager& commandManager) {
	commandManager.insert("sb", sb_main_func);
	commandManager.insert("sbm", sbm_main_func);

	commandManager.insert("echo", mcu_echo_func);

	commandManager.insert("seq", mcu_sequence_func);
	commandManager.insert("seq-chain", mcu_sequence_chain_func);
	commandManager.insert("send", sbm_vhmsg_send_func);

	commandManager.insert("terrain", mcu_terrain_func);
	commandManager.insert("time", mcu_time_func);
	commandManager.insert("tip", mcu_time_ival_prof_func);

	commandManager.insert("panim", mcu_panim_cmd_func);
	commandManager.insert("motionplayer", mcu_motion_player_func);

	commandManager.insert("ctrl", mcu_controller_func);

	commandManager.insert("vrExpress", mcu_vrExpress_func);

	commandManager.insert("receiver", mcu_joint_datareceiver_func);
	commandManager.insert("net_reset", mcu_net_reset);
	commandManager.insert("net_check", mcu_net_check);
	commandManager.insert("RemoteSpeechCmd", mcuFestivalRemoteSpeechCmd_func);

	commandManager.insert("check", mcu_check_func);        // check matching between .skm and .sk
	commandManager.insert("pythonscript", mcu_pythonscript_func);
	commandManager.insert("python", mcu_python_func);
	commandManager.insert("p", mcu_python_func);
	commandManager.insert("steer", mcu_steer_func);
	commandManager.insert("syncpoint", syncpoint_func);
	commandManager.insert("vhmsgconnect", mcu_vhmsg_connect_func);
	commandManager.insert("vhmsgdisconnect", mcu_vhmsg_disconnect_func);
	commandManager.insert("vhmsglog", vhmsglog_func);
	commandManager.insert("net", mcu_net_func);

	commandManager.insert("PlaySound", mcu_play_sound_func);
	commandManager.insert("StopSound", mcu_stop_sound_func);

	commandManager.insert("uscriptexec", mcu_uscriptexec_func);

	commandManager.insert("CommAPI", mcu_commapi_func);

	commandManager.insert("vrKillComponent", mcu_vrKillComponent_func);
	commandManager.insert("vrAllCall", mcu_vrAllCall_func);
	commandManager.insert("vrPerception", mcu_vrPerception_func);
	commandManager.insert("vrBCFeedback", mcu_vrBCFeedback_func);
	commandManager.insert("vrSpeech", mcu_vrSpeech_func);
	commandManager.insert("triggerevent", deprecatedMessage);

	commandManager.insert("pawn", pawn_cmd_func);
	commandManager.insert("char", character_cmd_func);

	commandManager.insert("create_remote_pawn", create_remote_pawn_func);
	commandManager.insert_set_cmd("pawn", pawn_set_cmd_funcx);
	commandManager.insert_set_cmd("character", character_set_cmd_func);
	commandManager.insert_set_cmd("char", character_set_cmd_func);
}
}