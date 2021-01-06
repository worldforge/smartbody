#ifndef _SBASSETMANAGER_H_
#define _SBASSETMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBObject.h>
#include <map>
#include <sb/SBAssetHandler.h>
#include "SBAssetStore.h"
#include "sr/sr_shared_ptr.hpp"
#include "SBSceneOwned.h"

class srPathList;
class SbmTexture;

namespace SmartBody {

class SBSkeleton;
class SBMotion;

class SBAssetManager : public SBObject, public SBAssetsProcessor, public SBSceneOwned
{
	public:
		SBAPI explicit SBAssetManager(SBScene& scene, SBAssetStore& store);
		SBAPI ~SBAssetManager();

		SBAPI void removeAllAssets();
		SBAPI double getGlobalMotionScale();
		SBAPI void setGlobalMotionScale(double val);
		SBAPI double getGlobalSkeletonScale();
		SBAPI void setGlobalSkeletonScale(double val);

		SBAPI boost::intrusive_ptr<SBSkeleton> createSkeleton(const std::string&char_name);
		SBAPI boost::intrusive_ptr<SBSkeleton> getSkeleton(const std::string& name);
		SBAPI boost::intrusive_ptr<SBSkeleton> addSkeletonDefinition(const std::string& skelName);
		SBAPI void removeSkeletonDefinition(const std::string& skelName);
		SBAPI void addSkeleton(boost::intrusive_ptr<SBSkeleton> skeleton);
		SBAPI int getNumSkeletons();
		SBAPI std::vector<std::string> getSkeletonNames();
	
//		SBAPI void loadMotions();
		SBAPI bool addMotion(std::unique_ptr<SmartBody::SBMotion> motion);
		SBAPI SBMotion* addMotionDefinition(const std::string& name, double duration, int numFrames);
		SBAPI void removeMotion(SmartBody::SBMotion* motion);
		//SBAPI void addMotions(const std::string& path, bool recursive);
		SBAPI SBMotion* createMotion(const std::string& motionName);
		SBAPI SBMotion* getMotion(const std::string& name);
		SBAPI int getNumMotions();
		SBAPI std::vector<std::string> getMotionNames();


		SBAPI FILE* open_sequence_file( const char *seq_name, std::string& fullPath );

		SBAPI std::string findFileName(const std::string& type, const std::string& filename);



		SBAPI std::string getAssetNameVariation(SBAsset* asset);
		SBAPI std::vector<std::string>& getAssetHistory();
		SBAPI void clearAssetHistory();


		void processAssets(std::vector<std::unique_ptr<SBAsset>>& assets) override;

protected:

		void addAssetHistory(const std::string& str);

		SBAssetStore& _store;

		std::map<std::string, boost::intrusive_ptr<SBSkeleton>> _skeletons;
		std::map<std::string, std::unique_ptr<SBMotion>> _motions;


		SBAssetsHistory _history;
		int uniqueSkeletonId;
		int _motionCounter;
		int _skeletonCounter;

};

}

#endif
