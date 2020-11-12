#pragma once
#include <sb/SBSceneListener.h>


class SBOpenVRListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
public:
	SBOpenVRListener() {}
	~SBOpenVRListener() {}

	virtual void OnLogMessage(const std::string & message);

	virtual void OnCharacterCreate(const std::string & name, const std::string & objectClass);
	virtual void OnCharacterDelete(const std::string & name);
	virtual void OnCharacterUpdate(const std::string & name);
	virtual void OnPawnCreate(const std::string & name);
	virtual void OnPawnDelete(const std::string & name);

	virtual void OnSimulationUpdate();

	virtual void notify(SmartBody::SBSubject* subject);
};
