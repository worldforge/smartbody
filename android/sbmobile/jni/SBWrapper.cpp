//
//  test.cpp
//  sbmwrapper
//
//  Created by Yuyu Xu on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vhcl.h"
#include <vhmsg-tt.h>
#include "SBWrapper.h"

#include <sb/SBScene.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBPython.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAttribute.h>
#include <sb/SBSceneListener.h>
#include <sb/SBAssetManager.h>
#include <sb/SBUtilities.h>

#include <sbm/mcontrol_callbacks.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/GPU/SbmShader.h>
#include <sbm/xercesc_utils.hpp>

#include <sr/sr_camera.h>
#include <sr/sr_sa_gl_render.h>
#include <sr/sr_gl.h>
#include <sr/sr_light.h>
#include <sr/sr_camera.h>
#include <sr/sr_gl_render_funcs.h>
#include <sr/sr_euler.h>
#include <sbm/gwiz_math.h>


#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <sbm/GPU/SbmDeformableMeshGPU.h>

#include "AppListener.h"
#include "Shader.h"

#if defined(ANDROID_BUILD)
//#include <EGL/egl.h>
//#include <GLES/gl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
//#include "wes.h"
//#include "wes_gl.h"
#elif defined(SB_IPHONE)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

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
#if USE_GL_FIXED_PIPELINE
std::vector<SrLight> _lights;
#endif

#if defined(SB_IPHONE)
  static int curH, curW;
  
  struct SceneListener : public SmartBody::SBSceneListener {
    void (*handler)(const char*);
    void OnLogMessage( const std::string & message ) override {
      handler(message.c_str());
    }
  };
  
  static SceneListener sceneListener;

  void SBIOSInitialize(const char* path, void (*listener)(const char*))
  {
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

    if (listener) {
      sceneListener.handler = listener;
      scene->addSceneListener(&sceneListener);
    }
    
    SmartBody::util::log("Start running SBIOSInitialize");
    std::string path1 = path;
    path1 += "/";
    SBSetup(path1.c_str(), "setup.py");
    SBInitialize();

    scene->addAssetPath("script", "scripts");
    SBInitScene("init.py");
    SmartBody::util::log("After running SBIOSInitialize");
  }
  
  void SBIOSSetupDrawing(int width, int height)
  {
    esContext.width = width;
    esContext.height = height;
    SBSetupDrawing(width,height, &esContext);
    curW = width;
    curH = height;
  }
  
  void SBIOSDrawFrame()
  {
    SrMat id;
    //SBDrawFrame(VHEngine::curW, VHEngine::curH, id);
    SBDrawFrame_ES20(curW, curH, &esContext, id);
  }
  
  void SBIOSReloadTexture()
  {
    SBInitGraphics(&esContext);
    SbmTextureManager& texm = SbmTextureManager::singleton();
    texm.reloadTexture();
  }
  
#endif
  
  const char* SBGetStringAttribute(const char* inAttributeName)
  {
    return SmartBody::SBScene::getScene()->getStringAttribute(inAttributeName)
    .c_str();
  }
  
  int SBGetBoolAttribute(const char* inAttributeName)
  {
    return SmartBody::SBScene::getScene()->getBoolAttribute(inAttributeName)
    ? 1 : 0;
  }
  
  double SBGetDoubleAttribute(const char* inAttributeName)
  {
    return SmartBody::SBScene::getScene()->getDoubleAttribute(inAttributeName);
  }
  
  int SBGetIntAttribute(const char* inAttributeName)
  {
    return SmartBody::SBScene::getScene()->getIntAttribute(inAttributeName);
  }
  
  
  
