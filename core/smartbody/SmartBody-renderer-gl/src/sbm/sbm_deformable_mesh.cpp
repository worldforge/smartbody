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


#include "sbm_deformable_mesh.h"
#include <google/protobuf/io/coded_stream.h>
#include <sb/SBTypes.h>

#if !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
#include "GL/glew.h"
#endif

#include "sbm/GPU/SbmBlendFace.h"

#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"
#include <sr/sr_sn_group.h>
#include <sr/sr_random.h>
#include "gwiz_math.h"
#include <sbm/GPU/SbmTexture.h>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/algorithm/string.hpp>
#include <protocols/sbmesh.pb.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <sbm/ParserCOLLADAFast.h>
#include <sbm/ParserOpenCOLLADA.h>





#define TEST_HAIR_RENDER 1

typedef std::pair<int,int> IntPair;
typedef std::pair<int,float> IntFloatPair;
static bool intFloatComp(const IntFloatPair& p1, const IntFloatPair& p2)
{
	return (p1.second > p2.second);
}

SkinWeight::SkinWeight() = default;

SkinWeight::~SkinWeight() = default;

void SkinWeight::normalizeWeights()
{
	int idx = 0;
	for (int nJoint : numInfJoints)
	{
			float wTotal = 0.f;
		for (int k=0;k<nJoint;k++)
		{
			int widx = weightIndex[idx+k];
			wTotal += bindWeight[widx];			
		}

		for (int k=0;k<nJoint;k++)
		{
			int widx = weightIndex[idx+k];
			bindWeight[widx] /= wTotal;			
		}

		idx+= nJoint;
	}
}

void SkinWeight::copyWeights(SkinWeight* copy, const std::string& morphName)
{
	infJointName.clear();
	for (size_t jn = 0; jn < copy->infJointName.size(); jn++)
	{
		infJointName.emplace_back(copy->infJointName[jn]);
	}

	infJoint.clear();
	for (size_t j = 0; j < copy->infJoint.size(); j++)
	{
		infJoint.emplace_back(copy->infJoint[j]);
	}

	bindWeight.clear();
	for (size_t bw = 0; bw < copy->bindWeight.size(); bw++)
	{
		bindWeight.emplace_back(copy->bindWeight[bw]);
	}

	bindPoseMat.clear();
	for (size_t bp = 0; bp < copy->bindPoseMat.size(); bp++)
	{
		bindPoseMat.emplace_back(copy->bindPoseMat[bp]);
	}

	bindShapeMat = copy->bindShapeMat;
	sourceMesh = morphName; // set the name to the input parameter, instead of the source name (i.e. copy->sourceMesh)

	numInfJoints.clear();
	for (size_t n = 0; n < copy->numInfJoints.size(); n++)
	{
		numInfJoints.emplace_back(copy->numInfJoints[n]);
	}

	weightIndex.clear();
	for (size_t wi = 0; wi < copy->weightIndex.size(); wi++)
	{
		weightIndex.emplace_back(copy->weightIndex[wi]);
	}

	jointNameIndex.clear();
	for (size_t jni = 0; jni < copy->jointNameIndex.size(); jni++)
	{
		jointNameIndex.emplace_back(copy->jointNameIndex[jni]);
	}
}

SBAPI void SkinWeight::initWeights(std::string srcMesh, std::vector<SrVec4i>& boneID, std::vector<SrVec4>& boneWeights, std::vector<std::string>& boneJointNameList, std::vector<SrMat>& bindPoseMatList )
{
	sourceMesh = srcMesh;
	bindShapeMat = SrMat::id;

	infJointName.clear();
	infJoint.clear();
	bindPoseMat.clear();
	for (unsigned int i=0;i<boneJointNameList.size();i++)
	{
		bindPoseMat.emplace_back(bindPoseMatList[i]);
		infJointName.emplace_back(boneJointNameList[i]);
	}

	numInfJoints.clear();
	weightIndex.clear();
	bindWeight.clear();
	jointNameIndex.clear();
	
	int icount = 0;
	for (unsigned int i=0;i<boneID.size();i++)
	{
		numInfJoints.emplace_back(4);
		for (int k=0;k<4;k++)
		{
			weightIndex.emplace_back(icount++);
			jointNameIndex.emplace_back(boneID[i][k]);
			bindWeight.emplace_back(boneWeights[i][k]);
		}
	}
}

SBAPI void SkinWeight::addWeight( SkinWeight* weight )
{
	std::map<std::string, size_t> newJointMap;
	// initialize with original map
	for (size_t i=0;i<infJointName.size();i++)
	{
		newJointMap[infJointName[i]] = i;
	}
	// add new joints from input weight
	for (size_t i=0;i<weight->infJointName.size();i++)
	{
		std::string jointName = weight->infJointName[i];
		if (newJointMap.find(jointName) == newJointMap.end()) // add new joint
		{
			newJointMap[jointName] = infJointName.size();
			// update new joint name and bind pose matrix
			infJointName.emplace_back(jointName);
			bindPoseMat.emplace_back(weight->bindPoseMat[i]);
		}
	}
	// append bind weights
	size_t weightIdxOffset = bindWeight.size();
	bindWeight.insert(bindWeight.end(), weight->bindWeight.begin(), weight->bindWeight.end());
	numInfJoints.insert(numInfJoints.end(), weight->numInfJoints.begin(), weight->numInfJoints.end());
	for (unsigned int i=0;i<weight->weightIndex.size();i++)
		weightIndex.emplace_back(weight->weightIndex[i] + weightIdxOffset);
	for (unsigned int i=0;i<weight->jointNameIndex.size();i++)
	{
		jointNameIndex.emplace_back(newJointMap[weight->infJointName[weight->jointNameIndex[i]]]);
	}
}

SBAPI void SkinWeight::mergeRedundantWeight( std::vector<int>& vtxIdxMap )
{
	if (vtxIdxMap.size() != numInfJoints.size())
	{
		SmartBody::util::log("Warning!! mergeRedundantWeight() : vtxIdxMap.size() != numInfJoint.size()");
		return;
	}
	
	std::vector<unsigned int>	newNumInfJoints;	// number of influenced joints for very vertex
	std::vector<unsigned int>	newWeightIdx;	// looking up the weight according to this index
	std::vector<unsigned int>	newJointNameIdx;	// looking up the joint name according to this index

	int icount = 0;	
	for (unsigned int i=0;i<vtxIdxMap.size();i++)
	{
		int numJoint = numInfJoints[i];
		unsigned int newIdx = vtxIdxMap[i];
		if (newIdx >= newNumInfJoints.size())
		{
			newNumInfJoints.emplace_back(numJoint);
			for (int k=0;k<numJoint;k++)
			{
				newWeightIdx.emplace_back(weightIndex[icount+k]);
				newJointNameIdx.emplace_back(jointNameIndex[icount+k]);
			}
		}
		icount += numJoint;
	}

	// remove unuse binding weights
	/* 
	std::vector<bool> useBindWeight(bindWeight.size(), false);
	for (unsigned int i=0;i<newWeightIdx.size();i++)
		useBindWeight[newWeightIdx[i]] = true;

	std::vector<int> weightIdxMap(bindWeight.size(),-1);
	std::vector<float> newBindWeight;
	int idx = 0;
	for (unsigned int i=0;i<useBindWeight.size();i++)
	{
		if (useBindWeight[i])
		{
			newBindWeight.emplace_back(bindWeight[i]);
			weightIdxMap[i] = idx++;
		}
	}
	for (unsigned int i=0;i<newWeightIdx.size();i++)
	{
		newWeightIdx[i] = weightIdxMap[newWeightIdx[i]];
	}
	
	bindWeight = newBindWeight;
	*/

	numInfJoints = newNumInfJoints;
	weightIndex = newWeightIdx;
	jointNameIndex = newJointNameIdx;
}

SBAPI void SkinWeight::buildSkinWeightBuf()
{
	size_t numSkinVtxs = numInfJoints.size();
	boneIDs.resize(numSkinVtxs);
	boneWeights.resize(numSkinVtxs);
	int globalCounter = 0;
	for (unsigned int i = 0; i < numInfJoints.size() ; i++)
	{
		unsigned int numOfInfJoints = numInfJoints[i];
		
		boneIDs[i] = SrVec4i(0,0,0,0);
		boneWeights[i] = SrVec4(0,0,0,0);
	
		std::vector<IntFloatPair> weightList;
		if (infJointName.size() > 0)
		{
			for (int j = 0; j < numOfInfJoints; j++)
			{
				float jointWeight = bindWeight[weightIndex[globalCounter]];
				int    jointIndex = jointNameIndex[globalCounter];
				weightList.emplace_back(IntFloatPair(jointIndex,jointWeight));
				globalCounter ++;									
			}
		}
		std::sort(weightList.begin(),weightList.end(),intFloatComp); // sort for minimum weight
		int numWeight = numOfInfJoints > 4 ? 4 : numOfInfJoints;
		float weightSum = 0.f;
		SrVec skinColor;
		for (int j=0;j<numWeight;j++)
		{
			if (j >= (int)weightList.size())
				continue;
			IntFloatPair& w = weightList[j];

			if ( j < 4)
			{
				boneIDs[i][j] = w.first;
				boneWeights[i][j] = w.second;
				weightSum += w.second;
			}
		}
		if (weightSum != 0)
		{
			for (int j=0;j<4;j++)
			{
				boneWeights[i][j] /= weightSum;
			}
		}
	}
}

std::vector<std::string> SkinWeight::getInfluenceJointNames()
{
	return this->infJointName;
}

int SkinWeight::getNumVertices()
{
	return this->numInfJoints.size();
}

std::vector<int> SkinWeight::getInfluenceJoints(int infIndex)
{
	if (infIndex >= numInfJoints.size() || infIndex < 0)
		return std::vector<int>();

	if (indexCache.size() != numInfJoints.size())
		createCache();

	int numInfluences = numInfJoints[infIndex];
	int index = indexCache[infIndex];

	std::vector<int> influenceJoints;
	for (int w = 0; w < numInfluences; w++)
	{
		influenceJoints.emplace_back(this->jointNameIndex[index + w]);
	}

	return influenceJoints;
}

std::vector<float> SkinWeight::getInfluenceWeights(int infIndex)
{
	if (infIndex >= numInfJoints.size() || infIndex < 0)
		return std::vector<float>();

	if (indexCache.size() != numInfJoints.size())
		createCache();

	int numInfluences = numInfJoints[infIndex];
	int index = indexCache[infIndex];

	std::vector<float> influenceWeights;
	for (int w = 0; w < numInfluences; w++)
	{
		influenceWeights.emplace_back(bindWeight[index + w]);
	}

	return influenceWeights;
}

std::vector<int> SkinWeight::getInfluencesByJointIndex(int index)
{
	this->buildSkinWeightBuf();

	std::vector<int> vertices;

	for (size_t id = 0; id < boneIDs.size(); id++)
	{
		SrVec4i& influence = boneIDs[id];
		for (int x = 0; x < 4; x++)
		{
			if (influence[x] == index)
			{
				vertices.emplace_back(id);
				break;
			}
		}
	}

	return vertices;
}

SrMat SkinWeight::getBindShape()
{
	return this->bindShapeMat;
}

SrMat SkinWeight::getBindJoint(int jointIndex)
{
	return this->bindPoseMat[jointIndex];
}

void SkinWeight::createCache()
{
	indexCache.clear();

	int count = 0;

	for (size_t i = 0; i < numInfJoints.size(); i++)
	{
		indexCache.emplace_back(count);
		count += numInfJoints[i];
	}
}

DeformableMesh::DeformableMesh() : SBAsset()
{	
	initSkinnedVertexBuffer = false;
	initStaticVertexBuffer = false;
	hasVertexColor = false;	
	hasTexCoord = false;
	skeleton = new SmartBody::SBSkeleton();
	skeleton->ref();
}

DeformableMesh::~DeformableMesh() 
{
  //SmartBody::util::log("Delete Deformable Mesh '%s'", this->getName().c_str());
	skeleton->unref();

  //SmartBody::util::log("Dynamic Mesh size = %d, static Mesh size = %d", dMeshDynamic_p.size(), dMeshStatic_p.size());
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
   // SmartBody::util::log("Dynamic Mesh '%s' ref count = %d", (const char*)dMeshDynamic_p[i]->shape().name, dMeshDynamic_p[i]->getref());
		dMeshDynamic_p[i]->unref();
		//delete dMeshDynamic_p[i];
	}
	dMeshDynamic_p.clear();
	for (unsigned int i = 0; i < dMeshStatic_p.size(); i++)
	{
   // SmartBody::util::log("Static Mesh '%s' ref count = %d", (const char*)dMeshStatic_p[i]->shape().name, dMeshStatic_p[i]->getref());
		dMeshStatic_p[i]->unref();
		//delete dMeshStatic_p[i];
	}
	dMeshStatic_p.clear();
  
  std::map<std::string, std::vector<SrSnModel*> >::iterator it;
  for (it  = blendShapeMap.begin();
       it != blendShapeMap.end();
       it++)
  {
    std::vector<SrSnModel*>& blendShapes = it->second;
    for (unsigned int i=0;i<blendShapes.size();i++)
      blendShapes[i]->unref();
    blendShapes.clear();
  }
  blendShapeMap.clear();
  
	for (unsigned int i = 0; i < skinWeights.size(); i++)
	{
		SkinWeight* sw = skinWeights[i];
		if (sw)
		{
			delete sw;
			sw = nullptr;
		}
	}	
	skinWeights.clear();
  
  SbmTextureManager& texManager = SbmTextureManager::singleton();
	for (size_t i = 0; i < subMeshList.size(); ++i)
	{
		if (subMeshList[i])
    {
      texManager.deleteTexture(SbmTextureManager::TEXTURE_DIFFUSE, subMeshList[i]->texName.c_str());
      texManager.deleteTexture(SbmTextureManager::TEXTURE_NORMALMAP, subMeshList[i]->normalMapName.c_str());
      texManager.deleteTexture(SbmTextureManager::TEXTURE_SPECULARMAP, subMeshList[i]->specularMapName.c_str());
			delete subMeshList[i];
    }
	}
	subMeshList.clear();
}


SBAPI void DeformableMesh::initDeformMesh( std::vector<SrModel*>& meshModelVec )
{
	dMeshStatic_p.clear();
	dMeshDynamic_p.clear();
	for (unsigned int i = 0; i < meshModelVec.size(); i++)
	{
		if (meshModelVec[i]->Fn.size() == 0)
		{
			meshModelVec[i]->computeNormals();
		}

		SrSnModel* srSnModelStatic = new SrSnModel();
		srSnModelStatic->shape(*meshModelVec[i]);
		srSnModelStatic->shape().name = meshModelVec[i]->name;

		SrSnModel* srSnModelDynamic = new SrSnModel();
		srSnModelDynamic->shape(*meshModelVec[i]);
		srSnModelDynamic->changed(true);
		srSnModelDynamic->visible(false);
		srSnModelDynamic->shape().name = meshModelVec[i]->name;

		dMeshStatic_p.emplace_back(srSnModelStatic);
		//srSnModelStatic->ref();
		dMeshDynamic_p.emplace_back(srSnModelDynamic);
		//srSnModelDynamic->ref();
	}
	
}
SkinWeight* DeformableMesh::getSkinWeightIndex(int index)
{
	if (index < 0 || index >= skinWeights.size())
		return nullptr;

	return skinWeights[index];
}

SkinWeight* DeformableMesh::getSkinWeight(const std::string& skinSourceName)
{
	for (unsigned int i = 0; i < skinWeights.size(); i++)
	{
		if (skinSourceName == skinWeights[i]->sourceMesh)
			return skinWeights[i];
	}
	return nullptr;
}

int DeformableMesh::getValidSkinMesh(const std::string& meshName)
{
	std::map<std::string, std::vector<std::string> >::iterator iter = morphTargets.find(meshName);
	if (iter == morphTargets.end())
    return getMesh(meshName);

  for (unsigned int morphCounter = 0;
       morphCounter < iter->second.size();
       ++morphCounter)
  {
    int pos = getMesh(iter->second[morphCounter]);
    if (pos != -1)
    {
      return pos;
    }
  }
	
	return -1;
}

int	DeformableMesh::getMesh(const std::string& meshName)
{
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		if (dMeshDynamic_p[i]->shape().name == meshName.c_str())
			return i;
	}
	return -1;
}


