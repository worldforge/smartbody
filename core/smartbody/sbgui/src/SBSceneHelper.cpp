/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>
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

#include <sb/SBAnimationTransition.h>
#include <sb/SBScript.h>
#include "SBSceneHelper.h"
#include "sb/SBCharacter.h"
#include "sb/SBController.h"
#include "sb/SBSkeleton.h"
#include "sb/SBService.h"
#include "sb/SBGestureMap.h"
#include "sb/SBJointMap.h"
#include "sb/SBAssetManager.h"
#include "sb/SBAnimationState.h"
#include "sb/SBEvent.h"

#include "sbm/sbm_deformable_mesh.h"

namespace SmartBody {
SmartBody::SBObject* SBSceneHelper::getObjectFromString(SBRenderScene& renderScene, const std::string& value)
{
	SBScene& scene = renderScene.mScene;
	int prefixPos = value.find('/');
	if (prefixPos == std::string::npos)
		return nullptr;

	std::string prefix = value.substr(0, prefixPos);
	std::string suffix = value.substr(prefixPos + 1);

	if (prefix == "character")
	{
		// check for a second level name
		int prefixPos2 = suffix.find("/");
		if (prefixPos2 == std::string::npos)
		{
			SmartBody::SBCharacter* character = scene.getCharacter(suffix);
			return character;
		}

		std::string characterName = suffix.substr(0, prefixPos2);
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (!character)
			return nullptr;

		std::string remainder = suffix.substr(prefixPos2 + 1);

		int prefixPos3 = remainder.find("/");
		if (prefixPos3 != std::string::npos)
		{
			std::string part = remainder.substr(0, prefixPos3);
			std::string rest = remainder.substr(prefixPos3 + 1);
			if (part == "controller")
			{
				SBController* controller = character->getControllerByName(rest);
				return controller;
			}
			if (part == "skeleton")
			{
				SBSkeleton* skeleton = character->getSkeleton();
				if (skeleton->getName() == rest)
					return skeleton;
				else
					return nullptr;
			}

		}
		else if (remainder == "minibrain")
		{
			SmartBody::Nvbg* nvbg = character->getNvbg();
			return nvbg;
		}

		return nullptr;
	}
	else if (prefix == "pawn")
	{
		SmartBody::SBPawn* pawn = scene.getPawn(suffix);
		return pawn;
	}
	else if (prefix == "scene")
	{
		return &scene;
	}
	else if (prefix == "motion")
	{
		SmartBody::SBMotion* motion = scene.getAssetManager()->getMotion(suffix);
		return motion;
	}
	else if (prefix == "skeleton")
	{
		SmartBody::SBSkeleton* skeleton = scene.getAssetManager()->getSkeleton(suffix);
		return skeleton;
	}
	else if (prefix == "service")
	{
		SmartBody::SBService* service = scene.getServiceManager()->getService(suffix);
		return service;
	}
	else if (prefix == "mesh")
	{
		DeformableMesh* mesh = scene.getAssetManager()->getDeformableMesh(suffix);
		return mesh;
	}
//	else if (prefix == "envmap")
//	{
//		SbmTextureManager& texManager = SbmTextureManager::singleton();
//		SbmTexture* texture = texManager.findTexture(SbmTextureManager::TEXTURE_HDR_MAP, suffix.c_str());
//		return texture;
//	}
	else if (prefix == "controller")
	{
		int prefixPos2 = suffix.find("/");
		if (prefixPos2 == std::string::npos)
			return nullptr;
		std::string prefix2 = suffix.substr(0, prefixPos2 - 1);
		std::string suffix2 = suffix.substr(prefixPos2 + 1);
		SmartBody::SBCharacter* character = scene.getCharacter(suffix);
		if (!character)
			return nullptr;
		return character->getControllerByName(suffix2);
	}
	else if (prefix == "jointmap")
	{
		SBJointMap* jointMap = scene.getJointMapManager()->getJointMap(suffix);
		return jointMap;
	}
	else if (prefix == "gesturemap")
	{
		SBGestureMap* gestureMap = scene.getGestureMapManager()->getGestureMap(suffix);
		return gestureMap;
	}
	else if (prefix == "eventhandler")
	{
		SBEventHandler* eventHandler = scene.getEventManager()->getEventHandler(suffix);
		return eventHandler;
	}
	else if (prefix == "blend")
	{
		SBAnimationBlend* blend = scene.getBlendManager()->getBlend(suffix);
		return blend;
	}
	else if (prefix == "transition")
	{
		SBAnimationTransition* transition = scene.getBlendManager()->getTransitionByName(suffix);
		return transition;
	}
	else if (prefix == "script")
	{
		SBScript* script = scene.getScript(suffix);
		return script;
	}
	else if (prefix == "facedefinition")
	{
		SBFaceDefinition* facedef = scene.getFaceDefinition(suffix);
		return facedef;
	}

	//Check if it can be provided by any registered "object provider".
	auto I = _objectProviders.find(prefix);
	if (I != _objectProviders.end()) {
		return I->second(suffix);
	}
	return nullptr;
}
}