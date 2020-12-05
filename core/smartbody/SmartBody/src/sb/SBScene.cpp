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

#ifndef SB_NO_JAVASCRIPT
#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif
#endif

#include "sb/SBAssetStore.h"
#include <sb/SBObject.h>
#include <sb/SBCharacter.h>
#include <sb/SBMotion.h>
#include <sb/SBScript.h>
#include <sb/SBEvent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include "sb/SBAnimationState.h"
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
#include <sb/SBVHMsgManager.h>
#include <sb/SBMotionGraph.h>
#include "SBUtilities.h"
#include <sbm/sbm_audio.h>
#include <utility>
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
#include <sbm/Heightfield.h>
#include <sbm/action_unit.hpp>
#include <sbm/xercesc_utils.hpp>
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


#include <sbm/KinectProcessor.h>
#include <sr/sr_sn_group.h>
#include <fstream>
#include <sstream>
#include <algorithm>





// for minizip compression

#if !defined(__FLASHPLAYER__) && !defined(EMSCRIPTEN)
#include <zlib.h>
#include <zip.h>
#endif


#ifndef WIN32
#define _stricmp strcasecmp
#endif

#define SHOW_DEPRECATION_MESSAGES 0
namespace SmartBody {

SBScene* SBScene::_scene = nullptr;
bool SBScene::_firstTime = true;


std::map<std::string, std::string> SBScene::_systemParameters;


class ForwardLogListener : public SmartBody::util::Listener
{
    public:
		ForwardLogListener() = default;
		~ForwardLogListener() override = default;

        void OnMessage( const std::string & message ) override
		{
			SBScene* scene = SmartBody::SBScene::getScene();
			if (!scene)
				return;
			std::vector<SBSceneListener*>& listeners = scene->getSceneListeners();
			for (auto & listener : listeners)
			{
				listener->OnLogMessage(message);
			}
		}
};


SBScene::SBScene(CoreServices coreServices) : SBObject(),
_assetStore(std::make_unique<SBAssetStore>(*this)),
_coreServices(std::move(coreServices))
{
	_scene = this;
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
	_gestureMapManager = new SBGestureMapManager();
	_jointMapManager = new SBJointMapManager();
	_boneBusManager = new SBBoneBusManager();
	_phonemeManager = new SBPhonemeManager();
	_behaviorSetManager = new SBBehaviorSetManager();
	_retargetManager = new SBRetargetManager();
	_eventManager = new SBEventManager();
	_assetManager = new SBAssetManager(*_assetStore);
	_speechManager = new SBSpeechManager();
	_vhmsgManager = new SBVHMsgManager();
	_commandManager = new SBCommandManager();
	_naviMeshManager = new SBNavigationMeshManager();
	_motionGraphManager = new SBMotionGraphManager();
	_handConfigManager = new SBHandConfigurationManager();
	//_debuggerServer = new SBDebuggerServer();

	//_scale = .01f; // default scale is centimeters
	_scale = 1.f;

	// add the services
	_serviceManager->addService(_steerManager);
	_serviceManager->addService(_coreServices.physicsManager.get());
	_serviceManager->addService(_boneBusManager);
	_serviceManager->addService(_coreServices.collisionManager.get());
	_serviceManager->addService(_vhmsgManager);
	_serviceManager->addService(_realtimeManager);
	_serviceManager->addService(_phonemeManager);
	_serviceManager->addService(_profiler);
	//_serviceManager->addService(_debuggerServer);

	_parser = new SBParser();

	//_debuggerClient = new SBDebuggerClient();
	//_debuggerUtility = new SBDebuggerUtility();
	_isRemoteMode = false;

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
	auto* forwardListener = new ForwardLogListener();
	SmartBody::util::g_log.AddListener(forwardListener);
#if !defined(WIN_BUILD) && !defined(NATIVE_FRAMEWORK_BUILD)
	auto* stdoutListener = new SmartBody::util::StdoutListener();
	SmartBody::util::g_log.AddListener(stdoutListener);
#endif
	
	//consoleAttr->setValue(true); // set up the console logging
	
	_mediaPath = ".";
		// re-initialize
	// initialize everything

	_viewer = nullptr;
	_viewerFactory = nullptr;

	_rootGroup = new SrSnGroup();
	_rootGroup->ref();

	_heightField = nullptr;
	_navigationMesh = nullptr;
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
		_viewer = nullptr;
//#if !defined (__ANDROID__) && !defined(SB_IPHONE) && !defined(__native_client__) && !defined(EMSCRIPTEN)
//		SbmShaderManager::singleton().setViewer(nullptr);
//#endif
	}

