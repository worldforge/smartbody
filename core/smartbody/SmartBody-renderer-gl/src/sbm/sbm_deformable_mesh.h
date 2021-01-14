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

#ifndef SBM_DEFORMABLE_MESH_H
#define SBM_DEFORMABLE_MESH_H

#include <sb/SBTypes.h>
#include <vector>
#include <string>
#include <map>
#include <sr/sr_sn_shape.h>
#include <sk/sk_skeleton.h>
#include <sr/sr_model.h>
#include <sb/SBAsset.h>
#include <sb/SBCharacter.h>

#include "glm/glm.hpp"
#include "nanoflann.hpp"

#define USE_SKIN_WEIGHT_SIZE_8 0

#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wingdi.h>
	#include <GL/ew.h>
#elif defined(SB_IPHONE)
#include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#include <OpenGL/gl.h>
//       #include <Carbon/Carbon.h>
       #define APIENTRY
#elif defined(EMSCRIPTEN)
#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#elif defined(__ANDROID__)
//#include <GLES/gl.h>
	#include <GLES3/gl3.h>
	//#include "wes_gl.h"
#else
#include <GL/glew.h>
#endif

typedef std::vector<SkJoint*> SkJointList;

namespace SmartBody {
class SBRenderAssetManager;
}

class SkinWeight
{
public:
	std::vector<std::string>	infJointName;	// name array
	std::vector<SkJoint*>		infJoint;         // corresponding joint for each infJointName
	std::vector<float>			bindWeight;		// weight array
	std::vector<SrMat>			bindPoseMat;	// each joint, binding pose transformation matrix
	SrMat						bindShapeMat;	// overall bind shape transformation matrix
	std::string					sourceMesh;		// skin Source Name
	std::vector<unsigned int>	numInfJoints;	// number of influenced joints for very vertex
	std::vector<unsigned int>	weightIndex;	// looking up the weight according to this index
	std::vector<unsigned int>	jointNameIndex;	// looking up the joint name according to this index
	std::vector<int>			indexCache;		// fast lookup for skin weights
	// skin weight buffer for easier access to skinning
	std::vector<SrVec4i> boneIDs;
	std::vector<SrVec4>  boneWeights;

public:
	SBAPI SkinWeight();
	
	SBAPI ~SkinWeight();
	SBAPI void normalizeWeights();
	SBAPI void copyWeights(SkinWeight* copy, const std::string& morphName);

	SBAPI void initWeights(std::string sourceMesh, std::vector<SrVec4i>& boneID, std::vector<SrVec4>& boneWeights, std::vector<std::string>& boneJointNameList, std::vector<SrMat>& bindPoseMatList);

	SBAPI void addWeight(SkinWeight* weight);
	SBAPI void mergeRedundantWeight(std::vector<int>& vtxIdxMap);
	SBAPI void buildSkinWeightBuf();

	SBAPI std::vector<std::string> getInfluenceJointNames() const;
	SBAPI int getNumVertices() const;
	SBAPI std::vector<int> getInfluenceJoints(int index);
	SBAPI std::vector<float> getInfluenceWeights(int index);
	SBAPI SrMat getBindShape() const;
	SBAPI SrMat getBindJoint(int jointIndex);
	SBAPI void createCache();
	SBAPI std::vector<int> getInfluencesByJointIndex(int index);




};

class SbmSubMesh
{
public:
	SrMaterial  material;
	bool isHair;
	std::string modelName; // the name of original SrModel
	std::string matName;
	std::string texName;
	std::string normalMapName;
	std::string specularMapName;	
	int numTri;
	std::vector<SrVec3i> triBuf;
	std::vector<int> faceIdxList;
};



struct MeshPointCloud
{	
	std::vector<SrVec>  pts;

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return pts.size(); }

	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline float kdtree_distance(const float *p1, const size_t idx_p2,size_t size) const
	{
		SrVec v1 = SrVec(p1[0],p1[1],p1[2]);		
		return (v1-pts[idx_p2]).norm2();
	}

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline float kdtree_get_pt(const size_t idx, int dim) const
	{
		return pts[idx][dim];
	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX &bb) const { return false; }
};
typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, MeshPointCloud>, MeshPointCloud, 3> MeshKDTree;


