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

#include "vhcl.h"
#include "vhcl_socket.h"

#include "sb/NetRequest.h"
#include "SBDebuggerUtility.h"
#include "SBDebuggerClient.h"

#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include <sb/SBMotion.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBUtilities.h>

#include <sr/sr_camera.h>


#ifndef __native_client__
#include <sb/SBPythonClass.h>
#endif

bool QueryResourcesCB(void* caller, NetRequest* req);

SBDebuggerUtility::SBDebuggerUtility()
{
}

SBDebuggerUtility::~SBDebuggerUtility()
{
}


void SBDebuggerUtility::initScene()
{
	
}

void SBDebuggerUtility::queryResources()
{
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	SmartBody::SBDebuggerClient* client = sbScene->getDebuggerClient();
	if (!client)
		return;

	// get assets
	std::string assetTypes[4] = {"script", "motion", "audio", "mesh"};
	NetRequest::RequestId correspondingIds[4] = {NetRequest::Get_Seq_Asset_Paths, NetRequest::Get_ME_Asset_Paths, NetRequest::Get_Audio_Asset_Paths, NetRequest::Get_Mesh_Asset_Paths};
	for (int i = 0; i < 4; i++)
	{
		std::string command = vhcl::Format("scene.getAssetPaths(\'%s\')", assetTypes[i].c_str());
		client->SendSBMCommand(correspondingIds[i], "string-array", command, QueryResourcesCB, this);
	}

	// get scripts, service, motion
	client->SendSBMCommand(NetRequest::Get_Script_Names, "string-array", "scene.getScriptNames()", QueryResourcesCB, this);
	client->SendSBMCommand(NetRequest::Get_Service_Names, "string-array", "scene.getServiceManager().getServiceNames()", QueryResourcesCB, this);
	client->SendSBMCommand(NetRequest::Get_Motion_Names, "string-array", "scene.getMotionNames()", QueryResourcesCB, this);
	//	client->SendSBMCommand(NetRequest::Get_Skeleton_Names, "string-array", "scene.getSkeletonNames()", QueryResourcesCB, this);
	//	client->SendSBMCommand(NetRequest::Get_BoneMap_Names, "string-array", "scene.getBoneMapNames()", QueryResourcesCB, this);
	//	client->SendSBMCommand(NetRequest::Get_EventHandler_Names, "string-array", "scene.getEventHandlerNames()", QueryResourcesCB, this);   
	client->SendSBMCommand(NetRequest::Get_Scene_Scale, "float", "scene.getScale()", QueryResourcesCB, this);
}

/*
	Init character given serialized skeleton information
*/
void SBDebuggerUtility::initCharacter(const std::string& name, const std::string& skelName)
{
	if (name == "")
	{
		SmartBody::util::log("Character has no name - will not be created.");
		return;
	}
	SmartBody::SBCharacter* sbCharacter = SmartBody::SBScene::getScene()->createCharacter(name, "");
	if (!sbCharacter)
	{
		SmartBody::util::log("Problem creating character %s, will not be created in remote session...", name.c_str());
		return;
	}
	SmartBody::SBSkeleton* sbSkeleton = SmartBody::SBScene::getScene()->getSkeleton(skelName);
	if (!sbSkeleton)
	{
		SmartBody::util::log("Problem creating skeleton %s, character %s will not be created in remote session...", name.c_str(), skelName.c_str());
		return;
	}
	SmartBody::SBSkeleton* copySbSkeleton = new SmartBody::SBSkeleton(sbSkeleton);
	if (!copySbSkeleton)
	{
		SmartBody::util::log("Problem creating copy of skeleton %s, character %s will not be created in remote session...", name.c_str(), skelName.c_str());
		return;
	}
	sbCharacter->setSkeleton(copySbSkeleton);	
}

void SBDebuggerUtility::initCharacterFaceDefinition(const std::string& characterName, const std::string& faceDefName, const std::string& message)
{
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	SmartBody::SBCharacter* sbCharacter = sbScene->getCharacter(characterName);
	if (!sbCharacter)
		return;

	SmartBody::SBFaceDefinition* faceDef = sbScene->getFaceDefinition(faceDefName);
	if (!faceDef)
		return;

	sbCharacter->setFaceDefinition(faceDef);
}

void SBDebuggerUtility::initPawn(const std::string& name)
{
	SmartBody::SBPawn* sbPawn = SmartBody::SBScene::getScene()->createPawn(name);
}

/*
	Run python command block sent from server. Current usages are listed as following:
	- Init blend given serialized blend information
	- Init transition
	- Init face definition
*/
void SBDebuggerUtility::runPythonCommand(const std::string& info)
{
	SmartBody::SBScene::getScene()->run(info.c_str());
}

