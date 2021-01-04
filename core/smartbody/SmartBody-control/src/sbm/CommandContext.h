/*
 Copyright (C) 2021 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SMARTBODY_COMMANDCONTEXT_H
#define SMARTBODY_COMMANDCONTEXT_H

namespace SmartBody {
class SBScene;

class SBCommandManager;

class SBVHMsgManager;

class SBBoneBusManager;

class SBSteerManager;

class SBSimulationManager;
}
namespace BML {
class Processor;
}

struct CommandContext {
	SmartBody::SBScene& scene;
	SmartBody::SBCommandManager& commandManager;
	SmartBody::SBSimulationManager& simMgr;
	SmartBody::SBVHMsgManager* sbvhMsgManager;
	SmartBody::SBBoneBusManager* boneBusManager;
	BML::Processor* bmlProcessor;
	SmartBody::SBSteerManager* steerManager;
};
#endif //SMARTBODY_COMMANDCONTEXT_H
