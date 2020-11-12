#ifndef _OGRELISTENER_H
#define _OGRELISTENER_H

#include "vhcl.h"
#include "bonebus.h"
#include "sb/smartbody-c-dll.h"
#include "ExampleApplication.h"
#include "SBListener.h"

// Event handler to animate
class OgreFrameListener : public ExampleFrameListener
{
	public:
		OgreFrameListener(RenderWindow * win, Camera * cam, const std::string & debugText, SceneManager * mgr, bonebus::BoneBusServer * bonebus, SBMHANDLE sbmdll, SBListener * sbListener, std::vector<std::string>& initialCommands);
		void windowFocusChange( RenderWindow * rw );
		bool processUnbufferedKeyInput(const FrameEvent& evt);
		virtual bool processUnbufferedMouseInput( const FrameEvent & evt );
		bool frameStarted( const FrameEvent & evt );
		void scheduleQuit(void);
		void SetOgreMouse( const bool enabled );							  
		std::vector<std::string>	m_characterList;
		std::vector<std::string>    m_pawnList;
		std::map<std::string, std::map<std::string, Ogre::Vector3> > m_initialBonePositions;

		std::set<std::string> m_validJointNames;

protected:
		SceneManager * mSceneMgr;
		bonebus::BoneBusServer * m_bonebus;
		SBMHANDLE m_sbmDLL;
		SBListener* m_sbListener;
		vhcl::Timer* m_timer;
		std::vector<std::string> m_initialCommands;

		bool m_ogreMouseEnabled;
		

		bool mQuit;
};

	

#endif
