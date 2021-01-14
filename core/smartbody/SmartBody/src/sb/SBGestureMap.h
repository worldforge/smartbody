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

#ifndef _SBGESTUREMAP_H_
#define _SBGESTUREMAP_H_

#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <sb/SBObject.h>
#include "sb/SBSceneOwned.h"

namespace SmartBody {

class SBGestureMap : public SBObject, public SBSceneOwned
{
	public:
		SBAPI struct GestureInfo
		{
			std::string getAnimation() { return _animation; };
			std::string getLexeme() { return _lexeme; };
			std::string getType() { return _type; };
			std::string getHand() { return _hand; };
			std::string getStyle() { return _style; };
			std::string getPosture() { return _posture; };

			std::string _animation;
			std::string _lexeme;
			std::string _type;
			std::string _hand;
			std::string _style;
			std::string _posture;
		};

	public:
		SBAPI SBGestureMap(SBScene& scene);
		SBAPI SBGestureMap(SBScene& scene, const std::string& name);
		SBAPI ~SBGestureMap();

		SBAPI SBGestureMap* copy();

		SBAPI void addGestureMapping(const std::string& name, const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture);
		SBAPI std::string getGestureByInfo(const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture, const std::string& policy, int useIndex, int& appliedIndex);
		SBAPI std::vector<std::string> getGestureListByInfo(const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture);
		SBAPI GestureInfo& getGestureByIndex(int i);
		SBAPI std::string getGestureAnimation(int i);
		SBAPI int getNumMappings();
		SBAPI void validate();

	private:
		bool gestureInfoCompare(const std::string& glexeme, const std::string& gtype, const std::string& ghand, const std::string& gstyle, const std::string& gposture,
								const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture, 
								std::string compType);

	protected:
		std::vector<GestureInfo> _gestureMaps;
		GestureInfo defaultGestureInfo;
};

}

#endif