/*************************************************************
Copyright (C) 2021 Erik Ogenvik <erik@ogenvik.org>

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

#ifndef SMARTBODY_BASICCHARACTER_H
#define SMARTBODY_BASICCHARACTER_H


#include <sb/SBScene.h>

class BasicCharacter {
public:
	BasicCharacter(SmartBody::SBScene& scene);

	void retargetBehaviorSetMaleMocapLocomotion(const std::string& charName);

	void retargetBehaviorSetGestures(const std::string& charName);

	void retargetBehaviorSetReaching(const std::string& charName);

	SmartBody::SBFaceDefinition& bradFace;
private:
	SmartBody::SBScene& mScene;
	void setupBehaviorSetMaleMocapLocomotion();
	void setupBehaviorSetGestures();
	void setupBehaviorSetReaching();
};


#endif //SMARTBODY_BASICCHARACTER_H
