#include "SBListener.h"
#include "ExampleApplication.h"
#include "OgreRenderer.h"
#include "OgreFrameListener.h"
//#include <sb/SBScene.h>
//#include <sb/SBCharacter.h>

SBListener::SBListener(OgreRenderer* app)
{
	m_app = app;
}

void SBListener::OnCharacterCreate( const  std::string & name )
{	     
	printf( "Character Create!\n" );
}


void SBListener::OnPawnCreate( const std::string& name )
{
	if (m_app->getSceneManager()->hasEntity(name))
		return;

	Entity * ent = NULL;
	try
	{
		//Create character from characterType
		ent = m_app->getSceneManager()->createEntity(name, "sphere.mesh" );						
	}
	catch( Ogre::ItemIdentityException& )
	{
		;
	}		

	if (ent == NULL)
	{
		return;
	}

	// Add entity to the scene node
	SceneNode * mSceneNode = m_app->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
	mSceneNode->attachObject(ent);	
	OgreFrameListener* frameListener = m_app->getOgreFrameListener();

	mSceneNode->scale( .05f, .05f, .05f );
	if (frameListener)
	{
		frameListener->m_pawnList.emplace_back(name);
	}
	return;
}


void SBListener::OnCharacterCreate( const  std::string & name, const  std::string & objectClass )
{	
	//printf("Character create callback\n");
	std::string logMsg = "Character " + name + " Created. Type is " + objectClass;
	LogManager::getSingleton().logMessage(logMsg.c_str());

	if (m_app->getSceneManager()->hasEntity(name))
		return;


	Entity * ent;

	
	try
	{
		//Create character from characterType

		ent = m_app->getSceneManager()->createEntity(name, objectClass + ".mesh" );						
		
	}
	catch( Ogre::ItemIdentityException& )
	{
		;
	}
	catch( Ogre::Exception& e )
	{
		if( e.getNumber() == Ogre::Exception::ERR_FILE_NOT_FOUND ) 
		{

			//Default to existing Brad character
			//ent = m_app->getSceneManager()->createEntity(name, "ChrBrad.mesh" );
			LOG("Cannot find mesh named %s. Ignoring mesh for character %s.", objectClass.c_str(), name.c_str());
			ent = NULL;
		}
	}
		

	if (ent == NULL)
	{
		return;
	}

	// Add entity to the scene node
	SceneNode * mSceneNode = m_app->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
	mSceneNode->attachObject(ent);
	
	//mSceneNode->scale(scaleFactor, scaleFactor, scaleFactor);

	Ogre::Skeleton* skel = ent->getSkeleton();
	OgreFrameListener* frameListener = m_app->getOgreFrameListener();
	if (frameListener)
	{		
		// insert into character list
		frameListener->m_characterList.emplace_back(name);
		
		// get intial bone position for every character
		std::map<std::string, Ogre::Vector3> intialBonePositions;
		
		if (skel)
		{
			for (int i = 0; i < skel->getNumBones(); i++)
			{
				Bone* bone = skel->getBone(i);
				intialBonePositions.insert(std::make_pair(bone->getName(), bone->getPosition()));
				frameListener->m_validJointNames.insert(bone->getName());

			}
		}		
		frameListener->m_initialBonePositions.insert(std::make_pair(name, intialBonePositions));		
	}
}

void SBListener::OnCharacterDelete( const  std::string & name )
{
	SceneNode * node = (SceneNode *)m_app->getSceneManager()->getRootSceneNode()->getChild(name);
	node->detachAllObjects();
	m_app->getSceneManager()->destroyEntity(name);
	m_app->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name);

	OgreFrameListener* frameListener = m_app->getOgreFrameListener();
	if (frameListener)
	{
		// delete from character list
		int eraseId = -1;
		for (unsigned int i = 0; i < frameListener->m_characterList.size(); i++)
		{
			if (frameListener->m_characterList[i] == name)
			{
				eraseId = i;
				break;
			}
		}
		if (eraseId >= 0)
			frameListener->m_characterList.erase(frameListener->m_characterList.begin() + eraseId);

		// delete from initial bone position map
		std::map<std::string, std::map<std::string, Ogre::Vector3> >::iterator iter = frameListener->m_initialBonePositions.find(name);
		if (iter != frameListener->m_initialBonePositions.end())
			frameListener->m_initialBonePositions.erase(iter);
	}
}

void SBListener::OnCharacterChange( const  std::string & name )
{
	printf( "Character Changed!\n" );
}
