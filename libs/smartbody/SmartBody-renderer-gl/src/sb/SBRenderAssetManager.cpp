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

#include "SBRenderAssetManager.h"

#include "sbm/GPU/SbmTexture.h"
#include "sbm/GPU/SbmDeformableMeshGPU.h"
#include "sbm/sbm_deformable_mesh.h"
#include "sb/SBScene.h"
#include "sb/SBSceneListener.h"

#include "SBUtilities.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <boost/filesystem.hpp>
#include "sb/SBAssetStore.h"

namespace {
#define SMALL_NUM   0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
#define ddot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)

// from Graphics Gems http://geomalgorithms.com/a06-_intersect-2.html
bool
intersect3D_RayTriangle(SrLine L, SrPnt V0, SrPnt V1, SrPnt V2, SrPnt& I, float& rdist, SrVec& normal)
{
	SrPnt    u, v, n;              // triangle vectors
	SrPnt    dir, w0, w;           // ray vectors
	float     r, a, b;              // params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	u = V1 - V0;
	v = V2 - V0;
	n = cross(u, v);              // cross product
	if (n.iszero())             // triangle is degenerate
		return false;                  // do not deal with this case

	normal = n;
	normal.normalize();

	dir = L.p2 - L.p1;              // ray direction vector
	w0 = L.p1 - V0;
	a = -ddot(n, w0);
	b = ddot(n, dir);
	if (fabs(b) < SMALL_NUM) {     // ray is  parallel to triangle plane
		if (a == 0)                 // ray lies in triangle plane
			return true;
		else
			return false;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                    // ray goes away from triangle
		return false;                   // => no intersect

	rdist = r;
	// for a segment, also test if (r > 1.0) => no intersect

	I = L.p1 + r * dir;            // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = ddot(u, u);
	uv = ddot(u, v);
	vv = ddot(v, v);
	w = I - V0;
	wu = ddot(w, u);
	wv = ddot(w, v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)         // I is outside T
		return false;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return false;

	return true;                       // I is in T
}
}

namespace SmartBody {

SBRenderAssetManager::SBRenderAssetManager(SBScene& scene, SBAssetStore& assetStore) :
		SBSceneOwned(scene),
		_assetStore(assetStore),
		_meshCounter(0) {

	assetStore.addAssetProcessor(this);
	scene.registerObjectProvider("mesh", SBScene::Provider{
			[&](const std::string& name) {
				return this->getDeformableMesh(name);
			},
			[](SmartBody::SBObject& sbObject) -> std::string {
				if (dynamic_cast<DeformableMesh*>(&sbObject)) {
					return sbObject.getName();
				}
				return "";
			}
	});

}

void SBRenderAssetManager::removeAllAssets() {
	_assetStore.removeAssetProcessor(this);

	_deformableMeshMap.clear();
}


void SBRenderAssetManager::addDeformableMesh(const std::string& meshName, std::unique_ptr<DeformableMesh> mesh) {
	_deformableMeshMap[meshName] = std::move(mesh);
}

void SBRenderAssetManager::removeDeformableMesh(const std::string& meshName) {
	auto iter = _deformableMeshMap.find(meshName);
	if (iter != _deformableMeshMap.end()) {
		_deformableMeshMap.erase(iter);
	}
}

DeformableMesh* SBRenderAssetManager::getDeformableMesh(const std::string& meshName) {
	auto iter = _deformableMeshMap.find(meshName);
	if (iter != _deformableMeshMap.end()) {
		return (*iter).second.get();
	}

	return nullptr;
}

std::vector<std::string> SBRenderAssetManager::getDeformableMeshNames() {
	std::vector<std::string> ret;

	for (auto& iter : _deformableMeshMap) {
		ret.emplace_back(iter.first);
	}

	return ret;
}

void SBRenderAssetManager::removeAllDeformableMeshes() {
	_deformableMeshMap.clear();
}

std::shared_ptr<SbmTexture> SBRenderAssetManager::getHDRTexture(const std::string& texName) {
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	return texManager.findTexture(texName.c_str());
}

std::vector<std::string> SBRenderAssetManager::getHDRTextureNames() {
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	return texManager.getTextureNames(SbmTextureManager::TEXTURE_HDR_MAP);
}


void SBRenderAssetManager::processAssets(std::vector<std::unique_ptr<SBAsset>>& assets) {
// place the assets in their proper place
	for (auto& asset : assets) {
		if (asset) {
			auto* mesh = dynamic_cast<DeformableMesh*>(asset.get());
			if (mesh) {
				DeformableMesh* existingMesh = this->getDeformableMesh(mesh->getName());
				if (existingMesh) {
					std::string name = getAssetNameVariation(existingMesh);
					SmartBody::util::log("Mesh named %s already exist, changing name to %s", existingMesh->getName().c_str(), name.c_str());
					existingMesh->setName(name);
				}
				asset.release();
				this->addMesh(std::unique_ptr<DeformableMesh>(mesh));
				_history.addAssetHistory("MESH " + mesh->getName());
				continue;
			}
//			auto* texture = dynamic_cast<SbmTexture*>(asset.get());
//			if (texture) {
//				_history.addAssetHistory("ENVMAP " + texture->getName());
//				continue;
//			}
		}
	}
}

SBAPI std::string SBRenderAssetManager::getAssetNameVariation(SBAsset* asset) {

	int* counter = nullptr;

	auto* mesh = dynamic_cast<DeformableMesh*>(asset);
	if (mesh) {
		while (true) {
			_meshCounter++;
			std::stringstream strstr;
			strstr << mesh->getName() << _meshCounter;
			if (!getDeformableMesh(strstr.str())) {
				return strstr.str();

			}
		}
	}

	return "xxxxxxx";


}

SBAPI void SBRenderAssetManager::addMesh(std::unique_ptr<DeformableMesh> mesh) {
	auto iter = _deformableMeshMap.find(mesh->getName());
	if (iter != _deformableMeshMap.end()) {
		SmartBody::util::log("Mesh named %s already exists, new mesh will not be added.", mesh->getName().c_str());
	} else {
		auto result = _deformableMeshMap.emplace(mesh->getName(), std::move(mesh));
		if (result.second) {
			auto& listeners = _scene.getSceneListeners();
			for (auto listener : listeners) {
				listener->OnObjectCreate(result.first->second.get());
			}
		}
	}
}

SBAPI void SBRenderAssetManager::addMesh(DeformableMesh* mesh) {
	addMesh(std::unique_ptr<DeformableMesh>(mesh));
}

SBAPI void SBRenderAssetManager::removeMesh(DeformableMesh* mesh) {
	if (!mesh) return;

	std::unique_ptr<DeformableMesh> existingMesh;
	auto iter = _deformableMeshMap.find(mesh->getName());
	if (iter != _deformableMeshMap.end()) {
		SmartBody::util::log("Remove mesh '%s'", mesh->getName().c_str());
		existingMesh = std::move(iter->second);
		_deformableMeshMap.erase(iter);
	}

	if (existingMesh) {
		auto& listeners = _scene.getSceneListeners();
		for (auto listener : listeners) {
			listener->OnObjectDelete(existingMesh.get());
		}
	}
}


int SBRenderAssetManager::getNumMeshes() {
	return _deformableMeshMap.size();
}


std::vector<std::string> SBRenderAssetManager::getMeshNames() {
	std::vector<std::string> ret;

	for (auto& iter : _deformableMeshMap) {
		auto& mesh = iter.second;
		ret.emplace_back(mesh->getName());
	}

	return ret;
}

bool SBRenderAssetManager::createMeshFromBlendMasks(const std::string& neutralShapeFile,
													const std::string& neutralTextureFile,
													const std::string& expressiveShapeFile,
													const std::string& expressiveTextureFile,
													const std::string& maskTextureFile,
													const std::string& outputMeshFile,
													const std::string& outputTextureFile) {
	bool ok = true;

	SbmTextureManager& texManager = SbmTextureManager::singleton();

	// load the neutral shape
	DeformableMesh* neutralMesh = nullptr;
	std::vector<std::unique_ptr<SBAsset>> assets = _assetStore.loadAsset(neutralShapeFile);
	for (auto& asset : assets) {
		auto mesh = dynamic_cast<DeformableMesh*>(asset.get());
		if (mesh) {
			neutralMesh = mesh;
			//Get the first one
			break;
		}
	}
	if (!neutralMesh) {
		SmartBody::util::log("Could not find neutral mesh in file %s.", neutralShapeFile.c_str());
		ok = false;
	}

	// make sure that a diffuse texture exists
	std::shared_ptr<SbmTexture> neutralTexture;

	if (neutralMesh) {
		for (auto& subMesh : neutralMesh->subMeshList) {
			neutralTexture = texManager.findTexture(subMesh->texName.c_str());
			if (neutralTexture) {
				break;
			}
		}
		if (!neutralTexture) {
			if (!neutralMesh->dMeshStatic_p.empty()) {
				SrModel& model = neutralMesh->dMeshStatic_p[0]->shape();
				for (auto& iter : model.mtlTextureNameMap) {
					std::string textureCandidate = iter.second;
					neutralTexture = texManager.findTexture(textureCandidate.c_str());
					if (neutralTexture)
						break;
				}
			}
		}
	}
	if (!neutralTexture) {
		SmartBody::util::log("Could not find neutral diffuse texture in file %s.", neutralShapeFile.c_str());
		ok = false;
	}

	// load the expressive shape
	DeformableMesh* expressiveMesh = nullptr;
	assets = _assetStore.loadAsset(expressiveShapeFile);
	for (auto& asset : assets) {
		auto mesh = dynamic_cast<DeformableMesh*>(asset.get());
		if (mesh) {
			expressiveMesh = mesh;
		}
	}
	if (!expressiveMesh) {
		SmartBody::util::log("Could not find expressive mesh in file %s.", expressiveShapeFile.c_str());
		ok = false;
	}

	// make sure that an expressive diffuse texture exists
	std::shared_ptr<SbmTexture> expressiveTexture = nullptr;

	if (expressiveMesh) {
		for (auto& subMesh : expressiveMesh->subMeshList) {
			expressiveTexture = texManager.findTexture(subMesh->texName.c_str());
			if (expressiveTexture) {
				break;
			}
		}
		if (!expressiveTexture) {
			if (!expressiveMesh->dMeshStatic_p.empty()) {
				SrModel& model = expressiveMesh->dMeshStatic_p[0]->shape();
				for (auto& iter : model.mtlTextureNameMap) {
					std::string textureCandidate = iter.second;
					expressiveTexture = texManager.findTexture(textureCandidate.c_str());
					if (expressiveTexture)
						break;
				}
			}
		}
	}
	if (!expressiveTexture) {
		SmartBody::util::log("Could not find expressive diffuse texture in file %s.", expressiveShapeFile.c_str());
		ok = false;
	}

	// load the masked texture
	texManager.loadTexture(SbmTextureManager::TEXTURE_DIFFUSE, maskTextureFile.c_str(), maskTextureFile.c_str());
	auto maskedTexture = texManager.findTexture(maskTextureFile.c_str());
	if (!maskedTexture) {
		SmartBody::util::log("Could not load masked texture file %s.", maskTextureFile.c_str());
		ok = false;
	}

	// make sure the neutral and the expression have the same number of vertices

	int numEMeshes = 0;
	if (expressiveMesh)
		numEMeshes = expressiveMesh->dMeshStatic_p.size();
	int numNMeshes = 0;
	if (neutralMesh)
		numNMeshes = neutralMesh->dMeshStatic_p.size();
	if (numEMeshes != numNMeshes) {
		SmartBody::util::log("Neutral mesh %s has different number of meshes as expresive mesh %s (%d vs %d).", neutralShapeFile.c_str(), expressiveShapeFile.c_str(), numNMeshes, numEMeshes);
		ok = false;
	}

	int numEVertices = 0;
	int numNVertices = 0;
	if (expressiveMesh) {
		for (auto& i : expressiveMesh->dMeshStatic_p) {
			numEVertices += i->shape().V.size();
		}
	}
	if (neutralMesh) {
		for (auto& i : neutralMesh->dMeshStatic_p) {
			numNVertices += i->shape().V.size();
		}
	}
	if (numEVertices != numNVertices) {
		SmartBody::util::log("Neutral mesh %s has different number of vertices as expresive mesh %s (%d vs %d).", neutralShapeFile.c_str(), expressiveShapeFile.c_str(), numNMeshes, numEMeshes);
		ok = false;
	}

	// make sure the neutral, expression and mask textures are the same size
	int eHeight = 0;
	int eWidth = 0;
	if (expressiveTexture) {
		eHeight = expressiveTexture->getHeight();
		eWidth = expressiveTexture->getWidth();
	}

	int nHeight = 0;
	int nWidth = 0;
	if (neutralTexture) {
		nHeight = neutralTexture->getHeight();
		nWidth = neutralTexture->getWidth();
	}

	int mHeight = 0;
	int mWidth = 0;
	if (maskedTexture) {
		mHeight = maskedTexture->getHeight();
		mWidth = maskedTexture->getWidth();
	}


	if (eHeight != nHeight ||
		eHeight != mHeight ||
		nHeight != mHeight) {
		SmartBody::util::log("Texture heights don't match (%s: %d, %s: %d, %s: %d", neutralTextureFile.c_str(), nHeight, expressiveTextureFile.c_str(), eHeight, maskTextureFile.c_str(), mHeight);
		ok = false;
	}
	if (eWidth != nWidth ||
		eWidth != mWidth ||
		nWidth != mWidth) {
		SmartBody::util::log("Texture heights don't match (%s: %d, %s: %d, %s: %d", neutralTextureFile.c_str(), nWidth, expressiveTextureFile.c_str(), eWidth, maskTextureFile.c_str(), mWidth);
		ok = false;
	}

	if (!ok)
		return false;

	// create a template for the final mesh
	// load the expressive shape
	DeformableMesh* maskedMesh = nullptr;
	assets = _assetStore.loadAsset(expressiveShapeFile);
	for (auto& asset : assets) {
		auto* mesh = dynamic_cast<DeformableMesh*>(asset.get());
		if (mesh) {
			maskedMesh = mesh;
		}
	}


	int numMaskChannels = maskedTexture->getNumChannels();

	unsigned char* maskedBuffer = maskedTexture->getBuffer();
	for (size_t m = 0; m < maskedMesh->dMeshStatic_p.size(); m++) {
		SrModel& neutralModel = neutralMesh->dMeshStatic_p[m]->shape();
		SrModel& expressiveModel = expressiveMesh->dMeshStatic_p[m]->shape();
		SrModel& maskedModel = maskedMesh->dMeshStatic_p[m]->shape();


		std::set<int> vertexUsed;
		for (size_t f = 0; f < neutralModel.F.size(); f++) {
			SrVec3i& face = neutralModel.F[f];
			SrVec3i& faceTextureIndex = neutralModel.Ft[f];

			int curVertex = -1;
			int curTextureIndex = -1;
			for (int v = 0; v < 3; v++) {
				curVertex = face[v];
				auto iter = vertexUsed.find(curVertex);
				if (iter == vertexUsed.end()) {
					vertexUsed.insert(curVertex);
				} else {
					continue;
				}
				curTextureIndex = faceTextureIndex[v];

				// get the texture coordinates for the vertex
				SrPnt2& uv = neutralModel.T[curTextureIndex];
				// normalize uv to size of texture
				float cf = ((float) mWidth) * uv.x;
				float rf = ((float) mHeight) * uv.y;
				int rpos = (int) rf;
				int cpos = (int) cf;

				// find the greyscale color of the mask texture
				int position = rpos * mWidth * numMaskChannels + (cpos * numMaskChannels);
				SrColor maskColor;
				maskColor.r = maskedBuffer[position];
				maskColor.g = maskedBuffer[position + 1];
				maskColor.b = maskedBuffer[position + 2];
				if (numMaskChannels == 4)
					maskColor.a = maskedBuffer[position + 3];
				else
					maskColor.a = 255;

				// assuming masks are in greyscale, use only red channel to determine 'whiteness'
				float mcolor[4];
				maskColor.get(mcolor);
				float maskGreyAmount = mcolor[0];

				SrPnt& pointNeutral = neutralModel.V[curVertex];
				SrPnt& pointExpressive = expressiveModel.V[curVertex];
				SrPnt& pointMasked = maskedModel.V[curVertex];
				for (int n = 0; n < 3; n++)
					pointMasked[n] = maskGreyAmount * pointExpressive[n] + (1.0f - maskGreyAmount) * pointNeutral[n];

			}
		}

	}

	boost::filesystem::path p(outputMeshFile);
	std::string fileName = boost::filesystem::basename(p);
	std::string extension = boost::filesystem::extension(p);
	std::stringstream strstr;
	strstr << fileName << ".mtl";

	// save the final mesh
	for (auto& m : maskedMesh->dMeshStatic_p) {
		m->shape().export_obj(outputMeshFile.c_str(), strstr.str().c_str(), outputTextureFile.c_str());
	}

	// for each texture:
	// get the color of the mask
	// interpolate the vertex according to the mask color (black = neutral, white = expression, gray = interpolated)

	unsigned char* neutralBuffer = neutralTexture->getBuffer();
	unsigned char* expressiveBuffer = expressiveTexture->getBuffer();


	int numExpressiveChannels = expressiveTexture->getNumChannels();
	int numNeutralChannels = neutralTexture->getNumChannels();

	int maxSize = mWidth * mHeight * 4 * sizeof(unsigned char);
	auto* outData = new unsigned char[maxSize];
	auto* outputExpressiveTexture = new SbmTexture("maskedTexture");
	outputExpressiveTexture->setTextureSize(mWidth, mHeight, 4);
	for (int h = 0; h < mHeight; h++) {
		for (int w = 0; w < mWidth; w++) {
			int position = h * mWidth * 4 + w * 4;
			int flippedPosition = (mHeight - h - 1) * mWidth * 4 + w * 4;

			int position3 = h * mWidth * 3 + w * 3;
			int flippedPosition3 = (mHeight - h - 1) * mWidth * 3 + w * 3;

			SrColor maskColor;
			if (numMaskChannels == 4) {
				maskColor.r = maskedBuffer[position];
				maskColor.g = maskedBuffer[position + 1];
				maskColor.b = maskedBuffer[position + 2];
				maskColor.a = maskedBuffer[position + 3];
			} else if (numMaskChannels == 3) {
				maskColor.r = maskedBuffer[position3];
				maskColor.g = maskedBuffer[position3 + 1];
				maskColor.b = maskedBuffer[position3 + 2];
				maskColor.a = 255;
			}


			// assuming masks are in greyscale, use only red channel to determine 'whiteness'
			float mcolor[4];
			maskColor.get(mcolor);
			float maskGreyAmount = mcolor[0];

			bool stop = false;
			if (maskGreyAmount > 0.0f)
				stop = true;


			SrColor expressiveColor;
			if (numExpressiveChannels == 4) {
				expressiveColor.r = expressiveBuffer[position];
				expressiveColor.g = expressiveBuffer[position + 1];
				expressiveColor.b = expressiveBuffer[position + 2];
				expressiveColor.a = expressiveBuffer[position + 3];
			} else if (numExpressiveChannels == 3) {
				expressiveColor.r = expressiveBuffer[position3];
				expressiveColor.g = expressiveBuffer[position3 + 1];
				expressiveColor.b = expressiveBuffer[position3 + 2];
				expressiveColor.a = 255;
			}
			float ecolor[4];
			expressiveColor.get(ecolor);

			SrColor neutralColor;
			if (numNeutralChannels == 4) {
				neutralColor.r = neutralBuffer[position];
				neutralColor.g = neutralBuffer[position + 1];
				neutralColor.b = neutralBuffer[position + 2];
				neutralColor.a = neutralBuffer[position + 3];
			} else if (numNeutralChannels == 3) {
				neutralColor.r = neutralBuffer[position3];
				neutralColor.g = neutralBuffer[position3 + 1];
				neutralColor.b = neutralBuffer[position3 + 2];
				neutralColor.a = 255;
			}
			float ncolor[4];
			neutralColor.get(ncolor);

			// white = use expressive mask, black = use neutral mask, grey = interpolate values
			float finalRf = ecolor[0] * maskGreyAmount + (1.0f - maskGreyAmount) * ncolor[0];
			float finalGf = ecolor[1] * maskGreyAmount + (1.0f - maskGreyAmount) * ncolor[1];
			float finalBf = ecolor[2] * maskGreyAmount + (1.0f - maskGreyAmount) * ncolor[2];
			float finalAf = ecolor[3] * maskGreyAmount + (1.0f - maskGreyAmount) * ncolor[3];

			SrColor finalColor(finalRf, finalGf, finalBf, finalAf);

			// make sure to flip the data so that it gets written correctly
			outData[flippedPosition3] = finalColor.r;
			outData[flippedPosition3 + 1] = finalColor.g;
			outData[flippedPosition3 + 2] = finalColor.b;
			outData[flippedPosition3 + 3] = finalColor.a;
		}
	}
	outputExpressiveTexture->setBuffer(outData, maxSize);
	// save the texture here....
	//int ret = SOIL_save_image(outputTextureFile.c_str(), SOIL_SAVE_TYPE_BMP, mWidth, mHeight, 3, outData);
	int ret = stbi_write_bmp(outputTextureFile.c_str(), mWidth, mHeight, 3, outData);

	if (ret == 0) {
		SmartBody::util::log("Could not save masked texture image to file %s.", outputTextureFile.c_str());
	} else {
		SmartBody::util::log("Successfully saved masked texture image to file %s.", outputTextureFile.c_str());
	}

	return true;

}

bool SBRenderAssetManager::handlePenetrations(std::string deformableMesh, std::string baseModel, std::string penetratingModel, float offset, bool showVisualization)
{
	DeformableMesh* mesh = getDeformableMesh(deformableMesh);
	if (!mesh)
	{
		SmartBody::util::log("Cannot find mesh %s to handle penetrations into %s by %s.", deformableMesh.c_str(), baseModel.c_str(), penetratingModel.c_str());
		return false;
	}

	int baseModelIndex = -1;
	// find the base mesh
	for (size_t m = 0; m < mesh->dMeshStatic_p.size(); m++)
	{
		std::string modelName = (const char*) mesh->dMeshStatic_p[m]->shape().name;
		if (modelName == baseModel)
		{
			baseModelIndex = m;
			break;
		}
	}

	if (baseModelIndex == -1)
	{
		SmartBody::util::log("Cannot find base model %s in mesh %s to handle penetrations.", baseModel.c_str(), deformableMesh.c_str());
		return false;
	}

	int penetratingModelIndex = -1;
	// find the penetrating mesh
	for (size_t m = 0; m < mesh->dMeshStatic_p.size(); m++)
	{
		std::string modelName = (const char*) mesh->dMeshStatic_p[m]->shape().name;
		if (modelName == penetratingModel)
		{
			penetratingModelIndex = m;
			break;
		}
	}

	if (penetratingModelIndex == -1)
	{
		SmartBody::util::log("Cannot find penetrating model %s in mesh %s to handle penetrations.", penetratingModel.c_str(), deformableMesh.c_str());
		return false;
	}

	SrModel& baseShape = mesh->dMeshStatic_p[baseModelIndex]->shape();
	SrModel& penetratingShape = mesh->dMeshStatic_p[penetratingModelIndex]->shape();

	if (baseShape.F.empty())
	{
		SmartBody::util::log("Base shape %s has no faces. ", baseModel.c_str());
		return false;
	}

	if (baseShape.V.empty())
	{
		SmartBody::util::log("Base shape %s has no vertices. ", penetratingModel.c_str());
		return false;
	}
	if (showVisualization)
		_scene.run("import GUIInterface");

	// find the center of the base shape
	SrVec center;
	for (int v = 0; v < baseShape.V.size(); v++)
	{
		center += baseShape.V[v];

		if (showVisualization)
		{
			std::stringstream strstr3;
			strstr3 << "GUIInterface.addPoint('x" << v << "', SrVec(" << baseShape.V[v].x << ", " << baseShape.V[v].y << ", " << baseShape.V[v].z << "), SrVec(1, .75, .79), 5)";
			//SmartBody::util::log("Running: %s", strstr2.str().c_str());
			_scene.run(strstr3.str());
		}
	}
	center /= baseShape.V.size();

	float normalSize = (baseShape.V[0] - center).len() / 5.0f;

	std::vector<SrPnt> avgFaces;

	// draw the face normals
	for (int f = 0; f < baseShape.F.size(); f++)
	{
		SrVec3i& face = baseShape.F[f];
		// calculate the average position
		SrPnt avgPoint = (baseShape.V[face[0]] + baseShape.V[face[1]] + baseShape.V[face[2]]) / 3.0;
		avgFaces.emplace_back(avgPoint);

		if (false)
		{
			SrVec normal = baseShape.face_normal(f);
			SrVec endPoint = avgPoint + normalSize * normal;

			// draw a line from the vertex in the direction of the normal
			//SmartBody::util::log("Drawing point from %f %f %f to %f %f %f", baseShape.V[face[0]].x, baseShape.V[face[0]].y, baseShape.V[face[0]].z, endPoint.x, endPoint.y, endPoint.z);
			std::stringstream strstr4;
			strstr4 << "x = VecArray()";
			_scene.run(strstr4.str());
			std::stringstream strstr5;
			strstr5 << "x.append(SrVec(" << avgPoint.x << ", " << avgPoint.y << ", " << avgPoint.z << "))";
			_scene.run(strstr5.str());
			std::stringstream strstr6;
			strstr6 << "x.append(SrVec(" << endPoint.x << ", " << endPoint.y << ", " << endPoint.z << "))";
			_scene.run(strstr6.str());

			std::stringstream strstr7;
			strstr7 << "GUIInterface.addLine('l" << f << "', x, SrVec(0,1,0), 1)";
			_scene.run(strstr7.str());
		}

	}

	SrPnt point1(center.x, center.y, center.z);

	if (showVisualization)
	{
		//SmartBody::util::log("Center interior point is %f %f %f", center.x, center.y, center.z);
		std::stringstream strstr10;
		strstr10 << "GUIInterface.addPoint('center', SrVec(" << center.x << ", " << center.y << ", " << center.z << "), SrVec(0, 1, 0), 8)";
		//SmartBody::util::log("Running: %s", strstr10.str().c_str());
		_scene.run(strstr10.str());
	}

	// loop through the vertices
	for (int v = 0; v < penetratingShape.V.size(); v++)
	{
		SrVec& curV = penetratingShape.V[v];

		if (showVisualization)
		{
			std::stringstream strstr2;
			strstr2 << "GUIInterface.addPoint('x" << v << "', SrVec(" << curV.x << ", " << curV.y << ", " << curV.z << "), SrVec(1,1,0), 2)";
			//SmartBody::util::log("Running: %s", strstr2.str().c_str());
			_scene.run(strstr2.str());
		}
		// find the intersecting triangle
		SrPnt point2(curV.x, curV.y, curV.z);
		SrLine line(point1, point2);

		float t = 0;
		float a = 0;
		float b = 0;
		bool foundIntersection = false;
		int closestFace = -1;
		float closestDistance = 999999999.9f;
		SrPnt closestIntersectionPoint;
		float closestRDist = 0.0;
		for (int f = 0; f < baseShape.F.size(); f++)
		{
			SrVec3i& face = baseShape.F[f];

			// ignore any face towards the Z and below the center point
			if (avgFaces[f].y < center.y)
				continue;

			SrPnt intersectionPoint;
			float rdist = 0.0;
			SrVec faceNormal;
			bool doesIntersect = intersect3D_RayTriangle(line, baseShape.V[face[0]], baseShape.V[face[1]], baseShape.V[face[2]], intersectionPoint, rdist, faceNormal);
			if (showVisualization && v == 0)
			{
				SrVec endNormal = avgFaces[f] + normalSize * faceNormal;
				// draw a line from the vertex in the direction of the normal
				//SmartBody::util::log("Drawing point from %f %f %f to %f %f %f", baseShape.V[face[0]].x, baseShape.V[face[0]].y, baseShape.V[face[0]].z, endPoint.x, endPoint.y, endPoint.z);
				std::stringstream strstr4;
				strstr4 << "x = VecArray()";
				_scene.run(strstr4.str());
				std::stringstream strstr5;
				strstr5 << "x.append(SrVec(" << avgFaces[f].x << ", " << avgFaces[f].y << ", " << avgFaces[f].z << "))";
				_scene.run(strstr5.str());
				std::stringstream strstr6;
				strstr6 << "x.append(SrVec(" << endNormal.x << ", " << endNormal.y << ", " << endNormal.z << "))";
				_scene.run(strstr6.str());

				std::stringstream strstr7;
				strstr7 << "GUIInterface.addLine('fn" << f << "', x, SrVec(0,1,0), 1)";
				_scene.run(strstr7.str());
			}
			//bool doesIntersect = line.intersects_triangle(baseShape.V[face[0]], baseShape.V[face[1]], baseShape.V[face[2]], t, a, b);
			if (doesIntersect)
			{
				//SmartBody::util::log("Vertex %d intersects with face %d with parameters %f %f %f", v, f, t, a, b);
				//SrPnt intersectionPoint = baseShape.V[face[0]] * (1.f - a - b) + baseShape.V[face[1]] * a + baseShape.V[face[2]] * b;


				// calculate distance to intersection point
				SrPnt distance = point2 - intersectionPoint;
				float currentLength = distance.len();
				if (currentLength < closestDistance)
				{
					closestDistance = currentLength;
					closestFace = f;
					closestIntersectionPoint = intersectionPoint;
					closestRDist = rdist;
				}

				if (showVisualization)
				{
					std::stringstream strstr;
					strstr << "GUIInterface.addPoint('" << v << "_" << "', SrVec(" << intersectionPoint[0] << ", " << intersectionPoint[1] << ", " << intersectionPoint[2] << "), SrVec(0,0,1), 4)";
					SmartBody::util::log("Running: %s", strstr.str().c_str());
					_scene.run(strstr.str());
				}

				break;
			}
		}

		if (closestFace != -1 && closestRDist > 1.0)
		{

			SrVec finalLocation = closestIntersectionPoint + (closestIntersectionPoint - point1) * offset;
			mesh->dMeshStatic_p[penetratingModelIndex]->shape().V[v].x = finalLocation[0];
			mesh->dMeshStatic_p[penetratingModelIndex]->shape().V[v].y = finalLocation[1];
			mesh->dMeshStatic_p[penetratingModelIndex]->shape().V[v].z = finalLocation[2];

			mesh->dMeshDynamic_p[penetratingModelIndex]->shape().V[v].x = finalLocation[0];
			mesh->dMeshDynamic_p[penetratingModelIndex]->shape().V[v].y = finalLocation[1];
			mesh->dMeshDynamic_p[penetratingModelIndex]->shape().V[v].z = finalLocation[2];


			if (showVisualization)
			{
				// draw a line from the original vertex to the intersection point
				std::stringstream strstr4;
				strstr4 << "x = VecArray()";
				_scene.run(strstr4.str());
				std::stringstream strstr5;
				strstr5 << "x.append(SrVec(" << closestIntersectionPoint[0] << ", " << closestIntersectionPoint[1] << ", " << closestIntersectionPoint[2] << "))";
				_scene.run(strstr5.str());
				std::stringstream strstr6;
				strstr6 << "x.append(SrVec(" << curV.x << ", " << curV.y << ", " << curV.z << "))";
				_scene.run(strstr6.str());

				std::stringstream strstr7;
				strstr7 << "GUIInterface.addLine('l" << v << "', x, SrVec(0,1,0), 1)";
				_scene.run(strstr7.str());
			}


		}

	}
	auto* gpuMesh = (SbmDeformableMeshGPU*)mesh;
	gpuMesh->rebuildVertexBuffer(true);
	gpuMesh->rebuildVertexBufferGPU(true); // rebuild vertex buffers


	return true;
}


bool SBRenderAssetManager::addModelToMesh(std::string templateMeshName, std::string modelFile, std::string newModelName, std::string rigidBindJoint, std::string bindPoseCopySubMeshName)
{
	DeformableMesh* mesh = getDeformableMesh(templateMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Cannot find template mesh %s in order to add mesh via file %s.", templateMeshName.c_str(), modelFile.c_str());
		return false;
	}

	// load the new model
	SrModel model;
	bool ok = model.import_ply(modelFile.c_str());
	if (!ok)
	{
		SmartBody::util::log("Cannot find new model %s to add to mesh %s.", modelFile.c_str(), templateMeshName.c_str());
		return false;
	}

	boost::intrusive_ptr<SrSnModel> modelSrSn(new SrSnModel());
	modelSrSn->shape(model);
	modelSrSn->shape().name = newModelName.c_str();

	// does a mesh with this name already exist? If so, replace it
	bool found = false;
	for (size_t m = 0; m < mesh->dMeshStatic_p.size(); m++)
	{
		auto& staticModel = mesh->dMeshStatic_p[m];
		SrModel& s = staticModel->shape();
		std::string curShapeName = (const char*) s.name;
		SmartBody::util::log("Comparing mesh names: %s to  %s", curShapeName.c_str(), newModelName.c_str());

		if (curShapeName == newModelName)
		{
			// model already exists, replace it
			auto& curModel = mesh->dMeshStatic_p[m];
			mesh->dMeshStatic_p[m] = modelSrSn;
			mesh->dMeshStatic_p[m]->changed(true);
			auto& curDynamicModel = mesh->dMeshDynamic_p[m];
			mesh->dMeshDynamic_p[m] = modelSrSn;
			mesh->dMeshDynamic_p[m]->changed(true);
			found = true;
			SmartBody::util::log("Replacing existing mesh %s", newModelName.c_str());
		}

	}

	if (!found)
	{
		SmartBody::util::log("Adding new mesh %s", newModelName.c_str());
		mesh->dMeshStatic_p.emplace_back(modelSrSn);
		mesh->dMeshDynamic_p.emplace_back(modelSrSn);
	}

	if (mesh->skinWeights.empty())
	{
		SmartBody::util::log("Mesh %s has no skin weights, model %s will be added but not skinned.", templateMeshName.c_str(), modelFile.c_str());

		return false;
	}

	// copy the skin weights from the proper mesh using a joint, bind matrix
	auto& existingSkinWeights = mesh->skinWeights[0];

	auto* modelSkin = new SkinWeight();

	SrMat existingBindShapeMat = existingSkinWeights->bindShapeMat;
	SrMat modelBindPose;
	const std::string& jointName = rigidBindJoint;

	for (unsigned int i = 0; i < existingSkinWeights->infJointName.size(); i++)
	{
		if (existingSkinWeights->infJointName[i] == jointName)
		{
			modelBindPose = existingSkinWeights->bindPoseMat[i];
			break;
		}
	}

	modelSkin->bindShapeMat = existingBindShapeMat;
	modelSkin->bindPoseMat.emplace_back(modelBindPose);
	modelSkin->infJointName.emplace_back(rigidBindJoint);
	modelSkin->sourceMesh = newModelName;
	modelSkin->bindWeight.emplace_back(1.0f);
	for (unsigned int i = 0; i < model.V.size(); i++)
	{
		modelSkin->jointNameIndex.emplace_back(0);
		modelSkin->numInfJoints.emplace_back(1);
		modelSkin->weightIndex.emplace_back(0);
	}
	mesh->skinWeights.emplace_back(modelSkin);
	return true;
}

bool SBRenderAssetManager::addBlendshapeToModel(std::string templateMeshName, std::string modelFile, std::string shapeName, std::string submeshName)
{
	auto mesh = getDeformableMesh(templateMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Cannot find template mesh %s in order to add blendshape via file %s.", templateMeshName.c_str(), modelFile.c_str());
		return false;
	}

	// load the new model
	SrModel blendshapeModel;
	bool ok = blendshapeModel.import_ply(modelFile.c_str());
	if (!ok)
	{
		SmartBody::util::log("Cannot find new model %s to add as blendshape to mesh %s.", modelFile.c_str(), templateMeshName.c_str());
		return false;
	}

	for (size_t m = 0; m < mesh->dMeshStatic_p.size(); m++)
	{
		auto& staticModel = mesh->dMeshStatic_p[m];
		SrModel& s = staticModel->shape();
		std::string curShapeName = (const char*) s.name;
		if (curShapeName == submeshName)
		{
			// make sure that the blendshape matches the existing model
			if (s.V.size() != blendshapeModel.V.size())
			{
				SmartBody::util::log("Submesh %s has %d vertice, candidate blendshape %s has %d vertices, so cannot create the blendshape.", shapeName.c_str(), s.V.size(), modelFile.c_str(), blendshapeModel.V.size());
				return false;
			}
			// loop through the morph target to see if this controller already exists
			auto mapIter = mesh->blendShapeMap.find(submeshName);
			if (mapIter == mesh->blendShapeMap.end())
			{
				// no controller for this exists yet, set one up
				boost::intrusive_ptr<SrSnModel> baseModel(new SrSnModel());
				baseModel->shape(blendshapeModel);
				baseModel->shape().name = shapeName.c_str();
				baseModel->changed(true);
				baseModel->visible(false);

				// since this is the base model, overwrite the submesh's V and N
				mesh->dMeshStatic_p[m]->shape().V = baseModel->shape().V;
				mesh->dMeshStatic_p[m]->shape().N = baseModel->shape().N;

				std::vector<boost::intrusive_ptr<SrSnModel>> modelList;
				modelList.emplace_back(baseModel);
				mesh->blendShapeMap.emplace(submeshName, modelList);
				std::vector<std::string> morphTargetList;
				morphTargetList.emplace_back(shapeName);
				mesh->morphTargets.insert(std::pair<std::string, std::vector<std::string> >(submeshName, morphTargetList));
			}
			else
			{
				// controller exists, see if the shape also exists
				bool found = false;
				auto& existingShapeModels = (*mapIter).second;
				for (auto & existingShapeModel : existingShapeModels)
				{
					std::string modelName = (const char*) existingShapeModel->shape().name;
					if (modelName == shapeName)
					{
						// shape already exists
						// replace it if the vertices match
						existingShapeModel->shape().V = blendshapeModel.V;
						existingShapeModel->shape().N = blendshapeModel.N;

						found = true;
						break;
					}
				}
				if (!found)
				{
					boost::intrusive_ptr<SrSnModel> baseModel(new SrSnModel());
					baseModel->shape(blendshapeModel);
					baseModel->shape().name = shapeName.c_str();
					baseModel->changed(true);
					baseModel->visible(false);

					existingShapeModels.emplace_back(baseModel);
					auto morphNameIter = mesh->morphTargets.find(submeshName);
					if (morphNameIter == mesh->morphTargets.end())
					{
						SmartBody::util::log("Couldn't find controller name %s in morph target list, strange...", submeshName.c_str());
						return false;
					}
					(*morphNameIter).second.emplace_back(shapeName);
				}

			}

		}
	}
	return true;
}



}