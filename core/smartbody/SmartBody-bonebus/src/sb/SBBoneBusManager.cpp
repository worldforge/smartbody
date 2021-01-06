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

#include "SBBoneBusManager.h"

#include <sb/SBAttribute.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include "SBUtilities.h"
#include <controllers/me_controller_tree_root.hpp>

namespace SmartBody {

SBBoneBusManager::SBBoneBusManager(SBScene& scene) : SmartBody::SBService(scene)
{
	setName("BoneBus");
	_host = "";

	createStringAttribute("host", "", true, "", 10, false, false, false, "Host where the BoneBus data will be sent. If not set, will use 'localhost'");
}

SBBoneBusManager::~SBBoneBusManager() = default;

bonebus::BoneBusClient& SBBoneBusManager::getBoneBus()
{
	return _boneBus;
}

void SBBoneBusManager::setEnable(bool val)
{
	SBService::setEnable(val);

	if (val)
	{
		if (_boneBus.IsOpen())
		{
			SmartBody::util::log("Closing Bone Bus connection.");
			_boneBus.CloseConnection();
		}

		std::string host = this->getStringAttribute("host");
		if (host.empty())
			host = "localhost";
		bool success = _boneBus.OpenConnection(host.c_str());
		if (!success)
		{
			SmartBody::util::log("Could not open Bone Bus connection to %s", host.c_str());
			SmartBody::BoolAttribute* enableAttribute = dynamic_cast<SmartBody::BoolAttribute*>(getAttribute("enable"));
			if (enableAttribute)
				enableAttribute->setValueFast(false);
			return;
		}
		else
		{
			SmartBody::util::log("Connected Bone Bus to %s", host.c_str());
		}

	}
	else
	{
		if (_boneBus.IsOpen())
		{
			SmartBody::util::log("Closing Bone Bus connection.");
			_boneBus.CloseConnection();
		}
	}

	SmartBody::BoolAttribute* enableAttribute = dynamic_cast<SmartBody::BoolAttribute*>(getAttribute("enable"));
	if (enableAttribute)
		enableAttribute->setValueFast(val);
}

void SBBoneBusManager::setHost(const std::string& host)
{
	_host = host;
	SmartBody::StringAttribute* hostAttribute = dynamic_cast<SmartBody::StringAttribute*>(getAttribute("host"));
	if (hostAttribute)
		hostAttribute->setValueFast(host);
}

const std::string& SBBoneBusManager::getHost()
{
	return _host;
}

void SBBoneBusManager::start()
{
}

void SBBoneBusManager::beforeUpdate(double time)
{
	// process commands received over BoneBus protocol
	std::vector<std::string> commands = _boneBus.GetCommand();
	for (auto & command : commands)
	{
		_scene.command( (char *)command.c_str() );
	}
}

void SBBoneBusManager::update(double time)
{
}

void SBBoneBusManager::afterUpdate(double time)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	bool isClosingBoneBus = false;
	for (auto& entry: _entries) {

		auto character = entry.second.character;
		auto bonebusCharacter = entry.second.bonebusCharacter.get();


		SkChannelArray& channels = character->getSkeleton()->channels();
		MeFrameData& frameData = character->ct_tree_p->getLastFrame();

		int i = 0;
		for (int c = character->viseme_channel_start_pos; c < character->viseme_channel_end_pos; c++, i++) {
			SkChannel& chan = channels[c];
			int buffIndex = character->ct_tree_p->toBufferIndex(c);

			if (buffIndex > -1) {
				float value = frameData.buffer()[buffIndex];
				if (true) //value != character->viseme_history_arr[ i ] )
				{

					bonebusCharacter->SetViseme(channels.name(c).c_str(), value, 0);

					character->viseme_history_arr[i] = value;
				}
			}
		}


		if (bonebusCharacter->GetNumErrors() > 3) {
			// connection is bad, remove the bonebus character
			isClosingBoneBus = true;
			SmartBody::util::log("BoneBus cannot connect to server after visemes sent. Removing all characters.");
		}

		if (character->getSkeleton()) {
			NetworkSendSkeleton(bonebusCharacter, character->getSkeleton().get(), &scene->getGeneralParameters());

			const SkJoint* joint = character->get_world_offset_joint();

			const SkJointPos* pos = joint->pos();
			float x = pos->value(SkJointPos::X);
			float y = pos->value(SkJointPos::Y);
			float z = pos->value(SkJointPos::Z);

			SkJoint::RotType rot_type = joint->rot_type();
			if (rot_type != SkJoint::TypeQuat) {
				//strstr << "ERROR: Unsupported world_offset rotation type: " << rot_type << " (Expected TypeQuat, "<<SkJoint::TypeQuat<<")"<<endl;
			}

			// const_cast because the SrQuat does validation (no const version of value())
			const SrQuat& q = ((SkJoint*) joint)->quat()->value();

			bonebusCharacter->SetPosition(x, y, z, scene->getSimulationManager()->getTime());
			bonebusCharacter->SetRotation((float) q.w, (float) q.x, (float) q.y, (float) q.z, scene->getSimulationManager()->getTime());

			if (bonebusCharacter->GetNumErrors() > 3) {
				// connection is bad, remove the bonebus character
				isClosingBoneBus = true;
				SmartBody::util::log("BoneBus cannot connect to server. Removing all characters");
			}
		}
//		else if (!isClosingBoneBus && !character->bonebusCharacter && getBoneBus().IsOpen())
//		{
//			// bonebus was connected after character creation, create it now
//			character->bonebusCharacter = getBoneBus().CreateCharacter( character->getName().c_str(), character->getClassType().c_str(), true );
//		}

	}


