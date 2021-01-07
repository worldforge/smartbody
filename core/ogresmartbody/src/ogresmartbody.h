#ifndef __OgreSmartBody_h_
#define __OgreSmartBody_h_
 
#include <OGRE/Ogre.h>
#include <OGRE/Bites/OgreWindowEventUtilities.h>

// smartbody
#ifndef NDEBUG
#define NDEBUG
#endif

#include <sb/SBScene.h>

class DeformableMeshInstance;

class OgreSmartBody : public OgreBites::WindowEventListener, public Ogre::FrameListener
{
public:
    OgreSmartBody();
    virtual ~OgreSmartBody();
    bool go();

	Ogre::SceneManager* getSceneManager();
 
protected:
    // Ogre::WindowEventListener
    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);
 
    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	void updateOgreLights();
	void updateOgreCamera();
	void ogreBlendShape(Ogre::Entity* sbEntity, DeformableMeshInstance* meshInstance);

	
 
private:
    Ogre::Root* mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
 
    
	double mStartTime;
	bool firstTime;

	// smartbody
	SmartBody::SBScene* m_pScene;
};
 
#endif // #ifndef __OgreSmartBody_h_