void setupLights()
{
	#if USE_GL_FIXED_PIPELINE
    _lights.clear();
	int numLightsInScene = 0;
	
	const std::vector<std::string>& pawnNames =  SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		 iter != pawnNames.end();
         iter++)
	{
		SmartBody::SBPawn* sbpawn = SmartBody::SBScene::getScene()->getPawn(*iter);
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
				light.diffuse = SrColor( 1.0f, 0.95f, 0.8f );
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

    //SmartBody::util::log("light size = %d\n",_lights.size());
	
	if (_lights.size() == 0 && numLightsInScene == 0)
	{
		SrLight light;		
		light.directional = true;
		light.diffuse = SrColor( 1.0f, 0.0f, 0.0f );
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
		light2.diffuse = SrColor( 0.8f, 0.0f, 0.0f );
		sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(-135));
		SrQuat orientation2(mat);
		lightDirection = -up * orientation2;
		light2.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
	//	light2.constant_attenuation = 1.0f;
	//	light2.linear_attenuation = 2.0f;
		_lights.push_back(light2);
	}
    
    
    myGLEnable(GL_LIGHTING);
	int maxLight = -1;
	for (size_t x = 0; x < _lights.size(); x++)
	{
		glLight ( x, _lights[x] );
		maxLight++;
	}
    
	if (maxLight < 0)
	{
		myGLDisable(GL_LIGHT0);
	}
	if (maxLight < 1)
	{
		myGLDisable(GL_LIGHT1);
	}
	if (maxLight < 2)
	{
		myGLDisable(GL_LIGHT2);
	}
	if (maxLight < 3)
	{
		myGLDisable(GL_LIGHT3);
	}
	if (maxLight < 4)
	{
		myGLDisable(GL_LIGHT4);
	}
	if (maxLight < 5)
	{
		myGLDisable(GL_LIGHT5);
	}
    
	if (maxLight > 0)
	{
		myGLEnable(GL_LIGHT0);
	}
	if (maxLight > 1)
	{
		myGLEnable(GL_LIGHT1);
	}
	if (maxLight > 2)
	{
		myGLEnable(GL_LIGHT2);
	}
	if (maxLight > 3)
	{
		myGLEnable(GL_LIGHT3);
	}
	if (maxLight > 4)
	{
		myGLEnable(GL_LIGHT4);
	}
	if (maxLight > 5)
	{
		myGLEnable(GL_LIGHT5);
	}
	#endif
}

void SBSetupDrawing(int w, int h, ESContext *esContext)
{   
	static bool firstSetupDrawing = true;
	if (firstSetupDrawing)
	{
		//SmartBody::util::log("First setup drawing, glViewport = %d %d", w, h);
		glViewport(0, 0, w, h);
		firstSetupDrawing = false;
	}
	
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	//myGLEnable(GL_CULL_FACE);
	//glShadeModel(GL_SMOOTH);
	//myGLDisable(GL_DEPTH_TEST);
}

void SBSetup(const char* mediapath, const char* setupScript)
{
	vhcl::Log::g_log.AddListener(&listener);
	XMLPlatformUtils::Initialize();
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string mpath = mediapath;
    std::string pythonPath = mpath + "pythonLib_27/Lib";
    SmartBody::util::log("python path: %s", pythonPath.c_str());
    initPython(pythonPath);
    SmartBody::util::log("media path: %s", mediapath);
	scene->setMediaPath(mediapath);
	scene->addAssetPath("script", ".");
	scene->runScript(setupScript);
}
    
void SBInitialize()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	AppListener* appListener = new AppListener();
	scene->addSceneListener(appListener);
	SrCamera& cam = *scene->createCamera("activeCamera");
	cam.init();
    SmartBody::SBSimulationManager* sim = scene->getSimulationManager();
    sim->setupTimer();
    scene->createBoolAttribute("enableAlphaBlend", true, true, "", 5, false, false, false, "enable alpha blending?");
	scene->createStringAttribute("background_img", "none", true, "", 5, false, false, false, "enable alpha blending?");
	//SBSetBackground("/sdcard/acdata/background.png", "background.png");
}

void SBInitGraphics(ESContext *esContext) {
	//SmartBody::util::log("Before shaderInit");
	shaderInit(esContext);
	//SmartBody::util::log("After shaderInit");
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();	
	scene->createIntAttribute("renderer.fboTexID",- 1, true,"",40,false,false,false,"texture id attached with the current fbo. For fast video recording.");
	scene->setIntAttribute("renderer.fboTexID", esContext->fboTexID);
	//SmartBody::util::log("fboTexID = %d", esContext->fboTexID);
}

void SBSetBackground(const char* fileName, const char* textureName, int textureRotate)
{
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();	
	
	std::string newTex = textureName;
	std::string oldTex = scene->getStringAttribute("background_img");
	//if (oldTex == newTex)
	texManager.deleteTexture(SbmTextureManager::TEXTURE_DIFFUSE, oldTex.c_str());

	if (newTex == "none") return; // delete texture directly

	if (texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, textureName) == NULL) // need to load the texture
	{		
		texManager.loadTexture(SbmTextureManager::TEXTURE_DIFFUSE, textureName, fileName);
		/*
		if (textureRotate == 90)
		{
			SbmTexture* tex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, textureName);
			tex->rotateTexture(SbmTexture::ROTATE_90);
		}
		*/
	}	
	scene->setStringAttribute("background_img", textureName);
}



