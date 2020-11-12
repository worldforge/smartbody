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
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include <sb/SBUtilities.h>
#include "sbm/ParserCOLLADAFast.h"
#include "sbm/ParserOpenCOLLADA.h"
#include <sbm/GPU/SbmDeformableMeshGPU.h>


namespace SmartBody {

SBAssetHandlerCOLLADA::SBAssetHandlerCOLLADA()
{
	assetTypes.push_back("dae");
}

SBAssetHandlerCOLLADA::~SBAssetHandlerCOLLADA()
{
}

std::vector<SBAsset*> SBAssetHandlerCOLLADA::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;

	boost::filesystem::path p(path);
	std::string extension = boost::filesystem::extension(p);
	std::string basename = boost::filesystem::basename(p);
	
	double skeletonScale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");
	double motionScale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalMotionScale");

	bool useFastParsing = SmartBody::SBScene::getScene()->getBoolAttribute("useFastCOLLADAParsing");
	if (useFastParsing)
	{
		rapidxml::xml_document<> doc;
		rapidxml::file<char>* rapidFile = ParserCOLLADAFast::getParserDocumentFile(convertedPath, &doc);
		if (!rapidFile)
		{
			SmartBody::util::log("Problem parsing file '%s'", convertedPath.c_str());
			return assets;
		}
		rapidxml::xml_node<>* colladaNode = doc.first_node("COLLADA");
		if (!colladaNode)
		{
			SmartBody::util::log("Problem parsing file '%s': not a COLLADA file.", convertedPath.c_str());
			delete rapidFile;
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
			rapidxml::xml_node<>* visualSceneNode = ParserCOLLADAFast::getNode("library_visual_scenes", colladaNode, 0, 1);
			if (!visualSceneNode)
				SmartBody::util::log(" .dae file %s doesn't contain visual scene information.", convertedPath.c_str());
			SBSkeleton* skeleton = new SBSkeleton();
			skeleton->setName(basename + extension);
			SBMotion* singleMotion = new SBMotion();
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

			SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
			std::vector<SBMotion*> motions;
			
			if (skmNode)
			{
				// NOTE: Some Collada files put each animation channel in a separate <animation> node. For this kind of special case, enable "parseSingleAnimation" to combine all nodes into one animation.
				// Otherwise, the default ( and correct ) behavior should be separate each <animation> node into an individual SBMotion. 
				if (scene->getBoolAttribute("parseSingleAnimation"))
				{					
					ParserCOLLADAFast::parseLibrarySingleAnimation(skmNode, *skeleton, *singleMotion, 1.0, order, false);
					motions.push_back(singleMotion);
				}
				else
				{
					ParserCOLLADAFast::parseLibraryAnimations(skmNode, *skeleton, motions, 1.0, order, false);
				}				
			}

			if (skeleton->getNumJoints() == 0)
				delete skeleton;
			else
				assets.push_back(skeleton);

			for (size_t m = 0; m < motions.size(); m++)
			{
				SmartBody::SBMotion* motion = motions[m];
				if (motion->getNumFrames() == 0)
					delete motion;
				else
					assets.push_back(motion);
			}

			// parsing geometry
#if  !defined(__FLASHPLAYER__) && !defined(__native_client__) && !defined(EMSCRIPTEN)
			SbmDeformableMeshGPU* mesh = new SbmDeformableMeshGPU();
#else
			DeformableMesh* mesh = new DeformableMesh();
#endif
			boost::filesystem::path meshPath(path);
			std::string meshBaseName = boost::filesystem::basename(meshPath);
			std::string extension = boost::filesystem::extension(meshPath);
			mesh->setName(meshBaseName + extension);

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

			rapidxml::xml_node<>* effectNode = ParserCOLLADAFast::getNode("library_effects", colladaNode, 0, 1);
			if (effectNode)
			{
				ParserCOLLADAFast::parseLibraryEffects(effectNode, mesh->getName(), effectId2MaterialId, materialId2Name, pictureId2File, pictureId2Name, M, mnames, mtlTextMap, mtlTextBumpMap, mtlTextSpecularMap, mtlTransparentMap, mtlGlossyMap);
			}
			
			
			std::vector<SrModel*> meshModelVec;
			if (geometryNode)
				ParserCOLLADAFast::parseLibraryGeometries(geometryNode, convertedPath.c_str(), M, mnames, materialId2Name, mtlTextMap, mtlTextBumpMap, mtlTextSpecularMap, mtlTransparentMap, mtlGlossyMap, meshModelVec, 1.0f);

			float factor = 1.0f;

			rapidxml::xml_node<>* controllerNode = ParserCOLLADAFast::getNode("library_controllers", colladaNode, 0, 2);		
			if (!controllerNode)
			{
				SmartBody::util::log("mcu_character_load_skinweights ERR: no binding info contained");
			}
			else
			{
				const char* jointNamePrefix = "";
				ParserCOLLADAFast::parseLibraryControllers(controllerNode, mesh, factor, jointNamePrefix);
			}	
			
			for (unsigned int skinCounter = 0; skinCounter < mesh->skinWeights.size(); skinCounter++)
			{
				SkinWeight* skinWeight = mesh->skinWeights[skinCounter];		
				for (size_t j = 0; j < skinWeight->infJointName.size(); j++)
				{
					std::string& jointName = skinWeight->infJointName[j];
					SkJoint* curJoint = skeleton->search_joint(jointName.c_str());					
					if (curJoint)
					{
						skinWeight->infJointName[j] = curJoint->getName();
					}
					//skinWeight->infJoint.push_back(curJoint); // NOTE: If joints are added/removed during runtime, this list will contain stale data
				}				
			}
	
			std::vector<SrModel*> meshModelToDelete;
			// handling morph targets
			for (unsigned int i = 0; i < meshModelVec.size(); i++)
			{
				for (unsigned int j = 0; j < meshModelVec[i]->V.size(); j++)
				{
					meshModelVec[i]->V[j] *= factor;
				}
		
				if (meshModelVec[i]->Fn.size() == 0)
				{
					//meshModelVec[i]->computeNormals();
				}
				//Zengrui: need this line for Emscripten build
#if 1 //defined(EMSCRIPTEN)
				meshModelVec[i]->computeNormals();
#endif
				SrSnModel* srSnModelStatic = new SrSnModel();
				srSnModelStatic->shape(*meshModelVec[i]);
				srSnModelStatic->shape().name = SrString(meshModelVec[i]->name);
				
				SrSnModel* srSnModelDynamic = new SrSnModel();
				srSnModelDynamic->shape(*meshModelVec[i]);
				srSnModelDynamic->changed(true);
				srSnModelDynamic->visible(false);

				std::string currentModelName = (const char*) meshModelVec[i]->name;
				srSnModelDynamic->shape().name = SrString(currentModelName.c_str());
				
				bool isBlendShape = false;
				bool isBaseShape = false;
				std::string baseShape = "";

				for (std::map<std::string, std::vector<std::string> >::iterator morphTargetIter = mesh->morphTargets.begin(); 
					 morphTargetIter != mesh->morphTargets.end(); 
					morphTargetIter++)	
				{
					std::string morphControllerName = (*morphTargetIter).first;
					std::vector<std::string>& targets = (*morphTargetIter).second;
				
					if (targets.size() < 1) // ignore any controllers that don't have targets
						continue;

					baseShape = morphTargetIter->second[0];
					if (strcmp(baseShape.c_str(), (const char*)meshModelVec[i]->name) == 0)
					{
						isBaseShape = true;
						SmartBody::util::log("Processing morph controller %s with %d targets.", morphControllerName.c_str(), targets.size());
						break;
					}
					else
					{
						for (size_t c = 1; c < targets.size(); c++)
						{
							std::string targetName = targets[c];
							std::string modelName = (const char*)meshModelVec[i]->name;

							if (strcmp(targets[c].c_str(), (const char*)meshModelVec[i]->name) == 0)
							{
									isBlendShape = true;
									SmartBody::util::log("Found morph target %s for controller %s.", (const char*)meshModelVec[i]->name, morphControllerName.c_str());
									break;
							}
						}
						if (isBlendShape)
							break;
					}
				}

				if (isBlendShape || isBaseShape)
				{
					std::map<std::string, std::vector<SrSnModel*> >::iterator iter = mesh->blendShapeMap.find(baseShape);
					if (iter == mesh->blendShapeMap.end())
					{
						mesh->blendShapeMap.insert(std::make_pair(baseShape, std::vector<SrSnModel*>()));
						//SmartBody::util::log("ADDED BLENDSHAPE FROM BASE %s", baseShape.c_str());
						iter = mesh->blendShapeMap.find(baseShape);
					}
					else
					{
						//SmartBody::util::log("RETRIEVED BASE %s", baseShape.c_str());
					}
					std::vector<SrSnModel*>& models = (*iter).second;
					models.push_back(srSnModelStatic);
					//SmartBody::util::log("INSERTED BLENDSHAPE %s INTO BASE %s", (const char*) srSnModelStatic->shape().name, baseShape.c_str());
					srSnModelStatic->ref();
					delete srSnModelDynamic;
					//SmartBody::util::log("Insert blend shape %s with base shape %s", (const char*)meshModelVec[i]->name, baseShape.c_str());
				}

				if (isBaseShape)
				{
					SrSnModel* srSnModelStaticBase = new SrSnModel();
					srSnModelStaticBase->shape(*meshModelVec[i]);
					srSnModelStaticBase->shape().name = meshModelVec[i]->name;

					SrSnModel* srSnModelDynamicBase = new SrSnModel();
					srSnModelDynamicBase->shape(*meshModelVec[i]);
					srSnModelDynamicBase->changed(true);
					srSnModelDynamicBase->visible(false);
					srSnModelDynamicBase->shape().name = meshModelVec[i]->name;

					mesh->dMeshStatic_p.push_back(srSnModelStaticBase);
					srSnModelStaticBase->ref();
					mesh->dMeshDynamic_p.push_back(srSnModelDynamicBase);
					srSnModelDynamicBase->ref();
					//SmartBody::util::log("Insert base mesh %s", (const char*)meshModelVec[i]->name);
				}

				if (!isBlendShape && !isBaseShape)
				{
					mesh->dMeshStatic_p.push_back(srSnModelStatic);
					srSnModelStatic->ref();
					mesh->dMeshDynamic_p.push_back(srSnModelDynamic);
					srSnModelDynamic->ref();
					//SmartBody::util::log("Insert mesh %s", (const char*)meshModelVec[i]->name);
				}
			
				/* perform this when the character wants to attach to the mesh
				SrSnGroup* meshGroup = new SrSnGroup();
				meshGroup->separator(true);
				meshGroup->add(srSnModelStatic);
				// find the group of the root joint
				SrSn* node = pawn->scene_p->get(0);
				if (node)
				{
					SrSnGroup* srSnGroup = dynamic_cast<SrSnGroup*>(node);
					if (srSnGroup)
						srSnGroup->add(meshGroup);
				}
				*/
				meshModelToDelete.push_back(meshModelVec[i]);
			}

			for (size_t d = 0; d < meshModelToDelete.size(); d++)
				delete meshModelToDelete[d];

			// dump the blend shape info
			for (std::map<std::string, std::vector<SrSnModel*> >::iterator iter = mesh->blendShapeMap.begin();
				 iter != mesh->blendShapeMap.end(); 
				 iter++)
			{
				//SmartBody::util::log("BLENDSHAPE: %s", iter->first.c_str());
				std::vector<SrSnModel*>& modelList = (*iter).second;
				for (std::vector<SrSnModel*>::iterator iter2 = modelList.begin();
					iter2 != modelList.end();
					iter2++)
				{
					SmartBody::util::log("\t %s", (const char*) (*iter2)->shape().name);
				}
			}


			assets.push_back(mesh);

			if (rapidFile)
				delete rapidFile;
		}
		else
		{
			SmartBody::util::log( "Could not load mesh from file '%s'", convertedPath.c_str());
			if (rapidFile)
				delete rapidFile;
			return assets;
		}
	}

	return assets;
}

};