class DeformableMeshInstance;
namespace SmartBodyBinary
{
	class StaticMesh;
}

struct BlendShapeData
{
	std::vector<std::pair<int, SrVec> > diffV;
	std::vector<std::pair<int, SrVec> > diffN;
};

/* This class is used to simulate and represent deformed mesh
   for Smartbody Characters.
*/
class DeformableMesh : public SmartBody::SBAsset
{
public:
	std::vector<boost::intrusive_ptr<SrSnModel>>		dMeshDynamic_p;
	std::vector<boost::intrusive_ptr<SrSnModel>>		dMeshStatic_p;
	std::vector<std::unique_ptr<SkinWeight>>	skinWeights;
	std::map<std::string, std::vector<boost::intrusive_ptr<SrSnModel>> > blendShapeMap;	// the key store the base shape name, vector stores morph target SrModels. first one in the vector is always the base one
	std::map<int, std::vector<int> > blendShapeNewVtxIdxMap; 
	std::map<std::string, std::vector<std::string> > morphTargets;	// stores a vector of morph target names, first one is always the base one
	std::map<std::string, std::vector<BlendShapeData> > optimizedBlendShapeData;						// stores optimized information when calculating blend shapes; list of vertices affected, and their differential vector and normal amounts
	
	std::string                 skeletonName;						// binding skeleton for this deformable model
	boost::intrusive_ptr<SkSkeleton>					skeleton;							// pointer to current skeleton
	
	// unrolled all vertices into a single buffer for faster GPU rendering
	bool initStaticVertexBuffer, initSkinnedVertexBuffer;	
	std::vector<SrVec>          posBuf;	
	std::vector<SrVec>          normalBuf;
	std::vector<SrVec>          tangentBuf;
	std::vector<SrVec>          binormalBuf;
	std::vector<SrVec>          skinColorBuf;
	std::vector<SrVec>          meshColorBuf;
	std::vector<SrVec2>         texCoordBuf;	
	std::vector<SrVec3i>        triBuf;
	std::vector<std::unique_ptr<SbmSubMesh>>    subMeshList;

	std::vector<int>			boneCountBuf;
	std::vector<SrVec>          boneColorMap;
	std::vector<SrVec4i>        boneIDBuf[2];
	std::vector<SrVec4>         boneIDBuf_f[2];
	std::vector<SrVec4>         boneWeightBuf[2];
	std::map<std::string,int>   boneJointIdxMap;
	std::vector<SkJoint*>		boneJointList;	
	std::vector<std::string>    boneJointNameList;
	std::vector<SrMat>          bindPoseMatList;	

	std::vector<int> meshIndexList;
	std::map<int,std::vector<int> > vtxNewVtxIdxMap;

	bool hasVertexColor;	
	bool hasTexCoord;

	SrModel _emptyModel;
public:
	SBAPI explicit DeformableMesh(boost::intrusive_ptr<SkSkeleton> skeleton);
	SBAPI virtual ~DeformableMesh();	
	SBAPI void initDeformMesh(std::vector<SrModel*>& meshVec);
	SBAPI SkinWeight* getSkinWeight(const std::string& skinSourceName);
	SBAPI SkinWeight* getSkinWeightIndex(int index);
	SBAPI int getNumMeshes() const;
	SBAPI std::string getMeshName(int index);
	SBAPI SrModel& getStaticModel(int index);
	SBAPI int	getMesh(const std::string& meshName);				// get the position given the mesh name
	int getValidSkinMesh(const std::string& meshName);
    
	SBAPI virtual void rebuildVertexBuffer(bool rebuild);