void SBInitScene( const char* initScriptName )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	scene->addAssetPath("script", ".");
	scene->runScript(initScriptName);
	SrCamera& cam = *scene->getCamera("activeCamera");
	cameraReset = new SrCamera(cam);
}

void SBDrawFrameAR(int width, int height, ESContext *esContext, SrMat modelViewMat, SrMat projMat)
{
	static bool initShader = false;
	static bool firstRender = true;
	if (!initShader)
	{
		//SmartBody::util::log("Shader not initialized, run SBInitGraphics.");
		SBInitGraphics(esContext);
		initShader = true;
	}


	UserData *userData = (UserData*)esContext->userData;
	ShapeData *shapeData = (ShapeData*)esContext->shapeData;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	// setup textures
	SbmTextureManager& texm = SbmTextureManager::singleton();
	texm.updateTexture();

	// clear background	
	SBSetupDrawing(width, height, esContext);

	SBUpdateCharacterGPUSkin();
	//cam.print();
	// setup view
	if (firstRender)
	{
		SrCamera& cam = *scene->getActiveCamera();
		float aspectRatio = float(width) / float(height);
		cam.setAspectRatio(aspectRatio);
		//SmartBody::util::log("First render, aspect ratio = %f", aspectRatio);
		firstRender = false;
	}



	SrMat matModelView, matMVP;
	matModelView = modelViewMat;
	matMVP = matModelView * projMat;
	//use the shape program object
	//SmartBody::util::log("Before use program");
	glUseProgram(shapeData->programObject);
	//SmartBody::util::log("After use program");
	glUniformMatrix4fv(shapeData->mvpLoc, 1, GL_FALSE, (GLfloat *)matMVP.pt(0));
	glUniformMatrix4fv(shapeData->mvLoc, 1, GL_FALSE, (GLfloat *)matModelView.pt(0));
	//SmartBody::util::log("Before draw pawns");
	SBDrawPawns(esContext);
	// Use the program object
	//SmartBody::util::log("Before userData useProgram");
	glUseProgram(userData->programObject);
	glUniformMatrix4fv(userData->mvLoc, 1, GL_FALSE, (GLfloat *)matModelView.pt(0));
	glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat *)matMVP.pt(0));
	//SmartBody::util::log("Before SBDrawCharacters_ES20");
	//SBDrawCharacters_ES20(esContext);
	SBDrawCharacterGPUSkin(esContext);
	//draw lights
	//SmartBody::util::log("After SBDrawCharacters_ES20");
	drawLights(esContext);


	// No background rendering in AR Mode
	//SBDrawBackground(esContext);	
}



