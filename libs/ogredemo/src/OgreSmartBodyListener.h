#ifndef _OGRESMARTBODYLISTENER_H_
#define _OGRESMARTBODYLISTENER_H_

#include <OgreSceneManager.h>
#include <sb/SBSceneListener.h>

namespace SmartBody {
class SBCharacter;

class SBSkeleton;

class SBJoint;
}

struct JointEntry {
	Ogre::SceneNode& node;
	SmartBody::SBJoint& joint;
};

struct CharacterEntry {
	Ogre::SceneNode& node;
	SmartBody::SBCharacter& character;
	std::vector<JointEntry> jointEntries;
};

class OgreSmartBodyListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver {
public:
	OgreSmartBodyListener(Ogre::SceneManager& sceneManager, SmartBody::SBScene& scene);

	~OgreSmartBodyListener() override;

	Ogre::SceneNode* createCharacter(SmartBody::SBCharacter& sbCharacter);

	void updateCharacters();

	void OnCharacterCreate(const std::string& name, const std::string& objectClass) override {}

	void OnCharacterDelete(const std::string& name) override;

	void OnLogMessage(const std::string& message) override;

	void OnCharacterUpdate(const std::string& name) override;

	void OnPawnCreate(const std::string& name) override {}

	void OnPawnDelete(const std::string& name) override {}

	void OnViseme(const std::string& name, const std::string& visemeName, const float weight, const float blendTime) override {}

	void OnChannel(const std::string& name, const std::string& channelName, const float value) override {}

	void OnEvent(const std::string& eventName, const std::string& eventParameters) override {}

	void OnObjectCreate(SmartBody::SBObject* object) override {}

	void OnObjectDelete(SmartBody::SBObject* object) override {}

	void OnSimulationStart() override {}

	void OnSimulationEnd() override {}

	void OnSimulationUpdate() override {}

	void notify(SmartBody::SBSubject* subject) override;

	//void addOgreMesh(std::string meshName, DeformableMeshInstance* meshInstance);

	Ogre::Entity* createOgreCharacter(SmartBody::SBCharacter* sbChar);

	void addSBSkeleton(SmartBody::SBSkeleton* skel);

	void addTexture(std::string texName);


private:
	Ogre::SceneManager& mSceneManager;
	SmartBody::SBScene& mScene;

	std::map<std::string, CharacterEntry> mCharacters;

};

#endif

