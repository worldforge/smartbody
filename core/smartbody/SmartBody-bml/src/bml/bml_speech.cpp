/*
 *  bml_speech.cpp - part of SmartBody-lib
 *  Copyright (C) 2009  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt

 */



#include <iostream>
#include <utility>
#include <vector>
#include <stack>

#include "bml_speech.hpp"

#include "bml.hpp"
#include "bml_exception.hpp"
#include "bml_xml_consts.hpp"
#include "bml_processor.hpp"
#include "sbm/BMLDefs.h"
#include <sb/SBScene.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBCommandManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCharacter.h>
#include "SBUtilities.h"
#include <sbm/sbm_speech_audiofile.hpp>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_blend.hpp>
#include <controllers/me_ct_face.h>

using namespace std;
using namespace BML;
using namespace SmartBody;



// XML Constants


const char* VISEME_NEUTRAL = "_";




// Replaces <tm> with <mark> in word break processing
// TODO: Enable both as part of a backward compatibile transition mode
//       or transition fully to <text> detection and processing.
#define ENABLE_BMLR_SPEECH_REQUEST_CODE  0

VecOfVisemeData BML::SpeechRequest::debugVisemeCurves;

// SpeechRequest Helper functions
void BML::SpeechRequest::createStandardSyncPoint( const std::wstring& sync_id, SyncPointPtr& sync ) {
	sync = trigger->addSyncPoint();
	behav_syncs.insert( sync_id, sync, behav_syncs.end() );
}


