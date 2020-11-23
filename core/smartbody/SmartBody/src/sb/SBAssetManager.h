#ifndef _SBASSETMANAGER_H_
#define _SBASSETMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBObject.h>
#include <map>
#include <sb/SBAssetHandler.h>
#include "SBAssetStore.h"

class srPathList;
class SbmTexture;

namespace SmartBody {

class SBSkeleton;
class SBMotion;
class SBNavigationMesh;

class SBAssetManager : public SBObject
{
	public:
		SBAPI explicit SBAssetManager(SBAssetStore& store);
		SBAPI ~SBAssetManager();

		SBAPI void removeAllAssets();
		SBAPI double getGlobalMotionScale();
		SBAPI void setGlobalMotionScale(double val);
		SBAPI double getGlobalSkeletonScale();
		SBAPI void setGlobalSkeletonScale(double val);

		//SBAPI void addAssetPath(const std::string& type, const std::string& path);
//		SBAPI std::vector<std::string> getAssetPaths(const std::string& type);
//		SBAPI std::vector<std::string> getLocalAssetPaths(const std::string& type);
//		SBAPI void removeAssetPath(const std::string& type, const std::string& path);
//		SBAPI void removeAllAssetPaths(const std::string& type);
//		SBAPI void loadAssets();
//		SBAPI std::vector<std::unique_ptr<SBAsset>> loadAsset(const std::string& assetPath);
//		SBAPI void loadAssetsFromPath(const std::string& assetPath);
//		SBAPI std::string findAsset(const std::string& type, const std::string& assetName);

		SBAPI SBSkeleton* createSkeleton(const std::string&char_name);
		SBAPI SBSkeleton* getSkeleton(const std::string& name);
		SBAPI SBSkeleton* addSkeletonDefinition(const std::string& skelName);
		SBAPI void removeSkeletonDefinition(const std::string& skelName);
		SBAPI void addSkeleton(std::unique_ptr<SmartBody::SBSkeleton> skeleton);
		SBAPI int getNumSkeletons();
		SBAPI std::vector<std::string> getSkeletonNames();
	
//		SBAPI void loadMotions();
		SBAPI bool addMotion(std::unique_ptr<SmartBody::SBMotion> motion);
		SBAPI SBMotion* addMotionDefinition(const std::string& name, double duration, int numFrames);
		SBAPI void removeMotion(SmartBody::SBMotion* motion);
		SBAPI void addMotions(const std::string& path, bool recursive);
		SBAPI SBMotion* createMotion(const std::string& motionName);
		SBAPI SBMotion* getMotion(const std::string& name);
		SBAPI int getNumMotions();
		SBAPI std::vector<std::string> getMotionNames();

//		SBAPI int load_motion( const void* data, int sizeBytes, const char* motionName );

		//TODO: remove it not used
		//SBAPI int load_skeleton( const void* data, int sizeBytes, const char* skeletonName );

		SBAPI FILE* open_sequence_file( const char *seq_name, std::string& fullPath );

		SBAPI const std::string findFileName(const std::string& type, const std::string& filename);



		SBAPI std::string getAssetNameVariation(SBAsset* asset);
		SBAPI std::vector<std::string>& getAssetHistory();
		SBAPI void clearAssetHistory();

//		SBAPI bool handlePenetrations(std::string deformableMesh, std::string baseModel, std::string penetratingModel, float offset, bool showVisualization);
//		SBAPI bool addBlendshapeToModel(std::string templateMeshName, std::string modelFile, std::string shapeName, std::string submeshName);
//		SBAPI bool addModelToMesh(std::string templateMeshName, std::string modelFile, std::string newModelName, std::string rigidBindJoint, std::string bindPoseCopySubMeshName);
		
protected:

//		std::string findAssetFromLocation(const std::string& filepath, const std::string& assetName);
		//SmartBody::SBSkeleton* load_skeleton( const char *filename, srPathList &path_list, double scale = 1.0 );
	
//		int load_skeletons( const char* pathname, bool recursive );
//		int load_motions( const char* pathname, bool recursive );
//
//		int load_me_motions( const char* pathname, bool recurse_dirs, double scale );
//		int load_me_motions_impl( const std::string& pathname, bool recurse_dirs, double scale, const char* error_prefix);
//		int load_me_motion_individual( SrInput & input, const std::string & motionName, std::map<std::string, std::unique_ptr<SBMotion>>& map, double scale );
//
//		int load_me_skeleton_individual( SrInput & input, const std::string & skeletonName, std::map<std::string, std::unique_ptr< SmartBody::SBSkeleton>>& map, double scale = 1.0 );
//		int load_me_skeletons( const char* pathname, std::map<std::string, std::unique_ptr<SmartBody::SBSkeleton>>& map, bool recursive, double scale = 1.0 );
//		int load_me_skeletons_impl( const std::string& pathname, std::map<std::string, std::unique_ptr<SmartBody::SBSkeleton>>& map, bool recurse_dirs, double scale, const char* error_prefix );

		void addAssetHistory(const std::string& str);

		SBAssetStore& _store;

		std::map<std::string, std::unique_ptr<SBSkeleton>> _skeletons;
		std::map<std::string, std::unique_ptr<SBMotion>> _motions;
//
//		srPathList*	seq_paths;
//		srPathList*	me_paths;
//		srPathList*	audio_paths;
//		srPathList*	mesh_paths;

		SBAssetsHistory _history;
		int uniqueSkeletonId;
		int _motionCounter;
		int _skeletonCounter;
//		int _meshCounter;
//		std::vector<std::string> _assetHistory;

};

}

#endif
