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

#include "SBPhoneme.h"


namespace SmartBody{


SBDiphone::SBDiphone()
{
	_fromPhoneme = "";
	_toPhoneme = "";
}

SBDiphone::SBDiphone(const std::string& fromPhoneme, const std::string& toPhoneme)
{
	_fromPhoneme = fromPhoneme;
	_toPhoneme = toPhoneme;
}

SBDiphone::~SBDiphone()
{
	clean();
}

void SBDiphone::addKey(const std::string& viseme, float time, float weight)
{
	// insert the viseme if not existing
	std::map<std::string, std::vector<float> >::iterator iter = _visemeKeysMap.find(viseme);
	if (iter == _visemeKeysMap.end())
	{
		std::vector<float> newFloat;
		_visemeKeysMap.insert(std::make_pair(viseme, newFloat));
	}

	std::vector<float>& curveData = _visemeKeysMap[viseme];
	// insert the data ascendingly
	if (curveData.size() == 0)
	{
		curveData.emplace_back(time);
		curveData.emplace_back(weight);
	}
	else
	{
		for (size_t i = 0; i < curveData.size(); i++)
		{
			if ((i % 2) == 0)
			{
				if (curveData[i] > time)
				{
					curveData.insert(curveData.begin() + i, time);
					curveData.insert(curveData.begin() + i + 1, weight);
					return;
				}
			}
		}
		curveData.emplace_back(time);
		curveData.emplace_back(weight);
	}
}

void SBDiphone::setKey(const std::string& viseme, std::vector<float>& keys)
{
	std::map<std::string, std::vector<float> >::iterator iter = _visemeKeysMap.find(viseme);
	if (iter == _visemeKeysMap.end())
	{
		std::vector<float> newFloat;
		_visemeKeysMap.insert(std::make_pair(viseme, newFloat));
	}

	_visemeKeysMap[viseme] = keys;
}

std::vector<float>& SBDiphone::getKeys(const std::string& viseme)
{
	std::map<std::string, std::vector<float> >::iterator iter = _visemeKeysMap.find(viseme);
	if (iter == _visemeKeysMap.end())
	{
		std::vector<float> newFloat;
		_visemeKeysMap.insert(std::make_pair(viseme, newFloat));
	}

	return _visemeKeysMap[viseme];
}

std::vector<std::string> SBDiphone::getVisemeNames()
{
	std::vector<std::string> visemeNames;
	std::map<std::string, std::vector<float> >::iterator iter;
	for (iter = _visemeKeysMap.begin(); iter != _visemeKeysMap.end(); iter++)
	{
		visemeNames.emplace_back(iter->first);
	}
	return visemeNames;
}


int SBDiphone::getNumVisemes()
{
	return _visemeKeysMap.size();
}

void SBDiphone::clean()
{
	std::map<std::string, std::vector<float> >::iterator iter = _visemeKeysMap.begin();
	for (; iter != _visemeKeysMap.end(); iter++)
	{
		iter->second.clear();
	}
	_visemeKeysMap.clear();
}

const std::string& SBDiphone::getFromPhonemeName()
{
	return _fromPhoneme;
}

const std::string& SBDiphone::getToPhonemeName()
{
	return _toPhoneme;
}

}