void SBDrawFrame_ES20(int width, int height, ESContext *esContext, SrMat eyeView, bool drawAR ) {
	//SmartBody::util::log("draw!");
	SbmShaderProgram::printOglError("drawFrame start");
	static bool initShader = false;
	static bool firstRender = true;
	if (!initShader)
	{
		//SmartBody::util::log("Shader not initialized, run SBInitGraphics.");
		SBInitGraphics(esContext);
        initShader = true;
	}
	//return;


	UserData *userData = (UserData*) esContext->userData;
	ShapeData *shapeData = (ShapeData*) esContext->shapeData;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrVec backgroundColor = scene->getVec3Attribute("GUI.BackgroundColor");
	
	SbmShaderProgram::printOglError("drawFrame #0");
	// setup textures
	SbmTextureManager& texm = SbmTextureManager::singleton();
	texm.updateTexture();
	SbmShaderProgram::printOglError("drawFrame #1");
	//SbmShaderManager& ssm = SbmShaderManager::singleton();
	//ssm.buildShaders();


	//bool useRenderTarget = false;
	//if (useRenderTarget)
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, esContext->fboID);
	SrCamera& cam = *scene->getActiveCamera();
	// clear background
	//glClearColor( 0.0f,0.0f,0.0f,1.0f );
	//glClearColor ( 0.6f, 0.6f, 0.6f, 1.0f );
	SBSetupDrawing(width, height, esContext);

	SbmShaderProgram::printOglError("drawFrame #2");
	if (!drawAR)
	{
		//glClearColor(0.33f, 0.78f, 0.95f, 1.f);
		//SmartBody::util::log("Background color = %f %f %f", backgroundColor[0], backgroundColor[1], backgroundColor[2]);
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	SbmShaderProgram::printOglError("drawFrame #3");
	SBUpdateCharacterGPUSkin();
	SbmShaderProgram::printOglError("drawFrame #4");

	
	
	//cam.print();
	// setup view
	if (firstRender)
	{
		float aspectRatio = float(width) / float(height);
		cam.setAspectRatio(aspectRatio);
		//SmartBody::util::log("First render, aspect ratio = %f", aspectRatio);
		firstRender = false;
	}

	
	
	SrMat matPerspective, matModelView;
	cam.get_perspective_mat(matPerspective);
	cam.get_view_mat(matModelView);
	matModelView *= cam.getScale();
	//matModelView = matModelView*eyeView;
	matModelView = eyeView*matModelView;
	//glViewport( 0, 0, width, height);
	SrMat matMVP = matModelView * matPerspective;
	//use the shape program object
	//SmartBody::util::log("Before use program");
	glUseProgram (shapeData->programObject );
	SbmShaderProgram::printOglError("drawFrame #5");
	//SmartBody::util::log("After use program");
	glUniformMatrix4fv(shapeData->mvpLoc, 1, GL_FALSE, (GLfloat *)matMVP.pt(0));
	glUniformMatrix4fv(shapeData->mvLoc, 1, GL_FALSE, (GLfloat *)matModelView.pt(0));
	//SmartBody::util::log("Before draw grid");
	//drawGrid(esContext);
	//SmartBody::util::log("Before draw pawns");
	SBDrawPawns(esContext);
	SbmShaderProgram::printOglError("drawFrame #6");
	// Use the program object
	//SmartBody::util::log("Before userData useProgram");
	glUseProgram ( userData->programObject );
	glUniformMatrix4fv(userData->mvLoc, 1, GL_FALSE, (GLfloat *)matModelView.pt(0));
	glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat *)matMVP.pt(0));
	//SmartBody::util::log("Before SBDrawCharacters_ES20");
	//SBDrawCharacters_ES20(esContext);
	SBDrawCharacterGPUSkin(esContext);
	SbmShaderProgram::printOglError("drawFrame #7");
	//draw lights
	//SmartBody::util::log("After SBDrawCharacters_ES20");
	drawLights(esContext);

	// draw background
	SBDrawBackground(esContext);
	SbmShaderProgram::printOglError("drawFrame #8");
	//SmartBody::util::log("SBDrawFrame_ES20::drawRenderTarget");
	/*
	if (useRenderTarget)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);		
		SBDrawFBOTex_ES20(width, height, esContext, eyeView);
	}
	*/
	//SmartBody::util::log("After drawLights");
}

void SBDrawFBOTex_ES20(int w, int h, ESContext *esContext, SrMat eyeView, int texID)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 0.0, 0.0, 1.0);
	//glViewport( 0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SrVec2 texScale = SrVec2(1,1), texOffset = SrVec2(0,0);
	if (texID == -1) 
		texID = esContext->fboTexID;
	drawBackgroundTexID(texID, texScale, texOffset, esContext);
}

void SBUpdateCharacterGPUSkin()
{
	const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();
	for (unsigned int i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnNames[i]);
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if (meshInstance)
		{
			GPUBlendShapeUpdate(meshInstance);
			if (!meshInstance->isStaticMesh())
			{
				SbmDeformableMeshGPUInstance* gpuMeshInstance = dynamic_cast<SbmDeformableMeshGPUInstance*>(meshInstance);
				GPUMeshUpdate(meshInstance);
			}
		}
	}
}

void SBDrawCharacterGPUSkin(ESContext *esContext)
{
	const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();
	for (unsigned int i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnNames[i]);
		//SmartBody::util::log("draw pawn %s", pawn->getName().c_str());
		//DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if (meshInstance)
		{
			drawMeshStaticVBO(meshInstance, esContext, false);
		}
	}

	
}

