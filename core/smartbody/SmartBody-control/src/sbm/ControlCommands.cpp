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
#include "text_speech_commands.h"
#include <sb/SBVHMsgManager.h>

namespace SmartBody {

void registerControlCommands(SmartBody::SBCommandManager& commandManager,
							 SmartBody::SBVHMsgManager* sbvhMsgManager,
							 SmartBody::SBBoneBusManager* boneBusManager,
							 BML::Processor* bmlProcessor,
							 SmartBody::SBSteerManager* steerManager) {
	commandManager.insert("sb", sb_main_func);
	commandManager.insert("sbm", sbm_main_func);

	commandManager.insert("echo", mcu_echo_func);

	commandManager.insert("seq", mcu_sequence_func);
	commandManager.insert("seq-chain", mcu_sequence_chain_func);

	commandManager.insert("terrain", mcu_terrain_func);
	commandManager.insert("time", mcu_time_func);
	commandManager.insert("tip", mcu_time_ival_prof_func);

	commandManager.insert("panim", mcu_panim_cmd_func);
	commandManager.insert("motionplayer", mcu_motion_player_func);

	commandManager.insert("ctrl", mcu_controller_func);

	commandManager.insert("vrExpress", mcu_vrExpress_func);

	commandManager.insert("receiver", mcu_joint_datareceiver_func);
	commandManager.insert("RemoteSpeechCmd", mcuFestivalRemoteSpeechCmd_func);

	commandManager.insert("check", mcu_check_func);        // check matching between .skm and .sk
	commandManager.insert("pythonscript", mcu_pythonscript_func);
	commandManager.insert("python", mcu_python_func);
	commandManager.insert("p", mcu_python_func);
	if (steerManager) {
		commandManager.insert("steer", [=](srArgBuffer& args) { return mcu_steer_func(args, *steerManager); });
	}
	commandManager.insert("syncpoint", syncpoint_func);
	if (sbvhMsgManager) {
		commandManager.insert("send", [=](srArgBuffer& args) { return sbm_vhmsg_send_func(args, *sbvhMsgManager); });
		commandManager.insert("vhmsgconnect", [=](srArgBuffer& args) { return mcu_vhmsg_connect_func(args, *sbvhMsgManager); });
		commandManager.insert("vhmsgdisconnect", [=](srArgBuffer& args) { return mcu_vhmsg_disconnect_func(args, *sbvhMsgManager); });
		commandManager.insert("vhmsglog", [=](srArgBuffer& args) { return vhmsglog_func(args, *sbvhMsgManager); });

		commandManager.insert("vrKillComponent", [=](srArgBuffer& args) { return mcu_vrKillComponent_func(args, *sbvhMsgManager); });
		commandManager.insert("vrAllCall", [=](srArgBuffer& args) { return mcu_vrAllCall_func(args, *sbvhMsgManager); });
		commandManager.insert("vrPerception", [=](srArgBuffer& args) { return mcu_vrPerception_func(args, *sbvhMsgManager); });

	}
	if (boneBusManager) {
		commandManager.insert("net", [=](srArgBuffer& args) { return mcu_net_func(args, *boneBusManager); });
		commandManager.insert("net_reset", [=](srArgBuffer& args) { return mcu_net_reset(args, *boneBusManager); });
		commandManager.insert("net_check", [=](srArgBuffer& args) { return mcu_net_check(args, *boneBusManager); });
		commandManager.insert("uscriptexec", [=](srArgBuffer& args) { return mcu_uscriptexec_func(args, *boneBusManager); });
		commandManager.insert("CommAPI", [=](srArgBuffer& args) { return mcu_commapi_func(args, *boneBusManager); });
	}

	commandManager.insert("PlaySound", [=](srArgBuffer& args) { return mcu_play_sound_func(args, boneBusManager); });
	commandManager.insert("StopSound", [=](srArgBuffer& args) { return mcu_stop_sound_func(args, boneBusManager); });





	commandManager.insert("vrBCFeedback", mcu_vrBCFeedback_func);
	commandManager.insert("vrSpeech", mcu_vrSpeech_func);
	commandManager.insert("triggerevent", deprecatedMessage);

	commandManager.insert("pawn", pawn_cmd_func);
	commandManager.insert("char", [=](srArgBuffer& args) { return character_cmd_func(args, bmlProcessor); });

	commandManager.insert("create_remote_pawn", create_remote_pawn_func);
	commandManager.insert_set_cmd("pawn", pawn_set_cmd_funcx);
	commandManager.insert_set_cmd("character", character_set_cmd_func);
	commandManager.insert_set_cmd("char", character_set_cmd_func);

	commandManager.insert("text_speech", [=](srArgBuffer& args) { return text_speech_commands::text_speech_func(args, boneBusManager); });

}
}