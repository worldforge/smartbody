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

#include "SBFaceDefinition.h"

#include <sk/sk_motion.h>
#include "sbm/action_unit.hpp"
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"
#include <fstream>


namespace SmartBody {

SBFaceDefinition::SBFaceDefinition() : SBObject()
{
	_faceNeutral = nullptr;
}

SBFaceDefinition::SBFaceDefinition(const std::string& name)
{
	_faceNeutral = nullptr;
	_name = name;
	setName(name);
}

SBFaceDefinition::SBFaceDefinition(SBFaceDefinition* source)
{
	if (!source)
	{
		_faceNeutral = nullptr;
		return;
	}
	_faceNeutral = source->getFaceNeutral();
	int numVisemes = source->getNumVisemes();
	for (int v = 0; v < numVisemes; v++)
	{
		std::string visemeName = source->getVisemeName(v);
		SkMotion* motion = source->getVisemeMotion(visemeName);
		_visemeMap.insert(std::pair<std::string, std::pair<SkMotion*, float> >(visemeName, std::pair<SkMotion*, float>(motion, 1.0f)));
	}

	int numAUs = source->getNumAUs();
	for (int a = 0; a < numAUs; a++)
	{
		int auNum = source->getAUNum(a);
		ActionUnit* sourceAu = source->getAU(auNum);
		ActionUnit* au = new ActionUnit(sourceAu);
		_auMap.insert(std::pair<int, ActionUnit*>(auNum, au));
	}

	// copy the default face poses
	std::vector<std::string> defaultPoses = source->getDefaultFacePoses();
	std::vector<float> defaultPoseValues = source->getDefaultFaceValues();
	this->setDefaultFacePose(defaultPoses, defaultPoseValues);
		
}

SBFaceDefinition::~SBFaceDefinition()
{
	for (auto & iter : _visemeMap)
	{
		SkMotion* motion = iter.second.first;
	}


}

void SBFaceDefinition::setFaceNeutral(const std::string& motionName)
{
	SmartBody::SBMotion* motion = nullptr;
	if (motionName.length() > 0)
	{
		motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(motionName);
		if (!motion)
		{
			SmartBody::util::log("ERROR: Unknown facial neutral motion \"%s\".", motionName.c_str());
			return;
		}
	}

	_faceNeutral = motion;

	//SmartBody::util::log("Face neutral motion is now '%s'.", motionName.c_str());
}

SkMotion* SBFaceDefinition::getFaceNeutral()
{
	return _faceNeutral;
}

void SBFaceDefinition::setDefaultFacePose(const std::vector<std::string>& poses, const std::vector<float>& values)
{
	if (poses.size() != values.size())
	{
		SmartBody::util::log("Cannot set default face pose; need the same number of face poses (%d) as values (%d)", poses.size(), values.size());
		return;
	}

	_defaultFacePoses.clear();
	_defaultFaceValues.clear();

	for (unsigned int x = 0; x < poses.size(); x++)
	{
		_defaultFacePoses.emplace_back(poses[x]);
		_defaultFaceValues.emplace_back(values[x]);
	}
}

std::vector<std::string> SBFaceDefinition::getDefaultFacePoses()
{
	std::vector<std::string> poses;
	for (unsigned int x = 0; x < _defaultFacePoses.size(); x++)
		poses.emplace_back(_defaultFacePoses[x]);
	return poses;
}

std::vector<float> SBFaceDefinition::getDefaultFaceValues()
{
	std::vector<float> values;
	for (float & _defaultFaceValue : _defaultFaceValues)
		values.emplace_back(_defaultFaceValue);
	return values;
}

bool SBFaceDefinition::hasAU(int auNum)
{
	auto iter = _auMap.find(auNum);
	if (iter == _auMap.end())
	{
		return false;
	}
	else // au already exists, replace it
	{
		return true;
	}
}

ActionUnit* SBFaceDefinition::getAUByName(const std::string& auName)
{
	int index = auName.find("au_");
	if (index == std::string::npos)
		return nullptr;

	std::string remainder = auName.substr(index + 3);

	int leftIndex = remainder.find("_left");
	if (leftIndex != std::string::npos)
	{
		std::string numStr = remainder.substr(0, leftIndex);
		int num = atoi(numStr.c_str());
		ActionUnit* au = this->getAU(num);
		if (!au)
			return nullptr;
		if (!au->is_left())
			return nullptr;
		else
			return au;
	}

	int rightIndex = remainder.find("_right");
	if (rightIndex != std::string::npos)
	{
		std::string numStr = remainder.substr(0, leftIndex);
		int num = atoi(numStr.c_str());
		ActionUnit* au = this->getAU(num);
		if (!au)
			return nullptr;
		if (!au->is_left())
			return nullptr;
		else
			return au;
	}

	int num = atoi(remainder.c_str());
	ActionUnit* au = this->getAU(num);
	if (au->is_bilateral())
		return au;
	return nullptr;

}

void SBFaceDefinition::setAU(int auNum, const std::string& side, const std::string& motionName)
{
	if (side != "left" &&
		side != "LEFT" &&
		side != "right" &&
		side != "RIGHT" &&
		side != "both" && 
		side != "BOTH" &&
		side != ""
		)
	{
		SmartBody::util::log("Unrecognized side '%s'. Action Unit %d not added.", side.c_str(), auNum);
		return;
	}

	SmartBody::SBMotion*  motion = nullptr;
	if (motionName.length() > 0)
	{
		motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(motionName);
		if (!motion)
		{
			SmartBody::util::log("ERROR: Unknown facial pose \"%s\".", motionName.c_str());
			return;
		}
	}

	ActionUnit* au = getAU(auNum);
	if (!au)
	{
		if (side == "both" || side == "BOTH" || side == "")
		{
			au = new ActionUnit( auNum, motion );
			au->reset_type();
			au->set_bilateral();
		}
		else if (side == "left" || side == "LEFT")
		{
			au = new ActionUnit( auNum, motion, nullptr );
			au->reset_type();
			au->set_left();
		}
		else if (side == "right" || side == "RIGHT")
		{
			au = new ActionUnit( auNum, nullptr, motion );
			au->reset_type();
			au->set_right();
		}

		addAU(auNum, au);
		//SmartBody::util::log("AU '%d' added to face definition %s.", auNum, getName().c_str());
		
	} 
	else
	{
		if (side == "")
		{
			if( au->is_left() || au->is_right() )
			SmartBody::util::log("WARNING: Overwritting au #%d", auNum);
			au->set( motion );
			au->reset_type();
			au->set_bilateral();
		}
		else if (side == "left" || side == "LEFT")
		{				
			if( au->is_left() || au->is_bilateral())
			SmartBody::util::log("WARNING: Overwritting au #%d left", auNum);
			au->set( motion, au->right );
			if (au->is_right())
			{
				au->reset_type();
				au->set_left();
				au->set_right();
			}
			else if (au->is_bilateral())
			{
				au->reset_type();
				au->set_left();
			}
		}
		else if (side == "right" || side == "RIGHT")
		{
			if( au->is_right() )
				SmartBody::util::log("WARNING: Overwritting au #%d right", auNum);
			au->set( au->left, motion );
			if (au->is_left())
			{
				au->reset_type();
				au->set_left();
				au->set_right();
			}
			else if (au->is_bilateral())
			{
				au->reset_type();
				au->set_right();
			}
		}
	}

}

void SBFaceDefinition::addAU(int auNum, ActionUnit* au)
{
	if (!au)
	{
		SmartBody::util::log("No AU given. Cannot add AU '%d'.", auNum);
		return;
	}

	auto iter = _auMap.find(auNum);
	if (iter == _auMap.end())
	{
		_auMap.insert(std::pair<int, ActionUnit*>(auNum, au));
		return;
	}
	else // au already exists, replace it
	{
		_auMap.erase(iter);
		_auMap.insert(std::pair<int, ActionUnit*>(auNum, au));
		SmartBody::util::log("Action unit '%d' replaced with new rules.", auNum);
	}
}

bool SBFaceDefinition::hasViseme(const std::string& visemeName)
{
	auto iter = _visemeMap.find(visemeName);
	if (iter == _visemeMap.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void SBFaceDefinition::setViseme(const std::string& visemeName, const std::string& motionName)
{
	auto iter = _visemeMap.find(visemeName);
	if (iter == _visemeMap.end())
	{
		// no motion given, add the viseme only
		if (motionName == "")
		{
			_visemeMap.insert(std::pair<std::string, std::pair<SkMotion*, float> >(visemeName,std::pair<SkMotion*, float>((SkMotion*)nullptr, 0.0f)));
			return;
		}

		// motion name given, find the associated motion
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(motionName);
		if (!motion)
		{
			SmartBody::util::log("Cannot find viseme named '%s'. Viseme named '%s' not added.", visemeName.c_str(), motionName.c_str());
			return;
		}

		_visemeMap.insert(std::pair<std::string, std::pair<SkMotion*, float> >(visemeName, std::pair<SkMotion*, float>(motion, 1.0f)));
		//SmartBody::util::log("Viseme '%s' added to face definition %s.", visemeName.c_str(), getName().c_str());
		return;
	}
	else // viseme already exists - replace it with the new definition
	{
		SkMotion* motion = (*iter).second.first;
		if (motion)
		{
			// no motion given, add the viseme only
			if (motionName == "")
			{
				_visemeMap.erase(iter);
				_visemeMap.insert(std::pair<std::string, std::pair<SkMotion*, float> >(visemeName, std::pair<SkMotion*, float>((SkMotion*)nullptr, 1.0f)));
				SmartBody::util::log("Viseme '%s' with motion '%s' replaced with no motion.", visemeName.c_str(), motion->getName().c_str()); 
				return;
			}
			else
			{
				SmartBody::SBMotion* newMotion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(motionName);
				if (!newMotion)
				{
					SmartBody::util::log("Cannot find viseme named '%s'. Viseme named '%s' not added.", visemeName.c_str(), motionName.c_str());
					return;
				}
				else
				{
					_visemeMap.erase(iter);
					_visemeMap.insert(std::pair<std::string, std::pair<SkMotion*, float> >(visemeName, std::pair<SkMotion*, float>(newMotion, 1.0f)));
					SmartBody::util::log("Viseme '%s' with motion '%s' replaced with motion '%s'.", visemeName.c_str(), motion->getName().c_str(), motionName.c_str());
					return;
				}
			}
		}
	}
}

void SBFaceDefinition::setVisemeWeight(const std::string& visemeName, float weight)
{
	auto iter = _visemeMap.find(visemeName);
	if (iter == _visemeMap.end())
	{
		SmartBody::util::log("Viseme '%s' does not exist, cannot set its' weight.", visemeName.c_str());
		return;
	}
	else // viseme already exists - replace it with the new definition
	{
		(*iter).second.second = weight;
		SmartBody::util::log("Viseme '%s' now has weight %f", visemeName.c_str(), weight);
	}
}

int SBFaceDefinition::getNumVisemes()
{
	return _visemeMap.size();
}

std::vector<std::string> SBFaceDefinition::getVisemeNames()
{
	std::vector<std::string> visemeNames;
	for (auto & iter : _visemeMap)
	{
		visemeNames.emplace_back(iter.first);
	}

	return visemeNames;
}

const std::string& SBFaceDefinition::getVisemeName(int index)
{
	int counter = 0;
	for (auto & iter : _visemeMap)
	{
		if (counter == index)
		{
			return iter.first;
			break;
		}
		counter++;
	}

	return _emptyString;
}

SkMotion* SBFaceDefinition::getVisemeMotion(const std::string& viseme)
{
	auto iter = _visemeMap.find(viseme);
	if (iter != _visemeMap.end())
	{
		return (*iter).second.first;
	}

	return nullptr;
}

float SBFaceDefinition::getVisemeWeight(const std::string& viseme)
{
	auto iter = _visemeMap.find(viseme);
	if (iter != _visemeMap.end())
	{
		return (*iter).second.second;
	}

	return 0.0f;
}

int SBFaceDefinition::getNumAUs()
{
	return _auMap.size();
}

int SBFaceDefinition::getNumAUChannels()
{
	int numAuChannels = 0;
	auto iter = _auMap.begin();
	for (; iter != _auMap.end(); iter++)
	{
		if (iter->second->is_bilateral())
			numAuChannels += 1;
		else
			numAuChannels += 2;
	}
	return numAuChannels;
}

int SBFaceDefinition::getAUNum(int index)
{
	int counter = 0;
	for (auto & iter : _auMap)
	{
		if (counter == index)
		{
			return iter.first;
			break;
		}
		counter++;
	}

	return -1;
}

ActionUnit* SBFaceDefinition::getAU(int index)
{
	auto iter = _auMap.find(index);
	if (iter == _auMap.end())
		return nullptr;
	else
		return (*iter).second.get();
}

std::vector<int> SBFaceDefinition::getAUNumbers()
{
	std::vector<int> auNames;
	for (auto & iter : _auMap)
	{
		auNames.emplace_back(iter.first);
	}

	return auNames;
}

std::string SBFaceDefinition::getAUSide(int num)
{
	auto iter = _auMap.find(num);
	if (iter == _auMap.end())
	{
		SmartBody::util::log("AU %d not found.", num);
		return "";
	}

	if ((*iter).second->is_bilateral())
		return "BOTH";
	else if ((*iter).second->is_left() && (*iter).second->is_right())
		return "LEFTRIGHT";
	else if ((*iter).second->is_left())
		return "LEFT";
	else 
		return "RIGHT";

}

SBMotion* SBFaceDefinition::getAUMotion(int num, const std::string& side)
{
	auto iter = _auMap.find(num);
	if (iter == _auMap.end())
	{
		SmartBody::util::log("AU %d not found.", num);
		return nullptr;
	}

	SkMotion* motion = nullptr;
	auto& au = (*iter).second;
	if (side == "LEFT" || side == "left")
	{
		if (au->is_left())
		{
			motion = au->left;
		}
		else
		{
			SmartBody::util::log("Action Unit %d does not have a LEFT motion.", num);
			return nullptr;
		}
	}
	else if (side == "RIGHT" || side == "right")
	{
		if (au->is_right())
		{
			motion = au->right;
		}
		else
		{
			SmartBody::util::log("Action Unit %d does not have a RIGHT motion.", num);
			return nullptr;
		}
	}
	else if (side == "BOTH" || side == "both")
	{
		if (au->is_bilateral())
		{
			motion = au->left;
		}
		else
		{
			SmartBody::util::log("Action Unit %d does not have a BOTH motion.", num);
			return nullptr;
		}
	}
	else
	{
		SmartBody::util::log("Use: LEFT, RIGHT, BOTH for Action Unit side.");
		return nullptr;
	}

	auto* sbmotion = dynamic_cast<SBMotion*>(motion);
	return sbmotion;
}

std::string SBFaceDefinition::saveToString()
{
	std::stringstream strstr;
	strstr << "# face definition " << this->getName() << "\n";
	strstr << "# autogenerated by SmartBody\n";
	
	std::string faceName = this->getName() + "_Face";
	strstr << faceName << " = scene.createFaceDefinition(\"" << this->getName() << "\")\n";
	if (this->getFaceNeutral())
		strstr << faceName << ".setFaceNeutral(\"" << this->getFaceNeutral()->getName() << "\")\n";

	for (int i = 0; i < this->getNumAUs(); i++)
	{
		int index = this->getAUNum(i);
		ActionUnit* aUnit = this->getAU(index);
		if (aUnit->is_left())
			strstr << faceName << ".setAU(" << index << ", \"left\", \"" << this->getAUMotion(index, "left")->getName() << "\")\n";
		if (aUnit->is_right())
			strstr << faceName << ".setAU(" << index << ", \"right\", \"" << this->getAUMotion(index, "right")->getName() << "\")\n";
		if (aUnit->is_bilateral())
			strstr << faceName << ".setAU(" << index << ", \"both\", \"" << this->getAUMotion(index, "both")->getName() << "\")\n";
	}

	const std::vector<std::string>& visemeNames = this->getVisemeNames();
	for (int i = 0; i < this->getNumVisemes(); i++)
	{
		strstr << faceName << ".setViseme(\"" << visemeNames[i] << "\", \"" << this->getVisemeMotion(visemeNames[i])->getName() << "\")\n";
	}

	return strstr.str();
}

void SBFaceDefinition::save(const std::string& fileName)
{
	std::ofstream file(fileName.c_str());
	if (!file.is_open())
	{
		SmartBody::util::log("Problem writing to file %s, face definition was not saved.", fileName.c_str());
		return;
	}
	const std::string& fileContent = saveToString();
	file << fileContent;
	file.close();		
}

}
