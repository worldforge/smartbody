#include "ogresmartbodylistener.h"
#include "ogresmartbody.h"
#include <sb/SBCharacter.h>
#include <sb/SBSubject.h>
#include <sb/SBAttribute.h>
#include <sb/SBPawn.h>
#include <sb/SBSkeleton.h>
#include <sb/SBAssetManager.h>
#include <iostream>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <OGRE/OgreBone.h>
#include <OGRE/OgreBlendMode.h>

OgreSmartBodyListener::OgreSmartBodyListener(OgreSmartBody* osb) : SBSceneListener(), SBObserver()
{
	ogreSB = osb;
}

OgreSmartBodyListener::~OgreSmartBodyListener()
{
}

void OgreSmartBodyListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	if (ogreSB->getSceneManager()->hasEntity(name))
	{
		SmartBody::util::log("An entity named %s already exists, ignoring...", name.c_str());
		return;
	}

	/*
	Ogre::Entity* entity = ogreSB->getSceneManager()->createEntity(name, objectClass + ".mesh");
	Ogre::SceneNode* node = ogreSB->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);

	Ogre::Skeleton* meshSkel = entity->getSkeleton();
	Ogre::Skeleton::BoneIterator it = meshSkel->getBoneIterator(); 
	while ( it.hasMoreElements() ) 
	{ 
		Ogre::Bone* bone = it.getNext();
		bone->setManuallyControlled(true);
	}
	*/
	// listen for the attributes
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(name);
	SmartBody::SBAttribute* attribute = character->getAttribute("deformableMesh");
	attribute->registerObserver(this);
	
	
}
	
void OgreSmartBodyListener::OnCharacterDelete( const std::string & name )
{
	if (!ogreSB->getSceneManager()->hasEntity(name))
	{
		SmartBody::util::log("An entity named %s already exists, ignoring...", name.c_str());
		return;
	}

	Ogre::SceneNode * node = (Ogre::SceneNode *)ogreSB->getSceneManager()->getRootSceneNode()->getChild(name);
	node->detachAllObjects();
	ogreSB->getSceneManager()->destroyEntity(name);
	ogreSB->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name);
}
	
void OgreSmartBodyListener::OnCharacterChanged( const std::string& name )
{
	if (!ogreSB->getSceneManager()->hasEntity(name))
	{
		SmartBody::util::log("An entity named %s already exists, ignoring...", name.c_str());
		return;
	}

	Ogre::Entity* entity = ogreSB->getSceneManager()->getEntity(name);
	Ogre::Skeleton* meshSkel = entity->getSkeleton();
	const std::vector<Ogre::Bone*>& bones = meshSkel->getBones();
	for (int b = 0; b < bones.size(); b++)
	{
		bones[b]->setManuallyControlled(true);
	}

}
		 
void OgreSmartBodyListener::OnLogMessage( const std::string & message )
{
#ifdef WIN32
	std::cout << message << std::endl;
#endif
}