void SBDrawCharacters_ES20(ESContext *esContext) {
	static bool meshUpdated = false;
	const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
	//printf("draw pawns::numOfPawns: %d\n", pawns.size());
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		 pawnIter != pawns.end();
		 pawnIter++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
		//SmartBody::util::log("draw pawn %s", pawn->getName().c_str());
		//DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if(meshInstance)
		{	
			#if 0
			if (!meshInstance->isStaticMesh())
			{
				//SmartBody::util::log("pawn %s is deformable mesh", pawn->getName().c_str());
				meshInstance->blendShapeStaticMesh();
				//meshInstance->setVisibility(1);
				//if (!meshInstance->isStaticMesh() && !meshUpdated)
				meshInstance->update();
			}
			else
			{
				//SmartBody::util::log("pawn %s is static mesh", pawn->getName().c_str());
			}
			#endif
			
			//if(pawn->dMeshInstance_p->getDeformableMesh())
			drawMeshStatic(meshInstance, esContext, false);
			//else
             //   drawSkeleton(esContext);
		}
	}
	meshUpdated = true;
}

void SBDrawPawns(ESContext *esContext) {

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& pawnNames = scene->getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		 iter != pawnNames.end();
		 iter++)
	{

		SmartBody::SBPawn *pawn = scene->getPawn((*iter));

		if (!pawn->getSkeleton())
			continue;

		SmartBody::SBCharacter *character = dynamic_cast<SmartBody::SBCharacter*>(pawn);
		if (character)
			continue;

		const bool isVisible = pawn->getBoolAttribute("visible");
		if (!isVisible)
			continue;
		SrMat gmat = pawn->get_world_offset();//pawn->get_world_offset_joint()->gmat();
		if (pawn->dMeshInstance_p) {
			pawn->dMeshInstance_p->setVisibility(1);
			if (pawn->dMeshInstance_p->getDeformableMesh())
				drawMeshStatic(pawn->dMeshInstance_p, esContext, false);
		}
	}
}

void SBDrawGrid(ESContext *esContext) {
    drawGrid(esContext);
}


    
void SBCreateCustomMeshFromBlendshapes(std::string templateMeshName, std::string blendshapesDir, std::string baseMeshName, std::string hairMeshName, std::string outMeshName)
{
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = assetManager->getDeformableMesh(templateMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Error creating custom mesh from blendshapes :: mesh '%s' does not exist.", templateMeshName.c_str());
		return;
	}

	for (std::map<std::string, std::vector<SrSnModel*> >::iterator iter = mesh->blendShapeMap.begin();
		iter != mesh->blendShapeMap.end();
		iter++)
	{
		std::vector<SrSnModel*>& targets = (*iter).second;
		for (size_t t = 0; t < targets.size(); t++) // ignore first target since it is a base mesh
		{
			if (targets[t] == NULL)
				continue;
			SrModel& curModel = targets[t]->shape();
			SrModel newShape;
			std::string blendshapeName = (const char*)curModel.name;

			if (t == 0)
				blendshapeName = baseMeshName;
			//SmartBody::util::log("mesh '%s', blendShapeName = '%s'", templateMeshName.c_str(), blendshapeName.c_str());
			std::string meshFileNamae = blendshapesDir + "/" + blendshapeName;
			newShape.import_ply(meshFileNamae.c_str());
			if (newShape.V.size() == curModel.V.size())
			{
				curModel.V = newShape.V;
				curModel.N = newShape.N;
			}
			if (t == 0)
			{
				SrModel& staticBaseModel = mesh->dMeshStatic_p[0]->shape();
				staticBaseModel.V = newShape.V;
				staticBaseModel.N = newShape.N;

				SrModel& dynamicBaseModel = mesh->dMeshDynamic_p[0]->shape();
				dynamicBaseModel.V = newShape.V;
				dynamicBaseModel.N = newShape.N;

				SrModel hairModel;
				std::string hairFileName = blendshapesDir + "/" + hairMeshName;
				hairModel.import_ply(hairFileName.c_str());

				SrSnModel* hairSrSn = new SrSnModel();
				std::string hairName = "HairMesh";
				hairSrSn->shape(hairModel);
				hairSrSn->shape().name = hairName.c_str();
				mesh->dMeshStatic_p.push_back(hairSrSn);

				SkinWeight* hairSkin = new SkinWeight();
				SkinWeight* headSkin = mesh->skinWeights[0];
				SrMat hairBindShape = headSkin->bindShapeMat;
				SrMat hairBindPose;
				std::string headJointName = "Head";
				SmartBody::util::log("headSkin bindShape Mat = %s", hairBindShape.toString().c_str());
				for (unsigned int i = 0; i < headSkin->infJointName.size(); i++)
				{
					if (headSkin->infJointName[i] == headJointName)
					{
						//SmartBody::util::log("headSkin inf joint %d : '%s'", i, headSkin->infJointName[i].c_str());
						hairBindPose = headSkin->bindPoseMat[i];
						//SmartBody::util::log("headSkin bindPose Mat %d = %s", i, bindPose.toString().c_str());
					}
				}

				hairSkin->bindShapeMat = hairBindShape;
				hairSkin->bindPoseMat.push_back(hairBindPose);
				hairSkin->infJointName.push_back(headJointName);
				hairSkin->sourceMesh = hairName;
				hairSkin->bindWeight.push_back(1.0f);
				for (unsigned int i = 0; i < hairModel.V.size(); i++)
				{
					hairSkin->jointNameIndex.push_back(0);
					hairSkin->numInfJoints.push_back(1);
					hairSkin->weightIndex.push_back(0);
				}
				mesh->skinWeights.push_back(hairSkin);
			}
		}
	}

	// handle hair mesh
	std::string outputMeshFile = blendshapesDir + "/" + outMeshName;
	mesh->saveToDmb(outputMeshFile);
	// load base model
	//mesh->rebuildVertexBuffer(true);
	// delete the hair mesh and skin weights
	{
		SkinWeight* hairSkin = mesh->skinWeights.back();
		SrSnModel*  hairModel = mesh->dMeshStatic_p.back();
		delete hairSkin;
		delete hairModel;
		mesh->skinWeights.pop_back();
		mesh->dMeshStatic_p.pop_back();
	}
}

