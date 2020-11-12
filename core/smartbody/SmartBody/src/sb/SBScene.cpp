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



#include "SBScene.h"
#ifdef WIN32
#include <direct.h>
#endif

#include <sb/SBTypes.h>

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
#include "external/glew/glew.h"
#include "sbm/GPU/SbmDeformableMeshGPU.h"
#endif

#ifndef SB_NO_JAVASCRIPT
#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif
#endif

#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <sb/SBObject.h>
#include <sb/SBCharacter.h>
#include <sb/SBMotion.h>
#include <sb/SBScript.h>
#include <sb/SBEvent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBReach.h>
#include <sb/SBReachManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBSteerManager.h>
#include <sb/SBRealtimeManager.h>
#include <sb/SBServiceManager.h>
#include <sb/SBService.h>
#include <sb/SBPhysicsManager.h>

#include <sb/SBBoneBusManager.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBSpeechManager.h>
#include <sb/SBCommandManager.h>
#include <sb/SBNavigationMeshManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBSkeleton.h>
#include <sb/SBParser.h>
#include <sb/SBRetarget.h>
#include <sb/SBDebuggerServer.h>
#include <sb/SBDebuggerClient.h>
#include <sb/SBDebuggerUtility.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBMotionGraph.h>
#include <sb/SBUtilities.h>
#include <sbm/sbm_audio.h>
#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <sb/nvbg.h>
#include <sb/SBJointMap.h>
#include <sb/SBSceneListener.h>
#include <sb/SBNavigationMesh.h>
#include <sbm/ParserBVH.h>
#include <sbm/ParserCOLLADAFast.h>
#include <sbm/ParserOpenCOLLADA.h>
#include <sbm/ParserOgre.h>
#include <sbm/Heightfield.h>
#include <sbm/action_unit.hpp>
#include <sbm/xercesc_utils.hpp>
#include <sbm/GPU/SbmTexture.h>
#include <sr/sr_camera.h>
#include <controllers/me_ct_gaze.h>
#include <controllers/me_ct_eyelid.h>
#include <controllers/me_ct_breathing.h>
#include <controllers/me_ct_example_body_reach.hpp>
#include <controllers/me_ct_saccade.h>
#include <controllers/me_ct_new_locomotion.h>
#include <controllers/me_ct_generic_hand.h>
#include <controllers/RealTimeLipSyncController.h>
#include <sbm/KinectProcessor.h>
#include <controllers/me_controller_tree_root.hpp>
#include <bml/bml_processor.hpp>
#include <sr/sr_sn_group.h>

#include <sbm/GPU/SbmShader.h>
#include <sbm/GPU/SbmTexture.h>
#include <sr/sr_gl.h>
#include <sbm/KinectProcessor.h>
#include <sr/sr_sn_group.h>
#include <fstream>
#include <sstream>
#include <algorithm>





// for minizip compression

#if !defined(__FLASHPLAYER__) && !defined(EMSCRIPTEN)
#include <external/zlib-1.2.5/zip.h>
#endif


#ifndef WIN32
#define _stricmp strcasecmp
#endif

#define SHOW_DEPRECATION_MESSAGES 0
namespace SmartBody {

SBScene* SBScene::_scene = NULL;
bool SBScene::_firstTime = true;

const std::string defautAssetPath = "Assets";

std::map<std::string, std::string> SBScene::_systemParameters;


class ForwardLogListener : public SmartBody::util::Listener
{
    public:
		ForwardLogListener() {}
		virtual ~ForwardLogListener() {}

        virtual void OnMessage( const std::string & message )
		{
			SBScene* scene = SmartBody::SBScene::getScene();
			if (!scene)
				return;
			std::vector<SBSceneListener*>& listeners = scene->getSceneListeners();
			for (size_t i = 0; i < listeners.size(); i++)
			{
				listeners[i]->OnLogMessage(message);
			}
		}
};


SBScene::SBScene(void) : SBObject()
{
	//initialize();
}

void SBScene::initialize()
{
#ifndef SB_NO_PYTHON
#ifndef __native_client__
//	_mainModule = NULL;
//	_mainDict = NULL;
#endif
#endif
	_processId = "";
	_lastScriptDirectory = "";

	createDefaultControllers();

	_sceneListeners.clear();

	_sim = new SBSimulationManager();
	_profiler = new SBProfiler();
	_bml = new SBBmlProcessor();
	_blendManager = new SBAnimationBlendManager();
	_reachManager = new SBReachManager();
	_steerManager = new SBSteerManager();
	_realtimeManager = new SBRealtimeManager();
	_serviceManager = new SBServiceManager();
	_physicsManager = new SBPhysicsManager();
	_gestureMapManager = new SBGestureMapManager();
	_jointMapManager = new SBJointMapManager();
	_boneBusManager = new SBBoneBusManager();
	_collisionManager = new SBCollisionManager();
	_phonemeManager = new SBPhonemeManager();
	_behaviorSetManager = new SBBehaviorSetManager();
	_retargetManager = new SBRetargetManager();
	_eventManager = new SBEventManager();
	_assetManager = new SBAssetManager();
	_speechManager = new SBSpeechManager();
	_vhmsgManager = new SBVHMsgManager();
	_commandManager = new SBCommandManager();
	_naviMeshManager = new SBNavigationMeshManager();
	_motionGraphManager = new SBMotionGraphManager();
	_handConfigManager = new SBHandConfigurationManager();
	_debuggerServer = new SBDebuggerServer();

	//_scale = .01f; // default scale is centimeters
	_scale = 1.f;

	// add the services
	_serviceManager->addService(_steerManager);
	_serviceManager->addService(_physicsManager);
	_serviceManager->addService(_boneBusManager);
	_serviceManager->addService(_collisionManager);
	_serviceManager->addService(_vhmsgManager);
	_serviceManager->addService(_realtimeManager);
	_serviceManager->addService(_phonemeManager);
	_serviceManager->addService(_profiler);
	_serviceManager->addService(_debuggerServer);

	_parser = new SBParser();

	_debuggerClient = new SBDebuggerClient();
	_debuggerUtility = new SBDebuggerUtility();
	_isRemoteMode = false;
   _isCameraLocked = false;

   _coneOfSight			= false;
   _coneOfSight_leftEye	= NULL;

    createBoolAttribute("bmlstatus", false, true, "", 5, false, false, false, "Use BML status feedback events.");
	createBoolAttribute("useNewBMLParsing",false,true,"",10,false,false,false,"Use new BML parsing scheme.");
	createBoolAttribute("internalAudio",false,true,"",10,false,false,false,"Use SmartBody's internal audio player.");
	createStringAttribute("speechRelaySoundCacheDir","../../../..",true,"",20,false,false,false,"Directory where sound files from speech relays will be placed. ");
	createDoubleAttribute("scale",1.f,true,"",30,false,false,false,"The scale of scene (1 = meters, .01 = centimeters, etc).");
	createIntAttribute("colladaTrimFrames",0,true,"",40,false,false,false,"Number of frames to be trimmed in the front when loading a collada motion.");
	createBoolAttribute("useFastCOLLADAParsing",true,true,"",50,false,false,false,"Use faster parsing when reading COLLADA from a file.");
	createBoolAttribute("parseSingleAnimation", false, true, "", 50, false, false, false, "Use faster parsing when reading COLLADA from a file.");
	createBoolAttribute("useFastSpeechParsing",false,true,"",50,false,false,false,"Use faster parsing when reading speech from a file.");
	createBoolAttribute("delaySpeechIfNeeded",true,true,"",60,false,false,false,"Delays any speech until other behaviors specified in the same BML need to execute beforehand. This can occur when a gesture is synchronized to a word early in the utterance, and the gesture motion needs to be played for awhile before the synch point.");
	createBoolAttribute("useXMLCache",false,true,"",500,false,false,false,"Cache the XML used when processing audio files.");
	createBoolAttribute("useXMLCacheAuto",false,true,"",510,false,false,false,"Automatically add the XML to the cache when processing audio files after playing for the first time.");
	createStringAttribute("defaultCharacter","",true,"",550,false,false,false,"Default character when processing BML.");
	createStringAttribute("defaultRecipient","ALL",true,"",550,false,false,false,"Default recipient when processing BML.");
	createIntAttribute("queuedCommandsIndex",1,true,"",560,false,false,false,"Unique identifier when executing sequence commands.");
	createIntAttribute("bmlIndex",1,true,"",560,false,false,false,"Unique identifier when executing BML commands.");
	createBoolAttribute("enableConsoleLogging",true,true,"",70,false,false,false,"Use SmartBody's internal audio player.");
	createBoolAttribute("warnDeprecatedCommands",false,true,"",600,false,false,false,"Shows a warning when a deprecated command is being used.");
	createBoolAttribute("enableFaceShader",false,true,"",70,false,false,false,"Enable the face shader for higher quality rendering.");
	createBoolAttribute("enableExportProcessedBML",false,true,"",80,false,false,false,"Enable exporting of bml after realization.");
	createBoolAttribute("enableExportProcessedBMLLOG",false,true,"",81,false,false,false,"Enable log for exporting of bml after realization.");
	createStringAttribute("processedBMLPath","./processedXML.xml",true,"",90,false,false,false,"path of exported bml file after realization.");

	createBoolAttribute("useDefaultLights", true, true, "", 92, false, false, false, "Add default lighting to scene when no lights objects are present.");
	createIntAttribute("numLightsInScene", 2, true, "", 100, false, false, false, "Size of the shadow map");
	createIntAttribute("shadowMapSize", 1024, true, "RenderingParameters", 100, false, false, false, "Size of the shadow map");
	createIntAttribute("shadowMapCount", 1, true, "RenderingParameters", 110, false, false, false, "Num of the shadow maps");
	createDoubleAttribute("optimalAdjustFactor", 1.0, true, "RenderingParameters", 120, false, false, false, "Num of the shadow maps");
	createDoubleAttribute("shadowFarDistance", 1000, true, "RenderingParameters", 130, false, false, false, "Num of the shadow maps");

	createStringAttribute("floorMaterial", "", true, "RenderingParameters", 200, false, false, false, "Material to be used for the floor.");

	createStringAttribute("randomseed", "", true, "", 400, false, false, false, "Seed for random number generator.");
	createDoubleAttribute("blendshapeProximity", .0001, true, "", 121, false, false, false, "Blendshape vertex distance for mesh optimization.");

#if ANDROID_BUILD
	createBoolAttribute("useGPUBlendshapes",false,true,"",120,false,false,false,"Use the GPU for processing blendshapes.");
#else
	createBoolAttribute("useGPUBlendshapes",false,true,"",120,false,false,false,"Use the GPU for processing blendshapes.");
#endif
	createBoolAttribute("drawMeshWireframe",false,true,"",120,false,false,false,"Render mesh with wireframe.");
	
	SmartBody::util::g_log.RemoveAllListeners();
	ForwardLogListener* forwardListener = new ForwardLogListener();
	SmartBody::util::g_log.AddListener(forwardListener);
#if !defined(WIN_BUILD) && !defined(NATIVE_FRAMEWORK_BUILD)
	SmartBody::util::StdoutListener* stdoutListener = new SmartBody::util::StdoutListener();
	SmartBody::util::g_log.AddListener(stdoutListener);
#endif
	
	//consoleAttr->setValue(true); // set up the console logging
	
	_mediaPath = ".";
		// re-initialize
	// initialize everything

	_viewer = NULL;
	_ogreViewer = NULL;
	_viewerFactory = NULL;
	_ogreViewerFactory = NULL;
	
	_rootGroup = new SrSnGroup();
	_rootGroup->ref();

	_heightField = NULL;
	_navigationMesh = NULL;
	_kinectProcessor = new KinectProcessor();

	// Create default settings
	createDefaultControllers();
	
	/*
	SmartBody::SBSceneListener* listener = getCharacterListener();
	//_scene = SmartBody::SBScene::getScene();
	addSceneListener(listener);
	*/
	getBlendManager()->createBlend0D(PseudoIdleState);

	// reset timer & viewer window
	getSimulationManager()->reset();
//	getSimulationManager()->start();

	/*
#ifndef __native_client__
	SrViewer* viewer = SmartBody::getViewer();
	if (viewer)
		viewer->show_viewer();
#endif

	command("vhmsgconnect");
#ifndef __native_client__
	//Py_Finalize();
	//initPython(initPythonLibPath);
#ifndef SB_NO_PYTHON
	PyRun_SimpleString("scene = getScene()");
	PyRun_SimpleString("bml = scene.getBmlProcessor()");
	PyRun_SimpleString("sim = scene.getSimulationManager()");
#endif
#endif
	*/	
	if (_viewer)	
	{
		if (_viewerFactory)
			_viewerFactory->reset(_viewer);
		_viewer = NULL;
#if !defined (__ANDROID__) && !defined(SB_IPHONE) && !defined(__native_client__) && !defined(EMSCRIPTEN)
		SbmShaderManager::singleton().setViewer(NULL);
#endif
	}

	if (_viewerFactory)
		_viewerFactory->remove(_ogreViewer);
	if (_ogreViewer)
	{
		delete _ogreViewer;
		_ogreViewer = NULL;
	}

	_logListener = NULL;


}

void SBScene::cleanup()
{
	// stop the simulation
	getSimulationManager()->stop();
	
	// reset the simulation parameters
	getSimulationManager()->setSimFps(0);

	// remove the characters
	removeAllCharacters();
	
	// remove the pawns
	removeAllPawns();

	// clear the joint maps
	getJointMapManager()->removeAllJointMaps();

	// remove all blends and transitions
	getBlendManager()->removeAllBlends();
	getBlendManager()->removeAllTransitions();

	// always need a PseudoIdle state
	//SmartBody::SBAnimationBlend0D* idleState = getBlendManager()->createBlend0D(PseudoIdleState);
	//addPABlend(idleState);


	// clear out the default face definitions
	std::vector<std::string> faceDefinitions = getFaceDefinitionNames();

	for (std::vector<std::string>::iterator iter = faceDefinitions.begin();
		 iter != faceDefinitions.end();
		 iter++)
	{
		std::string faceName = (*iter);
		removeFaceDefinition(faceName);
	}

	// stop the services
	SBServiceManager* serviceManager = getServiceManager();
	std::vector<std::string> serviceNames =  serviceManager->getServiceNames();
	for (std::vector<std::string>::iterator iter = serviceNames.begin();
		 iter != serviceNames.end();
		 iter++)
	{
		SBService* service = serviceManager->getService(*iter);
		service->setEnable(false);
	}

	removePendingCommands();

	clearAttributes();

	removeDefaultControllers();

	removeAllAssetPaths("script");
	removeAllAssetPaths("motion");
	removeAllAssetPaths("mesh");
	removeAllAssetPaths("audio");	

	delete _sim;
	delete _profiler;
	delete _bml;
	delete _blendManager;
	delete _reachManager;
	delete _steerManager;
	delete _serviceManager;
	delete _physicsManager;
	delete _gestureMapManager;
	delete _jointMapManager;
	delete _boneBusManager;
	delete _collisionManager;
	delete _phonemeManager;
	delete _behaviorSetManager;
	delete _retargetManager;
	delete _eventManager;
	delete _assetManager;
	delete _speechManager;
	delete _commandManager;
	delete _naviMeshManager;
	delete _kinectProcessor;
	delete _handConfigManager;
	delete _debuggerServer;

	_sim = NULL;
	_profiler = NULL;
	_bml = NULL;
	_blendManager = NULL;
	_reachManager = NULL;
	_steerManager= NULL;
	_serviceManager = NULL;
	_physicsManager = NULL;
	_gestureMapManager= NULL;
	_jointMapManager = NULL;
	_boneBusManager = NULL;
	_collisionManager = NULL;
	_phonemeManager = NULL;
	_behaviorSetManager = NULL;
	_retargetManager = NULL;
	_eventManager = NULL;
	_assetManager = NULL;
	_commandManager = NULL;
	_speechManager = NULL;

	_kinectProcessor = NULL;

	_cameraTracking.clear();
	
	if (_heightField)
	{
		delete _heightField;
	}
	_heightField = NULL;

#if 0 // this should be done in asset manager
	if (_navigationMesh)
	{
		delete _navigationMesh;
	}
#endif
	_navigationMesh = NULL;

	_rootGroup->unref();
	_rootGroup = NULL;

	_viewer = NULL;
	_ogreViewer = NULL;
	_viewerFactory = NULL;
	_ogreViewerFactory = NULL;
	
#ifndef SB_NO_VHCL_AUDIO
	AUDIO_Close();
	AUDIO_Init();
#endif

	if (_vhmsgManager->isEnable() && _vhmsgManager->isConnected())
		_vhmsgManager->send( "vrProcEnd sbm" );
	
//	delete _vhmsgManager;	
//	_vhmsgManager = NULL;

#if !defined(SB_IPHONE)
	SbmTextureManager::destroy_singleton();
	SbmShaderManager::destroy_singleton();
#endif

#ifndef SB_NO_PYTHON
//	Py_Finalize();

#if defined(WIN_BUILD)
	{
		// According to the python docs, .pyd files are not unloaded during Py_Finalize().
		// This causes issues when trying to re-load the smartbody dll over and over.
		// So, we force unload these .pyd files.  This list is all the standard .pyd files included in the Python26 DLLs folder.
		// For reference:  http://docs.python.org/2/c-api/init.html  "Dynamically loaded extension modules loaded by Python are not unloaded"

		// initPythonLibPath - eg:  "../../../../core/smartbody/Python26/Lib"
		std::string pythonLibPath = Py_GetPythonHome();
		HMODULE hmodule;
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/bz2.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/pyexpat.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/select.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/unicodedata.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/winsound.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_bsddb.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_ctypes.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_ctypes_test.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_elementtree.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_hashlib.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_msi.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_multiprocessing.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_socket.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_sqlite3.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_ssl.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_testcapi.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
		hmodule = GetModuleHandle(SmartBody::util::format("%s/../DLLs/_tkinter.pyd", pythonLibPath.c_str()).c_str());
		FreeLibrary(hmodule);
	}
#endif  // WIN_BUILD
#endif  // USE_PYTHON
}

SBScene::~SBScene(void)
{
	cleanup();
	for (std::map<std::string, SBScript*>::iterator iter = _scripts.begin();
		 iter != _scripts.end();
		 iter++)
	{
	//	delete (*iter).second;
	}

	delete _parser;

	_debuggerClient->Disconnect();
  // TODO: should delete these in reverse order?
	delete _debuggerClient;
	delete _debuggerUtility;
//	
	//mcu.reset();
}

SBDebuggerServer* SBScene::getDebuggerServer()
{
	return _debuggerServer; 
}

SBDebuggerClient* SBScene::getDebuggerClient()
{
	return _debuggerClient; 
}


SBDebuggerUtility* SBScene::getDebuggerUtility()
{
	return _debuggerUtility; 
}

SBScene* SBScene::getScene()
{
	if (_firstTime)
	{
		XMLPlatformUtils::Initialize(); 
		_firstTime = false;
		_scene = new SBScene();
		_scene->initialize();
	}

	return _scene;
}
 
void SBScene::destroyScene()
{
	if (_scene)
	{
		XMLPlatformUtils::Terminate(); 
		delete _scene;
		_scene = NULL;
		_firstTime = true;
	}
}

void SBScene::setProcessId(const std::string& id)
{
	_processId = id;
}

const std::string& SBScene::getProcessId()
{
	return _processId;
}

void SBScene::update()
{
	this->getProfiler()->updateProfiler(this->getSimulationManager()->getTime());
	this->getProfiler()->mark("SBScene::update()", 1, "start");
	// remote mode
	if (isRemoteMode())
	{
		// update client
		getDebuggerClient()->Update();
		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
		{
			SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
			pawn->ct_tree_p->evaluate(getSimulationManager()->getTime());
			pawn->ct_tree_p->applySkeletonToBuffer();
		}

		// update renderer as listener
		std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
		for (size_t i = 0; i < listeners.size(); i++)
		{
			listeners[i]->OnSimulationUpdate( );
		}

		return;
	}
	
	//SmartBody::util::log("After remote mode");
	// scripts
	this->getProfiler()->mark("scripts", 1, "beforeUpdate()");
	std::map<std::string, SmartBody::SBScript*>& scripts = getScripts();
	for (std::map<std::string, SmartBody::SBScript*>::iterator iter = scripts.begin();
		iter != scripts.end();
		iter++)
	{
		if ((*iter).second->isEnable())
			(*iter).second->beforeUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("scripts");
	//SmartBody::util::log("After script");
	// services
	this->getProfiler()->mark("services", 1, "beforeUpdate()");
	std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
	for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
		iter != services.end();
		iter++)
	{
		if ((*iter).second->isEnable())
			(*iter).second->beforeUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("services");
	//SmartBody::util::log("After services");

	this->getProfiler()->mark("allsequences", 1, "commands");
	std::string seqName = "";
	std::vector<std::string> sequencesToDelete;
	SequenceManager* activeSequences = getCommandManager()->getActiveSequences();
	int numSequences = activeSequences->getNumSequences();
	for (int s = 0; s < numSequences; s++)
	{
		srCmdSeq* seq = activeSequences->getSequence(s, seqName);
		std::string cmd = "";
		if (seq && seq->isValid())
		{
			do {
				cmd = seq->pop( (float) getSimulationManager()->getTime() );
				if (cmd != "")			
				{
					//SmartBody::util::log("execute command = %s",cmd.c_str());
					this->getProfiler()->mark("commands", 1, cmd.c_str() );
					int err = getCommandManager()->execute( (char*)  cmd.c_str() );
					if( err != CMD_SUCCESS )
					{
						SmartBody::util::log( "update ERR: execute FAILED: '%s'\n", cmd.c_str() );
					}
					this->getProfiler()->mark("commands");
				} 
			} while( cmd != "" );
			if( seq->get_count() < 1 )
			{
				sequencesToDelete.push_back(seqName);
			}
		}
	}

	for (size_t d = 0; d < sequencesToDelete.size(); d++)
	{
		activeSequences->removeSequence(sequencesToDelete[d], true);
	}
	activeSequences->cleanupMarkedSequences();
	this->getProfiler()->mark("allsequences");

	this->getProfiler()->mark("pawn", 1, "controller evaluation");
	const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		pawnIter != pawns.end();
		pawnIter++)
	{
		SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
		this->getProfiler()->mark(pawn->getName().c_str(), 1, "controller evaluation");
		pawn->reset_all_channels();
		pawn->ct_tree_p->evaluate( getSimulationManager()->getTime() );
		pawn->ct_tree_p->applyBufferToAllSkeletons();

// 		if (pawn->hasPhysicsSim() && SBPhysicsSim::getPhysicsEngine()->getBoolAttribute("enable"))
// 		{
// 			//pawn->updateFromColObject();
// 		}
// 		else
		{			
			//pawn->updateToColObject();
			pawn->updateToSteeringSpaceObject();
		}
		//SmartBody::util::log("After pawn update");
		SbmCharacter* char_p = getCharacter(pawn->getName().c_str() );

		if( char_p )
		{

			// run the minibrain, if available
			SmartBody::MiniBrain* brain = char_p->getMiniBrain();
			if (brain)
			{
				SmartBody::SBCharacter* sbchar = dynamic_cast<SmartBody::SBCharacter*>(char_p);
				brain->update(sbchar, getSimulationManager()->getTime(), getSimulationManager()->getTimeDt());
			}

			// scene update moved to renderer
			//if (char_p->scene_p)
			//	char_p->scene_p->update();
			//char_p->dMesh_p->update();
			//char_p->updateJointPhyObjs();
			/*
			bool hasPhySim = physicsEngine->getBoolAttribute("enable");
			char_p->updateJointPhyObjs(hasPhySim);
			//char_p->updateJointPhyObjs(false);
			*/
			char_p->_skeleton->update_global_matrices();

			char_p->forward_visemes( getSimulationManager()->getTime() );	
			//char_p->forward_parameters( getSimulationManager()->getTime() );	


		}  // end of char_p processing
		
		this->getProfiler()->mark(pawn->getName().c_str());
		//SmartBody::util::log("After character mode");
	} // end of loop
	this->getProfiler()->mark("pawn");
	
	this->getProfiler()->mark("pawn", 1, "afterUpdate()");
	const std::vector<std::string>& pawnNames = getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		iter != pawnNames.end();
		iter++)
	{
		SBPawn* pawn = getPawn(*iter);
		pawn->afterUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("pawn");

	this->getProfiler()->mark("listeners", 1, "OnSimulationUpdate()");
	std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
	for (size_t i = 0; i < listeners.size(); i++)
	{
		listeners[i]->OnSimulationUpdate( );
	}
	this->getProfiler()->mark("listeners");
	//SmartBody::util::log("After listener update");
	this->getProfiler()->mark("scripts", 1, "update()");
	for (std::map<std::string, SmartBody::SBScript*>::iterator iter = scripts.begin();
		iter != scripts.end();
		iter++)
	{
		if ((*iter).second->isEnable())
			(*iter).second->update(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("scripts");
	//SmartBody::util::log("After running scripts");
	this->getProfiler()->mark("services", 1, "update()");
	for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
		iter != services.end();
		iter++)
	{
		if ((*iter).second->isEnable())
			(*iter).second->update(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("services");
	//SmartBody::util::log("After service update");
	// services
	this->getProfiler()->mark("services", 1, "afterUpdate()");
	for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
		iter != services.end();
		iter++)
	{
		if ((*iter).second->isEnable())
			(*iter).second->afterUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("services");


	// scripts
	this->getProfiler()->mark("scripts", 1, "afterUpdate()");
	for (std::map<std::string, SmartBody::SBScript*>::iterator iter = scripts.begin();
		iter != scripts.end();
		iter++)
	{
		if ((*iter).second->isEnable())
			(*iter).second->afterUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("scripts");

	this->getProfiler()->mark("SBScene::update()");
	//SmartBody::util::log("After SBScene::update");
}



void SBScene::setScale(float val)
{
	if (val == 0.0)
	{
		SmartBody::util::log("Scene scale cannot be set to 0.0, keeping it at %f", val);
		return;
	}
	_scale = val;

	DoubleAttribute* scaleAttribute = dynamic_cast<DoubleAttribute*>(getAttribute("scale"));
	scaleAttribute->setValueFast(_scale);
}

float SBScene::getScale()
{
	return _scale;
}

void SBScene::reset()
{
	cleanup();
	initialize();	
}

void SBScene::notify( SBSubject* subject )
{
	BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(subject);

#ifndef SB_NO_VHCL_AUDIO
	if (boolAttr && boolAttr->getName() == "internalAudio")
	{
		bool val = boolAttr->getValue();
		if (!val)
		{
			SmartBody::util::log("Turning off audio...");
			AUDIO_Close();
		}
		else
		{
			SmartBody::util::log("Turning on audio...");
			AUDIO_Init();
		}
		return;
	}
#endif

  if (boolAttr && boolAttr->getName() == "enableConsoleLogging")
	{
		bool val = boolAttr->getValue();
		if (val)
		{
			if (SmartBody::util::g_log.IsEnabled())
				return;

			SmartBody::util::StdoutListener* listener = new SmartBody::util::StdoutListener();
			SmartBody::util::g_log.AddListener(listener);
		}
		else
		{		
			SmartBody::util::g_log.RemoveAllListeners();
		}
	}

  if (boolAttr && boolAttr->getName() == "bmlstatus")
	{
		this->getBmlProcessor()->getBMLProcessor()->set_bml_feedback(boolAttr->getValue());
	}

	DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(subject);
	if (doubleAttr && doubleAttr->getName() == "scale")
	{
		setScale((float) doubleAttr->getValue());
		return;
	}
}


SBAPI SBCharacter* SBScene::copyCharacter( const std::string& origCharName, const std::string& copyCharName )
{
	SmartBody::SBCharacter* origChar = getCharacter(origCharName);
	if (!origChar)
	{
		SmartBody::util::log("Character '%s' does not exists !", origCharName.c_str());
		return NULL;
	}
	else
	{
		SmartBody::SBCharacter* copyChar = createCharacter(copyCharName,"");
		if (!copyChar)
		{
			SmartBody::util::log("Can not copy to existing character '%s'",copyCharName.c_str());
			return NULL;
		}
		// successfully create a new character
		SmartBody::SBSkeleton* sk = new SmartBody::SBSkeleton(origChar->getSkeleton());
		copyChar->setSkeleton(sk);
		copyChar->createStandardControllers();
		copyChar->copy(origChar);
		SmartBody::SBSteerManager* steerManager = getSteerManager();
		SmartBody::SBSteerAgent* origAgent = steerManager->getSteerAgent(origCharName);
		if (origAgent) // the original character has steering
		{
			SmartBody::SBSteerAgent* agent = steerManager->createSteerAgent(copyCharName);
			agent->setSteerType(origAgent->getSteerType());
			agent->setSteerStateNamePrefix(origAgent->getSteerStateNamePrefix());
			bool steerEnable = steerManager->isEnable();
			if (steerEnable)
			{
				steerManager->setEnable(false);
				steerManager->setEnable(true);
			}			
		}
		return copyChar;
	}
}


SBAPI SBPawn* SBScene::copyPawn( const std::string& origPawnName, const std::string& copyPawnName )
{
	SmartBody::SBPawn* origPawn = getPawn(origPawnName);
	if (!origPawn)
	{
		SmartBody::util::log("Pawn '%s' does not exists !", origPawnName.c_str());
		return NULL;
	}
	else
	{
		SmartBody::SBPawn* copyPawn = createPawn(copyPawnName);
		if (!copyPawn)
		{
			SmartBody::util::log("Can not copy to existing pawn '%s'",copyPawnName.c_str());
			return NULL;
		}
		// successfully create a new character
		SmartBody::SBSkeleton* sk = new SmartBody::SBSkeleton(origPawn->getSkeleton());
		copyPawn->setSkeleton(sk);		
		copyPawn->copy(origPawn);		
		return copyPawn;
	}
}

SBCharacter* SBScene::createCharacter(const std::string& charName, const std::string& metaInfo)
{	
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (character)
	{
		SmartBody::util::log("Character '%s' already exists!", charName.c_str());
		return NULL;
	}
	else
	{
		SBCharacter* character = new SBCharacter(charName, metaInfo);
		
		std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.find(character->getName());
		if (iter != _pawnMap.end())
		{
			SmartBody::util::log( "Register character: pawn_map.insert(..) '%s' FAILED\n", character->getName().c_str() );
			delete character;
			return NULL;
		}

		_pawnMap.insert(std::pair<std::string, SbmPawn*>(character->getName(), character));
		_pawnNames.push_back(character->getName());
	
		std::map<std::string, SbmCharacter*>::iterator citer = _characterMap.find(character->getName());
		if (citer != _characterMap.end())
		{
			SmartBody::util::log( "Register character: character_map.insert(..) '%s' FAILED\n", character->getName().c_str() );
			_pawnMap.erase(iter);
			delete character;
			return NULL;
		}
		_characterMap.insert(std::pair<std::string, SbmCharacter*>(character->getName(), character));
		_characterNames.push_back(character->getName());

		//if (getCharacterListener() )
		//	getCharacterListener()->OnCharacterCreate( character->getName().c_str(), character->getClassType() );
		SBSkeleton* skeleton = new SBSkeleton();		
		character->setSkeleton(skeleton);
//		SkJoint* joint = skeleton->add_joint(SkJoint::TypeQuat);
//		joint->setName("world_offset");		
//		joint->update_gmat();

#ifndef SB_NO_BONEBUS
		if (getBoneBusManager()->isEnable())
		{
			std::string classType = character->getClassType();
			if (classType == "")
				classType = "<unknown>"; // make sure that the class type has some data. Empty string will cause problems with parsing.
			getBoneBusManager()->getBoneBus().CreateCharacter( character->getName().c_str(), classType.c_str(), true );
		}
#endif

		std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
		for (size_t i = 0; i < listeners.size(); i++)
		{
			listeners[i]->OnCharacterCreate( character->getName().c_str(), character->getClassType() );
		}

		// notify the services		
		std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
		for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
			iter != services.end();
			iter++)
		{
			SBService* service = (*iter).second;
			service->onCharacterCreate(character);
		}		
		return character;
	}
}

SBPawn* SBScene::createPawn(const std::string& pawnName)
{
	SBPawn* pawn = getPawn(pawnName);
	SBCharacter* character = dynamic_cast<SBCharacter*>(pawn);
	if (character)
	{
		SmartBody::util::log("Pawn '%s' is a character.", pawnName.c_str());
		return NULL;
	}
	if (pawn)
	{
		SmartBody::util::log("Pawn '%s' already exists!", pawnName.c_str());
		return NULL;
	}
	else
	{
		SBPawn* pawn = new SBPawn(pawnName.c_str());
		SBSkeleton* skeleton = new SBSkeleton();
		pawn->setSkeleton(skeleton);
		//SkJoint* joint = skeleton->add_joint(SkJoint::TypeQuat);
		//joint->setName("world_offset");

		std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.find(pawn->getName());
		if (iter != _pawnMap.end())
		{
			SmartBody::util::log( "Register pawn: pawn_map.insert(..) '%s' FAILED\n", pawn->getName().c_str() );
			delete pawn;
			return NULL;
		}

		_pawnMap.insert(std::pair<std::string, SbmPawn*>(pawn->getName(), pawn));
		_pawnNames.push_back(pawn->getName());
	

		std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
		for (size_t i = 0; i < listeners.size(); i++)
		{
			listeners[i]->OnPawnCreate( pawn->getName().c_str() );
		}

		// notify the services
		std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
		for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
			iter != services.end();
			iter++)
		{
			SBService* service = (*iter).second;
			service->onPawnCreate(character);
		}
		return pawn;
	}
}

void SBScene::removeCharacter(const std::string& charName)
{
	SBCharacter* character = this->getCharacter(charName);
	const std::string& name = character->getName();
	if (character)
	{

		string vrProcEnd_msg = "vrProcEnd sbm ";
		vrProcEnd_msg += getName();
		SmartBody::SBScene::getScene()->getVHMsgManager()->send( vrProcEnd_msg.c_str() );

		// notify the services
		std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
		for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
			iter != services.end();
			iter++)
		{
			SBService* service = (*iter).second;
			service->onCharacterDelete(character);
		}
	
		std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
		for (size_t i = 0; i < listeners.size(); i++)
		{
			listeners[i]->OnCharacterDelete( name);
		}

#ifndef SB_NO_BONEBUS
		if ( character->bonebusCharacter )
		{
			this->getBoneBusManager()->getBoneBus().DeleteCharacter(  character->bonebusCharacter );
			character->bonebusCharacter = NULL;
		}
#endif

		std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.find(name);
		if (iter != _pawnMap.end())
		{
			_pawnMap.erase(iter);
		}
		for (std::vector<std::string>::iterator iter = _pawnNames.begin();
			 iter != _pawnNames.end();
			 iter++)
		{
			if (name == (*iter))
			{
				_pawnNames.erase(iter);
				break;
			}
		}

		std::map<std::string, SbmCharacter*>::iterator citer = _characterMap.find(name);
		if (citer != _characterMap.end())
		{
			_characterMap.erase(citer);
		}
		for (std::vector<std::string>::iterator iter = _characterNames.begin();
			 iter != _characterNames.end();
			 iter++)
		{
			if (name == (*iter))
			{
				_characterNames.erase(iter);
				break;
			}
		}

		delete character;
	}	
}

void SBScene::removeAllCharacters()
{
	std::vector<std::string> characters = getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characters.begin();
		 iter != characters.end();
		 iter++)
	{
		removeCharacter((*iter));
	}
	
}

void SBScene::removePawn(const std::string& pawnName)
{
	SbmPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnName);
	if (pawn)
	{
		const std::string& name = pawn->getName();

		SbmCharacter* character = dynamic_cast<SbmCharacter*>(pawn);
		if (!character)
		{
			// notify the services
			std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
			for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
				iter != services.end();
				iter++)
			{
				SBService* service = (*iter).second;
				SBPawn* sbpawn = dynamic_cast<SBPawn*>(pawn);
				service->onPawnDelete(sbpawn);
			}
			
			std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
			for (size_t i = 0; i < listeners.size(); i++)
			{
				listeners[i]->OnPawnDelete( name );
			}

			std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.find(name);
			if (iter != _pawnMap.end())
			{
				_pawnMap.erase(iter);
			}
			for (std::vector<std::string>::iterator iter = _pawnNames.begin();
			 iter != _pawnNames.end();
			 iter++)
			{
				if (name == (*iter))
				{
					_pawnNames.erase(iter);
					break;
				}
			}

			delete pawn;
		}
	}	
}

