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

#include "OgreSmartBody.h"
#include "BasicCharacter.h"
#include <sb/SBSkeleton.h>

#include <sb/SBFaceDefinition.h>

#include <sb/SBCharacter.h>
#include <sb/SBAssetStore.h>
#include <sb/SBAssetManager.h>
#include <sb/SBSteerManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBSteerAgent.h>
#include <sbm/PPRAISteeringAgent.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBRetargetManager.h>
#include <controllers/me_ct_param_animation.h>

OgreSmartBody::OgreSmartBody(Ogre::SceneManager& sceneManager,
							 OgreBites::CameraMan& cameraMan,
							 SmartBody::SBScene& scene,
							 SmartBody::SBSteerManager& steerManager,
							 SmartBody::SBBmlProcessor& bmlProcessor
)
		:
		mInputListener(*this),
		mBmlProcessor(bmlProcessor),
		mCameraMan(cameraMan),
		mListener(sceneManager, scene) {


	BasicCharacter basicCharacter(scene);

	//print("Adding character into scene");
	// Set up Brad
	std::string characterName = "brad";
	auto brad = scene.createCharacter(characterName, "");
	auto bradSkeleton = scene.createSkeleton("ChrBrad.dae");
	brad->setSkeleton(bradSkeleton);
	// Set position
	auto bradPos = SrVec(0, 0, 0);
	brad->setPosition(bradPos);
	// Set facing direction
	auto bradFacing = SrVec(0, 0, 0);
	brad->setHPR(bradFacing);
	// Set face definition
	brad->setFaceDefinition(&basicCharacter.bradFace);
	// Set standard controller
	brad->createStandardControllers();
	// Deformable mesh
	brad->setVec3Attribute("deformableMeshScale", .01, .01, .01);
	brad->setStringAttribute("deformableMesh", "ChrBrad.dae");

//	// Lip syncing diphone setup
//	brad->setStringAttribute("lipSyncSetName", "default");
//	brad->setBoolAttribute("usePhoneBigram", True);
//	brad->setVoice("remote");


	// setup locomotion
//	mScene.run("BehaviorSetMaleMocapLocomotion.py");
	basicCharacter.retargetBehaviorSetMaleMocapLocomotion(characterName);

	// setup gestures
//	mScene.run("BehaviorSetGestures.py");
	basicCharacter.retargetBehaviorSetGestures(characterName);

	// setup reach
//	mScene.run("BehaviorSetReaching.py");
	basicCharacter.retargetBehaviorSetReaching(characterName);

	auto characterNode = mListener.createCharacter(*brad);
	characterNode->addChild(cameraMan.getCamera());

	// Set up steering
//	print("Setting up steering");
	steerManager.setEnable(false);
	steerManager.removeSteerAgent(characterName);
	auto steerAgent = steerManager.createSteerAgent(characterName);
	steerAgent->setSteerStateNamePrefix("mocap");
	steerAgent->setSteerType("example");
	brad->setBoolAttribute("steering.pathFollowingMode", false);  // disable path following mode so that obstacles will be respected
	steerManager.setEnable(true);
	// Start the simulation
//	print("Starting the simulation");
	scene.getSimulationManager()->start();

	mBmlProcessor.execBML(characterName, "<body posture=\"ChrMarine@Idle01\"/>");
	mBmlProcessor.execBML(characterName, "<saccade mode=\"listen\"/>");
	// bml.execBML(characterName, "<gaze sbm:handle="brad" target="camera"/>");

	scene.getSimulationManager()->resume();
	//mBmlProcessor.execBML(characterName, "<animation name=\"ChrHarmony_Relax001_ArmReachLfHigh\"/>");

	if (false) {
		auto* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);

		ppraiAgent->steerPath.clearPath();
		ppraiAgent->desiredSpeed = 5;
		ppraiAgent->goalList.clear();
		SrVec pos{-3, 0, 0};
		ppraiAgent->goalList.emplace_back(pos);
		//What does steerPath do?
		//ppraiAgent->steerPath.initPath({pos}, 1.0);

		//scene.command("steer move " + characterName + " normal -0.913949 0.0176676 -0.087908");
	}

	mCharacterController = std::make_unique<CharacterController>(mExternalCharacter, *brad);
	//mCharacterController->startLocomotionState();
}

bool OgreSmartBody::frameStarted(const Ogre::FrameEvent& evt) {
	auto forwardDelta = evt.timeSinceLastFrame * mInputListener.forwardChange;
	auto turnRate = evt.timeSinceLastFrame * mInputListener.turnChange;

	mExternalCharacter.velocity.y += forwardDelta;

	mExternalCharacter.turnRate = turnRate * 8000;

	mCharacterController->update(evt.timeSinceLastFrame);
	mListener.updateCharacters();
	return true;
}

