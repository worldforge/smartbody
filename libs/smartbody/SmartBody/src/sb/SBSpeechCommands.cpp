/*************************************************************
Copyright (C) 2021 Erik Ogenvik <erik@ogenvik.org>

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

#include "SBSpeechCommands.h"
#include "SBCommandManager.h"
#include "SBSpeechManager.h"
#include "SBScene.h"
#include "sbm/remote_speech.h"

namespace SmartBody {

void SBSpeechCommands::registerCommands(SBScene& scene, SBCommandManager& commandManager, SBSpeechManager& speechManager) {
	commandManager.insert("RemoteSpeechReply", [&](srArgBuffer& args) { return remoteSpeechResult_func(args, scene, speechManager); });
	commandManager.insert("RemoteSpeechTimeOut", [&](srArgBuffer& args) { return remoteSpeechTimeOut_func(args, speechManager); });// internally routed message
	commandManager.insert("RemoteSpeechReplyRecieved", [&](srArgBuffer& args) { return remoteSpeechReady_func(args, scene); });// TODO: move to test commands
	commandManager.insert_test_cmd( "rhet", [&](srArgBuffer& args) { return remote_speech_test(args, scene); });


}

}