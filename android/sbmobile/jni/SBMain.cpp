/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <vhcl.h>
#include <sb/SBScene.h>
#include <sb/SBPawn.h>
#include "SBUtilities.h"
#include <sr/sr_camera.h>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sbm/GPU/SbmTexture.h>

#define TEST_ALIB 0

#if TEST_ALIB
#include <jerome/npc/npc.hpp>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <sys/time.h>
#include "SBWrapper.h"
#include "SBMobile.h"
#include "ImageTransfer.h"
#include "esUtil.h"

#if TEST_ALIB
class Model
{
public:
	Model() {}
	~Model() {}
	//alib::npc::Collection collection;
	//std::map<std::string, alib::npc::detail::Ranker> rankerMap;
	jerome::npc::Platform p;
};
Model* _model = NULL;
#endif 

vhcl::Log::AndroidListener androidListener;
unsigned long prevTime = 0;
int curW, curH;
bool sbInit = false;
static unsigned long getCurrentTime(void)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec*1000000 + now.tv_nsec/1000;
}



extern "C" {
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_init(JNIEnv * env, jobject obj);	
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_setup(JNIEnv * env, jobject obj, jstring mediaPath);
	
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_setBackground(JNIEnv * env, jobject obj, jstring fileName, jstring textureName, jint texRotate);

	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_surfaceChanged(JNIEnv * env, jobject obj,  jint width, jint height);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_step(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_render(JNIEnv * env, jobject obj, jfloatArray modelview);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_renderAR(JNIEnv * env, jobject obj, jfloatArray modelview, jfloatArray proj, jint updateGaze);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_renderFBOTex(JNIEnv * env, jobject obj, jint width, jint height, jint texID);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_renderCardboard(JNIEnv * env, jobject obj, jfloatArray arr);	
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_reloadTexture(JNIEnv * env, jobject obj);	
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_createCustomMeshFromBlendshapes(JNIEnv * env, jobject obj, jstring templateMeshName, jstring blendshapesDir, jstring baseMeshName, jstring hairMeshName, jstring outMeshName);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_imageColorTransfer(JNIEnv * env, jobject obj, jstring srcImg, jstring srcMask, jstring tgtImg, jstring tgtMask, jstring outImg);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_deformableMeshTextureReplace(JNIEnv * env, jobject obj, jstring mesnName, jstring texName, jstring imgFileName);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_replaceSubMesh(JNIEnv * env, jobject obj, jstring mesnName, jstring subMeshName, jstring inputMeshName);


	JNIEXPORT jboolean JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_handleInputEvent(JNIEnv* env, jobject thiz, jint action, jfloat mx, jfloat my);

	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_initClassifier(JNIEnv * env, jobject obj, jstring classifierName, jstring dialogManagerName);
	JNIEXPORT jstring JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_classify(JNIEnv * env, jobject obj, jstring characterName, jstring question);
	JNIEXPORT jobjectArray  JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_classifyWithExternalID(JNIEnv * env, jobject obj, jstring characterName, jstring question);
	

	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_openConnection(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_closeConnection(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_executeSB(JNIEnv * env, jobject obj, jstring sbmCmd);
	JNIEXPORT jstring JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getLOG(JNIEnv * env, jobject obj);
    JNIEXPORT jstring JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getStringAttribute(JNIEnv * env, jobject obj, jstring attrName);
	JNIEXPORT jboolean JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getBoolAttribute(JNIEnv * env, jobject obj, jstring attrName);
	JNIEXPORT jdouble JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getDoubleAttribute(JNIEnv * env, jobject obj, jstring attrName);  
	JNIEXPORT jint JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getIntAttribute(JNIEnv * env, jobject obj, jstring attrName); 
};


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_initClassifier(JNIEnv * env, jobject obj, jstring classifierName, jstring dialogManagerName)
{
#if TEST_ALIB
	//std::stringstream strstr2;
	//strstr2 << "/sdcard/vhdata/" << filename;
	const char* fileNameStr = (env)->GetStringUTFChars(classifierName, NULL);
	std::ifstream file(fileNameStr);
	if (file.good())
		SmartBody::util::log("File stream '%s' is good.", fileNameStr);
	else
		SmartBody::util::log("File stream '%s' is not good.", fileNameStr);

	if (_model)
		delete _model;
	_model = new Model();
	auto error = _model->p.loadCollection(file);

	if (error)
		SmartBody::util::log("load collection error is %s", (*error).what());

	jerome::npc::Collection& collection = _model->p.collection();
	jerome::npc::Collection::states_type& states = collection.states();

	jerome::npc::Collection::states_type::iterator vi;
	for (vi = states.begin();
		vi != states.end();
		vi++)
	{
		jerome::String stateName = vi->name();
		SmartBody::util::log("load collection, state name = '%s'", stateName.c_str());
	}

	std::string dialogNameStr = (const char*)((env)->GetStringUTFChars(dialogManagerName, NULL));
	jerome::String mainStateMachineName;
	std::ifstream dialogFile(dialogNameStr);
	_model->p.loadDialogueManager(dialogFile, [&](const jerome::Result<jerome::String>& name) mutable { 
		if (name) { 
			mainStateMachineName = name.value(); 
		} 
		else { 
			std::cerr << name.error().what() << std::endl; 
		}});
	//LOG("Start Training Classifier");
	//auto trainError = _model->p.train("Anyone - The Twins");

	SmartBody::util::log("Classifier Init Done");
#endif
}

JNIEXPORT jstring JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_classify(JNIEnv * env, jobject obj, jstring characterName, jstring question)
{
#if TEST_ALIB
	SmartBody::util::log("Start Classify");
	std::string charNameStr = (env)->GetStringUTFChars(characterName, NULL);
	std::string questionStr = (env)->GetStringUTFChars(question, NULL);
	//std::string charNameStr = "SaxonPerson - SaxonPerson";
	//std::string questionStr = "what is your name";
	std::string answer = "None";
	SmartBody::util::log("Before get response");
	boost::optional<jerome::npc::Utterance> response = _model->p.respond(charNameStr, questionStr);
	SmartBody::util::log("After get response");
	if (response)
	{
		answer = (*response).get("text");
	}
	SmartBody::util::log("After get answer");
	return env->NewStringUTF(answer.c_str());
#endif
}

JNIEXPORT jobjectArray  JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_classifyWithExternalID(JNIEnv * env, jobject obj, jstring characterName, jstring question)
{
#if TEST_ALIB
	std::vector<std::string> ret;
	std::string answer = "None";
	std::string id = "None";
	std::string type = "None";
	const char* charNameStr = (env)->GetStringUTFChars(characterName, NULL);
	const char* questionStr = (env)->GetStringUTFChars(question, NULL);

	boost::optional<jerome::npc::Utterance> response = _model->p.respond(charNameStr, questionStr);
	if (response)
	{
		answer = (*response).get("text");
		//LOG("classifyWithExternalID, answer = %s", answer.c_str());
		id = (*response).get("id");
		//LOG("classifyWithExternalID, externalID = %s", id.c_str());

		if ((*response).has("type"))
			type = (*response).get("type");

	}

	jobjectArray result;
	jstring str;
	jsize len = 2;
	result = (env)->NewObjectArray(len, (env)->FindClass("java/lang/String"), 0);
	str = (env)->NewStringUTF(answer.c_str());
	(env)->SetObjectArrayElement(result, 0, str);
	str = (env)->NewStringUTF(id.c_str());
	(env)->SetObjectArrayElement(result, 1, str);

	return result;
#endif
}




JNIEXPORT jboolean JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_handleInputEvent(JNIEnv* env, jobject thiz, jint action, jfloat mx, jfloat my)
{
	SBMobile* engine = SBMobile::getSBMobile();
	if (!engine) 
	{
		//engine = new SBMobile();
		//SBMobile::setSBMobile(engine);
		return false;
	}
	else
	{
		engine->resize(curW, curH);
		return engine->eventScreenTouch(action,mx,my);
	}	
}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_setBackground(JNIEnv * env, jobject obj, jstring fileName, jstring textureName, jint texRotate)
{
	const char* fileNameStr = (env)->GetStringUTFChars(fileName, NULL);
	const char* textureNameStr = (env)->GetStringUTFChars(textureName, NULL);
	SBSetBackground(fileNameStr, textureNameStr, texRotate);
}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_createCustomMeshFromBlendshapes(JNIEnv * env, jobject obj, jstring templateMeshName, jstring blendshapesDir, jstring baseMeshName, jstring hairMeshName, jstring outMeshName)
{
	const char* templateMeshStr = (env)->GetStringUTFChars(templateMeshName, NULL);
	const char* blendshapesDirStr = (env)->GetStringUTFChars(blendshapesDir, NULL);
	const char* baseMeshNameStr = (env)->GetStringUTFChars(baseMeshName, NULL);
	const char* hairMeshNameStr = (env)->GetStringUTFChars(hairMeshName, NULL);
	const char* outMeshNameStr = (env)->GetStringUTFChars(outMeshName, NULL);
	SBCreateCustomMeshFromBlendshapes(templateMeshStr, blendshapesDirStr, baseMeshNameStr, hairMeshNameStr, outMeshNameStr);
}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_imageColorTransfer(JNIEnv * env, jobject obj, jstring srcImg, jstring srcMask, jstring tgtImg, jstring tgtMask, jstring outImg)
{
	const char* srcImgStr = (env)->GetStringUTFChars(srcImg, NULL);
	const char* srcMaskStr = (env)->GetStringUTFChars(srcMask, NULL);
	const char* tgtImgStr = (env)->GetStringUTFChars(tgtImg, NULL);
	const char* tgtMaskStr = (env)->GetStringUTFChars(tgtMask, NULL);
	const char* outImgStr = (env)->GetStringUTFChars(outImg, NULL);

	imageColorTransfer(srcImgStr, srcMaskStr, tgtImgStr, tgtMaskStr, outImgStr);
}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_deformableMeshTextureReplace(JNIEnv * env, jobject obj, jstring mesnName, jstring texName, jstring imgFileName)
{
	const char* meshNameStr = (env)->GetStringUTFChars(mesnName, NULL);
	const char* texNameStr = (env)->GetStringUTFChars(texName, NULL);
	const char* imgFileNameStr = (env)->GetStringUTFChars(imgFileName, NULL);
	deformableMeshTextureReplace(meshNameStr, texNameStr, imgFileNameStr);
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_replaceSubMesh(JNIEnv * env, jobject obj, jstring mesnName, jstring subMeshName, jstring inputMeshName)
{
	const char* meshNameStr = (env)->GetStringUTFChars(mesnName, NULL);
	const char* subMeshStr = (env)->GetStringUTFChars(subMeshName, NULL);
	const char* inputMeshStr = (env)->GetStringUTFChars(inputMeshName, NULL);
	replaceSubMesh(meshNameStr, subMeshStr, inputMeshStr);
}






JNIEXPORT jstring JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getStringAttribute(JNIEnv * env, jobject obj, jstring attrName)
{
	const char* attrNameStr = (env)->GetStringUTFChars( attrName , NULL ) ;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string strVal = scene->getStringAttribute(attrNameStr);
    return env->NewStringUTF(strVal.c_str());
}

JNIEXPORT jboolean JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getBoolAttribute(JNIEnv * env, jobject obj, jstring attrName)
{
	const char* attrNameStr = (env)->GetStringUTFChars( attrName , NULL ) ;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    bool val = scene->getBoolAttribute(attrNameStr);
    return (jboolean) val;
}

JNIEXPORT jdouble JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getDoubleAttribute(JNIEnv * env, jobject obj, jstring attrName)
{
	const char* attrNameStr = (env)->GetStringUTFChars( attrName , NULL ) ;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    double val = scene->getDoubleAttribute(attrNameStr);
    return (jdouble) val;
}

JNIEXPORT jint JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getIntAttribute(JNIEnv * env, jobject obj, jstring attrName)
{
	const char* attrNameStr = (env)->GetStringUTFChars( attrName , NULL ) ;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
    int val = scene->getIntAttribute(attrNameStr);
    return (jint) val;
}




JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_reloadTexture(JNIEnv * env, jobject obj)
{
	//SmartBody::util::log("Reload OpenGL Texture");
	SBInitGraphics(&esContext);
	SbmTextureManager& texm = SbmTextureManager::singleton();
	texm.reloadTexture();

}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_surfaceChanged(JNIEnv * env, jobject obj,  jint width, jint height)
{	
	//SmartBody::util::log("onSurfaceChanged, width = %d, height = %d", width, height);
	esContext.width = width;
	esContext.height = height;
	SBSetupDrawing(width,height, &esContext);
	curW = width;
	curH = height;
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_setup(JNIEnv * env, jobject obj, jstring mediaPath )
{
	if (sbInit) // don't need to setup twice
		return;
	const char* mediaPathStr = (env)->GetStringUTFChars( mediaPath , NULL ) ;
	//SBSetup("/sdcard/vhdata/","setup.py");
	SBSetup(mediaPathStr,"setup.py");
	if (!SBMobile::jvm)
	{
		int hasJVM = env->GetJavaVM(&SBMobile::jvm);
	}
	//esInitContext(&esContext);	
	SBInitialize(); // initialize smartbody with media path
	initSBMobilePythonModule(); // initialize Python APIs for SBMobile to enable video/audio playback through JNI

#if TEST_ALIB
	std::string dataFolder = "classifier";
	std::stringstream strstr;
	strstr << std::string(mediaPathStr) << "/" << dataFolder << "/kstem-lexicon-beta";
	SmartBody::util::log("kstem folder = %s",strstr.str().c_str());
	jerome::npc::Platform::setLexiconDirectory(strstr.str());
	SmartBody::util::log("after setLexicon directory");
	jerome::npc::Platform::initialize();
	SmartBody::util::log("after init alib::npc::Platform");
#endif

	sbInit = true;
}



JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_init(JNIEnv * env, jobject obj)
{		
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();	
	scene->addAssetPath("script", "scripts");
	SBInitScene("init.py");		
	vhcl::Log::g_log.AddListener(&androidListener);		
	//SmartBody::util::g_log.AddListener(&androidListener);		
	SmartBody::util::log("Initializing Done");
}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_render(JNIEnv * env, jobject obj, jfloatArray modelview)
{	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	//SmartBody::util::log("render, scene = %d, sbInit = %d", scene, sbInit);
	if(!scene)
		return;
	if (!sbInit)
	{
		/*
		SmartBody::util::log("No sbInit yet, draw test background curW = %d, curH = %d", curW, curH);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.33f, 0.78f, 0.95f, 1.f);
		glViewport( 0, 0, curW, curH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*/
		return;
	}
	SrMat id;	
	SrMat modelViewMat;
	jfloat *bodyf = env->GetFloatArrayElements(modelview, 0);
	for (int i = 0; i < 16; i++)
		modelViewMat[i] = bodyf[i];

	SBMobile* engine = SBMobile::getSBMobile();
	if (engine)
	{
		SrMat viewMat, projMat;
		SrCamera* cam = SmartBody::SBScene::getScene()->getActiveCamera();
		cam->get_view_mat(viewMat);
		cam->get_perspective_mat(projMat);
		viewMat = modelViewMat*viewMat;
		engine->setTransformMatrces(viewMat, projMat);
	}
	//SBDrawFrame(VHEngine::curW, VHEngine::curH, id);
	SBDrawFrame_ES20(curW, curH, &esContext, modelViewMat, false);
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_renderAR(JNIEnv * env, jobject obj, jfloatArray modelview, jfloatArray proj, jint updateGaze)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if (!scene)
		return;
	if (!sbInit)
	{
		/*
		SmartBody::util::log("No sbInit yet, draw test background curW = %d, curH = %d", curW, curH);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.33f, 0.78f, 0.95f, 1.f);
		glViewport( 0, 0, curW, curH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*/
		return;
	}
	SrMat id;
	SrMat modelViewMat, projMat;
	jfloat *bodyf = env->GetFloatArrayElements(modelview, 0);
	for (int i = 0; i < 16; i++)
		modelViewMat[i] = bodyf[i];
	jfloat *projf = env->GetFloatArrayElements(proj, 0);
	for (int i = 0; i < 16; i++)
		projMat[i] = projf[i];

	

	//SmartBody::util::log("render AR, scene = %d, sbInit = %d", scene, sbInit);
	SBDrawFrameAR(curW, curH, &esContext, modelViewMat, projMat);

	
	SmartBody::SBPawn* gazeTarget = scene->getPawn("None");
	


	

	SBMobile* engine = SBMobile::getSBMobile();
	if (engine)
	{
		engine->setTransformMatrces(modelViewMat, projMat);
	}
	
	if (!gazeTarget) return;

	if (updateGaze == 1)
	{
		SrMat invModelView = modelViewMat.inverse();
		SrVec newGazePos = invModelView.get_translation();
		//SmartBody::util::log("gaze user position = %s", newGazePos.toString().c_str());
		gazeTarget->setPosition(newGazePos); // update to new gaze
	}
	else
	{
		SrVec newGazePos = scene->getVec3Attribute("defaultGazePos");
		gazeTarget->setPosition(newGazePos);
	}

	//SBDrawFrame_ES20(curW, curH, &esContext, id, true);
	//SBDrawFrame(VHEngine::curW, VHEngine::curH, id);
	//SBDrawFrame_ES20(curW, curH, &esContext, id, true);
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_renderFBOTex(JNIEnv * env, jobject obj, jint width, jint height, jint texID)
{	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if(!scene)
		return;
	if (!sbInit)
		return;
	SrMat id;
	GLuint fboTexID = texID;
	//SmartBody::util::log("Runninmg FBOTex Rendering");
	SBDrawFBOTex_ES20(width, height, &esContext, id, fboTexID);
	//SmartBody::util::log("Done with FBOTex Rendering");
	//SBDrawFrame(VHEngine::curW, VHEngine::curH, id);
	//SBDrawFrame_ES20(curW, curH, &esContext, id);
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_renderCardboard(JNIEnv * env, jobject obj, jfloatArray arr)
{	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if(!scene)
		return;
	if (!sbInit)
		return;
	SrMat eyeViewMat;
	jfloat *body = env->GetFloatArrayElements(arr, 0);
	for (int i=0;i<16;i++)
		eyeViewMat[i] = body[i];
	//eyeViewMat.transpose();
	//SBDrawFrame(VHEngine::curW, VHEngine::curH, eyeViewMat);
	SBDrawFrame_ES20(curW, curH, &esContext, eyeViewMat);
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_step(JNIEnv * env, jobject obj)
{	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if(!scene)
		return;
	if (!sbInit)
		return;
	unsigned long curTime = getCurrentTime();	
	
	#if 1
	if (!scene->getBoolAttribute("ready"))
	{
		prevTime = curTime;
	    return;
	}
	
	static long frameDeltaTime = 1000000/30; // 30 frames per second
	static int firstCount = 0;	
	//if (curTime - prevTime > frameDeltaTime)
	//if (firstCount < 10)
	{
		float dt = (float)(curTime-prevTime)/1000000.f;			
		SBUpdate(dt);		
		firstCount++;		
	}
	#endif
}


JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_executeSB(JNIEnv * env, jobject obj, jstring sbmCmd)
{
	if (!sbInit)
		return;
	const char* pyCmdStrConst = (env)->GetStringUTFChars( sbmCmd , NULL ) ;
	//SmartBody::util::log("python cmd = %s",pyCmdStrConst);
	SBExecutePythonCmd(pyCmdStrConst);
}

JNIEXPORT jstring JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_getLOG( JNIEnv * env, jobject obj )
{	
	if (!sbInit)
		return env->NewStringUTF("");;
	std::string logStr = "No Text";
	logStr = androidListener.getLogs();	
	return env->NewStringUTF(logStr.c_str());
}



JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_openConnection( JNIEnv * env, jobject obj )
{
	return SBInitVHMSGConnection();
}

JNIEXPORT void JNICALL Java_edu_usc_ict_sbmobile_SBMobileLib_closeConnection( JNIEnv * env, jobject obj )
{	
	SBCloseVHMSGConnection();
}
