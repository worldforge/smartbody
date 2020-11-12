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

#ifndef _PARSER_OPENCOLLADA_H_
#define _PARSER_OPENCOLLADA_H_

#include <fstream>
#include <stdio.h>
#include "sbm/xercesc_utils.hpp"
#include "sk/sk_skeleton.h"
#include "sk/sk_motion.h"
#include <sb/SBJoint.h>
#include <sr/sr_material.h>

#include "sbm/gwiz_math.h"

typedef std::map<std::string, std::vector<SrVec> > VecListMap;
class DeformableMesh;

struct ColladaFloatArray
{
	std::vector<float> floatArray;
	std::string accessorParam;
	int stride;	 
};

struct ColladaSampler
{
	std::string inputName;
	std::string outputName;
};

struct ColladChannel
{
	std::string sourceName;
	std::string targetJointName;
	std::string targetType;
};

class ParserOpenCOLLADA
{
	public:
		static void getChildNodes(const std::string& nodeName, DOMNode* node, std::vector<DOMNode*>& childs );
		static DOMNode* getNode(const std::string& nodeName, DOMNode* node, int curDepth, int maximumDepth);
		static DOMNode* getNode(const std::string& nodeName, DOMNode* node);
		static DOMNode* getNode(const std::string& nodeName, std::string fileName, int maximumDepth);
		static DOMNode* getNode(const std::string& nodeName, std::string fileName);
		static XercesDOMParser* getParserFromFile(std::string fileName);
		static std::string getNodeAttributeString(DOMNode* node, XMLCh* attrName);
		static int         getNodeAttributeInt(DOMNode* node, XMLCh* attrName);
		static void nodeStr(const XMLCh* s, std::string& out);

		// parse from files
		static bool parse(SkSkeleton& skeleton, SkMotion& motion, std::string fileName, float scale, bool doParseSkeleton, bool doParseMotion);
		static bool parseStaticMesh(std::vector<SrModel*>& meshModelVecs, std::string fileName);

		// parse nodes
		static void parseLibraryControllers(DOMNode* node, DeformableMesh* mesh, float scaleFactor, std::string jointPrefix);
		static void parseLibraryVisualScenes(DOMNode* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name);
		static void parseJoints(DOMNode* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name, SkJoint* parent = NULL, bool hasRootJoint = false);
		static void parseLibraryAnimations(DOMNode* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, bool zaxis = false);

		static void parseNodeAnimation(DOMNode* node1, std::map<std::string, ColladaFloatArray > &floatArrayMap, float scale, std::map<std::string, ColladaSampler > &samplerMap, std::vector<ColladChannel> &channelSamplerNameMap, SkSkeleton &skeleton );
		static void parseLibraryAnimations2(DOMNode* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order);
	
		static void animationPostProcess(SkSkeleton& skeleton, SkMotion& motion);
		static void animationPostProcessByChannels(SkSkeleton& skeleton, SkMotion& motion, SkChannelArray& channels);
		static void parseLibraryGeometries(DOMNode* node, const char* file, std::vector<SrMaterial>& M, std::vector<std::string>& mnames,std::map<std::string, std::string>& materialId2Name, std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap, std::map<std::string, std::string>& mtlTexTransparentMap, std::map<std::string, std::string>& mtlTexGlossyMap, std::vector<SrModel*>& meshModelVec, float scale);
		static void load_texture(int type, const char* file, const SrStringArray& paths);
		static void parseLibraryMaterials(DOMNode* node, std::map<std::string, std::string>& effectId2MaterialId);
		static void parseLibraryImages(DOMNode* node, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name);
		static void parseLibraryEffects(DOMNode* node, std::map<std::string, std::string>&effectId2MaterialId, std::map<std::string, std::string>& materialId2Name, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name,std::vector<SrMaterial>& M, std::vector<std::string>& mnames, std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap);

		// exporting functionality
		static SBAPI bool exportCollada(std::string outPathname, std::string skeletonName, std::string deformMeshName, std::vector<std::string> motionNames, bool exportSk, bool exportMesh, bool exportMotion, double meshScale);
		static bool exportVisualScene(FILE* fp, std::string skeletonName, std::string defMeshName, double scale);
		static bool exportSkinMesh(FILE* fp, std::string deformMeshName, double scale, std::string skeletonName);
		static bool exportMaterials(FILE* fp, std::string deformMeshName);		
		static bool exportMotions(FILE* fp, std::vector<std::string> motionNames);
		static bool exportGeometry(FILE* fp, SrModel& model, double scale);



	private:
		static int getMotionChannelId(SkChannelArray& channels, const std::string&  sourceName);
		static int getMotionChannelId(SkChannelArray& channels, std::string& jointName, std::string& targetType);
		static std::string tokenize(std::string& str,const std::string& delimiters = " ", int mode = 1);
		static int getRotationOrder(std::vector<std::string> orderVec);
		static std::string getGeometryType(std::string s);
		static void setModelVertexSource(std::string& sourceName, std::string& semanticName, SrModel* model, VecListMap& vecMap);		
		static void writeJointNode(FILE* fp, SmartBody::SBJoint* joint, double scale);
		static std::string exportMaerialTexParam( FILE* fp, std::string texName );
};

#endif