	SBAPI virtual bool buildSkinnedVertexBuffer(); // unrolled all models inside this deformable mesh into a GPU-friendly format
	SBAPI virtual bool buildBlendShapes();
	SBAPI void updateVertexBuffer(); // update the values in the vertex buffer based on dMeshStatic_p	
	SBAPI bool isSkinnedMesh() const;
	SBAPI bool saveToSmb(std::string inputFileName);
	SBAPI bool saveToDmb(std::string inputFileName);
	SBAPI bool saveToDae(SmartBody::SBRenderAssetManager& renderAssetManager, std::string inputFileName, std::string skeletonName);
	SBAPI bool readFromSmb(std::string inputFileName);
	SBAPI bool readFromDmb(std::string inputFileName);
	// helper function
	void saveToStaticMeshBinary(SmartBodyBinary::StaticMesh* mesh, const std::string& objectName);
	void readFromStaticMeshBinary(SmartBodyBinary::StaticMesh* mesh, std::vector<boost::intrusive_ptr<SrModel>>& models, std::string file);
	void loadAllFoundTextures(std::string textureDirectory);
	SBAPI SrVec computeCenterOfMass();
	SBAPI SrBox computeBoundingBox();

	SBAPI void translate(SrVec trans);
	SBAPI void rotate(SrVec trans);
	SBAPI void scale(float factor);
	SBAPI void addTransform(const SrMat& transform);
	SBAPI void centralize();
	SBAPI void computeNormals();
	SBAPI void copySkinWeights(DeformableMesh* fromMesh, const std::string& morphName);
	SBAPI void copyClosestSkinWeights(DeformableMesh* fromMesh);

};

class DeformableMeshInstance
{
protected:
	DeformableMesh* _mesh;
	//std::vector<SrSnModel*>	dynamicMesh; 
	boost::intrusive_ptr<SkSkeleton>				_skeleton;
	SmartBody::SBCharacter*	_character;		// pointer to current character
	SmartBody::SBPawn*      _pawn;
	bool				  _updateMesh;
	std::vector<SkJointList> _boneJointList;
	SrVec _meshScale;
	int  meshVisibleType;
	bool _recomputeNormal;
	bool _isStaticMesh;

public:
	std::vector<SrVec> _restPosBuf;
	std::vector<SrVec> _deformPosBuf;	
	std::vector<SrVec> _deformNormalBuf;
	std::vector<SrVec> _deformTangentBuf;
	std::vector<SrMat>  transformBuffer;	

	GLuint _tempTex;
	GLuint _tempFBO;

	GLuint * _tempTexPairs;
	GLuint * _tempFBOPairs;

	GLuint * _tempTexWithMask;
	GLuint * _tempFBOTexWithMask;

public:
	SBAPI DeformableMeshInstance();
	SBAPI virtual ~DeformableMeshInstance();	
	SBAPI virtual void setDeformableMesh(DeformableMesh* mesh);
	SBAPI void updateJointList();
	SBAPI virtual void setPawn(SmartBody::SBPawn* pawn);
	SBAPI virtual void setVisibility(int deformableMesh);
	SBAPI virtual void setMeshScale(SrVec scale);
	SBAPI SrVec   getMeshScale() { return _meshScale; }
	SBAPI int    getVisibility();
	SBAPI void    setToStaticMesh(bool isStatic);
	SBAPI bool    isStaticMesh();
	SBAPI boost::intrusive_ptr<SmartBody::SBSkeleton> getSkeleton();
	SBAPI virtual void update();
	SBAPI virtual void updateFast();

	SBAPI void blendShapeStaticMesh();
	SBAPI virtual void GPUblendShapes(glm::mat4x4, glm::mat4x4);
	SBAPI virtual void blendShapes();
	SBAPI DeformableMesh* getDeformableMesh() { return _mesh; }
	SBAPI SmartBody::SBCharacter* getCharacter() { return _character; }
	SBAPI SmartBody::SBPawn* getPawn() { return _pawn; }
	SBAPI void updateTransformBuffer();
	void updateSkin(const std::vector<SrVec>& restPos, std::vector<SrVec>& deformPos);
protected:
	void cleanUp();
};

#endif
