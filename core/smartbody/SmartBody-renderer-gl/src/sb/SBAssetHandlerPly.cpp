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

#include "SBAssetHandlerPly.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include "sbm/GPU/SbmDeformableMeshGPU.h"
#include <sr/sr_model.h>

namespace SmartBody {

SBAssetHandlerPly::SBAssetHandlerPly()
{
	assetTypes.emplace_back("ply");
}

SBAssetHandlerPly::~SBAssetHandlerPly() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerPly::getAssets(const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );

	SrModel model;
	SmartBody::util::log("Loading PLY asset '%s'...", convertedPath.c_str());
	bool loadSuccess = model.import_ply(convertedPath.c_str());
	if (loadSuccess)
	{
#if !defined(__FLASHPLAYER__) && !defined(__native_client__) && !defined(EMSCRIPTEN)
		auto mesh = std::make_unique<SbmDeformableMeshGPU>();
#else
		DeformableMesh* mesh = new DeformableMesh();
#endif
		mesh->setName(fileName + extension);
		
		//float factor = 1.0f;
		//for (int j = 0; j < model->V.size(); j++)
		//{
		//	model->V[j] *= factor;
		//}
		
// 		if (model->Fn.size() == 0)
// 		{
// 			model->computeNormals();
// 		}

		auto* srSnModelStatic = new SrSnModel();
		srSnModelStatic->shape(model);
		srSnModelStatic->shape().name = model.name;
		mesh->dMeshStatic_p.emplace_back(srSnModelStatic);
		srSnModelStatic->ref();

		auto* srSnModelDynamic = new SrSnModel();
		srSnModelDynamic->shape(model);
		srSnModelDynamic->changed(true);
		srSnModelDynamic->visible(false);
		srSnModelDynamic->shape().name = model.name;
		mesh->dMeshDynamic_p.emplace_back(srSnModelDynamic);
		srSnModelDynamic->ref();

		assets.emplace_back(std::move(mesh));
	}

	return assets;
}

};
