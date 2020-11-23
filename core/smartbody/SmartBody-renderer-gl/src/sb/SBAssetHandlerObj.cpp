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

#include "SBAssetHandlerObj.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sr/sr_model.h>

namespace SmartBody {

SBAssetHandlerObj::SBAssetHandlerObj()
{
	assetTypes.emplace_back("obj");
}

SBAssetHandlerObj::~SBAssetHandlerObj()
= default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerObj::getAssets(const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );

	auto model = std::unique_ptr<SrModel>();
	bool loadSuccess = model->import_obj(convertedPath.c_str());	
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
		
		if (model->Fn.empty())
		{
			model->computeNormals();
		}

		auto* srSnModelStatic = new SrSnModel();
		srSnModelStatic->shape(*model);
		srSnModelStatic->shape().name = model->name;
		mesh->dMeshStatic_p.push_back(srSnModelStatic);
		srSnModelStatic->ref();

		auto* srSnModelDynamic = new SrSnModel();
		srSnModelDynamic->shape(*model);
		srSnModelDynamic->changed(true);
		srSnModelDynamic->visible(false);
		srSnModelDynamic->shape().name = model->name;
		mesh->dMeshDynamic_p.push_back(srSnModelDynamic);
		srSnModelDynamic->ref();

		assets.emplace_back(std::move(mesh));
	}

	return assets;
}

};
