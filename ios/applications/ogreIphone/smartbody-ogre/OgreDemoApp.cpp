#include "OgreDemoApp.h"
#include <iostream>

DemoApp::DemoApp()
{
	m_pCubeNode		= 0;
	m_pCubeEntity   = 0;
    m_sbm = NULL;
    m_sbListener = NULL;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoApp::~DemoApp()
{
#ifdef USE_RTSHADER_SYSTEM
    mShaderGenerator->removeSceneManager(OgreFramework::getSingletonPtr()->m_pSceneMgr);
    
    finalizeRTShaderSystem();
#endif
    
    delete OgreFramework::getSingletonPtr();
    
 	// Send vrProcEnd message to ActiveMQ
	vhmsg::ttu_notify2( "vrProcEnd", "renderer" );
    
	// Close ActiveMQ
	vhmsg::ttu_close();   
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#ifdef USE_RTSHADER_SYSTEM

/*-----------------------------------------------------------------------------
 | Initialize the RT Shader system.	
 -----------------------------------------------------------------------------*/
bool DemoApp::initializeRTShaderSystem(Ogre::SceneManager* sceneMgr)
{			
    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        
        mShaderGenerator->addSceneManager(sceneMgr);
        
        // Setup core libraries and shader cache path.
        Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
        Ogre::StringVector::iterator itGroup = groupVector.begin();
        Ogre::StringVector::iterator itGroupEnd = groupVector.end();
        Ogre::String shaderCoreLibsPath;
        Ogre::String shaderCachePath;
        
        for (; itGroup != itGroupEnd; ++itGroup)
        {
            Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
            Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
            Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
            bool coreLibsFound = false;
            
            // Try to find the location of the core shader lib functions and use it
            // as shader cache path as well - this will reduce the number of generated files
            // when running from different directories.
            for (; it != itEnd; ++it)
            {
                if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
                {
                    shaderCoreLibsPath = (*it)->archive->getName() + "/";
                    shaderCachePath = shaderCoreLibsPath;
                    coreLibsFound = true;
                    break;
                }
            }
            // Core libs path found in the current group.
            if (coreLibsFound) 
                break; 
        }
        
        // Core shader libs not found -> shader generating will fail.
        if (shaderCoreLibsPath.empty())			
            return false;			
        
        // Create and register the material manager listener.
        mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);				
        Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
    }
    
    return true;
}

/*-----------------------------------------------------------------------------
 | Finalize the RT Shader system.	
 -----------------------------------------------------------------------------*/
void DemoApp::finalizeRTShaderSystem()
{
    // Restore default scheme.
    Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
    
    // Unregister the material manager listener.
    if (mMaterialMgrListener != NULL)
    {			
        Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
        delete mMaterialMgrListener;
        mMaterialMgrListener = NULL;
    }
    
    // Finalize RTShader system.
    if (mShaderGenerator != NULL)
    {				
        Ogre::RTShader::ShaderGenerator::finalize();
        mShaderGenerator = NULL;
    }
}
#endif // USE_RTSHADER_SYSTEM

