//
// Created by feng on 10/14/2015.
//

#ifndef VHMOBILE_APPLISTENER_H
#define VHMOBILE_APPLISTENER_H
#include <sb/SBScene.h>
#include <sb/SBSceneListener.h>

class AppListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
   public:
	  AppListener();
	  ~AppListener();

      virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
      virtual void OnCharacterDelete( const std::string & name );
	  virtual void OnCharacterUpdate( const std::string & name );
      virtual void OnPawnCreate( const std::string & name );
      virtual void OnPawnDelete( const std::string & name );

	  virtual void OnSimulationUpdate();

	  virtual void notify(SmartBody::SBSubject* subject);
};

#endif //VHMOBILE_APPLISTENER_H
