#include "vhcl.h"

#include "OgreRenderer.h"
#include "OgreFrameListener.h"
#include "BoneBusHandler.h"
#include "SBUtilities.h"

OgreRenderer::OgreRenderer() : ExampleApplication()
{
	m_sbm = NULL;
	m_bonebus = NULL;
	m_useBoneBus = true;
	m_sbListener = NULL;
}

void OgreRenderer::setUseBoneBus(bool val)
{
	m_useBoneBus = val;
}

bool OgreRenderer::isUseBoneBus()
{
	return m_useBoneBus;
}

void OgreRenderer::destroyScene(void)
{
	// Send vrProcEnd message to ActiveMQ
	vhmsg::ttu_notify2( "vrProcEnd", "renderer" );

	// Close ActiveMQ
	vhmsg::ttu_close();
}

void OgreRenderer::createDefaultScene(std::string meshName)
{
	mSceneMgr->setShadowTechnique( SHADOWTYPE_TEXTURE_MODULATIVE );
	mSceneMgr->setShadowTextureSize( 1024 );
	mSceneMgr->setShadowColour( ColourValue( 0.3f, 0.3f, 0.3f ) );

	// Setup animation default
	Animation::setDefaultInterpolationMode( Animation::IM_LINEAR );
	Animation::setDefaultRotationInterpolationMode( Animation::RIM_LINEAR );

	// Set ambient light
	mSceneMgr->setAmbientLight( ColourValue( 0.2f, 0.2f, 0.2f ) );
	//mSceneMgr->setAmbientLight( ColourValue::Black );
	

	//mSceneMgr->setSkyBox( true, "Examples/CloudyNoonSkyBox" );
	//mSceneMgr->setSkyBox( true, "Examples/SceneSkyBox2" );

	// Give it a little ambience with lights

#if 1
          Light * l;
          Vector3 dir;

          l = mSceneMgr->createLight( "WhiteLight" );
          l->setType( Light::LT_SPOTLIGHT );
          l->setPosition( -150.0f, 450.0f, 200.0f );
          l->setCastShadows( true );
          l->setPowerScale( 1.0 );
          
          dir = -l->getPosition();
          //dir = Vector3( 15, 50, 0 );
          dir.normalise();
          l->setDirection( dir );
          l->setDiffuseColour( 1.24f, 1.22f, 1.15f );
          l->setSpecularColour(0.8f, 0.8f, 0.9f);

          Light * mR_FillLight;
          mR_FillLight = mSceneMgr->createLight("R_FillLight");
          mR_FillLight->setType(Light::LT_SPOTLIGHT);
          mR_FillLight->setPosition(1500,100,200);
          mR_FillLight->setSpotlightRange(Degree(30), Degree(50));
          dir = -mR_FillLight->getPosition();
          dir.normalise();
          mR_FillLight->setDirection(dir);
          mR_FillLight->setDiffuseColour(0.32f, 0.37f, 0.4f);
          mR_FillLight->setSpecularColour(0.32f, 0.37f, 0.4f);

          Light * mL_FillLight;
          mL_FillLight = mSceneMgr->createLight("L_FillLight");
          mL_FillLight->setType(Light::LT_SPOTLIGHT);
          mL_FillLight->setPosition(-1500.0f,100.0f,-100.0f);
          mL_FillLight->setSpotlightRange(Degree(30.0f), Degree(50.0f));
          dir = -mL_FillLight->getPosition();
          dir.normalise();
          mL_FillLight->setDirection(dir);
          mL_FillLight->setDiffuseColour(0.45f, 0.42f, 0.40f);
          mL_FillLight->setSpecularColour(0.45f, 0.42f, 0.40f);

          Light * mBounceLight;
          mBounceLight = mSceneMgr->createLight("BounceLight");
          mBounceLight->setType(Light::LT_SPOTLIGHT);
          mBounceLight->setPosition(-50.0f,-500.0f,400.0f);
          mBounceLight->setSpotlightRange(Degree(30.0f), Degree(50.0f));
          dir = -mBounceLight->getPosition();
          dir.normalise();
          mBounceLight->setDirection(dir);
          mBounceLight->setDiffuseColour(0.37f, 0.37f, 0.36f);
          mBounceLight->setSpecularColour(0.37f, 0.37f, 0.36f);

		  Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
		  directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
		  directionalLight->setDiffuseColour(Ogre::ColourValue(.25, .25, 0));
		  directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0));
	 
		  directionalLight->setDirection(Ogre::Vector3( 0, -1, 1 ));	
	          
