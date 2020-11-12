//SmartBoday Javascript miniwrapper
//Author: Zengrui Wang @USC ICT
//Revised on 12/15/2015

#include "minimalwrapper.h"
#include "Shader.h"
//vhcl libs

//SmartBody libs
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <sb/SBScene.h>
#include <sb/SBJavascript.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAttribute.h>
#include <sb/SBSceneListener.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/xercesc_utils.hpp>
#include <sbm/mcontrol_callbacks.h>
#include <sr/sr_camera.h>
#include <sr/sr_sa_gl_render.h>
#include <sr/sr_gl.h>
#include <sr/sr_light.h>
#include <sr/sr_camera.h>
#include <sr/sr_gl_render_funcs.h>
#include <sr/sr_euler.h>

//OGL ES 2.0 and Emscripten libs
#if defined(EMSCRIPTEN)
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <emscripten.h>
#endif

#include <string>


using namespace std;
using namespace boost::filesystem;
using namespace SmartBody;

#if __cplusplus
extern "C"
{
#endif

	static SrCamera* cameraReset;
	static vhcl::Log::StdoutListener listener;
	bool useRealTimeClock = true;

	//AppListener Implementation
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

		attr = pawn->getAttribute("showBones");
		if (attr)
			attr->registerObserver(this);

		attr = pawn->getAttribute("showJoints");
		if (attr)
			attr->registerObserver(this);

		attr = pawn->getAttribute("displayBoneScale");
		if (attr)
			attr->registerObserver(this);

		attr = pawn->getAttribute("displayJointScale");
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
		// remove any existing deformable mesh?
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
		printf("AppListener::notify\n");
		SmartBody::SBScene* scene =	SmartBody::SBScene::getScene();
		SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(subject);

		SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
		if (attribute)
		{
			SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(attribute->getObject());
			const std::string& name = attribute->getName();
			printf("Attribute Name = %s\n",name.c_str());
			//visibility attribute
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
			
			//if(name == "mesh")
			//{
			//	SmartBody::util::log("name = mesh");
			//}
			//mesh attribute
			if ( name == "deformableMeshScale")
			{
				bool useDeformableMesh = (name == "deformableMeshScale");
				SmartBody::Vec3Attribute* vec3Attribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
				if (vec3Attribute)
				{
					if (!pawn->dMeshInstance_p && useDeformableMesh)
						pawn->dMeshInstance_p = new DeformableMeshInstance();
					else if (!pawn->dStaticMeshInstance_p && !useDeformableMesh)
						pawn->dStaticMeshInstance_p = new DeformableMeshInstance();

					DeformableMeshInstance* meshInstance = useDeformableMesh ? pawn->dMeshInstance_p : pawn->dStaticMeshInstance_p;
					SrVec scale(vec3Attribute->getValue().x, vec3Attribute->getValue().y, vec3Attribute->getValue().z);
					meshInstance->setMeshScale(scale);
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
					printf("Handle deformableMesh = %s\n", value.c_str());
					// clean up any old meshes?

					if (value.compare("") == 0) return;

					SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
					DeformableMesh* mesh = assetManager->getDeformableMesh(value);
					if (!mesh)
					{
						printf("Cannot find mesh '%s', load from the file, ", value.c_str());
						int index = value.find(".");
						if (index != std::string::npos)
						{
							std::string prefix = value.substr(0, index);
							const std::vector<std::string>& meshPaths = assetManager->getAssetPaths("mesh");
							for (size_t x = 0; x < meshPaths.size(); x++)
							{
								std::string assetLoc = meshPaths[x] + "/" + prefix + "/" + value;
								printf("try to load mesh at '%s'\n", assetLoc.c_str());
								assetManager->loadAsset(assetLoc);
							}
						}
						mesh = assetManager->getDeformableMesh(value);
					}

					if (mesh)
					{
						if (!pawn->dMeshInstance_p && useDeformableMesh)
						{
							pawn->dMeshInstance_p = new DeformableMeshInstance();
							pawn->dMeshInstance_p->setToStaticMesh(false);
						}
						else if (!pawn->dStaticMeshInstance_p && !useDeformableMesh)
						{
							pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
							pawn->dStaticMeshInstance_p->setToStaticMesh(true);

						}
						else if (!pawn->dStaticMeshInstance_p && name == "mesh")
						{
							pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
							pawn->dStaticMeshInstance_p->setToStaticMesh(true);
						}

						DeformableMeshInstance* meshInstance = useDeformableMesh ? pawn->dMeshInstance_p : pawn->dStaticMeshInstance_p;
						meshInstance->setDeformableMesh(mesh);
						meshInstance->setPawn(pawn);
						printf("Has mesh %s, num. of vertices = %d\n", mesh->getName().c_str(), mesh->posBuf.size());
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
					//SmartBody Javascript uses GPU rendering for meshes by default
					//Also the rendering part is done out of SmartBody lib
					else if (value == "mesh")
					{
						if (pawn->scene_p)
							pawn->scene_p->set_visibility(0,0,0,0);
						if (pawn->dMeshInstance_p)
							pawn->dMeshInstance_p->setVisibility(1);

#if !defined(__ANDROID__) && !defined(__FLASHPLAYER__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
						SbmDeformableMeshGPU::useGPUDeformableMesh = false;
#endif          
					}
					else if (value == "GPUmesh")
					{
						if (pawn->scene_p)
							pawn->scene_p->set_visibility(0,0,0,0);
#if !defined(__ANDROID__) && !defined(__FLASHPLAYER__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
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

	//rendering wrapper
	void SBSetupDrawing(int w, int h)
	{   
		glClearColor ( 0.6f, 0.6f, 0.6f, 1.0f );
		glViewport(0, 0, w, h);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		glEnable(GL_CULL_FACE);
#ifndef EMSCRIPTEN
		glShadeModel(GL_SMOOTH); //No support or similar conversion
#endif		
		glDisable(GL_DEPTH_TEST);
	}

	void SBInitialize(ESContext *esContext)
	{
		//vhcl log listener
		vhcl::Log::g_log.AddListener(&listener);
		
		//xerces-c initialization
		XMLPlatformUtils::Initialize();

		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		AppListener* appListener = new AppListener();
		scene->addSceneListener(appListener);
		
		SmartBody::SBSimulationManager* sim = scene->getSimulationManager();
		if (useRealTimeClock){
			sim->setupTimer();
		}
		else{
			// otherwise, the time will run according
			sim->setTime(0.0);
		}
		//initialize camera
		SrCamera* cam = scene->createCamera("activeCamera");
		//cam->init();
		cameraReset = new SrCamera(cam);
		//set some render attributes
		scene->createBoolAttribute("enableAlphaBlend", false, true, "", 5, false, false, false, "enable alpha blending?");
		scene->createBoolAttribute("lightEnabled", true, true, "", 5, false, false, false, "enable lighting blending?");
		//set up javascript, expose some global variables
		setupJavascript();
		
		//Initialize shader and rendering
		//Loading shader program, binding buffer location
		shaderInit(esContext);
#if 0
		//load additional textures for sky box
		SbmTextureManager& texManager = SbmTextureManager::singleton();
		SbmCubeMapTexture* tex = texManager.findCubeMapTexture("skybox");

		if (!tex)
		{
			string textureNames[] = {"posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png"};
			vector<string> textureNames_v;
			vector<string> textureFileNames_v;
			for(int i = 0; i < 6; ++i){
				std::string textureFileName = "textures/" + textureNames[i];
				if (!boost::filesystem::exists(boost::filesystem::path(textureFileName)))
					SmartBody::util::log("Texture %s doesn't exist under same path of mesh %s", textureNames[i].c_str(), textureFileName.c_str());
				textureNames_v.push_back(textureNames[i]);
				textureFileNames_v.push_back(textureFileName);
			}
			
			texManager.loadCubeMapTextures("skybox", textureNames_v, textureFileNames_v);
		}
		tex = texManager.findCubeMapTexture("skybox");
		SmartBody::util::log("size of cubemap: %d\n", tex->getBufferSize());
#endif
	}

	void SBUpdate(float t)
	{
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		SmartBody::SBSimulationManager* sim = scene->getSimulationManager();
		sim->update();
		scene->update();
	}

	void SBDrawFrame(ESContext *esContext)
	{
		float width = (float)esContext->width;
		float height = (float)esContext->height;

		MeshData *meshData = (MeshData*) esContext->meshData;
		GeometryData *geoData = (GeometryData*) esContext->geometryData;
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

		// setup textures
		SbmTextureManager& texm = SbmTextureManager::singleton();
		texm.updateTexture();

		//clear background
		glClearColor ( 0.6f, 0.6f, 0.6f, 1.0f );
		glViewport( 0, 0, width, height);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//setup view
		SrCamera& cam = *scene->getActiveCamera();
		cam.setAspectRatio(width / height);

		SrMat matPerspective, matModelView, additionalXFormMat;
		cam.get_perspective_mat(matPerspective);
		cam.get_view_mat(matModelView);
		matModelView.get_rotation();
		//this additional transformation is used for mesh manipulation purpose, need to find a better place to store it.
		cam.getAdditionalXForm(additionalXFormMat);
		//why we multiply the scale to every entry of the matrix?
		matModelView *= cam.getScale();
		
		meshData->gMVP = matModelView * matPerspective;
		meshData->gMV  = matModelView;
		//use the shape program object
		glUseProgram (geoData->programObject );
#if 0
		geoData->gMVP = matModelView.get_rotation() * matPerspective;
		geoData->gMV  = matModelView.get_rotation();
		glUniformMatrix4fv(geoData->mvpLoc, 1, GL_FALSE, (GLfloat *)geoData->gMVP.pt(0));
		glUniformMatrix4fv(geoData->mvLoc, 1, GL_FALSE, (GLfloat *)geoData->gMV.pt(0));
		drawSkyBox(esContext);
#endif
		geoData->gMVP = meshData->gMVP;
		geoData->gMV  = meshData->gMV;
		glUniformMatrix4fv(geoData->mvpLoc, 1, GL_FALSE, (GLfloat *)geoData->gMVP.pt(0));
		glUniformMatrix4fv(geoData->mvLoc, 1, GL_FALSE, (GLfloat *)geoData->gMV.pt(0));
		drawGrid(esContext);
		SBDrawPawns(esContext);
		drawCoordinates(esContext);
		//draw lights
		drawLights(esContext);
		// Use the program object
		glUseProgram ( meshData->programObject );
		glUniformMatrix4fv(meshData->mvpLoc, 1, GL_FALSE, (GLfloat *)meshData->gMVP.pt(0));
		glUniformMatrix4fv(meshData->mvLoc, 1, GL_FALSE, (GLfloat *)geoData->gMV.pt(0));
		SBDrawCharacters(esContext);
	}

	void SBDrawCharacters(ESContext *esContext)
	{
		//static bool meshUpdated = false;
		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
		{
			SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
			if(pawn->dMeshInstance_p)
			{
				pawn->dMeshInstance_p->setVisibility(1);
				//if (!meshUpdated)
				pawn->dMeshInstance_p->update();
				if(pawn->dMeshInstance_p->getDeformableMesh())
					drawMesh(pawn->dMeshInstance_p, esContext, false);
				else
					drawSkeleton(esContext);
			}
		}
		//meshUpdated = true;
	}

	void SBDrawPawns(ESContext *esContext){
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		SrCamera* currentCamera = scene->getActiveCamera();
		GeometryData *geoData = (GeometryData*) esContext->geometryData;

		// determine the size of the pawns relative to the size of the characters
		float pawnSize = 1.0 / 20.0;
		const std::vector<std::string>& characterNames = scene->getCharacterNames();
		for (std::vector<std::string>::const_iterator iter = characterNames.begin();
			iter != characterNames.end();
			iter++)
		{
			SmartBody::SBCharacter* character = scene->getCharacter((*iter));
			pawnSize = character->getHeight()/ 30.0f;
			break;
		}

		const std::vector<std::string>& pawnNames = scene->getPawnNames();
		for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
			iter != pawnNames.end();
			iter++)
		{

			SmartBody::SBPawn* pawn = scene->getPawn((*iter));

			SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
			if (camera)
			{ 
				if ((scene->getNumCameras() == 1) || (camera == currentCamera)) // don't draw the current active camera
					continue;
			}

			if (!pawn->getSkeleton())
				continue;
			if(pawn->getStringAttribute("mesh").compare("") != 0){
				std::string meshName = pawn->getStringAttribute("mesh");
				DeformableMesh* mesh = scene->getAssetManager()->getDeformableMesh(meshName);
				SrBox bbox = mesh->computeBoundingBox();
				SrVec extent = (bbox.b - bbox.a) * 0.5;
				//SmartBody::util::log("curChar = %s, bounding box : max = %f %f %f, min = %f %f %f",pawn->getName().c_str(),bbox.b[0],bbox.b[1],bbox.b[2],bbox.a[0],bbox.a[1],bbox.a[2]);
				//drawBBox(esContext, bbox.getCenter(), extent);
			}
			SbmCharacter* character = dynamic_cast<SbmCharacter*>(pawn);
			if (character){
				//continue;
				SrBox bbox = character->getBoundingBox();
				//SmartBody::util::log("curChar = %s, bounding box : max = %f %f %f, min = %f %f %f",character->getName().c_str(),bbox.b[0],bbox.b[1],bbox.b[2],bbox.a[0],bbox.a[1],bbox.a[2]);
				SrVec extent = (bbox.b - bbox.a) * 0.5;
				//drawBBox(esContext, bbox.getCenter(), extent);
			}

			const bool isVisible = pawn->getBoolAttribute("visible");
			if (!isVisible)
				continue;

			SrMat gmat = pawn->get_world_offset();//pawn->get_world_offset_joint()->gmat();
			if (pawn->getGeomObject() && dynamic_cast<SBGeomNullObject*>(pawn->getGeomObject()) == NULL)
			{
				//drawColObject(pawn->getGeomObject(),gmat);
				//draw collision object?
				SBDrawColObject(esContext, pawn->getGeomObject(), pawnSize);
				continue;
			}
			else
			{
				SrVec color;
				SmartBody::SBAttribute* attr = pawn->getAttribute("color");
				if (attr)
				{
					SmartBody::Vec3Attribute* colorAttribute = dynamic_cast<SmartBody::Vec3Attribute*>(attr);
					if (colorAttribute)
						color.set(colorAttribute->getValue());
				}
				else
				{
					if (camera)
						color.set(SrVec(1.0, 1.0, 0.0));
					else
						color.set(SrVec(1.0, 0.0, 0.0));
				}
				drawSphere(esContext, pawn->getPosition(), pawnSize, 15, color);
			}
		}
	}  

	void SBDrawColObject(ESContext *esContext, SBGeomObject* colObj, float pawnSize){
		
		SrColor objColor;
		objColor = colObj->color;
		objColor.a = (srbyte)255;

		float color_4f[4];
		objColor.get(color_4f);
		SrVec color(color_4f[0], color_4f[1], color_4f[2]);

		if (dynamic_cast<SBGeomSphere*>(colObj))
		{
			// draw sphere
			SBGeomSphere* sph = dynamic_cast<SBGeomSphere*>(colObj);
			drawSphere(esContext, colObj->getCenter(), sph->getRadius(), 15, color);		
		}
		else if (dynamic_cast<SBGeomBox*>(colObj))
		{
			//draw box
			SBGeomBox* box = dynamic_cast<SBGeomBox*>(colObj);
			drawBox(esContext, colObj->getCenter(), box->extent, color);	
		}
		else if (dynamic_cast<SBGeomCapsule*>(colObj))
		{
			//SmartBody::util::log("Draw Col Object Capsule");
		}
		else if (dynamic_cast<SBGeomTriMesh*>(colObj))
		{
			//SmartBody::util::log("Draw Col Object Tri Mesh");
		}

	}
	
	void SBCameraOperation(float dx, float dy)
	{
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		SrCamera* cam = scene->getActiveCamera();

		float camDx = dx * cam->getAspectRatio() * 0.01f;
		float camDy = dy * cam->getAspectRatio() * 0.01f;
		int cameraMode = 1; // hard coded for now
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