void SBDebuggerUtility::initSkeleton(const std::string& skFileName, const std::string& info)
{
	SrInput input(info.c_str());
	SmartBody::SBSkeleton* sbSkel = new SmartBody::SBSkeleton();
	sbSkel->loadSk(input);
	sbSkel->skfilename(skFileName.c_str());
	SmartBody::SBScene::getScene()->getAssetManager()->addSkeleton(sbSkel);
}


void SBDebuggerUtility::updateCharacter(const std::string& cName, const std::string& jName, 
										 float& posX, float& posY, float& posZ, 
										 float& rotX, float& rotY, float& rotZ, float& rotW)
{
	SmartBody::SBCharacter* sbCharacter = SmartBody::SBScene::getScene()->getCharacter(cName);
	if (!sbCharacter)
		return;

	SmartBody::SBJoint* sbJoint = sbCharacter->getSkeleton()->getJointByName(jName);
	if (sbJoint)
	{
		sbJoint->pos()->value(0, (float)posX);
		sbJoint->pos()->value(1, (float)posY);
		sbJoint->pos()->value(2, (float)posZ);
		SrQuat q = SrQuat((float)rotW, (float)rotX, (float)rotY, (float)rotZ);
		SrQuat newq = sbJoint->getPrerotation().inverse()*q;
		sbJoint->quat()->value(newq);
	}
}

void SBDebuggerUtility::updatePawn(const std::string& pName, float& posX, float& posY, float& posZ, 
									float& rotX, float& rotY, float& rotZ, float& rotW)
{
	SmartBody::SBPawn* sbPawn  = SmartBody::SBScene::getScene()->getPawn(pName);
	if (!sbPawn)
		return;

	// TODO: const_cast? really?
	SkJoint* skJoint = const_cast<SkJoint*> (sbPawn->get_world_offset_joint());
	if (skJoint)
	{
		skJoint->pos()->value(0, (float)posX);
		skJoint->pos()->value(1, (float)posY);
		skJoint->pos()->value(2, (float)posZ);
		skJoint->quat()->value(SrQuat((float)rotW, (float)rotX, (float)rotY, (float)rotZ));
	}
}

void SBDebuggerUtility::updateCamera(float& eyePosX, float& eyePosY, float& eyePosZ, 
									  float& lookAtPosX, float& lookAtPosY, float& lookAtPosZ, 
									  float& fovY, float& aspect, float& zNear, float zFar)
{
	SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();

	if (!camera || !SmartBody::SBScene::getScene()->IsCameraLocked())
		return;

	camera->setEye(eyePosX, eyePosY, eyePosZ);
	camera->setCenter(lookAtPosX, lookAtPosY, lookAtPosZ);
	camera->setFov(sr_torad(fovY));
	camera->setAspectRatio(aspect);
	camera->setNearPlane(zNear);
	camera->setFarPlane(zFar);
}


bool QueryResourcesCB(void* caller, NetRequest* req)
{
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	std::vector<std::string> args = req->Args();
	switch (req->Rid())
	{
	case NetRequest::Get_Seq_Asset_Paths:
		for (size_t i = 0; i < args.size(); i++)
			sbScene->addAssetPath("script", args[i]);
		break;

	case NetRequest::Get_ME_Asset_Paths:
		for (size_t i = 0; i < args.size(); i++)
			sbScene->addAssetPath("motion", args[i]);
		break;

	case NetRequest::Get_Audio_Asset_Paths:
		for (size_t i = 0; i < args.size(); i++)
			sbScene->addAssetPath("audio", args[i]);
		break;

	case NetRequest::Get_Mesh_Asset_Paths:
		for (size_t i = 0; i < args.size(); i++)
			sbScene->addAssetPath("mesh", args[i]);
		break;

	case NetRequest::Get_Script_Names:
		break;

	case NetRequest::Get_Service_Names:
		break;

	case NetRequest::Get_Motion_Names:
		for (size_t i = 0; i < args.size(); i++)
		{
			SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getAssetManager()->createMotion(args[i]);
		}
		break;

	case NetRequest::Get_Skeleton_Names:
		break;

	case NetRequest::Get_BoneMap_Names:
		break;

	case NetRequest::Get_EventHandler_Names:
		break;

	case NetRequest::Get_Scene_Scale: 
		sbScene->setScale(SmartBody::util::toFloat(req->ArgsAsString()));
		break;
  case NetRequest::Get_Viseme_Names:
    break;
  case NetRequest::Get_AU_Side:
    break;
  case NetRequest::Get_Channel_Names:
    break;
  case NetRequest::Get_Controller_Names:
    break;
  case NetRequest::Get_Character_Height:
    break;
  case NetRequest::Send_Python_Command:
    break;
  case NetRequest::Get_Face_Definition:
    break;
  }
	return true;
}
