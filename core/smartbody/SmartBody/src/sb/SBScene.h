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

#ifndef SBSCENE_H
#define SBSCENE_H

#include <sb/SBTypes.h>
#include <sb/SBObject.h>
#include <sb/SBScript.h>
#include "SBUtilities.h"
#include <map>
#include <sstream>
#include <sr/sr_viewer.h>
#include <sbm/general_param_setting.h>


#include <memory>
#include <functional>

class SrSnGroup;
class SbmPawn;
class SbmCharacter;
class Heightfield;
class KinectProcessor;
class SkJoint;

namespace SmartBody {

class SBSceneListener;
class SBPawn;
class SBCharacter;
class SBSkeleton;
class SBFaceDefinition;
class SBMotion;
class SBEventManager;
class SBSimulationManager;
class SBProfiler;
class SBBmlProcessor;
class SBAnimationBlendManager;
class SBReachManager;
class SBSteerManager;
class SBRealtimeManager;
class SBServiceManager;
class SBPhysicsManager;
class SBBoneBusManager;
class SBGestureMapManager;
class SBJointMapManager;
class SBCollisionManager;
class SBPhonemeManager;
class SBBehaviorSetManager;
class SBRetargetManager;
class SBAssetManager;
class SBSpeechManager;
class SBNavigationMeshManager;
class SBParser;
class SBSubject;
class SBController;
class SBVHMsgManager;
class SBCommandManager;
class SBNavigationMesh;
class SBMotionGraphManager;
class SBHandConfigurationManager;
//class SBDebuggerServer;
//class SBDebuggerClient;
class SBAssetStore;

class SBScene : public SBObject
{
	public:

		struct Provider {
			std::function<SmartBody::SBObject*(const std::string&)> objectProvider;
			std::function<std::string(SmartBody::SBObject&)> stringProvider;
		};

		SBAPI SBScene();
		SBAPI ~SBScene();

		SBAPI void setProcessId(const std::string& id);
		SBAPI const std::string& getProcessId();

		SBAPI void setMediaPath(const std::string& path);
		SBAPI const std::string& getMediaPath();

		SBAPI void update();

		SBAPI std::string getStringFromObject(SmartBody::SBObject* object);
		SBAPI SmartBody::SBObject* getObjectFromString(const std::string& value);

//		SBAPI std::string save(bool remoteSetup = false, std::string mediaPath = "");
//		SBAPI std::string saveSceneSetting();
//		SBAPI std::string exportScene(const std::vector<std::string>& aspects, std::string mediaPath = "", bool remoteSetup = false);

#if !defined(__FLASHPLAYER__) && !defined(EMSCRIPTEN)
//		SBAPI void exportScenePackage(std::string outDir, std::string outZipArchiveName = "");
//		SBAPI void exportCharacter(std::string charName, std::string outDir);
#endif

		SBAPI static SBScene* getScene();		
		SBAPI static void destroyScene();

		SBAPI void setScale(float val);
		SBAPI float getScale();

		SBAPI bool command(const std::string& command);
		SBAPI bool commandAt(float seconds, const std::string& command);
		SBAPI void sendVHMsg(const std::string& message);
		SBAPI void sendVHMsg2(const std::string&, const std::string& encodedMessage);		

		SBAPI SBCharacter* createCharacter(const std::string&, const std::string&);
		SBAPI SBCharacter* copyCharacter(const std::string& origCharacter, const std::string& newCharacter);
		SBAPI SBPawn* createPawn(const std::string& pawnName);
		SBAPI SBPawn* copyPawn(const std::string& origPawn, const std::string& newPawn);

		SBAPI void removeAllCharacters();
		SBAPI void removeCharacter(const std::string& charName);
		SBAPI void removePawn(const std::string& pawnName);
		SBAPI void removeAllPawns();
		SBPawn* insertPawn(SBPawn* pawn);
		SBAPI int getNumCharacters();
		SBAPI int getNumPawns();
		SBAPI SBCharacter* getCharacter(const std::string& name);
		SBAPI SBPawn* getPawn(const std::string& name);

