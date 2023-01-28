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
#ifndef SMARTBODY_MCONTROL_DEBUGGER_H
#define SMARTBODY_MCONTROL_DEBUGGER_H

#include <sb/SBTypes.h>
#include <sbm/sr_arg_buff.h>
#include "sb/SBCommandManager.h"
namespace SmartBody {
class SBVHMsgManager;
void installDebuggerCommand(SBCommandManager& commandManager, SmartBody::SBVHMsgManager& vhMsgManager);
}

int mcu_sbmdebugger_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhMsgManager  );

#endif //SMARTBODY_MCONTROL_DEBUGGER_H