void OgreSmartBodyListener::notify(SmartBody::SBSubject* subject)
{
	SmartBody::util::log("Notification!");
	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);

	if (attribute)
	{
		std::string attrName = attribute->getName();
		if (attrName == "deformableMesh")
		{
			SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(attribute->getObject());
			SmartBody::SBPawn* pawn = character;
			// load the data into smartbody
			bool useDeformableMesh = true;
			SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
			SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			if (strAttribute)
			{
				const std::string& value = strAttribute->getValue();
				// clean up any old meshes

				if (value == "")
					return;

				
			
				SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
				DeformableMesh* mesh = assetManager->getDeformableMesh(value);
				if (!mesh)
				{
					int index = value.find(".");
					if (index != std::string::npos)
					{
						std::string prefix = value.substr(0, index);
						const std::vector<std::string>& meshPaths = assetManager->getAssetPaths("mesh");
						for (size_t x = 0; x < meshPaths.size(); x++)
						{
							assetManager->loadAsset(meshPaths[x] + "/" + prefix + "/" + value);
						}
					}
					mesh = assetManager->getDeformableMesh(value);
				}


				if (mesh)
				{
					if (!pawn->dMeshInstance_p && useDeformableMesh)
					{
						pawn->dMeshInstance_p = new SbmDeformableMeshGPUInstance();
						//pawn->dMeshInstance_p = new DeformableMeshInstance();
						pawn->dMeshInstance_p->setToStaticMesh(false);
					}
					else if (!pawn->dStaticMeshInstance_p && !useDeformableMesh)
					{
						pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
						//pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
						pawn->dStaticMeshInstance_p->setToStaticMesh(true);

					}
					else if (!pawn->dStaticMeshInstance_p && attrName == "mesh")
					{
						pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
						//pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
						pawn->dStaticMeshInstance_p->setToStaticMesh(true);
					}

#if 1
					// create attributes for all the blend shapes if there's any
					std::map<std::string, std::vector<SrSnModel*> > ::iterator iter = mesh->blendShapeMap.begin();
					for (; iter != mesh->blendShapeMap.end(); ++iter)
					{
						for (size_t c = 0; c < iter->second.size(); ++c)
						{
							//if (strcmp(iter->first.c_str(), (const char*)iter->second[c]->shape().name) == 0)	// you don't create it for base shape
							//continue;

							std::stringstream ss;
							ss << "blendShape.channelName." << (const char*)iter->second[c]->shape().name;
							std::stringstream ss1;
							ss1 << "blendShape.channelWeightLimit." << (const char*)iter->second[c]->shape().name;
							std::stringstream ss2;
							ss2 << "blendShape.controller." << (const char*)iter->second[c]->shape().name;
							pawn->createStringAttribute(ss.str(), (const char*)iter->second[c]->shape().name, true, "Blend Shape", c * 3 + 1, false, false, false, "blend shape channel name");
							pawn->createDoubleAttribute(ss1.str(), 1.0f, true, "Blend Shape", c * 3 + 2, false, false, false, "blend shape channel weight limit");
							pawn->createStringAttribute(ss2.str(), iter->first, true, "Blend Shape", c * 3 + 3, false, false, false, "blend shape controller name");
							pawn->createActionAttribute("updateChannel", true, "Blend Shape", 0, false, false, false, "update blend shape channel");
						}
					}

					if (mesh->blendShapeMap.size() == 0)
#endif
					{
						// if there are no blendshapes, but there are blendShape.channelName attributes, 
						// then add the morph targets
						std::vector<SmartBody::StringAttribute*> shapeAttributes;
						std::map<std::string, SmartBody::SBAttribute*>& attributes = pawn->getAttributeList();
						for (std::map<std::string, SmartBody::SBAttribute*>::iterator iter = attributes.begin();
							iter != attributes.end();
							iter++)
						{
							SmartBody::SBAttribute* attribute = (*iter).second;
							const std::string& attrName = attribute->getName();
							size_t pos = attrName.find("blendShape.channelName.");
							if (pos != std::string::npos)
							{
								SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
								shapeAttributes.emplace_back(strAttribute);
							}
						}

						int numShapeAttributes = shapeAttributes.size();
						if (numShapeAttributes > 0)
						{
							// make space for all the attributes
							mesh->morphTargets.insert(std::pair<std::string, std::vector<std::string> >("face", std::vector<std::string>()));
							std::map<std::string, std::vector<std::string> >::iterator shapeIter = mesh->morphTargets.begin();
							(*shapeIter).second.resize(numShapeAttributes);


							bool hasNeutral = false;
							for (std::vector<SmartBody::StringAttribute*>::iterator iter = shapeAttributes.begin();
								iter != shapeAttributes.end();
								iter++)
							{
								const std::string& attrName = (*iter)->getName();
								// get the shape name and value
								std::string shapeName = attrName.substr(23);

								std::string shapeChannel = (*iter)->getValue();
								if (shapeChannel == "Neutral")
								{
									DeformableMesh* neutralMesh = SmartBody::SBScene::getScene()->getAssetManager()->getDeformableMesh(shapeName);
									SrSnModel* staticModel = neutralMesh->dMeshStatic_p[0];
									SrModel& s = staticModel->shape();
									std::string blendShapeMapName = (const char*)s.name;
									(*shapeIter).second[0] = blendShapeMapName;
									//mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(neutralMesh->getName(), std::vector<SrSnModel*>() ));
									mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(blendShapeMapName, std::vector<SrSnModel*>()));
									std::map<std::string, std::vector<SrSnModel*> >::iterator blendshapeIter = mesh->blendShapeMap.begin();
									(*blendshapeIter).second.resize(numShapeAttributes);
									SrSnModel* model = new SrSnModel();
									model->shape(staticModel->shape());
									model->shape().name = staticModel->shape().name;
									model->changed(true);
									model->visible(false);
									(*blendshapeIter).second[0] = model;
									model->ref();
									hasNeutral = true;
									// add the shape to the deformable mesh
									//mesh->dMeshStatic_p.emplace_back(model);
									//mesh->dMeshDynamic_p.emplace_back(model);
								}

							}

							SrVec blendShapeOffset = pawn->getVec3Attribute("blendShape.translationOffset");
							std::map<std::string, std::vector<SrSnModel*> >::iterator blendshapeIter = mesh->blendShapeMap.begin();
							if (blendshapeIter != mesh->blendShapeMap.end())
							{
								(*blendshapeIter).second.resize(numShapeAttributes);

								int count = 1;
								if (hasNeutral)
								{
									for (std::vector<SmartBody::StringAttribute*>::iterator iter = shapeAttributes.begin();
										iter != shapeAttributes.end();
										iter++)
									{
										const std::string& attrName = (*iter)->getName();
										// get the shape name and value
										std::string shapeName = attrName.substr(23);
										std::string shapeChannel = (*iter)->getValue();
										if (shapeChannel == "Neutral")
											continue;
										if (blendshapeIter != mesh->blendShapeMap.end())
											(*shapeIter).second[count] = shapeName;
										DeformableMesh* shapeModel = SmartBody::SBScene::getScene()->getAssetManager()->getDeformableMesh(shapeName);

										if (shapeModel)
										{
											shapeModel->dMeshStatic_p[0]->shape().translate(blendShapeOffset);
											shapeModel->dMeshDynamic_p[0]->shape().translate(blendShapeOffset);
											(*blendshapeIter).second[count] = shapeModel->dMeshStatic_p[0];
											shapeModel->dMeshStatic_p[0]->ref();
										}
										else
										{
											(*blendshapeIter).second[count] = NULL;
										}
										count++;
									}
								}
							}
						}
					}
					//mesh->rebuildVertexBuffer(true);

					DeformableMeshInstance* meshInstance = useDeformableMesh ? pawn->dMeshInstance_p : pawn->dStaticMeshInstance_p;
					meshInstance->setDeformableMesh(mesh);
					//meshInsance->setSkeleton(pawn->getSkeleton());	
					meshInstance->setPawn(pawn);
					pawn->setStringAttribute("displayType", pawn->getStringAttribute("displayType"));

#if 0
					for (size_t i = 0; i < pawn->dMesh_p->dMeshDynamic_p.size(); i++)
					{
						scene->getRootGroup()->add(pawn->dMesh_p->dMeshDynamic_p[i]);
					}
#endif
				}
			}
		
			// transfer the data to Ogre
			SmartBody::StringAttribute* strAttr = dynamic_cast<SmartBody::StringAttribute*>(attribute);;
			SmartBody::util::log("Set deformable mesh, now creating character %s", strAttr->getValue());
			
			
			SmartBody::util::log("Setting up character %s", character->getName().c_str());
			Ogre::Entity* ogreCharacter = createOgreCharacter(character);
			if (ogreCharacter != NULL)
			{
							
				Ogre::SceneNode* node = ogreSB->getSceneManager()->getRootSceneNode()->createChildSceneNode("node" + character->getName());
				node->attachObject(ogreCharacter);
			}
			else
			{
				SmartBody::util::log("Problem setting up character %s", character->getName().c_str());
			}

			

		}
	}
}

