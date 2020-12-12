#pragma once
#ifndef NO_OGRE_VIEWER_CMD
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <Ogre.h>

#define USE_RTSHADER 0
#if USE_RTSHADER
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#endif
//#include <sbm/SBSkeleton.h>
//#include <sbm/SBCharacter.h>
//#include <sbm/sbm_deformable_mesh.h>

class ExampleFrameListener;
class OgreListener;
class DeformableMesh;
class DeformableMeshInstance;

namespace SmartBody
{
	class SBSkeleton;
	class SBCharacter;
	class SBPawn;
}

class EmbeddedOgre
{
public:
	EmbeddedOgre(void);
	~EmbeddedOgre(void);
	void createOgreWindow(void* windowHandle, void* parentHandle, unsigned long glContext, int width, int height, std::string windowName);
	void setupResource(std::string resourceLocation);
	void createDefaultScene();	
	void update();
	void finishRender();;
	void setCharacterVisibility(bool bVisible);
	bool getCharacterVisiblility();
	void updateOgreLights();
	void setLightParams(Ogre::Light* light, SmartBody::SBPawn* sbPawn);
	void updateOgreCharacterRenderMode(bool renderSkinWeight);
	void resetOgreScene();

	Ogre::SceneManager* getSceneManager() { return ogreSceneMgr; }
	Ogre::RenderWindow* getRenderWindow() { return ogreWnd; }
	ExampleFrameListener*  getOgreFrameListener() { return ogreFrameListener; }
	Ogre::Camera*       getCamera() { return ogreCamera; }
	unsigned long getGLContext() { return ogreGLContext; }
	void makeGLContextCurrent();
	Ogre::Entity* createOgreCharacter(SmartBody::SBCharacter* sbChar); // create a ogre character from a smartbody character
	Ogre::MovableObject* createOgrePawn(SmartBody::SBPawn* sbPawn); // create a ogre character from a smartbody character

	void addSBSkeleton(SmartBody::SBSkeleton* skel); // convert a SB skeleton to ogre
	void addDeformableMesh(std::string meshName, DeformableMeshInstance* meshInstance);
	void addTexture(std::string texName);	
	

protected:
	static unsigned long getCurrentGLContext();	
	void setCharacterVisible(bool bVisible, std::string charName);
protected:
	Ogre::Root*           ogreRoot;
	Ogre::RenderWindow*   ogreWnd;
	Ogre::SceneManager*   ogreSceneMgr;
	Ogre::Camera*		  ogreCamera;
	ExampleFrameListener* ogreFrameListener;
#if USE_RTSHADER
	Ogre::RTShader::ShaderGenerator* ogreShaderGenerator;   
	Ogre::RTShader::SubRenderState* ogreSrsHardwareSkinning;
#endif
	OgreListener*         ogreListener;
	unsigned long         ogreGLContext;
	bool                  ogreCharacterVisible;
	bool				  firstTime;
};
#endif