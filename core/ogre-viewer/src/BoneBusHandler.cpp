#include "BoneBusHandler.h"
#include "OgreRenderer.h"
#include <sstream>

void BoneBusHandler::OnClientConnect( const std::string & clientName, void * userData )
{
	//printf( "Client Connected! - %s\n", clientName.c_str() );
}


void BoneBusHandler::OnCreateCharacter( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData )
{
	//printf( "Character Create! - %d, %s, %s, %d\n", characterID, characterType.c_str(), characterName.c_str(), skeletonType );

	if (characterType == "pawn")
		return;
	OgreRenderer * app = (OgreRenderer *)userData;

	std::stringstream strstr;
	strstr << characterID;
	std::string charID = strstr.str();
	Entity * ent;

	if (app->getSceneManager()->hasEntity(charID.c_str()))
	{
		// id exists - remove it before creating the character again
		OnDeleteCharacter(characterID, userData);
	}

	try
	{
		//Create character from characterType
		ent = app->getSceneManager()->createEntity( charID.c_str(), characterType + ".mesh" );
	}
	catch( Ogre::ItemIdentityException& )
	{
		//Default to existing Brad character
		ent = app->getSceneManager()->createEntity( charID.c_str(), "Brad.mesh" );
	}
	catch( Ogre::Exception& e )
	{
		if( e.getNumber() == Ogre::Exception::ERR_FILE_NOT_FOUND ) {
			//Default to existing Brad character
			ent = app->getSceneManager()->createEntity( charID.c_str(), "Brad.mesh" );
		} else {
			// Re-throw exception for outer catch block
			throw;
		}
	}

	if (ent == NULL)
	{
		//printf( "Unable to create character %s", characterName);
		return;
	}

	std::vector<int>* lastPosTimes = new std::vector<int>();
	lastPosTimes->resize(1000);
	std::vector<int>* lastRotTimes = new std::vector<int>();
	lastRotTimes->resize(1000);
	for (int x = 0; x < 1000; x++)
	{
		(*lastPosTimes)[x] = -1;
		(*lastRotTimes)[x] = -1;
	}
	app->m_lastPosTimes[charID.c_str()] = lastPosTimes;
	app->m_lastRotTimes[charID.c_str()] = lastRotTimes;

	Ogre::Skeleton* skel = NULL;

	skel = ent->getSkeleton();
	if (!skel)
	{
		return;
	}


	//Number of the skeleton's bones 
	int count = skel->getNumBones(); 
	Ogre::Bone * bone = NULL;

	//Create a map of initial bone postions for the character
	std::map<std::string,Ogre::Vector3> cachedInitialBonePositions;

	//Iterate each bone in skeleton
	Ogre::Skeleton::BoneIterator boneIter = skel->getBoneIterator();
	while (boneIter.hasMoreElements())
	{
		Ogre::Bone* bone = boneIter.getNext();
		cachedInitialBonePositions[bone->getName()] = bone->getInitialPosition();
	}

	//Store the map containing the initial bone position using charachter ID
	app->characterInitBonePosMap[characterID] = cachedInitialBonePositions;
	

	// Add entity to the scene node
	SceneNode * mSceneNode = app->getSceneManager()->getRootSceneNode()->createChildSceneNode( charID.c_str() );
	mSceneNode->attachObject( ent );

}


void BoneBusHandler::OnUpdateCharacter( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData )
{
	//printf( "Character Update! - %d, %s, %s, %d\n", characterID, characterType.c_str(), characterName.c_str(), skeletonType );

	OgreRenderer * app = (OgreRenderer *)userData;

	std::stringstream strstr;
	strstr << characterID;
	std::string charID = strstr.str();

	if (!app->getSceneManager()->hasEntity(charID.c_str()))
		OnCreateCharacter(characterID, characterType, characterName, skeletonType, userData);			
}

void BoneBusHandler::OnDeleteCharacter( const int characterID, void * userData )
{
	//printf( "Character Delete! - %d\n", characterID );

	OgreRenderer * app = (OgreRenderer *)userData;

	std::stringstream strstr;
	strstr << characterID;
	std::string charID = strstr.str();
	SceneNode * node = (SceneNode *)app->getSceneManager()->getRootSceneNode()->getChild( charID.c_str() );
	node->detachAllObjects();
	app->getSceneManager()->destroyEntity( charID.c_str() );
	app->getSceneManager()->getRootSceneNode()->removeAndDestroyChild( charID.c_str() );
	//Remove initial bone positions for the character
	app->characterInitBonePosMap.erase(characterID);


}


