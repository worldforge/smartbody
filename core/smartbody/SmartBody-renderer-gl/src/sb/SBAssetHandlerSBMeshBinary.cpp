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

#include "SBAssetHandlerSBMeshBinary.h"
#include "SBUtilities.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>

namespace SmartBody {

	SBAssetHandlerSBMeshBinary::SBAssetHandlerSBMeshBinary()
	{
		assetTypes.emplace_back("smb");
		assetTypes.emplace_back("dmb");
	}

	SBAssetHandlerSBMeshBinary::~SBAssetHandlerSBMeshBinary() = default;

	std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerSBMeshBinary::getAssets(const std::string& path)
	{
		std::vector<std::unique_ptr<SBAsset>> assets;

		std::string convertedPath = checkPath(path);
		if (convertedPath.empty())
			return assets;

		boost::filesystem::path p(convertedPath);
		std::string fileName = boost::filesystem::basename(p);
		std::string extension =  boost::filesystem::extension(p);

#if !defined(__FLASHPLAYER__) && !defined(__native_client__) && !defined(EMSCRIPTEN)
		SmartBody::util::log("Creating GPU Deformable Mesh");
		auto mesh = std::make_unique<SbmDeformableMeshGPU>();
#else
		DeformableMesh* mesh = new DeformableMesh();
#endif
		if (extension == ".smb")	// load in
		{
			bool ok = mesh->readFromSmb(convertedPath);
			if (ok)
				assets.emplace_back(std::move(mesh));
		}
		else if (extension == ".dmb")
		{
			bool ok = mesh->readFromDmb(convertedPath);
			if (ok)
			{
				mesh->computeNormals();
				assets.emplace_back(std::move(mesh));
			}
		}
		return assets;
	}

};
