#include "BMLGazeObject.h"
#include <sb/SBAttribute.h>

BMLGazeObject::BMLGazeObject() : BMLObject()
{
	setName("gaze");

	createStringAttribute("target", "", true, "Basic", 10, false, false, false, 
		"Gaze target");
	
	SmartBody::StringAttribute* jointRangeAttr = 
		createStringAttribute("sbm:joint-range", "", true, "Basic", 20, false, false, false, 
			"Which logical joints will be used to gaze");

	std::vector<std::string> jointRanges;
	jointRanges.emplace_back("EYES");
	jointRanges.emplace_back("NECK");
	jointRanges.emplace_back("CHEST");
	jointRanges.emplace_back("BACK");
	jointRanges.emplace_back("EYES NECK");
	jointRanges.emplace_back("EYES CHEST");
	jointRanges.emplace_back("EYES BACK");
	jointRanges.emplace_back("HEAD CHEST");
	jointRanges.emplace_back("HEAD BACK");
	jointRanges.emplace_back("CHEST BACK");
	jointRangeAttr->setValidValues(jointRanges);


	SmartBody::StringAttribute* dirAttr = 
		createStringAttribute("direction", "", true, "Basic", 30, false, false, false, 
			"Offset from gaze target. A number can be used in place of the directions. If polar is used, then 'angle' must also be set");

	std::vector<std::string> directions;
	directions.emplace_back("LEFT");
	directions.emplace_back("RIGHT");
	directions.emplace_back("UP");
	directions.emplace_back("DOWN");
	directions.emplace_back("UPLEFT");
	directions.emplace_back("UPRIGHT");
	directions.emplace_back("DOWNLEFT");
	directions.emplace_back("DOWNRIGHT");
	directions.emplace_back("POLAR");
	dirAttr->setValidValues(directions);

	createDoubleAttribute("angle", 0, true, "Basic", 40, false, false, false, 
		"Angle of gaze offset when setting direction=\"POLAR\"");

	SmartBody::StringAttribute* priorityJointAttr =createStringAttribute("sbm:priority-joint", "", true, "Basic", 50, false, false, false, 
		"Which logical joint should acquire target (default is EYES.");

	std::vector<std::string> priorityJoints;
	priorityJoints.emplace_back("EYES");
	priorityJoints.emplace_back("NECK");
	priorityJoints.emplace_back("CHEST");
	priorityJoints.emplace_back("BACK");
	priorityJointAttr->setValidValues(priorityJoints);

	createStringAttribute("sbm:handle", "", true, "Basic", 60, false, false, false, "Handle of this reach instance, can be reused during later gaze commands.");
	
	createDoubleAttribute("sbm:time-hint", 0, true, "Basic", 70, false, false, false, 
		"Alternative to joint speed: choose overall time for gaze task (unfinished implementation)");

	createStringAttribute("sbm:joint-speed", "", true, "Basic", 80, false, false, false, 
		"Set overall task speed for head in deg per sec: <head> or <head eyes>");

	createStringAttribute("sbm:joint-smooth", "", true, "Basic", 90, false, false, false, 
		"Set decaying average smoothing values (0..1): <back neck eyes>");

	createDoubleAttribute("sbm:fade-in", 0, true, "Basic", 100, false, false, false, 
		"Set fade-out interval to stop gaze action, without pruning (default value of 0 should suffice due to smoothing).");

	createDoubleAttribute("sbm:fade-out", 0, true, "Basic", 110, false, false, false, 
		"Set fade-in interval to re-start a faded-out gaze.");

	createStringAttribute("start", "", true, "Basic", 120, false, false, false, 
		"When the gaze starts.");
}

BMLGazeObject::~BMLGazeObject()
{
}

void BMLGazeObject::notify(SmartBody::SBSubject* subject)
{
	BMLObject::notify(subject);
}

BMLObject* BMLGazeObject::copy()
{
	return new BMLGazeObject();
}