void SBDrawFrame(int width, int height, SrMat eyeViewMat)
{
#if USE_GL_FIXED_PIPELINE
	static bool initWes = false;
	if (!initWes)
	{
		wes_init(NULL);
		initWes = true;
	}

	
	//SmartBody::util::log("Getting Scene");
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if(!scene)
		return;
	//SmartBody::util::log("Got Scene");

#if 0
	SmartBody::SBAttribute* attribute = scene->getAttribute("appStatus");
	if (!attribute)
		return;

	SmartBody::StringAttribute* strattribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
	if (!strattribute)
		return;

	if (strattribute->getValue() != "finished")
		return;

	if (scene->getSimulationManager()->getTime() < .033)
		return;
#endif

    SrCamera& cam = *scene->getActiveCamera();
    
	// clear background
	glViewport( 0, 0, width, height);
	glClearColor(0.4f,0.4f,0.4f,1);
	myGLEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//SmartBody::util::log("Preparing to DrawCharacter");
    // setup view
    cam.setAspectRatio(float(width) / float(height));
	SrMat mat;
	glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
	cam.get_perspective_mat(mat);
	glLoadMatrixf ( (float*) mat);
	glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity();
	cam.get_view_mat(mat);
	mat = mat*eyeViewMat;
	glLoadMatrixf ( (float*) mat);
	glScalef ( cam.getScale(), cam.getScale(), cam.getScale());

	//SmartBody::util::log("Set camera");
#if 1
    // draw lights
    setupLights();
    
    // update texture
    myGLEnable(GL_TEXTURE_2D);
    SbmTextureManager& texm = SbmTextureManager::singleton();
    texm.updateTexture();
    
    /*
    // draw a ground plane
	myGLDisable(GL_LIGHTING);
	float planeSize  = 300.f;
	SrVec quad[4] = { SrVec(planeSize, 0.f, planeSize), SrVec(-planeSize, 0.f, planeSize), SrVec(-planeSize,0.f,-planeSize), SrVec(planeSize, 0.f, -planeSize) };
	SrVec quadN[4] = { SrVec(0.f, 1.f, 0.f), SrVec(0.f, 1.f, 0.f), SrVec(0.f, 1.f, 0.f), SrVec(0.f, 1.f, 0.f) };
	GLfloat quadColor[16] = { 0.2f,0.2f, 0.2f, 1.f , 0.3f,0.3f,0.3f, 1.f, 0.5f,0.5f,0.5f,1.f, 0.25f,0.25f,0.25f,1.f };
	unsigned short indices[] = {0,1,2, 0,2,3};
	glShadeModel(GL_SMOOTH);
	myGLDisable(GL_CULL_FACE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
     glVertexPointer(3, GL_FLOAT, 0, quad[0].data());
	glColorPointer(4, GL_FLOAT, 0, quadColor);
     glNormalPointer(GL_FLOAT, 0, quadN[0].data());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	myGLEnable(GL_LIGHTING);
     */
    
    // draw characters
    //SmartBody::util::log("Drawing Character");
	SBDrawBackground();
    SBDrawCharacters();
#endif
#endif
}
    

