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

#ifndef BVHPARSER_H
#define BVHPARSER_H

#include <fstream>
#include "sk/sk_skeleton.h"
#include "sk/sk_motion.h"

struct ChannelInfo
{
	int channels[6];
	int numBVHChannels;
	int order;
	int startingChannelIndex;
};

class ParserBVH
{
	public:
		static bool parse(SkSkeleton& skeleton, SkMotion& motion, std::string name, std::ifstream &file, float scale, int N1 = -1, int N2 = 2000000);
		static void convertBVHtoSmartBody(SkJoint* joint, SkMotion& motion, ChannelInfo* channelInfo, double data[6], float* posture, double frameTime, float scale);
		static int determineRotationOrder(int bvhChannels[6], int numBVHChannels);

};

#endif
