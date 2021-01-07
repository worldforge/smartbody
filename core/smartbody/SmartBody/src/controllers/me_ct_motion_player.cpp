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

MeCtMotionPlayer::MeCtMotionPlayer(SmartBody::SBCharacter& c, const std::string& name, double n)
: MeCtContainer(c, new MeCtMotionPlayer::Context(this)), character(c) {
	motionName = "";
	frameNum = 0;
	isActive = false;

	init(name, n);
}

void MeCtMotionPlayer::init(const std::string& name, double n)
{
	if (motionName == name)
	{
		frameNum = n;
		return;
	}
	//SmartBody::util::log("controller != nullptr");
	if (controller)
	{
		_sub_context->remove_controller(controller.get());
	}
	//SmartBody::util::log("after if (controller != nullptr)");

	SmartBody::SBMotion* motion = getScene()->getAssetManager()->getMotion(name);
	if (!motion)
	{
		SmartBody::util::log("No motion named '%s' found. Motionplayer cannot play it.", name.c_str());
		return;
	}
	motionName = name;
	motion->connect(character.getSkeleton().get());
	auto* mController = new MeCtMotion(character);
	controller.reset(mController);
	//SmartBody::util::log("mController->init");
	mController->init(motion);
	//SmartBody::util::log("after mController->init");
	std::string controllerName;
	controllerName = "motion player for " + motionName;
	controller->setName(controllerName);

	//SmartBody::util::log("_sub_context->add_controller");
	_sub_context->add_controller(controller.get());
	controller_map_updated();
	//SmartBody::util::log("after controller_map_updated");
	this->remap();
	frameNum = n;
}

MeCtMotionPlayer::~MeCtMotionPlayer() = default;

void MeCtMotionPlayer::setFrameNum(double n)
{
	frameNum = n;
}

double MeCtMotionPlayer::getFrameNum() const
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

bool MeCtMotionPlayer::getActive() const
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

	auto* mController = dynamic_cast<MeCtMotion*> (controller.get());
	SkMotion* motion = mController->motion();

	SmartBody::SBRetarget* retarget = nullptr;

		SmartBody::SBScene* scene = getScene();
		auto* sbMotion = dynamic_cast<SmartBody::SBMotion*>(motion);
		if (sbMotion)
			retarget = scene->getRetargetManager()->getRetarget(sbMotion->getMotionSkeletonName(),character.getSkeleton()->getName());


	double deltaT = motion->duration() / double(motion->frames() - 1);
	double time = deltaT * frameNum;
	if (time > motion->duration()) time = motion->duration();
	int lastFrame = int(frameNum);
	motion->apply((float)time, &frame.buffer()[0], &mController->get_context_map(), SkMotion::Linear, &lastFrame, false, retarget);
	return true;
}