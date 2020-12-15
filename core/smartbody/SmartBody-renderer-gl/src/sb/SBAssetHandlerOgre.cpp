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

#include "SBAssetHandlerOgre.h"

#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBAssetManager.h>
#include <sbm/ParserOgre.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>

namespace SmartBody {

SBAssetHandlerOgre::SBAssetHandlerOgre()
{
	assetTypes.emplace_back("xml");
}

SBAssetHandlerOgre::~SBAssetHandlerOgre() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerOgre::getAssets(const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );

	auto skeleton = std::make_unique<SmartBody::SBSkeleton>();
	skeleton->skfilename(convertedPath.c_str());
	skeleton->setName(fileName + extension);

	double scale = 1.0;
	if (SmartBody::SBScene::getScene()->getAttribute("globalSkeletonScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");

	// is this a .skeleton.xml or a .mesh.xml file?
	if (convertedPath.find(".skeleton.xml") != std::string::npos)
	{
		std::vector<std::unique_ptr<SmartBody::SBMotion>> motions;
		bool ok = ParserOgre::parse(*skeleton, motions, convertedPath, float(scale), true, true);

		if (ok)
		{
			assets.emplace_back(std::move(skeleton));
			for (auto & motion : motions)
			{
				assets.emplace_back(std::move(motion));
			}
		}
	}
	else if (convertedPath.find(".mesh.xml") != std::string::npos)
	{
		// parse the mesh and skinweights in a separate pass (should be collapsed into only one pass...)
#if !defined(__native_client__) && !defined(EMSCRIPTEN)
		auto mesh = std::make_unique<SbmDeformableMeshGPU>();
#else
		auto mesh = std::make_unique<DeformableMesh>();
#endif
		std::vector<SrModel*> meshModelVec;
		std::vector<SkinWeight*> tempWeights;
		std::string skeletonName;
		bool ok = ParserOgre::parseSkinMesh(meshModelVec, mesh->skinWeights, convertedPath, skeletonName, 1.0, true, true);		
		if (ok)
		{
			boost::intrusive_ptr<SmartBody::SBSkeleton> existingSkeleton = nullptr;
			if (!skeletonName.empty())
			{
				// find the skeleton
				existingSkeleton = SmartBody::SBScene::getScene()->getAssetManager()->getSkeleton(skeletonName);
				if (existingSkeleton)
				{
					for (auto sw : mesh->skinWeights)
					{
							for (int k=0; k < existingSkeleton->getNumJoints(); k++)
						{
							// manually add all joint names
							SmartBody::SBJoint* joint = existingSkeleton->getJoint(k);
							sw->infJointName.emplace_back(joint->getName());
							sw->infJoint.emplace_back(joint);
							SrMat gmatZeroInv = joint->gmatZero().rigidInverse();						
							sw->bindPoseMat.emplace_back(gmatZeroInv);
						}
					}
				}

				if (existingSkeleton)
				{
					for (auto & skinWeight : mesh->skinWeights)
					{
						for (size_t i = 0; i < skinWeight->jointNameIndex.size(); i++)
						{
							SmartBody::SBJoint* joint = existingSkeleton->getJoint(skinWeight->jointNameIndex[i]);
							skinWeight->infJointName.emplace_back(joint->getName());
						}
					}
				}

			}
			
			mesh->setName(fileName + extension);
			for (auto & i : meshModelVec)
			{
				//for (int j = 0; j < meshModelVec[i]->V.size(); j++)
				//{
				//	meshModelVec[i]->V[j] *= factor;
				//}
		
				if (i->Fn.empty())
				{
					i->computeNormals();
				}

				auto* srSnModelStatic = new SrSnModel();
				srSnModelStatic->shape(*i);
				srSnModelStatic->shape().name = i->name;
				mesh->dMeshStatic_p.emplace_back(srSnModelStatic);
				srSnModelStatic->ref();

				auto* srSnModelDynamic = new SrSnModel();
				srSnModelDynamic->shape(*i);
				srSnModelDynamic->changed(true);
				srSnModelDynamic->visible(false);
				srSnModelDynamic->shape().name = i->name;
				mesh->dMeshDynamic_p.emplace_back(srSnModelDynamic);
				srSnModelDynamic->ref();
			
				delete i;
			}
			assets.emplace_back(std::move(mesh));
		}
	}

	return assets;
}

};