		SBAPI SBFaceDefinition* createFaceDefinition(const std::string& name);
		SBAPI void removeFaceDefinition(const std::string& name);
		SBAPI SBFaceDefinition* getFaceDefinition(const std::string& name);
		SBAPI int getNumFaceDefinitions();
		SBAPI std::vector<std::string> getFaceDefinitionNames();

		SBAPI const std::vector<std::string>& getPawnNames();
		SBAPI const std::vector<std::string>& getCharacterNames();

		SBAPI void removePendingCommands();

		SBAPI std::vector<std::string> getEventHandlerNames();

		SBAPI void startFileLogging(const std::string& filename);
		SBAPI void stopFileLogging();

		/**
		* HACK: executes commands, which are Python scripts. This should be moved out of the core Scene instance, but we'll keep it for now to avoid updating all scripts.
		*/
		SBAPI bool run(const std::string& command);
		SBAPI bool runScript(const std::string& script);

		SBAPI void setDefaultCharacter(const std::string& character);
		SBAPI void setDefaultRecipient(const std::string& recipient);

		//SBAPI void reset();

		SBAPI std::string getLastScriptDirectory();
		SBAPI void setLastScriptDirectory(std::string dir);

		SBAPI void addScript(const std::string& name, SBScript* script);
		SBAPI void removeScript(const std::string& name);
		SBAPI int getNumScripts();
		SBAPI SBScript* getScript(const std::string& name);
		SBAPI std::vector<std::string> getScriptNames();
		SBAPI std::map<std::string, SBScript*>& getScripts();

		SBAPI SBEventManager* getEventManager();		
		SBAPI SBSimulationManager* getSimulationManager();
		SBAPI SBProfiler* getProfiler();
		SBAPI SBBmlProcessor* getBmlProcessor();
		SBAPI SBAnimationBlendManager* getBlendManager();
		SBAPI SBReachManager* getReachManager();
		SBAPI SBSteerManager* getSteerManager();
		SBAPI SBRealtimeManager* getRealtimeManager();
		SBAPI SBServiceManager* getServiceManager();
		SBAPI SBPhysicsManager* getPhysicsManager();
		SBAPI SBBoneBusManager* getBoneBusManager();
		SBAPI SBGestureMapManager* getGestureMapManager();
		SBAPI SBJointMapManager* getJointMapManager();
		SBAPI SBCollisionManager* getCollisionManager();
		SBAPI SBPhonemeManager* getDiphoneManager();
		SBAPI SBBehaviorSetManager* getBehaviorSetManager();
		SBAPI SBRetargetManager* getRetargetManager();
		SBAPI SBAssetManager* getAssetManager();
		SBAPI SBSpeechManager* getSpeechManager();
		SBAPI SBVHMsgManager* getVHMsgManager();
		SBAPI SBCommandManager* getCommandManager();
		SBAPI SBNavigationMeshManager* getNavigationMeshManager();
		SBAPI SBMotionGraphManager* getMotionGraphManager();
		SBAPI SBHandConfigurationManager* getHandConfigurationManager();

		SBAPI SBParser* getParser();

		//SBAPI SBDebuggerServer * getDebuggerServer();
		//SBAPI SBDebuggerClient * getDebuggerClient();
		//SBAPI SBDebuggerUtility* getDebuggerUtility();
		SBAPI bool isRemoteMode();
		SBAPI void setRemoteMode(bool val);

		SBAPI void addSceneListener(SBSceneListener* listener);
		SBAPI std::vector<SBSceneListener*>& getSceneListeners();
		SBAPI void removeSceneListener(SBSceneListener* listener);
		SBAPI void removeAllSceneListeners();

		SBAPI void notify(SBSubject* subject);

