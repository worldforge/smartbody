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

#include <controllers/me_ct_motion_player.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include <sb/SBCharacter.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>
#include "SBUtilities.h"

std::string MeCtMotionPlayer::Context::CONTEXT_TYPE = "MeCtMotionPlayer::Context";
std::string MeCtMotionPlayer::CONTROLLER_TYPE = "MeCtMotionPlayer";

void MeCtMotionPlayer::Context::child_channels_updated( MeController* child )
{
}

MeCtMotionPlayer::MeCtMotionPlayer(SbmCharacter* c) : MeCtContainer(new MeCtMotionPlayer::Context(this)), character(c)
{
	motionName = "";
	controller = nullptr;
	frameNum = 0;
	isActive = false;
}

MeCtMotionPlayer::~MeCtMotionPlayer()
{
	delete controller;
}

void MeCtMotionPlayer::init(SmartBody::SBPawn* pawn, std::string name, double n)
{
	if (motionName == name)
	{
		frameNum = n;
		return;
	}
	//SmartBody::util::log("controller != nullptr");
	if (controller != nullptr)
	{
		_sub_context->remove_controller(controller);
		//controller->unref();
		delete controller;
	}
	//SmartBody::util::log("after if (controller != nullptr)");
		 
	SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(name);
	if (!motion)
	{
		SmartBody::util::log("No motion named '%s' found. Motionplayer cannot play it.", name.c_str());
		return;
	}
	motionName = name;
	motion->connect(character->getSkeleton().get());
	controller = new MeCtMotion();
	MeCtMotion* mController = dynamic_cast<MeCtMotion*> (controller);
	//SmartBody::util::log("mController->init");
	mController->init(pawn,motion);
	//SmartBody::util::log("after mController->init");
	std::string controllerName;
	controllerName = "motion player for " + motionName;
	controller->setName(controllerName.c_str());
	
	//SmartBody::util::log("_sub_context->add_controller");
	_sub_context->add_controller(controller);
	controller->ref();
	controller_map_updated();
	//SmartBody::util::log("after controller_map_updated");
	this->remap();
	frameNum = n;
}

void MeCtMotionPlayer::setFrameNum(double n)
{
	frameNum = n;
}

double MeCtMotionPlayer::getFrameNum()
{
	return frameNum;
}

void MeCtMotionPlayer::setMotionName(const std::string& name)
{
	motionName = name;
}

const std::string& MeCtMotionPlayer::getMotionName()
{
	return motionName;
}

void MeCtMotionPlayer::setActive(bool a)
{
	isActive = a;
}

bool MeCtMotionPlayer::getActive()
{
	return isActive;
}

void MeCtMotionPlayer::controller_map_updated()
{
	channels.init();
	if (controller == nullptr)
		return;
	channels.merge(controller->controller_channels());
	controller->remap();
}

SkChannelArray& MeCtMotionPlayer::controller_channels()
{
	return channels;
}

double MeCtMotionPlayer::controller_duration()
{
	return -1;
}

bool MeCtMotionPlayer::controller_evaluate(double t, MeFrameData& frame)
{
	if (controller == nullptr)
		return false;

	if (!isActive)
		return false;

	MeCtMotion* mController = dynamic_cast<MeCtMotion*> (controller);
	SkMotion* motion = mController->motion();

	SmartBody::SBRetarget* retarget = nullptr;
	SmartBody::SBCharacter* _character = dynamic_cast<SmartBody::SBCharacter*>(character);
	if (_character)
	{
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		SmartBody::SBMotion* sbMotion = dynamic_cast<SmartBody::SBMotion*>(motion);
		if (sbMotion)
			retarget = scene->getRetargetManager()->getRetarget(sbMotion->getMotionSkeletonName(),_character->getSkeleton()->getName());		
	}

	double deltaT = motion->duration() / double(motion->frames() - 1);
	double time = deltaT * frameNum;
	if (time > motion->duration()) time = motion->duration();
	int lastFrame = int(frameNum);
	motion->apply((float)time, &frame.buffer()[0], &mController->get_context_map(), SkMotion::Linear, &lastFrame, false, retarget);
	return true;
}