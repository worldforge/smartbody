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

#include "SBAssetHandlerCOLLADA.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <sb/SBMotion.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"
#include "ParserCOLLADAFast.h"
#include <sr/sr_model.h>



namespace SmartBody {

SBAssetHandlerCOLLADA::SBAssetHandlerCOLLADA()
{
	assetTypes.emplace_back("dae");
}

SBAssetHandlerCOLLADA::~SBAssetHandlerCOLLADA() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerCOLLADA::getAssets(SBScene& scene, const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;

	boost::filesystem::path p(path);
	std::string extension = boost::filesystem::extension(p);
	std::string basename = boost::filesystem::basename(p);
	

	bool useFastParsing = scene.getBoolAttribute("useFastCOLLADAParsing");
	if (useFastParsing)
	{
		rapidxml::xml_document<> doc;
		auto rapidFile = ParserCOLLADAFast::getParserDocumentFile(convertedPath, &doc);
		if (!rapidFile)
		{
			SmartBody::util::log("Problem parsing file '%s'", convertedPath.c_str());
			return assets;
		}
		rapidxml::xml_node<>* colladaNode = doc.first_node("COLLADA");
		if (!colladaNode)
		{
			SmartBody::util::log("Problem parsing file '%s': not a COLLADA file.", convertedPath.c_str());
			return assets;
		}
		bool zaxis = false;
		rapidxml::xml_node<>* assetNode = ParserCOLLADAFast::getNode("asset", colladaNode, 0, 1);
		if (assetNode)
		{
			rapidxml::xml_node<>* upNode = ParserCOLLADAFast::getNode("up_axis", assetNode);
			if (upNode)
			{
				std::string upAxisName = upNode->value();
				if (upAxisName == "Z_UP" || upAxisName == "z_up")
				{
					// rotate the skeleton by -90 around the x-axis
					zaxis = true;
				}

			}
		}

		rapidxml::xml_node<>* geometryNode = ParserCOLLADAFast::getNode("library_geometries", colladaNode, 0, 1);
		rapidxml::xml_node<>* visualSceneNode = ParserCOLLADAFast::getNode("library_visual_scenes", colladaNode, 0, 1);
		if (geometryNode || visualSceneNode)
		{
			// first from library visual scene retrieve the material id to name mapping (TODO: needs reorganizing the assets management)
			std::map<std::string, std::string> materialId2Name;
			if (!visualSceneNode)
				SmartBody::util::log(" .dae file %s doesn't contain visual scene information.", convertedPath.c_str());
			auto skeleton = std::make_unique<SBSkeleton>(scene);
			skeleton->setName(basename + extension);
			auto singleMotion = std::make_unique<SBMotion>(scene);
			singleMotion->setName(basename + extension);
			int order;
			if (visualSceneNode)
			{
				ParserCOLLADAFast::parseLibraryVisualScenes(visualSceneNode, *skeleton, *singleMotion, 1.0, order, materialId2Name);

				if (zaxis)
				{				
					// get the root node
					SkJoint* root = skeleton->root();
					if (root)
					{
						if (root->quat())
						{
							SrQuat prerot = root->quat()->prerot();
							SrVec xaxis(1, 0, 0);
							SrQuat adjust(xaxis, 3.14159f / -2.0f);
							SrQuat adjustY(SrVec(0,1,0), 3.14159f );
							//SrQuat final = adjustY * adjust * prerot; 
							SrQuat final = adjust * prerot;
							//SmartBody::util::log("before = %f %f %f %f", prerot.w, prerot.x, prerot.y, prerot.z);
							//SmartBody::util::log("after = %f %f %f %f", final.w, final.x, final.y, final.z);
							root->quat()->prerot(final);
							root->offset(root->offset()*adjust);
						}
					}
				}		


			}
			
			rapidxml::xml_node<>* skmNode = ParserCOLLADAFast::getNode("library_animations", colladaNode, 0, 1);

			std::vector<std::unique_ptr<SBMotion>> motions;
			
			if (skmNode)
			{
				// NOTE: Some Collada files put each animation channel in a separate <animation> node. For this kind of special case, enable "parseSingleAnimation" to combine all nodes into one animation.
				// Otherwise, the default ( and correct ) behavior should be separate each <animation> node into an individual SBMotion. 
				if (scene.getBoolAttribute("parseSingleAnimation"))
				{					
					ParserCOLLADAFast::parseLibrarySingleAnimation(skmNode, *skeleton, *singleMotion, 1.0, order, false);
					motions.emplace_back(std::move(singleMotion));
				}
				else
				{
					ParserCOLLADAFast::parseLibraryAnimations(scene, skmNode, *skeleton, motions, 1.0, order, false);
				}				
			}



			// get picture id to file mapping
			std::map<std::string, std::string> pictureId2File;
			std::map<std::string, std::string> pictureId2Name;
			rapidxml::xml_node<>* imageNode = ParserCOLLADAFast::getNode("library_images", colladaNode, 0, 1);
			if (imageNode)
				ParserCOLLADAFast::parseLibraryImages(imageNode, pictureId2File, pictureId2Name);

			// start parsing mateiral
			std::map<std::string, std::string> effectId2MaterialId;
			rapidxml::xml_node<>* materialNode = ParserCOLLADAFast::getNode("library_materials", colladaNode, 0, 1);
			if (materialNode)
				ParserCOLLADAFast::parseLibraryMaterials(materialNode, effectId2MaterialId);

			// start parsing effect
			std::vector<SrMaterial> M;
			std::vector<std::string> mnames;
			std::map<std::string,std::string> mtlTextMap;
			std::map<std::string,std::string> mtlTextBumpMap;
			std::map<std::string,std::string> mtlTextSpecularMap;
			std::map<std::string, std::string> mtlTransparentMap;
			std::map<std::string, std::string> mtlGlossyMap;




			if (skeleton->getNumJoints() != 0)
				assets.emplace_back(std::move(skeleton));

			for (auto & motion : motions)
			{
				if (motion->getNumFrames() != 0)
					assets.emplace_back(std::move(motion));
			}

		}
		else
		{
			SmartBody::util::log( "Could not load mesh from file '%s'", convertedPath.c_str());
			return assets;
		}
	}

	return assets;
}

};