void BoneBusHandler::OnSetCharacterPosition( const int characterID, const float x, const float y, const float z, void * userData )
{
	//printf( "Set Character Position! - %d - %5.2f %5.2f %5.2f\n", characterID, x, y, z );

	OgreRenderer * app = (OgreRenderer *)userData;

	if ( characterID == -1 )
	{
		// getCamera the camera
		app->getCamera()->setPosition( x, y, z );
	}
	else
	{
		std::stringstream strstr;
		strstr << characterID;
		std::string charID = strstr.str();
		

		if (!app->getSceneManager()->hasEntity(charID.c_str()))
			return;
		
		SceneNode* sceneNode = app->getSceneManager()->getRootSceneNode();
		if (sceneNode)
		{
			Node * node = sceneNode->getChild( charID.c_str() );
			if (node)
				node->setPosition( x, y, z );
		}
	}
}


void BoneBusHandler::OnSetCharacterRotation( const int characterID, const float w, const float x, const float y, const float z, void * userData )
{
	//printf( "Set Character Rotation! - %d - %5.2f %5.2f %5.2f %5.2f\n", characterID, w, x, y, z );

	OgreRenderer * app = (OgreRenderer *)userData;

	if ( characterID == -1 )
	{
		// move the camera
		app->getCamera()->setOrientation( Quaternion( w, x, y, z ) );
	}
	else
	{
		std::stringstream strstr;
		strstr << characterID;
		std::string charID = strstr.str();

		if (!app->getSceneManager()->hasEntity(charID.c_str()))
			return;

		SceneNode* sceneNode = app->getSceneManager()->getRootSceneNode();
		if (sceneNode)
		{
			Node * node = sceneNode->getChild( charID.c_str() );
			if (node)
				node->setOrientation( Quaternion( w, x, y, z ) );
		}
	}
}


void BoneBusHandler::OnBoneRotations( const bonebus::BulkBoneRotations * bulkBoneRotations, void * userData )
{
				
	//printf( "Set Bone Rotations! - %d %d %d\n", bulkBoneRotations->packetId, bulkBoneRotations->charId, bulkBoneRotations->numBoneRotations );


	OgreRenderer * app = (OgreRenderer *)userData;


	for ( int z = 0; z < app->getSceneManager()->getRootSceneNode()->numChildren(); z++ )
	{
		//OutputDebugString( string( mSceneMgr->getRootSceneNode()->getChild( z )->getName() + "\n" ).c_str() );
		//OutputDebugString( "--------------------\n" );
	}


	std::stringstream strstr;
	strstr << bulkBoneRotations->charId;
	std::string charID = strstr.str();

	SceneNode * n;

	try
	{
		n = (SceneNode *)app->getSceneManager()->getRootSceneNode()->getChild( charID.c_str() );
	}
	catch ( Exception )
	{
		return;
	}

	Entity * ent = (Entity *)n->getAttachedObject( charID.c_str() );
	if ( ent == NULL )
	{
		return;
	}

	Ogre::Skeleton* skel = ent->getSkeleton();

	std::map<std::string, std::vector<int>*>::iterator iter = app->m_lastRotTimes.find(charID.c_str());
	if (iter == app->m_lastRotTimes.end())
		return;
	std::vector<int>* lastTimes = (*iter).second;


	std::map<std::string, std::vector<std::string> >::iterator iter2 = app->m_skeletonMap.find(charID.c_str());
	if (iter2 == app->m_skeletonMap.end())
		return;

	std::vector<std::string>& skeletonMap = (*iter2).second;
	int i;
	for ( i = 0; i < bulkBoneRotations->numBoneRotations; i++ )
	{
		int id = bulkBoneRotations->bones[ i ].boneId;
		if ((*lastTimes)[id] >= bulkBoneRotations->time)
		{
			continue;
		}
		(*lastTimes)[id] = bulkBoneRotations->time;

		if (id >= (int) skeletonMap.size())
			continue;
		std::string & boneName = skeletonMap[ id ];

		if ( boneName == "" )
			continue;

		try {
			Ogre::Bone * bone = skel->getBone( boneName.c_str() );

			if ( bone )
			{
				bone->setManuallyControlled( true );

				Quaternion q;

				q.w = bulkBoneRotations->bones[ i ].rot_w;
				q.x = bulkBoneRotations->bones[ i ].rot_x;
				q.y = bulkBoneRotations->bones[ i ].rot_y;
				q.z = bulkBoneRotations->bones[ i ].rot_z;

				bone->setOrientation( q );

				//Vector3 v;
				//v.x = bulkBoneData->bones[ i ].
			}
		} catch (ItemIdentityException&) {
			//printf("Could not find bone name %s", boneName.c_str());
		}
	}
}