void SBDrawBackground(ESContext* esContext)
{
#if 0
	SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE,"background_img");
	if (!tex)
	{
		//SmartBody::util::log("cannot find texture image .....");
		return; // no background image
	}

	//SmartBody::util::log("texture image id = %d", tex->getID());
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity ();

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);

	
	myGLEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, tex->getID());	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texIDs[0]);
	//SmartBody::util::log("before bindAttrib");
	float z_max = -(1.f - gwiz::epsilon10());
	SrVec4 quad[4] = { SrVec4(-1.0, 1.0f, z_max, 1.f), SrVec4(-1.0f, -1.0f, z_max, 1.f), SrVec4(1.0f, -1.0f, z_max, 1.f), SrVec4(1.0f, 1.0f, z_max, 1.f) };
	SrVec4 quadT[4] = { SrVec4(0.f, 1.f, 0.f, 0.f), SrVec4(0.f, 0.f, 0.f, 0.f), SrVec4(1.f, 0.f, 0.f, 0.f), SrVec4(1.f, 1.f, 0.f, 0.f) };
	unsigned short indices[] = {0,1,2, 0,2,3};
	
	glEnableClientState(GL_VERTEX_ARRAY);
    // todo: this smells. it relies on the memory model that represents SrVec4
    // as a block.
    glVertexPointer(4, GL_FLOAT, 0, quad[0].data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);  	
    glTexCoordPointer(4, GL_FLOAT, 0, quadT[0].data());

	glDrawElements_wes(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	myGLDisable(GL_TEXTURE_2D);	

	glMatrixMode (GL_PROJECTION);
	glPopMatrix(); 

	glMatrixMode (GL_MODELVIEW);
	glPopMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#else
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string backgroundTexName = scene->getStringAttribute("background_img");
	drawBackground(backgroundTexName, esContext);
	//drawBackground("background_img", esContext);
#endif
}


void SBDrawCharacters()
{

#define DRAW_DEBUG_BONE 0
#if DRAW_DEBUG_BONE   // draw bone figure
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
			//SmartBody::util::log("Joint %d, Position = %f %f %f", i, pos.x,pos.y,pos.z);
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
        //glPointSize(2.0f);
        //glLineWidth(1.0f);
        glColor4f(1, 1, 1, 1);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, jointPos);
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
			//pawn->dMeshInstance_p->blendShapeStaticMesh();
			SrGlRenderFuncs::renderDeformableMesh(pawn->dMeshInstance_p);
		}
	}
#endif
}
    
void SBUpdate(float t)
{
	//SmartBody::util::log("SBUpdate, t = %f", t);
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    SmartBody::SBSimulationManager* sim = scene->getSimulationManager();
    if((!scene)  || (!sim))
    	return;
	if (SmartBody::SBScene::getScene()->getVHMsgManager()->isEnable())
	{
		int err = SmartBody::SBScene::getScene()->getVHMsgManager()->poll();
		if( err == CMD_FAILURE )   {
			//SmartBody::util::log("ttu_poll ERROR\n" );
		}
	}
    //sim->update();
    bool update_sim = scene->getSimulationManager()->updateTimer();
    if (update_sim)
    {
    	scene->update();

    	// update mesh position
    	#if 1
		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		//printf("draw pawns::numOfPawns: %d\n", pawns.size());
		for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			 pawnIter != pawns.end();
			 pawnIter++)
		{
			SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
			//SmartBody::util::log("draw pawn %s", pawn->getName().c_str());
			//DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
			DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
			if(meshInstance)
			{
				meshInstance->blendShapeStaticMesh();
				if (!meshInstance->isStaticMesh())
				{
					meshInstance->updateTransformBuffer();
				}
			}
		}
		#endif
    }


    
}
    
void SBExecuteCmd(const char* command)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    scene->command(command);
    //SmartBody::util::log("%s\n", command);
}
    
void SBExecutePythonCmd(const char* command)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    scene->run(command);
    //SmartBody::util::log("%s\n", command);
}


void sb_vhmsg_callback( const char *op, const char *args, void * user_data ) {
	// Replace singleton with a user_data pointer
	//if (!mcuInit) return;
	//SmartBody::util::log("VHMSG Callback : op = %s ,args = %s\n",op,args);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	switch( scene->getCommandManager()->execute( op, (char *)args ) ) {
	case CMD_NOT_FOUND:
		SmartBody::util::log("SBM ERR: command NOT FOUND: '%s' + '%s'", op, args );
		break;
	case CMD_FAILURE:
		SmartBody::util::log("SBM ERR: command FAILED: '%s' + '%s'", op, args );
		break;
	}
}