Ogre::Entity* OgreSmartBodyListener::createOgreCharacter(SmartBody::SBCharacter* sbChar)
{
// ?	ogreWnd->setVSyncEnabled(true);


	Ogre::Entity* outChar = NULL;
	
	std::string skeletonName = sbChar->getSkeleton()->getName();
	SmartBody::util::log("SKELETON NAME = %s", skeletonName.c_str());
	std::string meshName = sbChar->getStringAttribute("deformableMesh");
	SmartBody::util::log("MESH NAME = %s", meshName.c_str());
	DeformableMeshInstance* meshInstance = sbChar->getActiveMesh();
	if (meshName == "")
		meshName = sbChar->getName();

	SmartBody::SBSkeleton* charSkel = SmartBody::SBScene::getScene()->getSkeleton(skeletonName);
	addSBSkeleton(charSkel);

	addOgreMesh(meshName, meshInstance);

	Ogre::SkeletonPtr ogreSkel = Ogre::SkeletonManager::getSingleton().getByName(skeletonName);
	Ogre::MeshPtr     ogreMesh = Ogre::MeshManager::getSingleton().getByName(meshName);
	if (!ogreSkel)
	{
		SmartBody::util::log("Can't retrieve ogre skeleton with name %s", skeletonName.c_str());
		return NULL;
	}
	if (!ogreMesh)
	{
		SmartBody::util::log("Can't retrieve ogre mesh with name %s", meshName.c_str());
		return NULL;
	}
	if (!meshInstance)
	{
		return NULL;
	}

	DeformableMesh* deformMesh = meshInstance->getDeformableMesh();
	if (!deformMesh) 
		return NULL;
#if 1
	if (deformMesh->isSkinnedMesh())
	{
		ogreMesh->setSkeletonName(skeletonName);
		ogreMesh->_notifySkeleton(ogreSkel);
		// combine skeleton and mesh with skinning information	
#if 0
		for (unsigned int i = 0; i<deformMesh->skinWeights.size(); i++)
		{
			// generate bone indices
			SkinWeight* skinWeight = deformMesh->skinWeights[i];
			if (deformMesh->getMesh(skinWeight->sourceMesh) == -1) continue;
			Ogre::SubMesh* subMesh = ogreMesh->getSubMesh(skinWeight->sourceMesh);
			int globalCounter = 0;
			for (unsigned int j = 0; j<subMesh->vertexData->vertexCount; j++)
			{
				Ogre::VertexBoneAssignment vba;
				// The index of the vertex in the vertex buffer
				vba.vertexIndex = static_cast<unsigned int>(j);
				for (unsigned int k = 0; k<skinWeight->numInfJoints[j]; k++)
				{
					std::string jName = skinWeight->infJointName[skinWeight->jointNameIndex[globalCounter]];
					vba.boneIndex = ogreSkel->getBone(jName)->getHandle();
					vba.weight = skinWeight->bindWeight[skinWeight->weightIndex[globalCounter]];
					//if (k < 4)
					subMesh->addBoneAssignment(vba);
					globalCounter++;
				}
			}
			subMesh->_compileBoneAssignments();
		}
#else	
		ogreMesh->clearBoneAssignments();
		for (unsigned int j = 0; j<ogreMesh->sharedVertexData->vertexCount; j++)
		{
			for (unsigned int m = 0; m<1; m++)
			{
				for (unsigned int k = 0; k<4; k++)
				{
					Ogre::VertexBoneAssignment vba;
					// The index of the vertex in the vertex buffer
					vba.vertexIndex = static_cast<unsigned int>(j);
					int origBoneID = deformMesh->boneIDBuf[m][j][k];
					std::string jName = deformMesh->boneJointNameList[origBoneID];
					//vba.boneIndex = (ogreSkel->getBone(jName)->getHandle()+2)%ogreSkel->getNumBones();				o
					vba.boneIndex = ogreSkel->getBone(jName)->getHandle();
					//if (origBoneID != vba.boneIndex)
					//vba.boneIndex = origBoneID;
					vba.weight = deformMesh->boneWeightBuf[m][j][k];
					if (vba.weight > 0)
					{
						ogreMesh->addBoneAssignment(vba);
						//SmartBody::util::log("jName = %s, origID = %d, ogreBoneID = %d, weight = %f",jName.c_str(),origBoneID,vba.boneIndex, vba.weight);
					}
				}
			}
		}
		ogreMesh->_compileBoneAssignments();
#endif
	}
#endif
	ogreMesh->load();
	outChar = ogreSB->getSceneManager()->createEntity(sbChar->getName(), meshName);

#if 1	//outChar->setMaterialName("Ogre/RTShader/shadow_caster_dq_skinning_4weight");	
	/*
	for (unsigned int i=0;i<outChar->getNumSubEntities();i++)
	{
	MaterialPtr entityMaterial = outChar->getSubEntity(i)->getMaterial();
	if(!entityMaterial.isNull())
	{
	Technique* bestTechnique = entityMaterial->getBestTechnique();
	if(bestTechnique)
	{
	Pass* pass = bestTechnique->getPass(0);
	if (pass && pass->hasVertexProgram() && pass->getVertexProgram()->isSkeletalAnimationIncluded())
	{
	LOG("Entity %d, Has Hardware Skinning",i);
	}
	else
	{
	LOG("Entity %d, Has Software Skinning",i);
	}
	}
	}
	}

	if (outChar->isHardwareAnimationEnabled())
	{
	LOG("outChar has hardware skinning");
	}
	else
	{
	LOG("outChar has software skinning");
	}
	*/
	// 	RTShader::HardwareSkinningFactory::getSingleton().setMaxCalculableBoneCount(150);
	// 	RTShader::HardwareSkinningFactory::getSingleton().prepareEntityForSkinning(outChar, Ogre::RTShader::ST_DUAL_QUATERNION, true, false);
	// 	//The following line is needed only because the Jaiqua model material has shaders and
	// 	//as such is not automatically reflected in the RTSS system
	// 	for (unsigned int i=0;i<outChar->getNumSubEntities();i++)
	// 	{
	// 		RTShader::ShaderGenerator::getSingleton().createShaderBasedTechnique(
	// 			outChar->getSubEntity(i)->getMaterialName(),
	// 			Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
	// 			Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
	// 			true);
	// 		ogreShaderGenerator->validateMaterial(Ogre::MaterialManager::DEFAULT_SCHEME_NAME,outChar->getSubEntity(i)->getMaterialName());
	// 		outChar->getSubEntity(i)->getMaterial()->compile();		
	//  	}			
#endif
	//outChar->setDisplaySkeleton(true);
	return outChar;
}