BML::SpeechRequestPtr BML::parse_bml_speech(
	DOMElement* xml,
	const std::string& unique_id,
	BML::BehaviorSyncPoints& behav_syncs,
	bool required,
	BML::BmlRequestPtr request,
	SmartBody::SBScene* scene )
{
	//SmartBody::util::log("parse BML speech");
	if (!request->actor->face_ct)
	{
		SmartBody::util::log("Character %s does not have a face controller, so cannot create speech.", request->actor->getName().c_str());
		return SpeechRequestPtr();
	}

	const XMLCh* id = xml->getAttribute(BMLDefs::ATTR_ID);
	std::string localId;
	xml_utils::xml_translate(&localId, id);

	request->localId = localId;

	// get the utterance policy: ignore, queue or interrupt
	const XMLCh* policy = xml->getAttribute(BMLDefs::ATTR_POLICY);
	std::string policyStr;
	xml_utils::xml_translate(&policyStr, policy);
	
	vector<SpeechMark> marks;  // Ordered list of named bookmarks

	// Parse <speech> for sync points
	const XMLCh* type = xml->getAttribute( BMLDefs::ATTR_TYPE );
	std::string typeStr;
	xml_utils::xml_translate(&typeStr, type);
	if( type ) {
#if ENABLE_BMLR_SPEECH_REQUEST_CODE
		// [BMLR] text/plain as default type
		if( *type == 0 ) {
			type = VALUE_TEXT_PLAIN;
		}
#endif

		if( XMLString::compareString( type, BML::BMLDefs::VALUE_TEXT_PLAIN )==0 || typeStr.size() == 0) {
#if !defined(__ANDROID__)
			if(LOG_SPEECH) wcout << "LOG: SpeechRequest::SpeechRequest(..): <speech type=\"" << BML::BMLDefs::VALUE_TEXT_PLAIN << "\">" << endl;
#endif
			// Search for <tm> sync_points
			DOMElement* child = xml_utils::getFirstChildElement( xml );
			while( child!=nullptr ) {
				const XMLCh* tag = child->getTagName();

#if ENABLE_BMLR_SPEECH_REQUEST_CODE
				 // [BMLR] Changed <tm> to <mark> and id="" to name=""
				if( XMLString::compareString( tag, TAG_MARK )==0 ) {
#if !defined(__ANDROID__)
					if(LOG_SPEECH) wcout << "LOG: SpeechRequest::SpeechRequest(..): Found <mark>" << endl;
#endif
#else
				if( XMLString::compareString( tag, BMLDefs::TAG_TM )==0 ) {
#if !defined(__ANDROID__)
					if(BML::LOG_SPEECH) wcout << "LOG: SpeechRequest::SpeechRequest(..): Found <tm>" << endl;
#endif
#endif

#if ENABLE_BMLR_SPEECH_REQUEST_CODE
					wstring tmId( child->getAttribute( ATTR_NAME ) );
#else
//					wstring tmId( child->getAttribute( BMLDefs::ATTR_ID ) );
					wstring tmId( xml_utils::xml_translate_wide( BMLDefs::ATTR_ID ) );
#endif
					// test validity?
					if( !tmId.empty() ) {
						if( isValidTmId( tmId ) ) {
							marks.emplace_back( SpeechMark( tmId, TIME_UNSET ) );
						} else {
#if ENABLE_BMLR_SPEECH_REQUEST_CODE
							wstrstr << "ERROR: Invalid <mark> name=\"" << tmId << "\"" << endl;
#else
							std::wstringstream wstrstr;
							wstrstr << "ERROR: Invalid <tm> id=\"" << tmId << "\"";
							SmartBody::util::log(convertWStringToString(wstrstr.str()).c_str());
#endif
							// TODO: remove mark from XML
						}
					}
				}
				child = xml_utils::getNextElement( child );
			}
		} else if( XMLString::compareString( type, BMLDefs::VALUE_SSML )==0 ) {
#if !defined(__ANDROID__)
			if(BML::LOG_SPEECH) wcout << "LOG: SpeechRequest::SpeechRequest(..): <speech type=\"" <<  BMLDefs::VALUE_SSML << "\">" << endl;
#endif
			// Search for <mark> sync_points
			DOMElement* child = xml_utils::getFirstChildElement( xml );
			while( child!=nullptr ) {
				const XMLCh* tag = child->getTagName();
				if( tag && XMLString::compareString( tag, BMLDefs::TAG_MARK )==0 ) {
#if !defined(__ANDROID__)
					if(BML::LOG_SPEECH) wcout << "LOG: SpeechRequest::SpeechRequest(..): Found <mark>" << endl;
#endif
					const XMLCh* tdIdXml = child->getAttribute(BMLDefs::ATTR_NAME);
					wstring tmId = xml_utils::xml_translate_wide(tdIdXml);
					// test validity?
					if( !tmId.empty() ) {
						if( isValidTmId( tmId ) ) {
							marks.emplace_back( SpeechMark( tmId, TIME_UNSET ) );
						} else {
							std::wstringstream wstrstr;
							wstrstr << "ERROR: Invalid <mark> name=\"" << tmId << "\"" << endl;
							SmartBody::util::log(convertWStringToString(wstrstr.str()).c_str());
							// TODO: remove <mark> from XML
						}
					}
				}
				child = xml_utils::getNextElement( child );
			}
		} else {
			std::wstringstream wstrstr;
			wstrstr << "ERROR: SpeechRequest::SpeechRequest(..): Unrecognized speech behavior type=\"" << type << "\"";
			SmartBody::util::log(convertWStringToString(wstrstr.str()).c_str());
		}
	} else {
		SmartBody::util::log("ERROR: SpeechRequest::SpeechRequest(..): Speech behavior lacks type attribute");
	}
	// Successfully parsed!!

	// request speech through Speech API
	SmartBody::SpeechInterface* speech_impl = request->actor->get_speech_impl();
	// get the backup speech
	SmartBody::SpeechInterface* speech_impl_backup = request->actor->get_speech_impl_backup();

	if( !speech_impl && speech_impl_backup ) {
		speech_impl = speech_impl_backup;
		speech_impl_backup = nullptr;
	}

	SmartBody::SpeechInterface* cur_speech_impl = speech_impl;
	SmartBody::SpeechInterface* cur_speech_impl_backup = speech_impl_backup;

	if (!cur_speech_impl) {
		ostringstream oss;
		oss << "No voice defined for actor \""<<request->actorId<<"\".  Cannot perform behavior \""<<unique_id<<"\".";
		throw BML::ParsingException( oss.str().c_str() );
	}

	

	// Before speech implementation, check if it's audio implementation, if yes, set the viseme mode
	AudioFileSpeech* audioSpeechImpl = dynamic_cast<AudioFileSpeech*>(cur_speech_impl);
	if (audioSpeechImpl)
	{	
		bool visemeMode = request->actor->get_viseme_curve_mode();
		audioSpeechImpl->setVisemeMode(visemeMode);
		audioSpeechImpl->setMotionMode(request->actor->getBoolAttribute("useCustomizedLipSyncIfPresent"));
	}
	AudioFileSpeech* audioSpeechImplBackup = dynamic_cast<AudioFileSpeech*>(cur_speech_impl_backup);
	if (audioSpeechImplBackup)
	{	
		bool visemeMode = request->actor->get_viseme_curve_mode();
		audioSpeechImplBackup->setVisemeMode(visemeMode);
		audioSpeechImplBackup->setMotionMode(request->actor->getBoolAttribute("useCustomizedLipSyncIfPresent"));
	}

	// Found speech implementation.  Making request.
	RequestId speech_request_id;
	try {
		//SmartBody::util::log("cur_speech, requestSpeechAudio");
		speech_request_id = cur_speech_impl->requestSpeechAudio( request->actorId.c_str(), request->actor->get_voice_code(), xml, "bp speech_ready " );
	} catch (...) {
		if (cur_speech_impl_backup) {
			cur_speech_impl = cur_speech_impl_backup;
			cur_speech_impl_backup = nullptr;
			speech_request_id = cur_speech_impl->requestSpeechAudio( request->actorId.c_str(), request->actor->get_voice_code_backup(), xml, "bp speech_ready " );
		}
		else
			throw BML::ParsingException("No backup speech available");
	}
	if (speech_request_id == 0)
	{
		if (cur_speech_impl_backup) {
			cur_speech_impl = cur_speech_impl_backup;
			cur_speech_impl_backup = nullptr;
			speech_request_id = cur_speech_impl->requestSpeechAudio( request->actorId.c_str(), request->actor->get_voice_code_backup(), xml, "bp speech_ready " );
		}
		else 
			throw BML::ParsingException("No backup speech available");
	}

	// TODO: SyncPoints of a speech behavior should be grouped under a unique TriggerEvent,
	//       rather the default start trigger.  The trigger identifies the additional processing
	//       necessary for the speech.
	//TriggerEventPtr trigger = request->createTrigger( L"SPEECH" );
	TriggerEventPtr trigger = behav_syncs.sync_start()->sync()->trigger.lock();

//// Old code:  behav_syncs are now parsed and passed in
//	// Current Speech behavior constraints prevent us from using the sync point attributes
//	// Creating new BehaviorSyncPoints instead of parsing the attributes.
//	createStandardSyncPoint( TM_START,        behav_syncs.sp_start );
//	createStandardSyncPoint( TM_READY,        behav_syncs.sp_ready );
//	createStandardSyncPoint( TM_STROKE_START, behav_syncs.sp_stroke_start );
//	createStandardSyncPoint( TM_STROKE,       behav_syncs.sp_stroke );
//	createStandardSyncPoint( TM_STROKE_END,   behav_syncs.sp_stroke_end );
//	createStandardSyncPoint( TM_RELAX,        behav_syncs.sp_relax );
//	createStandardSyncPoint( TM_END,          behav_syncs.sp_end );

	/* Removed by AS 2/10/12 - add this if <sync id=""/> from the audiofiles ever needs to be added directly to the sync points

	// convert any <sync id=""/> to synch points
	AudioFileSpeech* audioSpeech = dynamic_cast<AudioFileSpeech*>(cur_speech_impl);
	if (audioSpeech)		
	{
		std::map< SmartBody::RequestId, SmartBody::AudioFileSpeech::SpeechRequestInfo >& speechRequestInfo = 
			audioSpeech->getSpeechRequestInfo();

		std::map< SmartBody::RequestId, SmartBody::AudioFileSpeech::SpeechRequestInfo >::iterator iter = speechRequestInfo.find(speech_request_id);
		if (iter != speechRequestInfo.end())
		{
			SmartBody::AudioFileSpeech::SpeechRequestInfo& info = (*iter).second;
			for(std::map< std::string, float >::iterator markerIter = info.timeMarkers.begin();
				markerIter != info.timeMarkers.end();
				markerIter++)
			{
				std::string markerName = (*markerIter).first;
				float time = (*markerIter).second;
				SpeechMark speechMark(xml_utils::xml_s2w(markerName), time);
				marks.emplace_back(speechMark);
			}
		}
	}
	*/

	
	// extract the XML and store it in the request
	std::string speechXML;
	xml_utils::xmlToString( xml, speechXML ); 
	
	SpeechRequestPtr speechResult( new SpeechRequest( unique_id, localId, behav_syncs, cur_speech_impl, cur_speech_impl_backup, speech_request_id, policyStr, marks, request, speechXML ) );
	return speechResult;

}

//  SpeechRequest
//    (no transition/blend yet)
BML::SpeechRequest::SpeechRequest(
	const std::string& unique_id,
	const std::string& localId,
	BehaviorSyncPoints& syncs_in,
	SpeechInterface* speech_impl,
	SpeechInterface* speech_impl_backup,
	RequestId speech_request_id,
	std::string  policyOverride,
	vector<SpeechMark>& marks,
	const BmlRequestPtr& request,
	const std::string& speechAsXML
)
:	SequenceRequest( unique_id, localId, syncs_in, 0, 0, 0, 0, 0 ),
	speech_impl( speech_impl ),
	speech_impl_backup( speech_impl_backup ),
	speech_request_id( speech_request_id ),
	trigger( behav_syncs.sync_start()->sync()->trigger.lock() ),
	policy(std::move(policyOverride))
{
	speechXML = speechAsXML;
	// Add SyncPoints for SpeechMarks
	vector<SpeechMark>::const_iterator end = marks.end();
	for( vector<SpeechMark>::const_iterator mark = marks.begin(); mark != end; ++mark ) {
		// save the speech marks
		speechMarks.emplace_back(*mark);

		// Create a SyncPoint
		SyncPointPtr sync( trigger->addSyncPoint() );

		// Insert just before stroke_end
		auto stroke_end_pos = behav_syncs.sync_stroke_end();
		auto result_pos = behav_syncs.insert( mark->id, sync, stroke_end_pos );  // Test insertion, and throw error if problem

		// Remember Word Break
		if( !( wbToSync.insert( make_pair( mark->id, sync ) ).second ) )
		{
			std::wstringstream wstrstr;
			wstrstr << "ERROR: SpeechRequest(..): Failed to insert word break SyncPoint \""<<mark->id<<"\" into wbToSync map.";
			SmartBody::util::log(convertWStringToString(wstrstr.str()).c_str());
		}
	}
}

BML::SpeechRequest::~SpeechRequest() {
	// delete visemes
	size_t count = visemes.size();
	for( size_t i=0; i<count; ++i )
		delete visemes[i];

	// delete phonemes
	count = phonemes.size();
	for( size_t i=0; i<count; ++i )
		delete phonemes[i];
}

/*
SyncPoint* SpeechRequest::addWordBreakSync( const std::wstring& wbId ) {
	map< const XMLCh*, SyncPoint*, xml_utils::XMLStringCmp >& sync_points = trigger->request->sync_points;
	const XMLCh* tmId = buildBmlId( id, markId );

	if( sync_points.find( tmId ) == sync_points.end() ) {
		// id doesn't exist.. go ahead
		SyncPoint* sp = new SyncPoint( buildBmlId( id, markId ),
			                             trigger, relax->prev ); // append before relax
		sync_points.insert( make_pair( tmId, sp ) );
		return sp;
	} else {
		delete [] tmId;
		return nullptr;
	}
}
*/

SyncPointPtr BML::SpeechRequest::getWordBreakSync( const std::wstring& wbId ) {
	auto it = wbToSync.find( wbId );
	if( it == wbToSync.end() )
		return SyncPointPtr();
	else
		return it->second;
}

MapOfSyncPoint& BML::SpeechRequest::getWorkBreakSync()
{
	return wbToSync;
}

const std::vector<SpeechMark>& BML::SpeechRequest::getMarks()
{
	return speechMarks;
}

std::string BML::SpeechRequest::getSpeechXML()
{
	return speechXML;
}

std::string BML::SpeechRequest::getSpeechText()
{ 
	// if the speech text is empty but speech xml is not, extract the text from the XML
	if (speechText == "" && speechXML != "")
	{
		std::stringstream strstr;
		vector<char> xml_copy(speechXML.begin(), speechXML.end());
		xml_copy.emplace_back('\0');

		rapidxml::xml_document<> doc;
	
		//doc.parse<rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes>(&xml_copy[0]);
		doc.parse<rapidxml::parse_declaration_node>(&xml_copy[0]);

		std::stack<rapidxml::xml_node<>*> mystack;
		rapidxml::xml_node<>* speechNode = doc.first_node("speech");
		if (speechNode)
		{
			mystack.push(speechNode);
		}

		while (!mystack.empty())
		{
			rapidxml::xml_node<>* curNode = mystack.top();
			mystack.pop();

			rapidxml::xml_node<>* childNode = curNode->first_node(nullptr);
			if (childNode)
			{
				std::vector<rapidxml::xml_node<>*> children;
				children.emplace_back(childNode);
				rapidxml::xml_node<>* siblingNode = childNode->next_sibling(nullptr);
				while (siblingNode)
				{
					children.emplace_back(siblingNode);
					siblingNode = siblingNode->next_sibling(nullptr);
				}
				size_t numChildren = children.size();
				size_t i = numChildren - 1;
				for (size_t i = 0; i < numChildren; i++)
					mystack.push(children[numChildren - i - 1]);
			}
			

			rapidxml::node_type t = curNode->type();

			if (t == rapidxml::node_data)
				strstr << curNode->value();	
			else if (t == rapidxml::node_element)
				strstr << " ";
		}

		speechText = strstr.str();
	}
	
	return speechText;
}

void BML::SpeechRequest::speech_response( srArgBuffer& response_args ) {
	const char* status = response_args.read_token();
	const char* error_msg = nullptr;
	if( strcmp( status, "SUCCESS" )!=0 ) {
		if( strcmp( status, "ERROR" )==0 ) {
			error_msg = response_args.read_remainder_raw();
			if( error_msg == nullptr ) {
				error_msg = "!!NO ERROR MESSAGE!!";
			}
		} else {
			error_msg = "!!INVALID SPEECH CALLBACK SUBCOMMAND (bml_old_processor)!!";
			// TODO: include status in errorMsg without memory leak (use &std::String?)
		}
	}

	// TODO: parse response and set speech_error_msg
	this->speech_error_msg = error_msg? error_msg : string();
}

void BML::SpeechRequest::processVisemes(std::vector<VisemeData*>* result_visemes, BmlRequestPtr request, float scale)
{
	if (result_visemes == nullptr)
		return;

	// clean up debug data
	for (auto & debugVisemeCurve : debugVisemeCurves)
	{
		delete debugVisemeCurve;
	}
	debugVisemeCurves.clear();

	// process
	auto* character = dynamic_cast<SBCharacter*>(request->actor);
	if (!character)
		return;
	
	SpeechInterface* speechInterface = get_speech_interface();
	std::vector<std::string> emotionNames = speechInterface->getEmotionNames(get_speech_request_id());

	std::map<std::string, std::vector<float> > finalCurves;
	if (!emotionNames.empty())
	{
		std::map<std::string, std::vector<std::vector<float> > > emotionCurvesMap;
		for (auto & emotionName : emotionNames)
		{
			std::map<std::string, std::vector<float> > tempCurves = SBScene::getScene()->getDiphoneManager()->generateCurvesGivenDiphoneSet(result_visemes, emotionName, character->getName(), true, true, true, debugVisemeCurves);
			
			// merge it back according to emotion curve
			std::map<std::string, std::vector<float> >::iterator iter;
			for (iter = tempCurves.begin(); iter != tempCurves.end(); ++iter)
			{
				if (emotionCurvesMap.find(iter->first) == emotionCurvesMap.end())
				{
					emotionCurvesMap.insert(std::make_pair(iter->first, std::vector<std::vector<float> >()));
				}
				std::vector<float> weights = speechInterface->getEmotionCurve(get_speech_request_id(), emotionName);
				std::vector<float> weightedCurve = scaleCurve(iter->second, weights);
				emotionCurvesMap[iter->first].emplace_back(weightedCurve);

				// debug - before scale
				std::stringstream debugNameSS;
				debugNameSS << "[" << emotionName << "]" << iter->first;
				VisemeData* debugDiphoneVisemeCurve = new VisemeData(debugNameSS.str(), 0.0f);
				debugDiphoneVisemeCurve->setFloatCurve(iter->second, iter->second.size() / 2, 2);
				debugDiphoneVisemeCurve->setCurveInfo("6");
				debugVisemeCurves.emplace_back(debugDiphoneVisemeCurve);

				// debug - after scale
				std::stringstream debugNameSS1;
				debugNameSS1 << "[" << emotionName << "]" << iter->first;
				VisemeData* debugDiphoneVisemeCurve1 = new VisemeData(debugNameSS1.str(), 0.0f);
				debugDiphoneVisemeCurve1->setFloatCurve(weightedCurve, weightedCurve.size() / 2, 2);
				debugDiphoneVisemeCurve1->setCurveInfo("7");
				debugVisemeCurves.emplace_back(debugDiphoneVisemeCurve1);
			}
		}
		std::map<std::string, std::vector<std::vector<float> > >::iterator iter;
		for (iter = emotionCurvesMap.begin(); iter != emotionCurvesMap.end(); ++iter)
		{
			std::vector<float> mergedCurve;
			for (size_t i = 0; i < iter->second.size(); i++)
			{
				SmartBody::util::log("add curve for %s", iter->first.c_str());
				std::vector<float> tempCurve = addCurve(mergedCurve, iter->second[i]);
				mergedCurve = tempCurve;
			}
			if (finalCurves.find(iter->first) == finalCurves.end())
				finalCurves.insert(std::make_pair(iter->first, std::vector<float>()));
			finalCurves[iter->first] = mergedCurve;

			// debug merged curves
			std::stringstream debugNameSS;
			debugNameSS << iter->first;
			VisemeData* debugDiphoneVisemeCurve = new VisemeData(debugNameSS.str(), 0.0f);
			debugDiphoneVisemeCurve->setFloatCurve(mergedCurve, mergedCurve.size() / 2, 2);
			debugDiphoneVisemeCurve->setCurveInfo("8");
			debugVisemeCurves.emplace_back(debugDiphoneVisemeCurve);
		}
	}
	else
	{
		finalCurves = SBScene::getScene()->getDiphoneManager()->generateCurvesGivenDiphoneSet(result_visemes, character->getStringAttribute("lipSyncSetName"), character->getName(), true, true, true, debugVisemeCurves);
	}

	// assign back to viseme data
	for (auto & i : *result_visemes)
	{
		delete i;
	}
	(*result_visemes).clear();
	std::map<std::string, std::vector<float> >::iterator curveIter;
	for (curveIter = finalCurves.begin(); curveIter != finalCurves.end(); ++curveIter)
	{
		VisemeData* newVis = new VisemeData(curveIter->first, 0);
		newVis->setFloatCurve(curveIter->second, curveIter->second.size() / 2, 2);
		result_visemes->emplace_back(newVis);
	}
}




void BML::SpeechRequest::schedule( time_sec now ) {
	//// TODO: Sync to prior behaviors
	// behav_syncs.applyParentTimes()
	// find set SyncPoints
	// if more than one, warn and ignore least important
	

	// Convience references
	SyncPointPtr sp_start( behav_syncs.sync_start()->sync() );
	SyncPointPtr sp_ready( behav_syncs.sync_ready()->sync() );
	SyncPointPtr sp_stroke_start( behav_syncs.sync_stroke_start()->sync() );
	SyncPointPtr sp_stroke( behav_syncs.sync_stroke()->sync() );
	SyncPointPtr sp_stroke_end( behav_syncs.sync_stroke_end()->sync() );
	SyncPointPtr sp_relax( behav_syncs.sync_relax()->sync() );
	SyncPointPtr sp_end( behav_syncs.sync_end()->sync() );

	string warning_context = string( "Behavior \"" ) + unique_id + "\"";
	behav_syncs.applyParentTimes( warning_context );

	
	time_sec offset = 0;
	if (isTimeSet(sp_start->time))
		offset = sp_start->time - now;

	BmlRequestPtr       request  = trigger->request.lock();
	SbmCharacter* actor    = request->actor;
	string              actor_id = request->actorId;

	// Found speech implementation.  Making request.
	if( !speech_error_msg.empty() ) {
		ostringstream oss;
		oss << "SpeechInterface error: "<<speech_error_msg;
		throw SchedulingException( oss.str().c_str() );
	}

	audioPlay = speech_impl->getSpeechPlayCommand( speech_request_id, actor );
	audioStop = speech_impl->getSpeechStopCommand( speech_request_id, actor );
	audioFile = speech_impl->getSpeechAudioFilename(speech_request_id);
	// save the audio file on the character
	actor->setStringAttribute("lastAudioFile", audioFile);
	
	if( LOG_AUDIO ) {
		cout << "DEBUG: BML::SpeechRequest::processReply(): audioPlay = " << audioPlay << endl;
		cout << "DEBUG: BML::SpeechRequest::processReply(): audioStop = " << audioStop << endl;
	}

	// save timing;
	time_sec first_open  = TIME_UNSET;  // start of first non-neutral viseme
	time_sec last_open   = TIME_UNSET;  // end of last non-neutral viseme
	time_sec last_viseme = TIME_UNSET;  // end of last viseme
	time_sec longest_viseme = TIME_UNSET;

	// Extract Visemes	
	// first copy -> m_speechRequestInfo parsed the visemes from files
	// second copy -> now getVisems copied it again so it's ready for process
	vector<VisemeData*>* result_visemes = speech_impl->getVisemes( speech_request_id, actor );
	if( !result_visemes ) {
		if (speech_impl_backup) // run the backup speech server if available
			result_visemes = speech_impl->getVisemes( speech_request_id, nullptr );
	}

#if 0
	VisemeData* emotionViseme = nullptr;
	if (result_visemes->size() > 0)
	{
		VisemeData* temp = (*result_visemes)[result_visemes->size() - 1];
		if (strcmp(temp->id(), "emotion") == 0)
		{
			emotionViseme = new VisemeData(temp->id(), temp->time());
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(temp->getCurveInfo(), tokens);
			for (size_t i = 0; i < tokens.size(); ++i)
				temp->getFloatCurve().emplace_back(atof(tokens[i].c_str()));
			emotionViseme->setFloatCurve(temp->getFloatCurve(), temp->getNumKeys(), 4);
		}
	}
#endif
	if (result_visemes)
	{
		// Save Phonemes
		for (size_t i = 0; i < (*result_visemes).size(); i++)
		{
			VisemeData* v = (*result_visemes)[i];
			VisemeData* newV = new VisemeData(v->id(), v->time());
			phonemes.emplace_back(newV);
		}

		// Process Visemes
		if (actor && actor->isDiphone()) // if use diphone, reconstruct the curves
			processVisemes(result_visemes, request, actor->getDiphoneScale());
	}

#if 0
	bool shouldInsert = true;
	for ( size_t i = 0; i < (*result_visemes).size(); i++ )
	{
		VisemeData* v = (*result_visemes)[ i ];
		if (strcmp(v->id(), "emotion") == 0)
		{
			shouldInsert = false;
			break;
		}
	}
	if (shouldInsert && emotionViseme)
	{
		result_visemes->emplace_back(emotionViseme);
	}
#endif

	if (result_visemes)
	{
		//visemes = *result_visemes;  // Copy contents
		// third copy -> to drop any visemes that don't exceed the viseme threshold
		for ( size_t i = 0; i < (*result_visemes).size(); i++ )
		{
			VisemeData* v = (*result_visemes)[ i ];

			// drop any visemes that don't exceed the viseme threshold
			if (v->duration() < actor->getMinVisemeTime() && !actor->isDiphone())
				continue;
			if (v->isMotionMode())
				visemes.emplace_back(new VisemeData(v->id()));
			else if (!v->isCurveMode() && !v->isTrapezoidMode() && !v->isFloatCurveMode())
				visemes.emplace_back( new VisemeData( v->id(), v->weight(), v->time() ) );
			else if (v->isTrapezoidMode() && !v->isFloatCurveMode())
				visemes.emplace_back( new VisemeData( v->id(), v->weight(), v->time(), v->duration(), v->rampin(), v->rampout() ) );
			else if (!v->isFloatCurveMode())
				visemes.emplace_back( new VisemeData( v->id(), v->getNumKeys(), v->getCurveInfo() ));
			else
			{
				VisemeData* vcopy = new VisemeData( v->id(), v->time());
				vcopy->setFloatCurve(v->getFloatCurve(), v->getNumKeys(), v->getFloatsPerKey());
				visemes.emplace_back( vcopy );
			}
		}

		// remove results_visemes
		for (auto & result_viseme : *result_visemes)
			delete result_viseme;
		delete result_visemes;

		vector<VisemeData*>::iterator cur = visemes.begin();
		vector<VisemeData*>::iterator end = visemes.end();

		if( BML::LOG_SPEECH && cur==end )
		{
			std::stringstream strstr;
			strstr << "ERROR: BodyPlannerImpl::speechReply(): speech.getVisemes( " << speech_request_id << " ) is empty.";
			SmartBody::util::log(strstr.str().c_str());
		}

		for( ; cur!=end; ++cur ) {
			VisemeData* v = (*cur);
			v->calculateDuration();

			if( BML::LOG_SPEECH ) {
				//cout << "   " << (*v) << endl;  // Not linking
				cout << "   VisemeData: " << v->id() << " (" << v->weight() << ") @ " << v->time() << endl;
			}
			if( strcmp( v->id(), VISEME_NEUTRAL )!=0 ) {
				if( !isTimeSet( first_open ) )
					first_open = v->time();
				last_open = v->time() + v->duration();
			}
			last_viseme = v->time() + v->duration();
			if ( !isTimeSet(longest_viseme))
				longest_viseme = v->duration();
			else if (longest_viseme < v->duration())
				longest_viseme = v->duration();
		}
	} else {

		if( BML::LOG_SPEECH )
		{
			std::stringstream strstr;
			strstr << "WARNING: BodyPlannerImpl::speechReply(): speech.getVisemes( " << speech_request_id << " ) returned nullptr.";
			SmartBody::util::log(strstr.str().c_str());
		}
	}

	if (last_open < longest_viseme) // ensures that curve mode will send bml:end at the proper time
	{
		last_open = longest_viseme;
		last_viseme = longest_viseme;
	}

	time_sec start_time = now + offset;
	if (isTimeSet(sp_start->time))
		start_time = sp_start->time;
	else
		sp_start->time = start_time;
	//  Set core sync_point times
	
	if( isTimeSet( last_viseme ) ) {
		last_viseme += start_time;

		if( isTimeSet( first_open ) ) {
			first_open += start_time;
			last_open  += start_time;

			sp_ready->time        = first_open;
			sp_stroke_start->time = first_open;
			sp_stroke->time       = first_open;
			sp_stroke_end->time   = last_open;
			sp_relax->time        = last_open;
		} else {
			// Never opens mouth
			sp_ready->time        = start_time;
			sp_stroke_start->time = start_time;
			sp_stroke->time       = start_time;
			sp_stroke_end->time   = last_viseme;
			sp_relax->time        = last_viseme;
		}
		sp_end->time = last_viseme;
	} else {
		// No timing information
		sp_ready->time        = start_time;
		sp_stroke_start->time = start_time;
		sp_stroke->time       = start_time;
		sp_stroke_end->time   = start_time;
		sp_relax->time        = start_time;
		sp_end->time          = start_time;
	}

	// Process Word Break SyncPoints
	auto wb_it  = wbToSync.begin();
	auto wb_end = wbToSync.end();
	if( wb_it != wb_end ) {
		for(; wb_it != wb_end; ++wb_it ) {
			const wstring& wb_id = wb_it->first;
			SyncPointPtr  cur   = wb_it->second;

			if( cur->parent != nullptr && !isTimeSet( cur->parent->time ) )
			{
				std::wstringstream wstrstr;
				wstrstr << "ERROR: BodyPlannerImpl::speechReply(): Unhandled case of Wordbreak SyncPoint \"" << wb_id << "\" with scheduled parent SyncPoint.  Ignoring offset.";
				SmartBody::util::log(convertWStringToString(wstrstr.str()).c_str());
			}

//			float audioTime = speech_impl->getMarkTime( speech_request_id, wb_id.c_str() );

			XMLCh *xml_ch_p = xml_utils::xmlch_translate( xml_utils::xml_w2s( wb_id ) );
			float audioTime = speech_impl->getMarkTime( speech_request_id, xml_ch_p );
			xml_utils::xmlch_release( &xml_ch_p );
			
			if (audioTime < 0)
			{
				std::string wordBreakId(wb_id.begin(), wb_id.end());
				int pos = wordBreakId.find(":");
				if (pos == std::string::npos)
				{ // prefix was not given - try again with proper prefix
					std::string wordBreakIdWithPrefix = this->local_id;
					wordBreakIdWithPrefix.append(":");
					wordBreakIdWithPrefix.append(wordBreakId);
					audioTime = speech_impl->getMarkTime(speech_request_id, xml_utils::UTF16(wordBreakIdWithPrefix.c_str()));
				}
				else
				{ // prefix was given - try again without prefix
					std::string wordBreakSuffix = wordBreakId.substr(pos + 1, wordBreakId.size() - pos - 1);
					audioTime = speech_impl->getMarkTime(speech_request_id, xml_utils::UTF16(wordBreakSuffix.c_str()));
				}

			}
			if( audioTime >= 0 ) {
#if !defined(__ANDROID__)
				if( BML::LOG_SYNC_POINTS ) wcout << "   Wordbreak SyncPoint \"" << wb_id << "\" @ " << audioTime << endl;
#endif
				cur->time = start_time + audioTime;
			} else {
				std::wstringstream wstrstr;
				wstrstr << "ERROR: BodyPlannerImpl::speechReply(): No audioTime for Wordbreak SyncPoint \"" << wb_id << "\"";
				SmartBody::util::log(convertWStringToString(wstrstr.str()).c_str());
			}
		}
	} else {
		if( BML::LOG_SYNC_POINTS )
			cout << "   BodyPlannerImpl::speechReply(..): No speech bookmarks" << endl;
	}
}

void BML::SpeechRequest::realize_impl( BmlRequestPtr request, SmartBody::SBScene* scene )
{
	//SmartBody::util::log("SpeechRequest, realize impl");
	// Get times from SyncPoints
	time_sec startAt  = behav_syncs.sync_start()->time();
	time_sec readyAt  = behav_syncs.sync_ready()->time();
	time_sec strokeAt = behav_syncs.sync_stroke()->time();
	time_sec relaxAt  = behav_syncs.sync_relax()->time();
	time_sec endAt    = behav_syncs.sync_end()->time();

#if ENABLE_DIRECT_VISEME_SCHEDULE
	SbmCharacter *actor_p = (SbmCharacter*)( request->actor );
#endif
	const string& actor_id = request->actor->getName();

//// SyncPoints should already be set from viseme processing
//	{	// Offset prior syncpoint times by startAt
//		BehaviorSyncPoints::iterator it = behav_syncs.begin();
//		BehaviorSyncPoints::iterator end = behav_syncs.end();
//		for( ; it != end ; ++it ) {
//			SyncPointPtr sync = (*it);
//			if( isTimeSet( sync->time ) ) {
//				sync->time += startAt;
//			}
//		}
//	}

	// Schedule visemes
	//   visemes are stored in request->visemes as VisemeData objects (defined in bml.hpp)
	// add audioOffset to each viseme time,
	double lastTime = endAt;
	if( !visemes.empty() ) {
		//// Replaced by addition in next loop
		//for( int i=0; i<(int)request->visemes.size(); i++ ) {
		//	request->visemes.at(i)->time+= audioOffset;
		//}

		ostringstream command;
		const size_t viseme_count = visemes.size();
		for( size_t i=0; i<viseme_count; i++ ) { //adds visemes for audio into sequence file
			VisemeData* v = visemes.at(i);
			time_sec time = (time_sec)( v->time() + startAt );
			
			if (v->isMotionMode())
			{
				// calling face controller directly
				if (request->actor->face_ct)
				{
					request->actor->face_ct->customizeMotion(v->id(), time);
					if (time > lastTime)
						lastTime = time;
				}
			}
			else if (v->isFloatCurveMode())
			{
				command.str( "" );
				std::vector<float>& data = v->getFloatCurve();
				int numKeys = v->getNumKeys();
				int floatsPerKey = v->getFloatsPerKey();
				command << "char " << actor_id << " viseme " << v->id() << " curve " << numKeys << ' ';
				for (int x = 0; x < numKeys; x++)
				{
					command << data[x * floatsPerKey] << " " << data[x * floatsPerKey + 1] << " "; 
				}
				string cmd_str = command.str();
				SbmCommand *cmd = new SbmCommand( cmd_str, time );
				sbm_commands.emplace_back( cmd );
				double lastVisemeTime = data[(numKeys - 1) * floatsPerKey];
				if (time + lastVisemeTime > lastTime)
					lastTime = time  + lastVisemeTime;
			}
			else if (!v->isCurveMode())
			{
#if ENABLE_DIRECT_VISEME_SCHEDULE
				float ramp_dur;
				if( v->duration() > 0 ) {
					ramp_dur = v->duration();
				} else {
					// speech implementation doesn't appear to support durations.
					// using 0.1 transition duration (and start transition early)
					ramp_dur = 0.1f;
					time -= (time_sec)0.05;
				}
				actor_p->set_viseme_blend_ramp( v->id(), time, v->weight(), ramp_dur );
#else
				float duration = v->duration();
				if( duration <= 0 ) {
					// speech implementation doesn't appear to support durations.
					// using 0.1 transition duration (and start transition early)
					duration = .1f;
					time -= (time_sec)0.05;
				}
				
				command.str( "" );
				command << "char " << actor_id << " viseme " << v->id() << " trap " 
						<< v->weight() << " " 
						<< duration << " " 
						<< v->rampin() << " "
						<< v->rampout() << " ";
				
				string cmd_str = command.str();
				SbmCommand *cmd = new SbmCommand( cmd_str, time );
				sbm_commands.emplace_back( cmd );
//				sbm_commands.emplace_back( new SbmCommand( command.str(), time ) );
				if (time + duration > lastTime)
					lastTime = time + duration;
#endif
				if( LOG_BML_VISEMES ) cout << "command (complete): " << command.str() << endl;
			}
			else
			{
#if ENABLE_DIRECT_VISEME_SCHEDULE

				int n = v->getNumKeys();
				float *curve_info = new float[ 2 * n ];
				srArgBuffer curve_string( v->getCurveInfo() );
				curve_string.read_float_vect( curve_info, 2 * n );
#if 0
				actor_p->set_viseme_blend_curve( v->id(), mcu->time, 1.0f, curve_info, n, 2 );
#else
				actor_p->set_viseme_curve( v->id(), mcu->time + startAt, curve_info, n, 2, 0.1f, 0.1f );
#endif
				delete [] curve_info;

#else
				if (SmartBody::SBScene::getScene()->getCharacter(actor_id)->getBoolAttribute("dominancecurve"))
				{
					std::vector<std::string> cTokens;
					SmartBody::util::tokenize(v->getCurveInfo(), cTokens);
					std::vector<float> cValue;
					cValue.resize(cTokens.size());
					for (size_t i = 0; i < cTokens.size(); ++i)
					{
						cValue[i] = (float)atof(cTokens[i].c_str());
					}
					if (cTokens.size() == 4 * v->getNumKeys())
					{
						std::stringstream ss;

						for (size_t i = 0; i < cValue.size(); ++i)
						{
							if (i % 4 == 1)
							{
								if (!SmartBody::SBScene::getScene()->getCharacter(actor_id)->hasAttribute(std::string(v->id())))
								{
									SmartBody::util::log("Error! doesn't have attribute %s", v->id());
									continue;
								}
								double attr = SmartBody::SBScene::getScene()->getCharacter(actor_id)->getDoubleAttribute(std::string(v->id()));
								std::string defaultAttrString = std::string(v->id()) + "_default";
								double defaultAttr = SmartBody::SBScene::getScene()->getCharacter(actor_id)->getDoubleAttribute(defaultAttrString);
								if (attr == 0)
									attr = 1.0;
								cValue[i] /= (float)(attr - defaultAttr);
							}
							ss << cValue[i] << " ";
						}
						v->setCurveInfo(ss.str().c_str());
					}
					else
					{
						SmartBody::util::log("Bad input data!");
					}		
				}
				/*
				// Comment out old way of realizing visemes. There's no need to use commands as intermediate layer
				command.str( "" );
				command << "char " << actor_id << " viseme " << v->id() << " curve " << v->getNumKeys() << ' ' << v->getCurveInfo();			
				string cmd_str = command.str();
				SbmCommand *cmd = new SbmCommand( cmd_str, time );
				sbm_commands.emplace_back( cmd );
				*/

				// directly schedule into head_schedule_ct
				std::vector<std::string> tokens;
				SmartBody::util::tokenize(v->getCurveInfo(), tokens);
				int numKeyParam = tokens.size() / v->getNumKeys();
				float* curve = new float[tokens.size()];
				for (size_t i = 0; i < tokens.size(); ++i)
					curve[i] = (float)atof(tokens[i].c_str());
				request->actor->schedule_viseme_curve(v->id(), time, curve, v->getNumKeys(), numKeyParam, 0.0f, 0.0f);
				double lastVisemeTime = curve[(v->getNumKeys() - 1) * 2];
				if (time + lastVisemeTime > lastTime)
					lastTime = time  + lastVisemeTime;
#endif
				if( LOG_BML_VISEMES ) cout << "command (complete): " << command.str() << endl;
			}

			////visemes get set a specified time
			//if( seq->insert( time, (char*)(command.str().c_str()) )!=CMD_SUCCESS ) {
			//	strstr << "WARNING: BodyPlannerImpl::realizeRequest(..): msgId=\""<<bpMsg.msgId<<"\": "<<
			//		"Failed to insert viseme \""<<v->id()<<"\" @ "<<time<<endl;
			//}
			if( LOG_BML_VISEMES ) {
				ostringstream echo;
				echo << "echo LOG_BML_VISEMES:\t" << time << ":\t" << command.str();
				string cmd_str = echo.str();
				SbmCommand *cmd = new SbmCommand( cmd_str, time );
				sbm_commands.emplace_back( cmd );
//				sbm_commands.emplace_back( new SbmCommand( echo.str(), time ) );
			}
		}
	} else {
		SmartBody::util::log("WARNING: BodyPlannerImpl::realizeRequest(..): SpeechRequest has no visemes.");
	}

	// Schedule audio
	if( !audioPlay.empty() ) {
		if( LOG_AUDIO || LOG_BML_VISEMES )
			cout << "DEBUG: BodyPlannerImpl::realizeRequest(..): scheduling request->audioPlay: " << audioPlay << endl;
		// schedule for later		
		sbm_commands.emplace_back( new SbmCommand( audioPlay, startAt + request->actor->get_viseme_sound_delay() ) );
		//if( seq->insert( (float)(audioOffset<0? 0: audioOffset), audioPlay.c_str() ) != CMD_SUCCESS ) {
		//	SmartBody::util::log( "ERROR: BodyPlannerImpl::realizeRequest: insert audio trigger into seq FAILED, msgId=%s\n", bpMsg.msgId ); 
		//}
	} else {
		SmartBody::util::log("WARNING: BodyPlannerImpl::realizeRequest(..): SpeechRequest has no audioPlay command.");
	}

	behav_syncs.sync_end()->set_time(lastTime);
	realize_sequence( sbm_commands, scene );
}


void BML::SpeechRequest::unschedule(  SmartBody::SBScene* scene,
	                            BmlRequestPtr request,
	                            time_sec duration )
{
	unschedule_sequence( scene );

	/*
	// Clear visemes
	ostringstream cmd;
	cmd << "char " << request->actor->getName() << " viseme ALL 0 " << duration;
	SmartBody::SBScene::getScene()->getCommandManager()->execute_later( cmd.str().c_str(), 0 );
	*/
	MeCtSchedulerClass::VecOfTrack tracks = request->actor->head_sched_p->tracks();
	for (const auto& track : tracks)
	{
			const auto& unary_blend_ct = track->blending_ct();
		if( unary_blend_ct &&
			unary_blend_ct->controller_type() == MeCtBlend::CONTROLLER_TYPE )
		{
			MeCtBlend* blend = static_cast<MeCtBlend*>(unary_blend_ct.get());
			srLinearCurve& blend_curve = blend->get_curve();
			double t = scene->getSimulationManager()->getTime();
			double v = blend_curve.evaluate( t );
			blend_curve.clear_after( t );
			blend_curve.insert( t, v );
			blend_curve.insert( t + duration, 0.0 );
		}
	}

	if( !audioStop.empty() )
		SmartBody::SBScene::getScene()->getCommandManager()->execute_later( audioStop.c_str(), request->actor->get_viseme_sound_delay() );
	else
		SmartBody::util::log("WARNING: SpeechRequest::unschedule(): unique_id \"%s\": Missing audioStop.", unique_id.c_str());
}
	                            
void BML::SpeechRequest::cleanup( SmartBody::SBScene* scene, BmlRequestPtr request )
{
	for (auto & viseme : visemes)
		delete viseme;

	visemes.clear();
	unschedule_sequence( scene );

	speech_impl->requestComplete( speech_request_id );
}

BehaviorSpan BML::SpeechRequest::getBehaviorSpan()
{
	BehaviorSpan span = behav_syncs.getBehaviorSpan( PERSISTENCE_THRESHOLD );

	size_t numPhonemes = this->getPhonemes().size();
	if (numPhonemes > 0)
	{
		BML::VecOfVisemeData& p = this->getPhonemes();
		SmartBody::VisemeData* v = p[numPhonemes - 1];
		float t = v->time();
		span.end = span.start + (double) t;
	}
		
	

	return span;
}