	_logListener = nullptr;


	auto bmlCopy = _bml;


	auto lambda = [](){};

	_commandManager->insert("vrAgentBML", [this](srArgBuffer& args) -> int {
		return _bml->getBMLProcessor()->vrAgentBML_cmd_func(args, nullptr);
	});
	_commandManager->insert("bp", [this](srArgBuffer& args) -> int {
		return _bml->getBMLProcessor()->bp_cmd_func(args, nullptr);
	});
	_commandManager->insert("vrSpeak", [this](srArgBuffer& args) -> int {
		return _bml->getBMLProcessor()->vrSpeak_func(args, nullptr);
	});

	_commandManager->insert_set_cmd("bp", [this](srArgBuffer& args) -> int {
		return _bml->getBMLProcessor()->set_func(args, nullptr);
	});
	_commandManager->insert_print_cmd("bp", [this](srArgBuffer& args) -> int {
		return _bml->getBMLProcessor()->print_func(args, nullptr);
	});




}

SBScene::~SBScene()
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

	for (auto & faceDefinition : faceDefinitions)
	{
		std::string faceName = faceDefinition;
		removeFaceDefinition(faceName);
	}

	// stop the services
	SBServiceManager* serviceManager = getServiceManager();
	std::vector<std::string> serviceNames =  serviceManager->getServiceNames();
	for (auto & serviceName : serviceNames)
	{
		SBService* service = serviceManager->getService(serviceName);
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
	delete _gestureMapManager;
	delete _jointMapManager;
	delete _boneBusManager;
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
	//delete _debuggerServer;

	_sim = nullptr;
	_profiler = nullptr;
	_bml = nullptr;
	_blendManager = nullptr;
	_reachManager = nullptr;
	_steerManager= nullptr;
	_serviceManager = nullptr;
	_gestureMapManager= nullptr;
	_jointMapManager = nullptr;
	_boneBusManager = nullptr;
	_phonemeManager = nullptr;
	_behaviorSetManager = nullptr;
	_retargetManager = nullptr;
	_eventManager = nullptr;
	_assetManager = nullptr;
	_commandManager = nullptr;
	_speechManager = nullptr;

	_kinectProcessor = nullptr;




	delete _heightField;

	_heightField = nullptr;

#if 0 // this should be done in asset manager
	if (_navigationMesh)
	{
		delete _navigationMesh;
	}
#endif
	_navigationMesh = nullptr;

	_rootGroup->unref();
	_rootGroup = nullptr;

	_viewer = nullptr;
	_viewerFactory = nullptr;

#ifndef SB_NO_VHCL_AUDIO
	AUDIO_Close();
	AUDIO_Init();
#endif

	if (_vhmsgManager->isEnable() && _vhmsgManager->isConnected())
		_vhmsgManager->send( "vrProcEnd sbm" );

//	delete _vhmsgManager;
//	_vhmsgManager = nullptr;

//#if !defined(SB_IPHONE)
//	SbmTextureManager::destroy_singleton();
//	SbmShaderManager::destroy_singleton();
//#endif

	for (auto iter = _scripts.begin();
		 iter != _scripts.end();
		 iter++)
	{
	//	delete (*iter).second;
	}

	delete _parser;

	if (_scene == this) {
		_scene = nullptr;
	}

//	_debuggerClient->Disconnect();
//  // TODO: should delete these in reverse order?
//	delete _debuggerClient;
//	delete _debuggerUtility;
//	
	//mcu.reset();
}