void OgreSmartBodyListener::addSBSkeleton(SmartBody::SBSkeleton* skel)
{
	if (!skel)
	{
		SmartBody::util::log("No skeleton!");
		return;
	}
	Ogre::String skelName = skel->getName();
	Ogre::SkeletonManager& skelManager = Ogre::SkeletonManager::getSingleton();
	if (skelManager.getByName(skelName))
	{
		SmartBody::util::log("Skeleton %s already exists", skelName.c_str());
		return; // skeleton already exists
	}

	Ogre::SkeletonPtr ogreSkel = skelManager.create(skelName, "General", true);
	SmartBody::util::log("Created skeleton with name %s", skelName.c_str());

	// setup the initial state 
	for (int i = 0; i<skel->getNumJoints(); i++)
	{
		SmartBody::SBJoint* joint = skel->getJoint(i);
		Ogre::Bone* ogBone = ogreSkel->createBone(joint->getName());
		SmartBody::util::log("Created joint with name %s", joint->getName().c_str());
		SrVec p = joint->getOffset();
		ogBone->setPosition(p.x, p.y, p.z);
		SrQuat q = joint->quat()->orientation()*joint->quat()->prerot();
		ogBone->setOrientation(q.w, q.x, q.y, q.z);
		ogBone->setManuallyControlled(true);
		ogBone->setInitialState();
	}
	// setup the hierarchy
	for (int i = 0; i<skel->getNumJoints(); i++)
	{
		SmartBody::SBJoint* joint = skel->getJoint(i);
		Ogre::Bone* ogBone = ogreSkel->getBone(joint->getName());
		for (int k = 0; k<joint->getNumChildren(); k++)
		{
			Ogre::Bone* childBone = ogreSkel->getBone(joint->getChild(k)->getName());
			ogBone->addChild(childBone);
		}
	}
	ogreSkel->load(); // load the skeleton	
}


	void OgreSmartBodyListener::addOgreMesh(std::string meshName, DeformableMeshInstance* meshInstance)
	{
		SmartBody::util::log("ADDOGREMESH %s", meshName.c_str());
#if 0
		Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
		Ogre::MeshPtr ogreMesh = meshManager.create(meshName, "General", true);
		for (unsigned int i = 0; i<mesh->dMeshStatic_p.size(); i++)
		{
			SrModel& subModel = mesh->dMeshStatic_p[i]->shape();
			std::string subModelName = subModel.name;
			Ogre::SubMesh* ogSubMesh = ogreMesh->createSubMesh(subModelName);
			Ogre::VertexData* vtxData = new Ogre::VertexData();
			ogSubMesh->useSharedVertices = false;
			ogSubMesh->vertexData = vtxData;
			vtxData->vertexCount = subModel.V.size();
			Ogre::VertexDeclaration* decl = vtxData->vertexDeclaration;
			size_t offset = 0;
			decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
			decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
			// store texture coordinate in a separate element
			//decl->addElement(1, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

			// create vertex buffer 0 : position and normal
			Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				decl->getVertexSize(0),                     // This value is the size of a vertex in memory
				vtxData->vertexCount,                       // The number of vertices you'll put into this buffer
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY // Properties
			);
			float* tempFloatArray;
			tempFloatArray = new float[vtxData->vertexCount * 6];
			for (int j = 0; j<subModel.V.size(); j++)
			{
				for (int k = 0; k<3; k++)
				{
					tempFloatArray[j * 6 + k] = subModel.V.get(j)[k];
					tempFloatArray[j * 6 + 3 + k] = subModel.N.get(j)[k];
				}
			}
			vbuf->writeData(0, vbuf->getSizeInBytes(), tempFloatArray, true);
			Ogre::VertexBufferBinding* bind = vtxData->vertexBufferBinding;
			bind->setBinding(0, vbuf);

			// create vertex buffer 1 : texture coordinate
			// 		Ogre::HardwareVertexBufferSharedPtr tbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			// 			decl->getVertexSize(1),                     // This value is the size of a vertex in memory
			// 			vtxData->vertexCount,                       // The number of vertices you'll put into this buffer
			// 			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY // Properties
			// 			);			
			// 		tbuf->writeData(0, tbuf->getSizeInBytes(), &subModel.T[0], true);		
			// 		bind->setBinding(1, tbuf);

			// create index buffer
			unsigned int* tempUIntArray;
			tempUIntArray = new unsigned int[subModel.F.size() * 3];
			for (int j = 0; j<subModel.F.size(); j++)
			{
				tempUIntArray[j * 3] = (unsigned int)subModel.F[j].a;
				tempUIntArray[j * 3 + 1] = (unsigned int)subModel.F[j].b;
				tempUIntArray[j * 3 + 2] = (unsigned int)subModel.F[j].c;
			}


			Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
				Ogre::HardwareIndexBuffer::IT_32BIT,        // You can use several different value types here
				subModel.F.size() * 3,                       // The number of indices you'll put in that buffer
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY // Properties
			);
			ibuf->writeData(0, ibuf->getSizeInBytes(), tempUIntArray, true);
			ogSubMesh->indexData->indexBuffer = ibuf;
			ogSubMesh->indexData->indexCount = subModel.F.size() * 3;
			ogSubMesh->indexData->indexStart = 0;
		}
		ogreMesh->_setBounds(AxisAlignedBox(-100, -100, -100, 100, 100, 100));
		ogreMesh->_setBoundingSphereRadius(Math::Sqrt(3 * 100 * 100));
		ogreMesh->load(); // load the mesh

