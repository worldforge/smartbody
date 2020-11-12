//
//  test.cpp
//  sbmwrapper
//
//  Created by Yuyu Xu on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vhcl.h"
#include "minimalwrapper.h"
#include <sbm/mcontrol_callbacks.h>
#include <sb/SBScene.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <sbm/xercesc_utils.hpp>
#include <sr/sr_camera.h>
#include <sb/SBPython.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAttribute.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmTexture.h>
#include <sr/sr_sa_gl_render.h>
#include <sr/sr_gl.h>
#include <sr/sr_light.h>
#include <sr/sr_camera.h>
#include <sr/sr_gl_render_funcs.h>
#include <sr/sr_euler.h>
#include <sb/SBSceneListener.h>
#include <sb/SBAssetManager.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

using namespace boost::filesystem;
using namespace SmartBody;
/*
    Known issue for smartbody ios:
    - vhcl log OnMessage doesn't seem to work inside static libraries, it only works on this application level, need to look more into it. Now I used printf before OnMessage function inside vhcl_log.cpp
 */


#if __cplusplus
extern "C"
{
#endif

static SrCamera* cameraReset;
static vhcl::Log::StdoutListener listener;
std::vector<SrLight> _lights;
void SBSetupDrawing(int w, int h)
{   
    glViewport(0, 0, w, h);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);
}


class AppListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
   public:
	  AppListener();
	  ~AppListener();

      virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
      virtual void OnCharacterDelete( const std::string & name );
	  virtual void OnCharacterUpdate( const std::string & name );
      virtual void OnPawnCreate( const std::string & name );
      virtual void OnPawnDelete( const std::string & name );

	  virtual void OnSimulationUpdate();

	  virtual void notify(SmartBody::SBSubject* subject);
};

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
	if (!pawn)
		return;

	// add attribute observations
	SmartBody::SBAttribute* attr = pawn->getAttribute("mesh");
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

void AppListener::OnPawnCreate( const std::string & name )
{
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
#if 0
		if (name == "mesh")
		{
		}
		else 
#endif
		if ( name == "deformableMeshScale")
		{
			//LOG("name = deformableMeshScale");
			SmartBody::DoubleAttribute* doubleAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
			if (doubleAttribute)
			{
				if (!pawn->dMeshInstance_p)
					pawn->dMeshInstance_p = new DeformableMeshInstance();
				pawn->dMeshInstance_p->setMeshScale(doubleAttribute->getValue());
				//LOG("Set mesh scale = %f",doubleAttribute->getValue());
			}			
		}
		else if (name == "deformableMesh" || name == "mesh")
		{
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			if (strAttribute)
			{
				const std::string& value = strAttribute->getValue();
				// clean up any old meshes
#if 0
				if (pawn->dMesh_p)
				{
					for (size_t i = 0; i < pawn->dMesh_p->dMeshDynamic_p.size(); i++)
					{
						scene->getRootGroup()->remove( pawn->dMesh_p->dMeshDynamic_p[i] );
					}
				}
#endif
				if (pawn->dMeshInstance_p)
				{
					//delete pawn->dMeshInstance_p;
				}
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
					if (!pawn->dMeshInstance_p)
						pawn->dMeshInstance_p = new DeformableMeshInstance();
					pawn->dMeshInstance_p->setDeformableMesh(mesh);
                    pawn->dMeshInstance_p->setPawn(pawn);
					if (name == "mesh") // setting static mesh
					{
						pawn->dMeshInstance_p->setToStaticMesh(true);
					}
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
                pawn->dMeshInstance_p->update();
        }
	}
	
}
  
