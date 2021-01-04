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

void registerControlCommands(CommandContext context) {
	auto& commandManager = context.commandManager;
	commandManager.insert("sb", [=](srArgBuffer& args) { return sb_main_func(args, context.scene); });
	commandManager.insert("sbm", [=](srArgBuffer& args) { return sbm_main_func(args, context); });

	commandManager.insert("echo", [=](srArgBuffer& args) { return mcu_echo_func(args, context.simMgr); });

	commandManager.insert("seq", [=](srArgBuffer& args) { return mcu_sequence_func(args, context.commandManager, context.simMgr); });
	commandManager.insert("seq-chain", mcu_sequence_chain_func);

	commandManager.insert("terrain", [=](srArgBuffer& args) { return mcu_terrain_func(args, context.scene); });
	commandManager.insert("time", [=](srArgBuffer& args) { return mcu_time_func(args, context.scene); });
	commandManager.insert("tip", mcu_time_ival_prof_func);

	commandManager.insert("panim",  [=](srArgBuffer& args) { return mcu_panim_cmd_func(args, context.scene); });
	commandManager.insert("motionplayer", [=](srArgBuffer& args) { return mcu_motion_player_func(args, context.scene); });

	commandManager.insert("ctrl", [=](srArgBuffer& args) { return mcu_controller_func(args, context.scene); });

	commandManager.insert("vrExpress", [=](srArgBuffer& args) { return mcu_vrExpress_func(args, context.scene); });

	commandManager.insert("receiver", [=](srArgBuffer& args) { return mcu_joint_datareceiver_func(args, context.scene); });
	commandManager.insert("RemoteSpeechCmd", [=](srArgBuffer& args) { return mcuFestivalRemoteSpeechCmd_func(args, context.scene); });

	commandManager.insert("check", [=](srArgBuffer& args) { return mcu_check_func(args, context.scene); }); // check matching between .skm and .sk
	commandManager.insert("pythonscript", [=](srArgBuffer& args) { return mcu_pythonscript_func(args, context.scene); });
	commandManager.insert("python", [=](srArgBuffer& args) { return mcu_python_func(args, context.scene); });
	commandManager.insert("p", [=](srArgBuffer& args) { return mcu_python_func(args, context.scene); });
	//TODO: move the optional commands out into their separate libraries. So for example, anything related to the "SBSteerManager" should be handled by code contained close to that class.
	if (context.steerManager) {
		commandManager.insert("steer", [=](srArgBuffer& args) { return mcu_steer_func(args, context.scene, *context.steerManager); });
	}
	commandManager.insert("syncpoint", [=](srArgBuffer& args) { return syncpoint_func(args, context.scene); });
	if (context.sbvhMsgManager) {
		commandManager.insert("send", [=](srArgBuffer& args) { return sbm_vhmsg_send_func(args, *context.sbvhMsgManager); });
		commandManager.insert("vhmsgconnect", [=](srArgBuffer& args) { return mcu_vhmsg_connect_func(args, *context.sbvhMsgManager); });
		commandManager.insert("vhmsgdisconnect", [=](srArgBuffer& args) { return mcu_vhmsg_disconnect_func(args, *context.sbvhMsgManager); });
		commandManager.insert("vhmsglog", [=](srArgBuffer& args) { return vhmsglog_func(args, *context.sbvhMsgManager); });

		commandManager.insert("vrKillComponent", [=](srArgBuffer& args) { return mcu_vrKillComponent_func(args, context.scene, *context.sbvhMsgManager); });
		commandManager.insert("vrAllCall", [=](srArgBuffer& args) { return mcu_vrAllCall_func(args, *context.sbvhMsgManager, context.scene); });
		commandManager.insert("vrPerception", [=](srArgBuffer& args) { return mcu_vrPerception_func(args, context.scene, *context.sbvhMsgManager); });

	}
	if (context.boneBusManager) {
		commandManager.insert("net", [=](srArgBuffer& args) { return mcu_net_func(args, *context.boneBusManager); });
		commandManager.insert("net_reset", [=](srArgBuffer& args) { return mcu_net_reset(args, *context.boneBusManager); });
		commandManager.insert("net_check", [=](srArgBuffer& args) { return mcu_net_check(args, *context.boneBusManager); });
		commandManager.insert("uscriptexec", [=](srArgBuffer& args) { return mcu_uscriptexec_func(args, *context.boneBusManager); });
		commandManager.insert("CommAPI", [=](srArgBuffer& args) { return mcu_commapi_func(args, *context.boneBusManager); });
	}

	commandManager.insert("PlaySound", [=](srArgBuffer& args) { return mcu_play_sound_func(args, context); });
	commandManager.insert("StopSound", [=](srArgBuffer& args) { return mcu_stop_sound_func(args, context); });





	commandManager.insert("vrBCFeedback", [=](srArgBuffer& args) { return mcu_vrBCFeedback_func(args, context.scene); });
	commandManager.insert("vrSpeech", [=](srArgBuffer& args) { return mcu_vrSpeech_func(args, context.scene); });
	commandManager.insert("triggerevent", deprecatedMessage);

	commandManager.insert("pawn", [=](srArgBuffer& args) { return pawn_cmd_func(args, context.scene); });
	commandManager.insert("char", [=](srArgBuffer& args) { return character_cmd_func(args, context.bmlProcessor, context.scene); });

	commandManager.insert("create_remote_pawn", [=](srArgBuffer& args) { return create_remote_pawn_func(args, context.scene); });
	commandManager.insert("pawn", [=](srArgBuffer& args) { return pawn_set_cmd_funcx(args, context.scene); });
	commandManager.insert("character", [=](srArgBuffer& args) { return character_set_cmd_func(args, context.scene); });
	commandManager.insert("char", [=](srArgBuffer& args) { return character_set_cmd_func(args, context.scene); });

	commandManager.insert("text_speech", [=](srArgBuffer& args) { return text_speech_commands::text_speech_func(args, context.boneBusManager, context.scene); });

}
}