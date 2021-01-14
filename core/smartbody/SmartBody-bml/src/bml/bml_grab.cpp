/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/



#include <string>

#include "bml_grab.hpp"


#include "controllers/me_ct_hand.hpp"
#include "controllers/me_controller_tree_root.hpp"
#include "controllers/me_ct_scheduler2.h"

#include "bml_target.hpp"
#include "bml_xml_consts.hpp"
#include "sbm/xercesc_utils.hpp"
#include "sbm/BMLDefs.h"
#include <sb/SBReach.h>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include "SBUtilities.h"

#define TEST_GAZE_LOCOMOTION 0 // set to 1 if want to test gaze+locomotion control when reaching

using namespace std;
using namespace BML;
using namespace xml_utils;


BehaviorRequestPtr BML::parse_bml_grab( DOMElement* elem, const std::string& unique_id, BehaviorSyncPoints& behav_syncs, bool required, BmlRequestPtr request, SmartBody::SBScene* scene ) {
    
	const XMLCh* tag      = elem->getTagName();	
	boost::intrusive_ptr<MeCtHand> handCt;
	const XMLCh* attrHandle = elem->getAttribute( BMLDefs::ATTR_HANDLE );
	std::string handle = "";
	if( attrHandle && XMLString::stringLen( attrHandle ) ) {
		handle = asciiString(attrHandle);
		// look for a gaze controller with that handle
		
		const SbmCharacter* character = request->actor;
		if (character)
		{
			auto& controllerTree = character->ct_tree_p;
			MeController* controller = controllerTree->findControllerByHandle(handle);

			handCt = dynamic_cast<MeCtHand*>(controller);			
		}

		if (!handCt)
		{
			SmartBody::util::log("Handle : %s, controller not found.",handle.c_str());
		}
	}

	const XMLCh* attrTarget = elem->getAttribute( BMLDefs::ATTR_TARGET );
	const SbmPawn* target_pawn = nullptr;
	if (attrTarget && XMLString::stringLen( attrTarget ))
	{
		target_pawn = parse_target_pawn( tag, attrTarget, scene );		
	}

	const XMLCh* attrWrist = nullptr;
	attrWrist = elem->getAttribute(BMLDefs::ATTR_WRIST);
	SmartBody::SBJoint* wristJoint = nullptr;
	auto sbSkel = request->actor->getSkeleton();
	if( attrWrist && XMLString::stringLen( attrWrist ) ) 
	{
		wristJoint =sbSkel->getJointByName(asciiString(attrWrist).c_str());
	}

	const XMLCh* attrSourceJoint = nullptr;
	std::string sourceJointName = xml_parse_string(BMLDefs::ATTR_SOURCE_JOINT,elem,"",false);
	
	const XMLCh* attrAttachPawn = elem->getAttribute( BMLDefs::ATTR_ATTACH_PAWN );
	const SbmPawn* attachPawn = nullptr;
	if (attrAttachPawn && XMLString::stringLen( attrAttachPawn ))
	{
		attachPawn = parse_target_pawn( tag, attrAttachPawn, scene );		
	}

	std::string grabType = xml_parse_string(BMLDefs::ATTR_GRAB_TYPE,elem,"right",false);


	if (wristJoint == nullptr && !handCt) {  // Invalid target.  Assume parse_target(..) printed error.
		return BehaviorRequestPtr();  // a.k.a., nullptr
	}

	float grabVelocity = xml_parse_float(BMLDefs::ATTR_GRAB_SPEED,elem,-1.f,false);
	
	float fadeOutTime = xml_parse_float(BMLDefs::ATTR_FADE_OUT,elem,-1.f,false);
	float fadeInTime = xml_parse_float(BMLDefs::ATTR_FADE_IN,elem,-1.f,false);




	auto attrFadeOut = xml_utils::asciiString(elem->getAttribute( BMLDefs::ATTR_FADE_OUT ));
	auto attrFadeIn = xml_utils::asciiString(elem->getAttribute( BMLDefs::ATTR_FADE_IN ));
	

	const XMLCh* id = elem->getAttribute(BMLDefs::ATTR_ID);
	std::string localId = xml_parse_string(BMLDefs::ATTR_ID,elem,"",false);
	
	bool bCreateNewController = false;
	//SmartBody::util::log("grabSpeed = %f",grabVelocity);
	if (!handCt)
	{
		handCt = new MeCtHand(*request->actor, sbSkel, wristJoint);
		handCt->handle(handle);
		auto* chr = dynamic_cast<SmartBody::SBCharacter*>(request->actor);
		if (chr->getReach())
		{
			SmartBody::SBReach* reach = chr->getReach();
			handCt->init(grabType,reach->getReachHandData(),reach->getGrabHandData(),reach->getReleaseHandData(), reach->getPointHandData());				
			bCreateNewController = true;
		}
		else
		{
			// error, no reach controller is initialized
		}
	}

	std::string attrGrabState = xml_parse_string(BMLDefs::ATTR_GRAB_STATE,elem,"",false);
	if( stringICompare(attrGrabState,"start") ) 
	{			
		handCt->setGrabState(MeCtHand::GRAB_START);
	}
	else if( stringICompare(attrGrabState,"reach") )
	{			
		handCt->setGrabState(MeCtHand::GRAB_REACH);
	}
	else if( stringICompare(attrGrabState,"finish") )
	{			
		handCt->setGrabState(MeCtHand::GRAB_FINISH);
	}
	else if( stringICompare(attrGrabState,"return") )
	{			
		handCt->setGrabState(MeCtHand::GRAB_RETURN);
	}
	else if( stringICompare(attrGrabState,"point") )
	{			
		handCt->setGrabState(MeCtHand::GRAB_POINT);
	}
		

	std::string attrReleasePawn = xml_parse_string(BMLDefs::ATTR_RELEASE_PAWN,elem,"",false);
	attrReleasePawn = xml_utils::asciiString(elem->getAttribute(BMLDefs::ATTR_RELEASE_PAWN));
	if( stringICompare(attrReleasePawn,"true") ) 
	{			
		handCt->releasePawn();
	}
	

	if (grabVelocity > 0)
		handCt->grabSpeed = grabVelocity;

	if( target_pawn )	{	
		SbmPawn* pawn = const_cast<SbmPawn*>(target_pawn);
		handCt->setGrabTargetObject(pawn);		
	}

	if (attachPawn && !sourceJointName.empty())
	{
		std::string jointName = sourceJointName;
		SbmPawn* pawn = const_cast<SbmPawn*>(attachPawn);
		handCt->attachPawnTarget(pawn,jointName);
	}

	if (fadeInTime >= 0.0)
		handCt->setFadeIn(fadeInTime);

	if (fadeOutTime >= 0.0)
		handCt->setFadeOut(fadeOutTime);

	boost::shared_ptr<MeControllerRequest> ct_request;
	ct_request.reset();
	if (bCreateNewController)
	{
		SmartBody::SBSimulationManager* simManager = scene->getSimulationManager();
		//SmartBody::util::log("parse_bml_grab, time = %f",simManager->getTime());
		ct_request.reset( new MeControllerRequest( unique_id, localId, handCt, request->actor->grab_sched_p, behav_syncs ) );
		ct_request->set_persistent( true );
	}	
	return ct_request;
}
