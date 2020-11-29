#include "SBListener.h"

using namespace Ogre; 
SBListener::SBListener(DemoApp* app)
{
	m_app = app;
}

void SBListener::OnCharacterCreate( const  std::string & name )
{	     
	printf( "Character Create!\n" );
}


void SBListener::OnCharacterCreate( const  std::string & name, const  std::string & objectClass )
{	
	std::string logMsg = "Character " + name + " Created. Type is " + objectClass;
	LogManager::getSingleton().logMessage(logMsg.c_str());
	if (objectClass == "pawn")
		return;

	Entity * ent;
	if (m_app->getSceneManager()->hasEntity(name))
		return;

	try
	{
		//Create character from characterType
		ent = m_app->getSceneManager()->createEntity(name, name + ".mesh" );
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
			ent = m_app->getSceneManager()->createEntity(name, "Brad.mesh" );
		}
	}

	if (ent == NULL)
	{
		return;
	}

	// Add entity to the scene node
	SceneNode * mSceneNode = m_app->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
	mSceneNode->attachObject(ent);
	Ogre::Skeleton* skel = ent->getSkeleton();

	OgreFramework* framework = m_app->getOgreFramework();
	if (framework)
	{
		// insert into character list
		framework->m_characterList.emplace_back(name);
		
		// get intial bone position for every character
		std::map<std::string, Ogre::Vector3> intialBonePositions;

		for (int i = 0; i < skel->getNumBones(); i++)
		{
			Bone* bone = skel->getBone(i);
			intialBonePositions.insert(std::make_pair(bone->getName(), bone->getPosition()));
			framework->m_validJointNames.insert(bone->getName());

		}
		framework->m_initialBonePositions.insert(std::make_pair(name, intialBonePositions));
	}
}

void SBListener::OnCharacterDelete( const  std::string & name )
{
	SceneNode * node = (SceneNode *)m_app->getSceneManager()->getRootSceneNode()->getChild(name);
	node->detachAllObjects();
	m_app->getSceneManager()->destroyEntity(name);
	m_app->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name);

	OgreFramework* framework = m_app->getOgreFramework();
	if (framework)
	{
		// delete from character list
		int eraseId = -1;
		for (unsigned int i = 0; i < framework->m_characterList.size(); i++)
		{
			if (framework->m_characterList[i] == name)
			{
				eraseId = i;
				break;
			}
		}
		if (eraseId >= 0)
			framework->m_characterList.erase(framework->m_characterList.begin() + eraseId);

		// delete from initial bone position map
		std::map<std::string, std::map<std::string, Ogre::Vector3> >::iterator iter = framework->m_initialBonePositions.find(name);
		if (iter != framework->m_initialBonePositions.end())
			framework->m_initialBonePositions.erase(iter);
	}
}

void SBListener::OnCharacterChange( const  std::string & name )
{
	printf( "Character Changed!\n" );
}