void SBScene::removeAllPawns()
{
	std::vector<std::string> pawns = getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawns.begin();
		 iter != pawns.end();
		 iter++)
	{
		removePawn((*iter));
	}
	

	// clear the cameras
	_cameras.clear();
}

int SBScene::getNumCharacters() 
{  
	 
	return _characterMap.size(); 
}

int SBScene::getNumPawns() 
{  
	return _pawnMap.size() - _characterMap.size(); 
}

SBPawn* SBScene::getPawn(const std::string& name)
{
	std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.find(name);
	if (iter == _pawnMap.end())
	{
		return NULL;
	}
	else
	{
		SBPawn* sbpawn = dynamic_cast<SBPawn*>((*iter).second);
		return sbpawn;
	}
}

SBCharacter* SBScene::getCharacter(const std::string& name)
{
	std::map<std::string, SbmCharacter*>::iterator iter = _characterMap.find(name);
	if (iter == _characterMap.end())
	{
		return NULL;
	}
	else
	{
		SBCharacter* sbcharacter = dynamic_cast<SBCharacter*>((*iter).second);
		return sbcharacter;
	}
}

const std::vector<std::string>& SBScene::getPawnNames()
{
	return _pawnNames;
}

const std::vector<std::string>& SBScene::getCharacterNames()
{
	return _characterNames;
}

std::vector<std::string> SBScene::getEventHandlerNames()
{
	SBEventManager* eventManager = getEventManager();
	
	std::vector<std::string> ret;

	for(std::map<std::string, SBEventHandler*>::iterator iter = eventManager->getEventHandlers().begin();
		iter != eventManager->getEventHandlers().end();
		iter++)
	{

		ret.push_back(std::string(iter->first));
	}
	return ret;
}


void SBScene::setMediaPath(const std::string& path)
{
	_mediaPath = path;
}

const std::string& SBScene::getMediaPath()
{
	return _mediaPath;
}

void SBScene::setDefaultCharacter(const std::string& character)
{
	SmartBody::SBScene::getScene()->setStringAttribute("defaultCharacter", character);
}

void SBScene::setDefaultRecipient(const std::string& recipient)
{
	SmartBody::SBScene::getScene()->setStringAttribute("defaultRecipient", recipient);
}

SBEventManager* SBScene::getEventManager()
{
	return _eventManager;
}


bool SBScene::command(const std::string& command)
{
	if (this->getBoolAttribute("warnDeprecatedCommands"))
	{
		SmartBody::util::log("Warning: Deprecated command [%s]", command.c_str());
	}

	int ret = getCommandManager()->execute((char*) command.c_str());

	if (ret == CMD_SUCCESS)
		return true;
	else
		return false;
}

bool SBScene::commandAt(float seconds, const std::string& command)
{
	if (this->getBoolAttribute("warnDeprecatedCommands"))
	{
		SmartBody::util::log("Warning: Deprecated command [%f] [%s]", seconds, command.c_str());
	}

	int ret = getCommandManager()->execute_later((char*) command.c_str(), seconds);

	if (ret == CMD_SUCCESS)
		return true;
	else
		return false;
}

void SBScene::removePendingCommands()
{
	SmartBody::SBScene::getScene()->getCommandManager()->getActiveSequences()->clear();
	SmartBody::SBScene::getScene()->getCommandManager()->getPendingSequences()->clear();
}

void SBScene::sendVHMsg(const std::string& message)
{	 
	SmartBody::SBScene::getScene()->getVHMsgManager()->send(message.c_str());
}

void SBScene::sendVHMsg2(const std::string& message, const std::string& message2)
{
	 
	SmartBody::SBScene::getScene()->getVHMsgManager()->send2(message.c_str(), message2.c_str());
}

bool SBScene::run(const std::string& command)
{
#ifndef SB_NO_PYTHON
	try {
		//SmartBody::util::log("executePython = %s",command);

		int ret = PyRun_SimpleString(command.c_str());
		//SmartBody::util::log("cmd result = %d",result);
		if (ret == -1)
		{
			SBEvent* event = this->getEventManager()->createEvent("error", command, this->getStringFromObject(this));
			this->getEventManager()->handleEvent(event);
			delete event;
			PyErr_Print();
			PyErr_Clear();
		}

		return true;
	} catch (...) {
		PyErr_Print();
		PyErr_Clear();
		SBEvent* event = this->getEventManager()->createEvent("error", command, this->getStringFromObject(this));
		this->getEventManager()->handleEvent(event);
		delete event;
		return false;
	}
#endif
#if defined(EMSCRIPTEN)
#ifndef SB_NO_JAVASCRIPT
	emscripten_run_script(
		command.c_str()
	);
	//otehr options:
	//int emscripten_run_script_int(const char *script)
	//char *emscripten_run_script_string(const char *script)
#endif
#endif
	return true;
}

bool SBScene::runScript(const std::string& script)
{
#ifndef SB_NO_PYTHON
	// add the .seq extension if necessary
	std::string candidateSeqName = script;
	if (candidateSeqName.find(".py") == std::string::npos)
	{
		candidateSeqName.append(".py");
	}

	std::string curFilename = SmartBody::SBScene::getScene()->getAssetManager()->findFileName("script", candidateSeqName);
	SmartBody::util::log("script name = '%s', curFilename = '%s'", script.c_str(), curFilename.c_str());
	if (curFilename != "")
	{
		try {
			// save the last directory so that a script path can be used as a relative pathing for asset loading or other use
			boost::filesystem::path scriptPath = curFilename;
			boost::filesystem::path scriptDir = scriptPath.parent_path();
			this->setLastScriptDirectory(scriptDir.string());

			std::stringstream strstr;
			strstr << "execfile(\"" << curFilename << "\")";
			int ret = PyRun_SimpleString(strstr.str().c_str());
			if (ret == -1)
			{
				SBEvent* event = this->getEventManager()->createEvent("error", script, this->getStringFromObject(this));
				this->getEventManager()->handleEvent(event);
				delete event;
				
				PyErr_Print();
				PyErr_Clear();
			}
			PyErr_Print();
			PyErr_Clear();
			return true;
		} catch (...) {
			PyErr_Print();			
			PyErr_Clear();
			SBEvent* event = this->getEventManager()->createEvent("error", script, this->getStringFromObject(this));
			this->getEventManager()->handleEvent(event);
			delete event;
			return false;
		}
	}

	SmartBody::util::log("Could not find Python script '%s'", script.c_str());
	return false;

#endif
#if defined(EMSCRIPTEN)
#ifndef SB_NO_JAVASCRIPT
	emscripten_run_script(script.c_str());
#endif
#endif
	return true;
}

SBSimulationManager* SBScene::getSimulationManager()
{
	return _sim;
}

SBProfiler* SBScene::getProfiler()
{
	return _profiler;
}

SBBmlProcessor* SBScene::getBmlProcessor()
{
	return _bml;
}

SBAnimationBlendManager* SBScene::getBlendManager()
{
	return _blendManager;
}

SBReachManager* SBScene::getReachManager()
{
	return _reachManager;
}

SBSteerManager* SBScene::getSteerManager()
{
	return _steerManager;
}

SBAPI SBRealtimeManager* SBScene::getRealtimeManager()
{
	return _realtimeManager;
}

SBServiceManager* SBScene::getServiceManager()
{
	return _serviceManager;
}


SBCollisionManager* SBScene::getCollisionManager()
{
	return _collisionManager;
}

SBPhonemeManager* SBScene::getDiphoneManager()
{
	return _phonemeManager;
}

SBBehaviorSetManager* SBScene::getBehaviorSetManager()
{
	return _behaviorSetManager;
}

SBMotionGraphManager* SBScene::getMotionGraphManager()
{
	return _motionGraphManager;
}

SBRetargetManager* SBScene::getRetargetManager()
{
	return _retargetManager;
}

SBAssetManager* SBScene::getAssetManager()
{
	return _assetManager;
}

SBSpeechManager* SBScene::getSpeechManager()
{
	return _speechManager;
}

SBPhysicsManager* SBScene::getPhysicsManager()
{
	return _physicsManager;
}

