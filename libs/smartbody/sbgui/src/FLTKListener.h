#ifndef _FLTKLISTENER_H_
#define _FLTKLISTENER_H_

#include "sb/SBSceneListener.h"
#include "sb/SBObserver.h"
class FltkViewer;

class FLTKListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
   public:
	  explicit FLTKListener(FltkViewer& viewer);
	  ~FLTKListener() override;

      void OnCharacterCreate( const std::string & name, const std::string & objectClass ) override;
      void OnCharacterDelete( const std::string & name ) override;
	  void OnCharacterUpdate( const std::string & name ) override;
      void OnPawnCreate( const std::string & name ) override;
      void OnPawnDelete( const std::string & name ) override;
      void OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime ) override;
	  void OnChannel( const std::string & name, const std::string & channelName, const float value) override;
	  void OnLogMessage( const std::string & message ) override;
	  void OnEvent( const std::string & eventName, const std::string & eventParameters ) override;

	  void OnSimulationStart() override;
	  void OnSimulationEnd() override;
	  void OnSimulationUpdate() override;

	  void notify(SmartBody::SBSubject* subject) override;

	  void setOtherListener(SBSceneListener* listener);
	protected:
	FltkViewer& mViewer;
	  SmartBody::SBSceneListener* otherListener;
};

#endif
