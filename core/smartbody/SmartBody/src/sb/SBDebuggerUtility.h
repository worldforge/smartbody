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

#ifndef _SBM_DEBUGGER_UTILITY_
#define _SBM_DEBUGGER_UTILITY_

#include <string>

class SBDebuggerUtility
{
public:
	SBDebuggerUtility();
	~SBDebuggerUtility();

	void initScene();
	void queryResources();

	void initCharacter(const std::string& name, const std::string& skelName);
	void initCharacterFaceDefinition(const std::string& characterName, const std::string& faceDefName, const std::string& message);
	void initPawn(const std::string& name);
	void initSkeleton(const std::string& skFileName, const std::string& info);

	void runPythonCommand(const std::string& info);

	void updateCharacter(const std::string& cName, const std::string& jName, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ, float& rotW);
	void updatePawn(const std::string& pName, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ, float& rotW);
	void updateCamera(float& eyePosX, float& eyePosY, float& eyePosZ, float& lookAtPosX, float& lookAtPosY, float& lookAtPosZ, float& fovY, float& aspect, float& zNear, float zFar);
};

#endif