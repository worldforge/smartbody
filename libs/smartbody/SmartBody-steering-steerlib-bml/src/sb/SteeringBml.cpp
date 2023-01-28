/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>

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

#include "sbm/BMLDefs.h"
#include "SteeringBml.h"
#include "bml/bml_processor.hpp"
#include "sb/SBSteerManager.h"
#include "sbm/xercesc_utils.hpp"
#include "bml/bml_locomotion.hpp"


SmartBody::SteeringBml::SteeringBml(BML::Processor& bmlProcessor, SmartBody::SBSteerManager& steerManager)
		: bmlProcessor(bmlProcessor), steerManager(steerManager) {
	std::string tagStr;
	xml_utils::xml_translate(&tagStr, BML::BMLDefs::TAG_LOCOTMOTION);
	bmlProcessor.registerBmlHandler(tagStr, this);
}

SmartBody::SteeringBml::~SteeringBml() {
	std::string tagStr;
	xml_utils::xml_translate(&tagStr, BML::BMLDefs::TAG_LOCOTMOTION);
	bmlProcessor.deregisterBmlHandler(tagStr);
}

BML::BehaviorRequestPtr SmartBody::SteeringBml::parseBML(const XMLCh& tag, BML::BMLHandler::Payload& payload) {
	return parse_bml_locomotion(payload.child, payload.unique_id, payload.behav_syncs, payload.required, payload.request, payload.scene, steerManager);
}
