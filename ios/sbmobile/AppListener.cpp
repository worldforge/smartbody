//
// Created by feng on 10/14/2015.
//

#include "AppListener.h"
#include <sb/SBVHMsgManager.h>
#include <sb/SBPython.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAttribute.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"

#include <sbm/GPU/SbmDeformableMeshGPU.h>

AppListener::AppListener()
{
}

AppListener::~AppListener()
{
}

void AppListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    SmartBody::SBPawn* pawn = scene->getPawn(name);
	//SmartBody::util::log("On character create, name = '%s'", name.c_str());
    if (!pawn)
        return;

    // add attribute observations
    SmartBody::SBAttribute* attr = pawn->getAttribute("mesh");
    if (attr)
	{
		//SmartBody::util::log("pawn %s, register attribute 'mesh'", name.c_str());
        attr->registerObserver(this);
	}
    attr = pawn->getAttribute("deformableMesh");
    if (attr)
        attr->registerObserver(this);

    attr = pawn->getAttribute("deformableMeshScale");
    if (attr)
        attr->registerObserver(this);

    attr = pawn->getAttribute("displayType");
    if (attr)
        attr->registerObserver(this);

    OnCharacterUpdate(name);
}

void AppListener::OnCharacterDelete( const std::string & name )
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
    if (!pawn)
        return;

    // remove any existing scene
    if (pawn->scene_p)
    {
        if( scene->getRootGroup() )
        {
            scene->getRootGroup()->remove( pawn->scene_p );
        }
        pawn->scene_p->unref();
        pawn->scene_p = NULL;
    }
    // remove any existing deformable mesh
#if 0
    if (pawn->dMesh_p)
    {
        for (size_t i = 0; i < pawn->dMesh_p->dMeshDynamic_p.size(); i++)
        {
            scene->getRootGroup()->remove( pawn->dMesh_p->dMeshDynamic_p[i] );
        }
        //delete character->dMesh_p; // AS 1/28/13 causing crash related to mesh instances
        pawn->dMesh_p = NULL;
    }
#endif

}

void AppListener::OnCharacterUpdate( const std::string & name)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
    if (!pawn)
        return;
    /*
    // remove any existing scene
    if (pawn->scene_p)
    {
        if( scene->getRootGroup() )
        {
            scene->getRootGroup()->remove( pawn->scene_p );
        }
        pawn->scene_p->unref();
        pawn->scene_p = NULL;
    }

    pawn->scene_p = new SkScene();
    pawn->scene_p->ref();
    pawn->scene_p->init(pawn->getSkeleton());
    bool visible = pawn->getBoolAttribute("visible");
    if (visible)
        pawn->scene_p->visible(true);
    else
        pawn->scene_p->visible(false);


    if( scene->getRootGroup() )
    {
        scene->getRootGroup()->add( pawn->scene_p );
    }
    */
}

void AppListener::OnPawnCreate( const std::string & name )
{
	SmartBody::util::log("On pawn create, name = '%s'", name.c_str());
    OnCharacterCreate(name, "");
}

void AppListener::OnPawnDelete( const std::string & name )
{
    OnCharacterDelete(name);
}