		SBAPI static void setSystemParameter(const std::string& name, const std::string& value);
		SBAPI static std::string getSystemParameter(const std::string& name);
		SBAPI static void removeSystemParameter(const std::string& name);
		SBAPI static void removeAllSystemParameters();
		SBAPI static std::vector<std::string> getSystemParameterNames();

//		SBAPI SrCamera* createCamera(const std::string& name);
//		SBAPI void removeCamera(SrCamera* camera);
//		SBAPI void setActiveCamera(SrCamera* camera);
//		SBAPI SrCamera* getActiveCamera();
//		SBAPI SrCamera* getCamera(const std::string& name);
//		SBAPI void SetCameraLocked(bool locked);
//		SBAPI bool IsCameraLocked();
//		SBAPI int getNumCameras();
//		SBAPI std::vector<std::string> getCameraNames();
//		SBAPI bool hasCameraTrack();
//		SBAPI void setCameraTrack(const std::string& characterName, const std::string& jointName);
//		SBAPI void removeCameraTrack();
//		SBAPI void updateTrackedCameras();

//		SBAPI void updateConeOfSight();
//		SBAPI bool setCameraConeOfSight(const std::string& characterName);
//		SBAPI bool hasConeOfSight();
//		SBAPI void removeConeOfSight();
//
//		SBAPI std::vector<std::string> checkVisibility(const std::string& characterName);
//		SBAPI std::vector<std::string> checkVisibility_current_view();
//		std::vector<std::string> occlusionTest(const std::vector<std::string>& testPawns);
//		std::vector<std::string> frustumTest(const std::vector<std::string>& testPawnNames);

		// deprecated
		SBAPI SBSkeleton* createSkeleton(const std::string&char_name);
		SBAPI SBSkeleton* getSkeleton(const std::string& name);		
		SBAPI void addAssetPath(const std::string& type, const std::string& path);
		SBAPI std::vector<std::string> getAssetPaths(const std::string& type);
		SBAPI std::vector<std::string> getLocalAssetPaths(const std::string& type);
		SBAPI void removeAssetPath(const std::string& type, const std::string& path);
		SBAPI void removeAllAssetPaths(const std::string& type);
		SBAPI void loadAssets();
		SBAPI void loadAsset(const std::string& assetPath);
		SBAPI void loadAssetsFromPath(const std::string& assetPath);
		SBAPI void loadMotions();
		SBAPI void addMotions(const std::string& path, bool recursive);
		SBAPI SBMotion* addMotionDefinition(const std::string& motionName, double duration, int numFrames );
		SBAPI SBSkeleton* addSkeletonDefinition(const std::string& skelName);
		SBAPI void removeSkeletonDefinition(const std::string& skelName);
		SBAPI SBMotion* createMotion(const std::string& motionName);
		SBAPI SBMotion* getMotion(const std::string& name);
		SBAPI int getNumMotions();
		SBAPI std::vector<std::string> getMotionNames();
		SBAPI int getNumSkeletons();
		SBAPI std::vector<std::string> getSkeletonNames();

		SBAPI SrSnGroup* getRootGroup();
		SBAPI std::string getValidName(const std::string& name);
		SBAPI void updatePawnNames();
		SBAPI void updateCharacterNames();

		SBAPI void rescalePartialMeshSkeleton(const std::string& meshName, const std::string& skelName, const std::string& rootJointName, const std::vector<std::string>& skipMeshNames, float scaleRatio, float blendThreshold = 0.025f);

		//bool  blendScalePos(SrVec &v, SrVec &rootPos, int rootIdx, int headIdx, float blendThreshold, float scaleRatio);

#if 0 // should be creating navigation mesh directly in AssetManager
		SBAPI bool createNavigationMesh(const std::string& meshfilename);
#endif
		SBAPI void setNavigationMesh(const std::string& naviMeshName);
		SBAPI SBNavigationMesh* getNavigationMesh();
		
		SBAPI Heightfield* createHeightfield();
		SBAPI void removeHeightfield();
		SBAPI Heightfield* getHeightfield();
		SBAPI float queryTerrain( float x, float z, float *normal_p );

		SBAPI KinectProcessor* getKinectProcessor();
		SBAPI std::map<std::string, GeneralParam*>& getGeneralParameters();

		SBAPI SrViewer* getViewer();
		SBAPI SrViewer* getOgreViewer();
		SBAPI void setViewer(SrViewer* viewer);
		SBAPI void setOgreViewer(SrViewer* viewer);
		SBAPI void setViewerFactory(SrViewerFactory* viewerFactory);
		SBAPI void setOgreViewerFactory(SrViewerFactory* viewerFactory);
		SBAPI SrViewerFactory* getViewerFactory();
		SBAPI SrViewerFactory* getOgreViewerFactory();
		SBAPI std::vector<SBController*>& getDefaultControllers();

