#include "OgreFrameListener.h"
#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#include "sb/smartbody-c-dll.h"

OgreFrameListener::OgreFrameListener(RenderWindow * win, Camera * cam, const std::string & debugText, SceneManager * mgr, 
									 bonebus::BoneBusServer * bonebus, SBMHANDLE sbmdll, SBListener * sbListener, std::vector<std::string>& initialCommands) 
								   : ExampleFrameListener( win, cam )
	{
	mDebugText = debugText;
	mSceneMgr = mgr;
	m_bonebus = bonebus;
	m_sbmDLL = sbmdll;
	m_sbListener = sbListener;
	m_timer = NULL;
	mQuit = false;
	m_ogreMouseEnabled = true;
	for (size_t c = 0; c < initialCommands.size(); c++)
	{
		m_initialCommands.push_back(initialCommands[c]);
	}

	// turn off mouse look by default
	SetOgreMouse( false );
}


void OgreFrameListener::windowFocusChange( RenderWindow * rw )
{
	rw->setActive( true );
}

bool OgreFrameListener::processUnbufferedKeyInput(const FrameEvent& evt)
{
	if(mKeyboard->isKeyDown(OIS::KC_A))
		mTranslateVector.x = -mMoveScale;	// Move camera left

	if(mKeyboard->isKeyDown(OIS::KC_D))
		mTranslateVector.x = mMoveScale;	// Move camera RIGHT

	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
		mTranslateVector.z = -mMoveScale;	// Move camera forward

	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
		mTranslateVector.z = mMoveScale;	// Move camera backward

	if(mKeyboard->isKeyDown(OIS::KC_PGUP) || mKeyboard->isKeyDown(OIS::KC_Q) )
		mTranslateVector.y = mMoveScale;	// Move camera up

	if(mKeyboard->isKeyDown(OIS::KC_PGDOWN) || mKeyboard->isKeyDown(OIS::KC_E) )
		mTranslateVector.y = -mMoveScale;	// Move camera down

	if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mCamera->yaw(-mRotScale);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT))
		mCamera->yaw(mRotScale);

	if( mKeyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

   	if( mKeyboard->isKeyDown(OIS::KC_Z) && mTimeUntilNextToggle <= 0 )
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if( mKeyboard->isKeyDown(OIS::KC_M) && mTimeUntilNextToggle <= 0 )
	{
		switch(mFiltering)
		{
		case TFO_BILINEAR:
			mFiltering = TFO_TRILINEAR;
			mAniso = 1;
			break;
		case TFO_TRILINEAR:
			mFiltering = TFO_ANISOTROPIC;
			mAniso = 8;
			break;
		case TFO_ANISOTROPIC:
			mFiltering = TFO_BILINEAR;
			mAniso = 1;
			break;
		default: break;
		}
		MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);

		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++mNumScreenShots << ".png";
		mWindow->writeContentsToFile(ss.str());
		mTimeUntilNextToggle = 0.5;
		mDebugText = "Saved: " + ss.str();
	}

	if(mKeyboard->isKeyDown(OIS::KC_N) && mTimeUntilNextToggle <=0)
	{
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setPolygonMode(PM_SOLID); break;
			case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
			case 2 : mCamera->setPolygonMode(PM_POINTS); break;
		}
		mTimeUntilNextToggle = 0.5;
	}

	static bool displayCameraDetails = false;
	if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
	{
		displayCameraDetails = !displayCameraDetails;
		mTimeUntilNextToggle = 0.5;
		if (!displayCameraDetails)
			mDebugText = "";
	}

	// display appropriate scenes
	if(mKeyboard->isKeyDown(OIS::KC_1))
	{
		if (mSceneMgr->hasSceneNode("world_scene"))
		{
			mSceneMgr->getSceneNode("world_scene_ft")->setVisible(false);
			mSceneMgr->getSceneNode("world_scene_cm")->setVisible(false);
			mSceneMgr->getSceneNode("plane_node")->setVisible(false);			
			mSceneMgr->getSceneNode("world_scene")->setVisible(true);
		}
	}
	if(mKeyboard->isKeyDown(OIS::KC_2))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(true);
		mSceneMgr->getSceneNode("plane_node")->setVisible(false);
		if (mSceneMgr->hasSceneNode("world_scene"))
			mSceneMgr->getSceneNode("world_scene")->setVisible(false);
	}
	if(mKeyboard->isKeyDown(OIS::KC_3))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(true);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(false);
		mSceneMgr->getSceneNode("plane_node")->setVisible(false);
		if (mSceneMgr->hasSceneNode("world_scene"))
			mSceneMgr->getSceneNode("world_scene")->setVisible(false);
	}
	if(mKeyboard->isKeyDown(OIS::KC_4))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(false);
		mSceneMgr->getSceneNode("plane_node")->setVisible(true);
		if (mSceneMgr->hasSceneNode("world_scene"))
			mSceneMgr->getSceneNode("world_scene")->setVisible(false);
	}

	if ( mKeyboard->isKeyDown( OIS::KC_J ) )
	{
		SetOgreMouse( !m_ogreMouseEnabled );
	}


	if ( mKeyboard->isKeyDown( OIS::KC_T ) )
	{
		vhmsg::ttu_notify1("sbm bml char brad <?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><act><participant id=\"brad\" role=\"actor\"/><bml><speech id=\"sp1\" ref=\"speechTTS\" type=\"application/ssml+xml\">If the system cannot find my regular voice, it defaults back to the Windows standard voice.Depending on your version of Windows that can be a woman's voice. Don't I sound delightful ?</speech></bml></act>");
	}


	// Print camera details
	if(displayCameraDetails)
		mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
					 " " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());

	// Return true to continue rendering
	return true;
}