SBBoneBusManager* SBScene::getBoneBusManager()
{
	return _boneBusManager;
}

SBGestureMapManager* SBScene::getGestureMapManager()
{
	return _gestureMapManager;
}

SBHandConfigurationManager* SBScene::getHandConfigurationManager()
{
	return _handConfigManager;
}

SBJointMapManager* SBScene::getJointMapManager()
{
	return _jointMapManager;
}

SBCommandManager* SBScene::getCommandManager()
{
	return _commandManager;
}

SBNavigationMeshManager* SBScene::getNavigationMeshManager()
{
	return _naviMeshManager;
}


SBVHMsgManager* SBScene::getVHMsgManager()
{
	return _vhmsgManager;
}

SBParser* SBScene::getParser()
{
	return _parser;
}

bool SBScene::isRemoteMode()	
{ 
	return _isRemoteMode; 
}

void SBScene::setRemoteMode(bool val)	
{ 
	_isRemoteMode = val; 
}

SmartBody::SBFaceDefinition* SBScene::createFaceDefinition(const std::string& name)
{
	// make sure the name doesn't already exist
	if (_faceDefinitions.find(name) != _faceDefinitions.end())
	{
		SmartBody::util::log("Face definition named '%s' already exists. Returning existing one", name.c_str());
		return _faceDefinitions[name];
	}

	SBFaceDefinition* face = new SBFaceDefinition(name);
	_faceDefinitions.insert(std::pair<std::string, SBFaceDefinition*>(name, face));
	for (size_t l = 0; l < this->_sceneListeners.size(); l++)
	{
		this->_sceneListeners[l]->OnObjectCreate(face);
	}

	return face;
}

void SBScene::removeFaceDefinition(const std::string& name)
{
	

	// make sure the name doesn't already exist
	std::map<std::string, SBFaceDefinition*>::iterator iter = _faceDefinitions.find(name);
	if (iter ==_faceDefinitions.end())
	{
		SmartBody::util::log("Face definition named '%s' does not exist.", name.c_str());
		return;
	}
	for (size_t l = 0; l < this->_sceneListeners.size(); l++)
	{
		this->_sceneListeners[l]->OnObjectDelete(iter->second);
	}
	delete iter->second;
	iter->second = NULL;
	_faceDefinitions.erase(iter);

}

SmartBody::SBFaceDefinition* SBScene::getFaceDefinition(const std::string& name)
{
	// make sure the name doesn't already exist
	std::map<std::string, SBFaceDefinition*>::iterator iter = _faceDefinitions.find(name);
	if (iter == _faceDefinitions.end())
	{
		SmartBody::util::log("Face definition named '%s' does not exist.", name.c_str());
		return NULL;
	}

	return (*iter).second;
}

int SBScene::getNumFaceDefinitions()
{
	return 	_faceDefinitions.size();
}

std::vector<std::string> SBScene::getFaceDefinitionNames()
{
	std::vector<std::string> faces;
	for (std::map<std::string, SBFaceDefinition*>::iterator iter =  _faceDefinitions.begin();
		 iter !=  _faceDefinitions.end();
		 iter++)
	{
		faces.push_back((*iter).second->getName());
	}

	return faces;
}

void SBScene::addScript(const std::string& name, SBScript* script)
{
	std::map<std::string, SBScript*>::iterator iter = _scripts.find(name);
	if (iter != _scripts.end())
	{
		SmartBody::util::log("Script with name %s already exists. Remove current script.", name.c_str());
		//return;
		_scripts.erase(iter);
	}
	script->setName(name);

	_scripts.insert(std::pair<std::string, SBScript*>(name, script));
	script->start();
}

void SBScene::removeScript(const std::string& name)
{
	std::map<std::string, SBScript*>::iterator iter = _scripts.find(name);
	if (iter != _scripts.end())
	{
		(*iter).second->stop();
		_scripts.erase(iter);
		return;
	}
	SmartBody::util::log("Script with name %s does not exist.", name.c_str());


}

int SBScene::getNumScripts()
{
	return _scripts.size();
}

std::vector<std::string> SBScene::getScriptNames()
{
	std::vector<std::string> scriptNames;

	for (std::map<std::string, SBScript*>::iterator iter = _scripts.begin();
		 iter != _scripts.end();
		 iter++)
	{
		scriptNames.push_back((*iter).first);
	}

	return scriptNames;

}

SBScript* SBScene::getScript(const std::string& name)
{
	std::map<std::string, SBScript*>::iterator iter = _scripts.find(name);
	if (iter == _scripts.end())
	{
		SmartBody::util::log("Script with name %s already exists.", name.c_str());
		return NULL;
	}

	return (*iter).second;
}

std::map<std::string, SBScript*>& SBScene::getScripts()
{
	return _scripts;
}

void SBScene::addSceneListener(SBSceneListener* listener)
{
	std::vector<SBSceneListener*>::iterator iter = std::find(_sceneListeners.begin(), _sceneListeners.end(), listener);
	if (iter == _sceneListeners.end())
		_sceneListeners.push_back(listener);
}

void SBScene::removeSceneListener(SBSceneListener* listener)
{
	std::vector<SBSceneListener*>::iterator iter = std::find(_sceneListeners.begin(), _sceneListeners.end(), listener);
	if (iter != _sceneListeners.end())
		_sceneListeners.erase(iter);
}

void SBScene::removeAllSceneListeners()
{
	_sceneListeners.clear();
}

std::vector<SBSceneListener*>& SBScene::getSceneListeners()
{
	return _sceneListeners;
}

std::string SBScene::saveSceneSetting()
{
	std::stringstream strstr;
	strstr << "# Autogenerated by SmartBody\n";
	// save all default cameras
	std::vector<std::string> cameras = getCameraNames();
	for (std::vector<std::string>::iterator cameraIter = cameras.begin();
		cameraIter != cameras.end();
		cameraIter++)
	{
		if (*cameraIter == "cameraDefault")
			continue; // don't save default camera
		SrCamera* camera = getCamera((*cameraIter));
		strstr << "obj = scene.getCamera(\"" << camera->getName() << "\")\n";
		strstr << "if obj == None:\n";
		strstr << "\tobj = scene.createCamera(\"" << camera->getName() << "\")\n";		
		strstr << "obj.setEye(" << camera->getEye().x << ", " << camera->getEye().y << ", " << camera->getEye().z << ")\n";
		strstr << "obj.setCenter(" << camera->getCenter().x << ", " << camera->getCenter().y << ", " << camera->getCenter().z << ")\n";
		strstr << "obj.setUpVector(SrVec(" << camera->getUpVector().x << ", " << camera->getUpVector().y << ", " << camera->getUpVector().z << "))\n";
		strstr << "obj.setScale(" << camera->getScale() << ")\n";
		strstr << "obj.setFov(" << camera->getFov() << ")\n";
		strstr << "obj.setFarPlane(" << camera->getFarPlane() << ")\n";
		strstr << "obj.setNearPlane(" << camera->getNearPlane() << ")\n";
		strstr << "obj.setAspectRatio(" << camera->getAspectRatio() << ")\n";

		std::vector<std::string> attributeNames = camera->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			iter != attributeNames.end();
			iter++)
		{
			SmartBody::SBAttribute* attr = camera->getAttribute((*iter));
			if (!attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}
	}	

	// save all pawns (including light pawns) position & orientation.
	const std::vector<std::string>& pawns = getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		pawnIter != pawns.end();
		pawnIter++)
	{
		SBPawn* pawn = getPawn((*pawnIter));
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
			continue; // already wrote out pawns
		strstr << "\n# ---- pawn: " << pawn->getName() << "\n";
		strstr << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		strstr << "if obj == None:\n";
		strstr << "\tobj = scene.createPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		strstr << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrVec hpr = pawn->getHPR();
		strstr << "obj.setHPR(SrVec(" << hpr[0] << ", " << hpr[1] << ", " << hpr[2] << "))\n";
		// attributes
		std::vector<std::string> attributeNames = pawn->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			iter != attributeNames.end();
			iter++)
		{
			SmartBody::SBAttribute* attr = pawn->getAttribute((*iter));
			if (!attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}
	}

	// restore all character position/orientation
	const std::vector<std::string>& characters = getCharacterNames();
	for (std::vector<std::string>::const_iterator characterIter = characters.begin();
		characterIter != characters.end();
		characterIter++)
	{
		SBCharacter* character = getCharacter((*characterIter));
		strstr << "\n# ---- character: " << character->getName() << "\n";
		strstr << "obj = scene.getCharacter(\"" << character->getName() << "\")\n";
		strstr << "if obj != None:\n";				
		SrVec position = character->getPosition();
		strstr << "\tobj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrVec hpr = character->getHPR();
		strstr << "\tobj.setHPR(SrVec(" << hpr[0] << ", " << hpr[1] << ", " << hpr[2] << "))\n";
	}

	return strstr.str();
}

std::string SBScene::save(bool remoteSetup, std::string mediaPath)
{
	std::stringstream strstr;
	strstr << "# Autogenerated by SmartBody\n";

	saveScene(strstr, remoteSetup);
	saveAssets(strstr, remoteSetup, mediaPath);
	saveCameras(strstr, remoteSetup);
	saveLights(strstr, remoteSetup);
	saveFaceDefinitions(strstr, remoteSetup);
	//saveJointMaps(strstr, remoteSetup); // need to apply joint map before apply any motion operations
	saveLipSyncing(strstr, remoteSetup);
	saveBlends(strstr, remoteSetup);
	saveRetargets(strstr, remoteSetup);
	saveGestureMaps(strstr, remoteSetup);
	savePawns(strstr, remoteSetup);
	saveCharacters(strstr, remoteSetup);
	saveServices(strstr, remoteSetup);

	return strstr.str();
}

std::string SBScene::exportScene(const std::vector<std::string>& aspects, std::string mediaPath , bool remoteSetup)
{
	std::stringstream strstr;
	strstr << "# Autogenerated by SmartBody\n";
	
	std::set<std::string> set;
	for (std::vector<std::string>::const_iterator iter = aspects.begin();
		iter != aspects.end(); iter++)
	{
		set.insert(*iter);
	}

	if (set.find("scene") != set.end())
		saveScene(strstr, remoteSetup);
	if (set.find("assets") != set.end())
		saveAssets(strstr, remoteSetup, "");
	if (set.find("cameras") != set.end())
		saveCameras(strstr, remoteSetup);
	if (set.find("lights") != set.end())
		saveLights(strstr, remoteSetup);
	if (set.find("face definitions") != set.end())
		saveFaceDefinitions(strstr, remoteSetup);
	//if (set.find("joint maps") != set.end())
	//	saveJointMaps(strstr, remoteSetup);
	if (set.find("lip syncing") != set.end())
		saveLipSyncing(strstr, remoteSetup);
	if (set.find("blends and transitions") != set.end())
		saveBlends(strstr, remoteSetup);
	if (set.find("retargets") != set.end())
		saveRetargets(strstr, remoteSetup);
	if (set.find("gesture maps") != set.end())
		saveGestureMaps(strstr, remoteSetup);
	if (set.find("pawns") != set.end())
		savePawns(strstr, remoteSetup);
	if (set.find("characters") != set.end())
		saveCharacters(strstr, remoteSetup);
	if (set.find("services") != set.end())
		saveServices(strstr, remoteSetup);	

	return strstr.str();
}


void SBScene::saveScene(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- cameras\n";
	strstr << "scene.setScale(" << getScale() << ")\n";

	strstr << "# -------------------- scene\n";
	strstr << "obj = scene\n";
	// scene attributes
	std::vector<std::string> attributeNames = this->getAttributeNames();
	for (std::vector<std::string>::iterator iter = attributeNames.begin();
			iter != attributeNames.end();
			iter++)
	{
		SmartBody::SBAttribute* attr = this->getAttribute((*iter));
		if (!attr->isDefaultValue())
		{
			std::string attrWrite = attr->write();
			strstr << attrWrite;
		}
	}
	
}

void SBScene::saveRetargets( std::stringstream& strstr, bool remoteSetup )
{	
	strstr << "# -------------------- on-line retargeting setup\n";
	strstr << "print \"Save Retargets\"\n";
	if (remoteSetup) // don't save retarget instance for remote setup
		return;

	SmartBody::SBRetargetManager* retargetManager = getRetargetManager();
	strstr << "retargetManager = scene.getRetargetManager()\n";
	std::vector<StringPair> retargetNames = retargetManager->getRetargetNames();
	for (unsigned int i=0;i<retargetNames.size();i++)
	{
		StringPair& sp = retargetNames[i];
		SmartBody::SBRetarget* retarget = retargetManager->getRetarget(sp.first, sp.second);
		strstr << "retarget = retargetManager.getRetarget(\"" << sp.first << "\",\"" << sp.second << "\")\n";
		strstr << "if retarget == None:\n";
		strstr << "\tretarget = retargetManager.createRetarget(\"" << sp.first << "\",\"" << sp.second << "\")\n";
		std::vector<std::string> endJoints = retarget->getEndJointNames();
		std::vector<std::string> relativeJoints = retarget->getRelativeJointNames();
		strstr << "endJoints = StringVec()\n";
		for (unsigned int k=0;k<endJoints.size();k++)
			strstr << "endJoints.append(\"" << endJoints[k] << "\")\n";
		strstr << "relativeJoints = StringVec()\n";
		for (unsigned int k=0;k<endJoints.size();k++)
			strstr << "relativeJoints.append(\"" << relativeJoints[k] << "\")\n";
		
		strstr << "retarget.initRetarget(endJoints, relativeJoints)\n";
	}
}