void drawLights()
{
    _lights.clear();
	int numLightsInScene = 0;
	const std::vector<std::string>& pawnNames =  SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		 iter != pawnNames.end();
         iter++)
	{
		SmartBody::SBPawn* sbpawn = SmartBody::SBScene::getScene()->getPawn(*iter);
        if (!sbpawn)
            continue;
        
		const std::string& name = sbpawn->getName();
		if (name.find("light") == 0)
		{
			numLightsInScene++;
			SmartBody::BoolAttribute* enabledAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("enabled"));
			if (enabledAttr && !enabledAttr->getValue())
			{
				continue;
			}
			SrLight light;
            
			light.position = sbpawn->getPosition();
            
			SrQuat orientation = sbpawn->getOrientation();
			SmartBody::BoolAttribute* directionalAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("lightIsDirectional"));
			if (directionalAttr)
			{
				light.directional = directionalAttr->getValue();
			}
			else
			{
				light.directional = true;
			}
			if (light.directional)
			{
				light.position = -SrVec(0, 1, 0) * orientation;
			}
			
			SmartBody::Vec3Attribute* diffuseColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightDiffuseColor"));
			if (diffuseColorAttr)
			{
				const SrVec& color = diffuseColorAttr->getValue();
				light.diffuse = SrColor( color.x, color.y, color.z );
			}
			else
			{
				light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
			}
			SmartBody::Vec3Attribute* ambientColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightAmbientColor"));
			if (ambientColorAttr)
			{
				const SrVec& color = ambientColorAttr->getValue();
				light.ambient = SrColor( color.x, color.y, color.z );
			}
			else
			{
				light.ambient = SrColor( 0.0f, 0.0f, 0.0f );
			}
			SmartBody::Vec3Attribute* specularColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightSpecularColor"));
			if (specularColorAttr)
			{
				const SrVec& color = specularColorAttr->getValue();
				light.specular = SrColor( color.x, color.y, color.z );
			}
			else
			{
				light.specular = SrColor( 0.0f, 0.0f, 0.0f );
			}
			SmartBody::DoubleAttribute* spotExponentAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightSpotExponent"));
			if (spotExponentAttr)
			{
				light.spot_exponent = (float) spotExponentAttr->getValue();
			}
			else
			{
				light.spot_exponent = 0.0f;
			}
			SmartBody::Vec3Attribute* spotDirectionAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightSpotDirection"));
			if (spotDirectionAttr)
			{
				const SrVec& direction = spotDirectionAttr->getValue();
				light.spot_direction = direction;
				// override the explicit direction with orientation
				light.spot_direction = SrVec(0, 1, 0) * orientation;
			}
			else
			{
				light.spot_direction = SrVec( 0.0f, 0.0f, -1.0f );
			}
			SmartBody::DoubleAttribute* spotCutOffAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightSpotCutoff"));
			if (spotExponentAttr)
			{
				if (light.directional)
					light.spot_cutoff = 180.0f;
				else
					light.spot_cutoff = (float) spotCutOffAttr->getValue();
                
			}
			else
			{
				light.spot_cutoff = 180.0f;
			}
			SmartBody::DoubleAttribute* constantAttentuationAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightConstantAttenuation"));
			if (constantAttentuationAttr)
			{
				light.constant_attenuation = (float) constantAttentuationAttr->getValue();
			}
			else
			{
				light.constant_attenuation = 1.0f;
			}
			SmartBody::DoubleAttribute* linearAttentuationAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightLinearAttenuation"));
			if (linearAttentuationAttr)
			{
				light.linear_attenuation = (float) linearAttentuationAttr->getValue();
			}
			else
			{
				light.linear_attenuation = 0.0f;
			}
			SmartBody::DoubleAttribute* quadraticAttentuationAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightQuadraticAttenuation"));
			if (quadraticAttentuationAttr)
			{
				light.quadratic_attenuation = (float) quadraticAttentuationAttr->getValue();
			}
			else
			{
				light.quadratic_attenuation = 0.0f;
			}
			
			_lights.push_back(light);
		}
	}

    //LOG("light size = %d\n",_lights.size());
	
	if (_lights.size() == 0 && numLightsInScene == 0)
	{
		SrLight light;		
		light.directional = true;
		light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
		SrMat mat;
		sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(135	));
		SrQuat orientation(mat);
		SrVec up(0,1,0);
		SrVec lightDirection = -up * orientation;
		light.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
	//	light.constant_attenuation = 1.0f/cam.scale;
		light.constant_attenuation = 1.0f;
		_lights.push_back(light);

		SrLight light2 = light;
		light2.directional = true;
		light2.diffuse = SrColor( 0.8f, 0.8f, 0.8f );
		sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(-135));
		SrQuat orientation2(mat);
		lightDirection = -up * orientation2;
		light2.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
	//	light2.constant_attenuation = 1.0f;
	//	light2.linear_attenuation = 2.0f;
		_lights.push_back(light2);
	}
    
    
    glEnable(GL_LIGHTING);
	int maxLight = -1;
	for (size_t x = 0; x < _lights.size(); x++)
	{
		glLight ( x, _lights[x] );
		maxLight++;
	}
    
	if (maxLight < 0)
	{
		glDisable(GL_LIGHT0);
	}
	if (maxLight < 1)
	{
		glDisable(GL_LIGHT1);
	}
	if (maxLight < 2)
	{
		glDisable(GL_LIGHT2);
	}
	if (maxLight < 3)
	{
		glDisable(GL_LIGHT3);
	}
	if (maxLight < 4)
	{
		glDisable(GL_LIGHT4);
	}
	if (maxLight < 5)
	{
		glDisable(GL_LIGHT5);
	}
    
	if (maxLight > 0)
	{
		glEnable(GL_LIGHT0);
	}
	if (maxLight > 1)
	{
		glEnable(GL_LIGHT1);
	}
	if (maxLight > 2)
	{
		glEnable(GL_LIGHT2);
	}
	if (maxLight > 3)
	{
		glEnable(GL_LIGHT3);
	}
	if (maxLight > 4)
	{
		glEnable(GL_LIGHT4);
	}
	if (maxLight > 5)
	{
		glEnable(GL_LIGHT5);
	}
    
}
    
    
void SBInitialize(const char* mediapath)
{
    vhcl::Log::g_log.AddListener(&listener);
    XMLPlatformUtils::Initialize();
    LOG("media path%s", mediapath);
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	AppListener* appListener = new AppListener();
	scene->addSceneListener(appListener);
    SrCamera& cam = *scene->createCamera("activeCamera");
    cam.init();
    initPython("../../Python26/Libs");
    SmartBody::SBSimulationManager* sim = scene->getSimulationManager();
    sim->setupTimer();
    scene->setMediaPath(mediapath);
	scene->addAssetPath("seq", "sbm-common/scripts");
	scene->runScript("default-init.py");
    
    // store the camera information for resetting
    cameraReset = new SrCamera();
    cameraReset->copyCamera(&cam);
}
    
