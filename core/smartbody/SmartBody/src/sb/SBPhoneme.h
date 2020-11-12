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

#ifndef _SBPHONEME_H_
#define _SBPHONEME_H_

#include <sb/SBTypes.h>
#include <vector>
#include <string>
#include <map>

namespace SmartBody {

class SBDiphone
{
public:
	SBAPI SBDiphone();
	SBAPI SBDiphone(const std::string& fromPhoneme, const std::string& toPhoneme);
	SBAPI ~SBDiphone();

	SBAPI const std::string& getFromPhonemeName();
	SBAPI const std::string& getToPhonemeName();

	SBAPI void addKey(const std::string& viseme, float time, float weight);
	SBAPI void setKey(const std::string& viseme, std::vector<float>& keys);
	SBAPI std::vector<float>& getKeys(const std::string& viseme);
	SBAPI std::vector<std::string> getVisemeNames();
	SBAPI int getNumVisemes();
	SBAPI void clean();

private:
	std::string _fromPhoneme;
	std::string _toPhoneme;
	std::map<std::string, std::vector<float> > _visemeKeysMap;
};

}

#endif