bool OgreFrameListener::processUnbufferedMouseInput( const FrameEvent & evt )
{
	if ( m_ogreMouseEnabled )
	{
		return ExampleFrameListener::processUnbufferedMouseInput( evt );
	}

	return true;
}


bool OgreFrameListener::frameStarted( const FrameEvent & evt )
{
	if (mQuit)
	{
		return false;
	}

	if ( ExampleFrameListener::frameStarted( evt ) == false )
		return false;
	


	char temp[ 1024 ];
	sprintf( temp, "%5.2f %5.2f %5.2f", mCamera->getPosition().x, mCamera->getPosition().y, mCamera->getPosition().z );
	mDebugText = temp;


	vhmsg::ttu_poll();

	if (m_bonebus)
	{
		m_bonebus->Update();
	}
	else
	{
		bool firstTime = false;
		if (!m_timer)
		{
			m_timer = new vhcl::Timer();
			firstTime = true;
		}
		SBM_Update(m_sbmDLL, m_timer->GetTime());
		if (firstTime)
		{
			for (size_t i = 0; i < m_initialCommands.size(); i++)
			{
				printf("initial command %d : %s\n",i, m_initialCommands[i].c_str());
				vhmsg::ttu_notify2("sbm", m_initialCommands[i].c_str());
			}
		}


		char name[256];
		char objectClass[256];
		char visemeName[256];
		float weight;
		float blendTime;

		while (SBM_IsCharacterCreated(m_sbmDLL, name, 256, objectClass, 256))
		{
			m_sbListener->OnCharacterCreate(name, objectClass);
		}

		while (SBM_IsCharacterDeleted(m_sbmDLL, name, 256))
		{
			m_sbListener->OnCharacterDelete(name);
		}

		while (SBM_IsCharacterChanged(m_sbmDLL, name, 256))
		{
			m_sbListener->OnCharacterChange(name);
		}

		while (SBM_IsVisemeSet(m_sbmDLL, name, 256, visemeName, 256, &weight, &blendTime ))
		{
		}

		while (SBM_IsChannelSet(m_sbmDLL, name, 256, visemeName, 256, &weight))
		{
		}


		SceneNode* sceneNode = mSceneMgr->getRootSceneNode();
		if (!sceneNode)
			return false;

		// gets the position of pawns
		for ( size_t i = 0; i< m_pawnList.size(); i++ )
		{
			std::string& name = m_pawnList[i];
			
			std::string command = "tmp = scene.getPawn('"+name+"')";
			SBM_PythonCommandVoid(m_sbmDLL, command.c_str());
			SBM_PythonCommandVoid(m_sbmDLL, "position = tmp.getPosition()\norientation = tmp.getOrientation()");
			float x = SBM_PythonCommandFloat(m_sbmDLL, "ret = position.getData(0)");
			float y = SBM_PythonCommandFloat(m_sbmDLL, "ret = position.getData(1)");
			float z = SBM_PythonCommandFloat(m_sbmDLL, "ret = position.getData(2)");
			float qw = SBM_PythonCommandFloat(m_sbmDLL, "ret = orientation.getData(0)");
			float qx = SBM_PythonCommandFloat(m_sbmDLL, "ret = orientation.getData(1)");
			float qy = SBM_PythonCommandFloat(m_sbmDLL, "ret = orientation.getData(2)");
			float qz = SBM_PythonCommandFloat(m_sbmDLL, "ret = orientation.getData(3)");

			Node* node = sceneNode->getChild(name);
			if (!node) continue;
			node->setPosition(x,y,z);
			node->setOrientation(qw,qx,qy,qz);
		}
		
		// set character position and rotation
		for ( size_t i = 0; i < m_characterList.size(); i++ )
		{
			std::string& name = m_characterList[i];
			SBM_CharacterFrameDataMarshalFriendly c;
			SBM_InitCharacter(m_sbmDLL, name.c_str(), &c);  // TODO: should be refactored to be done at init
			SBM_GetCharacter(m_sbmDLL, name.c_str(), &c);
			if (!mSceneMgr->hasEntity(name))
				continue;

			Node* node = sceneNode->getChild(name);

			if (!node)
				continue;
			// set character positions& rotations
			node->setPosition(c.x, c.y, c.z);
			node->setOrientation(Quaternion(c.rw, c.rx, c.ry, c.rz));
			
			// set bone positions& rotations				
			SceneNode* n = (SceneNode*) sceneNode->getChild(name);
			Entity * ent = (Entity*)n->getAttachedObject(name);
			if ( ent == NULL )
				continue;
			std::map<std::string, Ogre::Vector3>& intialBonePositionMap = m_initialBonePositions[name];
			Ogre::Skeleton* skel = ent->getSkeleton();
			if (!skel) continue;
			for (size_t jId = 0; jId < c.m_numJoints; jId++)
			{
				std::string jointName = c.jname[jId];
				if (jointName == "")
					continue;

				std::set<std::string>::iterator iter = m_validJointNames.find(jointName);
				if (iter == m_validJointNames.end())
				{
					continue;
				}

				try
				{
					Ogre::Bone* bone = skel->getBone(jointName);
					if (bone)
					{
						bone->setManuallyControlled(true);
					 	
						Ogre::Vector3& vec = intialBonePositionMap[jointName];
						float x = c.jx[jId] + vec.x;
						float y = c.jy[jId] + vec.y;
						float z = c.jz[jId] + vec.z;
						bone->setPosition(x, y, z);

						bone->setOrientation(Quaternion(c.jrw[jId], c.jrx[jId], c.jry[jId], c.jrz[jId]));
					}
				}
				catch (ItemIdentityException&)
				{
					//printf("Could not find bone name %s", jointName.c_str());
				}
			}

			SBM_ReleaseCharacter(&c);  // TODO: should be refactored to be done at application quit
		}
	}

	/*
	//Limiting the frames per second as otherwise it takes up entire CPU
	// Setting it to 30, but in effect it comes up to 60 due to granularity issues
	Ogre::Root::getSingleton().setFrameSmoothingPeriod(0);
	Ogre::Real ttW;
	ttW = 1000.0f / FPS_LIMIT - 1000.0f * evt.timeSinceLastFrame;
	if (ttW > 0)
		Sleep(ttW);
		*/

	return true;
}