void SBDrawFrame(int width, int height)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    SrCamera& cam = *scene->getActiveCamera();
    
	// clear background
	glClearColor(1.0f,1.0f,1.0f,1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // setup view
    cam.setAspectRatio(float(width) / float(height));
	SrMat mat;
	glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
	glLoadMatrixf ( (const float*)cam.get_perspective_mat(mat) );
	glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity();
	glLoadMatrixf ( (const float*)cam.get_view_mat(mat) );
	glScalef ( cam.getScale(), cam.getScale(), cam.getScale());
    glViewport( 0, 0, width, height);

    
    //
    static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );
    
    // draw lights
    drawLights();
    
    // update texture
    glEnable(GL_TEXTURE_2D);
    SbmTextureManager& texm = SbmTextureManager::singleton();
    texm.updateTexture();
    
    /*
    // draw a ground plane
	glDisable(GL_LIGHTING);
	float planeSize  = 300.f;
	SrVec quad[4] = { SrVec(planeSize, 0.f, planeSize), SrVec(-planeSize, 0.f, planeSize), SrVec(-planeSize,0.f,-planeSize), SrVec(planeSize, 0.f, -planeSize) };
	SrVec quadN[4] = { SrVec(0.f, 1.f, 0.f), SrVec(0.f, 1.f, 0.f), SrVec(0.f, 1.f, 0.f), SrVec(0.f, 1.f, 0.f) };
	GLfloat quadColor[16] = { 0.2f,0.2f, 0.2f, 1.f , 0.3f,0.3f,0.3f, 1.f, 0.5f,0.5f,0.5f,1.f, 0.25f,0.25f,0.25f,1.f };
	unsigned short indices[] = {0,1,2, 0,2,3};
	glShadeModel(GL_SMOOTH);
	glDisable(GL_CULL_FACE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&quad[0]);
	glColorPointer(4, GL_FLOAT, 0, quadColor);
	glNormalPointer(GL_FLOAT, 0, (GLfloat*)&quadN[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnable(GL_LIGHTING);
     */
    
    // draw characters
    SBDrawCharacters();
}
    