SBAPI void DeformableMesh::updateVertexBuffer()
{
	int iVtx = 0;
	for (unsigned int c=0;c<meshIndexList.size();c++)
	{
		unsigned int pos = meshIndexList[c];
		int globalCounter = 0;
		SrSnModel* dMeshStatic = dMeshStatic_p[pos];
		SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];
		dMeshDynamic->visible(false);
		SrMat bindShapeMat;
		SkinWeight* skinWeight = nullptr;
		
		if (skinWeights.size() > pos)
		{
			skinWeight = skinWeights[pos];
			bindShapeMat = skinWeight->bindShapeMat;
		}
		for (unsigned int i = 0; i < dMeshStatic->shape().V.size(); i++)
		{
      SrVec value = dMeshStatic->shape().V[i] * bindShapeMat;
		
			if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
			{
				const std::vector<int>& idxMap(vtxNewVtxIdxMap[iVtx]);
				// copy related vtx components 
				for (unsigned int k=0;k<idxMap.size();k++)
				{
					posBuf[idxMap[k]] = value;
				}
			}
      posBuf[iVtx++] = value;
		}
	}

}


bool DeformableMesh::buildBlendShapes()
{
	//SmartBody::util::log("Start build blendshapes");
	if (blendShapeMap.size() == 0) return false;
	blendShapeNewVtxIdxMap.clear();
	std::string neutralName = blendShapeMap.begin()->first;
	int meshIdx = getMesh(neutralName);
	//SmartBody::util::log("Start build blendshapes #1");
	SrModel& neutralMesh = getStaticModel(meshIdx);
	// build Kd-tree from posBuf
	unsigned int numKNN = 10;
	MeshPointCloud posCloud;
	posCloud.pts = posBuf;
	std::vector<float> knnPtDists(numKNN);
	std::vector<size_t>   knnPtIdxs(numKNN);
	//SmartBody::util::log("Start build blendshapes #2");
	
	MeshKDTree* meshKDTree = new MeshKDTree(3, posCloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
	meshKDTree->buildIndex();
	// for each vertex in neutral blendshape model, search for closest vertex in Kd-tree
	// if distance is zero, put vertex index in the blendShapeNewVtxIdxMap 
	SmartBody::util::log("Start build blendshapes #3");
// 	for (unsigned int i=0;i<neutralMesh.V.size(); i++)
// 	{
// 		SrVec inPt = neutralMesh.V[i];
// 		meshKDTree->knnSearch(inPt.data(), numKNN, knnPtIdxs.data(), knnPtDists.data());
// 		for (unsigned int k=0;k<numKNN;k++)
// 		{
// 			if (knnPtDists[k] < 1e-30) // almost identical vertex
// 			{
// 				if (blendShapeNewVtxIdxMap.find(i) == blendShapeNewVtxIdxMap.end())
// 				{
// 					blendShapeNewVtxIdxMap[i] = std::vector<int>();
// 				}
// 				blendShapeNewVtxIdxMap[i].emplace_back(knnPtIdxs[k]);
// 			}
// 		}
// 	}
	SmartBody::util::log("Start build blendshapes #4");
   delete meshKDTree;
   
	//SmartBody::util::log("Start build blendshapes #5");
	return true;
}


void DeformableMesh::rebuildVertexBuffer( bool rebuild )
{
	if (rebuild)
	{
		initSkinnedVertexBuffer = false;
		buildSkinnedVertexBuffer();
	}
}


bool DeformableMesh::buildSkinnedVertexBuffer()
{	
	//SmartBody::util::log(" DeformableMesh::buildSkinnedVertexBuffer()");
	if (initSkinnedVertexBuffer) return true;

	if (initStaticVertexBuffer && !isSkinnedMesh()) return true;

	bool buildSkinnedBuffer = isSkinnedMesh();

	SmartBody::util::log("Build Skinned Buffer for Mesh '%s'", this->getName().c_str());
	int nTotalVtxs=0, nTotalTris = 0, nTotalBones = 0;	
	std::vector<std::set<IntPair> > vtxNormalIdxMap;
	std::vector<std::set<int> > vtxMaterialIdxMap;
	std::map<IntPair,int> ntNewVtxIdxMap;	
	
	std::map<int,std::vector<int> > meshSubsetMap;	

	SrColor colorArray[6] = { SrColor::blue, SrColor::red, SrColor::green, SrColor::magenta, SrColor::gray, SrColor::yellow};
	// setup deformable mesh color	
	int nMaterial = 1;	
	std::vector<SrMaterial> allMatList;
	//std::vector<SrColor> allMatList;
	std::vector<std::string> allTexNameList;
	std::vector<std::string> allMatNameList;
	std::vector<std::string> allNormalTexNameList;
	std::vector<std::string> allSpecularTexNameList;	
	std::map<std::string, std::string> allMaterialMeshMap;
	SrMaterial defaultMaterial;
	defaultMaterial.diffuse = SrColor(0.6f,0.6f,0.6f);//SrColor::gray;	
	defaultMaterial.specular = SrColor(101,101,101);//SrColor::gray;
	defaultMaterial.shininess = 29;

	allMatList.emplace_back(defaultMaterial);
	allMatNameList.emplace_back("defaultMaterial");
	allTexNameList.emplace_back("");
	allNormalTexNameList.emplace_back("");
	allSpecularTexNameList.emplace_back("");
	meshSubsetMap[0] = std::vector<int>(); // default material group : gray color

	//std::vector<int> meshIndexList;
	std::vector<SkinWeight*> skinWeightList;
	meshIndexList.clear();
	boneJointIdxMap.clear();
	bindPoseMatList.clear();
	//SmartBody::util::log("dynamic mesh size = %d, skin weight size = %d",dMeshDynamic_p.size(), skinWeights.size());
	if (buildSkinnedBuffer)
	{
		for (unsigned int skinCounter = 0; skinCounter < skinWeights.size(); skinCounter++)
		{
			SkinWeight* skinWeight = skinWeights[skinCounter];		
			int pos;
			int globalCounter = 0;
			//pos = this->getMesh(skinWeight->sourceMesh);
			pos = this->getValidSkinMesh(skinWeight->sourceMesh);
			//SmartBody::util::log("skinWeight->sourceMesh = %s, pos = %d", skinWeight->sourceMesh.c_str(), pos);
			if (pos != -1)
			{
				meshIndexList.emplace_back(pos);
				skinWeightList.emplace_back(skinWeight);
				for (size_t j = 0; j < skinWeight->infJointName.size(); j++)
				{
					std::string& jointName = skinWeight->infJointName[j];
					SkJoint* curJoint = skeleton->search_joint(jointName.c_str());		
					skinWeight->infJoint.emplace_back(curJoint); // NOTE: If joints are added/removed during runtime, this list will contain stale data
				}

				for (unsigned int k=0;k<skinWeight->infJointName.size();k++)
				{
					std::string& jointName = skinWeight->infJointName[k];
					SkJoint* curJoint = skinWeight->infJoint[k];
					if (boneJointIdxMap.find(jointName) == boneJointIdxMap.end()) // new joint
					{
						//SmartBody::util::log("BoneJointIdxMap, joint name = %s, idx = %d", jointName.c_str(), nTotalBones);
						boneJointIdxMap[jointName] = nTotalBones++;		
						boneJointList.emplace_back(curJoint);
						boneJointNameList.emplace_back(jointName);
						//bindPoseMatList.emplace_back(skinWeight->bindShapeMat*skinWeight->bindPoseMat[k]);
						bindPoseMatList.emplace_back(skinWeight->bindPoseMat[k]);
					}
				}
			}	
		}
	}
	else
	{
		for (size_t i=0;i<dMeshDynamic_p.size();i++)
			meshIndexList.emplace_back(i);
	}

	//SmartBody::util::log("meshIndexList.size() = %d \n", meshIndexList.size());
	for (unsigned int i=0;i<meshIndexList.size();i++)
	{
		int pos = meshIndexList[i];
		SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];
		SrSnModel* dMeshStatic = dMeshStatic_p[pos];
		dMeshStatic->shape().computeTangentBiNormal();
		std::string meshName = (const char*)dMeshStatic->shape().name;
		std::vector<SrMaterial>& matList = dMeshDynamic->shape().M; 	
		std::map<std::string,std::string> mtlTexMap = dMeshDynamic->shape().mtlTextureNameMap;
		std::map<std::string,std::string> mtlNormalTexMap = dMeshDynamic->shape().mtlNormalTexNameMap;		
		std::map<std::string,std::string> mtlSpecularTexMap = dMeshDynamic->shape().mtlSpecularTexNameMap;		
		//SmartBody::util::log("meshIndexList %d, matList.size() = %d \n", i, matList.size());
		for (size_t j=0;j<matList.size();j++)
		{			
			SrMaterial& mat = matList[j];	
			std::string mtlName = dMeshDynamic->shape().mtlnames[j];
			if (mtlTexMap.find(mtlName) != mtlTexMap.end())
			{
				allTexNameList.emplace_back(mtlTexMap[mtlName]);
			}
			else
			{
				allTexNameList.emplace_back("");
			}	

			if (mtlNormalTexMap.find(mtlName) != mtlNormalTexMap.end())
			{
				allNormalTexNameList.emplace_back(mtlNormalTexMap[mtlName]);
			}
			else
			{
				allNormalTexNameList.emplace_back("");
			}

			if (mtlSpecularTexMap.find(mtlName) != mtlSpecularTexMap.end())
			{
				allSpecularTexNameList.emplace_back(mtlSpecularTexMap[mtlName]);
			}
			else
			{
				allSpecularTexNameList.emplace_back("");
			}
			allMaterialMeshMap[mtlName] = meshName;
			allMatList.emplace_back(mat);
			allMatNameList.emplace_back(mtlName);
			//colorArray[j%6].get(fcolor);				
			meshSubsetMap[nMaterial] = std::vector<int>(); 
			nMaterial++;
		}
	}				
		

	//printf("num of mesh subset =  %d\n",meshSubsetMap.size());

	int iMaterialOffset = 1;	
	int iFaceIdxOffset = 0, iNormalIdxOffset = 0, iTextureIdxOffset = 0;
	int iFace = 0;
	SrVec3i defaultIdx = SrVec3i(-1,-1,-1);
	for (unsigned int i=0;i<meshIndexList.size();i++)
	{
		int pos = meshIndexList[i];
		
		SrSnModel* dMeshStatic = dMeshStatic_p[pos];
		SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];			
		size_t nVtx = dMeshStatic->shape().V.size();
		size_t nFace = dMeshStatic->shape().F.size();
		size_t nNormal = dMeshStatic->shape().N.size();
		size_t nTexture = dMeshStatic->shape().T.size();
		for (int i=0;i<nVtx;i++)
		{
			vtxNormalIdxMap.emplace_back(std::set<IntPair>());
		}

		nTotalVtxs += nVtx;				
		nTotalTris += nFace;
		
		size_t numTris = dMeshStatic->shape().F.size();
		for (unsigned int i=0; i < numTris ; i++)
		{
			SrModel& model = dMeshStatic->shape();
			if (dMeshStatic->shape().F.size() == 0)
				continue;
			SrVec3i& faceIdx = dMeshStatic->shape().F[i];	
			SrVec3i nIdx;
			nIdx = faceIdx; 
			if (dMeshStatic->shape().Fn.size() != 0)
			{
				SrVec3i& fnIdx = dMeshStatic->shape().Fn[i];	
				nIdx = fnIdx;
			}
			//SrModel::Face& nIdx = dMeshStatic->shape().Fn[i];
			SrVec3i& tIdx = defaultIdx;
			if (model.Ft.size() > i)
				tIdx = model.Ft[i]; 

			if (tIdx[0] >= 0 || 
				tIdx[1] >= 0 || 
				tIdx[2] >= 0)
			{
				vtxNormalIdxMap[faceIdx[0] + iFaceIdxOffset].insert(IntPair(nIdx[0]+iNormalIdxOffset,tIdx[0]+iTextureIdxOffset));
				vtxNormalIdxMap[faceIdx[1] + iFaceIdxOffset].insert(IntPair(nIdx[1]+iNormalIdxOffset,tIdx[1]+iTextureIdxOffset));
				vtxNormalIdxMap[faceIdx[2] + iFaceIdxOffset].insert(IntPair(nIdx[2]+iNormalIdxOffset,tIdx[2]+iTextureIdxOffset));
			}

			int nMatIdx = 0; // if no corresponding materials, push into the default gray material group
			if (i < dMeshStatic->shape().Fm.size())
			{
				nMatIdx = dMeshStatic->shape().Fm[i] + iMaterialOffset;		
				std::map<int,std::vector<int> >::iterator iter = meshSubsetMap.find(nMatIdx);
				if (iter == meshSubsetMap.end())
				{
					SmartBody::util::log("Bad material index %d", nMatIdx);
					nMatIdx = 0;
				}
			}
			meshSubsetMap[nMatIdx].emplace_back(iFace);
			iFace++;
		}
		iFaceIdxOffset += nVtx;
		iNormalIdxOffset += nNormal;
		iMaterialOffset += dMeshDynamic->shape().M.size();
		iTextureIdxOffset += nTexture;
		//printf("iMaterial Offset = %d\n",iMaterialOffset);			
	}

	SmartBody::util::log("nTotalVtxs = %d, nTotalTris = %d", nTotalVtxs, nTotalTris);

	if (nTotalVtxs == 0 || nTotalTris ==0)
		return false;

	vtxNewVtxIdxMap.clear();
	//printf("orig vtxs = %d\n",nTotalVtxs);
	for (unsigned int i=0;i<vtxNormalIdxMap.size();i++)
	{
		std::set<IntPair>& idxMap = vtxNormalIdxMap[i];
		if (idxMap.size() > 1)
		{
			//SmartBody::util::log("vtx %d, idxMap size = %d", i, idxMap.size());
			vtxNewVtxIdxMap[i] = std::vector<int>();
			std::set<IntPair>::iterator si = idxMap.begin();
			si++;
			for ( ;
				si != idxMap.end();
				si++)
			{
				vtxNewVtxIdxMap[i].emplace_back(nTotalVtxs);
				ntNewVtxIdxMap[*si] = nTotalVtxs;
				nTotalVtxs++;
			}
		}
	}
	//SmartBody::util::log("new vtxs = %d\n",nTotalVtxs);

	// temporary storage 
	posBuf.resize(nTotalVtxs);
	normalBuf.resize(nTotalVtxs); 
	tangentBuf.resize(nTotalVtxs); 
	binormalBuf.resize(nTotalVtxs);
	texCoordBuf.resize(nTotalVtxs);
	skinColorBuf.resize(nTotalVtxs);
	meshColorBuf.resize(nTotalVtxs);
	boneCountBuf.resize(nTotalVtxs);
	triBuf.resize(nTotalTris);	

#if USE_SKIN_WEIGHT_SIZE_8
	int skinWeightIter = 2;
#else
	int skinWeightIter = 1; 
