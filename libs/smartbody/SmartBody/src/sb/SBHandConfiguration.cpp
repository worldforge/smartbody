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

#include "SBHandConfiguration.h"

#include <sb/SBMotion.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"


namespace SmartBody {

SBHandConfiguration::SBHandConfiguration(SmartBody::SBScene& scene) : SBSceneOwned(scene)
{
}

SBHandConfiguration::~SBHandConfiguration()
{
	// don't delete any motions here
	_motions.clear();
}

void SBHandConfiguration::addMotion(const std::string& motionName)
{
	// extract motion from scene
	SBMotion* motion = _scene.getAssetManager()->getMotion(motionName);

	if (!motion)
	{
		SmartBody::util::log("Unable to find motion named %s",motionName.c_str());
		return;
	}

	// add motion
	_motions.emplace_back(motion);
}

void SBHandConfiguration::removeMotion(const std::string& motionName)
{
	SBMotion* motion = _scene.getAssetManager()->getMotion(motionName);

	if (!motion)
	{
		SmartBody::util::log("Unable to find motion named %s",motionName.c_str());
		return;
	}

	auto iter = std::find(_motions.begin(), _motions.end(), motion);

	// if found remove
	if (iter != _motions.end())
	{
		_motions.erase(iter);
	}
}

int SBHandConfiguration::getNumMotions()
{
	return _motions.size();
}

void SBHandConfiguration::printMotionNames()
{
	auto iter = _motions.begin();
	for (; iter != _motions.end() ; iter++)
	{
		SBMotion* motion = *iter;
		SmartBody::util::log("Motion name is %s", motion->getName().c_str());
	}
}

SBMotion* SBHandConfiguration::getMotion(int i)
{
	if (i >= _motions.size())
	{
		SmartBody::util::log("i is greater than motion vector size");
		return nullptr;
	}

	return _motions[i];
}

std::vector<std::string> SBHandConfiguration::getMotionNames()
{
	std::vector<std::string> motionNames;
	auto iter = _motions.begin();
	for (; iter != _motions.end() ; iter++)
	{
		SBMotion* motion = *iter;
		motionNames.emplace_back(motion->getName());
	}

	return motionNames;
}



}