void SBDrawCharacters()
{
    
#if 0   // draw bone figure
    const std::vector<std::string>& chars = SmartBody::SBScene::getScene()->getCharacterNames();
	for (std::vector<std::string>::const_iterator charIter = chars.begin();
         charIter != chars.end();
         charIter++)
	{
		SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter((*charIter));
        float jointPos[600];
        unsigned short boneIdx[400];
        int numJoints;
        SmartBody::SBSkeleton* sk = character->getSkeleton();
        sk->update_global_matrices();
        std::map<int,int> indexMap;
        numJoints = sk->joints().size();
        for (int i=0;i<sk->joints().size();i++)
        {
            SkJoint* joint = sk->joints()[i];
            SrVec pos = joint->gmat().get_translation();
            jointPos[i * 3 + 0] = pos.x;
            jointPos[i * 3 + 1] = pos.y;
            jointPos[i * 3 + 2] = pos.z;
            indexMap[joint->index()] = i;
            boneIdx[i*2+0] = joint->index();
            if (joint->parent())
                boneIdx[i*2+1] = joint->parent()->index();
            else
                boneIdx[i*2+1] = joint->index();
        }
        for (int i=0;i<sk->joints().size();i++)
        {
            boneIdx[i*2] = indexMap[boneIdx[i*2]];
            boneIdx[i*2+1] = indexMap[boneIdx[i*2+1]];
        }
        glPointSize(2.0f);
        glLineWidth(1.0f);
        glColor4f(1, 1, 1, 1);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, (const GLfloat*)&jointPos[0]);
        glDrawArrays(GL_POINTS, 0, numJoints);
        glDrawElements(GL_LINES,numJoints*2,GL_UNSIGNED_SHORT, boneIdx);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
#else   // draw CPU deformable mesh
	const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
         pawnIter != pawns.end();
         pawnIter++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
		if(pawn->dMeshInstance_p)
		{
			pawn->dMeshInstance_p->setVisibility(1);
			//pawn->dMeshInstance_p->update();
			SrGlRenderFuncs::renderDeformableMesh(pawn->dMeshInstance_p);
		}
	}
#endif
}
    
void SBUpdate(float t)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    SmartBody::SBSimulationManager* sim = scene->getSimulationManager();
    sim->update();
    scene->update();
}
    
void SBExecuteCmd(const char* command)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    scene->command(command);
    printf("%s\n", command);
}
    
void SBExecutePythonCmd(const char* command)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    scene->run(command);
    printf("%s\n", command);
}
    
    
void SBCameraOperation(float dx, float dy)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrCamera* cam = scene->getActiveCamera();
    
    float camDx = dx * cam->getAspectRatio() * 0.01f;
    float camDy = dy * cam->getAspectRatio() * 0.01f;
    
    if (cameraMode == 0) // zoom
    {
		float tmpFov = cam->getFov() + (-camDx + camDy);
		cam->setFov(SR_BOUND(tmpFov, 0.001f, srpi));
    }
    else if (cameraMode == 1) // rotation
    {   
        SrVec origUp = cam->getUpVector();
		SrVec origCenter = cam->getCenter();
		SrVec origCamera = cam->getEye();
		
        SrVec dirX = origUp;
        SrVec dirY;
        dirY.cross(origUp, (origCenter - origCamera));
        dirY /= dirY.len();
        
        SrVec cameraPoint = rotatePoint(origCamera, origCenter, dirX, -camDx * float(M_PI));
        //cameraPoint = rotatePoint(origCamera, origCenter, dirX, -camDy * float(M_PI));
        cam->setEye(cameraPoint.x, cameraPoint.y, cameraPoint.z);
    }
    else if (cameraMode == 2) // reset
    {
        cam->copyCamera(cameraReset);
    }
    else if (cameraMode == 3) // dolly
    {
        float amount = camDx - camDy;
        SrVec cameraPos(cam->getEye());
        SrVec targetPos(cam->getCenter());
        SrVec diff = targetPos - cameraPos;
        float distance = diff.len();
        diff.normalize();
        
        if (amount >= distance);
        amount = distance - 0.00001f;
        
        SrVec diffVector = diff;
        SrVec adjustment = diffVector * distance * amount;
        cameraPos += adjustment;
        SrVec oldEyePos = cam->getEye();
        cam->setEye(cameraPos.x, cameraPos.y, cameraPos.z);
        SrVec cameraDiff = cam->getEye() - oldEyePos;
        SrVec tmpCenter = cam->getCenter();
        tmpCenter += cameraDiff;
        cam->setCenter(tmpCenter.x, tmpCenter.y, tmpCenter.z);
    }
}
#if __cplusplus
}
#endif