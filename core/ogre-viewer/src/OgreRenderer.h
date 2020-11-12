#ifndef _OGRERENDERER_
#define _OGRERENDERER_

#include "vhcl.h"
#include "bonebus.h"
#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#include <string>
#include <map>

#include <Ogre.h>
#include "ExampleApplication.h"
#include "SBListener.h"
#include "OgreFrameListener.h"


namespace vhcl { class Audio; }


class OgreRenderer : public ExampleApplication
{

	public:
		OgreRenderer();

		void setUseBoneBus(bool val);
		bool isUseBoneBus();

		SceneManager* getSceneManager();
		Camera* getCamera();
		OgreFrameListener* getOgreFrameListener();

		std::map<int,std::map<std::string, Ogre::Vector3> > characterInitBonePosMap;
		std::map<std::string, std::vector<int>*> m_lastPosTimes;
		std::map<std::string, std::vector<int>*> m_lastRotTimes;
		std::map<std::string, std::vector<std::string> > m_skeletonMap;
		std::map<std::string, std::vector<std::string> > m_visemeMap;
		
		std::set<std::string> m_validJointNames;
		std::vector<std::string> m_initialCommands;
		std::string m_initialMeshName;
	
	protected:
		// ExampleApplication callbacks
		void destroyScene(void);
		void createDefaultScene(std::string meshName);
		void createScene();
		void createFrameListener();

		static void tt_client_callback( const char * op, const char * args, void * user_data );

	protected:
		Ogre::SceneNode * sceneNode;
		Ogre::Entity * sceneEntity;

		SBMHANDLE m_sbm;
		bonebus::BoneBusServer* m_bonebus;
		SBListener* m_sbListener;
		bool m_useBoneBus;
		std::string mDebugText;
		double m_startTime;
	public:		
};

#endif