#if (BOOST_VERSION > 104400)
bool copyDir(boost::filesystem::path const & source,boost::filesystem::path const & destination)
{
	namespace fs = boost::filesystem;
#else
bool copyDir(boost::filesystem2::path const & source,boost::filesystem2::path const & destination)
{
	namespace fs = boost::filesystem2;
#endif
	try
	{
		// Check whether the function call is valid
		if(
			!fs::exists(source) ||
			!fs::is_directory(source)
			)
		{
			std::cerr << "Source directory " << source.string()
				<< " does not exist or is not a directory." << std::endl
				;
			return false;
		}
		if(fs::exists(destination))
		{
			// still try to overwrite ?
// 			std::cerr << "Destination directory " << destination.string()
// 				<< " already exists." << std::endl
// 				;
// 			return false;
		}
		else if(!fs::create_directories(destination)) // create destination dir
		{
			std::cerr << "Unable to create destination directory"
				<< destination.string() << std::endl
				;
			return false;
		}
	}
	catch(fs::filesystem_error const & e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	// Iterate through the source directory
	for(
		fs::directory_iterator file(source);
		file != fs::directory_iterator(); ++file
		)
	{
		try
		{
			fs::path current(file->path());
			if(fs::is_directory(current))
			{
				// Found directory: Recursion
				if(
					!copyDir(
					current,
					destination / current.filename()
					)
					)
				{
					return false;
				}
			}
			else if (!fs::exists(destination / current.filename()))
			{
				// Found file: Copy
				fs::copy_file(
					current,
					destination / current.filename(), fs::copy_option::none);
			}
		}
		catch(fs::filesystem_error const & e)
		{
			std:: cerr << e.what() << std::endl;
		}
	}
	return true;
}


#if (BOOST_VERSION > 104400)
bool find_file(const boost::filesystem::path&  dir_path, const std::string & file_name, boost::filesystem::path& path_found) {
		using boost::filesystem::path;
		using boost::filesystem::directory_iterator;
		//using boost::filesystem::dot;
		//using boost::filesystem::slash;
#else
bool find_file(const boost::filesystem2::path&  dir_path, const std::string & file_name, boost::filesystem2::path& path_found) { 
		using boost::filesystem2::path;
		using boost::filesystem2::directory_iterator;
		//using boost::filesystem2::dot;
		//using boost::filesystem2::slash;
#endif

	if ( !exists( dir_path ) ) return false;
	directory_iterator end_itr; // default construction yields past-the-end
	for ( directory_iterator itr( dir_path );
		itr != end_itr;
		++itr )
	{
		if ( is_directory(itr->status()) )
		{
			if ( find_file( itr->path(), file_name, path_found ) ) return true;
		}
		else if ( itr->path().filename() == file_name ) // see below
		{
			path_found = itr->path();
			return true;
		}
	}
	return false;
}

/**
 * https://svn.boost.org/trac/boost/ticket/1976#comment:2
 * 
 * "The idea: uncomplete(/foo/new, /foo/bar) => ../new
 *  The use case for this is any time you get a full path (from an open dialog, perhaps)
 *  and want to store a relative path so that the group of files can be moved to a different
 *  directory without breaking the paths. An IDE would be a simple example, so that the
 *  project file could be safely checked out of subversion."
 * 
 * ALGORITHM:
 *  iterate path and base
 * compare all elements so far of path and base
 * whilst they are the same, no write to output
 * when they change, or one runs out:
 *   write to output, ../ times the number of remaining elements in base
 *   write to output, the remaining elements in path
 */
#if (BOOST_VERSION > 104400)
boost::filesystem::path
naive_uncomplete(boost::filesystem::path const p, boost::filesystem::path const base) {
    using boost::filesystem::path;
    //using boost::filesystem::dot;
    //using boost::filesystem::slash;
#else
boost::filesystem2::path
naive_uncomplete(boost::filesystem2::path const p, boost::filesystem2::path const base) {
    using boost::filesystem2::path;
    //using boost::filesystem2::dot;
    //using boost::filesystem2::slash;
#endif


    if (p == base)
        return "./";
        /*!! this breaks stuff if path is a filename rather than a directory,
             which it most likely is... but then base shouldn't be a filename so... */

    path from_path, from_base, output;

    path::iterator path_it = p.begin(),    path_end = p.end();
    path::iterator base_it = base.begin(), base_end = base.end();

    // check for emptiness
    if ((path_it == path_end) || (base_it == base_end))
        throw std::runtime_error("path or base was empty; couldn't generate relative path");

#ifdef WIN32
    // drive letters are different; don't generate a relative path
    if (*path_it != *base_it)
        return p;

    // now advance past drive letters; relative paths should only go up
    // to the root of the drive and not past it
    ++path_it, ++base_it;
#endif

    // Cache system-dependent dot, double-dot and slash strings
    const std::string _dot  = ".";//std::string(1, dot<path>::value);
    const std::string _dots = "..";//std::string(2, dot<path>::value);
    const std::string _sep = "/";//std::string(1, slash<path>::value);

    // iterate over path and base
    while (true) {

        // compare all elements so far of path and base to find greatest common root;
        // when elements of path and base differ, or run out:
        if ((path_it == path_end) || (base_it == base_end) || (*path_it != *base_it)) {

            // write to output, ../ times the number of remaining elements in base;
            // this is how far we've had to come down the tree from base to get to the common root
            for (; base_it != base_end; ++base_it) {
                if (*base_it == _dot)
                    continue;
                else if (*base_it == _sep)
                    continue;

                output /= "../";
            }

            // write to output, the remaining elements in path;
            // this is the path relative from the common root
            path::iterator path_it_start = path_it;
            for (; path_it != path_end; ++path_it) {

                if (path_it != path_it_start)
                    output /= "/";

                if (*path_it == _dot)
                    continue;
                if (*path_it == _sep)
                    continue;

                output /= *path_it;
            }

            break;
        }

        // add directory level to both paths and continue iteration
        from_path /= path(*path_it);
        from_base /= path(*base_it);

        ++path_it, ++base_it;
    }

    return output;
}

#if !defined(__FLASHPLAYER__) && !defined(EMSCRIPTEN)
void writeFileToZip(zipFile& zf, std::string readFileName, std::string fileNameInZip)
{
	//SmartBody::util::log("writeFileToZip, srcFile = %s, fileInZip = %s",readFileName.c_str(),fileNameInZip.c_str());
	const int bufferSize = 16384;
	int size_buf = bufferSize;
	void* buf = NULL; 
	buf = (void*)malloc(size_buf);	
	zip_fileinfo zi;
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =	zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
	FILE* fp = fopen(readFileName.c_str(),"rb");	
	int err = zipOpenNewFileInZip(zf,fileNameInZip.c_str(),&zi,NULL,0,NULL,0,NULL,0,Z_DEFAULT_COMPRESSION);
	int size_read = 0;
	do
	{
		err = ZIP_OK;
		size_read = (int)fread(buf,1,size_buf,fp);
		if (size_read>0)
		{
			err = zipWriteInFileInZip (zf,buf,size_read);
		}

	} while ((err == ZIP_OK) && (size_read>0));
	err = zipCloseFileInZip(zf);
	fclose(fp);
	free(buf);
}

void writeDirToZip(zipFile& zf, std::string sourceFolder, std::string folderNameInZip)
{
#if (BOOST_VERSION > 104400)	
		namespace fs = boost::filesystem;
#else	
		namespace fs = boost::filesystem2;
#endif
	fs::path source(sourceFolder);
	for(
		fs::directory_iterator file(source);
		file != fs::directory_iterator(); ++file
		)
	{
		try
		{
			fs::path current(file->path());
#if (BOOST_VERSION > 104400)
			std::string curFileName = current.filename().string();
#else
			std::string curFileName = current.filename();
#endif
			if(fs::is_directory(current))
			{
				// Found directory: Recursion
				writeDirToZip(zf,current.string(), folderNameInZip + "/" + curFileName);				
			}
			else
			{
				// Found file: Copy
				writeFileToZip(zf, current.string(), folderNameInZip + "/" + curFileName);					
			}
		}
		catch(fs::filesystem_error const & e)
		{
			std:: cerr << e.what() << std::endl;
		}
	}
	
}


void SBScene::exportCharacter( std::string charName, std::string outDir )
{
	SmartBody::SBCharacter* sbChar = getCharacter(charName);
	if (!sbChar) return;
#if (BOOST_VERSION > 104400)	
	namespace fs =  boost::filesystem;
	//using boost::filesystem::dot;
	//using boost::filesystem::slash;
#else	
	namespace fs = boost::filesystem2;
	//using boost::filesystem2::dot;
	//using boost::filesystem2::slash;
#endif
	fs::path newOutPath(outDir);
	if (!fs::exists(newOutPath))
	{
		fs::create_directories(newOutPath);
	}
	std::vector<std::string> motions;
	SrVec scale = sbChar->getVec3Attribute("deformableMeshScale");
	ParserOpenCOLLADA::exportCollada(outDir, sbChar->getSkeleton()->getName(), sbChar->getStringAttribute("deformableMesh"), motions, true, true, false, scale.x);

}

void SBScene::exportScenePackage( std::string outDir, std::string outZipArchiveName )
{
	std::string mediaPath = getMediaPath();
	std::vector<std::string> motionNames = getMotionNames();
	std::vector<std::string> skelNames = getSkeletonNames();	


#if (BOOST_VERSION > 104400)	
	namespace fs =  boost::filesystem;
	//using boost::filesystem::dot;
	//using boost::filesystem::slash;
#else	
	namespace fs = boost::filesystem2;
	//using boost::filesystem2::dot;
	//using boost::filesystem2::slash;
#endif

	bool writeToZip = false;
	if (outZipArchiveName != "")
		writeToZip = true;
		
	fs::path newOutPath(outDir);
	fs::path mePath(mediaPath);
	fs::path tempPath(mePath.parent_path().string()+"/tempExportFiles");
	if (!fs::exists(newOutPath))
	{
		fs::create_directories(newOutPath);
	}

	if (!fs::exists(tempPath))
	{
		fs::create_directories(tempPath);
	}

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string fileString = scene->save(false,"__FILE__");
	std::string initScriptFile = "initScene.py";
	std::string scriptFileLocation = outDir + "/" + initScriptFile;
	std::ofstream file(scriptFileLocation.c_str());
	if (file.good())
	{
		file << fileString;
		file.close();
	}
	else
	{
		SmartBody::util::log("Fail to save scene. Abort export operation");
		file.close();
		return;
	}
	
	zipFile zf = NULL;	
	if (writeToZip)
	{
		zf = zipOpen((outDir+"/"+outZipArchiveName).c_str(),0);		
		writeFileToZip(zf,scriptFileLocation,initScriptFile);
	}		

	SmartBody::SBAssetManager* assetManager = getAssetManager();
	for (unsigned int i=0;i<motionNames.size();i++)
	{
		SmartBody::SBMotion* motion = getMotion(motionNames[i]);
		if (!motion)
		{
			SmartBody::util::log("Motion %s cannot be found.", motionNames[i].c_str());
			continue;
		}
		if (motion->getTransformDepth() > 0) // not an original motion
			continue;

		if (motion->getFullFilePath() == "")
		{
			std::string motionFullPath = tempPath.string()+ "/" + motion->getName() + ".skm";
			SmartBody::util::log("Skeleton %s is not loaded from a file, save the skeleton to temp directory %s",motion->getName().c_str(), motionFullPath.c_str());
			motion->saveToSkm(motionFullPath);
			motion->setFullFilePath(motionFullPath);
		}

		fs::path motionFile = fs::path(motion->getFullFilePath());	
		fs::path motionPath = motionFile.parent_path();
		std::string motionFileExt = fs::extension(motionFile);
		std::string assetLocation = assetManager->findAsset("motion",motion->getName()+motionFileExt);
		
		fs::path diffPath;	
		fs::path outPath;
		diffPath = naive_uncomplete(motionPath,mePath);
		
		if (tempPath == motionPath)
		{
			diffPath = fs::path(defautAssetPath);			
		}
		else if (assetLocation == "")
		{
			fs::path foundPath;
			if (!find_file(mePath,motionFile.filename().string(),foundPath)) // only set to defaultAssetPath if can not find the file under media directory
				diffPath = fs::path(defautAssetPath);			
		}
// 		else if (assetLocation != "")
// 		{
// 			fs::path mePath(mediaPath);
// 			fs::path assetPath(assetLocation);			
// 			diffPath = naive_uncomplete(assetPath,mePath);			
// 		}
		outPath = fs::path(outDir + "/" + diffPath.string());

		if (!writeToZip)
		{
			if (!fs::exists(outPath))
			{
				fs::create_directories(outPath);
			}
#if (BOOST_VERSION > 104400)
			std::string newFileName = outPath.string()+"/"+motionFile.filename().string();
#else
			std::string newFileName = outPath.string()+"/"+motionFile.filename();
#endif
			if (!fs::exists(newFileName))
				fs::copy_file(motionFile,fs::path(newFileName), fs::copy_option::none);
		}
		else
		{
#if (BOOST_VERSION > 104400)
			std::string zipFileName = diffPath.string()+"/"+motionFile.filename().string();
#else
			std::string zipFileName = diffPath.string()+"/"+motionFile.filename();
#endif			
			writeFileToZip(zf, motionFile.string(), zipFileName);
		}	
	}

	for (unsigned int i=0;i<skelNames.size();i++)
	{
		SmartBody::SBSkeleton* skel = getSkeleton(skelNames[i]);
		if (!skel)
		{
			SmartBody::util::log("Skeleton %s cannot be found.", skelNames[i].c_str());
			continue;
		}	
		
		if (skel->getFullFilePath() == "")
		{
			std::string skelFullPath = tempPath.string()+ "/" + skel->getName();
			SmartBody::util::log("Skeleton %s is not loaded from a file, save the skeleton to temp directory %s",skel->getName().c_str(), skelFullPath.c_str());
			skel->save(skelFullPath);
			skel->setFullFilePath(skelFullPath);
		}
				
		fs::path skelFile = fs::path(skel->getFullFilePath());
		fs::path skelDir = skelFile.parent_path();		
		std::string assetLocation = assetManager->findAsset("motion",skel->getName());
		fs::path diffPath;	
		fs::path outPath;
		diffPath = naive_uncomplete(skelDir,mePath);
		//SmartBody::util::log("diffPath after uncomplete = '%s'",diffPath.string().c_str());
		if (tempPath == skelDir)
		{
			diffPath = fs::path(defautAssetPath);			
		}
		else if (assetLocation == "")
		{
			fs::path foundPath;
			if (!find_file(mePath,skelFile.filename().string(),foundPath)) // only set to defaultAssetPath if can not find the file under media directory
				diffPath = fs::path(defautAssetPath);			
		}
// 		else if (assetLocation != "")
// 		{
// 			fs::path mePath(mediaPath);
// 			fs::path assetPath(assetLocation);			
// 			diffPath = naive_uncomplete(assetPath,mePath);			
// 		}
		//SmartBody::util::log("final diffpath = '%s'",diffPath.string().c_str());
		outPath = fs::path(outDir + "/" + diffPath.string());
		//SmartBody::util::log("skeleton = %s, file = %s, outpath = %s", skel->getName().c_str(), skelFile.string().c_str(), outPath.string().c_str());
		if (!writeToZip)
		{
			if (!fs::exists(outPath))
			{
				fs::create_directories(outPath);
			}
#if (BOOST_VERSION > 104400)
			std::string newFileName = outPath.string()+"/"+skelFile.filename().string();
#else
			std::string newFileName = outPath.string()+"/"+skelFile.filename();
#endif
			if (!fs::exists(newFileName))
				fs::copy_file(skelFile,fs::path(newFileName), fs::copy_option::none);
		}
		else
		{
#if (BOOST_VERSION > 104400)
			std::string zipFileName = diffPath.string()+"/"+skelFile.filename().string();
#else
			std::string zipFileName = diffPath.string()+"/"+skelFile.filename();
#endif			
			writeFileToZip(zf, skelFile.string(), zipFileName);
		}	
	}
	//SmartBody::util::log("finish copying skeleton files");
	std::vector<std::string> deformableMeshNames = assetManager->getDeformableMeshNames();
	for (unsigned int i=0;i<deformableMeshNames.size();i++)
	{
		std::string meshName = deformableMeshNames[i];
		DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
		if (!mesh)
		{
			SmartBody::util::log("Mesh %s cannot be found.", meshName.c_str());
			continue;
		}
		
		std::string meshBaseName = boost::filesystem::basename(meshName);

		if (mesh->getFullFilePath() == "") // the mesh is not loaded from a file, so write it to a temp directory first
		//if (true)
		{
			std::vector<std::string> moNames;
			std::string meshSkName = mesh->skeletonName;
			std::string exportMeshFullPath = tempPath.string() + "/" + meshBaseName;
			fs::path meshPath(exportMeshFullPath);
			if (!fs::exists(meshPath))
			{
				fs::create_directories(meshPath);
			}
			SmartBody::util::log("mesh %s is not loaded from a file. Export the mesh to %s",meshName.c_str(),exportMeshFullPath.c_str());
			ParserOpenCOLLADA::exportCollada(exportMeshFullPath,meshSkName,meshName,moNames,true,true,false, 1.0);
			//SmartBody::util::log("finish exporting mesh as Collada files");
			mesh->setFullFilePath(exportMeshFullPath +  "/" + meshName);
			//continue;
		}

		fs::path meshFile = fs::path(mesh->getFullFilePath());
		fs::path meshDir = meshFile.parent_path();		
		std::string assetLocation = assetManager->findAsset("mesh",meshName);
		fs::path diffPath, outPath;	

		diffPath = naive_uncomplete(meshDir,mePath);

		if (tempPath == meshDir.parent_path())
		{
			diffPath = fs::path(defautAssetPath + "/" + meshBaseName);			
		}
		else if (assetLocation == "")
		{
			fs::path foundPath;
			if (!find_file(mePath,meshFile.filename().string(),foundPath)) // only set to defaultAssetPath if can not find the file under media directory
				diffPath = fs::path(defautAssetPath + "/" + meshBaseName);			
		}	
		
		outPath = fs::path(outDir + "/" + diffPath.string());

		if (!writeToZip)
		{
			copyDir(meshDir,outPath);						
		}
		else
		{
			writeDirToZip(zf,meshDir.string(),diffPath.string());
		}
	}
	//SmartBody::util::log("finish copying deformable mesh files");
	/*
	const std::vector<std::string>& characters = getCharacterNames();
	for (unsigned int i=0;i<characters.size();i++)
	{
		std::string charName = characters[i];
		SmartBody::SBCharacter* sbChar = getCharacter(charName);
		if (!sbChar) continue; 

		std::string meshName = sbChar->getStringAttribute("deformableMesh");		
		if (meshName == "") // otherwise we will copy all mesh directories ...
			continue;
		boost::filesystem::path path(meshName);
		std::string meshDir = boost::filesystem::basename(path);

		std::vector<std::string> meshPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("mesh");
		bool hasMeshPath = false;
		for (size_t m = 0; m < meshPaths.size(); m++)
		{
			std::string meshPath = meshPaths[m];
			fs::path curpath( meshPath + "/" + meshDir );
			if (!boost::filesystem::is_directory(curpath))
			{

				// should we write out the mesh instead ?
				//continue;
			}
			//curpath /= std::string(meshDir);	

			fs::path mePath(mediaPath);		
			fs::path diffPath = naive_uncomplete(curpath,mePath);
			fs::path newMeshPath(outDir+"/"+diffPath.string());

			//SmartBody::util::log("curPath = %s, newMeshPath = %s", curpath.string().c_str(), newMeshPath.string().c_str());
			//path targetPath()
			// copy dir
			hasMeshPath = true;
			if (!writeToZip)
			{
				copyDir(curpath,newMeshPath);						
			}
			else
			{
				writeDirToZip(zf,curpath.string(),diffPath.string());
			}
		}
	}
	*/

#if 0
	// save motions
	//SmartBody::util::log("Num of motionNames = %d",motionNames.size());
	for (unsigned int i=0;i<motionNames.size();i++)
	{
		SmartBody::SBMotion* motion = getMotion(motionNames[i]);
		//SmartBody::util::log("motionName = %s",motionNames[i].c_str());

		if (!motion)
		{
			SmartBody::util::log("Motion %s cannot be found.", motionNames[i].c_str());
			continue;
		}
	
		fs::path motionFile(motion->filename());			
		fs::path motionPath = motionFile.parent_path();
		//SmartBody::util::log("motion %d, motionName = %s, filename = %s",motionNames[i].c_str(), motion->filename().c_str());
		if (motionPath.empty()) // don't care about empty path
			continue;
		if (motion->getTransformDepth() > 0) // not an original motion
			continue;

		fs::path mePath(mediaPath);
		fs::path diffPath = naive_uncomplete(motionPath,mePath);

		if (diffPath.is_absolute()) // the target path is not under mePath
		{			
			diffPath = fs::path("Assets");
		}
		fs::path newPath(outDir+"/"+diffPath.string());	
		
		//SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s, filename = %s", motionFile.directory_string().c_str(), mePath.native_directory_string().c_str(), diffPath.string().c_str(), motionFile.filename().c_str());		
		//SmartBody::util::log("new Path = %s, newFileName = %s",newPath.string().c_str(), newFileName.c_str());
		//motion->saveToSkm(newFileName);
		if (!writeToZip)
		{
			if (!fs::exists(newPath))
			{
				fs::create_directories(newPath);
			}
#if (BOOST_VERSION > 104400)
			std::string newFileName = newPath.string()+"/"+motionFile.filename().string();
#else
			std::string newFileName = newPath.string()+"/"+motionFile.filename();
#endif
			fs::copy_file(motionFile,fs::path(newFileName), fs::copy_option::none);
		}
else
		{
#if (BOOST_VERSION > 104400)
			std::string zipFileName = diffPath.string()+"/"+motionFile.filename().string();
#else
			std::string zipFileName = diffPath.string()+"/"+motionFile.filename();
#endif			
			writeFileToZip(zf, motionFile.string(), zipFileName);
		}		
	}

	// save skeletons
	for (unsigned int i=0;i<skelNames.size();i++)
	{
		SmartBody::SBSkeleton* skel = getSkeleton(skelNames[i]);
		fs::path skelFile(skel->getFileName());			
		fs::path skelPath = skelFile.parent_path();
		if (skelPath.empty()) // don't care about empty path
			continue;

		fs::path mePath(mediaPath);
		fs::path diffPath = naive_uncomplete(skelPath,mePath);
		if (diffPath.is_absolute()) // the target path is not under mePath
		{			
			diffPath = fs::path("Assets");
		}
		fs::path newPath(outDir+"/"+diffPath.string());		

		  //SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s", skelPath.directory_string().c_str(), mePath.directory_string().c_str(), diffPath.directory_string().c_str());
		//skel->save(newFileName);
		//copy_file(path(skelFile),path(newFileName), fs::copy_option::none);

		if (!writeToZip)
		{	
			if (!fs::exists(newPath))
			{
				fs::create_directories(newPath);
			}
#if (BOOST_VERSION > 104400)
			std::string newFileName = newPath.string()+"/"+skelFile.filename().string();
#else
			std::string newFileName = newPath.string()+"/"+skelFile.filename();
#endif

			fs::copy_file(skelFile,fs::path(newFileName), fs::copy_option::none);
		}
		else
		{
#if (BOOST_VERSION > 104400)
			std::string zipFileName = diffPath.string()+"/"+skelFile.filename().string();
#else
			std::string zipFileName = diffPath.string()+"/"+skelFile.filename();
#endif			
			writeFileToZip(zf, skelFile.string(), zipFileName);
		}		
	}
	
#if 1
	// save character meshes
	const std::vector<std::string>& characters = getCharacterNames();
	for (unsigned int i=0;i<characters.size();i++)
	{
		std::string charName = characters[i];
		SmartBody::SBCharacter* sbChar = getCharacter(charName);
		if (!sbChar) continue; 
		
		std::string meshName = sbChar->getStringAttribute("deformableMesh");		
		if (meshName == "") // otherwise we will copy all mesh directories ...
			continue;
		boost::filesystem::path path(meshName);
		std::string meshDir = boost::filesystem::basename(path);
		
		std::vector<std::string> meshPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("mesh");
		bool hasMeshPath = false;
		for (size_t m = 0; m < meshPaths.size(); m++)
		{
			std::string meshPath = meshPaths[m];
			fs::path curpath( meshPath + "/" + meshDir );
			if (!boost::filesystem::is_directory(curpath))
			{

				// should we write out the mesh instead ?
				//continue;
			}
			//curpath /= std::string(meshDir);	

			fs::path mePath(mediaPath);		
			fs::path diffPath = naive_uncomplete(curpath,mePath);
			fs::path newMeshPath(outDir+"/"+diffPath.string());
			
			//SmartBody::util::log("curPath = %s, newMeshPath = %s", curpath.string().c_str(), newMeshPath.string().c_str());
			//path targetPath()
			// copy dir
			hasMeshPath = true;
			if (!writeToZip)
			{
				copyDir(curpath,newMeshPath);						
			}
			else
			{
				writeDirToZip(zf,curpath.string(),diffPath.string());
			}
		}
	}
#else
	std::vector<std::string> meshNames = assetManager->getDeformableMeshNames();
	for (unsigned int i=0;i<meshNames.size();i++)
	{
		std::string meshName = meshNames[i];
		DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
		std::vector<std::string> meshPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("mesh");
		bool hasMeshPath = false;
		fs::path curpath, curMeshPath;
		for (size_t m = 0; m < meshPaths.size(); m++)
		{
			std::string meshPath = meshPaths[m];
			curpath = fs::path( meshPath + "/" + meshName );			
			if (boost::filesystem::is_directory(curpath))
			{	
				curMeshPath = curpath;
				hasMeshPath = true;
				break;
			}
		}	

		if (!hasMeshPath) // file does not exist
		{
			curMeshPath = fs::path(getMediaPath()+ "/temp/" + meshName); // create a temp directory to write out the mesh
			fs::create_directory(curMeshPath);
			ParserOgre::exportOgreXMLMesh(mesh, meshName, curMeshPath.string());
		}

		fs::path mePath(mediaPath);		
		fs::path diffPath = naive_uncomplete(curpath,mePath);
		fs::path newMeshPath(outDir+"/"+diffPath.string());

		if (!writeToZip)
		{
			copyDir(curMeshPath,newMeshPath);						
		}
		else
		{
			writeDirToZip(zf,curMeshPath.string(),diffPath.string());
		}
	}
#endif
#endif

	if (writeToZip)
	{
		int errClose = zipClose(zf,NULL);
		fs::path initScrptPath(scriptFileLocation);
		if (fs::exists(initScrptPath))
		{
			fs::remove(initScrptPath);			
		}
	}

	if (fs::exists(tempPath)) 
	{
		fs::remove_all(tempPath);
	}
}
#endif
void SBScene::saveAssets(std::stringstream& strstr, bool remoteSetup, std::string outMediaPath)
{
	strstr << "# -------------------- media and asset paths\n";
	// mediapath
	strstr << "print \"Save Assets\"\n";
	std::string systemMediaPath = getMediaPath();
	if (outMediaPath == "__FILE__")
	{
		strstr << "import inspect\n";
		strstr << "scriptFilename = inspect.getframeinfo(inspect.currentframe()).filename\n";
		strstr << "fileMediaPath = os.path.dirname(os.path.abspath(scriptFilename))\n";
		strstr << "scene.setMediaPath(fileMediaPath)\n";
	}
	else if (outMediaPath != "")
	{
		strstr << "scene.setMediaPath(\"" << outMediaPath << "\")\n";
	}
	else
	{
		strstr << "scene.setMediaPath(\"" << systemMediaPath << "\")\n";
	}
		
	// asset paths
	std::vector<std::string>::iterator iter;
	std::vector<std::string> motionPaths = getLocalAssetPaths("motion");
	std::vector<std::string> motionNames = getMotionNames();
	std::vector<std::string> skelNames = getSkeletonNames();
	std::set<std::string> extraAssetPathSet;

	//exportScenePackage("../../../../temp/","test1.zip");

#if (BOOST_VERSION > 104400)
    using boost::filesystem::path;
    //using boost::filesystem::dot;
    //using boost::filesystem::slash;
#else
    using boost::filesystem2::path;
    //using boost::filesystem2::dot;
    //using boost::filesystem2::slash;
#endif


	SmartBody::SBAssetManager* assetManager = getAssetManager();
	// feng : since we may have use "loadAssetsFromPath" to load the motions, we should infer all other motion paths from existing motions
	for (unsigned int i=0;i<motionNames.size();i++)
	{
		SmartBody::SBMotion* motion = getMotion(motionNames[i]);	
		path motionFile(motion->filename());			
		path motionPath = motionFile.parent_path();
		if (motionPath.empty()) // don't care about empty path
			continue;
		std::string ext = boost::filesystem::extension(motionFile);
		std::string base = boost::filesystem::basename(motionFile);
		std::string assetName = base+ext;
		if (remoteSetup)
		{
			continue;
		}
		std::string assetExist = assetManager->findAsset("motion",assetName);
		path mePath(systemMediaPath);
		path diffPath = naive_uncomplete(motionPath,mePath);
		if (diffPath.is_absolute()) // the path is not under media path, skip
		{
			continue;
		}
		//SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s", motionFile.directory_string().c_str(), mePath.string().c_str(), diffPath.string().c_str());
		std::vector<std::string>::iterator st = std::find(motionPaths.begin(),motionPaths.end(),diffPath.string());
		//if (st == motionPaths.end())
		if (assetExist == "") // can not find this asset in the motion path
		{
			extraAssetPathSet.insert(diffPath.string());
		}
	}

	for (unsigned int i=0;i<skelNames.size();i++)
	{
		SmartBody::SBSkeleton* skel = getSkeleton(skelNames[i]);
		path skelFile(skel->getFileName());			
		path skelPath = skelFile.parent_path();
		if (skelPath.empty()) // don't care about empty path
			continue;

		if (remoteSetup)
		{
			continue;
		}

		std::string ext = boost::filesystem::extension(skelFile);
		std::string base = boost::filesystem::basename(skelFile);
		std::string assetName = base+ext;
		std::string assetExist = assetManager->findAsset("motion",assetName);
		path mePath(systemMediaPath);
		path diffPath = naive_uncomplete(skelPath,mePath);
		//SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s", skelPath.directory_string().c_str(), mePath.directory_string().c_str(), diffPath.directory_string().c_str());
		if (diffPath.is_absolute()) // the path is not under media path, skip
		{
			continue;
		}
		std::vector<std::string>::iterator st = std::find(motionPaths.begin(),motionPaths.end(),diffPath.string());
		//if (st == motionPaths.end())
		if (assetExist == "") // can not find this asset in the skeleton path
		{
			extraAssetPathSet.insert(diffPath.string());
		}
	}

	for (iter = motionPaths.begin(); iter != motionPaths.end(); iter++)
	{ 
		const std::string& path = (*iter);
		strstr << "scene.addAssetPath(\"motion\", \"" << path << "\")\n";
	}

	std::vector<std::string> scriptPaths = getLocalAssetPaths("script");
	for (iter = scriptPaths.begin(); iter != scriptPaths.end(); iter++)
	{
		const std::string& path = (*iter);
		strstr << "scene.addAssetPath(\"script\", \"" << path << "\")\n";
	}

	std::vector<std::string> audioPaths = getLocalAssetPaths("audio");
	for (iter = audioPaths.begin(); iter != audioPaths.end(); iter++)
	{
		const std::string& path = (*iter);
		strstr << "scene.addAssetPath(\"audio\", \"" << path << "\")\n";
	}

	std::vector<std::string> meshPaths = getLocalAssetPaths("mesh");
	for (iter = meshPaths.begin(); iter != meshPaths.end(); iter++)
	{
		const std::string& path = (*iter);
		strstr << "scene.addAssetPath(\"mesh\", \"" << path << "\")\n";
	}
	if (remoteSetup) // to-do : different treatment when saving setup script for remote connection
	{
		// need to go through all skeleton, and explicitly create those skeletons from script
		std::vector<std::string> skeletonNames = getSkeletonNames();
		const std::vector<std::string>& charNames = getCharacterNames();
		std::map<std::string, std::string> charSkelMap;

		for (unsigned int i=0;i<skeletonNames.size();i++)
		{
			std::string skelName = skeletonNames[i];
			SBSkeleton* skel = getSkeleton(skelName);
			if (skel)
			{
				std::string skelStr = skel->saveToString();
				strstr << "tempSkel = scene.addSkeletonDefinition(\"" << skelName << "\")\n";	
				std::string skelSaveStr = skel->saveToString();
				//skelSaveStr.replace('\n',)
				boost::replace_all(skelSaveStr,"\n","\\n");
				boost::replace_all(skelSaveStr,"\"","");
				SmartBody::util::log("Skeleton %s :\n%s",skelName.c_str(),skelSaveStr.c_str());
				strstr << "tempSkel.loadFromString(\"" << skelSaveStr << "\")\n";
				charSkelMap[skelName] = skelName;
			}			
		}

		for (unsigned int i=0;i<charNames.size(); i++)
		{
			std::string charName = charNames[i];
			SBCharacter* sbChar = getCharacter(charName);
			if (!sbChar)
				continue;
			std::string skelName = sbChar->getSkeleton()->getName();
			SBSkeleton* skel = getSkeleton(skelName);
			if (skel && charSkelMap.find(skelName) == charSkelMap.end())
			{
				std::string skelStr = skel->saveToString();
				strstr << "tempSkel = scene.addSkeletonDefinition(\"" << skelName << "\")\n";	
				std::string skelSaveStr = skel->saveToString();
				//skelSaveStr.replace('\n',)
				boost::replace_all(skelSaveStr,"\n","\\n");
				boost::replace_all(skelSaveStr,"\"","");
				//SmartBody::util::log("Skeleton %s :\n%s",skelName.c_str(),skelSaveStr.c_str());
				SmartBody::util::log("Skeleton %s :\n",skelName.c_str());
				strstr << "tempSkel.loadFromString(\"" << skelSaveStr << "\")\n";
				charSkelMap[skelName] = charName;
			}
		}

		std::vector<std::string> motionNames = getMotionNames();
		for (unsigned int i=0;i<motionNames.size();i++)
		{
			std::string moName = motionNames[i];
			SBMotion* motion = getMotion(moName);
			if (motion)
			{
				// add motion definition
				strstr << "tempMotion = scene.createMotion(\"" << moName << "\")\n";	
				strstr << "tempMotion.setEmptyMotion(" << motion->getDuration() << ", " << motion->getNumFrames() << ")\n";
				// add sync points
				strstr << "tempMotion.setSyncPoint(\"start\"," << motion->getTimeStart() <<")\n";
				strstr << "tempMotion.setSyncPoint(\"ready\"," << motion->getTimeReady() <<")\n";
				strstr << "tempMotion.setSyncPoint(\"stroke_start\"," << motion->getTimeStrokeStart() <<")\n";
				strstr << "tempMotion.setSyncPoint(\"stroke\"," << motion->getTimeStroke() <<")\n";
				strstr << "tempMotion.setSyncPoint(\"stroke_stop\"," << motion->getTimeStrokeEnd() <<")\n";
				strstr << "tempMotion.setSyncPoint(\"relax\"," << motion->getTimeRelax() <<")\n";
				strstr << "tempMotion.setSyncPoint(\"stop\"," << motion->getTimeStop() <<")\n";				
				// add meta data tag
				BOOST_FOREACH(const std::string& tagName, motion->getMetaDataTags())
				{
					strstr << "tempMotion.addMetaData(\"" << tagName << "\",\"" << motion->getMetaDataString(tagName) << "\")\n";
				}
			}
		}
	}
	else // loading the actual assets
	{
		std::set<std::string>::iterator si;
		for ( si  = extraAssetPathSet.begin();
			si != extraAssetPathSet.end();
			si++)
		{
			const std::string& path = (*si);
			strstr << "scene.loadAssetsFromPath(\"" << path << "\")\n";
		}
		// load the extra assets in asset directory
		strstr << "scene.loadAssetsFromPath(\"" << defautAssetPath << "\")\n";
		strstr << "# -------------------- load assets\n";
		strstr << "scene.loadAssets()\n";
	}	

	// this is a hack to save joint map here, should probably move motion transformation into another function ?
	saveJointMaps(strstr, remoteSetup);
	// create any mirrored assets
	std::vector<std::string> motions = this->getMotionNames();
#if 0	
	for (std::vector<std::string>::iterator iter = motions.begin();
		 iter != motions.end();
		 iter++)
	{
		SBMotion* mirroredMotion = this->getMotion(*iter);
		StringAttribute* mirroredMotionAttr = dynamic_cast<StringAttribute*>(mirroredMotion->getAttribute("mirrorMotion"));	
		if (mirroredMotionAttr) // the motion is generated from mirroring
		{
			strstr << "motion = scene.getMotion(\"" << mirroredMotionAttr->getValue() << "\")\n";
			// make sure the skeleton exists
			StringAttribute* mirroredSkeletonAttr = dynamic_cast<StringAttribute*>(mirroredMotion->getAttribute("mirrorSkeleton"));			
			if (mirroredSkeletonAttr) 
			{
				const std::string& skeletonName = mirroredSkeletonAttr->getValue();
				strstr << "mirrorSkeleton = scene.getSkeleton(\"" << skeletonName << "\")\n";
				strstr << "if mirrorSkeleton is not None:\n";
				strstr << "\tmirroredMotion = motion.mirror(\"" << mirroredMotion->getName() << "\", \"" << skeletonName << "\")\n";
			}			
		}			
	}
#else
	std::vector<SmartBody::SBMotion*> motionList;
	for (unsigned int i=0;i<motions.size();i++)
		motionList.push_back(getMotion(motions[i]));
	// sort motion array according to its transformation depth level
	std::sort(motionList.begin(),motionList.end(),SmartBody::motionComp);
	// process motion from low depth to high depth to capture all dependency of motion operations

	for (unsigned int i=0;i<motionList.size();i++)
	{
		SBMotion* motion = motionList[i];
		StringAttribute* mirroredMotionAttr = dynamic_cast<StringAttribute*>(motion->getAttribute("mirrorMotion"));	
		StringAttribute* smoothedMotionAttr = dynamic_cast<StringAttribute*>(motion->getAttribute("smoothMotion"));	
		if (mirroredMotionAttr)
		{
			strstr << "motion = scene.getMotion(\"" << mirroredMotionAttr->getValue() << "\")\n";
			// make sure the skeleton exists
			StringAttribute* mirroredSkeletonAttr = dynamic_cast<StringAttribute*>(motion->getAttribute("mirrorSkeleton"));			
			if (mirroredSkeletonAttr) 
			{
				const std::string& skeletonName = mirroredSkeletonAttr->getValue();
				strstr << "mirrorSkeleton = scene.getSkeleton(\"" << skeletonName << "\")\n";
				strstr << "if mirrorSkeleton is not None:\n";
				strstr << "\tmirroredMotion = motion.mirror(\"" << motion->getName() << "\", \"" << skeletonName << "\")\n";
			}
		}
		else if (smoothedMotionAttr)
		{
			strstr << "motion = scene.getMotion(\"" << smoothedMotionAttr->getValue() << "\")\n";
			DoubleAttribute* smoothIntervalAttr = dynamic_cast<DoubleAttribute*>(motion->getAttribute("smoothInterval"));
			if (smoothIntervalAttr)
			{
				strstr << "smoothMotion = motion.smoothCycle(\"" << motion->getName() << "\", " << boost::lexical_cast<std::string>(smoothIntervalAttr->getValue()) << ")\n";
			}
		}
	}

	std::vector<SBSkeleton*> skeletonList;	
	for (unsigned int i=0;i<skelNames.size();i++)
	{
		std::string skName = skelNames[i];
		SBSkeleton* sbSk = getSkeleton(skName);
		// rescale the skeleton to right size if necessary
		strstr << "sbSk = scene.getSkeleton(\"" << skName << "\")\n";
		//strstr << "print 'numSkeleton = ' + str(scene.getNumSkeletons())\n";
		strstr << "if sbSk != None:\n";
		strstr << "\tsbSk.rescale(" << boost::lexical_cast<std::string>(sbSk->getScale()) << ")\n";
	}
	
#endif

	for (std::vector<std::string>::iterator iter = motions.begin();
		iter != motions.end();
		iter++)
	{
		SBMotion* motion = this->getMotion(*iter);
		strstr << "tempMotion = scene.getMotion(\"" << *iter << "\")\n";
		strstr << "tempMotion.setMotionSkeletonName(\"" << motion->getMotionSkeletonName() <<"\")\n";	
	}
	

}
void SBScene::saveCameras(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- cameras\n";
	strstr << "print \"Save Cameras\"\n";
	// save all default cameras
	std::vector<std::string> cameras = getCameraNames();
	for (std::vector<std::string>::iterator cameraIter = cameras.begin();
		cameraIter != cameras.end();
		cameraIter++)
	{
		SrCamera* camera = getCamera((*cameraIter));
		strstr << "obj = scene.getCamera(\"" << camera->getName() << "\")\n";
		strstr << "if obj == None:\n";
		strstr << "\tobj = scene.createCamera(\"" << camera->getName() << "\")\n";		
		strstr << "obj.setEye(" << camera->getEye().x << ", " << camera->getEye().y << ", " << camera->getEye().z << ")\n";
		strstr << "obj.setCenter(" << camera->getCenter().x << ", " << camera->getCenter().y << ", " << camera->getCenter().z << ")\n";
		strstr << "obj.setUpVector(SrVec(" << camera->getUpVector().x << ", " << camera->getUpVector().y << ", " << camera->getUpVector().z << "))\n";
		strstr << "obj.setScale(" << camera->getScale() << ")\n";
		strstr << "obj.setFov(" << camera->getFov() << ")\n";
		strstr << "obj.setFarPlane(" << camera->getFarPlane() << ")\n";
		strstr << "obj.setNearPlane(" << camera->getNearPlane() << ")\n";
		strstr << "obj.setAspectRatio(" << camera->getAspectRatio() << ")\n";

		std::vector<std::string> attributeNames = camera->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			iter != attributeNames.end();
			iter++)
		{
			SmartBody::SBAttribute* attr = camera->getAttribute((*iter));
			if (!attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}
	}	
}

void SBScene::savePawns(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- pawns and characters\n";
	strstr << "print \"Save Pawns\"\n";
	// pawns
	const std::vector<std::string>& pawns = getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		 pawnIter != pawns.end();
		 pawnIter++)
	{
		SBPawn* pawn = getPawn((*pawnIter));
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		SmartBody::SBCharacter* sbChar = dynamic_cast<SmartBody::SBCharacter*>(pawn);
		if (sbChar) // we will handle character in saveCharacters
			continue; 
		if (camera)
			continue; // already wrote out pawns
		if (pawn->getName().find("light") == 0)
		{
			// already wrote lights
			continue;
		}
		strstr << "\n# ---- pawn: " << pawn->getName() << "\n";
		strstr << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		strstr << "if obj == None:\n";
		strstr << "\tobj = scene.createPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		strstr << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrQuat orientation = pawn->getOrientation();
		strstr << "obj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << "))\n";
		// attributes
		std::vector<std::string> attributeNames = pawn->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			 iter != attributeNames.end();
			 iter++)
		{
			SmartBody::SBAttribute* attr = pawn->getAttribute((*iter));
			if (!attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}	
	}
}

void SBScene::savePositions(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- pawn positions\n";
	strstr << "print \"Save Positions\"\n";
	const std::vector<std::string>& pawns = getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		 pawnIter != pawns.end();
		 pawnIter++)
	{
		SBPawn* pawn = getPawn((*pawnIter));
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
			continue; // already wrote out pawns
		if (pawn->getName().find("light") == 0)
		{
			// already wrote lights
			continue;
		}
		strstr << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		SrQuat orientation = pawn->getOrientation();
		strstr << "if obj is not None:\n";
		strstr << "\tobj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		strstr << "\tobj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ". " << orientation.z << "))\n";
	}

	strstr << "# -------------------- character positions\n";
	const std::vector<std::string>& characters = getCharacterNames();
	for (std::vector<std::string>::const_iterator characterIter = characters.begin();
			characterIter != characters.end();
			characterIter++)
	{
		SBCharacter* character = getCharacter((*characterIter));
		SrVec position = character->getPosition();
		SrQuat orientation = character->getOrientation();
		strstr << "obj = scene.getCharacter(\"" << character->getName() << "\", \"" << character->getType() << "\")\n";
		strstr << "if obj is not None:\n";
		strstr << "\tobj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		strstr << "\tobj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ". " << orientation.z << "))\n";
	}
}