#else 
		if (!meshInstance)
		{
			SmartBody::util::log("No mesh instance!");
			return;
		}
			
		// do not create anything if there is no skin weights or meshes
		DeformableMesh* mesh = meshInstance->getDeformableMesh();
		if (!mesh)
		{
			SmartBody::util::log("No deformable mesh!");
			return;
		}
		if (mesh->dMeshStatic_p.size() == 0)
		{
			SmartBody::util::log("Mesh static size is zero!");
			return;
		}

	

		bool isSkinMesh = true;
		if (mesh->skinWeights.size() == 0)
			isSkinMesh = false;


		Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
		if (!meshManager.getByName(meshName).isNull())
		{
			SmartBody::util::log("Mesh exists!");
			return; // mesh already exists
		}
		
		int perVertexSize = 6;
		SrVec scaleVec = meshInstance->getMeshScale();
		float meshScale = scaleVec[0];
		Ogre::MeshPtr ogreMesh = meshManager.create(meshName, "General", true);
		mesh->buildSkinnedVertexBuffer(); // if not already built
		SmartBody::util::log("Generating Deformable Model Name = %s, size of pos buffer = %d, size of color buffer = %d", meshName.c_str(), meshInstance->getDeformableMesh()->posBuf.size(), meshInstance->getDeformableMesh()->meshColorBuf.size());
		Ogre::VertexData* vtxData = new Ogre::VertexData();
		ogreMesh->sharedVertexData = vtxData;
		vtxData->vertexCount = mesh->posBuf.size();
		bool hasColorBuf = (mesh->meshColorBuf.size() == mesh->posBuf.size() && mesh->hasVertexColor);
		bool hasTexture = (mesh->texCoordBuf.size() == mesh->posBuf.size() && mesh->hasTexCoord);
		Ogre::VertexDeclaration* decl = vtxData->vertexDeclaration;
		size_t offset = 0;
		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		// store texture coordinate in a separate element
		//offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);


		// create vertex buffer 0 : position and normal

		Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			decl->getVertexSize(0),                     // This value is the size of a vertex in memory
			vtxData->vertexCount,                       // The number of vertices you'll put into this buffer
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, // Properties
			true
		);
		float* tempFloatArray;
		tempFloatArray = new float[vtxData->vertexCount*perVertexSize];
		SrVec bbMax, bbMin;
		bbMax = SrVec((float)-1e20, (float)-1e20, (float)-1e20);
		bbMin = SrVec((float)1e20, (float)1e20, (float)1e20);
		for (unsigned int j = 0; j<vtxData->vertexCount; j++)
		{
			SrVec meshPos = mesh->posBuf[j] * meshScale;
			for (int k = 0; k<3; k++)
			{
				tempFloatArray[j*perVertexSize + k] = meshPos[k];
				tempFloatArray[j*perVertexSize + 3 + k] = mesh->normalBuf[j][k];
				if (bbMax[k] < meshPos[k])
					bbMax[k] = meshPos[k];
				if (bbMin[k] > meshPos[k])
					bbMin[k] = meshPos[k];
			}
			//LOG("vtx %d, mesh normal = %f %f %f", j, mesh->normalBuf[j][0], mesh->normalBuf[j][1], mesh->normalBuf[j][2]);
		}
		vbuf->writeData(0, vbuf->getSizeInBytes(), tempFloatArray, true);
		Ogre::VertexBufferBinding* bind = vtxData->vertexBufferBinding;
		bind->setBinding(0, vbuf);
		delete[] tempFloatArray;

		if (hasColorBuf && !hasTexture)
		{
			decl->addElement(1, 0, Ogre::VET_FLOAT3, Ogre::VES_DIFFUSE);
			//perVertexSize = 9;
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

			Ogre::HardwareVertexBufferSharedPtr cbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				decl->getVertexSize(1),                     // This value is the size of a vertex in memory
				vtxData->vertexCount,                       // The number of vertices you'll put into this buffer
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, // Properties
				true
			);
			tempFloatArray = new float[vtxData->vertexCount * 3];
			for (unsigned int j = 0; j<vtxData->vertexCount; j++)
			{
				tempFloatArray[j * 3] = mesh->meshColorBuf[j][0];
				tempFloatArray[j * 3 + 1] = mesh->meshColorBuf[j][1];
				tempFloatArray[j * 3 + 2] = mesh->meshColorBuf[j][2];
			}
			cbuf->writeData(0, cbuf->getSizeInBytes(), tempFloatArray, true);
			bind->setBinding(1, cbuf);
		}
		else if (hasTexture)
		{
			// create vertex buffer 1 : texture coordinate
			decl->addElement(1, 0, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
			Ogre::HardwareVertexBufferSharedPtr tbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				decl->getVertexSize(1),                     // This value is the size of a vertex in memory
				vtxData->vertexCount,                       // The number of vertices you'll put into this buffer
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, // Properties
				true
			);
			tempFloatArray = new float[vtxData->vertexCount * 2];
			for (unsigned int j = 0; j<vtxData->vertexCount; j++)
			{
				tempFloatArray[j * 2] = mesh->texCoordBuf[j][0];
				tempFloatArray[j * 2 + 1] = mesh->texCoordBuf[j][1];
			}

			tbuf->writeData(0, tbuf->getSizeInBytes(), tempFloatArray, true);
			bind->setBinding(1, tbuf);
		}

		// 	int nextBindIdx = bind->getNextIndex();

		for (unsigned int i = 0; i<mesh->subMeshList.size(); i++)
		{
			SbmSubMesh* subModel = mesh->subMeshList[i];
			Ogre::SubMesh* ogSubMesh = ogreMesh->createSubMesh();
			ogSubMesh->useSharedVertices = true;
			// create index buffer
			unsigned int* tempUIntArray;
			tempUIntArray = new unsigned int[subModel->numTri * 3];
			for (int j = 0; j<subModel->numTri; j++)
			{
				tempUIntArray[j * 3] = (unsigned int)subModel->triBuf[j][0];
				tempUIntArray[j * 3 + 1] = (unsigned int)subModel->triBuf[j][1];
				tempUIntArray[j * 3 + 2] = (unsigned int)subModel->triBuf[j][2];
			}
			Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
				Ogre::HardwareIndexBuffer::IT_32BIT,        // You can use several different value types here
				subModel->numTri * 3,                       // The number of indices you'll put in that buffer
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, // Properties
				true
			);
			ibuf->writeData(0, ibuf->getSizeInBytes(), tempUIntArray, true);
			ogSubMesh->indexData->indexBuffer = ibuf;
			ogSubMesh->indexData->indexCount = subModel->numTri * 3;
			ogSubMesh->indexData->indexStart = 0;

			// create material for the mesh
			addTexture(subModel->texName);

			std::string materialName = subModel->matName + boost::lexical_cast<std::string>(i); //meshName + boost::lexical_cast<std::string>(i) + "Mat";
			Ogre::MaterialPtr ogreMat = Ogre::MaterialManager::getSingleton().create(materialName, "General");
			Ogre::Pass* pass = ogreMat->getTechnique(0)->getPass(0);
			Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().getByName(subModel->texName);
			if (!texPtr.isNull())
			{
				Ogre::TextureUnitState* texUnit = pass->createTextureUnitState();
				texUnit->setTextureName(texPtr->getName());
				texUnit->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_CURRENT);//, 1.0, 0.1);			
			}
			//pass->setDiffuse(1.0,1.0,1.0,1.0);
			SrMaterial& mat = subModel->material;
			//LOG("diffuse material = %f %f %f %f",mat.diffuse.r,mat.diffuse.g,mat.diffuse.b,mat.diffuse.a);
			float color[4];
			mat.ambient.get(color);
			//LOG("ambient color = %f %f %f",color[0],color[1],color[2]);
			//pass->setAmbient(color[0],color[1],color[2]);
			mat.diffuse.get(color);
			//LOG("diffuse color = %f %f %f %f",color[0],color[1],color[2],color[3]);
			if (hasColorBuf && !hasTexture)
			{
				pass->setVertexColourTracking(Ogre::TVC_DIFFUSE);
				pass->setLightingEnabled(false);
			}
			else
			{
				pass->setDiffuse(color[0], color[1], color[2], color[3]);
				mat.specular.get(color);
				//LOG("specular color = %f %f %f %f",color[0],color[1],color[2],color[3]);
				pass->setSpecular(color[0], color[1], color[2], color[3]);
				pass->setShininess(mat.shininess);
				pass->setLightingEnabled(true);
			}

			if (!hasColorBuf || hasTexture)
			{
				// disable texture alpha blending if we are using vertex color
				if (mat.useAlphaBlend)
				{
					//pass->setAlphaRejectSettings(CMPF_GREATER, 0, true);	
					//LOG("material %s has alpha blending", materialName.c_str());
					pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
					pass->setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
					pass->setTransparentSortingEnabled(true);
					pass->setTransparentSortingForced(true);
					//pass->setDepthWriteEnabled(false);				
					//pass->setDepthCheckEnabled(false);
					//pass->setSceneBlending(SBF_ONE, SBF_ONE_MINUS_SOURCE_ALPHA);

				}
			}
			pass->setHashFunction(Ogre::Pass::MIN_GPU_PROGRAM_CHANGE);
			//pass->setSceneBlending(Ogre::SBF_ONE, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);;
			//pass->setShadingMode(SO_PHONG);
			//pass->setShadowCasterVertexProgram("Ogre/RTShader/shadow_caster_dq_skinning_4weight_vs");
			//pass->setVertexProgram("Ogre/RTShader/shadow_caster_dq_skinning_4weight_vs");		
			//pass->setVertexProgram("Ogre/HardwareSkinningFourWeightsGLSL");
			//pass->setShadowCasterVertexProgram("Ogre/HardwareSkinningFourWeightsShadowCasterGLSL");
			//LOG("subMesh mat name = %s",materialName.c_str());
			ogSubMesh->setMaterialName(materialName);
			ogSubMesh->generateExtremes(8);
			//ogSubMesh->setMaterialName("smartbody");
		}
		ogreMesh->_setBounds(Ogre::AxisAlignedBox(bbMin[0], bbMin[1], bbMin[2], bbMax[0], bbMax[1], bbMax[2]));
		ogreMesh->_setBoundingSphereRadius(Ogre::Math::Sqrt(3 * 10 * 10));
		ogreMesh->load(); // load the mesh
