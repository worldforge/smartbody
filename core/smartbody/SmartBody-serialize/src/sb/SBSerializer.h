/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>

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

#ifndef SMARTBODY_SBSERIALIZER_H
#define SMARTBODY_SBSERIALIZER_H

#include <memory>
#include <vector>
#include <string>

namespace SmartBody {
class SBMotion;

class SBSkeleton;
class SBJoint;

/**
 * TODO: all inputs for save functions should be "const". Requires a larger const-refactoring of the whole code base though.
 */
struct SBSerializer {

	static void saveToSkm(SBMotion& motion, const std::string& fileName);

	static void saveToSkb(SBMotion& motion, const std::string& fileName);

	static bool readFromSkb(SBMotion& motion, const std::string& fileName);

	static void saveToSkmByFrames(SBMotion& motion, const std::string& fileName, int startFrame, int endFrame);

	static void saveToBVH(SBMotion& motion, const std::string& fileName, SBSkeleton& skel);

	static void printHierarchy(FILE* file, SmartBody::SBJoint* joint, int depth, std::vector<int>& jointIdxs);

};
}


#endif //SMARTBODY_SBSERIALIZER_H