void SBScene::saveCharacters(std::stringstream& strstr, bool remoteSetup)
{
	// characters
	strstr << "print \"Save Characters\"\n";
	const std::vector<std::string>& characters = getCharacterNames();
	for (std::vector<std::string>::const_iterator characterIter = characters.begin();
		 characterIter != characters.end();
		 characterIter++)
	{
		SBCharacter* character = getCharacter((*characterIter));
		strstr << "\n# ---- character: " << character->getName() << "\n";
		strstr << "obj = scene.getCharacter(\"" << character->getName() << "\")\n";
		strstr << "if obj == None:\n";
		strstr << "\tobj = scene.createCharacter(\"" << character->getName() << "\", \"" << character->getType() << "\")\n";
		strstr << "print 'character skeleton rescale'\n";
		strstr << "skeleton = scene.createSkeleton(\"" << character->getSkeleton()->getName() << "\")\n";
		strstr << "skeleton.rescale(" <<  character->getSkeleton()->getScale() << ")\n";
		strstr << "obj.setSkeleton(skeleton)\n";
		SrVec position = character->getPosition();
		strstr << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrQuat orientation = character->getOrientation();
		strstr << "obj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << "))\n";

		// face definition
		SBFaceDefinition* faceDef = character->getFaceDefinition();
		if (faceDef)
			strstr << "obj.setFaceDefinition(scene.getFaceDefinition(\"" << faceDef->getName() << "\"))\n";

		// controllers
		strstr << "obj.createStandardControllers()\n";
		// attributes
		std::vector<std::string> attributeNames = character->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			 iter != attributeNames.end();
			 iter++)
		{
			SmartBody::SBAttribute* attr = character->getAttribute((*iter));
			if (!attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}

		// reach
		strstr << "# -------------------- reaching for character " << character->getName() << "\n";
		// reach
		SBReachManager* reachManager = this->getReachManager();
		SBReach* reach = character->getReach();
		if (reach)
		{
			strstr << "reach = scene.getReachManager().createReach(\"" << character->getName() << "\")\n";
			std::string interpType = reach->getInterpolatorType();
			strstr << "reach.setInterpolatorType(\"" << interpType << "\")\n";
			// motions
			const std::vector<std::string>& leftMotionNames = reach->getMotionNames("left");
			for (std::vector<std::string>::const_iterator iter = leftMotionNames.begin();
				 iter != leftMotionNames.end();
				 iter++)
			{
				strstr << "reach.addMotion(\"left\", scene.getMotion(\"" << (*iter) << "\"))\n";
			}
			const std::vector<std::string>& rightMotionNames = reach->getMotionNames("right");
			for (std::vector<std::string>::const_iterator iter = rightMotionNames.begin();
				 iter != rightMotionNames.end();
				 iter++)
			{
				strstr << "reach.addMotion(\"right\", scene.getMotion(\"" << (*iter) << "\"))\n";
			}
			// point hand
			SBMotion* m = NULL;
			m = reach->getPointHandMotion("left");
			if (m)
			{
				strstr << "reach.setPointHandMotion(\"left\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			m = reach->getPointHandMotion("right");
			if (m)
			{
				strstr << "reach.setPointHandMotion(\"right\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			// grab hand
			m = reach->getGrabHandMotion("left");
			if (m)
			{
				strstr << "reach.setGrabHandMotion(\"left\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			m = reach->getGrabHandMotion("right");
			if (m)
			{
				strstr << "reach.setGrabHandMotion(\"right\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			// release hand
			m = reach->getReleaseHandMotion("left");
			if (m)
			{
				strstr << "reach.setReleaseHandMotion(\"left\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			m = reach->getReleaseHandMotion("right");
			if (m)
			{
				strstr << "reach.setReleaseHandMotion(\"right\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			// reach hand
			m = reach->getReachHandMotion("left");
			if (m)
			{
				strstr << "reach.setReachHandMotion(\"left\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			m = reach->getReachHandMotion("right");
			if (m)
			{
				strstr << "reach.setReachHandMotion(\"right\", scene.getMotion(\"" << m->getName() << "\"))\n";
			}
			strstr << "reach.build(scene.getCharacter(\"" << character->getName() << "\"))\n";
		}
		else
		{
			strstr << "# -- no reach\n";
		}

		// steering
		strstr << "# --- steering for character " << character->getName() << "\n";
		SBSteerManager* steerManager = this->getSteerManager();
		SBSteerAgent* steerAgent = steerManager->getSteerAgent(character->getName());
		if (steerAgent)
		{
			strstr << "steeragent = scene.getSteerManager().getSteerAgent(\"" << character->getName() << "\")\n";
			strstr << "if steeragent is None:\n";
			strstr << "\tsteeragent = scene.getSteerManager().createSteerAgent(\"" << character->getName() << "\")\n";
			strstr << "\tsteeragent.setSteerStateNamePrefix(\"" << steerAgent->getSteerStateNamePrefix() << "\")\n";
			strstr << "\tsteeragent.setSteerType(\"" << steerAgent->getSteerType() << "\")\n";
		}
		else
		{
			strstr << "# --- no steering for character " << character->getName() << "\n";
		}
		// set character posture
		strstr << "bml.execBML('" << character->getName() << "', '<body posture=\"" << character->getPostureName() <<"\"/>')\n";
	}

	// enable steering
	if (this->getSteerManager()->getSteerAgents().size() > 0)
	{
		strstr << "scene.getServiceManager().getService(\"steering\").setBoolAttribute(\"enable\", True)\n";
	}
}

void SBScene::saveLights(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- lights\n";
	strstr << "print \"Save Lights\"\n";
	// lights
	const std::vector<std::string>& pawns = getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		 pawnIter != pawns.end();
		 pawnIter++)
	{
		SBPawn* pawn = getPawn((*pawnIter));
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
			continue; 
		if (pawn->getName().find("light") != 0)
		{
			continue;
		}
		strstr << "\n# ---- light: " << pawn->getName() << "\n";
		strstr << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		strstr << "if obj == None:\n";
		strstr << "\tobj = scene.createPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		strstr << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrQuat orientation = pawn->getOrientation();
		strstr << "obj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << "))\n";

		// attributes
		std::vector<std::string> attributeNames = pawn->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			 iter != attributeNames.end();
			 iter++)
		{
			SmartBody::SBAttribute* attr = pawn->getAttribute((*iter));
			if (true) // !attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}
	}
}

void SBScene::saveBlends(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- blends and transitions\n";
	strstr << "print \"Save Blends\"\n";
	// blends & transitions
	SBAnimationBlendManager* blendManager = getBlendManager();
	std::vector<std::string> blends = blendManager->getBlendNames();
	for (std::vector<std::string>::iterator blendIter = blends.begin();
		 blendIter != blends.end();
		 blendIter++)
	{
		SBAnimationBlend* blend = blendManager->getBlend((*blendIter));
		std::string blendString = blend->saveToString();
		strstr << blendString;
		strstr << "\n";
	}

	int numTransitions = blendManager->getNumTransitions();
	std::vector<std::string> transitions = blendManager->getBlendNames();
	for (int t = 0; t < numTransitions; t++)
	{
		SBAnimationTransition* transition = blendManager->getTransitionByIndex(t);
		std::string transitionString = transition->saveToString();
		strstr << transitionString;
		strstr << "\n";
	}
}

void SBScene::saveJointMaps(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- joint maps\n";
	strstr << "print \"Save Joint Maps\"\n";
	// joint maps
	SBJointMapManager* jointMapManager = getJointMapManager();
	std::vector<std::string> jointMapNames = jointMapManager->getJointMapNames();
	for (std::vector<std::string>::iterator iter = jointMapNames.begin(); iter != jointMapNames.end(); iter++)
	{
		const std::string& jointMapName = (*iter);
		SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);

		strstr << "jointMap = scene.getJointMapManager().getJointMap(\"" << jointMapName << "\")\n";
		strstr << "if jointMap == None:\n";
		strstr << "\tjointMap = scene.getJointMapManager().createJointMap(\"" << jointMapName << "\")\n";

		int numMappings = jointMap->getNumMappings();
		for (int m = 0; m < numMappings; m++)
		{
			std::string target = jointMap->getTarget(m);
			std::string source = jointMap->getSource(m);
			strstr << "jointMap.setMapping(\"" << source << "\", \"" << target << "\")\n";
		}
	}

	strstr << "# -------------------- applying joint maps\n";
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector<std::string> mappedMotions = scene->getMotionNames();
	std::vector<std::string> mappedSkeletons = scene->getSkeletonNames();

	for (std::vector<std::string>::iterator iter = jointMapNames.begin(); iter != jointMapNames.end(); iter++)
	{
		const std::string& jointMapName = (*iter);
		SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);

		strstr << "jointMap = scene.getJointMapManager().getJointMap(\"" << jointMapName << "\")\n";

		


#if 0
		std::vector<std::string>& mappedMotions = jointMap->getMappedMotions();
		for (std::vector<std::string>::iterator iter = mappedMotions.begin();
			 iter != mappedMotions.end();
			 iter++)
		{
			strstr << "jointMap.applyMotion(scene.getMotion(\"" << (*iter) << "\"))\n";
		}

		std::vector<std::string>& mappedSkeletons = jointMap->getMappedSkeletons();
		for (std::vector<std::string>::iterator iter = mappedSkeletons.begin();
			 iter != mappedSkeletons.end();
			 iter++)
		{
			strstr << "jointMap.applySkeleton(scene.getSkeleton(\"" << (*iter) << "\"))\n";
		}
#else	
		for (std::vector<std::string>::iterator iter = mappedMotions.begin();
			iter != mappedMotions.end();
			iter++)
		{
			SmartBody::SBMotion* motion = scene->getMotion(*iter);
			if (motion && motion->channels().getJointMapName() == jointMapName)
				strstr << "jointMap.applyMotion(scene.getMotion(\"" << (*iter) << "\"))\n";
		}

		for (std::vector<std::string>::iterator iter = mappedSkeletons.begin();
			iter != mappedSkeletons.end();
			iter++)
		{
			SmartBody::SBSkeleton* skel = scene->getSkeleton(*iter);
			if (skel && skel->getJointMapName() == jointMapName)
				strstr << "jointMap.applySkeleton(scene.getSkeleton(\"" << (*iter) << "\"))\n";
		}
#endif
	}
}

void SBScene::saveFaceDefinitions(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- face definitions\n";
	strstr << "print \"Save Face Definitions\"\n";
	// face definitions
	std::vector<std::string> faceDefinitions = getFaceDefinitionNames();
	for (std::vector<std::string>::iterator iter = faceDefinitions.begin(); iter != faceDefinitions.end(); iter++)
	{
		const std::string& faceDefName = (*iter);		
		SmartBody::SBFaceDefinition* faceDef = getFaceDefinition(faceDefName);
		std::string faceDefinitionName = "face";
		faceDefinitionName.append(faceDef->getName());
		strstr << faceDefinitionName << " = scene.getFaceDefinition(\"" << faceDefName << "\")\n";
		strstr << "if " << faceDefinitionName << " == None:\n";
		strstr << "\t" << faceDefinitionName << " = scene.createFaceDefinition(\""<< faceDefName << "\")\n";

		SkMotion* neutral = faceDef->getFaceNeutral();
		if (neutral)
		{
			strstr << faceDefinitionName << ".setFaceNeutral(\"" << neutral->getName() << "\")\n";
		}

		std::vector<std::string> visemeNames = faceDef->getVisemeNames();
		for (std::vector<std::string>::iterator faceIter = visemeNames.begin(); 
			 faceIter != visemeNames.end(); 
			 faceIter++)
		{
			const std::string& viseme = (*faceIter);
			strstr << faceDefinitionName << ".setViseme(\"" << viseme << "\", ";
			SkMotion* visemeMotion = faceDef->getVisemeMotion(viseme);
			if (visemeMotion)
			{
				strstr << "\"" + visemeMotion->getName() + "\")\n";
			}
			else
			{
				strstr << "\"\")\n";
			}
		}	

		std::vector<int> auNum = faceDef->getAUNumbers();
		for (std::vector<int>::iterator auIter = auNum.begin(); 
			 auIter != auNum.end(); 
			 auIter++)
		{
			int num = (*auIter);
			ActionUnit* au = faceDef->getAU((*auIter));
			strstr << faceDefinitionName << ".setAU(" << num << ", ";
			SkMotion* motion = NULL;
			if (au->is_bilateral())
			{
				strstr << "\"both\", \"";
				motion = au->left;
			}
			else
			{
				
				if (au->is_left())
				{
					strstr << "\"left\", \"";
					motion = au->left;
				}
				else
				{
					strstr << "\"right\", \"";
					motion = au->right;
				}
			}
			if (motion)
			{
				strstr << motion->getName(); 
			}
			strstr << "\")\n";

		}
	}
}

void SBScene::saveGestureMaps(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- gesture maps\n";
	strstr << "print \"Save Gesture Maps\"\n";
	const std::vector<std::string>& gestureMapNames = this->getGestureMapManager()->getGestureMapNames();
	for (std::vector<std::string>::const_iterator iter = gestureMapNames.begin();
		 iter != gestureMapNames.end();
		 iter++)
	{
		SBGestureMap* gestureMap = this->getGestureMapManager()->getGestureMap(*iter);
		strstr << "gestureMap = scene.getGestureMapManager().createGestureMap(\"" << (*iter) << "\")\n";
		int numMappings = gestureMap->getNumMappings();
		for (int m = 0; m < numMappings; m++)
		{
			SBGestureMap::GestureInfo& info = gestureMap->getGestureByIndex(m);
			strstr << "gestureMap.addGestureMapping(\"" << info._animation << "\", \"" << info._lexeme << "\", \"" << info._type << "\", \"" 
				                                        << info._hand << "\", \"" << info._style << "\", \"" << info._posture << "\")\n";

		}
	}

}

void SBScene::saveLipSyncing(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- lip syncing\n";
	strstr << "print \"Save Lip Syncing\"\n";
	// diphones
	SBPhonemeManager* diphoneManager = getDiphoneManager();
	std::vector<std::string> diphoneMapNames = diphoneManager->getDiphoneMapNames();
	strstr << "diphoneMapManager = scene.getDiphoneManager()\n";
	for (std::vector<std::string>::iterator iter = diphoneMapNames.begin(); iter != diphoneMapNames.end(); iter++)
	{
		const std::string& diphoneMapName = (*iter);
		std::vector<SBDiphone*>& diphones = diphoneManager->getDiphones(diphoneMapName);
		for (std::vector<SBDiphone*>::iterator diphoneIter = diphones.begin();
			 diphoneIter != diphones.end();
			 diphoneIter++)
		{
			SBDiphone* diphone = (*diphoneIter);
			const std::string& fromPhoneme = diphone->getFromPhonemeName();
			const std::string& toPhoneme = diphone->getToPhonemeName();
			strstr << "diphone = diphoneMapManager.createDiphone(\"" << fromPhoneme << "\", \"" << toPhoneme << "\", \"" << diphoneMapName << "\")\n";
			std::vector<std::string> visemes = diphone->getVisemeNames();
			for (std::vector<std::string>::iterator visemeIter = visemes.begin();
				 visemeIter != visemes.end();
				 visemeIter++)
			{
				std::vector<float>& keys = diphone->getKeys((*visemeIter));
				for (size_t x = 0; x < keys.size(); x++)
				{
					if (x + 1 < keys.size())
					{
						strstr << "diphone.addKey(\"" << (*visemeIter) << "\", " << keys[x] << ", " << keys[x + 1] << ")\n";
					}
					x++;
				}
				strstr << "\n";
			}
		}
	}
}

void SBScene::saveServices(std::stringstream& strstr, bool remoteSetup)
{
	strstr << "# -------------------- Services\n";
	// services
	SmartBody::SBServiceManager* serviceManager = this->getServiceManager();
	std::vector<std::string> serviceNames = serviceManager->getServiceNames();
	for (std::vector<std::string>::iterator iter = serviceNames.begin();
		 iter != serviceNames.end();
		 iter++)
	{
		SmartBody::SBService* service = serviceManager->getService((*iter));
		strstr << "obj = scene.getServiceManager().getService(\"" << service->getName() << "\")\n";
		std::vector<std::string> attributeNames = service->getAttributeNames();
		for (std::vector<std::string>::iterator iter = attributeNames.begin();
			 iter != attributeNames.end();
			 iter++)
		{
			SmartBody::SBAttribute* attr = service->getAttribute((*iter));
			if (!attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				strstr << attrWrite;
			}
		}

	}

}



void SBScene::setSystemParameter(const std::string& name, const std::string& value)
{
	std::map<std::string, std::string>::iterator iter = _systemParameters.find(name);
	if (iter != _systemParameters.end())
	{
		(*iter).second = value;
	}
	else
	{
		_systemParameters.insert(std::pair<std::string, std::string>(name, value));
	}
		
}

std::string SBScene::getSystemParameter(const std::string& name)
{
	std::map<std::string, std::string>::iterator iter = _systemParameters.find(name);
	if (iter != _systemParameters.end())
	{
		return (*iter).second;
	}
	else
	{
		return "";
	}
}

void SBScene::removeSystemParameter(const std::string& name)
{
	std::map<std::string, std::string>::iterator iter = _systemParameters.find(name);
	if (iter != _systemParameters.end())
	{
		_systemParameters.erase(iter);
		return;
	}

	SmartBody::util::log("Cannot remove system parameter named '%s', does not exist.", name.c_str());

}

void SBScene::removeAllSystemParameters()
{
	_systemParameters.clear();
}

std::vector<std::string> SBScene::getSystemParameterNames()
{

	std::vector<std::string> names;
	for (std::map<std::string, std::string>::iterator iter = _systemParameters.begin();
		 iter != _systemParameters.end();
		 iter++)
	{
		names.push_back((*iter).first);
	}

	return names;
}

SrCamera* SBScene::createCamera(const std::string& name)
{
	SBPawn* pawn = getPawn(name);
	SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
// 	if (camera)
// 	{
// 		SmartBody::util::log("A camera with name '%s' already exists.", name.c_str());
// 		return camera;
// 	}
// 	else 
	if (pawn)
	{
		SmartBody::util::log("A pawn with name '%s' already exists. Camera will not be created.", name.c_str());
		return NULL;
	}
	camera = new SrCamera();
	camera->setName(name);
	//SBSkeleton* skeleton = new SBSkeleton();
	//camera->setSkeleton(skeleton);
	//SkJoint* joint = skeleton->add_joint(SkJoint::TypeQuat);
	//joint->setName("world_offset");

	_cameras.insert(std::pair<std::string, SrCamera*>(name, camera));

	std::map<std::string, SbmPawn*>:: iterator iter = _pawnMap.find(camera->getName());
	if (iter != _pawnMap.end())
	{
		SmartBody::util::log( "Register pawn: pawn_map.insert(..) '%s' FAILED\n", camera->getName().c_str() ); 
	}

	_pawnMap.insert(std::pair<std::string, SbmPawn*>(camera->getName(), camera));
	_pawnNames.push_back(camera->getName());
	
	std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
	for (size_t i = 0; i < listeners.size(); i++)
	{
		listeners[i]->OnPawnCreate( camera->getName() );
	}

	// notify the services
	std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
	for (std::map<std::string, SmartBody::SBService*>::iterator iter = services.begin();
		iter != services.end();
		iter++)
	{
		SBService* service = (*iter).second;
		service->onPawnCreate(camera);
	}
	

	// if this is the first camera that is created, make it the active camera
	if (_cameras.size() == 1)
	{
		this->setActiveCamera(camera);
	}

	return camera;
}

void SBScene::removeCamera(SrCamera* camera)
{
	SBPawn* pawn = getPawn(camera->getName());
	if (!pawn)
	{
		SmartBody::util::log("No camera with name '%s' already exists. Camera will not be removed.", camera->getName().c_str());
		return;
	}

	std::map<std::string, SrCamera*>::iterator iter = _cameras.find(camera->getName());
	if (iter == _cameras.end())
	{
		SmartBody::util::log("Pawn with name '%s' already exists, but is not a camera. It will not be removed.", camera->getName().c_str());
		return;
	}

	// is this the active camera?
	if (this->getActiveCamera() == camera)
	{
		setActiveCamera(NULL);
	}
	_cameras.erase(iter);
	removePawn(camera->getName());
}

void SBScene::setActiveCamera(SrCamera* camera)
{
	if (!camera)
	{
		_activeCamera = "";
		return;
	}

	_activeCamera = camera->getName();
}

SrCamera* SBScene::getActiveCamera()
{
	if (_activeCamera == "")
		return NULL;
	std::map<std::string, SrCamera*>::iterator iter = _cameras.find(_activeCamera);
	if (iter == _cameras.end())
		return NULL;

	return (*iter).second;
}

SrCamera* SBScene::getCamera(const std::string& name)
{
	
	std::map<std::string, SrCamera*>::iterator iter = _cameras.find(name);
	if (iter == _cameras.end())
	{
		//SmartBody::util::log("No camera with name '%s' found.", name.c_str());
		return NULL;
	}
	return (*iter).second;
}

void SBScene::SetCameraLocked(bool locked)
{
   _isCameraLocked = locked;
}

bool SBScene::IsCameraLocked()
{
   return _isCameraLocked;
}

int SBScene::getNumCameras()
{
	return _cameras.size();
}

std::vector<std::string> SBScene::getCameraNames()
{
	std::vector<std::string> cameraNames;
	for (std::map<std::string, SrCamera*>::iterator iter = _cameras.begin();
		iter != _cameras.end();
		iter++)
	{
		cameraNames.push_back((*iter).first);
	}

	return cameraNames;
}


std::vector<SBController*>& SBScene::getDefaultControllers()
{
	return _defaultControllers;
}

void SBScene::createDefaultControllers()
{
	 _defaultControllers.push_back(new MeCtEyeLidRegulator());
	 _defaultControllers.push_back(new MeCtSaccade(NULL));
	 std::map<int, MeCtReachEngine*> reachMap;
	 _defaultControllers.push_back(new MeCtExampleBodyReach(NULL));
	 _defaultControllers.push_back(new MeCtBreathing());
	 _defaultControllers.push_back(new MeCtGaze());
	 _defaultControllers.push_back(new MeCtNewLocomotion());
	 _defaultControllers.push_back(new MeCtGenericHand());
	 _defaultControllers.push_back(new RealTimeLipSyncController());

	 for (size_t x = 0; x < _defaultControllers.size(); x++)
		 _defaultControllers[x]->ref();
}

void SBScene::removeDefaultControllers()
{
	 for (size_t x = 0; x < _defaultControllers.size(); x++)
		 _defaultControllers[x]->unref();
	 _defaultControllers.clear();
}

std::vector<std::string> SBScene::getAssetPaths(const std::string& type)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getAssetPaths() instead.");
	return getAssetManager()->getAssetPaths(type);
}

