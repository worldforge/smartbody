#ifndef _BMLOBJECT_H
#define _BMLOBJECT_H

#include "sb/SBObject.h"
#include <string>
#include "rapidxml_utils.hpp"
#include <sb/SBBmlProcessor.h>

class BMLObject : public SmartBody::SBObject
{
	public:
		SBAPI BMLObject();
		SBAPI ~BMLObject();

		SBAPI void notify(SBSubject* subject) override;

		SBAPI virtual void parse(rapidxml::xml_node<>* node);
		SBAPI virtual BMLObject* copy();
		SBAPI virtual void constructBML();
		SBAPI virtual std::string getBML();

		SBAPI std::vector<SmartBody::SBSyncPoint*> getSyncPointObjects();
		SBAPI std::vector<SmartBody::SBTrigger*> getTriggers();

		SBAPI void start() override;
		SBAPI void stop() override;

	protected:
		std::string _bml;

};

#endif