#endif

	if (buildSkinnedBuffer)
	{
		
		for (int i=0;i<skinWeightIter;i++)
		{
			boneIDBuf[i].resize(nTotalVtxs);
			boneIDBuf_f[i].resize(nTotalVtxs);
			boneWeightBuf[i].resize(nTotalVtxs);
		}
	}
	

	int iVtx = 0;
	iFace = 0;
	iFaceIdxOffset = 0;
	iNormalIdxOffset = 0;
	iTextureIdxOffset = 0;		
	float step = 1.f/boneJointList.size();
	float floatBuf[4];
	SrRandom random;	

	boneColorMap.clear();
	for (unsigned int i=0;i<boneJointList.size();i++)
	{
		//SrColor boneColor = SrColor::interphue(random.getf());
		SrColor boneColor = SrColor::interphue(i*step);
		boneColor.get(floatBuf);
// 		if (i == 3)
// 			boneColorMap.emplace_back(SrVec(1.f,0.f,0.f));
// 		else
		boneColorMap.emplace_back(SrVec(floatBuf[0],floatBuf[1],floatBuf[2]));
	}	
	
	for (unsigned int c=0;c<meshIndexList.size();c++)
	{
		int pos = meshIndexList[c];
		int globalCounter = 0;
		SrSnModel* dMeshStatic = dMeshStatic_p[pos];
		SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];
		dMeshDynamic->visible(false);
		size_t numVertices = dMeshStatic->shape().V.size();
		size_t numNormals = dMeshStatic->shape().N.size();
		size_t numTexCoords = dMeshStatic->shape().T.size();
		SrMat bindShapeMat;
		SkinWeight* skinWeight = nullptr;
		if (buildSkinnedBuffer)
		{
			skinWeight = skinWeightList[c];
			bindShapeMat = skinWeight->bindShapeMat;
		}
		for (unsigned int i = 0; i < numVertices; i++)
		{
			
			if (buildSkinnedBuffer)
			{				
				if (i >= (int) skinWeight->numInfJoints.size())
					continue;				
			}
			//if (i % 1000 == 0)
			//	SmartBody::util::log("mesh vtx %d = %s", i, lv.toString().c_str());
			posBuf[iVtx] = dMeshStatic->shape().V[i]*bindShapeMat;
			SrVec meshColor = SrVec(1.f,1.f,1.f);
			if (i < dMeshStatic->shape().Vc.size())
			{
				meshColor = dMeshStatic->shape().Vc[i];
				hasVertexColor = true;
			}			
			if (dMeshStatic->shape().T.size() > 0)
			{
				hasTexCoord = true;
			}

			meshColorBuf[iVtx] = meshColor;

			SrMat bindShapeRotate = bindShapeMat.get_rotation();
			SrVec& lt =	dMeshStatic->shape().Tangent[i];		
			SrVec& lb = dMeshStatic->shape().BiNormal[i];
			tangentBuf[iVtx] = lt*bindShapeRotate;
			binormalBuf[iVtx] = lb*bindShapeRotate;
				
			//normalBuffer(iVtx) = Vec3f(ln[0],ln[1],ln[2]);

			if (buildSkinnedBuffer && skinWeight)
			{
				int numOfInfJoints = skinWeight->numInfJoints[i];
				for (int k=0;k<skinWeightIter;k++)
				{
					boneIDBuf[k][iVtx] = SrVec4i(0,0,0,0);
					boneIDBuf_f[k][iVtx] = SrVec4(0,0,0,0);
					boneWeightBuf[k][iVtx] = SrVec4(0,0,0,0);
				}
				std::vector<IntFloatPair> weightList;
				if (skinWeight->infJointName.size() > 0)
				{
					for (int j = 0; j < numOfInfJoints; j++)
					{
						const std::string& curJointName = skinWeight->infJointName[skinWeight->jointNameIndex[globalCounter]];					
						float jointWeight = skinWeight->bindWeight[skinWeight->weightIndex[globalCounter]];
						int    jointIndex  = boneJointIdxMap[curJointName];
						// 					 					if (numOfInfJoints > 8)
						// 					 						printf("w = %d : %f\n",jointIndex,jointWeight);
	
						weightList.emplace_back(IntFloatPair(jointIndex,jointWeight));
						globalCounter ++;									
					}
				}
				std::sort(weightList.begin(),weightList.end(),intFloatComp); // sort for minimum weight
				int numWeight = numOfInfJoints > 8 ? 8 : numOfInfJoints;
				float weightSum = 0.f;
				SrVec skinColor;
				boneCountBuf[iVtx] = numWeight;
				for (int j=0;j<numWeight;j++)
				{
					if (j >= (int)weightList.size())
						continue;
					IntFloatPair& w = weightList[j];
					
					if ( j < 4)
					{
						boneIDBuf[0][iVtx][j] = w.first;
						boneIDBuf_f[0][iVtx][j] = (float)w.first;
						boneWeightBuf[0][iVtx][j] = w.second;
						weightSum += w.second;
					}
					else if (j < 8)
					{
#if USE_SKIN_WEIGHT_SIZE_8
						boneIDBuf[1][iVtx][j-4] = w.first;
						boneIDBuf_f[1][iVtx][j-4] = (float)w.first;
						boneWeightBuf[1][iVtx][j-4] = w.second;
						weightSum += w.second;
#endif
					}	

					if (w.first >= 0 && w.first < (int) boneJointList.size())
					{
						skinColor += boneColorMap[w.first]*w.second;
					}
				}
				skinColorBuf[iVtx] = skinColor;

				for (int j=0;j<4;j++)
				{
					boneWeightBuf[0][iVtx][j] /= weightSum;
#if USE_SKIN_WEIGHT_SIZE_8
					boneWeightBuf[1][iVtx][j] /= weightSum;
#endif
				}	

			}
			

			if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
			{
				std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
				// copy related vtx components 
				for (unsigned int k=0;k<idxMap.size();k++)
				{
					posBuf[idxMap[k]] = posBuf[iVtx];
					meshColorBuf[idxMap[k]] = meshColorBuf[iVtx];
					tangentBuf[idxMap[k]] = tangentBuf[iVtx];
					binormalBuf[idxMap[k]] = binormalBuf[iVtx];

					if (buildSkinnedBuffer)
					{
						boneIDBuf[0][idxMap[k]] = boneIDBuf[0][iVtx];						
						boneIDBuf_f[0][idxMap[k]] = boneIDBuf_f[0][iVtx];
						boneWeightBuf[0][idxMap[k]] = boneWeightBuf[0][iVtx];
#if USE_SKIN_WEIGHT_SIZE_8
						boneIDBuf[1][idxMap[k]] = boneIDBuf[1][iVtx];
						boneIDBuf_f[1][idxMap[k]] = boneIDBuf_f[1][iVtx];	
						boneWeightBuf[1][idxMap[k]] = boneWeightBuf[1][iVtx];						
#endif						
						skinColorBuf[idxMap[k]] = skinColorBuf[iVtx];
						boneCountBuf[idxMap[k]] = boneCountBuf[iVtx];
					}	
				}
			}
			iVtx++;
		}

		size_t numTris = dMeshStatic->shape().F.size();
		for (int i=0; i < numTris ; i++)
		{
			if (dMeshStatic->shape().F.size() <= i)
				continue;				
			SrVec3i& faceIdx = dMeshStatic->shape().F[i];
			if (faceIdx[0] < 0 ||
				faceIdx[1] < 0 ||
				faceIdx[2] < 0)
				continue;
			SrVec3i normalIdx = faceIdx;
			
			if (dMeshStatic->shape().Fn.size() > i)
			{
				normalIdx = dMeshStatic->shape().Fn[i];							
			}
			SrVec3i& texCoordIdx = defaultIdx;
			if (dMeshStatic->shape().Ft.size() > i)
				texCoordIdx = dMeshStatic->shape().Ft[i];
			SrVec3i fIdx = faceIdx; //{ faceIdx.a, faceIdx.b, faceIdx.c};
			SrVec3i nIdx = normalIdx; //{ normalIdx.a, normalIdx.b, normalIdx.c};
			SrVec3i tIdx = texCoordIdx; //{ texCoordIdx.a, texCoordIdx.b, texCoordIdx.c};
				
			SrVec faceNormal = dMeshStatic->shape().face_normal(i);
			for (int k=0;k<3;k++)
			{
				SrVec nvec;
				SrPnt2 tvec	= SrPnt2(0,0);
				unsigned int nidx	= nIdx[k];

				if (dMeshStatic->shape().N.size() > nidx &&
					nidx > 0)
					nvec = dMeshStatic->shape().N[nIdx[k]];
				else
					nvec = faceNormal;

				if (dMeshStatic->shape().T.size() > tIdx[k] && 
					dMeshStatic->shape().T.size() > 0 && 
					dMeshStatic->shape().Ft.size() > 0)
					tvec = dMeshStatic->shape().T[tIdx[k]];

				int newNIdx = nIdx[k] + iNormalIdxOffset;
				int newTIdx	= tIdx[k] + iTextureIdxOffset;
				int vIdx	= fIdx[k] + iFaceIdxOffset;

				if (ntNewVtxIdxMap.find(IntPair(newNIdx,newTIdx)) != ntNewVtxIdxMap.end())
					vIdx = ntNewVtxIdxMap[IntPair(newNIdx,newTIdx)];

				normalBuf[vIdx]		= nvec;
				texCoordBuf[vIdx]	= SrVec2(tvec.x, tvec.y);
				triBuf[iFace][k]	= vIdx;
			}			
			iFace++;
		}
		iFaceIdxOffset += numVertices;
		iNormalIdxOffset += numNormals;
		iTextureIdxOffset += numTexCoords;		
	}
    
	int group = 0;
	std::vector<SbmSubMesh*> hairMeshList;
	std::vector<SbmSubMesh*> alphaMeshList;
	std::map<int,std::vector<int> >::iterator vi;
  
  for (size_t i = 0; i < subMeshList.size(); ++i)
  {
    if (subMeshList[i])
    {
      delete subMeshList[i];
    }
  }
	subMeshList.clear();
	//SmartBody::util::log("subMeshList.size() = %d \n", meshSubsetMap.size());
	for (vi  = meshSubsetMap.begin();
		vi != meshSubsetMap.end();
		vi++)
	{
		int iMaterial = vi->first;

		std::vector<int>& faceIdxList = vi->second;	
		if (faceIdxList.size() == 0)
			continue;		
		SbmSubMesh* mesh = new SbmSubMesh();
		mesh->isHair = false;
		mesh->material = allMatList[iMaterial];		
		mesh->texName  = allTexNameList[iMaterial];
		mesh->normalMapName = allNormalTexNameList[iMaterial];		
		mesh->specularMapName = allSpecularTexNameList[iMaterial];
		mesh->numTri = faceIdxList.size();
		mesh->triBuf.resize(faceIdxList.size());	
		mesh->faceIdxList = faceIdxList;
		for (unsigned int k=0;k<faceIdxList.size();k++)
		{
			mesh->triBuf[k][0] = triBuf[faceIdxList[k]][0];
			mesh->triBuf[k][1] = triBuf[faceIdxList[k]][1];
			mesh->triBuf[k][2] = triBuf[faceIdxList[k]][2];
		}

		mesh->matName = allMatNameList[iMaterial];
		mesh->modelName = allMaterialMeshMap[mesh->matName];
		auto tex = SbmTextureManager::singleton().findTexture(mesh->texName.c_str());
		//if (lowMatName.find("hair") != std::string::npos || lowMatName.find("lash") != std::string::npos 
		//	|| lowMatName.find("shadow") != std::string::npos || lowMatName.find("shell") != std::string::npos)
#if TEST_HAIR_RENDER		
		std::string lowMatName = mesh->matName;
		boost::algorithm::to_lower(lowMatName);		
		if (mesh->material.useAlphaBlend)
		{
			//SmartBody::util::log("alpha mesh = %s",mesh->matName.c_str());
			alphaMeshList.emplace_back(mesh);
		}
		else
		{
			subMeshList.emplace_back(mesh);
		}
#else
		subMeshList.emplace_back(mesh);
#endif			
	}		
	subMeshList.insert(subMeshList.end(),hairMeshList.begin(),hairMeshList.end());
	subMeshList.insert(subMeshList.end(),alphaMeshList.begin(),alphaMeshList.end());

	buildBlendShapes();

	initStaticVertexBuffer = true;
	if (buildSkinnedBuffer)
		initSkinnedVertexBuffer = true;
	return true;
}

bool DeformableMesh::isSkinnedMesh()
{
	return skinWeights.size() > 0;
}