		/**
		 * Registers a provider which allows lookup of object through the "prefix/suffix" notation.
		 */
		void registerObjectProvider(std::string prefix, Provider provider);
		void removeObjectProvider(const std::string& prefix);

		//HACK: emitted when all pawns are removed. Used to allow SBRenderScene to know when to clear all cameras. For now.
		std::function<void()> _removeAllPawnsCallback;

		SBAssetStore& getAssetStore() {
			return *_assetStore;
		}

	void setCommandRunner(std::function<bool(const std::string&)> commandRunner);
	void setScriptRunner(std::function<bool(const std::string&)> commandRunner);

	protected:

		void cleanup();
		void createDefaultControllers();
		void removeDefaultControllers();

		std::unique_ptr<SBAssetStore> _assetStore;

		SBSimulationManager* _sim;
		SBProfiler* _profiler;
		SBBmlProcessor* _bml;
		SBAnimationBlendManager* _blendManager;
		SBReachManager* _reachManager;
		SBSteerManager* _steerManager;
		SBRealtimeManager* _realtimeManager;
		SBServiceManager* _serviceManager;
		SBPhysicsManager* _physicsManager;
		SBBoneBusManager* _boneBusManager;
		SBGestureMapManager* _gestureMapManager;
		SBJointMapManager* _jointMapManager;
		SBCollisionManager* _collisionManager;
		SBPhonemeManager* _phonemeManager;
		SBRetargetManager* _retargetManager;
		SBEventManager* _eventManager;
		SBAssetManager* _assetManager;
		SBSpeechManager* _speechManager;
		SBVHMsgManager* _vhmsgManager;
		SBCommandManager* _commandManager;
		SBNavigationMeshManager* _naviMeshManager;
		SBMotionGraphManager* _motionGraphManager;
		SBHandConfigurationManager* _handConfigManager;

		SBParser* _parser;

		std::vector<SBSceneListener*> _sceneListeners;

		SBBehaviorSetManager* _behaviorSetManager;

		std::map<std::string, SBScript*> _scripts;
		float _scale;
		bool _isRemoteMode;
//		bool _isCameraLocked;
		static bool _firstTime;

//		SBDebuggerServer*	_debuggerServer;
//		SBDebuggerClient*	_debuggerClient;
//		SBDebuggerUtility*	_debuggerUtility;
//		std::map<std::string, SrCamera*> _cameras;
//		std::string _activeCamera;
//		std::vector<CameraTrack*> _cameraTracking;

		std::map<std::string, SmartBody::SBFaceDefinition*> _faceDefinitions;

		std::string _mediaPath;
		std::vector<SBController*> _defaultControllers;
		
		std::string _processId;
		static SBScene* _scene;
		static std::map<std::string, std::string> _systemParameters;

		SrSnGroup* _rootGroup;
		std::map<std::string, SbmPawn*>	_pawnMap;
		std::vector<std::string> _pawnNames;
		std::map<std::string, SbmCharacter*> _characterMap;
		std::vector<std::string> _characterNames;

		SrViewer* _viewer;
		SrViewer* _ogreViewer;
		SrViewerFactory* _viewerFactory;
		SrViewerFactory* _ogreViewerFactory;

		SmartBody::util::Listener* _logListener;

		KinectProcessor* _kinectProcessor;
		Heightfield* _heightField;
		SBNavigationMesh* _navigationMesh;
		std::map<std::string, GeneralParam*> _generalParams;

		std::string _lastScriptDirectory;

		/**
		 * Allows for lookup of objects through the "prefix/suffix" notation.
		 */
		std::map<std::string, Provider> _objectProviders;

		/**
		 * HACK: executes commands, which are Python scripts. This should be moved out of the core Scene instance, but we'll keep it for now to avoid updating all scripts.
		 */
		std::function<bool(const std::string&)> _commandRunner;

		/**
		 * HACK: executes scripts. This should be moved out of the core Scene instance, but we'll keep it for now to avoid updating all scripts.
		 */
		std::function<bool(const std::string&)> _scriptRunner;
};


};



#endif