void AppListener::notify(SmartBody::SBSubject* subject)
{
    SmartBody::SBScene* scene =	SmartBody::SBScene::getScene();
    SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(subject);

    SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
    if (attribute)
    {
        SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(attribute->getObject());
        const std::string& name = attribute->getName();
        if (name == "visible")
        {
            SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
            if (boolAttribute)
            {
                if (!pawn->scene_p)
                    return;
                if (boolAttribute->getValue())
                    pawn->scene_p->visible(true);
                else
                    pawn->scene_p->visible(false);
            }
        }

        if ( name == "deformableMeshScale")
        {
            SmartBody::Vec3Attribute* vec3Attribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
            if (vec3Attribute)
            {
                if (!pawn->dMeshInstance_p)
                    pawn->dMeshInstance_p = new SbmDeformableMeshGPUInstance();
                SrVec val = vec3Attribute->getValue();
                pawn->dMeshInstance_p->setMeshScale(SrVec(val));
				//SmartBody::util::log("Set mesh to size %f", val[0]);
            }
        }
        else if (name == "deformableMesh" || name == "mesh")
		{
			bool useDeformableMesh = (name == "deformableMesh");
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			if (strAttribute)
			{
				const std::string& value = strAttribute->getValue();
				// clean up any old meshes
				//SmartBody::util::log("Set 'mesh' to %s", value.c_str());
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
						//SmartBody::util::log("useDeformableMesh : %d", value.c_str());
						pawn->dMeshInstance_p = new SbmDeformableMeshGPUInstance();
						//pawn->dMeshInstance_p = new DeformableMeshInstance();
						pawn->dMeshInstance_p->setToStaticMesh(false);
					}
					else if (!pawn->dStaticMeshInstance_p && !useDeformableMesh)
					{
						//SmartBody::util::log("useStaticMesh1 : %d", value.c_str());
						pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
						//pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
						pawn->dStaticMeshInstance_p->setToStaticMesh(true);

					}
					else if (!pawn->dStaticMeshInstance_p && name == "mesh")
					{
						//SmartBody::util::log("useStaticMesh2 : %d", value.c_str());
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
						//SmartBody::util::log("Building blenshapes");
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
						//SmartBody::util::log("Num Shape Attributes = %d", numShapeAttributes);
						if (numShapeAttributes > 0)
						{
							// make space for all the attributes
							mesh->morphTargets.insert(std::pair<std::string, std::vector<std::string> >("face", std::vector<std::string>() ));
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
									std::string blendShapeMapName = (const char*) s.name;
									//mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(neutralMesh->getName(), std::vector<SrSnModel*>() ));
									mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(blendShapeMapName, std::vector<SrSnModel*>() ));
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
								}
								
							}

							std::map<std::string, std::vector<SrSnModel*> >::iterator blendshapeIter = mesh->blendShapeMap.begin();
							if (blendshapeIter !=  mesh->blendShapeMap.end())
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
										if (blendshapeIter !=  mesh->blendShapeMap.end())
											(*shapeIter).second[count] = shapeName;
										DeformableMesh* shapeModel = SmartBody::SBScene::getScene()->getAssetManager()->getDeformableMesh(shapeName);
										if (shapeModel)
										{
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
							//SmartBody::util::log("Num blendshapes = %d", blendshapeIter->second.size());
						}
					}
					
					DeformableMeshInstance* meshInstance = useDeformableMesh ? pawn->dMeshInstance_p : pawn->dStaticMeshInstance_p;
					meshInstance->setDeformableMesh(mesh);
					//meshInsance->setSkeleton(pawn->getSkeleton());	
					meshInstance->setPawn(pawn);
					
#if 0
					for (size_t i = 0; i < pawn->dMesh_p->dMeshDynamic_p.size(); i++)
					{
						scene->getRootGroup()->add( pawn->dMesh_p->dMeshDynamic_p[i] );
					}
#endif
				}
			}
		}
        else if (name == "displayType")
        {
            SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
            if (strAttribute)
            {
                const std::string& value = strAttribute->getValue();
                if (value == "bones")
                {
                    if (pawn->scene_p)
                        pawn->scene_p->set_visibility(1,0,0,0);
                    if (pawn->dMeshInstance_p)
                        pawn->dMeshInstance_p->setVisibility(0);
                }
                else if (value == "visgeo")
                {
                    if (pawn->scene_p)
                        pawn->scene_p->set_visibility(0,1,0,0);
                    if (pawn->dMeshInstance_p)
                        pawn->dMeshInstance_p->setVisibility(0);
                }
                else if (value == "colgeo")
                {
                    if (pawn->scene_p)
                        pawn->scene_p->set_visibility(0,0,1,0);
                    if (pawn->dMeshInstance_p)
                        pawn->dMeshInstance_p->setVisibility(0);
                }
                else if (value == "axis")
                {
                    if (pawn->scene_p)
                        pawn->scene_p->set_visibility(0,0,0,1);
                    if (pawn->dMeshInstance_p)
                        pawn->dMeshInstance_p->setVisibility(0);
                }
                else if (value == "mesh")
                {
                    if (pawn->scene_p)
                        pawn->scene_p->set_visibility(0,0,0,0);
                    if (pawn->dMeshInstance_p)
                        pawn->dMeshInstance_p->setVisibility(1);
#if !defined(__ANDROID__) && !defined(__FLASHPLAYER__) && !defined(SB_IPHONE)
                    SbmDeformableMeshGPU::useGPUDeformableMesh = false;
#endif
                }
                else if (value == "GPUmesh")
                {
                    if (pawn->scene_p)
                        pawn->scene_p->set_visibility(0,0,0,0);
#if !defined(__ANDROID__) && !defined(__FLASHPLAYER__) && !defined(SB_IPHONE)
                    SbmDeformableMeshGPU::useGPUDeformableMesh = true;
#endif
                    if (pawn->dMeshInstance_p)
                        pawn->dMeshInstance_p->setVisibility(1);

                }
            }
        }

    }
}

void AppListener::OnSimulationUpdate()
{
    //return;
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

    const std::vector<std::string>& pawns = scene->getPawnNames();
    for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
         pawnIter != pawns.end();
         pawnIter++)
    {
        SmartBody::SBPawn* pawn = scene->getPawn((*pawnIter));
        if (pawn->scene_p)
        {
            pawn->scene_p->update();
            if (pawn->dMeshInstance_p)
            {
                //pawn->dMeshInstance_p->blendShapeStaticMesh();
                //pawn->dMeshInstance_p->update();
            }
        }
    }

}