void DeformableMesh::saveToStaticMeshBinary(SmartBodyBinary::StaticMesh* outputStaticMesh, std::string objectName)
{
	// 1	StaticMesh
	outputStaticMesh->set_staticmeshname(objectName);
	// 2
	std::vector<SrModel*> modelsToSave;
	// prepare models
	for (size_t i = 0; i < dMeshStatic_p.size(); ++i)
	{
		modelsToSave.emplace_back(&(dMeshStatic_p[i]->shape()));
	}
	// prepare morph targets
	for (std::map<std::string, std::vector<SrSnModel*> >::iterator iter = blendShapeMap.begin();
		iter != blendShapeMap.end();
		iter++)
	{
		std::vector<SrSnModel*>& targets = (*iter).second;
		for (size_t t = 1; t < targets.size(); t++) // ignore first target since it is a base mesh
		{
			if (targets[t] == nullptr)
				continue;
			SrModel& curModel = targets[t]->shape();
			modelsToSave.emplace_back(&curModel);
		}
	}

	for (size_t s = 0; s < modelsToSave.size(); s++)
	{
		SmartBodyBinary::MeshModel* newMeshModel = outputStaticMesh->add_meshmodels();
		// 1	MeshModel
		SrModel* curModel = modelsToSave[s];
		newMeshModel->set_meshname((const char*)curModel->name);
		// 2
		for (unsigned int m = 0; m < curModel->M.size(); ++m)
		{
			SmartBodyBinary::Material* newMaterial = newMeshModel->add_materials();
			// 1 Material
			newMaterial->add_ambient(curModel->M[m].ambient.r);
			newMaterial->add_ambient(curModel->M[m].ambient.g);
			newMaterial->add_ambient(curModel->M[m].ambient.b);
			newMaterial->add_ambient(curModel->M[m].ambient.a);
			// 2
			newMaterial->add_diffuse(curModel->M[m].diffuse.r);
			newMaterial->add_diffuse(curModel->M[m].diffuse.g);
			newMaterial->add_diffuse(curModel->M[m].diffuse.b);
			newMaterial->add_diffuse(curModel->M[m].diffuse.a);
			// 3
			newMaterial->add_specular(curModel->M[m].specular.r);
			newMaterial->add_specular(curModel->M[m].specular.g);
			newMaterial->add_specular(curModel->M[m].specular.b);
			newMaterial->add_specular(curModel->M[m].specular.a);
			// 4
			newMaterial->add_emission(curModel->M[m].emission.r);
			newMaterial->add_emission(curModel->M[m].emission.g);
			newMaterial->add_emission(curModel->M[m].emission.b);
			newMaterial->add_emission(curModel->M[m].emission.a);
			// 5
			newMaterial->set_shininess(curModel->M[m].shininess);
			// 6
			newMaterial->set_transparency(curModel->M[m].transparency);
			// 7
			newMaterial->set_usealphablend(curModel->M[m].useAlphaBlend);

		}
		// 3
		for (unsigned int v = 0; v < curModel->V.size(); ++v)
		{
			newMeshModel->add_vertexcoordinates(curModel->V[v].x);
			newMeshModel->add_vertexcoordinates(curModel->V[v].y);
			newMeshModel->add_vertexcoordinates(curModel->V[v].z);
		}
		// 4
		for (unsigned int n = 0; n < curModel->N.size(); ++n)
		{
			newMeshModel->add_normals(curModel->N[n].x);
			newMeshModel->add_normals(curModel->N[n].y);
			newMeshModel->add_normals(curModel->N[n].z);
		}
		// 5
		for (unsigned int n = 0; n < curModel->Tangent.size(); ++n)
		{
			newMeshModel->add_tangents(curModel->Tangent[n].x);
			newMeshModel->add_tangents(curModel->Tangent[n].y);
			newMeshModel->add_tangents(curModel->Tangent[n].z);
		}
		// 5
		for (unsigned int n = 0; n < curModel->BiNormal.size(); ++n)
		{
			newMeshModel->add_binormals(curModel->BiNormal[n].x);
			newMeshModel->add_binormals(curModel->BiNormal[n].y);
			newMeshModel->add_binormals(curModel->BiNormal[n].z);
		}
		//7
		for (unsigned int t = 0; t < curModel->T.size(); ++t)
		{
			newMeshModel->add_texturecoordinates(curModel->T[t].x);
			newMeshModel->add_texturecoordinates(curModel->T[t].y);
		}
		//8
		for (unsigned int t = 0; t < curModel->F.size(); ++t)
		{
			newMeshModel->add_trianglefaceindices(curModel->F[t][0]);
			newMeshModel->add_trianglefaceindices(curModel->F[t][1]);
			newMeshModel->add_trianglefaceindices(curModel->F[t][2]);
		}
		// 9
		for (unsigned int t = 0; t < curModel->Fm.size(); ++t)
		{
			newMeshModel->add_materialindices(curModel->Fm[t]);
		}
		// 10
		for (unsigned int t = 0; t < curModel->Fn.size(); ++t)
		{
			newMeshModel->add_normalindices(curModel->Fn[t][0]);
			newMeshModel->add_normalindices(curModel->Fn[t][1]);
			newMeshModel->add_normalindices(curModel->Fn[t][2]);
		}
		// 11
		for (unsigned int t = 0; t < curModel->Ft.size(); ++t)
		{
			newMeshModel->add_texturecoordinatesindices(curModel->Ft[t][0]);
			newMeshModel->add_texturecoordinatesindices(curModel->Ft[t][1]);
			newMeshModel->add_texturecoordinatesindices(curModel->Ft[t][2]);
		}
		// 12
		newMeshModel->set_culling(curModel->culling);
		// 13
		for (unsigned int m = 0; m < curModel->mtlnames.size(); ++m)
		{
			std::string materialName = curModel->mtlnames[m];
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(materialName, tokens, "|");
			if (tokens.size() > 1)
			{
				materialName = objectName + "|" + tokens[1];
			}
			newMeshModel->add_materialnames(materialName.c_str());
		}
		// 14
		std::map<std::string,std::string>::iterator iter;
		for (iter = curModel->mtlTextureNameMap.begin(); iter != curModel->mtlTextureNameMap.end(); ++iter)
		{
			SmartBodyBinary::StringToStringMap* m2DiffuseMapping = newMeshModel->add_materialtodiffusetexturemapping();
			std::string fromStr = iter->first;
			std::vector<std::string> tokensFrom;
			SmartBody::util::tokenize(iter->first, tokensFrom, "|");
			if (tokensFrom.size() > 1)
			{
				fromStr = objectName + "|" + tokensFrom[1];
			}
			m2DiffuseMapping->set_from(fromStr);
			// make sure that the prefix isn't save with the data
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(iter->second, tokens, "|");
			if (tokens.size() > 1)
			{
				m2DiffuseMapping->set_to(tokens[1]);
			}
			else
			{
				m2DiffuseMapping->set_to(iter->second);
			}
			
		}
		// 15
		for (iter = curModel->mtlNormalTexNameMap.begin(); iter != curModel->mtlNormalTexNameMap.end(); ++iter)
		{
			SmartBodyBinary::StringToStringMap* m2NormalMapping = newMeshModel->add_materialtonormaltexturemapping();

			
			std::string fromStr = iter->first;
			std::vector<std::string> tokensFrom;
			SmartBody::util::tokenize(iter->first, tokensFrom, "|");
			if (tokensFrom.size() > 1)
			{
				fromStr = objectName + "|" + tokensFrom[1];
			}
			m2NormalMapping->set_from(fromStr);


			// make sure that the prefix isn't save with the data
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(iter->second, tokens, "|");
			if (tokens.size() > 1)
			{
				m2NormalMapping->set_to(tokens[1]);
			}
			else
			{
				m2NormalMapping->set_to(iter->second);
			}
		}
		// 16
		for (iter = curModel->mtlSpecularTexNameMap.begin(); iter != curModel->mtlSpecularTexNameMap.end(); ++iter)
		{
			SmartBodyBinary::StringToStringMap* m2SpecularMapping = newMeshModel->add_materialtospeculartexturemapping();

			
			std::string fromStr = iter->first;
			std::vector<std::string> tokensFrom;
			SmartBody::util::tokenize(iter->first, tokensFrom, "|");
			if (tokensFrom.size() > 1)
			{
				fromStr = objectName + "|" + tokensFrom[1];
			}
			m2SpecularMapping->set_from(fromStr);


			// make sure that the prefix isn't save with the data
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(iter->second, tokens, "|");
			if (tokens.size() > 1)
			{
				m2SpecularMapping->set_to(tokens[1]);
			}
			else
			{
				m2SpecularMapping->set_to(iter->second);
			}
		}
		// 17
		std::map<std::string,std::vector<int> >::iterator iter1;
		for (iter1 = curModel->mtlFaceIndices.begin(); iter1 != curModel->mtlFaceIndices.end(); ++iter1)
		{
			SmartBodyBinary::StringToIntVectorMap* m2FaceIndicesMapping = newMeshModel->add_materialtofaceindices();
			m2FaceIndicesMapping->set_from(iter1->first);
			for (size_t f = 0; f < iter1->second.size(); ++f)
				m2FaceIndicesMapping->add_to(iter1->second[f]);
		}
		// 18
		for (iter = curModel->mtlTransparentTexNameMap.begin(); iter != curModel->mtlTransparentTexNameMap.end(); ++iter)
		{
			SmartBodyBinary::StringToStringMap* m2TransparentMapping = newMeshModel->add_materialtotransparenttexturemapping();

			
			std::string fromStr = iter->first;
			std::vector<std::string> tokensFrom;
			SmartBody::util::tokenize(iter->first, tokensFrom, "|");
			if (tokensFrom.size() > 1)
			{
				fromStr = objectName + "|" + tokensFrom[1];
			}
			m2TransparentMapping->set_from(fromStr);
			
			// make sure that the prefix isn't save with the data
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(iter->second, tokens, "|");
			if (tokens.size() > 1)
			{
				m2TransparentMapping->set_to(tokens[1]);
			}
			else
			{
				m2TransparentMapping->set_to(iter->second);
			}
		}
		// 19
		for (iter = curModel->mtlGlossyTexNameMap.begin(); iter != curModel->mtlGlossyTexNameMap.end(); ++iter)
		{
			SmartBodyBinary::StringToStringMap* m2GlossyMapping = newMeshModel->add_materialtoglossytexturemapping();

			
			std::string fromStr = iter->first;
			std::vector<std::string> tokensFrom;
			SmartBody::util::tokenize(iter->first, tokensFrom, "|");
			if (tokensFrom.size() > 1)
			{
				fromStr = objectName + "|" + tokensFrom[1];
			}
			m2GlossyMapping->set_from(fromStr);


			// make sure that the prefix isn't save with the data
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(iter->second, tokens, "|");
			if (tokens.size() > 1)
			{
				m2GlossyMapping->set_to(tokens[1]);
			}
			else
			{
				m2GlossyMapping->set_to(iter->second);
			}
		}
	}
	
}

std::string buildName(std::string modelName, std::string textureName)
{
	if (textureName.find("|") != std::string::npos)
		return textureName;
	std::stringstream strstr;
	strstr << modelName << "|" << textureName;
	return strstr.str();
}


void DeformableMesh::readFromStaticMeshBinary(SmartBodyBinary::StaticMesh* mesh, std::vector<SrModel*>& models, std::string file)
{
	SmartBodyBinary::StaticMesh staticMesh = *mesh;

	// 1 StaticMesh
	//this->setName(staticMesh.staticmeshname());
	// set the mesh name based on the filename
	boost::filesystem::path p(file);
	this->setName(p.filename().string());

	for (int numMeshModels = 0; numMeshModels < staticMesh.meshmodels_size(); ++numMeshModels)
	{
		const SmartBodyBinary::MeshModel& meshModel = staticMesh.meshmodels(numMeshModels);
		SrModel* newModel = new SrModel();
		// 1 MeshModel
		newModel->name.set(meshModel.meshname().c_str());
		// 2
		for (int numMaterials = 0; numMaterials < meshModel.materials_size(); ++numMaterials)
		{
			const SmartBodyBinary::Material& material = meshModel.materials(numMaterials);
			SrMaterial newMat;
			// 1 Material
			newMat.ambient.r = material.ambient(0);
			newMat.ambient.g = material.ambient(1);
			newMat.ambient.b = material.ambient(2);
			newMat.ambient.a = material.ambient(3);
			// 2
			newMat.diffuse.r = material.diffuse(0);
			newMat.diffuse.g = material.diffuse(1);
			newMat.diffuse.b = material.diffuse(2);
			newMat.diffuse.a = material.diffuse(3);
			if (newMat.diffuse.a < 1.0)
			{
				SmartBody::util::log("mesh %d, mat %d, useAlphaBlending",numMeshModels, numMaterials);
				newMat.useAlphaBlend = true;
			}
			// 3
			newMat.specular.r = material.specular(0);
			newMat.specular.g = material.specular(1);
			newMat.specular.b = material.specular(2);
			newMat.specular.a = material.specular(3);
			// 4
			newMat.emission.r = material.emission(0);
			newMat.emission.g = material.emission(1);
			newMat.emission.b = material.emission(2);
			newMat.emission.a = material.emission(3);
			// 5
			newMat.shininess = material.shininess();

			// 6 transparency
			newMat.transparency = material.transparency();

			// 7 useAlphaBlend
			newMat.useAlphaBlend = material.usealphablend();

			newModel->M.emplace_back(newMat);
		}
		// 3
		for (int x = 0; x < meshModel.vertexcoordinates_size() / 3; ++x)
		{
			SrPnt newPoint(meshModel.vertexcoordinates(x * 3 + 0), meshModel.vertexcoordinates(x * 3 + 1), meshModel.vertexcoordinates(x * 3 + 2));
			newModel->V.emplace_back(newPoint);
		}
		// 4
		for (int x = 0; x < meshModel.normals_size() / 3; ++x)
		{
			SrPnt newPoint(meshModel.normals(x * 3 + 0), meshModel.normals(x * 3 + 1), meshModel.normals(x * 3 + 2));
			newModel->N.emplace_back(newPoint);
		}
		// 5
		for (int x = 0; x < meshModel.tangents_size() / 3; ++x)
		{
			SrPnt newPoint(meshModel.tangents(x * 3 + 0), meshModel.tangents(x * 3 + 1), meshModel.tangents(x * 3 + 2));
			newModel->Tangent.emplace_back(newPoint);
		}
		// 6
		for (int x = 0; x < meshModel.binormals_size() / 3; ++x)
		{
			SrPnt newPoint(meshModel.binormals(x * 3 + 0), meshModel.binormals(x * 3 + 1), meshModel.binormals(x * 3 + 2));
			newModel->BiNormal.emplace_back(newPoint);
		}
		// 7
		for (int x = 0; x < meshModel.texturecoordinates_size() / 2; ++x)
		{
			SrPnt2 newPoint(meshModel.texturecoordinates(x * 2 + 0), meshModel.texturecoordinates(x * 2 + 1));
			newModel->T.emplace_back(newPoint);
		}
		// 8
		for (int x = 0; x < meshModel.trianglefaceindices_size() / 3; ++x)
		{
			SrVec3i newFace;
			newFace[0] = meshModel.trianglefaceindices(x * 3 + 0);
			newFace[1] = meshModel.trianglefaceindices(x * 3 + 1);
			newFace[2] = meshModel.trianglefaceindices(x * 3 + 2);
			newModel->F.emplace_back(newFace);
		}
		// 9
		for (int x = 0; x < meshModel.materialindices_size(); ++x)
		{
			newModel->Fm.emplace_back(meshModel.materialindices(x));
		}
		// 10
		for (int x = 0; x < meshModel.normalindices_size() / 3; ++x)
		{
			SrVec3i newFace;
			newFace[0] = meshModel.normalindices(x * 3 + 0);
			newFace[1] = meshModel.normalindices(x * 3 + 1);
			newFace[2] = meshModel.normalindices(x * 3 + 2);
			newModel->Fn.emplace_back(newFace);
		}
		// 11
		for (int x = 0; x < meshModel.texturecoordinatesindices_size() / 3; ++x)
		{
			SrVec3i newFace;
			newFace[0] = meshModel.texturecoordinatesindices(x * 3 + 0);
			newFace[1] = meshModel.texturecoordinatesindices(x * 3 + 1);
			newFace[2] = meshModel.texturecoordinatesindices(x * 3 + 2);
			newModel->Ft.emplace_back(newFace);
		}
		// 12
		newModel->culling = meshModel.culling();
		// 13
		for (int x = 0; x < meshModel.materialnames_size(); ++x)
		{

			std::string fromName = buildName(getName(), meshModel.materialnames(x).c_str());
			newModel->mtlnames.emplace_back(fromName);
		}
		// 14
		for (int x = 0; x < meshModel.materialtodiffusetexturemapping_size(); ++x)
		{
			const SmartBodyBinary::StringToStringMap& m2d = meshModel.materialtodiffusetexturemapping(x);

			std::string fromName = buildName(getName(), m2d.from());
			newModel->mtlTextureNameMap.insert(std::make_pair(fromName, m2d.to()));
		}
		// 15
		for (int x = 0; x < meshModel.materialtonormaltexturemapping_size(); ++x)
		{
			const SmartBodyBinary::StringToStringMap& m2d = meshModel.materialtonormaltexturemapping(x);

			std::string fromName = buildName(getName(), m2d.from());
			newModel->mtlNormalTexNameMap.insert(std::make_pair(fromName, m2d.to()));
		}
		// 16
		for (int x = 0; x < meshModel.materialtospeculartexturemapping_size(); ++x)
		{
			const SmartBodyBinary::StringToStringMap& m2d = meshModel.materialtospeculartexturemapping(x);

			std::string fromName = buildName(getName(), m2d.from());
			newModel->mtlSpecularTexNameMap.insert(std::make_pair(fromName, m2d.to()));
		}
		// 17
		for (int x = 0; x < meshModel.materialtofaceindices_size(); ++x)
		{
			const SmartBodyBinary::StringToIntVectorMap& m2d = meshModel.materialtofaceindices(x);
			std::vector<int> indices;
			for (int z = 0; z < m2d.to_size(); ++z)
				indices.emplace_back(m2d.to(z));

			std::string fromName = buildName(getName(), m2d.from());
			newModel->mtlFaceIndices.insert(std::make_pair(fromName, indices));
		}
		// 18
		for (int x = 0; x < meshModel.materialtotransparenttexturemapping_size(); ++x)
		{
			const SmartBodyBinary::StringToStringMap& m2d = meshModel.materialtotransparenttexturemapping(x);

			std::string fromName = buildName(getName(), m2d.from());
			newModel->mtlTransparentTexNameMap.insert(std::make_pair(fromName, m2d.to()));
		}
		// 19
		for (int x = 0; x < meshModel.materialtoglossytexturemapping_size(); ++x)
		{
			const SmartBodyBinary::StringToStringMap& m2d = meshModel.materialtoglossytexturemapping(x);

			std::string fromName = buildName(getName(), m2d.from());
			newModel->mtlGlossyTexNameMap.insert(std::make_pair(fromName, m2d.to()));
		}

		// bake transparent and glossy maps into diffuse and specular maps
		SrString path = file.c_str();
		SrString filename;
		path.extract_file_name(filename);
		SrStringArray paths;
		paths.push(path);

#if 1
		SbmTextureManager& texManager = SbmTextureManager::singleton();

		for (size_t i = 0; i < newModel->M.size(); i++)
		{
			std::string matName = newModel->mtlnames[i];			
			if (newModel->mtlTextureNameMap.find(matName) != newModel->mtlTextureNameMap.end())
			{
				std::string prefixedName = newModel->mtlTextureNameMap[matName];
       				std::string::size_type index = prefixedName.find_first_of("|");
				std::string fileName = newModel->mtlTextureNameMap[matName];				ParserCOLLADAFast::load_texture(SbmTextureManager::TEXTURE_DIFFUSE, prefixedName.c_str(), fileName.c_str(), paths);

				if (newModel->mtlTransparentTexNameMap.find(matName) != newModel->mtlTransparentTexNameMap.end())
				{
					auto diffuseTex = texManager.findTexture(prefixedName.c_str());
					SbmTexture* transTex = new SbmTexture("Dummy");
					std::string transpTexFile = (*newModel->mtlTransparentTexNameMap.find(matName)).second;
					std::string finalTextureName = ParserCOLLADAFast::getFinalTextureFileName(transpTexFile, paths);
					transTex->loadImage(finalTextureName.c_str());
					if (diffuseTex && transTex)
					{
						diffuseTex->bakeAlphaIntoTexture(transTex, newModel->M[i].useAlphaBlend);
					}

				delete transTex;
				}

				//newModel->mtlTextureNameMap[matName] = prefixedName;
			}

			if (newModel->mtlSpecularTexNameMap.find(matName) != newModel->mtlSpecularTexNameMap.end())
			{
				std::string prefixedName = newModel->mtlSpecularTexNameMap[matName];
				std::string::size_type index = prefixedName.find_first_of('|');
				std::string fileName = prefixedName;

				if (index != std::string::npos)
					fileName = prefixedName.substr(index + 1);
				ParserCOLLADAFast::load_texture(SbmTextureManager::TEXTURE_SPECULARMAP, prefixedName.c_str(), fileName.c_str(), paths);

				if (newModel->mtlGlossyTexNameMap.find(matName) != newModel->mtlGlossyTexNameMap.end())
				{
					auto specularTex = texManager.findTexture(prefixedName.c_str());
					SbmTexture glossyTex("Dummy");
					std::string glossyTexFile = (*newModel->mtlGlossyTexNameMap.find(matName)).second;
					std::string finalTextureName = ParserCOLLADAFast::getFinalTextureFileName(glossyTexFile, paths);
					glossyTex.loadImage(finalTextureName.c_str());
					if (specularTex)
					{
						specularTex->bakeAlphaIntoTexture(&glossyTex, newModel->M[i].useAlphaBlend);
					}

				}


				//newModel->mtlSpecularTexNameMap[matName] = prefixedName;
			}
		}
#endif
		
		//if (newModel->Fn.size() == 0)
		{
			newModel->computeNormals();
		}

		models.emplace_back(newModel);
	}
}