void BoneBusHandler::OnBonePositions( const bonebus::BulkBonePositions * bulkBonePositions, void * userData )
{
	//printf( "Set Bone Positions! - %d %d %d\n", bulkBonePositions->packetId, bulkBonePositions->charId, bulkBonePositions->numBonePositions );


	OgreRenderer * app = (OgreRenderer *)userData;


	std::stringstream strstr;
	strstr << bulkBonePositions->charId;
	std::string charID = strstr.str();

	SceneNode * n;

	try
	{
		n = (SceneNode *)app->getSceneManager()->getRootSceneNode()->getChild( charID.c_str() );
	}
	catch ( Exception )
	{
		return;
	}

	Entity * ent = (Entity *)n->getAttachedObject( charID.c_str() );
	if ( ent == NULL )
	{
		return;
	}

	Ogre::Skeleton* skel = ent->getSkeleton();

	std::map<std::string, std::vector<int>*>::iterator iter = app->m_lastPosTimes.find(charID.c_str());
	if (iter == app->m_lastPosTimes.end())
		return;
	std::vector<int>* lastTimes = (*iter).second;

	std::map<std::string, std::vector<std::string> >::iterator iter2 = app->m_skeletonMap.find(charID.c_str());
	if (iter2 == app->m_skeletonMap.end())
		return;
	std::vector<std::string>& skeletonMap = (*iter2).second;

	//Get map of initial bone positions for the character
	std::map<std::string,Ogre::Vector3> cachedInitialBonePositions =
		app->characterInitBonePosMap[bulkBonePositions->charId];

	int i;
	for ( i = 0; i < bulkBonePositions->numBonePositions; i++ )
	{
		int id = bulkBonePositions->bones[ i ].boneId;
		if ((*lastTimes)[id] >= bulkBonePositions->time)
		{
			continue;
		}
		(*lastTimes)[id] = bulkBonePositions->time;

		if (id >= (int) skeletonMap.size())
			continue;
		std::string & boneName = skeletonMap[ id ];

		if ( boneName == "" )
			continue;

		try {
			Ogre::Bone * bone = skel->getBone( boneName.c_str() );

			if ( bone )
			{
				bone->setManuallyControlled( true );
				
				//Get the initial bone position for the bone using bone name
				Vector3 initialBonePosition = cachedInitialBonePositions[boneName];
				Vector3 v;

				
				//Add initial bone position to delta
				v.x = initialBonePosition.x + bulkBonePositions->bones[ i ].pos_x;
				v.y = initialBonePosition.y + bulkBonePositions->bones[ i ].pos_y;
				v.z = initialBonePosition.z + bulkBonePositions->bones[ i ].pos_z;
				

				bone->setPosition( v );
			}
		} catch (ItemIdentityException&) {
			printf("Could not find bone name %s", boneName.c_str());
		}
	}
}

void BoneBusHandler::OnBoneId( const int characterID, const std::string boneName, const int boneId, void * user_data )
{
	OgreRenderer * app = (OgreRenderer *)user_data;

	std::stringstream strstr;
	strstr << characterID;
	std::string charID = strstr.str();

	std::map<std::string, std::vector<std::string> >::iterator iter = app->m_skeletonMap.find(charID.c_str());
	if (iter == app->m_skeletonMap.end())
	{
		app->m_skeletonMap[charID.c_str()] = std::vector<std::string>();
		iter = app->m_skeletonMap.find(charID.c_str());
		(*iter).second.resize(1000);
	}
	std::vector<std::string>& localSkeletonMap = (*iter).second;
	if (localSkeletonMap.size() <= (size_t) boneId)
	{
		// boneId is out of range, ignore...
		return;
	}
	localSkeletonMap[boneId] = boneName;
}

void BoneBusHandler::OnVisemeId( const int characterID, const std::string visemeName, const int visemeId, void * user_data )
{
	OgreRenderer * app = (OgreRenderer *)user_data;

	std::stringstream strstr;
	strstr << characterID;
	std::string charID = strstr.str();

	std::map<std::string, std::vector<std::string> >::iterator iter = app->m_visemeMap.find(charID.c_str());
	if (iter == app->m_visemeMap.end())
	{
		app->m_visemeMap[charID.c_str()] = std::vector<std::string>();
		iter = app->m_visemeMap.find(charID.c_str());
		(*iter).second.resize(1000);
	}
	std::vector<std::string>& localVisemeMap = (*iter).second;
	if (localVisemeMap.size() <= (size_t) visemeId)
	{
		// viseme id is out of range, ignore...
		return;
	}
	localVisemeMap[visemeId] = visemeName;
}