#endif

	// Position the camera
	mCamera->setPosition( 0, 140, 225 );
	mCamera->lookAt( 0, 92, 0 );

	// Open ActiveMQ
	vhmsg::ttu_set_client_callback( &OgreRenderer::tt_client_callback, this );

	int err = vhmsg::ttu_open();
	if ( err != vhmsg::TTU_SUCCESS )
	{
		printf("%s", "ttu_open failed!\n" );
	}

	// Register with ActiveMQ
	vhmsg::ttu_register( "vrAllCall" );
	vhmsg::ttu_register( "vrKillComponent" );

	// Send vrComponent message to ActiveMQ
	vhmsg::ttu_notify2( "vrComponent", "renderer Ogre" );


	//adding plane entity to the scene
	Plane plane;
	plane.normal = Vector3::UNIT_Y;
	plane.d = 0;
	MeshManager::getSingleton().createPlane( "Myplane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20, true, 1, 60, 60, Vector3::UNIT_Z );
	Entity * pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	//pPlaneEnt->setMaterialName( "Examples/Rockwall" );
	pPlaneEnt->setMaterialName( "Rockwall" );
	pPlaneEnt->setCastShadows( false );
	mSceneMgr->getRootSceneNode()->createChildSceneNode("plane_node", Vector3( 0, 0, 0 ) )->attachObject( pPlaneEnt );
	mSceneMgr->getSceneNode("plane_node")->setVisible(false);

	// adding diagnostic scene to measure in feet
	sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("world_scene_ft");
	sceneEntity = mSceneMgr->createEntity("world_entity_ft","Diagnostic_Level(ft).mesh");
	sceneNode->attachObject(sceneEntity);
	sceneNode->setVisible(false);

	// adding diagnostic scene to measure in cm
	sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("world_scene_cm");
	sceneEntity = mSceneMgr->createEntity("world_entity_cm","Diagnostic_Level(meter_in_cm).mesh");
	sceneNode->attachObject(sceneEntity);
	sceneNode->setVisible(false);

	// adding generic scene if specified
	if (meshName != "")
	{
		sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("world_scene");
		sceneEntity = mSceneMgr->createEntity("world_entity", meshName);
		sceneNode->attachObject(sceneEntity);
		sceneNode->setVisible(true); // if the user specifies a scene to load, make it the default
	}
	else
	{
		mSceneMgr->getSceneNode("plane_node")->setVisible(true); // if the user doesn't specify a scene to load, make it the default
	}
	
}

// Just override the mandatory create scene method
void OgreRenderer::createScene()
{

	createDefaultScene(m_initialMeshName);

	if (m_useBoneBus)
	{
		m_bonebus = new bonebus::BoneBusServer();
		m_bonebus->SetOnClientConnectCallback( BoneBusHandler::OnClientConnect, this );
		m_bonebus->SetOnCreateCharacterFunc( BoneBusHandler::OnCreateCharacter, this );
		m_bonebus->SetOnDeleteCharacterFunc( BoneBusHandler::OnDeleteCharacter, this );
		m_bonebus->SetOnUpdateCharacterFunc( BoneBusHandler::OnUpdateCharacter, this );
		m_bonebus->SetOnSetCharacterPositionFunc( BoneBusHandler::OnSetCharacterPosition, this );
		m_bonebus->SetOnSetCharacterRotationFunc( BoneBusHandler::OnSetCharacterRotation, this );
		m_bonebus->SetOnBoneRotationsFunc( BoneBusHandler::OnBoneRotations, this );
		m_bonebus->SetOnBonePositionsFunc( BoneBusHandler::OnBonePositions, this );
		m_bonebus->SetOnBoneIdFunc( BoneBusHandler::OnBoneId, this );
		m_bonebus->SetOnVisemeIdFunc( BoneBusHandler::OnVisemeId, this );
		m_bonebus->OpenConnection();

		// ask SmartBody to connect to this server if it hasn't already done so
		vhmsg::ttu_notify2("sbm", "net_check");
	}
	else
	{
		m_sbListener = new SBListener(this);
		m_sbm = SBM_CreateSBM();
		SBM_Init(m_sbm, "../../../Python27/Lib", true);
		SBM_SBVHMsgManager_SetEnable(m_sbm, true);
	}

	vhmsg::ttu_notify2( "vrComponent", "renderer all" );

	// sbm related vhmsgs
    vhmsg::ttu_register( "sb" );
    vhmsg::ttu_register( "sbm" );
    vhmsg::ttu_register( "vrAllCall" );
    vhmsg::ttu_register( "vrKillComponent" );
    vhmsg::ttu_register( "vrAgentBML" );
    vhmsg::ttu_register( "vrSpeak" );
    vhmsg::ttu_register( "vrExpress" );
	vhmsg::ttu_register( "vrPerception" );
	vhmsg::ttu_register( "vrBCFeedback" );
	vhmsg::ttu_register( "vrSpeech" );
    vhmsg::ttu_register( "vrSpoke" );
    vhmsg::ttu_register( "RemoteSpeechReply" );
    vhmsg::ttu_register( "PlaySound" );
    vhmsg::ttu_register( "StopSound" );
    vhmsg::ttu_register( "CommAPI" );
    vhmsg::ttu_register( "object-data" );
	vhmsg::ttu_register( "sbmdebugger" );
}


// Create new frame listener
void OgreRenderer::createFrameListener()
{
	mFrameListener = new OgreFrameListener( mWindow, mCamera, mDebugText, mSceneMgr, m_bonebus, m_sbm, m_sbListener, m_initialCommands);
	mRoot->addFrameListener( mFrameListener );
}




void OgreRenderer::tt_client_callback( const char * op, const char * args, void * user_data )
{
   OgreRenderer * app = (OgreRenderer *)user_data;

   //NILOG( "ActiveMQ message received: '%s %s'\n", op, args );

   std::string sOp = op;
   std::string sArgs = args;
   std::vector< std::string > splitArgs;
   SmartBody::util::tokenize( sArgs, splitArgs );

   if ( sOp == "vrAllCall" )
   {
	  vhmsg::ttu_notify2( "vrComponent", "renderer Ogre" );
   }
   else if ( sOp == "vrKillComponent" )
   {
	  if ( splitArgs.size() > 0 )
	  {
		 if ( splitArgs[ 0 ] == "renderer" ||
			  splitArgs[ 0 ] == "all" )
		 {
			 ((OgreFrameListener*)app->mFrameListener)->scheduleQuit();
		 }
	  }
   }
  

   if (!app->isUseBoneBus())
     SBM_ProcessVHMsgs(app->m_sbm, op, args);
}


SceneManager* OgreRenderer::getSceneManager()
{
	return mSceneMgr;
}


Camera* OgreRenderer::getCamera()
{
	return mCamera;
}

OgreFrameListener* OgreRenderer::getOgreFrameListener()
{
	return dynamic_cast<OgreFrameListener*>(mFrameListener);
}