SBAPI void DeformableMesh::addTransform( const SrMat& transform )
{
	for (auto & i : dMeshDynamic_p)
	{
		i->shape().addTransform(transform);
	}
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		dMeshStatic_p[i]->shape().addTransform(transform);
	}
}


void DeformableMesh::translate(SrVec trans)
{
	for (auto & i : dMeshDynamic_p)
	{
		i->shape().translate(trans);
	}
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		dMeshStatic_p[i]->shape().translate(trans);
	}
}

void DeformableMesh::rotate(SrVec rot)
{
	for (auto & i : dMeshDynamic_p)
	{
		i->shape().rotate(rot);
	}
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		dMeshStatic_p[i]->shape().rotate(rot);
	}
}

void DeformableMesh::scale(float factor)
{
	for (auto & i : dMeshDynamic_p)
	{
		i->shape().scale(factor);
	}
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		dMeshStatic_p[i]->shape().scale(factor);
	}
}

void DeformableMesh::centralize()
{
	for (auto & i : dMeshDynamic_p)
	{
		i->shape().centralize();
	}
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		dMeshStatic_p[i]->shape().centralize();
	}
}

void DeformableMesh::computeNormals()
{
	for (auto & i : dMeshDynamic_p)
	{
		i->shape().computeNormals();
		//dMeshDynamic_p[i]->shape().computeTangentBiNormal();
	}
	for (unsigned int i = 0; i < dMeshDynamic_p.size(); i++)
	{
		dMeshStatic_p[i]->shape().computeNormals();
		//dMeshDynamic_p[i]->shape().computeTangentBiNormal();
	}
}


bool DeformableMesh::saveToSmb(std::string inputFileName)
{
	auto* outputStaticMesh = new SmartBodyBinary::StaticMesh();

	
	// set properly the binary static mesh to be output file name (fileName + extension)
	boost::filesystem::path p(inputFileName);
	std::string fileName = boost::filesystem::basename(p);
	std::string extension =  boost::filesystem::extension(p);
	if (extension != ".smb")
	{
		extension = ".smb";
		inputFileName = fileName + extension;
	}
	saveToStaticMeshBinary(outputStaticMesh, fileName + extension);
	outputStaticMesh->set_staticmeshname(fileName + extension);
	
	// TODO: copy the textures out if needed

	std::fstream file(inputFileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!outputStaticMesh->SerializeToOstream(&file)) 
	{
		SmartBody::util::log("Fail to write to binary file %s", inputFileName.c_str());
		return false;
	}
	google::protobuf::ShutdownProtobufLibrary();

	return true;
}

bool DeformableMesh::saveToDae(SmartBody::SBRenderAssetManager& renderAssetManager, std::string inputFileName, std::string skeletonName)
{
	if (!isSkinnedMesh())
	{
		SmartBody::util::log("mesh %s is a static mesh, skip saving to %s", this->getName().c_str(), inputFileName.c_str());
		return false;
	}
	boost::filesystem::path p(inputFileName);
	std::string baseName = boost::filesystem::basename(p);
	std::string extension = boost::filesystem::extension(p);
	if (extension != ".dae")
	{
		extension = ".dae";
		inputFileName = baseName + extension;
	}
	std::string filePath = p.parent_path().string();

	auto* outputDeformableMesh = new SmartBodyBinary::DeformableMesh();

	std::vector<std::string> moNames;
	double scale = 1.0;
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(skeletonName);
	if (character)
	{
		SrVec scale3 = character->getVec3Attribute("deformableMeshScale");
		scale = scale3.x;
	}

	ParserOpenCOLLADA::exportCollada(renderAssetManager, filePath, skeletonName, this->getName(), moNames, true, true, false, scale);
	return true;
}

bool DeformableMesh::saveToDmb(std::string inputFileName)
{
	if (!isSkinnedMesh())
	{
		SmartBody::util::log("mesh %s is a static mesh, skip saving to %s", this->getName().c_str(), inputFileName.c_str());
		return false;
	}
	boost::filesystem::path p(inputFileName);
	std::string fileName = boost::filesystem::basename(p);
	std::string extension =  boost::filesystem::extension(p);
	if (extension != ".dmb")
	{
		extension = ".dmb";
		inputFileName = fileName + extension;
	}

	auto* outputDeformableMesh = new SmartBodyBinary::DeformableMesh();
	// 1 DeformableMesh
	outputDeformableMesh->set_deformablemeshname(fileName + extension);
	// 2
	SmartBodyBinary::StaticMesh* outputStaticMesh = outputDeformableMesh->mutable_staticmesh(); 

	saveToStaticMeshBinary(outputStaticMesh, fileName + extension);
	outputStaticMesh->set_staticmeshname(fileName + extension);

	// save skin weights
	for (auto & skinWeight : skinWeights)
	{
		SmartBodyBinary::SkinWeight* newSkinWeight = outputDeformableMesh->add_skinweights();
		// 1 SkinWeight
		newSkinWeight->set_sourcemeshname(skinWeight->sourceMesh);
		// 2
		for (auto & x : skinWeight->infJointName)
		{
			newSkinWeight->add_influencejointnames(x);
		}
		// 3
		for (float x : skinWeight->bindWeight)
		{
			newSkinWeight->add_bindweights(x);
		}
		// 4
		for (auto & mat : skinWeight->bindPoseMat)
		{
				for (int m = 0; m < 16; ++m)
				newSkinWeight->add_bindposematrice(mat.get(m));
		}
		// 5
		for (int m = 0; m < 16; ++m)
			newSkinWeight->add_bindshapematrix(skinWeight->bindShapeMat.get(m));
		// 6
		for (unsigned int numInfJoint : skinWeight->numInfJoints)
			newSkinWeight->add_numberofinfluencejoints(numInfJoint);
		// 7
		for (unsigned int x : skinWeight->weightIndex)
			newSkinWeight->add_weightindices(x);
		// 8
		for (unsigned int x : skinWeight->jointNameIndex)
			newSkinWeight->add_jointnameindices(x);
	}
	
	// save morph targets
	std::map<std::string, std::vector<std::string> >::iterator iter;
	for (iter = this->morphTargets.begin(); iter != this->morphTargets.end(); ++iter)
	{
		SmartBodyBinary::StringToStringVectorMap* morphMap = outputDeformableMesh->add_morphtargets();
		morphMap->set_from(iter->first);
		for (auto & x : iter->second)
		{
			morphMap->add_to(x);
		}
	}
	

	std::fstream file(inputFileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!outputDeformableMesh->SerializeToOstream(&file)) 
	{
		SmartBody::util::log("Fail to write to binary file %s", inputFileName.c_str());
		return false;
	}
	google::protobuf::ShutdownProtobufLibrary();
	return true;
}

bool DeformableMesh::readFromSmb(std::string inputFileName)
{
	SmartBodyBinary::StaticMesh staticMesh;

	std::fstream input(inputFileName.c_str(), std::ios::in | std::ios::binary);
  google::protobuf::io::IstreamInputStream raw_input(&input);
  google::protobuf::io::CodedInputStream coded_input(&raw_input);
	coded_input.SetTotalBytesLimit(256000000);
	
	if (!staticMesh.ParseFromCodedStream(&coded_input))
	{
		SmartBody::util::log("Failed to parse binary static mesh from file %s", inputFileName.c_str());
		return false;
	}

	std::vector<SrModel*> models;
	readFromStaticMeshBinary(&staticMesh, models, inputFileName);

	for (size_t m = 0; m > models.size(); m++)
	{
		SrSnModel* srSnModelStatic = new SrSnModel();
		srSnModelStatic->shape(*models[m]);
		srSnModelStatic->shape().name = models[m]->name;
		srSnModelStatic->ref();
		
		dMeshStatic_p.emplace_back(srSnModelStatic);
		
		SrSnModel* srSnModelDynamic = new SrSnModel();
		srSnModelDynamic->shape(*(models[m]));
		srSnModelDynamic->changed(true);
		srSnModelDynamic->visible(false);
		srSnModelDynamic->shape().name = models[m]->name;
		srSnModelDynamic->ref();
		
		dMeshDynamic_p.emplace_back(srSnModelDynamic);
		//delete models[m];
	}

	// explicitly load all the textures
	boost::filesystem::path p(inputFileName);
	std::string filePath = p.parent_path().string();
	loadAllFoundTextures(filePath);
  
  for (auto & model : models)
    delete model;
  models.clear();
  
	
	return true;
}

bool DeformableMesh::readFromDmb(std::string inputFileName)
{
	SmartBodyBinary::DeformableMesh deformableMesh;

	std::fstream input(inputFileName.c_str(), std::ios::in | std::ios::binary);
  google::protobuf::io::IstreamInputStream raw_input(&input);
  google::protobuf::io::CodedInputStream coded_input(&raw_input);
	coded_input.SetTotalBytesLimit(256000000);

	if (!deformableMesh.ParseFromCodedStream(&coded_input))
	{
		SmartBody::util::log("Failed to parse binary deformable mesh from file %s", inputFileName.c_str());
		return false;
	}

	SmartBodyBinary::StaticMesh staticMesh = deformableMesh.staticmesh();

	std::vector<SrModel*> models;
	readFromStaticMeshBinary(&staticMesh, models, inputFileName);

	// don't write into static/dynamic models until we'll sure which models
	// are base shapes and which are morphs. Morphs fo in the blendShapeMap

	// make the models easier to access
	std::map<std::string, SrSnModel*> modelMap;
	std::map<std::string, bool> modelsUsed;
	for (auto & model : models)
	{
		SrSnModel* srSnModelStatic = new SrSnModel();
		srSnModelStatic->shape(*model);
		srSnModelStatic->shape().name = model->name;
    srSnModelStatic->ref();
		modelMap.insert(std::pair<std::string, SrSnModel*>(std::string(model->name), srSnModelStatic));
		modelsUsed.insert(std::pair<std::string, bool>(std::string(model->name), false));
	}
	// keep track of the models used for morphs; non-morph models are assumed to be part of the basic mesh

	// load morph targets
	for (int i = 0; i < deformableMesh.morphtargets_size(); ++i)
	{
		const SmartBodyBinary::StringToStringVectorMap& morphMap = deformableMesh.morphtargets(i);
		std::vector<std::string> morphs;
		
		for (int x = 0; x < morphMap.to_size(); ++x)
		{
			morphs.emplace_back(morphMap.to(x));
		}
		// morphTargets contains a morphgroup->morphname relationship
		morphTargets.insert(std::make_pair(morphMap.from(), morphs));

		// blendshapemap contains the morphgroup->model relationship
		// add the accompanying models
		std::vector<SrSnModel*> morphModels;
		for (size_t n = 0; n < morphs.size(); n++)
		{
			auto iter = modelMap.find(morphs[n]);
			if (iter != modelMap.end())
			{
				SrSnModel* model = (*iter).second;
				SrSnModel* baseModelCopy = new SrSnModel();
				baseModelCopy->shape(model->shape());
				baseModelCopy->ref();
				morphModels.emplace_back(baseModelCopy);
				if (n > 0) // mark non-base models as morph targets so that they are not added as static/dynamic meshes
					modelsUsed[morphs[n]] = true;
			}
		}
		if (!morphs.empty())
			blendShapeMap.insert(std::make_pair(morphs[0], morphModels));
	}

	for (auto model : models)
	{
			std::string modelName((const char*) model->name);

		auto iter2 = modelsUsed.find(modelName);
		if (!(*iter2).second)
		{
			auto iter3 = modelMap.find(modelName);
			SrSnModel* srsnmodel = (*iter3).second;
			dMeshStatic_p.emplace_back(srsnmodel);
			srsnmodel->ref();

			SrSnModel* srSnModelDynamic = new SrSnModel();
			srSnModelDynamic->shape(srsnmodel->shape());
			srSnModelDynamic->changed(true);
			srSnModelDynamic->visible(false);
			srSnModelDynamic->shape().name = srsnmodel->shape().name;
			
			dMeshDynamic_p.emplace_back(srSnModelDynamic);
			srSnModelDynamic->ref();
			//delete models[m];
		}
	}

	// load skin weights
	for (int i = 0; i < deformableMesh.skinweights_size(); ++i)
	{
		auto* newSkinWeights = new SkinWeight();
		// 1 SkinWeight
		newSkinWeights->sourceMesh = deformableMesh.skinweights(i).sourcemeshname();
		// 2
		for (int x = 0; x < deformableMesh.skinweights(i).influencejointnames_size(); ++x)
		{
			newSkinWeights->infJointName.emplace_back(deformableMesh.skinweights(i).influencejointnames(x));
		}
		// 3
		for (int x = 0; x < deformableMesh.skinweights(i).bindweights_size(); ++x)
		{
			newSkinWeights->bindWeight.emplace_back(deformableMesh.skinweights(i).bindweights(x));
		}
		// 4
		for (int x = 0; x < deformableMesh.skinweights(i).bindposematrice_size() / 16; ++x)
		{
			SrMat mat;
			for (int m = 0; m < 16; ++m)
			{
				mat.set(m, deformableMesh.skinweights(i).bindposematrice(x * 16 + m));
			}
			newSkinWeights->bindPoseMat.emplace_back(mat);
		}
		// 5
		for (int m = 0; m < deformableMesh.skinweights(i).bindshapematrix_size(); ++m)
		{
			newSkinWeights->bindShapeMat.set(m, deformableMesh.skinweights(i).bindshapematrix(m));
		}
		// 6
		for (int x = 0; x < deformableMesh.skinweights(i).numberofinfluencejoints_size(); ++x)
		{
			newSkinWeights->numInfJoints.emplace_back(deformableMesh.skinweights(i).numberofinfluencejoints(x));
		}
		// 7
		for (int x = 0; x < deformableMesh.skinweights(i).weightindices_size(); ++x)
		{
			newSkinWeights->weightIndex.emplace_back(deformableMesh.skinweights(i).weightindices(x));
		}
		// 8
		for (int x = 0; x < deformableMesh.skinweights(i).jointnameindices_size(); ++x)
		{
			newSkinWeights->jointNameIndex.emplace_back(deformableMesh.skinweights(i).jointnameindices(x));
		}

		this->skinWeights.emplace_back(newSkinWeights);
	}

	
	
	// explicitly load all the textures
	boost::filesystem::path p(inputFileName);
	std::string filePath = p.parent_path().string();
	loadAllFoundTextures(filePath);
  
  for (auto & model : models)
    delete model;
  models.clear();
  
  std::map<std::string, SrSnModel*>::iterator it;
  for (it  = modelMap.begin();
       it != modelMap.end();
       it++)
  {
    it->second->unref();
  }
  modelMap.clear();
	
	return true;
}

