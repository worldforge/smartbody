/*************************************************************
Copyright (C) 2017 University of Southern California
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

#include "SBBmlProcessor.h"

#include <sb/SBSimulationManager.h>
#include <sb/SBCommandManager.h>
#include "sb/SBScene.h"
#include "sb/SBCharacter.h"
#include "SBUtilities.h"
#include <bml/bml_processor.hpp>
#include <utility>
#include "rapidxml_utils.hpp"
#include <bml/BMLObject.h>
#include <bml/BMLAnimationObject.h>
#include <bml/BMLBodyObject.h>
#include <bml/BMLConstraintObject.h>
#include <bml/BMLEventObject.h>
#include <bml/BMLFaceObject.h>
#include <bml/BMLGazeObject.h>
#include <bml/BMLGestureObject.h>
#include <bml/BMLHandObject.h>
#include <bml/BMLHeadObject.h>
#include <bml/BMLLocomotionObject.h>
#include <bml/BMLNoiseObject.h>
#include <bml/BMLReachObject.h>
#include <bml/BMLSaccadeObject.h>
#include <bml/BMLSpeechObject.h>
#include <bml/BMLStateObject.h>

namespace SmartBody {

struct SceneObserver : public SBObserver {
	SBBmlProcessor& processor;
	SceneObserver(SBBmlProcessor& processor_) : processor(processor_){

	}

	void notify(SBSubject* subject) override {
		BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(subject);
		if (boolAttr && boolAttr->getName() == "bmlstatus")
		{
			processor.getBMLProcessor()->set_bml_feedback(boolAttr->getValue());
		}
	}

};

SBTrigger::SBTrigger()
{
	_offset = 0;
}

SBTrigger::~SBTrigger() = default;

void SBTrigger::setOffset(double val)
{
	_offset = val;
}

double SBTrigger::getOffset() const
{
	return _offset;
}

void SBTrigger::setStartTag(std::string tag)
{
	_startTag = std::move(tag);
}

std::string SBTrigger::getStartTag()
{
	return _startTag;
}

void SBTrigger::setEndTag(std::string tag)
{
	_endTag = std::move(tag);
}

std::string SBTrigger::getEndTag()
{
	return _endTag;
}

SBSyncPoint::SBSyncPoint(const std::string& name)
{
	_name = name;
}

SBSyncPoint::~SBSyncPoint()
{
}


SBBMLSchedule::SBBMLSchedule()
{
}

SBBMLSchedule::~SBBMLSchedule()
{
}

void SBBMLSchedule::setCharacter(std::string name)
{
	_characterName = name;
}

std::string SBBMLSchedule::getCharacter()
{
	return _characterName;
}

void SBBMLSchedule::remove()
{
	// mark any dependent blocks since they are no longer valid
}

SBBmlProcessor::SBBmlProcessor(SBScene& scene)
: _scene(scene),
  _sceneObserver(std::make_unique<SceneObserver>(*this)),
_bmlProcessor(std::make_unique<BML::Processor>())
{

	auto addBmlObject = [&](std::unique_ptr<BMLObject> bmlObject){
		auto name = bmlObject->getName();
		_bmlHandlers.emplace(std::move(name), std::move(bmlObject));
	};

	addBmlObject(std::make_unique<BMLAnimationObject>());
	addBmlObject(std::make_unique<BMLBodyObject>());
	addBmlObject(std::make_unique<BMLConstraintObject>());
	addBmlObject(std::make_unique<BMLEventObject>());
	addBmlObject(std::make_unique<BMLFaceObject>());
	addBmlObject(std::make_unique<BMLGazeObject>());
	addBmlObject(std::make_unique<BMLGestureObject>());
	addBmlObject(std::make_unique<BMLHandObject>());
	addBmlObject(std::make_unique<BMLHeadObject>());
	addBmlObject(std::make_unique<BMLLocomotionObject>());
	addBmlObject(std::make_unique<BMLNoiseObject>());
	addBmlObject(std::make_unique<BMLReachObject>());
	addBmlObject(std::make_unique<BMLSaccadeObject>());
	addBmlObject(std::make_unique<BMLSpeechObject>());
	addBmlObject(std::make_unique<BMLStateObject>());

	_scene.registerObserver(_sceneObserver.get());

	scene.getCommandManager()->insert("vrAgentBML", [this](srArgBuffer& args) -> int {
		return getBMLProcessor()->vrAgentBML_cmd_func(args, _scene);
	});
	scene.getCommandManager()->insert("bp", [this](srArgBuffer& args) -> int {
		return getBMLProcessor()->bp_cmd_func(args, _scene);
	});
	scene.getCommandManager()->insert("vrSpeak", [this](srArgBuffer& args) -> int {
		return getBMLProcessor()->vrSpeak_func(args, _scene);
	});

	scene.getCommandManager()->insert_set_cmd("bp", [this](srArgBuffer& args) -> int {
		return getBMLProcessor()->set_func(args, nullptr);
	});
	scene.getCommandManager()->insert_print_cmd("bp", [this](srArgBuffer& args) -> int {
		return getBMLProcessor()->print_func(args, nullptr);
	});
}

SBBmlProcessor::~SBBmlProcessor() {
	_scene.unregisterObserver(_sceneObserver.get());
}

// This command is inside bml_processor.cpp. So unable to rewrite, instead, re-routine to bp.
void SBBmlProcessor::vrSpeak(std::string agent, std::string recip, std::string msgId, std::string msg)
{
	std::stringstream msgStr;
	msgStr << agent << " " << recip << " " << msgId << " " << msg;
	srArgBuffer vrMsg(msgStr.str().c_str());
	_bmlProcessor->vrSpeak_func(vrMsg, _scene);
}

void SBBmlProcessor::vrAgentBML(std::string op, std::string agent, std::string msgId, std::string msg)
{
	if (op == "request" || op == "start" || op == "end")
	{
		std::stringstream msgStr;
		msgStr << agent << " " << msgId << " " << op << " " << msg;
		srArgBuffer vrMsg(msgStr.str().c_str());
		_bmlProcessor->vrAgentBML_cmd_func(vrMsg, _scene);
	}
	else
	{
		SmartBody::util::log("vrAgentBML option %s not recognized!", op.c_str());
		return;	
	}
}

std::string SBBmlProcessor::build_vrX(std::ostringstream& buffer, const std::string& cmd, const std::string& char_id, const std::string& recip_id, const std::string& content, bool for_seq, std::string candidateMsgId ) 
{
	

	std::stringstream msgId;
	if (candidateMsgId != "")
	{
		msgId << candidateMsgId;
	}
	else
	{
		SmartBody::IntAttribute* intAttr = dynamic_cast<SmartBody::IntAttribute*>(_scene.getAttribute("bmlIndex"));

		if (!_scene.getProcessId().empty())
			msgId << "sbm_" << _scene.getProcessId() << "_test_bml_" << (intAttr->getValue());
		else
			msgId << "sbm_test_bml_" << intAttr->getValue();
		intAttr->setValue(intAttr->getValue() + 1);
	}
	

	buffer.str("");
	if( for_seq )
		buffer << "send " << cmd << " ";
	buffer << char_id << " "<< recip_id << " " << msgId.str() << std::endl << content;
	return msgId.str();
}

std::string SBBmlProcessor::send_vrX( const char* cmd, const std::string& char_id, const std::string& recip_id,
			const std::string& seq_id, bool echo, bool send, const std::string& bml, std::string candidateMsgId ) 
{
	//SmartBody::util::log("run send_vrX, cmd = %s", cmd);
	std::ostringstream msg;
	std::string msgId;
	bool all_characters = ( char_id=="*" );
	//SmartBody::util::log("send_vrX cmd =  %s, bml = %s", cmd, bml.c_str());
	if( seq_id.length()==0 ) {
		if( echo ) {
			msgId = build_vrX( msg, cmd, char_id, recip_id, bml, false, candidateMsgId);
			// don't log a vrX message
			////SmartBody::util::log("%s %s", cmd, msg.str().c_str());
		}

		if( send ) {
			// execute directly
			if( all_characters )
			{
				const std::vector<std::string>& characterNames = _scene.getCharacterNames();
				for (const auto & characterName : characterNames)
				{
					SmartBody::SBCharacter* character = _scene.getCharacter(characterName);
					msgId = build_vrX( msg, cmd, character->getName().c_str(), recip_id, bml, false, candidateMsgId);
					//SmartBody::util::log("vvmsg cmd =  %s, msg = %s", cmd, msg.str().c_str());
					//_scene.getVHMsgManager()->send2( cmd, msg.str().c_str() );
					_scene.getCommandManager()->execute(cmd, const_cast<char*>(msg.str().c_str()));
				}
			} else {
				msgId = build_vrX( msg, cmd, char_id, recip_id, bml, false, candidateMsgId);
				//SmartBody::util::log("vvmsg cmd =  %s, msg = %s", cmd, msg.str().c_str());
				//_scene.getVHMsgManager()->send2( cmd, msg.str().c_str() );
				_scene.getCommandManager()->execute(cmd, const_cast<char*>(msg.str().c_str()));
			}
		}		
		return msgId;
	}else {
		// Command sequence to trigger vrSpeak
		auto seq = std::make_unique<srCmdSeq>(); // sequence file that holds the bml command(s)
		seq->offset( (float)( _scene.getSimulationManager()->getTime()) );

		if( echo ) {
			msg << "echo // Running sequence \"" << seq_id << "\"...";
			if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
				std::stringstream strstr;
				strstr << "WARNING: send_vrX(..): Failed to insert echo header command for character \"" << char_id << "\".";
				SmartBody::util::log(strstr.str().c_str());
			}
			msgId = build_vrX( msg, cmd, char_id, recip_id, bml, false, candidateMsgId);
			if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
				std::stringstream strstr;
				strstr << "WARNING: send_vrX(..): Failed to insert echoed command for character \"" << char_id << "\".";
				SmartBody::util::log(strstr.str().c_str());
			}
		}
		if( all_characters )
		{
			const std::vector<std::string>& characterNames = _scene.getCharacterNames();
			for (const auto & characterName : characterNames)
			{
				SmartBody::SBCharacter* character = _scene.getCharacter(characterName);
				msgId = build_vrX( msg, cmd, character->getName().c_str(), recip_id, bml, true );
				if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
					std::stringstream strstr;
					strstr << "WARNING: send_vrX(..): Failed to insert vrSpeak command for character \"" << char_id << "\".";
					SmartBody::util::log(strstr.str().c_str());
				}
			}
		} else {
			msgId = build_vrX( msg, cmd, char_id, recip_id, bml, true );
			if( seq->insert( 0, msg.str().c_str() )!=CMD_SUCCESS ) {
				std::stringstream strstr;
				strstr << "WARNING: send_vrX(..): Failed to insert vrSpeak command for character \"" << char_id << "\".";
				SmartBody::util::log(strstr.str().c_str());
			}
		}

		if( send ) {
			_scene.getCommandManager()->getActiveSequences()->removeSequence(seq_id, true); // remove old sequence by this name
			if( !_scene.getCommandManager()->getActiveSequences()->addSequence(seq_id, std::move(seq) ))
			{
				std::stringstream strstr;
				strstr << "ERROR: send_vrX(..): Failed to insert seq into active sequences.";
				SmartBody::util::log(strstr.str().c_str());
				return msgId;
			}
		} else {
			_scene.getCommandManager()->getPendingSequences()->removeSequence(seq_id, true);  // remove old sequence by this name
			if (_scene.getCommandManager()->getPendingSequences()->addSequence(seq_id, std::move(seq)))
			{
				std::stringstream strstr;
				strstr << "ERROR: send_vrX(..): Failed to insert seq into pending sequences.";
				SmartBody::util::log(strstr.str().c_str());
				return msgId;
			}
		}
		return msgId;
	}
}

std::string SBBmlProcessor::execBML(std::string character, std::string bml)
{
	std::ostringstream entireBml;
	entireBml	<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	const std::string& procId = _scene.getProcessId();
	if (procId != "")
		entireBml << "<act procid=\"" << procId << "\">\n";
	else
		entireBml << "<act>\n";
	entireBml   << "\t<bml>\n"
				<< "\t\t" << bml
				<< "\t</bml>\n"
				<< "</act>";		

	if (_scene.getBoolAttribute("useNewBMLParsing"))
	{
		std::vector<BMLObject*> behaviors = parseBML(entireBml.str());
		scheduleBML(behaviors);
	}
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, entireBml.str() );
	return send_vrX( "vrSpeak", character, "ALL", "", false, true, entireBml.str() );	
}

std::string SBBmlProcessor::execBMLFile(std::string character, std::string filename)
{
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, filename );
	return send_vrX( "vrSpeak", character, "ALL", "", false, true, filename );
}

std::string SBBmlProcessor::execBMLFileId(std::string character, std::string filename, std::string id)
{
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, filename );
	return send_vrX("vrSpeak", character, "ALL", "", false, true, filename, id);
}

std::string SBBmlProcessor::execXMLFileId(std::string character, std::string filename, std::string id)
{
	return send_vrX("vrSpeak", character, "ALL", "", false, true, filename, id);
}


std::string SBBmlProcessor::execXMLFile(std::string character, std::string filename)
{
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, filename );
	return send_vrX("vrSpeak", character, "ALL", "", false, true, filename);
}

std::string SBBmlProcessor::execXML(std::string character, std::string xml)
{
	std::ostringstream entireXML;
	entireXML	<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				<< xml;
	//return send_vrX( "vrSpeak", character, "ALL", "", true, true, entireXML.str() );
	return send_vrX( "vrSpeak", character, "ALL", "", false, true, entireXML.str() );
	// do not send out VHMsg command, run it directly in this SmartBody instance.
	//return send_vrX( "vrSpeak", character, "ALL", "", false, false, entireXML.str() );
}

void SBBmlProcessor::execBMLAt(double time, std::string character, std::string bml)
{

	std::stringstream strstr;
	strstr << "bml char " << character << " " << bml;
	_scene.commandAt((float) time, strstr.str());
}

void SBBmlProcessor::execBMLFileAt(double time, std::string character, std::string filename)
{

	std::stringstream strstr;
	strstr << "bml char " << character << " file " << filename;
	_scene.commandAt((float) time, strstr.str());
}

void SBBmlProcessor::execXMLAt(double time, std::string character, std::string xml)
{
	std::stringstream strstr;
	strstr << "bml char " << character << " " << xml;
	_scene.commandAt((float) time, strstr.str());
}

void SBBmlProcessor::interruptCharacter(const std::string& character, double seconds)
{
	SBCharacter* sbCharacter = _scene.getCharacter(character);
	if (!sbCharacter)
	{
		SmartBody::util::log("No character named '%s' found. Interrupt not done.", character.c_str());
		return;
	}

	_bmlProcessor->interrupt(sbCharacter, seconds, _scene);
	
}

void SBBmlProcessor::interruptBML(const std::string& character, const std::string& id, double seconds)
{
	SBCharacter* sbCharacter = _scene.getCharacter(character);
	if (!sbCharacter)
	{
		SmartBody::util::log("No character named '%s' found. Interrupt not done.", character.c_str());
		return;
	}

	_bmlProcessor->interrupt(sbCharacter, id, seconds, _scene);
}


BML::Processor* SBBmlProcessor::getBMLProcessor()
{
	return _bmlProcessor.get();
}

bool SBBmlProcessor::hasSpeechBehavior(SbmCharacter& character) const {
	return !_bmlProcessor->hasSpeechBehavior(character).empty();
}


std::vector<BMLObject*> SBBmlProcessor::parseBML(const std::string& bml)
{
	rapidxml::xml_document<> doc;
	doc.parse< 0 >((char*) bml.c_str());
	
	std::string participant = "";
	std::vector<BMLObject*> behaviors;

	// process each act
	rapidxml::xml_node<>* actNode = doc.first_node("act"); 
	while (actNode)
	{
		// process each bml
		rapidxml::xml_node<>* participantNode = actNode->first_node("participant");
		if (participantNode)
		{
			rapidxml::xml_attribute<>* attr = participantNode->first_attribute("id");
			if (attr)
			{
				participant = attr->name();
			}
		}

		rapidxml::xml_node<>* bmlNode = actNode->first_node("bml");
		while (bmlNode)
		{
			rapidxml::xml_node<>* behaviorNode = bmlNode->first_node();
			while (behaviorNode)
			{
				char* behaviorName = behaviorNode->name();
				auto iter = _bmlHandlers.find(behaviorName);
				if (iter == _bmlHandlers.end())
				{
					SmartBody::util::log("BML parsing problem: cannot find handler for BML instruction: '%s'", behaviorName);
					continue;
				}
				auto& bmlHandlerObject = (*iter).second;
				// create an instance of this object that will handle the parsing
				BMLObject* bmlInstance = bmlHandlerObject->copy();
				// parse the XML
				bmlInstance->parse(behaviorNode);

				behaviors.emplace_back(bmlInstance);

				behaviorNode = behaviorNode->next_sibling();
			}
			bmlNode = bmlNode->next_sibling("bml");

		}
		actNode = actNode->next_sibling();
	}

	return behaviors;
}

void SBBmlProcessor::scheduleBML(std::vector<BMLObject*>& behaviors)
{
	for (auto iter = behaviors.begin();
		 iter != behaviors.end();
		 iter++)
	{


	}
}

void SBBmlProcessor::processBML(double time)
{
	for (auto & _bmlSchedule : _bmlSchedules)
	{
		SBBMLSchedule* schedule = _bmlSchedule.second;
		std::string characterName = _bmlSchedule.first;
		SBCharacter* character = _scene.getCharacter(characterName);
		if (!character)
		{
			// character no longer exists, remove the schedule
			schedule->remove();
		}

		// go through all the 


	}
}




} // namespace

