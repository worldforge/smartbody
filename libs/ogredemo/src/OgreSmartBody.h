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

#ifndef SMARTBODY_OGRESMARTBODY_H
#define SMARTBODY_OGRESMARTBODY_H

#include <Ogre.h>
#include <sb/SBScene.h>
#include <sb/SBBmlProcessor.h>
#include "OgreSmartBodyListener.h"
#include <Bites/OgreCameraMan.h>
#include <sr/sr_vec.h>

struct ExternalCharacter {
	SrVec pos;
	SrVec velocity;
	float turnRate;
};

class CharacterController;

class OgreSmartBody {
public:
	struct MyInputListener : public OgreBites::InputListener {
		explicit MyInputListener(OgreSmartBody& ogreSmartBody);

		bool keyReleased(const OgreBites::KeyboardEvent& evt) override;

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

		OgreSmartBody& mOgreSmartBody;
		float forwardChange;
		float sidestepChange;
		float turnChange;

	};


	OgreSmartBody(Ogre::SceneManager& sceneManager,
				  OgreBites::CameraMan& cameraMan,
				  SmartBody::SBScene& scene,
				  //SmartBody::SBSteerManager& steerManager,
				  SmartBody::SBBmlProcessor& bmlProcessor
	);

	bool frameStarted(const Ogre::FrameEvent& evt);


	MyInputListener mInputListener;
protected:
	SmartBody::SBBmlProcessor& mBmlProcessor;
	OgreBites::CameraMan& mCameraMan;

	ExternalCharacter mExternalCharacter;

	OgreSmartBodyListener mListener;

	std::unique_ptr<CharacterController> mCharacterController;

};


class CharacterController {
public:
	explicit CharacterController(const ExternalCharacter& externalCharacter,
								 SmartBody::SBCharacter& character,
								 const std::string& prefix);

	void update(float dt);

	void startIdleToWalkState(float angleDiff, float targetSpeed);

	void startLocomotionState(float turningAngle, float targetSpeed);

	void adjustLocomotionBlend(const std::string& blendName, int blendDimension, double x, double y, double z, bool directPlay, bool loop);

	void adjustFacingAngle(float angleDiff);

protected:
	const ExternalCharacter& mExternalCharacter;
	SmartBody::SBCharacter& mCharacter;

	std::string stepStateName;
	std::string locomotionName;
	std::string startingLName;
	std::string startingRName;
	std::string idleTurnName;
	std::string jumpName;

	SrVec lastVelocity;
};


#endif //SMARTBODY_OGRESMARTBODY_H