std::vector<std::string> SBScene::getLocalAssetPaths(const std::string& type)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getLocalAssetPaths() instead.");
	return getAssetManager()->getLocalAssetPaths(type);
}

void SBScene::addAssetPath(const std::string& type, const std::string& path)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addAssetPath() instead.");
	getAssetManager()->addAssetPath(type, path);
}

void SBScene::removeAssetPath(const std::string& type, const std::string& path)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addAssetPath() instead.");
	getAssetManager()->removeAssetPath(type, path);
}

void SBScene::removeAllAssetPaths(const std::string& type)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.removeAllAssetPaths() instead.");
	getAssetManager()->removeAllAssetPaths(type);
}

void SBScene::loadAssets()
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addAssetPath() instead.");
	getAssetManager()->loadAssets();
}

void SBScene::loadAsset(const std::string& assetPath)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.loadAsset() instead.");
	getAssetManager()->loadAsset(assetPath);
}

void SBScene::loadAssetsFromPath(const std::string& assetPath)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.loadAssetsFromPath() instead.");
	getAssetManager()->loadAssetsFromPath(assetPath);
}

SBSkeleton* SBScene::addSkeletonDefinition(const std::string& skelName )
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addSkeletonDefinition() instead.");
	return getAssetManager()->addSkeletonDefinition(skelName);
}

void SBScene::removeSkeletonDefinition(const std::string& skelName )
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.removeSkeletonDefinition() instead.");
	return getAssetManager()->removeSkeletonDefinition(skelName);
}


SBMotion* SBScene::addMotionDefinition(const std::string& motionName, double duration, int numFrames )
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addSkeletonDefinition() instead.");
	
	//SBMotion* sbMotion = new SBMotion();
	return this->getAssetManager()->addMotionDefinition(motionName, duration, numFrames);
}

SBMotion* SBScene::createMotion(const std::string& motionName)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.createMotion() instead.");
	return getAssetManager()->createMotion(motionName);
}

void SBScene::addMotions(const std::string& path, bool recursive)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addMotion() instead.");
	getAssetManager()->addMotions(path, recursive);
}

int SBScene::getNumSkeletons()
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getNumSkeletons() instead.");
	return getAssetManager()->getNumSkeletons();
}

std::vector<std::string> SBScene::getSkeletonNames()
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getSkeletonNames() instead.");
	return getAssetManager()->getSkeletonNames();
}

SBMotion* SBScene::getMotion(const std::string& name)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getMotion() instead.");
	return getAssetManager()->getMotion(name);
}

int SBScene::getNumMotions()
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getNumMotions() instead.");
	return getAssetManager()->getNumMotions();
}

std::vector<std::string> SBScene::getMotionNames()
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getNumMotions() instead.");
	return getAssetManager()->getMotionNames();
}

SBSkeleton* SBScene::createSkeleton(const std::string& skeletonDefinition)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.createSkeleton() instead.");
	return getAssetManager()->createSkeleton(skeletonDefinition);
}

SBSkeleton* SBScene::getSkeleton(const std::string& name)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getSkeleton() instead.");
	return getAssetManager()->getSkeleton(name);
}

SrSnGroup* SBScene::getRootGroup()
{
	return _rootGroup;
}


std::string SBScene::getValidName(const std::string& name)
{
	bool nameFound = true;
	int nameCounter = 0;
	std::string currentName = name;
	while (nameFound)
	{
		SmartBody::SBPawn* pawn = getPawn(currentName);
		if (!pawn)
		{
			nameFound = false;
		}
		else
		{
			SmartBody::SBCharacter* character = getCharacter(currentName);
			if (!character)
			{
				nameFound = false;
			}
			else
			{
				std::stringstream strstr;
				strstr << name << nameCounter;
				nameCounter++;
				currentName = strstr.str();
			}
		}
	}
	return currentName;
}

void SBScene::updatePawnNames()
{
	std::vector<SbmPawn*> allPawns;
	for (std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.begin();
		 iter != _pawnMap.end();
		 iter++)
	{
		allPawns.push_back((*iter).second);
	}
	_pawnMap.clear();
	_pawnNames.clear();

	for (std::vector<SbmPawn*>::iterator iter = allPawns.begin();
		 iter != allPawns.end();
		 iter++)
	{
		_pawnMap.insert(std::pair<std::string, SbmPawn*>((*iter)->getName(), (*iter))); 
		_pawnNames.push_back((*iter)->getName());
	}

}

void SBScene::updateCharacterNames()
{
	std::vector<SbmCharacter*> allCharacters;
	for (std::map<std::string, SbmCharacter*>::iterator iter = _characterMap.begin();
		 iter != _characterMap.end();
		 iter++)
	{
		allCharacters.push_back((*iter).second);
	}
	_characterMap.clear();
	_characterNames.clear();

	for (std::vector<SbmCharacter*>::iterator iter = allCharacters.begin();
		 iter != allCharacters.end();
		 iter++)
	{
		_characterMap.insert(std::pair<std::string, SbmCharacter*>((*iter)->getName(), (*iter))); 
		_characterNames.push_back((*iter)->getName());
	}
}

bool blendScalePos(SrVec &v, SrVec &rootPos, int rootIdx, int headIdx, SrVec4i& boneIdx, SrVec4& boneWeight, float blendThreshold, float scaleRatio)
{
	if (v.y > rootPos.y + blendThreshold)
		return false;

	float rootHeadSkinWeightMax = 0.9f;
	float rootHeadSkinWeightMin = 0.7f;
	float rootSkinWeightMin = 0.1f;
	float headSkinWeight = -1.f;
	float rootSkinWeight = -1.f;
	for (int k = 0; k < 4; k++)
	{
		if (boneIdx[k] == rootIdx)
		{
			rootSkinWeight = boneWeight[k];
			//SmartBody::util::log("neck skin weight = %f", rootSkinWeight);
		}
		else if (boneIdx[k] == headIdx)
		{
			headSkinWeight = boneWeight[k];
			//SmartBody::util::log("head skin weight = %f", headSkinWeight);
		}
	}
	if (headSkinWeight > rootHeadSkinWeightMax) return false; // don't scale the part when head weight is larger than threshold
	//if (v.y > rootPos.y && rootSkinWeight < rootSkinWeightMin) return false;
	//float blendRatio = (v.y - rootPos.y + blendThreshold) / (blendThreshold*2.0);
	float blendRatio = (v.y - rootPos.y) / (blendThreshold);
	//float blendRatio = 0.f;
	if (blendRatio < 0.f) blendRatio = 0.0;
// 	if (headSkinWeight > rootHeadSkinWeightMin)
// 	{		
// 		blendRatio = (headSkinWeight - rootHeadSkinWeightMin) / (rootHeadSkinWeightMax - rootHeadSkinWeightMin);
// 		SmartBody::util::log("head skin weight = %f, root skin weight = %f, blendRatio = %f", headSkinWeight, rootSkinWeight, blendRatio);
// 	}
	
	float blendScaleRatio = (blendRatio)+(1.0 - blendRatio)*scaleRatio;
	v = rootPos + (v - rootPos)*blendScaleRatio;
	return true;
}

bool blendScalePos2(SrVec &v, SrVec &rootPos, SrVec &neckPos, int rootIdx, int neckIdx, SrVec4i& boneIdx, SrVec4& boneWeight, float blendThreshold, float scaleRatio)
{
	if (v.y > rootPos.y) // don't change anything
		return false;

	float rootHeadSkinWeightMax = 0.9f;
	float rootHeadSkinWeightMin = 0.7f;
	float rootSkinWeightMin = 0.1f;
	float neckSkinWeight = -1.f;
	float rootSkinWeight = -1.f;
	for (int k = 0; k < 4; k++)
	{
		if (boneIdx[k] == rootIdx)
		{
			rootSkinWeight = boneWeight[k];
			//SmartBody::util::log("neck skin weight = %f", rootSkinWeight);
		}
		else if (boneIdx[k] == neckIdx)
		{
			neckSkinWeight = boneWeight[k];
			//SmartBody::util::log("head skin weight = %f", headSkinWeight);
		}
	}
	if (rootSkinWeight > rootHeadSkinWeightMax) return false; // don't scale the part when head weight is larger than threshold
	if (v.y > neckPos.y && neckSkinWeight < 0.1f) return false;

	//float blendRatio = (v.y - rootPos.y + blendThreshold) / (blendThreshold*2.0);
	float blendRatio = (rootPos.y - v.y) / (blendThreshold);
	//float blendRatio = 0.f;
	if (blendRatio > 1.f) blendRatio = 1.f;
	//if (v.y < neckPos.y) blendRatio = 1.f;
	//if (rootPos.y < neckPos.y) blendRatio = 0.f
	// 	if (headSkinWeight > rootHeadSkinWeightMin)
	// 	{		
	// 		blendRatio = (headSkinWeight - rootHeadSkinWeightMin) / (rootHeadSkinWeightMax - rootHeadSkinWeightMin);
	// 		SmartBody::util::log("head skin weight = %f, root skin weight = %f, blendRatio = %f", headSkinWeight, rootSkinWeight, blendRatio);
	// 	}

	float blendScaleRatio = (1.0 - blendRatio)+(blendRatio)*scaleRatio;
	v = rootPos + (v - rootPos)*blendScaleRatio;
	return true;
}




