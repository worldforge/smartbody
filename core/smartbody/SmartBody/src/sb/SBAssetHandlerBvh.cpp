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

#include "SBAssetHandlerBvh.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include "SBUtilities.h"
#include <sbm/ParserBVH.h>


namespace SmartBody {

SBAssetHandlerBvh::SBAssetHandlerBvh()
{
	assetTypes.push_back("bvh");
}

SBAssetHandlerBvh::~SBAssetHandlerBvh()
{
}

std::vector<SBAsset*> SBAssetHandlerBvh::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;
	
	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );
	FILE* myfile = fopen(convertedPath.c_str(), "rt");
	SrInput input(myfile);


	std::ifstream filestream(convertedPath.c_str());

	SmartBody::SBSkeleton* skeleton = new SmartBody::SBSkeleton();
	double scale = 1.0;
	if (SmartBody::SBScene::getScene()->getAttribute("globalSkeletonScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");

	SmartBody::SBMotion* motion = new SmartBody::SBMotion();

	bool ok = ParserBVH::parse(*skeleton, *motion, convertedPath, filestream, float(scale));
	if (!ok)
	{
		delete motion;
		delete skeleton;
		SmartBody::util::log("Could not .bvh file %s", convertedPath.c_str());
	}
	else
	{
		motion->setName(fileName + extension);
		skeleton->setName(fileName + extension);
		skeleton->setFileName(convertedPath);
		assets.push_back(motion);
		assets.push_back(skeleton);
	}
	
	return assets;
}

};
