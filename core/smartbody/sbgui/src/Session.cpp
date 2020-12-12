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

#include <SBPythonClass.h>
#include "Session.h"

#include "sb/SBAssetHandlerSkm.h"
#include "sb/SBAssetHandlerSk.h"
#include "sb/SBAssetHandlerCOLLADA.h"
#include "sb/SBAssetHandlerAsf.h"
#include "sb/SBAssetHandlerAmc.h"
#include "sb/SBAssetHandlerOgre.h"
#include "sb/SBAssetHandlerObj.h"
#include "sb/SBAssetHandlerPly.h"
#include "sb/SBAssetHandlerSkmb.h"
#include "sb/SBAssetHandlerBvh.h"
#include "sb/SBAssetHandlerHDR.h"
#include "sb/SBAssetHandlerSBMeshBinary.h"

#include "sb/SBServiceManager.h"

#include "sb/SBPhysicsManager.h"
#include "sb/SBCollisionManager.h"

#include "sbm/mcontrol_debugger.h"

#include "sbm/ODEPhysicsSim.h"
#include "sbm/ControlCommands.h"

Session* Session::current = nullptr;

Session::Session()
		: scene(SmartBody::SBScene::CoreServices{std::make_unique<SmartBody::SBPhysicsManager>(std::make_unique<ODEPhysicsSim>()),
												 std::make_unique<SmartBody::SBCollisionManager>(std::make_unique<ODECollisionSpace>())}),
		  renderAssetManager(scene, scene.getAssetStore()),
		  renderScene(scene, renderAssetManager),
		  debuggerServer(renderScene) {
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSkm>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSk>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerCOLLADA>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerAsf>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerAmc>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerOgre>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerObj>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerPly>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSkmb>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerBvh>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerHdr>());
	scene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSBMeshBinary>());

	scene.setVHMsgProvider(&vhmMsgManager);

	scene.getServiceManager()->addService(&vhmMsgManager);

	SmartBody::installDebuggerCommand(*scene.getCommandManager(), vhmMsgManager);
	SmartBody::PythonInterface::renderScene = &renderScene;

	registerControlCommands(*scene.getCommandManager(), &vhmMsgManager);

}

Session::~Session() {
	if (vhmMsgManager.isEnable() && vhmMsgManager.isConnected())
		vhmMsgManager.send( "vrProcEnd sbm" );

	scene.getServiceManager()->removeService(vhmMsgManager.getName());
}