void CharacterController::update(float dt) {
	//if (lastVelocity.y != mExternalCharacter.velocity.y) {
	auto speed = mExternalCharacter.velocity.y;

	auto currentState = mCharacter.param_animation_ct->getCurrentStateName();
	//std::cout << currentState << std::endl;
	//std::cout << speed << " " << mExternalCharacter.turnRate << std::endl;
	if (speed == 0) {
		adjustFacingAngle(mExternalCharacter.turnRate);
	} else {
//		if (lastVelocity.y == 0) {
//			startIdleToWalkState(mExternalCharacter.turnRate, speed);
//		} else {
		startLocomotionState(mExternalCharacter.turnRate, speed);
		//}
	}
	lastVelocity = mExternalCharacter.velocity;
	//}
}


void CharacterController::startIdleToWalkState(float angleDiff, float targetSpeed) {
	std::cout << "startIdleToWalkState angleDiff" << angleDiff << " targetSpeed" << targetSpeed << std::endl;
	//		if (mCharacter.steeringConfig == mCharacter.STANDARD)
	{
		/*
		float angleGlobal = radToDeg(atan2(forward.x, forward.z));
		normalizeAngle(angleGlobal);
		normalizeAngle(yaw);
		float diff = angleGlobal - yaw;
		normalizeAngle(diff);
		*/

		std::stringstream command;
		//double w;
		float maxRotAngle = 180;

		//SmartBody::util::log("angleDiff = %f",angleDiff);
		if (angleDiff > 0) {
			adjustLocomotionBlend(startingLName, 1, -angleDiff, 0, 0, false, false);
		} else {
			adjustLocomotionBlend(startingRName, 1, -angleDiff, 0, 0, false, false);
		}
		//SmartBody::util::log("start turn command = %s",command.str().c_str());

		startLocomotionState(angleDiff, targetSpeed);
		return;
	}
}

void CharacterController::adjustFacingAngle(float angleDiff) {

	bool facingDirectBlend = true;
	std::string playNow;
	if (angleDiff != 0.0 && !mCharacter.param_animation_ct->hasPABlend(idleTurnName)) {
		if (facingDirectBlend)
			adjustLocomotionBlend(idleTurnName, 1, -angleDiff, 0, 0, true, false);
		else
			adjustLocomotionBlend(idleTurnName, 1, -angleDiff, 0, 0, false, false);
	}

	/*
	else
	{
		facingAngle = -200;
	}
	*/
}


void CharacterController::startLocomotionState(float turningAngle, float targetSpeed) {

	auto* stateManager = mCharacter._scene.getBlendManager();
	auto* state = stateManager->getBlend(locomotionName);
	if (!state) {
		SmartBody::util::log("No state named %s found for character %s. Cannot start locomotion.", locomotionName.c_str(), mCharacter.getName().c_str());
		return;
	}

	float parameterScale = 1.f;

	SmartBody::SBRetarget* retarget = mCharacter._scene.getRetargetManager()->getRetarget(state->getBlendSkeleton(), mCharacter.getSkeleton()->getName());
	if (retarget) {
		parameterScale = 1.f / retarget->getHeightRatio();
	}

	//SmartBody::util::log("desire speed = %f, parameter scele = %f",targetSpeed, parameterScale);
	adjustLocomotionBlend(locomotionName, 3, targetSpeed * parameterScale, turningAngle, 0, true, true);
}

void CharacterController::adjustLocomotionBlend(const std::string& blendName, int blendDimension, double x, double y, double z, bool directPlay, bool loop) {


	if (!mCharacter.param_animation_ct) {
		SmartBody::util::log("Parameterized animation Not Enabled, cannot update locomotion to (%d, %d, %d)", x, y, z);
		return;
	}

	SmartBody::SBAnimationBlend* blend = mCharacter._scene.getBlendManager()->getBlend(blendName);
	if (!blend) {
		SmartBody::util::log("Blend %s does not exist, cannot update locomotion to (%d, %d, %d)", blendName.c_str(), x, y, z);
		return;
	}

	const std::string& currentStateName = mCharacter.param_animation_ct->getCurrentStateName();

	std::vector<double> weights;
	weights.resize(blend->getNumMotions());
	if (blendDimension == 0) {
	} else if (blendDimension == 1) {
		blend->getWeightsFromParameters(x, weights);
	} else if (blendDimension == 2) {
		blend->getWeightsFromParameters(x, y, weights);
	} else if (blendDimension == 3) {
		blend->getWeightsFromParameters(x, y, z, weights);
	}

	if (currentStateName == blendName) {
		mCharacter.param_animation_ct->updateWeights(weights);
	} else {
		std::cout << "adjustLocomotionBlend currentState: " << currentStateName << " blendName: " << blendName << " blendDimension: " << blendDimension << " x: " << x << " y: " << y << "z: " << z << " loop: " << loop << " directPlay: " << directPlay << std::endl;
		PABlendData::WrapMode wrap = PABlendData::Loop;
		if (!loop)
			wrap = PABlendData::Once;
		PABlendData::ScheduleMode schedule = PABlendData::Now;
		PABlendData::BlendMode blendMode = PABlendData::Overwrite;
		//SmartBody::util::log("blendName = %s, parameter = %f %f %f", blendName.c_str(), x, y, z);
		mCharacter.param_animation_ct->schedule(blend, weights, wrap, schedule, blendMode, "null", 0.0, 0.0, 0.0, -1.0, directPlay);
	}
}

