#pragma once
#include <sb/SBObject.h>
#include <sb/SBSubject.h>

class TreeInfoObject : public SmartBody::SBObject
{
public:
	TreeInfoObject();
	~TreeInfoObject();

	virtual void notify(SmartBody::SBSubject* subject);
};
