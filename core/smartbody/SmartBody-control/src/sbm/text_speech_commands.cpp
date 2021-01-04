/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>
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

#include "text_speech_commands.h"

#include "sb/SBScene.h"
#include "sb/SBCommandManager.h"
#include "sb/SBBoneBusManager.h"
#include "sb/SBSpeechManager.h"
#include "sbm/text_speech.h"

int text_speech_commands::text_speech_func(srArgBuffer& args, SmartBody::SBBoneBusManager* boneBusManager, SmartBody::SBScene& scene) {
	if (args.calc_num_tokens() == 1) {
		int id = args.read_int();
		scene.getSpeechManager()->speech_text()->startSchedule(id);
	} else {
		if (boneBusManager) {
			int msgNumber = args.read_int();
			char* agentName = args.read_token();
			char* text = args.read_remainder_raw();
			boneBusManager->getBoneBus().SendSpeakText(msgNumber, agentName, text);
		}
	}
	return CMD_SUCCESS;
}