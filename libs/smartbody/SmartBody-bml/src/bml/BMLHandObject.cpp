#include "BMLHandObject.h"
#include <sb/SBAttribute.h>

BMLHandObject::BMLHandObject() : BMLObject()
{
	setName("sbm:grab");

	createStringAttribute("target", "", true, "Basic", 10, false, false, false, "reach target");
	createStringAttribute("sbm:wrist", "", true, "Basic", 10, false, false, false, "wrist joint");
	createStringAttribute("sbm:source-joint", "", true, "Basic", 10, false, false, false, "wrist joint");
	createStringAttribute("sbm:attach-pawn", "", true, "Basic", 10, false, false, false, "wrist joint");
	SmartBody::StringAttribute*  grabActionAttr = createStringAttribute("sbm:grab-type", "", true, "Basic", 10, false, false, false, "wrist joint");
	std::vector<std::string> grabTypes;
	grabTypes.emplace_back("left");
	grabTypes.emplace_back("right");
	grabActionAttr->setValidValues(grabTypes);

	SmartBody::StringAttribute*  grabStateAttr = createStringAttribute("sbm:grab-state", "", true, "Basic", 10, false, false, false, "wrist joint");
	std::vector<std::string> grabStates;
	grabStates.emplace_back("start");
	grabStates.emplace_back("reach");
	grabStates.emplace_back("finish");
	grabStates.emplace_back("return");
	grabStates.emplace_back("point");
	grabStateAttr->setValidValues(grabStates);

	createDoubleAttribute("sbm:grab-velocity",-1.0,true,"Basic", 60, false, false, false,"the end effector movement velocity when interpolating two poses.");
	createBoolAttribute("sbm:release-pawn",true,true,"Basic", 40, false, false, false,"whether to apply ik and fixed the foot sliding.");


	createStringAttribute("sbm:handle", "", true, "Basic", 30, false, false, false, "Handle of this reach instance, can be reused during later reach commands.");
	createStringAttribute("start", "", "", "Basic", 100, false, false, false, "Time when reach starts.");
}

BMLHandObject::~BMLHandObject()
{
}

void BMLHandObject::notify(SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLHandObject::copy()
{
	return new BMLHandObject();
}