#endif	
	}

	void OgreSmartBodyListener::addTexture(std::string texName)
	{

		SbmTextureManager& texManager = SbmTextureManager::singleton();
		auto tex = texManager.findTexture(texName.c_str());
		Ogre::TextureManager& ogreTexManager = Ogre::TextureManager::getSingleton();
		ogreTexManager.setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
		Ogre::TexturePtr ogreTex = ogreTexManager.getByName(texName);
		if (!ogreTex.isNull()) return; // the texture already exist in ogre
		if (!tex) return; // the texture not exist in SmartBody
		Ogre::PixelFormat pixelFormat = Ogre::PF_R8G8B8;
		if (tex->getNumChannels() == 4) pixelFormat = Ogre::PF_R8G8B8A8;
		// initialze the texture
		ogreTex = Ogre::TextureManager::getSingleton().createManual(
			texName, // name
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,      // type
			tex->getWidth(), tex->getHeight(),         // width & height
			0,                // number of mipmaps
			pixelFormat,     // pixel format
			Ogre::TU_DEFAULT);
		Ogre::HardwarePixelBufferSharedPtr pixelBuffer = ogreTex->getBuffer();
		//LOG("texture format = %d, buffer format = %d",ogreTex->getFormat(), pixelBuffer->getFormat());
		pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
		const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
		Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
		// our texture buffer format is in RGB(A), but Ogre internal format is always XBGR ( or ABGR )
		// so the following hassle is needed to convert the data to ogre format
		for (int i = 0; i<tex->getHeight(); i++)
			for (int j = 0; j<tex->getWidth(); j++)
			{
				int idx = i*tex->getWidth()*tex->getNumChannels() + j*tex->getNumChannels();
				int idx1 = i*tex->getWidth() * 4 + j * 4;
				pDest[idx1 + 3] = 255;
				if (tex->getNumChannels() == 4)
				{
					unsigned char alpha = tex->getBuffer()[idx + 3];
					pDest[idx1 + 3] = alpha;
				}
				pDest[idx1] = tex->getBuffer()[idx + 2];
				pDest[idx1 + 1] = tex->getBuffer()[idx + 1];
				pDest[idx1 + 2] = tex->getBuffer()[idx + 0];
			}
		pixelBuffer->unlock();
		ogreTex->load();

	}



