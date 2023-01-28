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

#ifndef SMARTBODY_OGREDEMO_H
#define SMARTBODY_OGREDEMO_H

#include <Bites/OgreApplicationContext.h>
#include <Bites/OgreCameraMan.h>

#include <sb/SBScene.h>
#include <sb/SBSteerManager.h>
#include <sb/SBBmlProcessor.h>

#include "OgreSmartBodyListener.h"
#include "OgreSmartBody.h"

class OgreDemo : public OgreBites::ApplicationContext, public OgreBites::InputListener {
public:
	OgreDemo();

	~OgreDemo() override;

	void setup() override;

	void shutdown() override;

	bool frameStarted(const Ogre::FrameEvent& evt) override;

private:

	SmartBody::SBScene mScene;
	//SmartBody::SBSteerManager mSteerManager;
	SmartBody::SBBmlProcessor mBmlProcessor;
	Ogre::SceneManager* mSceneManager;
	std::unique_ptr<OgreBites::CameraMan> mCameraMan;
	std::unique_ptr<OgreSmartBody> mOgreSmartBody;

};

#endif //SMARTBODY_OGREDEMO_H
