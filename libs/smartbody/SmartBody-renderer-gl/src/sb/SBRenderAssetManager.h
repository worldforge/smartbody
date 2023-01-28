/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>
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

#ifndef SMARTBODY_SBRENDERASSETMANAGER_H
#define SMARTBODY_SBRENDERASSETMANAGER_H

#include "sb/SBAssetStore.h"
#include "sb/SBSceneOwned.h"
#include <boost/noncopyable.hpp>
#include <map>
#include <string>
#include <memory>

class DeformableMesh;
class SbmTexture;
namespace SmartBody {
class SBRenderAssetManager : public SBAssetsProcessor, public SBSceneOwned, public boost::noncopyable {
public:
	explicit SBRenderAssetManager(SBScene& scene, SBAssetStore& assetStore);

	void processAssets(std::vector<std::unique_ptr<SBAsset>>& assets) override ;
	SBAPI void removeAllAssets();

	SBAPI void addMesh(std::unique_ptr<DeformableMesh> mesh);
	SBAPI void addMesh(DeformableMesh* mesh);
	SBAPI void removeMesh(DeformableMesh* mesh);
	SBAPI int getNumMeshes();
	SBAPI std::vector<std::string> getMeshNames();
	SBAPI bool createMeshFromBlendMasks(const std::string& neutralShapeFile,
									 const std::string& neutralTextureFile,
									 const std::string& expressiveShapeFile,
									 const std::string& expressiveTextureFile,
									 const std::string& maskTextureFile,
									 const std::string& outputMeshFile,
									 const std::string& outputTextureFile);


	SBAPI void addDeformableMesh(const std::string& meshName, std::unique_ptr<DeformableMesh> mesh);
	SBAPI void removeDeformableMesh(const std::string& meshName);
	SBAPI DeformableMesh* getDeformableMesh(const std::string& meshName);
	SBAPI std::vector<std::string> getDeformableMeshNames();
	SBAPI void removeAllDeformableMeshes();

	SBAPI std::shared_ptr<SbmTexture> getHDRTexture(const std::string& texName);
	SBAPI std::vector<std::string> getHDRTextureNames();

	SBAPI std::string getAssetNameVariation(SBAsset* asset);
//	SBAPI std::vector<std::string>& getAssetHistory();
//	SBAPI void clearAssetHistory();

	SBAPI bool handlePenetrations(std::string deformableMesh, std::string baseModel, std::string penetratingModel, float offset, bool showVisualization);
	SBAPI bool addBlendshapeToModel(std::string templateMeshName, std::string modelFile, std::string shapeName, std::string submeshName);
	SBAPI bool addModelToMesh(std::string templateMeshName, std::string modelFile, std::string newModelName, std::string rigidBindJoint, std::string bindPoseCopySubMeshName);

protected:

	SBAssetStore& _assetStore;
	std::map<std::string, std::unique_ptr<DeformableMesh>> _deformableMeshMap;
	int _meshCounter;

	SBAssetsHistory _history;

};
}


#endif //SMARTBODY_SBRENDERASSETMANAGER_H