//SBDebuggerServer* SBScene::getDebuggerServer()
//{
//	return _debuggerServer;
//}
//
//SBDebuggerClient* SBScene::getDebuggerClient()
//{
//	return _debuggerClient;
//}
//
//
//SBDebuggerUtility* SBScene::getDebuggerUtility()
//{
//	return _debuggerUtility;
//}

SBScene* SBScene::getScene()
{
//	if (_firstTime)
//	{
//		XMLPlatformUtils::Initialize();
//		_firstTime = false;
//		_scene = new SBScene();
//		_scene->initialize();
//	}

	return _scene;
}
 
void SBScene::destroyScene()
{
	if (_scene)
	{
		XMLPlatformUtils::Terminate(); 
		delete _scene;
		_scene = nullptr;
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
		//getDebuggerClient()->Update();
		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		for (const auto & pawnIter : pawns)
		{
			SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnIter);
			pawn->ct_tree_p->evaluate(getSimulationManager()->getTime());
			pawn->ct_tree_p->applySkeletonToBuffer();
		}

		// update renderer as listener
		std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnSimulationUpdate( );
		}

		return;
	}
	
	//SmartBody::util::log("After remote mode");
	// scripts
	this->getProfiler()->mark("scripts", 1, "beforeUpdate()");
	std::map<std::string, SmartBody::SBScript*>& scripts = getScripts();
	for (auto & script : scripts)
	{
		if (script.second->isEnable())
			script.second->beforeUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("scripts");
	//SmartBody::util::log("After script");
	// services
	this->getProfiler()->mark("services", 1, "beforeUpdate()");
	std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
	for (auto & service : services)
	{
		if (service.second->isEnable())
			service.second->beforeUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("services");
	//SmartBody::util::log("After services");

	this->getProfiler()->mark("allsequences", 1, "commands");
	std::string seqName;
	std::vector<std::string> sequencesToDelete;
	SequenceManager* activeSequences = getCommandManager()->getActiveSequences();
	int numSequences = activeSequences->getNumSequences();
	for (int s = 0; s < numSequences; s++)
	{
		srCmdSeq* seq = activeSequences->getSequence(s, seqName);
		std::string cmd;
		if (seq && seq->isValid())
		{
			do {
				cmd = seq->pop( (float) getSimulationManager()->getTime() );
				if (!cmd.empty())
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
			} while( !cmd.empty() );
			if( seq->get_count() < 1 )
			{
				sequencesToDelete.emplace_back(seqName);
			}
		}
	}

	for (auto & d : sequencesToDelete)
	{
		activeSequences->removeSequence(d, true);
	}
	activeSequences->cleanupMarkedSequences();
	this->getProfiler()->mark("allsequences");

	this->getProfiler()->mark("pawn", 1, "controller evaluation");
	const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
	for (const auto & pawnIter : pawns)
	{
		SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnIter);
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
	for (const auto & pawnName : pawnNames)
	{
		SBPawn* pawn = getPawn(pawnName);
		pawn->afterUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("pawn");

	this->getProfiler()->mark("listeners", 1, "OnSimulationUpdate()");
	std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnSimulationUpdate( );
	}
	this->getProfiler()->mark("listeners");
	//SmartBody::util::log("After listener update");
	this->getProfiler()->mark("scripts", 1, "update()");
	for (auto & script : scripts)
	{
		if (script.second->isEnable())
			script.second->update(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("scripts");
	//SmartBody::util::log("After running scripts");
	this->getProfiler()->mark("services", 1, "update()");
	for (auto & service : services)
	{
		if (service.second->isEnable())
			service.second->update(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("services");
	//SmartBody::util::log("After service update");
	// services
	this->getProfiler()->mark("services", 1, "afterUpdate()");
	for (auto & service : services)
	{
		if (service.second->isEnable())
			service.second->afterUpdate(getSimulationManager()->getTime());
	}
	this->getProfiler()->mark("services");


	// scripts
	this->getProfiler()->mark("scripts", 1, "afterUpdate()");
	for (auto & script : scripts)
	{
		if (script.second->isEnable())
			script.second->afterUpdate(getSimulationManager()->getTime());
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
		return nullptr;
	}
	else
	{
		SmartBody::SBCharacter* copyChar = createCharacter(copyCharName,"");
		if (!copyChar)
		{
			SmartBody::util::log("Can not copy to existing character '%s'",copyCharName.c_str());
			return nullptr;
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
		return nullptr;
	}
	else
	{
		SmartBody::SBPawn* copyPawn = createPawn(copyPawnName);
		if (!copyPawn)
		{
			SmartBody::util::log("Can not copy to existing pawn '%s'",copyPawnName.c_str());
			return nullptr;
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
		return nullptr;
	}
	else
	{
		SBCharacter* character = new SBCharacter(charName, metaInfo);
		
		std::map<std::string, SbmPawn*>::iterator iter = _pawnMap.find(character->getName());
		if (iter != _pawnMap.end())
		{
			SmartBody::util::log( "Register character: pawn_map.insert(..) '%s' FAILED\n", character->getName().c_str() );
			delete character;
			return nullptr;
		}

		_pawnMap.insert(std::pair<std::string, SbmPawn*>(character->getName(), character));
		_pawnNames.emplace_back(character->getName());
	
		std::map<std::string, SbmCharacter*>::iterator citer = _characterMap.find(character->getName());
		if (citer != _characterMap.end())
		{
			SmartBody::util::log( "Register character: character_map.insert(..) '%s' FAILED\n", character->getName().c_str() );
			_pawnMap.erase(iter);
			delete character;
			return nullptr;
		}
		_characterMap.insert(std::pair<std::string, SbmCharacter*>(character->getName(), character));
		_characterNames.emplace_back(character->getName());

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
		return nullptr;
	}
	if (pawn)
	{
		SmartBody::util::log("Pawn '%s' already exists!", pawnName.c_str());
		return nullptr;
	}
	else
	{
		SBPawn* pawn = new SBPawn(pawnName.c_str());
		SBSkeleton* skeleton = new SBSkeleton();
		pawn->setSkeleton(skeleton);
		//SkJoint* joint = skeleton->add_joint(SkJoint::TypeQuat);
		//joint->setName("world_offset");

		return insertPawn(pawn);
	}
}

SBPawn* SBScene::insertPawn(SBPawn* pawn) {
	auto iter = _pawnMap.find(pawn->getName());
	if (iter != _pawnMap.end())
	{
		SmartBody::util::log( "Register pawn: pawn_map.insert(..) '%s' FAILED\n", pawn->getName().c_str() );
		delete pawn;
		return nullptr;
	}

	_pawnMap.insert(std::pair<std::string, SbmPawn*>(pawn->getName(), pawn));
	_pawnNames.emplace_back(pawn->getName());


	std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnPawnCreate( pawn->getName() );
	}

	// notify the services
	auto& services = getServiceManager()->getServices();
	for (auto & entry : services)
	{
		SBService* service = entry.second;
		service->onPawnCreate(pawn);
	}
	return pawn;
}


void SBScene::removeCharacter(const std::string& charName)
{
	SBCharacter* character = this->getCharacter(charName);
	const std::string& name = character->getName();
	if (character)
	{
				// notify the services
		std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
		for (auto & iter : services)
		{
			SBService* service = iter.second;
			service->onCharacterDelete(character);
		}
	
		std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnCharacterDelete( name);
		}

#ifndef SB_NO_BONEBUS
		if ( character->bonebusCharacter )
		{
			this->getBoneBusManager()->getBoneBus().DeleteCharacter(  character->bonebusCharacter );
			character->bonebusCharacter = nullptr;
		}
#endif

		auto iter = _pawnMap.find(name);
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
	for (const auto & character : characters)
	{
		removeCharacter(character);
	}
	
}

void SBScene::removePawn(const std::string& pawnName)
{
	SbmPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnName);
	if (pawn)
	{
		const std::string& name = pawn->getName();

		auto* character = dynamic_cast<SbmCharacter*>(pawn);
		if (!character)
		{
			// notify the services
			std::map<std::string, SmartBody::SBService*>& services = getServiceManager()->getServices();
			for (auto & iter : services)
			{
				SBService* service = iter.second;
				auto* sbpawn = dynamic_cast<SBPawn*>(pawn);
				service->onPawnDelete(sbpawn);
			}
			
			std::vector<SmartBody::SBSceneListener*>& listeners = this->getSceneListeners();
			for (auto & listener : listeners)
			{
				listener->OnPawnDelete( name );
			}

			auto iter = _pawnMap.find(name);
			if (iter != _pawnMap.end())
			{
				_pawnMap.erase(iter);
			}
			for (auto nameIter = _pawnNames.begin();
				 nameIter != _pawnNames.end();
				 nameIter++)
			{
				if (name == (*nameIter))
				{
					_pawnNames.erase(nameIter);
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
	for (const auto & pawn : pawns)
	{
		removePawn(pawn);
	}

	//HACK since we need to inform SBRenderScene when it can remove cameras. This way of doing things should be removed in a future refactor.
	if (_removeAllPawnsCallback) {
		_removeAllPawnsCallback();
	}

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
	auto iter = _pawnMap.find(name);
	if (iter == _pawnMap.end())
	{
		return nullptr;
	}
	else
	{
		SBPawn* sbpawn = dynamic_cast<SBPawn*>((*iter).second);
		return sbpawn;
	}
}

SBCharacter* SBScene::getCharacter(const std::string& name)
{
	auto iter = _characterMap.find(name);
	if (iter == _characterMap.end())
	{
		return nullptr;
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

	for(auto & iter : eventManager->getEventHandlers())
	{

		ret.emplace_back(std::string(iter.first));
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
	getCommandManager()->getActiveSequences()->clear();
	getCommandManager()->getPendingSequences()->clear();
}

void SBScene::sendVHMsg(const std::string& message)
{
	getVHMsgManager()->send(message.c_str());
}

void SBScene::sendVHMsg2(const std::string& message, const std::string& message2)
{

	getVHMsgManager()->send2(message.c_str(), message2.c_str());
}

void SBScene::setCommandRunner(std::function<bool(const std::string&)> commandRunner) {
	_commandRunner = std::move(commandRunner);
}

void SBScene::setScriptRunner(std::function<bool(const std::string&)> scriptRunner) {
	_scriptRunner = std::move(scriptRunner);
}


bool SBScene::run(const std::string& command)
{
	if (_commandRunner) {
		return _commandRunner(command);
	} else {
		SmartBody::util::log("No command runner registered in the Scene.");
		return false;
	}
}

bool SBScene::runScript(const std::string& script)
{
	if (_scriptRunner) {
		return _scriptRunner(script);
	} else {
		SmartBody::util::log("No script runner registered in the Scene.");
		return false;
	}
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
	return _coreServices.collisionManager.get();
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
	return _coreServices.physicsManager.get();
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
	for (auto & _sceneListener : this->_sceneListeners)
	{
		_sceneListener->OnObjectCreate(face);
	}

	return face;
}

void SBScene::removeFaceDefinition(const std::string& name)
{
	

	// make sure the name doesn't already exist
	auto iter = _faceDefinitions.find(name);
	if (iter ==_faceDefinitions.end())
	{
		SmartBody::util::log("Face definition named '%s' does not exist.", name.c_str());
		return;
	}
	for (auto & _sceneListener : this->_sceneListeners)
	{
		_sceneListener->OnObjectDelete(iter->second);
	}
	delete iter->second;
	iter->second = nullptr;
	_faceDefinitions.erase(iter);

}

SmartBody::SBFaceDefinition* SBScene::getFaceDefinition(const std::string& name)
{
	// make sure the name doesn't already exist
	auto iter = _faceDefinitions.find(name);
	if (iter == _faceDefinitions.end())
	{
		SmartBody::util::log("Face definition named '%s' does not exist.", name.c_str());
		return nullptr;
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
	for (auto & _faceDefinition : _faceDefinitions)
	{
		faces.emplace_back(_faceDefinition.second->getName());
	}

	return faces;
}

void SBScene::addScript(const std::string& name, SBScript* script)
{
	auto iter = _scripts.find(name);
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
	auto iter = _scripts.find(name);
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

	for (auto & _script : _scripts)
	{
		scriptNames.emplace_back(_script.first);
	}

	return scriptNames;

}

SBScript* SBScene::getScript(const std::string& name)
{
	auto iter = _scripts.find(name);
	if (iter == _scripts.end())
	{
		SmartBody::util::log("Script with name %s already exists.", name.c_str());
		return nullptr;
	}

	return (*iter).second;
}

std::map<std::string, SBScript*>& SBScene::getScripts()
{
	return _scripts;
}

void SBScene::addSceneListener(SBSceneListener* listener)
{
	auto iter = std::find(_sceneListeners.begin(), _sceneListeners.end(), listener);
	if (iter == _sceneListeners.end())
		_sceneListeners.emplace_back(listener);
}

void SBScene::removeSceneListener(SBSceneListener* listener)
{
	auto iter = std::find(_sceneListeners.begin(), _sceneListeners.end(), listener);
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





void SBScene::setSystemParameter(const std::string& name, const std::string& value)
{
	auto iter = _systemParameters.find(name);
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
	auto iter = _systemParameters.find(name);
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
	auto iter = _systemParameters.find(name);
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
	for (auto & _systemParameter : _systemParameters)
	{
		names.emplace_back(_systemParameter.first);
	}

	return names;
}

std::vector<SBController*>& SBScene::getDefaultControllers()
{
	return _defaultControllers;
}

void SBScene::createDefaultControllers()
{
	 _defaultControllers.emplace_back(new MeCtEyeLidRegulator());
	 _defaultControllers.emplace_back(new MeCtSaccade(nullptr));
	 std::map<int, MeCtReachEngine*> reachMap;
	 _defaultControllers.emplace_back(new MeCtExampleBodyReach(nullptr));
	 _defaultControllers.emplace_back(new MeCtBreathing());
	 _defaultControllers.emplace_back(new MeCtGaze());
	 _defaultControllers.emplace_back(new MeCtNewLocomotion());
	 _defaultControllers.emplace_back(new MeCtGenericHand());
	 _defaultControllers.emplace_back(new RealTimeLipSyncController());

	 for (auto & _defaultController : _defaultControllers)
		 _defaultController->ref();
}

void SBScene::removeDefaultControllers()
{
	 for (auto & _defaultController : _defaultControllers)
		 _defaultController->unref();
	 _defaultControllers.clear();
}

std::vector<std::string> SBScene::getAssetPaths(const std::string& type)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getAssetPaths() instead.");
	return _assetStore->getAssetPaths(type);
}

std::vector<std::string> SBScene::getLocalAssetPaths(const std::string& type)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.getLocalAssetPaths() instead.");
	return _assetStore->getLocalAssetPaths(type);
}

void SBScene::addAssetPath(const std::string& type, const std::string& path)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addAssetPath() instead.");
	_assetStore->addAssetPath(type, path);
}

void SBScene::removeAssetPath(const std::string& type, const std::string& path)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addAssetPath() instead.");
	_assetStore->removeAssetPath(type, path);
}

void SBScene::removeAllAssetPaths(const std::string& type)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.removeAllAssetPaths() instead.");
	_assetStore->removeAllAssetPaths(type);
}

void SBScene::loadAssets()
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.addAssetPath() instead.");
	_assetStore->loadAssets();
}

void SBScene::loadAsset(const std::string& assetPath)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.loadAsset() instead.");
	_assetStore->loadAsset(assetPath);
}

void SBScene::loadAssetsFromPath(const std::string& assetPath)
{
	if (SHOW_DEPRECATION_MESSAGES)
		SmartBody::util::log("DEPRECATED: Use AssetManager.loadAssetsFromPath() instead.");
	_assetStore->loadAssetsFromPath(assetPath);
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

//void SBScene::addMotions(const std::string& path, bool recursive)
//{
//	if (SHOW_DEPRECATION_MESSAGES)
//		SmartBody::util::log("DEPRECATED: Use AssetManager.addMotion() instead.");
//	getAssetManager()->addMotions(path, recursive);
//}

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
	for (auto & iter : _pawnMap)
	{
		allPawns.emplace_back(iter.second);
	}
	_pawnMap.clear();
	_pawnNames.clear();

	for (auto & allPawn : allPawns)
	{
		_pawnMap.insert(std::pair<std::string, SbmPawn*>(allPawn->getName(), allPawn));
		_pawnNames.emplace_back(allPawn->getName());
	}

}

void SBScene::updateCharacterNames()
{
	std::vector<SbmCharacter*> allCharacters;
	for (auto & iter : _characterMap)
	{
		allCharacters.emplace_back(iter.second);
	}
	_characterMap.clear();
	_characterNames.clear();

	for (auto & allCharacter : allCharacters)
	{
		_characterMap.insert(std::pair<std::string, SbmCharacter*>(allCharacter->getName(), allCharacter));
		_characterNames.emplace_back(allCharacter->getName());
	}
}


Heightfield* SBScene::getHeightfield()
{
	return _heightField;
}


Heightfield* SBScene::createHeightfield()
{
	delete _heightField;
	_heightField = new Heightfield();
	return _heightField;
}

void SBScene::removeHeightfield()
{

	delete _heightField;
	_heightField = nullptr;
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

SrViewer* SBScene::getViewer()
{
	return _viewer;
}

void SBScene::setViewer(SrViewer* viewer)
{
	_viewer = viewer;
}

SrViewerFactory* SBScene::getViewerFactory()
{
	return _viewerFactory;
}

void SBScene::setViewerFactory(SrViewerFactory* viewerFactory)
{
	if (_viewerFactory)
		delete _viewerFactory;
	_viewerFactory = viewerFactory;
}

KinectProcessor* SBScene::getKinectProcessor()
{
	return _kinectProcessor;
}

std::map<std::string, GeneralParam*>& SBScene::getGeneralParameters()
{
	return _generalParams;
}

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

	for (auto& provider : _objectProviders) {
		auto providedString = provider.second.stringProvider(*object);
		if (!providedString.empty()) {
			return provider.first + "/" + providedString;
		}
	}

	return "";
}

void SBScene::registerObjectProvider(std::string prefix, Provider provider) {
	_objectProviders.emplace(std::move(prefix), std::move(provider));
}

void SBScene::removeObjectProvider(const std::string& prefix) {
	_objectProviders.erase(prefix);
}

SmartBody::SBObject* SBScene::getObjectFromString(const std::string& value)
{
	int prefixPos = value.find('/');
	if (prefixPos == std::string::npos)
		return nullptr;

	std::string prefix = value.substr(0, prefixPos);
	std::string suffix = value.substr(prefixPos + 1);

	if (prefix == "character")
	{
		// check for a second level name
		int prefixPos2 = suffix.find('/');
		if (prefixPos2 == std::string::npos)
		{
			SmartBody::SBCharacter* character = this->getCharacter(suffix);
			return character;
		}

		std::string characterName = suffix.substr(0, prefixPos2);
		SmartBody::SBCharacter* character = this->getCharacter(characterName);
		if (!character)
			return nullptr;

		std::string remainder = suffix.substr(prefixPos2 + 1);

		int prefixPos3 = remainder.find('/');
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
					return nullptr;
			}
			
		}
		else if (remainder == "minibrain")
		{
			SmartBody::Nvbg* nvbg = character->getNvbg();
			return nvbg;
		}
		
		return nullptr;
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
	else if (prefix == "controller")
	{
		int prefixPos2 = suffix.find('/');
		if (prefixPos2 == std::string::npos)
			return nullptr;
		std::string prefix2 = suffix.substr(0, prefixPos2 - 1);
		std::string suffix2 = suffix.substr(prefixPos2 + 1);
		SmartBody::SBCharacter* character = this->getCharacter(suffix);
		if (!character)
			return nullptr;
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

	//Check if it can be provided by any registered "object provider".
	auto I = _objectProviders.find(prefix);
	if (I != _objectProviders.end()) {
		return I->second.objectProvider(suffix);
	}
	return nullptr;
}

std::string SBScene::getLastScriptDirectory()
{
	return _lastScriptDirectory;
}

void SBScene::setLastScriptDirectory(std::string dir)
{
	_lastScriptDirectory = std::move(dir);
}

};