SBAPI void SBScene::rescalePartialMeshSkeleton(const std::string& meshName, const std::string& skelName, const std::string& rootJointName, const std::vector<std::string>& skipMeshNames, float scaleRatio, float blendRatio)
{
	SmartBody::util::log("Rescale mesh and skeleton");
	SBAssetManager* assetManager = getAssetManager();
	DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
	
	
	
	SmartBody::SBSkeleton* skel = assetManager->getSkeleton(skelName);
	if (!mesh || !skel)
	{
		SmartBody::util::log("Warning, can't find mesh '%s' or skeleton '%s'.", meshName.c_str(), skelName.c_str());
		return;
	}

	SmartBody::SBJoint* rootJoint = skel->getJointByName(rootJointName);
	if (!rootJoint)
	{
		SmartBody::util::log("Warning, can't find joint '%s'.", rootJointName.c_str());
		return;
	}
	SmartBody::util::log("before build skin vertex buffer");
	mesh->buildSkinnedVertexBuffer();
	SmartBody::util::log("after build skin vertex buffer");

	SmartBody::util::log("before skeleton update global matrices");
	skel->update_global_matrices();
	SrVec rootPos = rootJoint->gmat().get_translation();
	std::vector<std::string> belowJointNames;
	std::string headJointName = "none";
	belowJointNames.push_back(rootJointName);
	int rootJointIdx = mesh->boneJointIdxMap[rootJointName];
	int headJointIdx = -1;
	int neckJointIdx = -1;
	SmartBody::util::log("before find head joints");
	for (int i = 0; i < skel->getNumJoints(); i++)
	{
		SmartBody::SBJoint* joint = skel->getJoint(i);
#if 1
		//if (joint->getParent() == rootJoint) // children of root joint
		{
			std::string jointName = joint->getName();
			std::transform(jointName.begin(), jointName.end(), jointName.begin(), ::tolower);
			if (jointName.find("head") != std::string::npos)
			{
				SmartBody::util::log("Head Joint = %s", joint->getMappedJointName().c_str());
				headJointName = joint->getMappedJointName();
				headJointIdx = mesh->boneJointIdxMap[headJointName];
			}
		}
#endif

		SrVec jpos = joint->gmat().get_translation();
		if (jpos.y < rootPos.y) // lower than root position
		{
			belowJointNames.push_back(joint->getMappedJointName());
		}
	}
	SrVec neckPos = rootJoint->getParent()->gmat().get_translation();
	neckJointIdx = mesh->boneJointIdxMap[rootJoint->getParent()->getMappedJointName()];
	SmartBody::util::log("Head Joint name = %s, idx = %d, Root Joint name = %s, idx = %d", headJointName.c_str(), headJointIdx, rootJointName.c_str(), rootJointIdx);
	
	float blendThreshold = skel->getBoundingBox().size().y*blendRatio;	
	
	//rootPos = rootPos + SrVec(0, blendThreshold, 0);
	std::map<std::string, bool> skipMeshMap;
	for (unsigned int i = 0; i < skipMeshNames.size(); i++)
		skipMeshMap[skipMeshNames[i]] = true;

	for (unsigned int i = 0; i < mesh->dMeshStatic_p.size(); i++)
	{
		SrModel& model = mesh->dMeshStatic_p[i]->shape();
		SrModel& dynModel = mesh->dMeshDynamic_p[i]->shape();
		SkinWeight* skinWeight = mesh->skinWeights[i];
		skinWeight->buildSkinWeightBuf();		
		std::string modelName = (const char*)model.name;
		if (skipMeshMap.find(modelName) != skipMeshMap.end()) // skip the mesh
			continue;
		//std::map<std::string, std::vector<SrSnModel*> > blendShapeMap;
		for (unsigned int k = 0; k < model.V.size(); k++)
		{		
			SrVec& v = model.V[k];
			SrVec4i gboneIdx;
			SrVec4i& lboneIdx = skinWeight->boneIDs[k];
			for (int b = 0; b < 4; b++)
			{
				gboneIdx[b] = mesh->boneJointIdxMap[skinWeight->infJointName[lboneIdx[b]]];
			}
			//LOG("old bone idx = %d %d %d %d", lboneIdx[0], lboneIdx[1], lboneIdx[2], lboneIdx[3]);
			//LOG("global bone idx = %d %d %d %d", gboneIdx[0], gboneIdx[1], gboneIdx[2], gboneIdx[3]);
			bool willScale = blendScalePos(v, rootPos, rootJointIdx, headJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
			//bool willScale = blendScalePos2(v, rootPos, neckPos, rootJointIdx, neckJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
			if (!willScale) continue;

			model.V[k] = v; // update the new position
			dynModel.V[k] = v;
			//model.VOrig[k] = v;
		}		
		model.computeNormals();
		dynModel.computeNormals();
		//std::string modelName = (const char*)model.name;
		if (mesh->blendShapeMap.find(modelName) != mesh->blendShapeMap.end()) // the shape is associated with blendshape
		{
			std::vector<SrSnModel*>& blendShapes = mesh->blendShapeMap[modelName];
			for (unsigned int j = 0; j < blendShapes.size(); j++)
			{
				SrModel& faceModel = blendShapes[j]->shape();
				for (unsigned int k = 0; k < faceModel.V.size(); k++)
				{
					SrVec v = faceModel.V[k];
#if 0
					if (v.y > rootPos.y + blendThreshold)
						continue;

					float blendRatio = (v.y - rootPos.y) / (blendThreshold);
					if (blendRatio < 0.f) blendRatio = 0.0;
					float blendScaleRatio = (blendRatio)+(1.0 - blendRatio)*scaleRatio;
					v = rootPos + (v - rootPos)*blendScaleRatio;
#else
					SrVec4i gboneIdx;
					SrVec4i& lboneIdx = skinWeight->boneIDs[k];
					for (int b = 0; b < 4; b++)
					{
						gboneIdx[b] = mesh->boneJointIdxMap[skinWeight->infJointName[lboneIdx[b]]];
					}
					bool willScale = blendScalePos(v, rootPos, rootJointIdx, headJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
					//bool willScale = blendScalePos2(v, rootPos, neckPos, rootJointIdx, neckJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
					if (!willScale) continue;
#endif

					//v = rootPos + (v - rootPos)*scaleRatio;
					//faceModel.V[k] = v; // update the new position					
					faceModel.V[k] = model.V[k];
				}
			}

		}
	}
	
	// rescale the offset for each joint below root joint
	for (unsigned int i = 0; i < belowJointNames.size(); i++)
	{
		SmartBody::SBJoint* joint = skel->getJointByMappedName(belowJointNames[i]);
		joint->setOffset(joint->getOffset()*scaleRatio);
	}

	skel->invalidate_global_matrices();
	skel->update_global_matrices();
	SrVec newRootPos = rootJoint->gmat().get_translation();
	SrVec rootOffset = rootPos - newRootPos;
	SmartBody::SBJoint* skelRoot = dynamic_cast<SmartBody::SBJoint*>(skel->root());
	skelRoot->setOffset(skelRoot->getOffset() + rootOffset);

	skel->invalidate_global_matrices();
	skel->update_global_matrices();
	skel->updateGlobalMatricesZero();

	// update bind pose matrices
	for (unsigned int i = 0; i < mesh->skinWeights.size(); i++)
	{
		SkinWeight* sw = mesh->skinWeights[i];
		for (unsigned int k = 0; k < sw->infJointName.size(); k++)
		{
			// manually add all joint names
			SmartBody::SBJoint* joint = skel->getJointByName(sw->infJointName[k]);

			SrMat gmatZeroInv = joint->gmatZero().rigidInverse();
			sw->bindPoseMat[k] = gmatZeroInv;
		}
	}

	mesh->rebuildVertexBuffer(true);
}

Heightfield* SBScene::getHeightfield()
{
	return _heightField;
}


Heightfield* SBScene::createHeightfield()
{
	if (_heightField)
		delete _heightField;
	_heightField = new Heightfield();
	return _heightField;
}

void SBScene::removeHeightfield()
{
	if (_heightField)
		delete _heightField;
	_heightField = NULL;
}

float SBScene::queryTerrain( float x, float z, float *normal_p )
{
	if (_heightField)
	{
		return( _heightField->get_elevation( x, z, normal_p ) );
	}
	if( normal_p )	{
		normal_p[ 0 ] = 0.0;
		normal_p[ 1 ] = 1.0;
		normal_p[ 2 ] = 0.0;
	}
	return( 0.0 );
}

#ifndef SB_NO_PYTHON
void SBScene::setPythonMainModule(boost::python::object& pyobject)
{
	_mainModule = pyobject;
}

void SBScene::setPythonMainDict(boost::python::object& pyobject)
{
	_mainDict = pyobject;
}

boost::python::object& SBScene::getPythonMainModule()
{
	return _mainModule;
}

boost::python::object& SBScene::getPythonMainDict()
{
	return _mainDict;
}
#endif


bool SBScene::setCameraConeOfSight(const std::string& characterName) {
	SrCamera* camera = getActiveCamera();
	if (!camera)
	{
		SmartBody::util::log("No active camera found. Cannot create camera track.");
		return false;
	}
	SbmPawn* pawn = SmartBody::SBScene::getScene()->getPawn(characterName);
	if (!pawn)
	{
		SmartBody::util::log("Object %s was not found, cannot track.", characterName.c_str());
		return false;
	}

	SkSkeleton* skeleton	= pawn->getSkeleton();

	if(!skeleton->search_joint("eyeball_left")) {
		SmartBody::util::log("Can't enable coneOfsight: 'eyeball_left' joint not found.");
		return false;
	}

	if(!skeleton->search_joint("eyeball_right")) {
		SmartBody::util::log("Can't enable coneOfsight: 'eyeball_right' joint not found.");
		return false;
	}

	_coneOfSight			= true;
	_coneOfSight_character	= characterName;

	return true;
}

bool SBScene::hasConeOfSight() {
	return _coneOfSight;
}

void SBScene::removeConeOfSight() {
	_coneOfSight			= false;
	_coneOfSight_character	= "";
}

//
//	void SBScene::updateConeOfSight()
//
//	Updates the camera location parameters when coneOfSight is enabled. This
//	renders the scene from the characters viewpoint
//
void SBScene::updateConeOfSight()
{
	//	Gets the character from which we want to look from
	SmartBody::SBCharacter * character = getCharacter(_coneOfSight_character);
	character->getSkeleton()->invalidate_global_matrices();
	character->getSkeleton()->update_global_matrices();

	//	Left eye world location
	SkJoint * leftEye			= character->getSkeleton()->search_joint("eyeball_left");
	if(!leftEye) {
		SmartBody::util::log("Can't find 'eyeball_left' joint.\n");
    return;
	}
	const SrMat& gmat_leftEye	= leftEye->gmat();
	SrVec leftEye_location		= SrVec(gmat_leftEye.get(3, 0), gmat_leftEye.get(3, 1), gmat_leftEye.get(3, 2));

	//	Right eye world location
	SkJoint* rightEye			= character->getSkeleton()->search_joint("eyeball_right");
	if(!rightEye) {
		SmartBody::util::log("Can't find 'eyeball_right' joint.\n");
    return;
	}
	const SrMat& gmat_rightEye	= rightEye->gmat();
	SrVec rightEye_location		= SrVec(gmat_rightEye.get(3, 0), gmat_rightEye.get(3, 1), gmat_rightEye.get(3, 2));

	//	Center of the eyse world coordinates
	SrVec center_eyes_location	= (leftEye_location + rightEye_location) / 2;
	
	float scale					= 1.f / getScale();
	float znear					= 0.01f * scale;
	float zfar					= 100.0f * scale;
	SrCamera* camera			= getActiveCamera();
	float eyebeamLength			= 100 * character->getHeight() / 175.0f;
	
	SrVec localAxis_right		= rightEye->localGlobalAxis(2)*eyebeamLength;
	SrVec eyes_look_at_right	= localAxis_right * rightEye->gmat();

	SrVec localAxis_left		= leftEye->localGlobalAxis(2)*eyebeamLength;
	SrVec eyes_look_at_left		= localAxis_left * leftEye->gmat();
	
	SrVec eyes_look_at			= (eyes_look_at_right + eyes_look_at_left)/2;

	//	Sets FROM where the chararacter is looking
	camera->setEye(	center_eyes_location.x,
					center_eyes_location.y,
					center_eyes_location.z);

	//	Sets where the character looking is AT
	camera->setCenter(	eyes_look_at.x, 
						eyes_look_at.y,
						eyes_look_at.z);
	
	//	Sets near clip plane
	camera->setNearPlane(znear);
	
	//	Sets far clip plane
	camera->setFarPlane(zfar);
}

//	
//		std::vector<std::string> SBScene::checkVisibility(const std::string& characterName)
//		
//		Returns a list of visible pawns from a given chracter
//	
std::vector<std::string> SBScene::checkVisibility(const std::string& characterName)
{
	std::vector<std::string> visible_pawns, nonOccludePawns;
#if !defined(EMSCRIPTEN)
	//	Gets the character from which we want to look from
	SmartBody::SBCharacter* character = getCharacter(characterName);
	
	if(!character) {
		SmartBody::util::log("Character %s not found.", characterName.c_str());
		return visible_pawns;
	}

	character->getSkeleton()->invalidate_global_matrices();
	character->getSkeleton()->update_global_matrices();

	//	Left eye world location
	SkJoint* leftEye			= character->getSkeleton()->search_joint("eyeball_left");
	if(!leftEye) {
		SmartBody::util::log("Can't find 'eyeball_left' joint.\n");
		return visible_pawns;
	}
	const SrMat& gmat_leftEye	= leftEye->gmat();
	SrVec leftEye_location		= SrVec(gmat_leftEye.get(3, 0), gmat_leftEye.get(3, 1), gmat_leftEye.get(3, 2));

	//	Right eye world location
	SkJoint* rightEye			= character->getSkeleton()->search_joint("eyeball_right");
	if(!rightEye) {
		SmartBody::util::log("Can't find 'eyeball_right' joint.\n");
		return visible_pawns;
	}
	const SrMat& gmat_rightEye	= rightEye->gmat();
	SrVec rightEye_location		= SrVec(gmat_rightEye.get(3, 0), gmat_rightEye.get(3, 1), gmat_rightEye.get(3, 2));

	//	Center of the eyse world coordinates
	SrVec center_eyes_location	= (leftEye_location + rightEye_location) / 2;
	
	float scale					= 1.f / getScale();
	float znear					= 0.01f * scale;
	float zfar					= 100.0f * scale;
	float eyebeamLength			= 100 * character->getHeight() / 175.0f;
	
	SrVec localAxis_right		= rightEye->localGlobalAxis(2)*eyebeamLength;
	SrVec eyes_look_at_right	= localAxis_right * rightEye->gmat();
	SrVec localAxis_left		= leftEye->localGlobalAxis(2)*eyebeamLength;
	SrVec eyes_look_at_left		= localAxis_left * leftEye->gmat();
	SrVec eyes_look_at			= (eyes_look_at_right + eyes_look_at_left)/2;

	//	Stores current camera setup, to restore it at the end
	SrCamera * camera			= getActiveCamera();
	SrVec tmp_eye				= camera->getEye();
	SrVec tmp_center			= camera->getCenter();
	float tmp_near				= camera->getNearPlane();
	float tmp_far				= camera->getFarPlane();

	//	Sets the camera setup to simulate characters viewport
	//	Sets FROM where the chararacter is looking
	camera->setEye(	center_eyes_location.x,
					center_eyes_location.y,
					center_eyes_location.z);

	//	Sets where the character looking is AT
	camera->setCenter(	eyes_look_at.x, 
						eyes_look_at.y,
						eyes_look_at.z);
	
	//	Sets near clip plane
	camera->setNearPlane(znear);
	
	//	Sets far clip plane
	camera->setFarPlane(zfar);

#if USE_GL_FIXED_PIPELINE
	SrMat mat( SrMat::NotInitialized );
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrix( camera->get_perspective_mat(mat) );
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrix( camera->get_view_mat(mat) );
	//	Creates characters frustrm 
	SmartBody::SBScene * scene	= SmartBody::SBScene::getScene();
	std::vector<std::string> pawnNames = scene->getPawnNames();
	std::vector<std::string>::iterator removeItem = std::remove(pawnNames.begin(),pawnNames.end(), characterName);
	pawnNames.erase(removeItem, pawnNames.end());
	visible_pawns = frustumTest(pawnNames);
	nonOccludePawns = occlusionTest(visible_pawns);
	// Restores matrices and camera set up
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif
	//	Sets FROM where the chararacter is looking
	camera->setEye(	tmp_eye.x,
					tmp_eye.y,
					tmp_eye.z);

	//	Sets where the character looking is AT
	camera->setCenter(	tmp_center.x, 
						tmp_center.y,
						tmp_center.z);

	//	Sets near clip plane
	camera->setNearPlane(tmp_near);

	//	Sets near clip plane
	camera->setFarPlane(tmp_far);
#endif
	//	Returns visible pawns
	return nonOccludePawns;
}


std::vector<std::string> SBScene::occlusionTest( const std::vector<std::string>& testPawns)
{
	std::vector<std::string> visiblePawns;
#if !defined(__ANDROID__) && !defined(EMSCRIPTEN) && !defined(SB_IPHONE) 
	float m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	SrMat modelViewMat = SrMat(m);
	SmartBody::SBScene * scene	= SmartBody::SBScene::getScene();
	// sort the pawn from front to back
	std::vector<std::pair<float,int> > pawnZSortedList;
	for (unsigned int i=0;i<testPawns.size();i++)
	{
		SmartBody::SBPawn* pawn		= scene->getPawn(testPawns[i]);

		SBGeomObject* geomObject = pawn->getGeomObject();
		SBGeomBox* box = dynamic_cast<SBGeomBox*>(geomObject);
		SrBox pawn_bb;
		pawn_bb = pawn->getBoundingBox();		
		std::vector<SrVec> bbPts = pawn_bb.getCorners();
		float zMin = -1e30f;
		for (unsigned int k=0;k<bbPts.size();k++)
		{
			SrVec camPt = bbPts[k]*modelViewMat;
			if (camPt.z >  zMin)
				zMin = camPt.z;
		}	
		pawnZSortedList.push_back(std::pair<float,int>(zMin,i));
	}
	std::sort(pawnZSortedList.begin(),pawnZSortedList.end()); // sort pawns from back to front
	reverse(pawnZSortedList.begin(),pawnZSortedList.end()); // reverse the order so the order is front to back

	//for (unsigned int i=0;i<pawnZSortedList.size();i++)
	//	SmartBody::util::log("Pawn %d, zMin = %f", pawnZSortedList[i].second, pawnZSortedList[i].first);

	// occlusion window width/height
	int width = 256, height = 256;
	// create render buffer
	GLuint fb, color_rb, depth_rb;
	//RGBA8 RenderBuffer, 24 bit depth RenderBuffer, 256x256
	glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    //Create and attach a color buffer
    glGenRenderbuffers(1, &color_rb);
    //We must bind color_rb before we call glRenderbufferStorageEXT
    glBindRenderbuffer(GL_RENDERBUFFER, color_rb);
    //The storage format is RGBA8
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    //Attach color buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rb);
    //-------------------------
    glGenRenderbuffers(1, &depth_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    //-------------------------
    //Attach depth buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
    //-------------------------
    //Does the GPU support current FBO configuration?
	/*
    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
		case GL_FRAMEBUFFER_COMPLETE:
            SmartBody::util::log("FrameBufferObject success");
			break;
	
	   default:
			SmartBody::util::log("FrameBufferObject error");
	}
	*/
    //-------------------------
   	// bind FBO for rendering
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //-------------------------
    glViewport(0, 0, width, height);
   
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  
	// perform occlusion test
	GLuint uiOcclusionQuery;
	glGenQueries(1, &uiOcclusionQuery);
	for (size_t i=0;i<pawnZSortedList.size();i++)
	{
		int pawnIdx = pawnZSortedList[i].second;
		SmartBody::SBPawn* pawn		= scene->getPawn(testPawns[pawnIdx]);
		SrBox pawn_bb;
		pawn_bb = pawn->getBoundingBox();		
		//SmartBody::util::log("pawn %s, min = %.3f %.3f %.3f, max = %.3f %.3f %.3f", pawn->getName().c_str(), pawn_bb.a[0], pawn_bb.a[1], pawn_bb.a[2], pawn_bb.b[0], pawn_bb.b[1], pawn_bb.b[2]);
		glBeginQuery(GL_SAMPLES_PASSED, uiOcclusionQuery); 
		// Every pixel that passes the depth test now gets added to the result		
		glDrawBox(pawn_bb.a, pawn_bb.b);
		glEndQuery(GL_SAMPLES_PASSED); 
		// Now get tthe number of pixels passed
		int iSamplesPassed = 0; 
		glGetQueryObjectiv(uiOcclusionQuery, GL_QUERY_RESULT, &iSamplesPassed); 
		if (iSamplesPassed > 0) // not occluded
		{
			//SmartBody::util::log("pawn %s, visible samples = %d", pawn->getName().c_str(), iSamplesPassed);
			visiblePawns.push_back(pawn->getName());
			//glDrawBox(pawn_bb.a, pawn_bb.b);
		}		
	}
	glDeleteQueries(1, &uiOcclusionQuery);
    //Bind 0, which means render to back buffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//Delete resources
	glDeleteRenderbuffersEXT(1, &color_rb);
	glDeleteRenderbuffersEXT(1, &depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fb);

	//for (unsigned int i=0;i<visiblePawns.size();i++)
	//	SmartBody::util::log("Visible pawn %d = %s", i, visiblePawns[i].c_str());
#endif
	return visiblePawns;
}


std::vector<std::string> SBScene::frustumTest(const std::vector<std::string>& testPawnNames)
{
	std::vector<std::string> visiblePawns;

	SrFrustum frustum;
	frustum.extractFrustum();

	SmartBody::SBScene * scene	= SmartBody::SBScene::getScene();
	const std::vector<std::string>& pawns = testPawnNames;

	//	Iterates over all pawns, to check which ones are visible from the characters viewport
	for (	std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		pawnIter != pawns.end();
		pawnIter++)
	{
		SmartBody::SBPawn* pawn		= scene->getPawn((*pawnIter));

		SBGeomObject* geomObject = pawn->getGeomObject();
		SBGeomBox* box = dynamic_cast<SBGeomBox*>(geomObject);
		SrBox pawn_bb;
		if (box)
		{
			SBTransform& transform = pawn->getGlobalTransform();
			SrVec min(-box->extent[0] / 2.0, -box->extent[1] / 2.0, -box->extent[2] / 2.0);
			SrVec max(box->extent[0] / 2.0, box->extent[1] / 2.0, box->extent[2] / 2.0);

			SrVec finalMin = transform.localToGlobal(min);
			SrVec finalMax = transform.localToGlobal(max);
			pawn_bb.set(finalMin, finalMax);
		}
		else
		{
			pawn_bb = pawn->getBoundingBox();
		}

		float ax = pawn_bb.a.x;
		float ay = pawn_bb.a.y;
		float az = pawn_bb.a.z;
		float bx = pawn_bb.b.x;
		float by = pawn_bb.b.y;
		float bz = pawn_bb.b.z;

		SrVec pointAAB(pawn_bb.a.x, pawn_bb.a.y, pawn_bb.b.z);
		SrVec pointABB(pawn_bb.a.x, pawn_bb.b.y, pawn_bb.b.z);
		SrVec pointBBA(pawn_bb.b.x, pawn_bb.b.y, pawn_bb.a.z); 
		SrVec pointBAA(pawn_bb.b.x, pawn_bb.a.y, pawn_bb.a.z); 
		SrVec pointABA(pawn_bb.a.x, pawn_bb.b.y, pawn_bb.a.z);
		SrVec pointBAB(pawn_bb.b.x, pawn_bb.a.y, pawn_bb.b.z); 

		//	If bounding box visible, adds pawn to list of visible pawns
		SrPnt center = pawn_bb.getCenter();
		if (frustum.pointInFrustum(pawn_bb.a) || 
			frustum.pointInFrustum(pawn_bb.b) ||
			frustum.pointInFrustum(center) ||
			frustum.pointInFrustum(pointAAB) || 
			frustum.pointInFrustum(pointABB) ||
			frustum.pointInFrustum(pointBBA) || 
			frustum.pointInFrustum(pointBAA) || 
			frustum.pointInFrustum(pointABA) ||
			frustum.pointInFrustum(pointBAB)) 
			visiblePawns.push_back(pawn->getName());
	}
	return visiblePawns;
}



//	
//		std::vector<std::string> SBScene::checkVisibility_current_view()
//		
//		Returns a list of visible pawns from current view port
//	
std::vector<std::string> SBScene::checkVisibility_current_view() {
	//SmartBody::util::log("Checking visibility...\n");
	SmartBody::SBScene * scene	= SmartBody::SBScene::getScene();
	std::vector<std::string> pawnNames = scene->getPawnNames();
	std::vector<std::string>::iterator removeItem = std::remove(pawnNames.begin(),pawnNames.end(), "cameraDefault");
	pawnNames.erase(removeItem, pawnNames.end());
	std::vector<std::string> visible_pawns = frustumTest(pawnNames);
	
#if 0
	SrFrustum frustum;
	frustum.extractFrustum();
	
	// Checks visibility 
	SmartBody::SBScene * scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& pawns = scene->getPawnNames();

	for (	std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
	{
		SmartBody::SBPawn* pawn		= scene->getPawn((*pawnIter));

		SrBox pawn_bb				= pawn->getBoundingBox();
			
		if(frustum.pointInFrustum(pawn_bb.a) || frustum.pointInFrustum(pawn_bb.b)) 
			visible_pawns.push_back(pawn->getName());

		/*
		// Usign OgreCamera
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		if (!camera) {
			SrBox pawn_bb				= pawn->getBoundingBox();
			Ogre::Vector3 point_min		= Ogre::Vector3(pawn_bb.a.x, pawn_bb.a.y, pawn_bb.a.z); 
			Ogre::Vector3 point_max		= Ogre::Vector3(pawn_bb.b.x, pawn_bb.b.y, pawn_bb.b.z); 

			if(ogreCam->isVisible(point_min) || ogreCam->isVisible(point_max))
				SmartBody::util::log("Pawn %s is visible", pawn->getName().c_str());
		}
		*/
	}
#endif
	std::vector<std::string> nonOccludePawns = occlusionTest(visible_pawns);

	return nonOccludePawns;
}



/*
void SBScene::checkVisibility() {
	// Checks visibility 
	const std::vector<std::string>& pawns = getPawnNames();
	for (	std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
	{
		
		SmartBody::SBPawn* pawn	= getPawn((*pawnIter));
		SrCamera * camera		= getActiveCamera();
				
		SrVec p, l, u;
		float angle, aspect, znear, zfar;
		
		angle	= camera->getFov() * 180 / 3.14159265358979323846;
		aspect	= camera->getAspectRatio();
		znear	= camera->getNearPlane();
		zfar	= camera->getFarPlane();

		p		= camera->getEye();
		l		= camera->getCenter();
		u		= camera->getUpVector();

		//SmartBody::util::log("Eye:    %f, %f, %f", p.x, p.y, p.z);
		//SmartBody::util::log("Center: %f, %f, %f", l.x, l.y, l.z);
		//SmartBody::util::log("Up: %f, %f, %f", u.x, u.y, u.z);
		//SmartBody::util::log("Fovy: %f\tAspect %f\tClip %f, %f\n", angle, aspect, znear, zfar);

		SrFrustum frustum;
		frustum.setCamInternals(angle, aspect, znear, zfar);
		frustum.setCamDef(p, l, u);
		
		//if(frustum.pointInFrustum(SrVec(0,0,0)) != SrFrustum::OUTSIDE)
		//	SmartBody::util::log("Center visible\n");
		//else
		//	SmartBody::util::log("Center NOt visible\n");
		
		SrCamera* check_is_camera = dynamic_cast<SrCamera*>(pawn);
		if (!check_is_camera) {
			
	

			
			SrBox pawn_bb				= pawn->getBoundingBox();
			SrVec min_point, max_point;

			min_point.x = pawn_bb.a.x;
			min_point.y = pawn_bb.a.y;
			min_point.z = pawn_bb.a.z;

			max_point.x = pawn_bb.b.x;
			max_point.y = pawn_bb.b.y;
			max_point.z = pawn_bb.b.z;

			SmartBody::util::log("InFrustum: %d\t%d\n", frustum.pointInFrustum(min_point), frustum.pointInFrustum(min_point));

			if(frustum.pointInFrustum(min_point) > 0 || frustum.pointInFrustum(max_point) > 0) 
				SmartBody::util::log("Pawn %s is visible", pawn->getName().c_str());
			
		}
	}
}
*/

void SBScene::setCameraTrack(const std::string& characterName, const std::string& jointName)
{
	SrCamera* camera = getActiveCamera();
	if (!camera)
	{
		SmartBody::util::log("No active camera found. Cannot create camera track.");
		return;
	}
	SbmPawn* pawn = SmartBody::SBScene::getScene()->getPawn(characterName);
	if (!pawn)
	{
		SmartBody::util::log("Object %s was not found, cannot track.", characterName.c_str());
		return;
	}
	if (jointName == "")
	{
		SmartBody::util::log("Need to specify a joint to track.");
		return;
	}

	SkSkeleton* skeleton = NULL;
	skeleton = pawn->getSkeleton();

	SkJoint* joint = pawn->getSkeleton()->search_joint(jointName.c_str());
	if (!joint)
	{
		SmartBody::util::log("Could not find joint %s on object %s.", jointName.c_str(), characterName.c_str());
		return;
	}

	joint->skeleton()->update_global_matrices();
	joint->update_gmat();
	const SrMat& jointMat = joint->gmat();
	SrVec jointPos(jointMat[12], jointMat[13], jointMat[14]);
	CameraTrack* cameraTrack = new CameraTrack();
	cameraTrack->joint = joint;
	cameraTrack->jointToCamera = camera->getEye() - jointPos;
	SmartBody::util::log("Vector from joint to target is %f %f %f", cameraTrack->jointToCamera.x, cameraTrack->jointToCamera.y, cameraTrack->jointToCamera.z);
	cameraTrack->targetToCamera = camera->getEye() - camera->getCenter();
	SmartBody::util::log("Vector from target to eye is %f %f %f", cameraTrack->targetToCamera.x, cameraTrack->targetToCamera.y, cameraTrack->targetToCamera.z);				
	_cameraTracking.push_back(cameraTrack);
	SmartBody::util::log("Object %s will now be tracked at joint %s.", characterName.c_str(), jointName.c_str());
}

void SBScene::removeCameraTrack()
{
	if (_cameraTracking.size() > 0)
	{
		for (std::vector<CameraTrack*>::iterator iter = _cameraTracking.begin();
			 iter != _cameraTracking.end();
			 iter++)
		{
			CameraTrack* cameraTrack = (*iter);
			delete cameraTrack;
		}
		_cameraTracking.clear();
		SmartBody::util::log("Removing current tracked object.");
	}
}

bool SBScene::hasCameraTrack()
{
	return _cameraTracking.size() > 0;
}



void SBScene::updateTrackedCameras()
{
	for (size_t x = 0; x < _cameraTracking.size(); x++)
	{
		// move the camera relative to the joint
		SkJoint* joint = _cameraTracking[x]->joint;
		joint->skeleton()->update_global_matrices();
		joint->update_gmat();
		const SrMat& jointGmat = joint->gmat();
		SrVec jointLoc(jointGmat[12], jointGmat[13], jointGmat[14]);
		SrVec newJointLoc = jointLoc;
		if (fabs(jointGmat[13] - _cameraTracking[x]->yPos) < _cameraTracking[x]->threshold)
			newJointLoc.y = (float) _cameraTracking[x]->yPos;
		SrVec cameraLoc = newJointLoc + _cameraTracking[x]->jointToCamera;
		SrCamera* activeCamera = getActiveCamera();
		activeCamera->setEye(cameraLoc.x, cameraLoc.y, cameraLoc.z);
		SrVec targetLoc = cameraLoc - _cameraTracking[x]->targetToCamera;
		activeCamera->setCenter(targetLoc.x, targetLoc.y, targetLoc.z);
	}	
}

SrViewer* SBScene::getViewer()
{
	return _viewer;
}

SrViewer* SBScene::getOgreViewer()
{
	return _ogreViewer;
}

void SBScene::setViewer(SrViewer* viewer)
{
	_viewer = viewer;
}

void SBScene::setOgreViewer(SrViewer* viewer)
{
	_ogreViewer = viewer;
}

SrViewerFactory* SBScene::getViewerFactory()
{
	return _viewerFactory;
}

SrViewerFactory* SBScene::getOgreViewerFactory()
{
	return _ogreViewerFactory;
}

void SBScene::setViewerFactory(SrViewerFactory* viewerFactory)
{
	if (_viewerFactory)
		delete _viewerFactory;
	_viewerFactory = viewerFactory;
}

void SBScene::setOgreViewerFactory(SrViewerFactory* viewerFactory)
{
	if (_ogreViewerFactory)
		delete _ogreViewerFactory;
	_ogreViewerFactory = viewerFactory;
}

KinectProcessor* SBScene::getKinectProcessor()
{
	return _kinectProcessor;
}

std::map<std::string, GeneralParam*>& SBScene::getGeneralParameters()
{
	return _generalParams;
}

#if 0
SBAPI bool SBScene::createNavigationMesh( const std::string& meshfilename )
{	
	std::vector<SrModel*> meshVec;
#if (BOOST_VERSION > 104400)
	std::string ext = boost::filesystem::extension(meshfilename);
#else
	std::string ext = boost::filesystem2::extension(meshfilename);
#endif
	std::string file = boost::filesystem::basename(meshfilename);	
	bool loadSuccess = false;
	if (ext == ".obj" || ext == ".OBJ")
	{
		SrModel *mesh = new SrModel();
		loadSuccess = mesh->import_obj(meshfilename.c_str());		
		meshVec.push_back(mesh);
	}
	else if ( ext == ".xml" || ext == ".XML" )
	{
		std::vector<SkinWeight*> tempWeights;
		std::string skeletonName;
		loadSuccess = ParserOgre::parseSkinMesh(meshVec,tempWeights,meshfilename,skeletonName, 1.0,true,false);	
	}
	else if ( ext == ".dae" || ext == ".DAE" )
	{
		if (SmartBody::SBScene::getScene()->getBoolAttribute("useFastCOLLADAParsing"))
			loadSuccess = ParserCOLLADAFast::parseStaticMesh(meshVec, meshfilename);
		else
			loadSuccess = ParserOpenCOLLADA::parseStaticMesh(meshVec, meshfilename);
	}
	
	if (!loadSuccess || meshVec.size() == 0)
	{
		SmartBody::util::log("Error loading navigation mesh, filename = %s",meshfilename.c_str());
		return false;
	}
	//mesh.scale(0.3f);
	SrModel* srMesh = meshVec[0];
	for (unsigned int i=1;i<meshVec.size();i++)
		srMesh->add_model(*meshVec[i]); // adding all mesh together	
	srMesh->remove_redundant_materials();
	srMesh->validate();
	srMesh->computeNormals();
	if (_navigationMesh) 
	{
		delete _navigationMesh;
		_navigationMesh = NULL;
	}
	_navigationMesh = new SBNavigationMesh();
	_navigationMesh->buildNavigationMesh(*srMesh);
	return true;
}
#endif

SBAPI void SBScene::setNavigationMesh( const std::string& naviMeshName )
{
	SBNavigationMeshManager* naviManager = getNavigationMeshManager();
	SBNavigationMesh* naviMesh = naviManager->getNavigationMesh(naviMeshName);
	if (naviMesh)
		_navigationMesh = naviMesh;
}

SBAPI SBNavigationMesh* SBScene::getNavigationMesh()
{
	return _navigationMesh;
}

void SBScene::startFileLogging(const std::string& filename)
{
	_logListener = new SmartBody::util::FileListener(filename.c_str());
	SmartBody::util::g_log.AddListener(_logListener);
}

void SBScene::stopFileLogging()
{
	if (_logListener)
		SmartBody::util::g_log.RemoveListener(_logListener);
}

std::string SBScene::getStringFromObject(SmartBody::SBObject* object)
{
	std::stringstream strstr;
	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(object);
	if (character)
	{
		strstr << "character/" << character->getName();
		return strstr.str();
	}

	SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(object);
	if (pawn)
	{
		strstr << "pawn/" << pawn->getName();
		return strstr.str();
	}

	SmartBody::SBScene* scene = dynamic_cast<SmartBody::SBScene*>(object);
	if (scene)
	{
		strstr << "scene/" << scene->getName();
		return strstr.str();
	}

	SmartBody::SBSkeleton* skeleton = dynamic_cast<SmartBody::SBSkeleton*>(object);
	if (skeleton)
	{
		strstr << "skeleton/" << skeleton->getName();
		return strstr.str();
	}

	SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(object);
	if (motion)
	{
		strstr << "motion/" << motion->getName();
		return strstr.str();
	}

	SmartBody::SBService* service = dynamic_cast<SmartBody::SBService*>(object);
	if (service)
	{
		strstr << "service/" << service->getName();
		return strstr.str();
	}

	DeformableMesh* mesh = dynamic_cast<DeformableMesh*>(object);
	if (mesh)
	{
		strstr << "model/" << mesh->getName();
		return strstr.str();
	}

	SmartBody::SBController* controller = dynamic_cast<SmartBody::SBController*>(object);
	if (controller)
	{
		strstr << "character/" << controller->getCharacterName() << "/controller/" << controller->getName();
		return strstr.str();
	}

	SmartBody::SBJointMap* jointmap = dynamic_cast<SmartBody::SBJointMap*>(object);
	if (jointmap)
	{
		strstr << "jointmap/" << jointmap->getName();
		return strstr.str();
	}

	SmartBody::SBGestureMap* gesturemap = dynamic_cast<SmartBody::SBGestureMap*>(object);
	if (gesturemap)
	{
		strstr << "gesturemap/" << gesturemap->getName();
		return strstr.str();
	}

	SmartBody::SBEventHandler* eventHandler = dynamic_cast<SmartBody::SBEventHandler*>(object);
	if (eventHandler)
	{
		strstr << "eventhandler/" << eventHandler->getName();
		return strstr.str();
	}

	SmartBody::SBAnimationBlend* blend = dynamic_cast<SmartBody::SBAnimationBlend*>(object);
	if (blend)
	{
		strstr << "blend/" << blend->getName();
		return strstr.str();
	}

	SmartBody::SBAnimationTransition* transition = dynamic_cast<SmartBody::SBAnimationTransition*>(object);
	if (transition)
	{
		strstr << "transition/" << transition->getName();
		return strstr.str();
	}

	SmartBody::SBScript* script = dynamic_cast<SmartBody::SBScript*>(object);
	if (script)
	{
		strstr << "script/" << script->getName();
		return strstr.str();
	}

	SmartBody::SBFaceDefinition* facedef = dynamic_cast<SmartBody::SBFaceDefinition*>(object);
	if (facedef)
	{
		strstr << "facedefinition/" << facedef->getName();
		return strstr.str();
	}

	return "";
}


SmartBody::SBObject* SBScene::getObjectFromString(const std::string& value)
{
	int prefixPos = value.find("/");
	if (prefixPos == std::string::npos)
		return NULL;

	std::string prefix = value.substr(0, prefixPos);
	std::string suffix = value.substr(prefixPos + 1);

	if (prefix == "character")
	{
		// check for a second level name
		int prefixPos2 = suffix.find("/");
		if (prefixPos2 == std::string::npos)
		{
			SmartBody::SBCharacter* character = this->getCharacter(suffix);
			return character;
		}

		std::string characterName = suffix.substr(0, prefixPos2);
		SmartBody::SBCharacter* character = this->getCharacter(characterName);
		if (!character)
			return NULL;

		std::string remainder = suffix.substr(prefixPos2 + 1);

		int prefixPos3 = remainder.find("/");
		if (prefixPos3 != std::string::npos)
		{
			std::string part = remainder.substr(0, prefixPos3);		
			std::string rest = remainder.substr(prefixPos3 + 1);
			if (part == "controller")
			{
				SBController* controller = character->getControllerByName(rest);
				return controller;
			}
			if (part == "skeleton")
			{
				SBSkeleton* skeleton = character->getSkeleton();
				if (skeleton->getName() == rest)
					return skeleton;
				else
					return NULL;
			}
			
		}
		else if (remainder == "minibrain")
		{
			SmartBody::Nvbg* nvbg = character->getNvbg();
			return nvbg;
		}
		
		return NULL;
	}
	else if (prefix == "pawn")
	{
		SmartBody::SBPawn* pawn = this->getPawn(suffix);
		return pawn;
	}
	else if (prefix == "scene")
	{
		return this;
	}
	else if (prefix == "motion")
	{
		SmartBody::SBMotion* motion = this->getAssetManager()->getMotion(suffix);
		return motion;
	}
	else if (prefix == "skeleton")
	{
		SmartBody::SBSkeleton* skeleton = this->getAssetManager()->getSkeleton(suffix);
		return skeleton;
	}
	else if (prefix == "service")
	{
		SmartBody::SBService* service = this->getServiceManager()->getService(suffix);
		return service;
	}
	else if (prefix == "mesh")
	{
		DeformableMesh* mesh = this->getAssetManager()->getDeformableMesh(suffix);
		return mesh;
	}
	else if (prefix == "envmap")
	{
		SbmTextureManager& texManager = SbmTextureManager::singleton();
		SbmTexture* texture = texManager.findTexture(SbmTextureManager::TEXTURE_HDR_MAP, suffix.c_str());
		return texture;
	}
	else if (prefix == "controller")
	{
		int prefixPos2 = suffix.find("/");
		if (prefixPos2 == std::string::npos)
			return NULL;
		std::string prefix2 = suffix.substr(0, prefixPos2 - 1);
		std::string suffix2 = suffix.substr(prefixPos2 + 1);
		SmartBody::SBCharacter* character = this->getCharacter(suffix);
		if (!character)
			return NULL;
		return character->getControllerByName(suffix2);
	}
	else if (prefix == "jointmap")
	{
		SBJointMap* jointMap = this->getJointMapManager()->getJointMap(suffix);
		return jointMap;
	}
	else if (prefix == "gesturemap")
	{
		SBGestureMap* gestureMap = this->getGestureMapManager()->getGestureMap(suffix);
		return gestureMap;
	}
	else if (prefix == "eventhandler")
	{
		SBEventHandler* eventHandler = this->getEventManager()->getEventHandler(suffix);
		return eventHandler;
	}
	else if (prefix == "blend")
	{
		SBAnimationBlend* blend = this->getBlendManager()->getBlend(suffix);
		return blend;
	}
	else if (prefix == "transition")
	{
		SBAnimationTransition* transition = this->getBlendManager()->getTransitionByName(suffix);
		return transition;
	}
	else if (prefix == "script")
	{
		SBScript* script = this->getScript(suffix);
		return script;
	}
	else if (prefix == "facedefinition")
	{
		SBFaceDefinition* facedef = this->getFaceDefinition(suffix);
		return facedef;
	}
	return NULL;
}

std::string SBScene::getLastScriptDirectory()
{
	return _lastScriptDirectory;
}

void SBScene::setLastScriptDirectory(std::string dir)
{
	_lastScriptDirectory = dir;
}

};

