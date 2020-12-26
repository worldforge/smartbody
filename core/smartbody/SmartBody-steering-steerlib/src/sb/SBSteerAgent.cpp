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

#include "SBSteerAgent.h"

#include <sb/SBCharacter.h>
#include <sbm/PPRAISteeringAgent.h>

#include <utility>
#include "SBSteerManager.h"

namespace SmartBody {

SBSteerAgent::SBSteerAgent(SBCharacter* sbCharacter, SmartBody::SBSteerManager& steerManager)
: _steerManager(steerManager), _character(sbCharacter)
{
}

SBSteerAgent::~SBSteerAgent() = default;

void SBSteerAgent::evaluate(double dtime)
{
}

SBCharacter* SBSteerAgent::getCharacter()
{
	return _character;
}

void SBSteerAgent::setSteerStateNamePrefix(std::string prefix)
{
	_stateNamePrefix = std::move(prefix);
	auto* character = getCharacter();
	if (character)
		character->statePrefix = _stateNamePrefix;
}

const std::string& SBSteerAgent::getSteerStateNamePrefix()
{
	return _stateNamePrefix;
}

void SBSteerAgent::setSteerType(const std::string& type)
{
	_steerType = type;
	
	auto* character = getCharacter();
	if (!character)
		return;

	

	if (_steerType == "example")
	{
		if (_steerManager.checkExamples(*this))
			character->locomotion_type = character->Example;
		else
			character->locomotion_type = character->Basic;
	}
	if (type == "procedural")
	{
		character->locomotion_type = character->Procedural;
		//FIXME: This should be put in the subclass
		PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(this);
		if (ppraiAgent)
			ppraiAgent->desiredSpeed = 1.6f;
	}
	if (type == "basic")
	{
		character->locomotion_type = character->Basic;
	}
	if (type == "new")
	{
		character->locomotion_type = character->New;
	}
}

const std::string& SBSteerAgent::getSteerType()
{
	return _steerType;
}

}