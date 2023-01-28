#include "TransparentListener.h"
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sb/SBPawn.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAttribute.h>
#include <sb/SBAssetManager.h>

#include "TransparentViewer.h"


TransparentListener::TransparentListener()
{
}

TransparentListener::~TransparentListener()
{
}

void TransparentListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBPawn* pawn = scene->getPawn(name);
	if (!pawn)
		return;

	// add attribute observations
	SmartBody::SBAttribute* attr = pawn->getAttribute("mesh");
	if (attr)
		attr->registerObserver(this);

	attr = pawn->getAttribute("meshScale");
	if (attr)
		attr->registerObserver(this);

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

	
	TransparentViewer* window = dynamic_cast<TransparentViewer*>(SmartBody::SBScene::getScene()->getViewer());
	
	if (name.find("light") == 0)
	{
		pawn->registerObserver(this);		
		if (window)
		{
			window->viewer->updateLights();
		}
	
	}
	
}

void TransparentListener::OnCharacterDelete( const std::string & name )
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

}

void TransparentListener::OnCharacterUpdate( const std::string & name)
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
}

void TransparentListener::OnPawnCreate( const std::string & name )
{
	OnCharacterCreate(name, "");
}

void TransparentListener::OnPawnDelete( const std::string & name )
{
	OnCharacterDelete(name);
}


void TransparentListener::OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime )
{
}


void TransparentListener::OnChannel( const std::string & name, const std::string & channelName, const float value)
{
}

void TransparentListener::OnReset()
{
	

}

void TransparentListener::OnLogMessage(const std::string& message)
{
	SmartBody::util::log(message.c_str());
}


void TransparentListener::notify(SmartBody::SBSubject* subject)
{
	SmartBody::SBScene* scene =	SmartBody::SBScene::getScene();
	SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(subject);
	if (pawn)
	{
		const std::string& pawnName = pawn->getName();
		if (pawn->getName().find("light") == 0)
		{
			// adjust the lights based on the new position and color
			TransparentViewer* window = dynamic_cast<TransparentViewer*>(SmartBody::SBScene::getScene()->getViewer());
			if (window)
			{
#if 1 //!USE_OGRE_VIEWER
				window->viewer->updateLights();
#endif
			}


		}

	}
	
	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(attribute->getObject());
		SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(attribute->getObject());
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
#if 0
		if (name == "mesh")
		{
		}
		else 
#endif
		if ( name == "deformableMeshScale" || name == "meshScale")
		{
			//SmartBody::util::log("name = deformableMeshScale");
			bool useDeformableMesh = (name == "deformableMeshScale");
			SmartBody::Vec3Attribute* vec3Attribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
			if (vec3Attribute)
			{
				if (!pawn->dMeshInstance_p && useDeformableMesh)
					pawn->dMeshInstance_p = new SbmDeformableMeshGPUInstance();
				else if (!pawn->dStaticMeshInstance_p && !useDeformableMesh)
					pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
				
				DeformableMeshInstance* meshInstance = useDeformableMesh ? pawn->dMeshInstance_p : pawn->dStaticMeshInstance_p;
				meshInstance->setMeshScale(SrVec((float) vec3Attribute->getValue().x, (float) vec3Attribute->getValue().y, (float) vec3Attribute->getValue().z));
				//SmartBody::util::log("Set mesh scale = %f",doubleAttribute->getValue());
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
						pawn->dMeshInstance_p->setToStaticMesh(false);
					}
					else if (!pawn->dStaticMeshInstance_p && !useDeformableMesh)
					{
						pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
						pawn->dStaticMeshInstance_p->setToStaticMesh(true);
					}
					else if (!pawn->dStaticMeshInstance_p && name == "mesh")
					{
						pawn->dStaticMeshInstance_p = new SbmDeformableMeshGPUInstance();
						pawn->dStaticMeshInstance_p->setToStaticMesh(true);
					}

					// create attributes for all the blend shapes if there's any
					std::map<std::string, std::vector<SrSnModel*> > ::iterator iter = mesh->blendShapeMap.begin();
					for (; iter != mesh->blendShapeMap.end(); ++iter)
					{
						for (size_t c = 0; c < iter->second.size(); ++c)
						{
							if (strcmp(iter->first.c_str(), (const char*)iter->second[c]->shape().name) == 0)	// you don't create it for base shape
								continue;

							std::stringstream ss;
							ss << "blendShape.channelName." << (const char*)iter->second[c]->shape().name;
							std::stringstream ss1;
							ss1 << "blendShape.channelWeightLimit." << (const char*)iter->second[c]->shape().name;
							character->createStringAttribute(ss.str().c_str(), "", true, "Blend Shape", c + 1, false, false, false, "blend shape channel name");
							character->createDoubleAttribute(ss1.str().c_str(), 1.0f, true, "Blend Shape", c + 1, false, false, false, "blend shape channel weight limit");
							character->createActionAttribute("updateChannel", true, "Blend Shape", 0, false, false, false, "update blend shape channel");
						}
					}

					DeformableMeshInstance* meshInsance = useDeformableMesh ? pawn->dMeshInstance_p : pawn->dStaticMeshInstance_p;
					meshInsance->setDeformableMesh(mesh);
					//meshInsance->setSkeleton(pawn->getSkeleton());	
					meshInsance->setPawn(pawn);
					
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
				else if (value == "skinWeight")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(2);
				}
				else if (value == "mesh")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(1);
					SbmDeformableMeshGPU::useGPUDeformableMesh = false;
				}
				else if (value == "GPUmesh")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,0,0);
					SbmDeformableMeshGPU::useGPUDeformableMesh = true;
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(1);

				}
			}
		}
		
	}
}

void TransparentListener::OnSimulationUpdate()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	const std::vector<std::string>& pawns = scene->getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		pawnIter != pawns.end();
		pawnIter++)
	{
		SmartBody::SBPawn* pawn = scene->getPawn((*pawnIter));
 		if (pawn->scene_p)
 			pawn->scene_p->update();	
	}

	scene->updateTrackedCameras();
		
	if (scene->getViewer())
		scene->getViewer()->render();
}