void SBInitVHMSGConnection()
{
	//if (!mcuInit) return;	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const char* serverName = "172.16.33.14";
	const char* scope = "DEFAULT_SCOPE";
	const char* port = "61616";
	int err;
	int openConnection = vhmsg::ttu_open(serverName,scope,port);
	if( openConnection == vhmsg::TTU_SUCCESS )
	{
		vhmsg::ttu_set_client_callback( sb_vhmsg_callback );
		err = vhmsg::ttu_register( "sb" );
		err = vhmsg::ttu_register( "sbm" );
		err = vhmsg::ttu_register( "vrAgentBML" );
		err = vhmsg::ttu_register( "vrExpress" );
		err = vhmsg::ttu_register( "vrSpeak" );
		err = vhmsg::ttu_register( "RemoteSpeechReply" );
		err = vhmsg::ttu_register( "PlaySound" );
		err = vhmsg::ttu_register( "StopSound" );
		err = vhmsg::ttu_register( "CommAPI" );
		err = vhmsg::ttu_register( "object-data" );
		err = vhmsg::ttu_register( "vrAllCall" );
		err = vhmsg::ttu_register( "vrKillComponent" );
		err = vhmsg::ttu_register( "wsp" );
		err = vhmsg::ttu_register( "receiver" );
		scene->getVHMsgManager()->setEnable(true);
		//SmartBody::util::log("TTU Open Success : server = %s, scope = %s, port = %s",serverName,scope,port);
	}
	else
	{
		//SmartBody::util::log("TTU Open Failed : server = %s, scope = %s, port = %s",serverName,scope,port);
	}
}
    
    
void SBCameraOperation(float dx, float dy)
{
    SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrCamera* cam = scene->getActiveCamera();
    
    float camDx = dx * cam->getAspectRatio() ;
    float camDy = dy * cam->getAspectRatio() ;
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
        
        SrVec cameraPoint = rotatePoint(origCamera, origCenter, dirX, camDx * float(M_PI));
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

void SBCloseVHMSGConnection()
{
	vhmsg::ttu_close();
}

bool SBPythonCommandVoid( const char * command)
{
#ifndef SB_NO_PYTHON
   return SmartBody::SBScene::getScene()->run(command);
#else
   return false;
#endif
}

bool SBPythonCommandBool( const char * command )
{
#ifndef SB_NO_PYTHON
   try
   {
      boost::python::object mainDict = SmartBody::SBScene::getScene()->getPythonMainDict();
      boost::python::object obj = boost::python::exec(command, mainDict);
      bool result = boost::python::extract<bool>(mainDict["ret"]);
      return result;
   }
   catch (...)
   {
      PyErr_Print();
      return false;
   }
#else
   return false;
#endif
}

int SBPythonCommandInt( const char * command )
{
#ifndef SB_NO_PYTHON
   try
   {
      boost::python::object mainDict = SmartBody::SBScene::getScene()->getPythonMainDict();
      boost::python::object obj = boost::python::exec(command, mainDict);
      int result = boost::python::extract<int>(mainDict["ret"]);
      return result;
   }
   catch (...)
   {
      PyErr_Print();
      return 0;
   }
#else
   return 0;
#endif
}

float SBPythonCommandFloat( const char * command )
{
#ifndef SB_NO_PYTHON
   try
   {
      boost::python::object mainDict = SmartBody::SBScene::getScene()->getPythonMainDict();
      boost::python::object obj = boost::python::exec(command, mainDict);
      float result = boost::python::extract<float>(mainDict["ret"]);
      return result;
   }
   catch (...)
   {
      PyErr_Print();
      return 0;
   }
#else
   return 0;
#endif
}

void SBPythonCommandString( const char * command, char * output, int maxLen)
{
#ifndef SB_NO_PYTHON
   try
   {
      boost::python::object mainDict = SmartBody::SBScene::getScene()->getPythonMainDict();
      boost::python::object obj = boost::python::exec(command, mainDict);
      std::string result = boost::python::extract<std::string>(mainDict["ret"]);
      strncpy(output, result.c_str(), maxLen);
   }
   catch (...)
   {
      PyErr_Print();
   }
#else
   return;
#endif
}


#endif