	if (isClosingBoneBus)
	{
		for (auto& entry: _entries) {
			auto bonebusCharacter = entry.second.bonebusCharacter.get();
			getBoneBus().DeleteCharacter(bonebusCharacter);
		}
		_entries.clear();

		getBoneBus().CloseConnection();
	}

}

void SBBoneBusManager::stop()
{
}

void SBBoneBusManager::notify(SBSubject* subject)
{

	SBService::notify(subject);

	auto* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (!attribute)
	{
		return;
	}

	const std::string& name = attribute->getName();
	if (name == "enable")
	{
		bool val = getBoolAttribute("enable");
		setEnable(val);
		return;
	}
	else if (name == "host")
	{
		setHost(getStringAttribute("host"));
		return;
	}

}

void SBBoneBusManager::NetworkSendSkeleton( bonebus::BoneBusCharacter * character, SmartBody::SBSkeleton* skeleton, GeneralParamMap * param_map )
{

	if ( character == nullptr )
	{
		return;
	}

	// Send the bone rotation for each joint in the skeleton
	auto & joints  = skeleton->joints();

	character->IncrementTime();
	character->StartSendBoneRotations();

	std::vector<int> otherJoints;

	for ( size_t i = 0; i < joints.size(); i++ )
	{
		auto& j = joints[ i ];
		if (j->getJointType() != SkJoint::TypeJoint)
		{
			if (j->getJointType() == SkJoint::TypeOther)
				otherJoints.emplace_back(i); // collect the 'other' joins
			continue;
		}

		const SrQuat& q = j->quat()->value();

		character->AddBoneRotation( j->extName().c_str(), q.w, q.x, q.y, q.z, _scene.getSimulationManager()->getTime() );

		//printf( "%s %f %f %f %f\n", (const char *)j->name(), q.w, q.x, q.y, q.z );
	}

	character->EndSendBoneRotations();


	character->StartSendBonePositions();

	for (auto& j : joints)
	{
			if (j->getJointType() != SkJoint::TypeJoint)
			continue;

		float posx = j->pos()->value( 0 );
		float posy = j->pos()->value( 1 );
		float posz = j->pos()->value( 2 );
		if ( false )
		{
			posx += j->offset().x;
			posy += j->offset().y;
			posz += j->offset().z;
		}

		//these coordinates are meant to mimic the setpositionbyname coordinates you give to move the character
		//so if you wanted to move a joint on the face in the x direction you'd do whatever you did to move the actor
		//itself further in the x position.
		character->AddBonePosition( j->extName().c_str(), posx, posy, posz, _scene.getSimulationManager()->getTime() );
	}

	character->EndSendBonePositions();

	if (!otherJoints.empty())
	{
		character->StartSendGeneralParameters();
		for (size_t i = 0; i < otherJoints.size(); i++)
		{
			auto& joint = joints[otherJoints[i]];
			character->AddGeneralParameters(i, 1, joint->pos()->value( 0 ), i, _scene.getSimulationManager()->getTime());

		}
		character->EndSendGeneralParameters();
	}
	

/*
	// Passing General Parameters
	character->StartSendGeneralParameters();
<<<<<<< .mine
	for (size_t i = 0; i < joints.size(); i++)
=======
	int numFound = 0;
	for (int i = 0; i < joints.size(); i++)
>>>>>>> .r2317
	{
		SkJoint* j = joints[ i ];
		if (j->getJointType() != SkJoint::TypeOther)
			continue;

		// judge whether it is joint for general parameters, usually should have a prefix as "param"
		string j_name = j->name();
		int name_end_pos = j_name.find_first_of("_");
		string test_prefix = j_name.substr( 0, name_end_pos );
		if( test_prefix == character->m_name )	
		{
			// if is, prepare adding data
			int index = 0;
			GeneralParamMap::iterator pos;
			for(pos = param_map->begin(); pos != param_map->end(); pos++)
			{
				for(int n = 0; n < (int)pos->second->char_names.size(); n++)
				{
					if( character->m_name == pos->second->char_names[n] )
					{
						index ++;
						for(int m = 0 ; m < pos->second->size; m++)
						{
							std::stringstream joint_name;
							joint_name << character->m_name << "_" << index << "_" << ( m + 1 );
							if(_stricmp( j->name().c_str(), joint_name.str().c_str()) == 0)
								character->AddGeneralParameters(index, pos->second->size, j->pos()->value(0), m, time);
						}
					}
				}
			}
		}
	}
	character->EndSendGeneralParameters();
*/
	
}

void SBBoneBusManager::onCharacterCreate(SBCharacter* character) {
	if (_enabled) {
		std::string classType = character->getClassType().empty() ? "<unknown>" : character->getClassType(); // make sure that the class type has some data. Empty string will cause problems with parsing.

		std::unique_ptr<bonebus::BoneBusCharacter> bonebusCharacter(getBoneBus().CreateCharacter( getName().c_str(), classType.c_str(), true ));
		if (bonebusCharacter) {

			if (character->param_map) {
				int index = 0;
				auto pos = character->param_map->begin();
				for ( ; pos != character->param_map->end(); pos++ )
				{
					bonebusCharacter->SetParams( pos->first.c_str(), index );
				}

			}

			_entries.emplace(character->getName(), BoneBusEntry{std::move(bonebusCharacter)});
		}
	}
}

void SBBoneBusManager::onCharacterDelete(SBCharacter* character) {
	auto I = _entries.find(character->getName());
	if (I != _entries.end()) {
		getBoneBus().DeleteCharacter(I->second.bonebusCharacter.get());
		_entries.erase(I);
	}
}



}