CharacterController::CharacterController(const ExternalCharacter& externalCharacter,
										 SmartBody::SBCharacter& character)
		: mExternalCharacter(externalCharacter), mCharacter(character) {
	std::string prefix = mCharacter.getName();
	if (!mCharacter.statePrefix.empty())
		prefix = mCharacter.statePrefix;
	stepStateName = prefix + "Step";
	locomotionName = prefix + "Locomotion";
	startingLName = prefix + "StartingLeft";
	startingRName = prefix + "StartingRight";
	idleTurnName = prefix + "IdleTurn";
	jumpName = prefix + "Jump";
}


//
//void OgreSmartBodyListener::OnCharacterCreate(const std::string& name, const std::string& objectClass) {
//	if (mSceneManager.hasEntity(name)) {
//		SmartBody::util::log("An entity named %s already exists, ignoring...", name.c_str());
//		return;
//	}
//
//	/*
//	Ogre::Entity* entity = mSceneManager.createEntity(name, objectClass + ".mesh");
//	Ogre::SceneNode* node = mSceneManager.getRootSceneNode()->createChildSceneNode();
//	node->attachObject(entity);
//
//	Ogre::Skeleton* meshSkel = entity->getSkeleton();
//	Ogre::Skeleton::BoneIterator it = meshSkel->getBoneIterator();
//	while ( it.hasMoreElements() )
//	{
//		Ogre::Bone* bone = it.getNext();
//		bone->setManuallyControlled(true);
//	}
//	*/
//	// listen for the attributes
//	SmartBody::SBCharacter* character = mScene.getCharacter(name);
//
//	auto skeleton = mCharacter.getSkeleton();
//
//	auto node = mSceneManager.getRootSceneNode()->createChildSceneNode();
//
//	std::vector<JointEntry> jointEntries;
//
//	for (auto i = 0; i < skeleton->getNumJoints(); ++i) {
//		auto joint = skeleton->getJoint(i);
//		auto jointNode = node->createChildSceneNode();
//		auto sphereEntity = mSceneManager.createEntity(Ogre::SceneManager::PT_SPHERE);
//		sphereEntity->setMaterial(Ogre::MaterialManager::getSingleton().getDefaultMaterial(false));
//		jointNode->attachObject(sphereEntity);
//		jointNode->setPosition({0, 0, 0});
//		jointNode->showBoundingBox(true);
//
//		auto scale = 0.1f / sphereEntity->getBoundingBox().getSize().x;
//		jointNode->setScale({scale, scale, scale});
//
//		jointEntries.emplace_back(JointEntry{*jointNode, *joint});
//	}
//
//
//	SmartBody::SBAttribute* attribute = mCharacter.getAttribute("deformableMesh");
//	attribute->registerObserver(this);
//
//
//}
bool OgreSmartBody::MyInputListener::keyReleased(const OgreBites::KeyboardEvent& evt) {
	if (evt.keysym.sym == OgreBites::SDLK_UP || evt.keysym.sym == OgreBites::SDLK_DOWN) {
		forwardChange = 0;
	} else if (evt.keysym.sym == OgreBites::SDLK_RIGHT || evt.keysym.sym == OgreBites::SDLK_LEFT) {
		turnChange = 0;
	}

	return InputListener::keyReleased(evt);
}

bool OgreSmartBody::MyInputListener::keyPressed(const OgreBites::KeyboardEvent& evt) {

	if (evt.keysym.sym == OgreBites::SDLK_UP) {
		forwardChange = 1;
	} else if (evt.keysym.sym == OgreBites::SDLK_DOWN) {
		forwardChange = -1;
	} else if (evt.keysym.sym == OgreBites::SDLK_RIGHT) {
		turnChange = -1;
	} else if (evt.keysym.sym == OgreBites::SDLK_LEFT) {
		turnChange = 1;
	}
	return InputListener::keyPressed(evt);
}

OgreSmartBody::MyInputListener::MyInputListener(OgreSmartBody
												& ogreSmartBody) :
		mOgreSmartBody(ogreSmartBody), turnChange(0), forwardChange(0), sidestepChange(0) {

}

