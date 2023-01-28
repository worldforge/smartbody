#ifndef _OGRESMARTBODYLISTENER_H_
#define _OGRESMARTBODYLISTENER_H_

#include <OGRE/Ogre.h>
#include "ogresmartbody.h"
#include <sb/SBSceneListener.h>

#include <sbm/sbm_deformable_mesh.h>

class OgreSmartBodyListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
   public:
	   OgreSmartBodyListener(OgreSmartBody* osb);
	   ~OgreSmartBodyListener();

		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );	
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterChanged( const std::string& name );		 
		virtual void OnLogMessage( const std::string & message );


		virtual  void OnCharacterUpdate(const std::string & name) {}

		virtual  void OnPawnCreate(const std::string & name) {}
		virtual  void OnPawnDelete(const std::string & name) {}

		virtual  void OnViseme(const std::string & name, const std::string & visemeName, const float weight, const float blendTime) {}
		virtual  void OnChannel(const std::string & name, const std::string & channelName, const float value) {}
		
		virtual  void OnEvent(const std::string & eventName, const std::string & eventParameters) {}

		virtual  void OnObjectCreate(SmartBody::SBObject* object) {}
		virtual  void OnObjectDelete(SmartBody::SBObject* object) {}

		virtual  void OnSimulationStart() {}
		virtual  void OnSimulationEnd() {}
		virtual  void OnSimulationUpdate() {}

		virtual void notify(SmartBody::SBSubject* subject);

		void addOgreMesh(std::string meshName, DeformableMeshInstance* meshInstance);
		Ogre::Entity* createOgreCharacter(SmartBody::SBCharacter* sbChar);
		void addSBSkeleton(SmartBody::SBSkeleton* skel);
		void addTexture(std::string texName);
		



	private:
		OgreSmartBody* ogreSB;
};

#endif