void DeformableMesh::loadAllFoundTextures(std::string textureDirectory)
{
	// load texture specifically into SmartBody after parsing the texture file names
	SrStringArray paths;
	paths.push_path(textureDirectory.c_str()); 
	std::map<std::string,std::string>::iterator iter;
	for (auto & i : dMeshStatic_p)
	{
		for (iter = i->shape().mtlTextureNameMap.begin(); iter != i->shape().mtlTextureNameMap.end(); ++iter)
		{
			SbmTextureManager& texManager = SbmTextureManager::singleton();

			auto tex = texManager.findTexture(iter->first.c_str());
			/*
			if (tex)
			{
				texManager.deleteTexture(SbmTextureManager::TEXTURE_DIFFUSE, iter->second.c_str());
				tex = nullptr;
			}*/
			if (!tex)
			{
				// separate the texture prefix and |
				std::string textureLabel = iter->second;
				std::string::size_type labelIndex = textureLabel.find_first_of('|');
				std::string textureFile = textureLabel.substr(labelIndex + 1);
				//SmartBody::util::log("loading texture label = %s, textureFile = %s", textureLabel.c_str(), textureFile.c_str());
#if 0
				std::string::size_type prefixIndex = textureFile.find_first_of("file:///");
				if (prefixIndex == 0)
					textureFile = textureFile.substr(7);
				prefixIndex = textureFile.find_first_of("file://");
				if (prefixIndex == 0)
					textureFile = textureFile.substr(6);
#endif
				//std::string textureName = textureDirectory + "/" + textureFile;
				std::string textureName = ParserCOLLADAFast::getFinalTextureFileName(textureFile, paths);
				if (!boost::filesystem::exists(boost::filesystem::path(textureName)))
					SmartBody::util::log("Texture %s doesn't exist under same path of mesh %s", textureName.c_str(), getName().c_str());
				else
					texManager.loadTexture(SbmTextureManager::TEXTURE_DIFFUSE, iter->second.c_str(), textureName.c_str());
			}
		}
		for (iter = i->shape().mtlNormalTexNameMap.begin(); iter != i->shape().mtlNormalTexNameMap.end(); ++iter)
		{
			SbmTextureManager& texManager = SbmTextureManager::singleton();
			auto tex = texManager.findTexture(iter->second.c_str());

			/*
			if (tex)
			{
				texManager.deleteTexture(SbmTextureManager::TEXTURE_NORMALMAP, iter->second.c_str());
				tex = nullptr;
			}
			*/
			if (!tex)
			{
				// separate the texture prefix and |
				std::string textureLabel = iter->second;
				std::string::size_type labelIndex = textureLabel.find_first_of('|');
				std::string textureFile = textureLabel.substr(labelIndex + 1);
				SmartBody::util::log("loading normal map label = %s, textureFile = %s", textureLabel.c_str(), textureFile.c_str());
#if 0
				std::string::size_type prefixIndex = textureFile.find_first_of("file:///");
				if (prefixIndex == 0)
					textureFile = textureFile.substr(7);
				prefixIndex = textureFile.find_first_of("file://");
				if (prefixIndex == 0)
					textureFile = textureFile.substr(6);
#endif
				std::string textureName = ParserCOLLADAFast::getFinalTextureFileName(textureFile, paths);
				if (!boost::filesystem::exists(boost::filesystem::path(textureName)))
					SmartBody::util::log("Texture %s doesn't exist under same path of mesh %s", textureName.c_str(), getName().c_str());
				else
					texManager.loadTexture(SbmTextureManager::TEXTURE_NORMALMAP, iter->second.c_str(), textureName.c_str());
			}
		}
		for (iter = i->shape().mtlSpecularTexNameMap.begin(); iter != i->shape().mtlSpecularTexNameMap.end(); ++iter)
		{
			SbmTextureManager& texManager = SbmTextureManager::singleton();
			auto tex = texManager.findTexture(iter->second.c_str());

			/*
			if (tex)
			{
				texManager.deleteTexture(SbmTextureManager::TEXTURE_SPECULARMAP, iter->second.c_str());
				tex = nullptr;
			}*/
			if (!tex)
			{
				// separate the texture prefix and |
				std::string textureLabel = iter->second;
				std::string::size_type labelIndex = textureLabel.find_first_of('|');
				std::string textureFile = textureLabel.substr(labelIndex + 1);
				SmartBody::util::log("loading specular map = %s, textureFile = %s", textureLabel.c_str(), textureFile.c_str());
#if 0
				std::string::size_type prefixIndex = textureFile.find_first_of("file:///");
				if (prefixIndex == 0)
					textureFile = textureFile.substr(7);
				prefixIndex = textureFile.find_first_of("file://");
				if (prefixIndex == 0)
					textureFile = textureFile.substr(6);
#endif
				std::string textureName = ParserCOLLADAFast::getFinalTextureFileName(textureFile, paths);
				if (!boost::filesystem::exists(boost::filesystem::path(textureName)))
					SmartBody::util::log("Texture %s doesn't exist under same path of mesh %s", textureName.c_str(), getName().c_str());
				else
					texManager.loadTexture(SbmTextureManager::TEXTURE_SPECULARMAP, iter->second.c_str(), textureName.c_str());
			}
		}
	}
}

SrVec DeformableMesh::computeCenterOfMass()
{
	SrVec CoM = SrVec(0,0,0);
	for (auto & i : posBuf)
	{
		CoM += i;
	}
	CoM /= (float) posBuf.size();
	return CoM;
}

SBAPI SrBox DeformableMesh::computeBoundingBox()
{
	SrBox bbox;
	for (auto & i : posBuf)
	{
		bbox.extend(i);
	}
	return bbox;
}

/************************************************************************/
/* Deformable Mesh Instance                                             */
/************************************************************************/

DeformableMeshInstance::DeformableMeshInstance()
{
	_mesh				= nullptr;
	_skeleton			= nullptr;
	_updateMesh			= false;
	_isStaticMesh		= false;
	_recomputeNormal	= true;
	_meshScale			= SrVec(1.f, 1.f, 1.f);
	_character			= nullptr;
	meshVisibleType		= 1;

	//	Auxiliar FBO and Texture for offline rendering to blend textures
	_tempTex			= 0;
	_tempFBO			= 0;

	_tempTexPairs		= nullptr;
	_tempFBOPairs		= nullptr;

	_tempTexWithMask	= nullptr;
	_tempFBOTexWithMask = nullptr;
	_pawn = _character = nullptr;
}

DeformableMeshInstance::~DeformableMeshInstance()
{
	if(_tempFBO > 0)
	{
		glDeleteBuffers(1, &_tempFBO);
	}

	if(_tempTex > 0)
	{
		SmartBody::util::log("Deleting _tempTex #%d", _tempTex);
		glDeleteTextures(1, &_tempTex);
	}

	if(_tempFBOPairs != nullptr)
	{
		glDeleteBuffers((GLsizei)_mesh->blendShapeMap.size(), _tempFBOPairs);
	}
	delete _tempFBOPairs;

	if(_tempTexPairs != nullptr)
	{
		glDeleteTextures((GLsizei)_mesh->blendShapeMap.size(), _tempTexPairs);
		std::cerr << "Deleting _tempTexPairs\n";
	}
	delete _tempTexPairs;
}


void DeformableMeshInstance::setPawn(SmartBody::SBPawn* pawn)
{
	//if (_skeleton)
	//	_skeleton->unref();
	if (pawn)
		_skeleton = pawn->getSkeleton();
	if (_skeleton)
		_skeleton->ref();
	_pawn = pawn;
	_character = dynamic_cast<SmartBody::SBCharacter*>(pawn);
	updateJointList();
}



