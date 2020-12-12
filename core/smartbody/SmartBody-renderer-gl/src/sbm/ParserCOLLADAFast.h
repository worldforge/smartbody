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

#ifndef _PARSER_COLLADAFAST_H_
#define _PARSER_COLLADAFAST_H_

#include <fstream>
#include "rapidxml_utils.hpp"
#include "rapidxml.hpp"
#include "sk/sk_skeleton.h"
#include "sk/sk_motion.h"
#include <sr/sr_material.h>

#include "gwiz_math.h"
#include "sbm/sbm_deformable_mesh.h"
#include <memory>

typedef std::map<std::string, std::vector<SrVec> > VecListMap;

struct ColladaFloatArrayFast
{
	std::vector<float> floatArray;
	std::string accessorParam;
	int stride;	 
};

struct ColladaSamplerFast
{
	std::string inputName;
	std::string outputName;
};

struct ColladChannelFast
{
	std::string sourceName;
	std::string targetJointName;
	std::string targetType;
};

class ParserCOLLADAFast
{
	public:
		static void getChildNodes(const std::string& nodeName, rapidxml::xml_node<>* node, std::vector<rapidxml::xml_node<>*>& children );
		static rapidxml::xml_node<>* getNode(const std::string& nodeName, rapidxml::xml_node<>* node, int curDepth, int maximumDepth);
		static rapidxml::xml_node<>* getNode(const std::string& nodeName, rapidxml::xml_node<>* node);

		//static rapidxml::xml_node<>* getNode(const std::string& nodeName, std::string fileName, int maximumDepth);
		//static rapidxml::xml_node<>* getNode(const std::string& nodeName, std::string fileName);

		static std::unique_ptr<rapidxml::file<char>> getParserDocumentFile(std::string fileName, rapidxml::xml_document<>* doc);
		static std::string getNodeAttributeString(rapidxml::xml_node<>* node, std::string attrName);
		static int getNodeAttributeInt(rapidxml::xml_node<>* node, std::string attrName);
		static void nodeStr(const std::string s, std::string& out);

		// parse from files
		static bool parse(SkSkeleton& skeleton, SkMotion& motion, std::string fileName, float scale, bool doParseSkeleton, bool doParseMotion);
		static bool parseStaticMesh(std::vector<SrModel*>& meshModelVecs, std::string fileName);

		// parse nodes
		static void parseLibraryControllers(rapidxml::xml_node<>* node, DeformableMesh& mesh, float scaleFactor, std::string jointPrefix);
		static void parseLibraryVisualScenes(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name);
		static void parseJoints(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name, SkJoint* parent = nullptr, bool hasRootJoint = false);
		static void parseLibraryAnimations(rapidxml::xml_node<>* node, SkSkeleton& skeleton, std::vector<std::unique_ptr<SmartBody::SBMotion>>& motions, float scale, int& order, bool zaxis = false);
		static void parseLibrarySingleAnimation(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, bool zaxis);
		static void parseNodeAnimation(rapidxml::xml_node<>* node1, std::map<std::string, ColladaFloatArrayFast > &floatArrayMap, float scale, std::map<std::string, ColladaSamplerFast > &samplerMap, std::vector<ColladChannelFast> &channelSamplerNameMap, SkSkeleton &skeleton );

		static void parseLibraryAnimations2(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order);
	
		static void animationPostProcess(SkSkeleton& skeleton, SmartBody::SBMotion* motion);
		static void animationPostProcessByChannels(SkSkeleton& skeleton, SmartBody::SBMotion* motion, SkChannelArray& motionChannels);
		static void parseLibraryGeometries(rapidxml::xml_node<>* node, const char* file, std::vector<SrMaterial>& M, std::vector<std::string>& mnames, \
			std::map<std::string, std::string>& materialId2Name, std::map<std::string,
			std::string>& mtlTexMap, 
			std::map<std::string,std::string>& mtlTexBumpMap, 
			std::map<std::string,std::string>& mtlTexSpecularMap, 
			std::map<std::string, std::string>& mtlTransparentMap, 
			std::map<std::string, std::string>& mtlGlossyMap, 
			std::vector<SrModel*>& meshModelVec, float scale);

		static void load_texture(int type, const char* textureName, const char* file, const SrStringArray& paths);
		static std::string getFinalTextureFileName(std::string filename, const SrStringArray& paths);
		static void parseLibraryMaterials(rapidxml::xml_node<>* node, std::map<std::string, std::string>& effectId2MaterialId);
		static void parseLibraryImages(rapidxml::xml_node<>* node, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name);

		static void parseLibraryEffects(rapidxml::xml_node<>* node, std::string assetName, std::map<std::string, std::string>&effectId2MaterialId, std::map<std::string, std::string>& materialId2Name, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name, std::vector<SrMaterial>& M, std::vector<std::string>& mnames, \
			std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap, std::map<std::string, std::string>& mtlTransparentMap, std::map<std::string, std::string>& mtlGlossyMap);

	private:
		static int getMotionChannelId(SkChannelArray& channels, const std::string&  sourceName);
		static int getMotionChannelId(SkChannelArray& channels, std::string& jointName, std::string& targetType);
		static std::string tokenize(std::string& str,const std::string& delimiters = " ", int mode = 1);
		static int getRotationOrder(std::vector<std::string> orderVec);
		static std::string getGeometryType(std::string s);
		static void setModelVertexSource(std::string& sourceName, std::string& semanticName, SrModel* model, VecListMap& vecMap);		
};

#endif