void OgreFrameListener::scheduleQuit(void)
{
	mQuit = true;
}



void OgreFrameListener::SetOgreMouse( const bool enabled )
{
	m_ogreMouseEnabled = enabled;


	// There's no interface in OIS to set the coop level on individual devices at runtime.
	// There's also no interface in OIS to set the coop level on individual devices at startup,
	// so you have to tear down the entire input manager and start from scratch.
	// Hopefully there's no side-effects from this behavior

	// http://www.wreckedgames.com/forum/index.php/topic,1149.msg6170.html#msg6170


	// taken from ExampleFrameListener::windowClosed()
	LogManager::getSingletonPtr()->logMessage( "*** Destroying OIS ***" );
	if ( mInputManager )
	{
		mInputManager->destroyInputObject( mMouse );
		mInputManager->destroyInputObject( mKeyboard );
		mInputManager->destroyInputObject( mJoy );

		OIS::InputManager::destroyInputSystem( mInputManager );
		mInputManager = 0;

		mMouse = 0;
		mKeyboard = 0;
		mJoy = 0;
	}


	// taken from ExampleFrameListener::ExampleFrameListener()
	LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS ***" );
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute( "WINDOW", &windowHnd );
	windowHndStr << windowHnd;
	pl.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );


	if ( m_ogreMouseEnabled )
	{
		pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_EXCLUSIVE" ) ) );
	}
	else
	{
		pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_NONEXCLUSIVE" ) ) );
	}

	pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_FOREGROUND" ) ) );


	mInputManager = OIS::InputManager::createInputSystem( pl );

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<OIS::Keyboard *>( mInputManager->createInputObject( OIS::OISKeyboard, false ) );
	mMouse = static_cast<OIS::Mouse *>( mInputManager->createInputObject( OIS::OISMouse, false ) );
	try
	{
		mJoy = static_cast<OIS::JoyStick *>( mInputManager->createInputObject( OIS::OISJoyStick, false ) );
	}
	catch(...)
	{
		mJoy = 0;
	}
}