void DeformableMeshInstance::GPUblendShapes(glm::mat4x4 translation, glm::mat4x4 rotation)
{
#if WIN_BUILD
	DeformableMesh * _mesh		= this->getDeformableMesh();
	
	bool showMasks = false;

	SrSnModel* writeToBaseModel = nullptr;
	SrSnModel* baseModel		= nullptr;

	int tex_h = 1024;
	int tex_w = 1024;

	// find the base shape from static meshes
	std::map<std::string, std::vector<SrSnModel*> >::iterator mIter;

	//	Initializes vector of wieghts, of size (#shapes) 
	if (_mesh->blendShapeMap.begin() == _mesh->blendShapeMap.end())
		return;
	std::vector<float> weights(_mesh->blendShapeMap.begin()->second.size(), 0);

	//	Initializes vector of wieghts, of size (#shapes) each shape got a texture
	std::vector<GLuint> texIDs(_mesh->blendShapeMap.begin()->second.size(), 0);

	std::vector<std::string> texture_names(_mesh->blendShapeMap.begin()->second.size());
	std::vector<std::string> textureFileNames(_mesh->blendShapeMap.begin()->second.size());

	for (mIter = _mesh->blendShapeMap.begin(); mIter != _mesh->blendShapeMap.end(); ++mIter)
	{
		bool foundBaseModel = false;
		std::vector<SrSnModel*>& targets = (*mIter).second;
		for (size_t i = 0; i < _mesh->dMeshStatic_p.size(); ++i)
		{
			if (strcmp(_mesh->dMeshStatic_p[i]->shape().name, mIter->first.c_str()) == 0)
			{
				//	If base shape, copies pointer to _mesh->dMeshStatic (here is where the result resulting vertices position are stored)
				writeToBaseModel = _mesh->dMeshStatic_p[i];
				break;
			}
		}

		if (writeToBaseModel == nullptr)
		{
			//SmartBody::util::log("base model to write to cannot be found");
			if (_mesh->dMeshStatic_p.size() == 0 || targets.size() == 0)
				continue;
			if (_mesh->dMeshStatic_p[0]->shape().V.size() == targets[0]->shape().V.size())
			{
				// can't find the base model, assuming it's the first mesh
				//SmartBody::util::log("Can't find BlendShape BaseModel, will assume it's the first mesh.");
				writeToBaseModel = _mesh->dMeshStatic_p[0];
				//readBaseModel = _mesh->dMeshDynamic_p[0];
			}
		}
		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if (strcmp(mIter->first.c_str(), (const char*)mIter->second[i]->shape().name) == 0)
			{
				baseModel		= mIter->second[i];
				foundBaseModel	= true;
				break;
			}
		}
		if (baseModel == nullptr)
		{
			SmartBody::util::log("original base model cannot be found");
			continue;
		}

		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if (!mIter->second[i])
				continue;

			float w = 0.0f;
			float wLimit = 1.0f;
			// get weight
			std::stringstream ss;
			ss << "blendShape.channelName." << (const char*)mIter->second[i]->shape().name;
			std::stringstream ss1;
			ss1 << "blendShape.channelWeightLimit." << (const char*)mIter->second[i]->shape().name;
					
			if (_character->hasAttribute(ss1.str()))
			{
				wLimit = (float)_character->getDoubleAttribute(ss1.str());
			}

			if (_character->hasAttribute(ss.str()))
			{
				const std::string& mappedCName	= _character->getStringAttribute(ss.str());
				SmartBody::SBSkeleton* sbSkel	= _character->getSkeleton();
				if (sbSkel && mappedCName != "")
				{
					SmartBody::SBJoint* joint = sbSkel->getJointByName(mappedCName);
					if (joint)
					{
						SrVec pos = joint->getPosition();
						w = pos.x;
						//SmartBody::util::log("shape %s(%s) with weight %f", (const char*)mIter->second[i]->shape().name, mappedCName.c_str(), w);
						// clamp
						//if (w > wLimit)
						//	w = wLimit;
						
						// multiplier
						w = w * wLimit;
					}
				}
			}
			else
				continue;

			// Stores weights of each face
			weights[i]		= w;
		}

		// Starts computing blended textures
		for (size_t i = 0; i < mIter->second.size(); ++i)
		{
			if (!mIter->second[i])
				continue;

			//	Gets the map of (material name, texture) for the current mesh
			std::vector<std::string> materials;
			std::map<std::string, std::string> textures_map = mIter->second[i]->shape().mtlTextureNameMap;
			for(std::map<std::string,std::string>::iterator it = textures_map.begin(); it != textures_map.end(); ++it) {
				materials.emplace_back(it->first);
			}

			//	In a face there will be just one texture, material name will be always the first
			std::string matName = "";
			if (materials.size() > 0)
				matName = materials[0];

			// If base model
			if (strcmp(mIter->first.c_str(), (const char*)mIter->second[i]->shape().name) == 0)
			{
				std::string fileName = (std::string)mIter->second[i]->shape().mtlTextureNameMap[matName];
				SbmTexture* tex		= SbmTextureManager::singleton().findTexture(fileName.c_str());
				if (tex)
				{
					texIDs[i]		= tex->getID();
					texture_names[i]= fileName;
					textureFileNames[i] = tex->getFileName();
					//std::cerr << "Retriving texture " << matName << "\ttexIDs[" << i << "]: " << texIDs[i] << "\n";

					tex_h			= tex->getHeight();
					tex_w			= tex->getWidth();
				} 
				else
				{
					texIDs[i] = 0;
				}
				continue;	// don't do anything about base model
			}

			// Rest of the models
			std::string fileName = (std::string)mIter->second[i]->shape().mtlTextureNameMap[matName];
			SbmTexture* tex		= SbmTextureManager::singleton().findTexture(fileName.c_str());
			if (tex)
			{
				texIDs[i]		= tex->getID();
				
				texture_names[i]= fileName;
				textureFileNames[i] = tex->getFileName();
				//std::cout << "Retriving texture " << matName << "\ttexIDs[" << i << "]: " << texIDs[i] << "\n";
			}
			else
			{
				texIDs[i] =  0;
			}
		}
	}

	if(tex_w > 2048)
		tex_w = 2048;

	if(tex_h > 2048)
		tex_h = 2048;

	if(_tempFBO == 0) 
	{
		glGenFramebuffersEXT(1, &_tempFBO);
	}


	// Aux textures used when calling BlendAllAppearancesPairwise to store temporary results for texture blending pair wise
	if(_tempTexPairs == nullptr)
	{
		_tempTexPairs = new GLuint[weights.size()];
		glGenTextures(weights.size(), _tempTexPairs);
		for(unsigned int i=0; i<weights.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, _tempTexPairs[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(__ANDROID__) || defined(SB_IPHONE) || defined(EMSCRIPTEN)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_FLOAT, nullptr);
#endif
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	if(_tempFBOTexWithMask == nullptr)
	{
		_tempFBOTexWithMask = new GLuint[weights.size()];
		glGenFramebuffers(weights.size(), _tempFBOTexWithMask);
	}

	// If images with masks in the alpha channel have not been created
	if(_tempTexWithMask == nullptr)
	{
		_tempTexWithMask = new GLuint[weights.size()];
		glGenTextures(weights.size(), _tempTexWithMask);
		for(unsigned int i=0; i<weights.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, _tempTexWithMask[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if !defined(ANDROID_BUILD) && !defined(EMSCRIPTEN)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
#endif
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Adds masking images to the alpha channel of the textures
		SbmBlendTextures::ReadMasks(_tempFBOTexWithMask, _tempTexWithMask, weights, texIDs, texture_names, textureFileNames, SbmBlendTextures::getShader("ReadMasks"), tex_w, tex_h); 
	}



	if (texIDs.size() > 0 && texIDs[0] != 0)
	{

		//SbmShaderProgram::printOglError("texIDs.size() > 0 ");

		// New attempt to blend textures with masks (also renders a face). It uses the _tempTexWithMask, which are the texture maps with the masking encoded in its ALPHA channel.
		// The _tempTexWithMask texture were created above in the SbmBlendTextures::ReadMasks call
		//SbmBlendTextures::BlendGeometryWithMasks( _tempFBOTexWithMask, weights, _tempTexWithMask, texture_names, this,  SbmBlendTextures::getShader("BlendGeometryWithMasks"), translation, rotation);
		
		SbmBlendTextures::BlendGeometryWithMasksFeedback( _tempFBOTexWithMask, weights, _tempTexWithMask, texture_names, this,  SbmBlendTextures::getShader("BlendGeometryWithMasksFeedback"), translation, rotation);
		//SbmBlendTextures::BlendTextureWithMasks(_tempFBO, _tempTexPairs[0], weights, _tempTexWithMask, texture_names, this,  SbmBlendTextures::getShader("BlendAllTexturesWithMask"),tex_w, tex_h);
		SbmBlendTextures::RenderGeometryWithMasks( _tempFBOTexWithMask, weights, _tempTexWithMask, texture_names, this,  SbmBlendTextures::getShader("RenderGeometryWithMasks"), translation, rotation);

		// Blends geometry and texture in the same GLSL (also renders a face) (this does NOT use masking)
		//SbmBlendTextures::BlendGeometry( _tempFBOPairs, weights, texIDs, texture_names, this,  SbmBlendTextures::getShader("BlendGeometry"));

		// Computes blended texture pairwise, and saves it into _tempTexPairs[0], which is going to be used later as a texture (in the normal blendshape pipeline)
		//SbmBlendTextures::BlendAllAppearancesPairwise( _tempFBOPairs, _tempTexPairs, weights, texIDs, texture_names, SbmBlendTextures::getShader("Blend_All_Textures_Pairwise"), tex_w, tex_h);
	}
#endif
}

void DeformableMeshInstance::blendShapes()
{
	
	//SbmShaderProgram::printOglError("DeformableMeshInstance::blendShapes() #0 ");

	if (!_character)
	{
		return;
	}

	SrSnModel* writeToBaseModel = nullptr;
	SrSnModel* readBaseModel = nullptr;
	SrSnModel* baseModel = nullptr;

	// find the base shape from static meshes
	for (auto & mIter : _mesh->blendShapeMap)
	{
		bool foundBaseModel = false;
		std::string baseModelName = mIter.first;
		std::vector<SrSnModel*>& targets = mIter.second;

		for (size_t i = 0; i < _mesh->dMeshStatic_p.size(); ++i)
		{
			if (strcmp(_mesh->dMeshStatic_p[i]->shape().name, baseModelName.c_str()) == 0)
			{
				//	If base shape, copies pointer to _mesh->dMeshStatic (here is where the result resulting vertices position are stored)
				writeToBaseModel = _mesh->dMeshStatic_p[i];
				readBaseModel = _mesh->dMeshDynamic_p[i];
				break;
			}
		}

		if (writeToBaseModel == nullptr)
		{
			//SmartBody::util::log("base model to write to cannot be found");
			//continue;
			if (_mesh->dMeshStatic_p.empty() || targets.empty())
				continue;
			if (_mesh->dMeshStatic_p[0]->shape().V.size() == targets[0]->shape().V.size())
			{
				// can't find the base model, assuming it's the first mesh
				//SmartBody::util::log("Can't find BlendShape BaseModel, will assume it's the first mesh.");
				writeToBaseModel = _mesh->dMeshStatic_p[0];
				readBaseModel = _mesh->dMeshDynamic_p[0];
			}
		}
		for (auto & target : targets)
		{
			if (strcmp(mIter.first.c_str(), (const char*) target->shape().name) == 0)
			{
				baseModel		= target;
				foundBaseModel	= true;
				break;
			}
		}
		if (baseModel == nullptr)
		{
			SmartBody::util::log("original base model cannot be found");
			continue;
		}
		

		//SrArray<SrPnt>& neutralV = baseModel->shape().V;
		std::vector<SrVec>& neutralV = baseModel->shape().V;
		std::vector<SrPnt>& neutralN = baseModel->shape().N;
		std::vector<SrVec> newV = neutralV;
		std::vector<SrPnt> newN = neutralN;
		
		//std::vector<SrVec> newV = readBaseModel->shape().V;
		//std::vector<SrPnt> newN = readBaseModel->shape().N;
		
		double blendShapeProximity = gwiz::epsilon4();
		if (SmartBody::SBScene::getScene()->hasAttribute("blendshapeProximity"))
		{
			blendShapeProximity = SmartBody::SBScene::getScene()->getDoubleAttribute("blendshapeProximity");
		}

		if (foundBaseModel && 
			_character->getBoolAttribute("useOptimizedBlendShapes"))
		{
			auto optIter = _mesh->optimizedBlendShapeData.find(baseModelName);
			if (optIter == _mesh->optimizedBlendShapeData.end())
			{
				_mesh->optimizedBlendShapeData.insert(std::pair<std::string, std::vector<BlendShapeData> >(baseModelName, std::vector<BlendShapeData>()));
				optIter = _mesh->optimizedBlendShapeData.find(baseModelName);
			}

			std::vector<BlendShapeData>& optimizedShapeData = (*optIter).second;
			if (optimizedShapeData.size() != targets.size())
			{
				SmartBody::util::log("Optimizing blend shapes. Only have %d/%d shapes.", optimizedShapeData.size(), targets.size());
				optimizedShapeData.clear();
				// optimize the blend shape maps as needed
				for (size_t i = 0; i < targets.size(); ++i)
				{
					optimizedShapeData.emplace_back();
					if (i == 0)
					{
						continue;
					}
					if (!targets[i])
					{
						continue;
					}
					BlendShapeData& blendData = optimizedShapeData[i];
					//SrArray<SrPnt>& visemeV = mIter->second[i]->shape().V;
					std::vector<SrVec>& visemeV = targets[i]->shape().V;
					std::vector<SrPnt>& visemeN = targets[i]->shape().N;


					SrVec vVec;
					SrVec nVec;
					for (unsigned int v = 0; v < visemeV.size(); ++v)
					{
						vVec = visemeV[v] - neutralV[v];
						if (fabs(vVec[0]) >  blendShapeProximity ||
							fabs(vVec[1]) >  blendShapeProximity ||
							fabs(vVec[2]) >  blendShapeProximity )
						{
							std::pair<int, SrVec> temp;
							temp.first = v;
							temp.second = vVec;
							blendData.diffV.emplace_back(temp);
						}
					}
					for (unsigned int n = 0; n < visemeN.size(); ++n)
					{
						nVec = visemeN[n] - neutralN[n];
						if (fabs(nVec[0]) >  blendShapeProximity ||
							fabs(nVec[1]) >  blendShapeProximity ||
							fabs(nVec[2]) >  blendShapeProximity)	
						{
							blendData.diffN.emplace_back(n, nVec);
						}
					}
					SmartBody::util::log("Optimized blend %s has %d/%d vertices, %d/%d normals.", (const char*) targets[i]->shape().name, blendData.diffV.size(), visemeV.size(), blendData.diffN.size(), visemeN.size());
				}
			}
		}


		//	Initializes vector of wieghts, of size (#shapes) 
		std::vector<float> weights(targets.size(), 0);


		//	Initializes vector of wieghts, of size (#shapes) each shape got a texture
		std::vector<GLuint> texIDs(targets.size(), 0);

		std::vector<std::string> texture_names(targets.size());
		
		int tex_h = 1024;
		int tex_w = 1024;
		//std::string weightsStr = "weights = ";
		for (size_t i = 0; i < targets.size(); ++i)
		{
			if (!targets[i])
				continue;

			float w = 0.0f;
			float wLimit = 1.0f;
			// get weight
			std::stringstream ss;
			ss << "blendShape.channelName." << (const char*) targets[i]->shape().name;
			std::stringstream ss1;
			ss1 << "blendShape.channelWeightLimit." << (const char*) targets[i]->shape().name;
					
			if (_character->hasAttribute(ss1.str()))
			{
				wLimit = (float)_character->getDoubleAttribute(ss1.str());
			}

			if (_character->hasAttribute(ss.str()))
			{
				const std::string& mappedCName	= _character->getStringAttribute(ss.str());
				auto sbSkel	= _character->getSkeleton();
				if (sbSkel && !mappedCName.empty())
				{
					//SmartBody::util::log("blendShape, mappedCName = %s", mappedCName.c_str());
					SmartBody::SBJoint* joint = sbSkel->getJointByName(mappedCName);
					if (joint)
					{
						SrVec pos = joint->getPosition();
						w = pos.x;
						//SmartBody::util::log("shape %s(%s) with weight %f", (const char*)mIter->second[i]->shape().name, mappedCName.c_str(), w);
						// clamp
						//if (w > wLimit)
						//	w = wLimit;
						
						// multiplier
						w = w * wLimit;
					}
					else
					{
						//SmartBody::util::log("mappedCName = %s, can't find joint...", mappedCName.c_str());
					}
				}
				//SmartBody::util::log("character has channel = '%s', mappedCName = '%s'", ss.str().c_str(), mappedCName.c_str());
			}
			else
			{
				//SmartBody::util::log("Error, character doesn't have channel = %s", ss.str().c_str());
				continue;
			}

			// Stores weights of each face
			weights[i]		= w;
			//if (fabs(w) > gwiz::epsilon4())
			//	weightsStr += " " + boost::lexical_cast<std::string>(w);

			//std::cerr << "weights[" << i << "]: " << w << "\n";
			//SmartBody::util::log("weights[%s] = %f", (const char*)targets[i]->shape().name, w);
			if (fabs(w) > gwiz::epsilon4())	// if it has weight
			{
				//SmartBody::util::log("blend in %s with weight %f", (const char*)mIter->second[i]->shape().name, w);
				//SrArray<SrPnt>& visemeV = mIter->second[i]->shape().V;
				std::vector<SrVec>& visemeV = targets[i]->shape().V;
				std::vector<SrPnt>& visemeN = targets[i]->shape().N;
				if (visemeV.size() != neutralV.size())
				{
					SmartBody::util::log("number of vertices for %s (%d) is not same as neutral (%d)", baseModelName.c_str(), visemeV.size(), neutralV.size());
					continue;
				}
				if (visemeN.size() != neutralN.size())
				{
					SmartBody::util::log("number of normals for %s (%d) is not same as neutral (%d)", baseModelName.c_str(), visemeN.size(), neutralN.size());
					continue;
				}

				if (_character->getBoolAttribute("useOptimizedBlendShapes"))
				{
					// loop through a shorter list of different vertices and normals
					auto optIter = _mesh->optimizedBlendShapeData.find(baseModelName);
					std::vector<BlendShapeData>& optimizedShapeData = (*optIter).second;
					BlendShapeData& blendData = optimizedShapeData[i];
					size_t vSize = optimizedShapeData[i].diffV.size();
					for (size_t v = 0; v < vSize; ++v)
					{
						int index	= blendData.diffV[v].first;
						SrVec& diff = blendData.diffV[v].second;
						newV[index] = newV[index] + diff * w;
					}
					size_t nSize = optimizedShapeData[i].diffN.size();
					for (size_t n = 0; n < nSize; ++n)
					{
						int index	= blendData.diffN[n].first;
						SrVec& diff = blendData.diffN[n].second;
						newN[index] = newN[index] + diff * w;
					}
				}
				else
				{
					// loop through all vertices and normals
					for (unsigned int v = 0; v < visemeV.size(); ++v)
					{
						SrPnt diff = visemeV[v] - neutralV[v];
						if (fabs(diff[0]) >  gwiz::epsilon4() ||
							fabs(diff[1]) >  gwiz::epsilon4() ||
							fabs(diff[2]) >  gwiz::epsilon4())	
							newV[v] = newV[v] + diff * w;
					}
					for (unsigned int n = 0; n < visemeN.size(); ++n)
					{
						SrPnt diff = visemeN[n] - neutralN[n];
						if (fabs(diff[0]) >  gwiz::epsilon4() ||
							fabs(diff[1]) >  gwiz::epsilon4() ||
							fabs(diff[2]) >  gwiz::epsilon4())	
							newN[n] = newN[n] + diff * w;
					}
				}
			}
		}
		for (auto & n : newN)
		{
			n.normalize();
		}

		//SmartBody::util::log("%s", weightsStr.c_str());

		// END OF SECOND ATTEMPT
		writeToBaseModel->shape().V = newV;
		writeToBaseModel->shape().N = newN;
		writeToBaseModel->changed(true);

		if (_character->getStringAttribute("texturesType") != "dynamic")
			continue;


		

		// Starts computing blended textures
		for (size_t i = 0; i < targets.size(); ++i)
		{
			if (!mIter.second[i])
				continue;

			//	Gets the map of (material name, texture) for the current mesh
			std::vector<std::string> materials = targets[i]->shape().mtlnames;
#if 0
			std::map<std::string, std::string> textures_map = mIter->second[i]->shape().mtlTextureNameMap;
			for(std::map<std::string,std::string>::iterator it = textures_map.begin(); it != textures_map.end(); ++it) {
				materials.emplace_back(it->first);
			}
#endif
			//	In a face there will be just one texture, material name will be always the first
			std::string matName;
			if (!materials.empty())
				matName = materials[0];

			// If base model
			if (strcmp(mIter.first.c_str(), (const char*) targets[i]->shape().name) == 0)
			{
				std::string fileName = (std::string) targets[i]->shape().mtlTextureNameMap[matName];
				auto tex		= SbmTextureManager::singleton().findTexture(fileName.c_str());
				if (tex)
				{
					texIDs[i]		= tex->getID();
					texture_names[i]= fileName;
					//std::cerr << "Retriving texture " << matName << "\ttexIDs[" << i << "]: " << texIDs[i] << "\n";

					tex_h			= tex->getHeight();
					tex_w			= tex->getWidth();
				} 
				else
				{
					texIDs[i] = 0;
				}
				continue;	// don't do anything about base model
			}

			// Rest of the models
			std::string fileName = (std::string) targets[i]->shape().mtlTextureNameMap[matName];
			auto tex		= SbmTextureManager::singleton().findTexture(fileName.c_str());
			if (tex)
			{
				texIDs[i]		= tex->getID();
				
				texture_names[i]= fileName;
				//std::cout << "Retriving texture " << matName << "\ttexIDs[" << i << "]: " << texIDs[i] << "\n";
			}
			else
			{
				texIDs[i] =  0;
			}
		}

	

		//	Here I try to blend the faces two at a time. This way I avoid hardcoded constant vector size.
#if !defined(SB_IPHONE) 

		SbmTextureManager& texManager = SbmTextureManager::singleton();
		if(_tempFBOPairs == nullptr)
		{
			_tempFBOPairs = new GLuint[weights.size()];
			//glGenFramebuffers(weights.size(), _tempFBOPairs);
			std::string FBOName = "TempFBO";
			for (unsigned int i=0;i<weights.size();i++)
			{
				std::string tempName = FBOName + "_weight" + std::to_string(i);
				texManager.createFBO(tempName.c_str());
		}
		}
		std::string FBOName = "TempFBO";
		for (unsigned int i=0;i<weights.size();i++)
		{
			std::string tempName = FBOName + "_weight" + boost::lexical_cast<std::string>(i);
			_tempFBOPairs[i] = texManager.findFBO(tempName.c_str());
		}

//		SbmShaderProgram::printOglError("HERE #4 ");

		if(tex_w > 2048)
			tex_w = 2048;

		if(tex_h > 2048)
			tex_h = 2048;

		
		// Aux textures used when calling BlendAllAppearancesPairwise to store temporary results for texture blending pair wise
		if(_tempTexPairs == nullptr)
		{
#define USE_TEXTURE_MANAGER 1
#if USE_TEXTURE_MANAGER
			_tempTexPairs = new GLuint[weights.size()];
			std::string meshName = this->getDeformableMesh()->getName();
			
			for (unsigned int i=0;i<weights.size();i++)
			{
				std::string tempTexName = meshName + "_weight" + std::to_string(i);
				texManager.createWhiteTexture(tempTexName.c_str(), tex_w, tex_h);
			}
#else
			glGenTextures(weights.size(), _tempTexPairs);
			for(int i=0; i<weights.size(); i++) {
				glBindTexture(GL_TEXTURE_2D, _tempTexPairs[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(__ANDROID__) || defined(SB_IPHONE) || defined(EMSCRIPTEN)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_FLOAT, nullptr);
#endif
				glBindTexture(GL_TEXTURE_2D, 0);
			}
#endif
		}
		for (unsigned int i=0;i<weights.size();i++)
		{
			std::string meshName = this->getDeformableMesh()->getName();
			std::string tempTexName = meshName + "_weight" + std::to_string(i);
			auto tex = texManager.findTexture(tempTexName.c_str());
			if (tex)
			{
				_tempTexPairs[i] = tex->getID();
			}
			else
			{
				SmartBody::util::log("Error : BlendShape Texture, '%s' does not exist.", tempTexName.c_str());
			}
		}		

		if (!texIDs.empty() && texIDs[0] != 0)
		{		
#if !defined(EMSCRIPTEN)
			// Computes blended texture pairwise, and saves it into _tempTexPairs[0], which is going to be used later as a texture (in the normal blendshape pipeline)
			SbmBlendTextures::BlendAllAppearancesPairwise( _tempFBOPairs, _tempTexPairs, weights, texIDs, texture_names, SbmBlendTextures::getShader("Blend_All_Textures_Pairwise"), tex_w, tex_h);
#endif
		}
#endif

	}

	return;
}


void DeformableMeshInstance::setDeformableMesh( DeformableMesh* mesh )
{
	//SmartBody::util::log("setDeformableMesh to be %s", mesh->meshName.c_str());
	_mesh = mesh;
	_mesh->buildSkinnedVertexBuffer(); // make sure the deformable mesh has vertex buffer
	_deformPosBuf.resize(_mesh->posBuf.size()); // initialized deformation posBuffer
	_restPosBuf.resize(_mesh->posBuf.size());
	_deformNormalBuf.resize(_mesh->normalBuf.size());
	_deformTangentBuf.resize(_mesh->tangentBuf.size());	
	_deformNormalBuf = _mesh->normalBuf;
	_deformTangentBuf = _mesh->tangentBuf;
	for (unsigned int i=0;i<_deformPosBuf.size();i++)
	{
		_restPosBuf[i] = _mesh->posBuf[i];
		_deformPosBuf[i] = _mesh->posBuf[i];		
	}
	updateJointList();
}

void DeformableMeshInstance::updateJointList()
{
	if (!_skeleton || !_mesh) return;
	std::vector<SkinWeight*>& skinWeights = _mesh->skinWeights;
	_boneJointList.clear();
	for (auto skinWeight : skinWeights)
	{
			SkJointList jlist;
		for (auto & jname : skinWeight->infJointName)
		{
				SkJoint* joint = _skeleton->search_joint(jname.c_str());
			jlist.emplace_back(joint);
		}
		_boneJointList.emplace_back(jlist);
	}
}

void DeformableMeshInstance::setVisibility(int deformableMesh)
{
	if (deformableMesh != -1)
	{		
		_updateMesh = deformableMesh != 0;
		meshVisibleType = deformableMesh;
	}
}

void DeformableMeshInstance::updateTransformBuffer()
{
	if (!_mesh) return;
  size_t transformSize = std::max(size_t(120), _mesh->boneJointIdxMap.size());
	if (transformBuffer.size() != transformSize)
		transformBuffer.resize(transformSize);
	std::map<std::string,int>& boneIdxMap = _mesh->boneJointIdxMap;
	std::map<std::string,int>::iterator mi;	
	for ( mi  = boneIdxMap.begin();
		mi != boneIdxMap.end();
		mi++)	
	{
		int idx = mi->second;
		SkJoint* joint = _skeleton->search_joint(mi->first.c_str());//boneJointList[i];		
		if (!joint)
			continue;
		SrMat bindPoseMat = _mesh->bindPoseMatList[idx];
		bindPoseMat.set_translation(bindPoseMat.get_translation()*_meshScale[0]);
		transformBuffer[idx] = bindPoseMat*joint->gmat();			
	}
}


void DeformableMeshInstance::updateSkin( const std::vector<SrVec>& restPos, std::vector<SrVec>& deformPos )
{
	_skeleton->update_global_matrices();
	updateTransformBuffer();

	for (unsigned int i=0;i<restPos.size();i++)
	{
		SrVec vPos = restPos[i]*_meshScale[0];
		SrVec vSkinPos = SrVec(0,0,0);	
		for (int k=0;k<_mesh->boneCountBuf[i];k++)		
		{	

#if USE_SKIN_WEIGHT_SIZE_8
			int a = (k<4) ? 0 : 1;
			int b = k%4;		
#else
			if (k >= 4)
				break;
			int a = 0;
			int b = k;			
#endif
			vSkinPos += (vPos*transformBuffer[_mesh->boneIDBuf[a][i][b]])*_mesh->boneWeightBuf[a][i][b];
		}
		deformPos[i] = vSkinPos;
	}
}


SBAPI void DeformableMeshInstance::updateFast()
{
	if (!_updateMesh)	return;
	if (!_skeleton || !_mesh) return;	
	if (isStaticMesh()) return; // not update the buffer if it's a static mesh
	//SmartBody::util::log("Update Skin...");
	//updateSkin(_mesh->posBuf,  _deformPosBuf);
	updateSkin(_restPosBuf, _deformPosBuf);

	SrVec tempPos = _deformPosBuf[150];
	//SmartBody::util::log("deformPos = %f %f %f",tempPos[0],tempPos[1],tempPos[2]);
}


void DeformableMeshInstance::update()
{
	//blendShapes();
	//SmartBody::util::log("Update deformable mesh");
#define RECOMPUTE_NORMAL 0
	if (!_updateMesh)
	{
		//SmartBody::util::log("!_updateMesh...");
		return;
	}
	if (!_skeleton || !_mesh)
	{
		//SmartBody::util::log("No skeleton or no mesh...");
		return;
	}
	if (isStaticMesh())
	{
		//SmartBody::util::log("is static mesh...");
		return; // not update the buffer if it's a static mesh
	}
	_skeleton->update_global_matrices();
#if defined(EMSCRIPTEN)
	updateTransformBuffer();
	return;
#endif
	//SmartBody::util::log("Update DeformableMeshInstance");
	updateFast();
	return;

//  int maxJoint = -1;
//  std::vector<SkinWeight*>& skinWeights = _mesh->skinWeights;
//  if (skinWeights.size() != _boneJointList.size()) updateJointList();
//  std::map<int,std::vector<int> >& vtxNewVtxIdxMap = _mesh->vtxNewVtxIdxMap;
//  int iVtx = 0;
//
//#ifdef RECOMPUTE_NORMAL
//  int iNormalVtx = 0;
//#endif
//
//  for (unsigned int skinCounter = 0; skinCounter < skinWeights.size(); skinCounter++)
//  {
//    SkinWeight* skinWeight = skinWeights[skinCounter];
//    SkJointList& jointList = _boneJointList[skinCounter];
//    std::map<std::string, std::vector<std::string> >::iterator iter = _mesh->morphTargets.find(skinWeight->sourceMesh);
//    int pos;
//    int globalCounter = 0;
//    if (iter != _mesh->morphTargets.end() && iter->second.size() > 0)  pos = _mesh->getMesh(iter->second[0]);
//    else                                pos = _mesh->getMesh(skinWeight->sourceMesh);
//    if (pos != -1)
//    {
//      //SmartBody::util::log("update skinning, skin counter = %d", skinCounter);
//      SrSnModel* dMeshStatic = _mesh->dMeshStatic_p[pos];
//      //SrSnModel* dMeshDynamic = dynamicMesh[pos];
//      int numVertices = dMeshStatic->shape().V.size();
//      for (int i = 0; i < numVertices; i++)
//      {
//        if (i >= (int) skinWeight->numInfJoints.size())
//          continue;
//        int numOfInfJoints = skinWeight->numInfJoints[i];
//        if (numOfInfJoints > maxJoint)
//          maxJoint = numOfInfJoints;
//        SrVec& skinLocalVec = dMeshStatic->shape().V[i];
//        SrVec finalVec;    
//
//        if (numOfInfJoints >= 5 )
//        {
//          int a = 0; // dummy for break point
//        }
//
//
//        for (int j = 0; j < numOfInfJoints; j++)
//        {
//          //std::string jointName = skinWeight->infJointName[skinWeight->jointNameIndex[globalCounter]];  
//          int jointIndex = skinWeight->jointNameIndex[globalCounter];
//          SkJoint* curJoint = jointList[jointIndex];//skinWeight->infJoint[skinWeight->jointNameIndex[globalCounter]];
//          if (curJoint == nullptr) continue;
//            
//          const SrMat& gMat = curJoint->gmat();
//          SrMat& invBMat = skinWeight->bindPoseMat[skinWeight->jointNameIndex[globalCounter]];  
//          double jointWeight = skinWeight->bindWeight[skinWeight->weightIndex[globalCounter]];
//          globalCounter ++;
//          SrVec transformVec = _meshScale[0]*(skinLocalVec * skinWeight->bindShapeMat * invBMat);
//          SrVec finalTransformVec = (transformVec  * gMat);
//          finalVec = finalVec + (float(jointWeight) * finalTransformVec);              
//        }
//        
//        _deformPosBuf[iVtx] = finalVec;
//        if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
//        {
//          std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
//          // copy related vtx components 
//          for (unsigned int k=0;k<idxMap.size();k++)
//          {
//            _deformPosBuf[idxMap[k]] = finalVec;
//          }
//        }          
//        iVtx++;
//#if RECOMPUTE_NORMAL
//        if (_recomputeNormal)
//          dMeshDynamic->shape().V[i] = finalVec;
//#endif
//      }
//#if RECOMPUTE_NORMAL
//      if (_recomputeNormal)
//      {
//        dMeshDynamic->shape().computeNormals();
//        for (int i = 0; i < numVertices; i++)
//        {
//          SrVec finalN = dMeshDynamic->shape().N[i];
//          _mesh->normalBuf[iNormalVtx] = finalN;
//          if (vtxNewVtxIdxMap.find(iNormalVtx) != vtxNewVtxIdxMap.end())
//          {
//            std::vector<int>& idxMap = vtxNewVtxIdxMap[iNormalVtx];
//            // copy related vtx components 
//            for (unsigned int k=0;k<idxMap.size();k++)
//            {
//              _mesh->normalBuf[idxMap[k]] = finalN;
//            }
//          }
//          iNormalVtx++;
//        }
//          
//      }
//#endif
//      //dMeshDynamic->changed(true);  
//    }
//    else
//      continue;
//  }
//  _recomputeNormal = false;
}

int DeformableMeshInstance::getVisibility()
{
	return meshVisibleType;
}

void DeformableMeshInstance::setMeshScale( SrVec scale )
{
	_meshScale = scale;
}

boost::intrusive_ptr<SmartBody::SBSkeleton> DeformableMeshInstance::getSkeleton()
{
	return {dynamic_cast<SmartBody::SBSkeleton*>(_skeleton.get())};
}

void DeformableMeshInstance::setToStaticMesh( bool isStatic )
{
	_isStaticMesh = isStatic;
}

SBAPI bool DeformableMeshInstance::isStaticMesh()
{
	if (!_mesh) return true;
	return (_isStaticMesh || !_mesh->isSkinnedMesh());
}

SBAPI void DeformableMeshInstance::blendShapeStaticMesh()
{
	//return;
	//SmartBody::util::log("Running blendShapeStaticMesh");

	//SbmShaderProgram::printOglError("DeformableMeshInstance::blendShapeStaticMesh() #0 ");

	if (!_mesh) 
		return;
	//SmartBody::util::log("Mesh blendshape size = %d", _mesh->blendShapeMap.size());
	if (_mesh->blendShapeMap.size() == 0)
		return;

	DeformableMeshInstance::blendShapes();
	bool mergeBoundary = false;

	if (_character)
		mergeBoundary = _character->getBoolAttribute("blendShape.mergeBoundary");

	for (auto & mIter : _mesh->blendShapeMap)
	{
		SrSnModel* writeToBaseModel = nullptr;
		int vtxBaseIdx = 0;
		std::vector<SrSnModel*>& targets = mIter.second;
		for (auto & i : _mesh->dMeshStatic_p)
		{
			if (strcmp(i->shape().name, mIter.first.c_str()) == 0)
			{
				writeToBaseModel = i;
				break;
			}
			else
			{
				// skip vertices for this sub mesh
				vtxBaseIdx += i->shape().V.size();
			}
		}

		if (!writeToBaseModel)
		{
			if (_mesh->dMeshStatic_p.size() == 0 || targets.empty())
				continue;
			if (_mesh->dMeshStatic_p[0]->shape().V.size() == targets[0]->shape().V.size())
			{
				// can't find the base model, assuming it's the first mesh
				//SmartBody::util::log("Can't find BlendShape BaseModel, will assume it's the first mesh.");
				writeToBaseModel = _mesh->dMeshStatic_p[0];
				vtxBaseIdx = 0;
			}
			//continue;
		}

		std::map<int, std::vector<int> >& vtxNewVtxIdxMap = _mesh->vtxNewVtxIdxMap;
		std::map<int, std::vector<int> >& vtxBlendShapeVtxIdxMap = _mesh->blendShapeNewVtxIdxMap;
		SrModel& baseModel = writeToBaseModel->shape();

		std::vector<SrVec>& newPosBuf = (_mesh->isSkinnedMesh()) ? _restPosBuf : _deformPosBuf;
		std::vector<SrVec>& origPosBuf = _mesh->posBuf;
		for (unsigned int i = 0; i < baseModel.V.size(); i++)
		{
			int iVtx = vtxBaseIdx + i;
			SrVec& basePos = baseModel.V[i];
			newPosBuf[iVtx] = basePos;
			SrVec& origPos = origPosBuf[iVtx];
			if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
			{
				std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
				// copy related vtx components 
				for (int idx : idxMap)
				{
						newPosBuf[idx] = basePos;	// Here copies blended vertices position
				}
			}			
			//if (origPos[0] != basePos[0] || origPos[1] != basePos[1] || origPos[2] != basePos[2])
			//	SmartBody::util::log("iVtx = %d, orig Pos = %s, new Pos = %s", iVtx, origPosBuf[iVtx].toString().c_str(), basePos.toString().c_str());
			if (mergeBoundary && vtxBlendShapeVtxIdxMap.find(i) != vtxBlendShapeVtxIdxMap.end())
			{
			 	std::vector<int>& idxMap = vtxBlendShapeVtxIdxMap[i];
			 	// copy related vtx components 
			 	for (int idx : idxMap)
			 	{
			 			newPosBuf[idx]	= basePos;	// Here copies blended vertices position
			 	}
			}
		}
	}
	
	//SbmShaderProgram::printOglError("DeformableMeshInstance::blendShapeStaticMesh() #FINAL");
}

int DeformableMesh::getNumMeshes()
{
	return dMeshStatic_p.size();
}

std::string DeformableMesh::getMeshName(int index)
{
	if ((int) dMeshStatic_p.size() > index &&
		index >= 0)
		return (const char*) dMeshStatic_p[index]->shape().name;
	else
		return "";
}

SrModel& DeformableMesh::getStaticModel(int index)
{
	if ((int) dMeshStatic_p.size() > index &&
		index >= 0)
		return dMeshStatic_p[index]->shape();
	else
		return _emptyModel;
}

void DeformableMesh::copySkinWeights(DeformableMesh* fromMesh, const std::string& morphName)
{
	SmartBody::util::log("Start copy skin weights");
	// clear any existing skin weights
	for (auto weight : this->skinWeights)
	{
			delete weight;
	}
	this->skinWeights.clear();


	for (size_t w = 0; w < fromMesh->skinWeights.size(); w++)
	{
		SmartBody::util::log("copy weight %d...", w);
		auto* weight = new SkinWeight();
		weight->copyWeights(fromMesh->skinWeights[w], morphName);
		this->skinWeights.emplace_back(weight);
	}
	SmartBody::util::log("Finish copy skin weights");
}

void DeformableMesh::copyClosestSkinWeights(DeformableMesh* fromMesh)
{
	if (!fromMesh)
	{
		SmartBody::util::log("No mesh, skin weights will not be copied...");
		return;
	}

	// make sure that the skeletons match between the two deformable meshes
	auto* sbskeleton1 = dynamic_cast<SmartBody::SBSkeleton*>(skeleton);
	auto* sbskeleton2 = dynamic_cast<SmartBody::SBSkeleton*>(fromMesh->skeleton);
	
	if (sbskeleton1->getNumJoints() != sbskeleton2->getNumJoints())
	{
		SmartBody::util::log("Number of joints in skeleton %d does not match current mesh %d", sbskeleton1->getNumJoints(), sbskeleton2->getNumJoints());
		return;
	}

	// make sure there is the same number of submeshes
	if (this->dMeshStatic_p.size() != fromMesh->dMeshStatic_p.size())
	{
		SmartBody::util::log("Mesh has different number of mesNumber of joints in skeleton %d does not match current mesh %d", sbskeleton1->getNumJoints(), sbskeleton2->getNumJoints());
		return;
	}

	SmartBody::util::log("Start copy skin weights");
	// clear any existing skin weights
	for (auto weight : this->skinWeights)
	{
			delete weight;
	}
	skinWeights.clear();

	for (size_t m = 0; m < this->getNumMeshes(); m++)
	{
		auto* weight = new SkinWeight();
		skinWeights.emplace_back(weight);
		weight->copyWeights(fromMesh->skinWeights[m], (const char*) this->dMeshStatic_p[m]->shape().name);

		weight->numInfJoints.clear();
		weight->weightIndex.clear();
		weight->jointNameIndex.clear();

		std::vector<int> mapping;
		int count = 0;
		for (int numInfluences : fromMesh->skinWeights[m]->numInfJoints)
		{
			mapping.emplace_back(count);
				count += numInfluences;
		}

		for (int v = 0; v < this->dMeshStatic_p[m]->shape().V.size(); v++)
		{
			int closestIndex = fromMesh->dMeshStatic_p[m]->shape().getClosestIndex(this->dMeshStatic_p[m]->shape().V[v]);
			
			int numInfluences = fromMesh->skinWeights[m]->numInfJoints[closestIndex];
			weight->numInfJoints.emplace_back(numInfluences);
			int index = mapping[closestIndex];

			for (int i = 0; i < numInfluences; i++)
			{
				weight->weightIndex.emplace_back(fromMesh->skinWeights[m]->weightIndex[index + i]);
				weight->jointNameIndex.emplace_back(fromMesh->skinWeights[m]->jointNameIndex[index + i]);
			}
		}
	}
}