void DemoApp::startDemo()
{
    setupSmartbody();    
    
	new OgreFramework();
	if(!OgreFramework::getSingletonPtr()->initOgre("DemoApp v1.0", this, 0, m_sbm))
		return;
    
	m_bShutdown = false;
    
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");
	
#ifdef USE_RTSHADER_SYSTEM
    initializeRTShaderSystem(OgreFramework::getSingletonPtr()->m_pSceneMgr);
    Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);				
    baseWhite->setLightingEnabled(false);
    mShaderGenerator->createShaderBasedTechnique(
                                                 "BaseWhite", 
                                                 Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
                                                 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
                                       "BaseWhite");
    baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(
                                                             baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(
                                                               baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
    
    // creates shaders for base material BaseWhiteNoLighting using the RTSS
    mShaderGenerator->createShaderBasedTechnique(
                                                 "BaseWhiteNoLighting", 
                                                 Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
                                                 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
                                       "BaseWhiteNoLighting");
    Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(
                                                                       baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(
                                                                         baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
#endif
    
	setupDemoScene();
#if !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	runDemo();
#endif
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoApp::setupDemoScene()
{
    OgreFramework::getSingletonPtr()->m_pSceneMgr->setSkyBox(true, "Examples/SceneSkyBox1");
    
    OgreFramework::getSingletonPtr()->m_pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE );
//	OgreFramework::getSingletonPtr()->m_pSceneMgr->setShadowTextureSize( 4048 );
	OgreFramework::getSingletonPtr()->m_pSceneMgr->setShadowColour( Ogre::ColourValue( 0.3f, 0.3f, 0.3f ) );   
    OgreFramework::getSingletonPtr()->m_pSceneMgr->setAmbientLight( Ogre::ColourValue( 0.2f, 0.2f, 0.2f ) );
    Ogre::Light * l;
    Ogre::Vector3 dir;	
    l = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("WhiteLight");
    l->setType( Ogre::Light::LT_SPOTLIGHT );
    l->setPosition( -150.0f, 450.0f, 200.0f );
    l->setCastShadows( true );
    l->setPowerScale( 1.0 );
    
    dir = -l->getPosition();
    //dir = Vector3( 15, 50, 0 );
    dir.normalise();
    l->setDirection( dir );
    l->setDiffuseColour( 1.24f, 1.22f, 1.15f );
    l->setSpecularColour(0.8f, 0.8f, 0.9f);
    
    Ogre::Light * mR_FillLight;
    mR_FillLight = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("R_FillLight");
    mR_FillLight->setType(Ogre::Light::LT_SPOTLIGHT);
    mR_FillLight->setPosition(1500,100,200);
    mR_FillLight->setSpotlightRange(Ogre::Degree(30), Ogre::Degree(50));
    dir = -mR_FillLight->getPosition();
    dir.normalise();
    mR_FillLight->setDirection(dir);
    mR_FillLight->setDiffuseColour(0.32f, 0.37f, 0.4f);
    mR_FillLight->setSpecularColour(0.32f, 0.37f, 0.4f);
    mR_FillLight->setCastShadows(true);
    
    Ogre::Light * mL_FillLight;
    mL_FillLight = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("L_FillLight");
    mL_FillLight->setType(Ogre::Light::LT_SPOTLIGHT);
    mL_FillLight->setPosition(-1500.0f,100.0f,-100.0f);
    mL_FillLight->setSpotlightRange(Ogre::Degree(30.0f), Ogre::Degree(50.0f));
    dir = -mL_FillLight->getPosition();
    dir.normalise();
    mL_FillLight->setDirection(dir);
    mL_FillLight->setDiffuseColour(0.45f, 0.42f, 0.40f);
    mL_FillLight->setSpecularColour(0.45f, 0.42f, 0.40f);
    mL_FillLight->setCastShadows(true);
    
    Ogre::Light * mBounceLight;
    mBounceLight = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("BounceLight");
    mBounceLight->setType(Ogre::Light::LT_SPOTLIGHT);
    mBounceLight->setPosition(-50.0f,-500.0f,400.0f);
    mBounceLight->setSpotlightRange(Ogre::Degree(30.0f), Ogre::Degree(50.0f));
    dir = -mBounceLight->getPosition();
    dir.normalise();
    mBounceLight->setDirection(dir);
    mBounceLight->setDiffuseColour(0.37f, 0.37f, 0.36f);
    mBounceLight->setSpecularColour(0.37f, 0.37f, 0.36f);    
    mBounceLight->setCastShadows(true);
    
    m_pCubeNode = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("world_scene_vh");
	m_pCubeEntity = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("world_entity_vh","VH_Defaultlevel_Ogre.mesh");
	m_pCubeNode->attachObject(m_pCubeEntity);
	m_pCubeNode->setVisible(true);    
 
    /*
    m_pCubeEntity = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Brad", "Brad.mesh");
    m_pCubeNode = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("Brad");
    m_pCubeNode->attachObject(m_pCubeEntity);
    
    m_pCubeEntity = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Doctor", "Brad.mesh");
    m_pCubeNode = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode("Doctor");
    m_pCubeNode->attachObject(m_pCubeEntity);
    */
}

void DemoApp::setupSmartbody()
{
    vhmsg::ttu_set_client_callback(&DemoApp::tt_client_callback, this);
    
#if 0
    const char* serverName = "172.16.33.21";
    const char* scope = "DEFAULT_SCOPE";
    const char* port = "61616";  
	int err = vhmsg::ttu_open(serverName,scope,port); 
	if ( err != vhmsg::TTU_SUCCESS )
	{
		printf("%s", "ttu_open failed!\n" );
	}
    else
    {
        printf("%s", "ttu_open success!\n");
    }
    
	// Register with ActiveMQ
	vhmsg::ttu_register( "vrAllCall" );
	vhmsg::ttu_register( "vrKillComponent" );
    
	// Send vrComponent message to ActiveMQ
	vhmsg::ttu_notify2( "vrComponent", "renderer Ogre" );   
	vhmsg::ttu_notify2( "vrComponent", "Lauching Ogre" );
    vhmsg::ttu_register( "vrAllCall" );
    vhmsg::ttu_register( "vrKillComponent" ); 
    vhmsg::ttu_register( "sbm" );
    vhmsg::ttu_register( "vrAgentBML" );
    vhmsg::ttu_register( "vrSpeak" );
    vhmsg::ttu_register( "vrExpress" );
    vhmsg::ttu_register( "vrSpoke" );
    vhmsg::ttu_register( "RemoteSpeechReply" );
    vhmsg::ttu_register( "PlaySound" );
    vhmsg::ttu_register( "StopSound" );
    vhmsg::ttu_register( "CommAPI" );
    vhmsg::ttu_register( "object-data" );
#endif    
    
    m_sbListener = new SBListener(this);
    m_sbm = new Smartbody_dll;
    m_sbm->Init("", true);
    m_sbm->SetListener(m_sbListener);
    
    
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoApp::runDemo()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Start main loop...");
	
	double timeSinceLastFrame = 0;
	double startTime = 0;
    
    OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
    
#if (!defined(OGRE_IS_IOS)) && !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	while(!m_bShutdown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown()) 
	{
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;
        
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			startTime = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
            
#if !OGRE_IS_IOS
			OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
#endif
			OgreFramework::getSingletonPtr()->m_pMouse->capture();
            
			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			OgreFramework::getSingletonPtr()->m_pRoot->renderOneFrame();
            
			timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
		}
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
		}
	}
#endif
    
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
#endif
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoApp::keyPressed(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
	
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F))
	{
        //do something
	}
#endif
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoApp::keyReleased(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
#endif

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework* DemoApp::getOgreFramework()
{
    return OgreFramework::getSingletonPtr();
}

Ogre::SceneManager* DemoApp::getSceneManager()
{
    return OgreFramework::getSingletonPtr()->m_pSceneMgr;
}


void DemoApp::tt_client_callback( const char * op, const char * args, void * user_data )
{
    DemoApp * app = (DemoApp *)user_data;    
    app->m_sbm->ProcessVHMsgs(op, args);
}
