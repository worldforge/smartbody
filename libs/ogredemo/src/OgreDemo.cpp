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

#include "OgreDemo.h"
#include "SBAssetHandlerCOLLADA.h"
#include "BasicCharacter.h"
#include <sbm/ODEPhysicsSim.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBCollisionManager.h>


#include <sb/SBSkeleton.h>

#include <sb/SBFaceDefinition.h>

#include <sb/SBCharacter.h>
#include <sb/SBAssetStore.h>
#include <sb/SBAssetManager.h>


#include <sb/SBSteerAgent.h>

#include <sb/SBSimulationManager.h>


#include <sb/SBServiceManager.h>

#include <xercesc/util/PlatformUtils.hpp>
#include <memory>
#include <sb/SBAssetHandlerSk.h>
#include <sb/SBAssetHandlerSkm.h>
#include <sb/SBAssetHandlerAsf.h>
#include <sb/SBAssetHandlerAmc.h>
#include <sb/SBAssetHandlerBvh.h>
#include <sb/SBAssetHandlerSkmb.h>
#include <sbm/CommandContext.h>
#include <sbm/ControlCommands.h>
#include <sbm/BMLDefs.h>

OgreDemo::OgreDemo() :
		OgreBites::ApplicationContext("SmartBodyOgreDemo"),
		mScene([](SmartBody::SBScene& newScene) {
			return SmartBody::SBScene::CoreServices{std::make_unique<SmartBody::SBPhysicsManager>(newScene, std::make_unique<ODEPhysicsSim>()),
													std::make_unique<SmartBody::SBCollisionManager>(newScene, std::make_unique<ODECollisionSpace>())};
		}),
		//mSteerManager(mScene),
		mBmlProcessor(mScene),
		mSceneManager(nullptr) {

	SmartBody::SBScene::setSystemParameter("mediapath", SMARTBODY_DATADIR "/smartbody/data");
	mScene.setMediaPath(SMARTBODY_DATADIR "/smartbody/data");

	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSkm>());
	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSk>());
	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerAsf>());
	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerAmc>());
	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerSkmb>());
	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerBvh>());
	mScene.getAssetStore().addAssetHandler(std::make_unique<SmartBody::SBAssetHandlerCOLLADA>());


	//mScene.getServiceManager()->addService(&mSteerManager);
	mScene.setSpeechBehaviourProvider(&mBmlProcessor);

	CommandContext commandContext{
			mScene,
			*mScene.getCommandManager(),
			*mScene.getSimulationManager(),
			nullptr,
			nullptr,
			mBmlProcessor.getBMLProcessor(),
			nullptr //&mSteerManager
	};
	SmartBody::registerControlCommands(commandContext);


	mScene.loadAssetsFromPath("mesh/ChrBrad");
	mScene.addAssetPath("mesh", "mesh");
	mScene.addAssetPath("motion", "ChrBrad");
//	mScene.addAssetPath("script", "behaviorsets");
//	mScene.addAssetPath("script", "scripts");
	mScene.loadAssets();


}

OgreDemo::~OgreDemo() {
	//mScene.getServiceManager()->removeService(mSteerManager.getName());
}

bool OgreDemo::frameStarted(const Ogre::FrameEvent& evt) {
	pollEvents();
	mScene.getSimulationManager()->stepDt(evt.timeSinceLastFrame);
	mScene.update();
	mOgreSmartBody->frameStarted(evt);
	return true;
}

void OgreDemo::setup() {

	mRoot->initialise(false);
	createWindow(mAppName, 600, 400);

	locateResources();
	initialiseRTShaderSystem();
	loadResources();

	// adds context as listener to process context-level (above the sample level) events
	mRoot->addFrameListener(this);


	auto& window = mWindows[0];
	window.render->removeAllViewports();
	mSceneManager = getRoot()->createSceneManager();
	mSceneManager->setAmbientLight(Ogre::ColourValue{.3f, .3f, .3f, 1.0f});
	auto directionalLight = mSceneManager->createLight();
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	directionalLight->setDiffuseColour(0.7, 0.7, 0.7);
	auto lightNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(directionalLight);


	Ogre::Camera* cam = mSceneManager->createCamera("");
	auto cameraNode = mSceneManager->createSceneNode();
	//cameraNode->setFixedYawAxis(true);
	cameraNode->attachObject(cam);
	//cameraNode->setPosition({0, 0, -5});
	//cameraNode->setDirection({0,0, 1});
//	cameraNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TS_WORLD);

	auto viewPort = window.render->addViewport(cam);
	cam->setAspectRatio((Ogre::Real) viewPort->getActualWidth() / (Ogre::Real) viewPort->getActualHeight());
	cam->setAutoAspectRatio(true);
	cam->setNearClipDistance(0.5);

	mCameraMan = std::make_unique<OgreBites::CameraMan>(cameraNode);   // create a default camera controller
	addInputListener(mCameraMan.get());

//	auto jointNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
//	jointNode->showBoundingBox(true);
//	auto sphereEntity = mSceneManager->createEntity( Ogre::SceneManager::PT_SPHERE);
//	sphereEntity->setMaterial(Ogre::MaterialManager::getSingleton().getDefaultMaterial(false));
//	jointNode->attachObject(sphereEntity);
//	jointNode->setPosition({0,0,0});



	// Initialize shader generator.
	// Must be before resource loading in order to allow parsing extended material attributes.
	if (!initialiseRTShaderSystem()) {
		OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND,
					"Shader Generator Initialization failed - Core shader libs path not found",
					"ApplicationContextBase::createDummyScene");
	}

	mShaderGenerator->addSceneManager(mSceneManager);

	mCameraMan->setStyle(OgreBites::CS_ORBIT);
	mCameraMan->setYawPitchDist(Ogre::Degree(0), Ogre::Degree(25), 10);
	mCameraMan->setTarget(mSceneManager->getRootSceneNode());

	mOgreSmartBody = std::make_unique<OgreSmartBody>(*mSceneManager, *mCameraMan, mScene, mBmlProcessor);
	addInputListener(window.native, &mOgreSmartBody->mInputListener);

}

void OgreDemo::shutdown() {
	mOgreSmartBody.reset();

	mShaderGenerator->removeSceneManager(mSceneManager);
	mWindows[0].render->removeAllViewports();
	destroyRTShaderSystem();

	getRoot()->destroySceneManager(mSceneManager);
}


int main(int argc, char** argv) {
	xercesc::XMLPlatformUtils::Initialize();
	{
		OgreDemo app;
		app.initApp();
		app.getRoot()->startRendering();
		app.closeApp();
		BML::BMLDefs::shutdown();
	}
	xercesc::XMLPlatformUtils::Terminate();
	return 0;
}
