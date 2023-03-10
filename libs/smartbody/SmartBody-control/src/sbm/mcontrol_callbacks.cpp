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


#include "mcontrol_callbacks.h"

#include "vhmsg-tt.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/algorithm/string/replace.hpp>
#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include <sb/SBMotion.h>
#include <sb/SBAssetManager.h>
#include <sb/SBSkeleton.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBSpeechManager.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBBoneBusManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBCommandManager.h>
#include "SBUtilities.h"
#include <controllers/me_ct_param_animation.h>
#include <controllers/me_ct_data_receiver.h>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_breathing.h>
#include <controllers/me_controller_tree_root.hpp>
#include <sbm/PPRAISteeringAgent.h>
#include "sb/SBSteerManager.h"
#include <sbm/Heightfield.h>
#include <sbm/KinectProcessor.h>
#include <sbm/local_speech.h>

#include <sbm/action_unit.hpp>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "sbm/sbm_audio.h"

#include "sr/sr_model.h"
#include "sb/SBEvent.h"

#include <boost/algorithm/string.hpp>
#include "sb/SBCharacter.h"
#include <sb/SBJointMapManager.h>
#include <sb/SBJointMap.h>
#include <cmath>
#include <controllers/me_ct_gaze.h>
#include <controllers/me_ct_motion_player.h>

#ifdef USE_GOOGLE_PROFILER
#include <gperftools/profiler.h>
#include <gperftools/heap-profiler.h>
#endif




using namespace std;


int deprecatedMessage( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr  )
{
	SmartBody::util::log("Command %s has been deprecated.", args.read_remainder_raw());
	return CMD_FAILURE;
}



/*

	PREPROCESSOR: make paths available immediately to inline seq preprocessor
		seq <name> inline

*/

void mcu_preprocess_sequence( srCmdSeq *to_seq_p, srCmdSeq *fr_seq_p, SmartBody::SBCommandManager& cmdMgr )	{

	
	fr_seq_p->reset();
	auto command_event = fr_seq_p->remove();
	while (!command_event.cmd.empty()) {
		auto t = command_event.time;
		auto& cmd = command_event.cmd;
		srArgBuffer args(command_event.cmd.c_str());
		srCmdSeq* inline_seq_p = nullptr;

		char* tok = args.read_token();
		if (strcmp(tok, "seq") == 0) {
			char* name = args.read_token();
			tok = args.read_token();
			if (strcmp(tok, "inline") == 0) {

				inline_seq_p = cmdMgr.lookup_seq(name);

				if (inline_seq_p == nullptr) {
					SmartBody::util::log("mcu_preprocess_sequence ERR: inline seq '%s' NOT FOUND\n", name);
					return;
				}
			}
		}

		float absolute_offset = fr_seq_p->offset() + t;
		if (inline_seq_p) {
			// iterate hierarchy
			inline_seq_p->offset(absolute_offset);
			mcu_preprocess_sequence(to_seq_p, inline_seq_p, cmdMgr);
		} else if (!cmd.empty()) {
			// propagate un-consumed command
			to_seq_p->insert(absolute_offset, cmd);
		}

		command_event = fr_seq_p->remove();
	}
	delete fr_seq_p;
}

int begin_sequence( char* name, SmartBody::SBCommandManager& cmdMgr, SmartBody::SBSimulationManager& simMgr)	{
	
	

	srCmdSeq *seq = cmdMgr.lookup_seq( name );
	
	if( seq )
	{
		auto copySeq = std::make_unique<srCmdSeq>();
		mcu_preprocess_sequence( copySeq.get(), seq, cmdMgr);

		copySeq->offset( (float)( simMgr.getTime() ) );
		bool success = cmdMgr.getActiveSequences()->addSequence(name, std::move(copySeq) );

		if( !success )
		{
			SmartBody::util::log( "begin_sequence ERR: insert active: '%s' FAILED\n", name ); 
			return CMD_FAILURE;
		}
		
	}

	return( CMD_SUCCESS );
}

/*

	seq <name> at <time> <cmd...>	
	seq <name> [begin|abort|print]
#	seq <name> write

*/

int mcu_sequence_func( srArgBuffer& args, SmartBody::SBCommandManager& cmdMgr, SmartBody::SBSimulationManager& simMgr )
{
	char *seqName = args.read_token();
	char *seqCmd = args.read_token();
	

	//std::cout << "SEQUENCE LOADED: " << seq_name << " " << seq_cmd << std::endl;
	//std::string seqStr = seq_cmd;
	//SmartBody::util::log("mcu_sequence_func : seq_name = %s, seqStr = %s",seq_cmd, seqStr.c_str());
	if( ( strcmp( seqCmd, "begin" ) == 0 )||( strcmp( seqCmd, EMPTY_STRING ) == 0 ) )	{
		int ret = begin_sequence( seqName, cmdMgr, simMgr );
		return ret;
	}
	else
	if( strcmp( seqCmd, "at" ) == 0 )	{
		
		srCmdSeq* seq = cmdMgr.getPendingSequences()->getSequence( seqName );
		if (!seq)
		{
			seq = new srCmdSeq;
			bool success = cmdMgr.getPendingSequences()->addSequence( seqName, std::unique_ptr<srCmdSeq>(seq) );
			if( !success )
			{
				SmartBody::util::log( "mcu_sequence_func ERR: insert pending '%s' FAILED\n", seqName ); 
				return( CMD_FAILURE );
			}
		}
			
		float seqTime = args.read_float();
		char* seqStr = args.read_remainder_raw();
		int ret = seq->insert( seqTime, seqStr );
		return ret;
	}
	else
	if( strcmp( seqCmd, "print" ) == 0 )	{
			
		srCmdSeq* seq = cmdMgr.getPendingSequences()->getSequence( seqName );
		if (!seq)
		{
			SmartBody::util::log( "mcu_sequence_func ERR: print: '%s' NOT FOUND\n", seqName ); 
			return( CMD_FAILURE );
		}
		seq->print( stdout );
	}
	else
	if( ( strcmp( seqCmd, "abort" ) == 0 )||( strcmp( seqCmd, "delete" ) == 0 ) )	{
		int result = cmdMgr.abortSequence( seqName );
		if( result == CMD_NOT_FOUND )	{
			SmartBody::util::log( "mcu_sequence_func ERR: abort|delete: '%s' NOT FOUND\n", seqName ); 
		}
		return( result );
	}
	else {
		return( CMD_NOT_FOUND );
	}
	return( CMD_SUCCESS );
}

/*
	seq-chain <seqname>*
*/

int mcu_sequence_chain_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr ) {
	vector<string> seq_names;
	const char* token = args.read_token();
	while( token[0] != '\0' ) {
		seq_names.emplace_back( token );

		token = args.read_token();
	}

	if( seq_names.empty() ) {
		SmartBody::util::log("ERROR: seq-chain expected one or more .seq filenames.");
		return CMD_FAILURE;
	}

	return cmdMgr->execute_seq_chain( seq_names, "ERROR: seq-chian: " );
}


/////////////////////////////////////////////////////////////

/*
	Executes a command to set a configuration variable.
*/



/////////////////////////////////////////////////////////////


std::string tokenize( std::string& str,
					  const std::string& delimiters = " ",
					  int mode = 1 )
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	if (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		std::string return_string = str.substr(lastPos, pos - lastPos);
		std::string::size_type lastPos = str.find_first_not_of(delimiters, pos);
		if (mode == 1)
		{
			if (std::string::npos == lastPos)	str = "";
			else								str = str.substr(lastPos, str.size() - lastPos);
		}
		return return_string;
	}
	else
		return "";
}

/**
	panim state <state-name> cycle <cycle> <number of motions> <motion1> <motion2>... <motionN> <number of keys per motion> <keys for motion1> <keys for motion2> <keys for motion3>
	panim state <state-name> parameter <type> <number of motions that have parameters> <motion1> <parameter1> <motion2> <parameter2>
	panim schedule char <char-name> state <state-name> loop <loop> <weight list>
	panim update char <char-name> state <weight list>
	panim transition fromstate <state-name> tostate <state-name> <motion-name1 in first state> <2 keys for motion-name1> <motion-name2 in second state> <2 keys for motion-name2>
**/
double parseMotionParameters(SmartBody::SBAssetManager& assetManager, const std::string& m, std::string parameter, double min, double max)
{
	std::string skeletonName = tokenize(parameter, "|");
	boost::intrusive_ptr<SmartBody::SBSkeleton> sk;
	if (!parameter.empty())
	{

		sk = assetManager.getSkeleton(skeletonName);
		if (!sk)
			SmartBody::util::log("parseMotionParameters ERR: skeleton %s not found! Parameter won't be setup properly", skeletonName.c_str());
	}
	int type = 0;
	if (parameter == "speed1")
		type = 0;
	if (parameter == "speed2")
		type = 1;
	if (parameter == "angular1")
		type = 2;
	if (parameter == "angular2")
		type = 3;
	if (parameter == "transitionx")
		type = 4;
	if (parameter == "transitiony")
		type = 5;
	if (parameter == "transitionz")
		type = 6;
	if (parameter == "avgrooty")
		type = 7;
	if (!sk) return -9999;
	MotionParameters mParam(assetManager.getMotion(m), sk.get(), "base");
	mParam.setFrameId(min, max);
	return mParam.getParameter(type);
}


int mcu_panim_cmd_func( srArgBuffer& args, SmartBody::SBScene& scene )
{
	std::string operation = args.read_token();
	if (operation == "enable")
	{
		SmartBody::util::log("Command: 'panim enable' has been deprecated.");
		return CMD_FAILURE;
	}
	if (operation == "state")
	{
		std::string blendName = args.read_token();
		std::string nextString = args.read_token();
		if (nextString == "cycle")
		{
			PABlend newState(blendName);
			std::string cycle = args.read_token();
			if (cycle == "true")
				newState.cycle = true;
			if (cycle == "false")
				newState.cycle = false;
			int numMotions = args.read_int();
			for (int i = 0; i < numMotions; i++)
			{
				std::string motionName = args.read_token();
				SmartBody::SBMotion* motion = scene.getAssetManager()->getMotion(motionName);

				if (!motion)
					return CMD_FAILURE;
				newState.motions.emplace_back(motion);
			}
			int numKeys = args.read_int();
			for (int i = 0; i < numMotions; i++)
			{
				std::vector<double> keysForOneMotion;
				if (numKeys == 0)
				{
					keysForOneMotion.emplace_back(0.0);
					keysForOneMotion.emplace_back(newState.motions[i]->duration());
				}
				else
				{
					for (int j = 0; j < numKeys; j++)
					{
						double key = args.read_double();
						keysForOneMotion.emplace_back(key);
					}
					for (int j = 0; j < numKeys - 1; j++)
					{
						if (keysForOneMotion[j] > keysForOneMotion[j + 1])
							keysForOneMotion[j + 1] += newState.motions[i]->duration();
					}
				}
				newState.keys.emplace_back(keysForOneMotion);
			}
			//commandContext.scene.getBlendManager()->addBlend(newState);
		}
		else if (nextString == "parameter")
		{
			SmartBody::SBAnimationBlend* blend = scene.getBlendManager()->getBlend(blendName);
			if (!blend) return CMD_FAILURE;
			std::string type = args.read_token();
			if (type == "1D") blend->setType(0);
			else if (type == "2D") blend->setType(1);
			else if (type == "3D") blend->setType(2);
			else return CMD_FAILURE;
			int num = args.read_int();
			for (int i = 0; i < num; i++)
			{
				std::string m = args.read_token();
				if (type == "1D")
				{
					std::string parameter = args.read_token();
					int motionId = blend->getMotionId(m);
					if (motionId < 0) return CMD_FAILURE;
					double param = parseMotionParameters(*scene.getAssetManager(), m, parameter, blend->keys[motionId][0], blend->keys[motionId][blend->getNumKeys() - 1]);
					if (param < -9000) param = atof(parameter.c_str());
					blend->setParameter(m, param);
				}
				else if (type == "2D")
				{
					std::string parameterX = args.read_token();
					std::string parameterY = args.read_token();
					double paramX = parseMotionParameters(*scene.getAssetManager(), m, parameterX, blend->keys[blend->getMotionId(m)][0], blend->keys[blend->getMotionId(m)][blend->getNumKeys() - 1]);
					double paramY = parseMotionParameters(*scene.getAssetManager(), m, parameterY, blend->keys[blend->getMotionId(m)][0], blend->keys[blend->getMotionId(m)][blend->getNumKeys() - 1]);
					if (paramX < -9000) paramX = atof(parameterX.c_str());
					if (paramY < -9000) paramY = atof(parameterY.c_str());
					blend->setParameter(m, paramX, paramY);
				}
				else if (type == "3D")
				{
					double param[3];
					for (double & pc : param)
					{
						std::string para = args.read_token();
						pc = parseMotionParameters(*scene.getAssetManager(), m, para, blend->keys[blend->getMotionId(m)][0], blend->keys[blend->getMotionId(m)][blend->getNumKeys() - 1]);
						if (pc < -9000) pc = atof(para.c_str());
					}
					blend->setParameter(m, param[0], param[1], param[2]);
				}
			}
//				if (type == "3D")
//					state->buildTetrahedron();
		}
		else if (nextString == "triangle")
		{
			SmartBody::SBAnimationBlend* blend = scene.getBlendManager()->getBlend(blendName);
			if (!blend) return CMD_FAILURE;
			int numTriangles = args.read_int();
			for (int i = 0; i < numTriangles; i++)
			{
				std::string motion1 = args.read_token();
				std::string motion2 = args.read_token();
				std::string motion3 = args.read_token();
				blend->addTriangle(motion1, motion2, motion3);
			}
		}
		else if (nextString == "tetrahedron")
		{ 
			SmartBody::SBAnimationBlend* blend = scene.getBlendManager()->getBlend(blendName);
			if (!blend) return CMD_FAILURE;
			int numTetrahedrons = args.read_int();
			for (int i = 0; i < numTetrahedrons; i++)
			{
				std::string motion1 = args.read_token();
				std::string motion2 = args.read_token();
				std::string motion3 = args.read_token();
				std::string motion4 = args.read_token();
				blend->addTetrahedron(motion1, motion2, motion3, motion4);
			}				
		}
		else
			return CMD_FAILURE;
	}
	else if (operation == "schedule" || operation == "unschedule" || operation == "update" ||  operation == "updatestate" || operation == "basename")
	{
		std::string charString = args.read_token();
		if (charString != "char")
			return CMD_FAILURE;
		SmartBody::SBCharacter* character = scene.getCharacter(args.read_token());
		if (!character)
			return CMD_FAILURE;
		if (!character->param_animation_ct)
		{
			SmartBody::util::log("Parameterized Animation Not Enabled!");
			return CMD_SUCCESS;
		}

		if (operation == "schedule")
		{
			std::string blendString = args.read_token();
			if (blendString != "state")
				return CMD_FAILURE;
			std::string blendName = args.read_token();
			SmartBody::SBAnimationBlend* blend = scene.getBlendManager()->getBlend(blendName);
			if (!blend)
				SmartBody::util::log("Blend %s not exist, schedule Idle blend.", blendName.c_str());
			std::string loopString = args.read_token();
			if (loopString != "loop")
				return CMD_FAILURE;
			std::string loop = args.read_token();
			PABlendData::WrapMode wrap = PABlendData::Loop;
			if (loop == "true") wrap = PABlendData::Loop;
			if (loop == "false") wrap = PABlendData::Once;
			std::string playNowString = args.read_token();
			if (playNowString != "playnow")
				return CMD_FAILURE;
			PABlendData::ScheduleMode schedule = PABlendData::Queued;
			std::string playNow = args.read_token();
			if (playNow == "true") schedule = PABlendData::Now;
			else if (playNow == "false") schedule = PABlendData::Queued;
			else 
				return CMD_FAILURE;
			PABlendData::BlendMode blendMode = PABlendData::Overwrite;
			std::string additiveString = args.read_token();
			if (additiveString != "additive")
				return CMD_FAILURE;
			std::string addtive = args.read_token();
			if (addtive == "true") blendMode = PABlendData::Additive;
			else if (addtive == "false") blendMode = PABlendData::Overwrite;
			else
				return CMD_FAILURE;
			std::string jointString = args.read_token();
			if (jointString != "joint")
				return CMD_FAILURE;
			std::string joint = args.read_token();


			std::string directPlayStr = args.peek_string();			
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(directPlayStr, tokens);
			bool directPlay = false;
			if (!tokens.empty() && tokens[0] == "direct-play")
			{
				directPlayStr = args.read_token();
				std::string dplay = args.read_token();
				if (dplay == "true") 
					directPlay = true;
				else if (dplay == "false")
					directPlay = false;
			}
			

			std::vector<double> weights;			
			int numWeights = args.calc_num_tokens();
			if (numWeights > 0)
			{
				if (blend)
				{
					for (int i = 0; i < blend->getNumMotions(); i++)
						weights.emplace_back(args.read_double());
				}
			}
			if (blend && numWeights < blend->getNumMotions())
			{
				character->param_animation_ct->schedule(blend, 0, 0, 0, wrap, schedule, blendMode, joint, 0.0, 0.0, 0.0, -1.0, directPlay) ;
			}
			else
			{
				character->param_animation_ct->schedule(blend, weights, wrap, schedule, blendMode, joint, 0.0, 0.0, 0.0, -1.0, directPlay);
			}
		}

		if (operation == "unschedule")
			character->param_animation_ct->unschedule();

		if (operation == "update")
		{
			std::vector<double> w;
			if (args.calc_num_tokens() == character->param_animation_ct->getNumWeights())
			{
				for (int i = 0; i < character->param_animation_ct->getNumWeights(); i++)
					w.emplace_back(args.read_double());
				//SmartBody::util::log("weight size = %d",w.size());
				character->param_animation_ct->updateWeights(w);
			}
		}
		if (operation == "updatestate")
		{
			if (!character->param_animation_ct->getCurrentPABlendData())
			{
				SmartBody::util::log("No state available for character %s.", character->getName().c_str());
				return CMD_FAILURE;
			}

			int type = character->param_animation_ct->getCurrentPABlendData()->state->getType();
			if (args.calc_num_tokens() != type + 1)
			{
				SmartBody::util::log("Cannot update state %s for character %s which has %d parameters, you sent %d.", character->param_animation_ct->getName().c_str(), character->getName().c_str(), (type + 1), args.calc_num_tokens());
				return CMD_FAILURE;
			}
			std::vector<double> p;
			p.reserve((type + 1));
			for (int i = 0; i < (type + 1); i++)
				p.emplace_back(args.read_double());
			std::vector<double> weights;
			weights.resize(character->param_animation_ct->getCurrentPABlendData()->state->getNumMotions());
			if (type == 0)
				character->param_animation_ct->getCurrentPABlendData()->state->getWeightsFromParameters(p[0], weights);
			else if (type == 1)
				character->param_animation_ct->getCurrentPABlendData()->state->getWeightsFromParameters(p[0], p[1], weights);
			else if (type == 2)
				character->param_animation_ct->getCurrentPABlendData()->state->getWeightsFromParameters(p[0], p[1], p[2], weights);
			character->param_animation_ct->updateWeights(weights);
			return CMD_SUCCESS;
		}


		if (operation == "basename")
			character->param_animation_ct->setBaseJointName(args.read_token());
	}
		
	else
	{
		SmartBody::util::log("mcu_panim_cmd_func ERR: operation not valid.");
		return CMD_FAILURE;
	}
	return CMD_SUCCESS;
}

int mcu_motion_player_func(srArgBuffer& args, SmartBody::SBScene& scene )
{
	std::string charName = args.read_token();
	SmartBody::SBCharacter* character = scene.getCharacter(charName);
	if (character)
	{
		if (!character->motionplayer_ct)
			return CMD_FAILURE;

		std::string next = args.read_token();
		if (next == "on")
			character->motionplayer_ct->setActive(true);
		else if (next == "off")
			character->motionplayer_ct->setActive(false);
		else
		{			
			double frameNumber = args.read_double();
			//SmartBody::util::log("before motionplayer_ct->init(), next = %s, frameNumber = %f", next.c_str(), frameNumber);
			character->motionplayer_ct->init(next, frameNumber);
			//SmartBody::util::log("after motionplayer_ct->init()");
		}
		return CMD_SUCCESS;
	}
	return CMD_FAILURE;
}

/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////

int mcu_terrain_func( srArgBuffer& args, SmartBody::SBScene& scene )	{
	
	
	
	Heightfield* heightfield = scene.getHeightfield();
	char *terr_cmd = args.read_token();

	if( strcmp( terr_cmd, "help" ) == 0 )	{
		SmartBody::util::log("HELP: terrain");	
		SmartBody::util::log("  load [<filename>]");
		SmartBody::util::log("  scale <X Y Z>");
		SmartBody::util::log("  origin <X Y Z>|auto");
		SmartBody::util::log("  delete");
		return( CMD_SUCCESS );
	}
	else
	if( strcmp( terr_cmd, "load" ) == 0 )	{

		Heightfield* heightfield = scene.createHeightfield();
		int n = args.calc_num_tokens();
		if( n == 0 )	{
			heightfield->load( (char*)"../../../../data/terrain/range1.e.ppm" );
			heightfield->set_scale( 5000.0f * scene.getScale() / 100.0f, 300.0f * scene.getScale() / 100.0f, 5000.0f  * scene.getScale() / 100.0f);
			heightfield->set_auto_origin();
		}
		else	{
			char *filename = args.read_token();
			heightfield->load( filename );
		}
		return( CMD_SUCCESS );
	}
	else
	if( heightfield == nullptr ) {
		SmartBody::util::log( "mcu_terrain_func: ERR: no heightfield loaded" );
		return( CMD_FAILURE );
	}

	if( strcmp( terr_cmd, "scale" ) == 0 )	{
			
		float x = args.read_float();
		float y = args.read_float();
		float z = args.read_float();
		heightfield->set_scale( x, y, z );
	}
	else
	if( strcmp( terr_cmd, "origin" ) == 0 )	{

		int n = args.calc_num_tokens();
		if( n == 1 )	{
			char *sub_cmd = args.read_token();
			if( strcmp( sub_cmd, "auto" ) == 0 )	{
				heightfield->set_auto_origin();
			}
			else	{
				SmartBody::util::log( "mcu_terrain_func: ERR: token '%s' not recognized", sub_cmd );
				return( CMD_NOT_FOUND );
			}
		}
		else	{
				
			float x = args.read_float();
			float y = args.read_float();
			float z = args.read_float();
			heightfield->set_origin( x, y, z );
		}
	}
	else
	if( strcmp( terr_cmd, "delete" ) == 0 )
	{
		scene.removeHeightfield();
	}
	else {
		return( CMD_NOT_FOUND );
	}
	return( CMD_SUCCESS );
	
}

/////////////////////////////////////////////////////////////

void mcu_print_timer_deprecation_warning( )	{

	SmartBody::util::log("WARNING: fps/lockdt feature is deprecated");
	SmartBody::util::log("  - If you insist, be sure to set fps first, then lockdt...");
	SmartBody::util::log("  - Use 'time sleepfps <fps>' and 'time simfps <fps>' instead");
}

void mcu_print_timer_help( int level = 0 )	{
	
	if( level == 2 ) {
		SmartBody::util::log("API help...");
	
	}
	else
	if( level == 1 )	{
		SmartBody::util::log("Full help...");
	}
	else	{
		SmartBody::util::log("HELP: time");	
		SmartBody::util::log("  help | fullhelp");
		SmartBody::util::log("  maxfps | fps <desired-max-fps>	(DEPRECATED)");
		SmartBody::util::log("  lockdt [0|1]					(DEPRECATED)");
		SmartBody::util::log("  perf [0|1 [<interval>]]");
		SmartBody::util::log("  speed <real-time-factor>");
		SmartBody::util::log("  sleepfps | simfps | evalfps <fps>");
		SmartBody::util::log("  sleepdt | simdt | evaldt <dt>");
		SmartBody::util::log("  pause | resume");
		SmartBody::util::log("  step [num-steps]");
		SmartBody::util::log("  perf [<interval>]");
		SmartBody::util::log("  print");
	}
}

int mcu_time_func( srArgBuffer& args, SmartBody::SBScene& scene )	{
	
	char *time_cmd = args.read_token();

	if( strcmp( time_cmd, "help" ) == 0 )	{
		mcu_print_timer_help();
		return( CMD_SUCCESS );
	}
	else
	if( strcmp( time_cmd, "fullhelp" ) == 0 )	{
		mcu_print_timer_help( 1 );
		return( CMD_SUCCESS );
	}
	else
	if( strcmp( time_cmd, "api" ) == 0 )	{
		mcu_print_timer_help( 2 );
		return( CMD_SUCCESS );
	}
	else
	if( strcmp( time_cmd, "test" ) == 0 )	{
//			void test_time_regulator();
//			test_time_regulator();
		return( CMD_SUCCESS );
	}
	else
	if( strcmp( time_cmd, "print" ) == 0 )
	{
		{
			scene.getSimulationManager()->printInfo();
		}
		{
			SmartBody::util::log( "TIME:%.3f ~ DT:%.3f %.2f:FPS\n",
				scene.getSimulationManager()->getTime(),
				scene.getSimulationManager()->getTimeDt(),
				1.0 / scene.getSimulationManager()->getTimeDt()
		);
		}
		return( CMD_SUCCESS );
	}

//		if( mcu.timer_p == nullptr )	{
//		SmartBody::util::log( "mcu_time_func NOTICE: %s: TimeRegulator was NOT REGISTERED\n", time_cmd );
//		scene.getSimulationManager()->switch_internal_timer();
//		}
		
	if( strcmp( time_cmd, "reset" ) == 0 ) {
		scene.getSimulationManager()->reset();
	}
	else 
	if( ( strcmp( time_cmd, "maxfps" ) == 0 ) || ( strcmp( time_cmd, "fps" ) == 0 ) )	{ // deprecate
		mcu_print_timer_deprecation_warning();
		scene.getSimulationManager()->setSleepFps( args.read_float() );
	}
	else
	if( strcmp( time_cmd, "lockdt" ) == 0 )	{ // deprecate
		mcu_print_timer_deprecation_warning();
		int enable = true;
		int n = args.calc_num_tokens();
		if( n ) enable = args.read_int();
		scene.getSimulationManager()->setSleepLock();
	}
	else 
	if( strcmp( time_cmd, "speed" ) == 0 ) {
		scene.getSimulationManager()->setSpeed( args.read_float() );
	}
	else 
	if( strcmp( time_cmd, "sleepfps" ) == 0 ) {
		scene.getSimulationManager()->setSleepFps( args.read_float() );
	}
	else 
	if( strcmp( time_cmd, "evalfps" ) == 0 ) {
		scene.getSimulationManager()->setEvalFps( args.read_float() );
	}
	else
	if( strcmp( time_cmd, "simfps" ) == 0 ) {
		scene.getSimulationManager()->setSimFps( args.read_float() );
	}
	else 
	if( strcmp( time_cmd, "sleepdt" ) == 0 ) {
		scene.getSimulationManager()->setSleepDt( args.read_float() );
	}
	else 
	if( strcmp( time_cmd, "evaldt" ) == 0 ) {
		scene.getSimulationManager()->setEvalDt( args.read_float() );
	}
	else 
	if( strcmp( time_cmd, "simdt" ) == 0 ) {
		scene.getSimulationManager()->setSimDt( args.read_float() );
	}
	else
	if( strcmp( time_cmd, "pause" ) == 0 )	{
		scene.getSimulationManager()->pause();
	}
	else 
	if( strcmp( time_cmd, "resume" ) == 0 )	{
		scene.getSimulationManager()->resume();
	}
	else 
	if( strcmp( time_cmd, "step" ) == 0 )	{
		int n = args.calc_num_tokens();
		if( n ) {
			scene.getSimulationManager()->step( args.read_int() );
		}
		else	{
			scene.getSimulationManager()->step( 1 );
		}
	}
	else 
	if( strcmp( time_cmd, "perf" ) == 0 )	{
		int n = args.calc_num_tokens();
		if( n ) {
			scene.getSimulationManager()->set_perf( args.read_float() );
		}
		else	{
			scene.getSimulationManager()->set_perf( 10.0 );
		}
	}
	else {
		return( CMD_NOT_FOUND );
	}
	return( CMD_SUCCESS );
}

/////////////////////////////////////////////////////////////

void mcu_print_profiler_help( int level = 0 )	{
	
	if( level == 2 ) {
		SmartBody::util::log("API help...");
	}
	else
	if( level == 1 )	{
		SmartBody::util::log("Full help...");
	}
	else	{
		SmartBody::util::log( "HELP: tip" );	
		SmartBody::util::log( "  help | fullhelp | legend" );
		SmartBody::util::log( "  on | off | enable | disable | preload" );
		SmartBody::util::log( "  group <name> enable | disable | preload" );
		SmartBody::util::log( "  suppress | select [<level>]" );
		SmartBody::util::log( "  abs <delta> | 0" );
		SmartBody::util::log( "  rel <factor> | 0" );
		SmartBody::util::log( "  fix [abs|rel]" );
		SmartBody::util::log( "  dyn [abs|rel]" );
		SmartBody::util::log( "  sniff <factor:[0.0,1.0)>" );
		SmartBody::util::log( "  avoid <factor:(1.0,...)>" );
		SmartBody::util::log( "  decaying <factor:[0.0,1.0)>" );
		SmartBody::util::log( "  rolling <count>" );
		SmartBody::util::log( "  print | report" );
		SmartBody::util::log( "  erase | reset" );
		SmartBody::util::log( "  test [reps]" );
	}
}

int mcu_time_ival_prof_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
/*	
	if( mcu_p )	{
		char *tip_cmd = args.read_token();

		if( strcmp( tip_cmd, "help" ) == 0 )	{
			mcu_print_profiler_help();
			return( CMD_SUCCESS );
		}
		else
		if( strcmp( tip_cmd, "fullhelp" ) == 0 )	{
			mcu_print_profiler_help( 1 );
			return( CMD_SUCCESS );
		}
		else
		if( strcmp( tip_cmd, "api" ) == 0 )	{
			mcu_print_profiler_help( 2 );
			return( CMD_SUCCESS );
		}

		if( mcu.profiler_p == nullptr )	{
			SmartBody::util::log( "mcu_time_ival_prof_func NOTICE: %s: TimeIntervalProfiler was NOT REGISTERED\n", tip_cmd ); 
			mcu.switch_internal_profiler();
		}
		TimeIntervalProfiler *prof_p = mcu.profiler_p;

		if( strcmp( tip_cmd, "legend" ) == 0 ) {
			prof_p->print_legend();
		}
		else
		if( strcmp( tip_cmd, "print" ) == 0 )	{
			prof_p->print();
		}
		else
		if( strcmp( tip_cmd, "report" ) == 0 )	{
			prof_p->report();
//			prof_p->full_report();
		}
		else
		if( strcmp( tip_cmd, "erase" ) == 0 )	{
			prof_p->erase();
		}
		else
		if( strcmp( tip_cmd, "reset" ) == 0 )	{
			prof_p->reset();
		}
		else 
		if( strcmp( tip_cmd, "on" ) == 0 )	{
			prof_p->bypass( false );
		}
		else
		if( strcmp( tip_cmd, "off" ) == 0 )	{
			prof_p->bypass( true );
		}
		else 
		if( strcmp( tip_cmd, "enable" ) == 0 )	{
			prof_p->enable( true );
		}
		else
		if( strcmp( tip_cmd, "disable" ) == 0 )  {
			prof_p->enable( false );
		}
		else
		if( strcmp( tip_cmd, "preload" ) == 0 )	{
			prof_p->preload();
		}
		else
		if( strcmp( tip_cmd, "group" ) == 0 )	{
			char *group_name = args.read_token();
			char *group_cmd = args.read_token();
			if( strcmp( group_cmd, "enable" ) == 0 )	{
				prof_p->enable( group_name, true );
			}
			else
			if( strcmp( group_cmd, "disable" ) == 0 )	{
				prof_p->enable( group_name, false );
			}
			else
			if( strcmp( group_cmd, "preload" ) == 0 )	{
				prof_p->preload( group_name );
			}
		}
		else
		if( strcmp( tip_cmd, "suppress" ) == 0 )	{
			int level = -1;
			int n = args.calc_num_tokens();
			if( n ) level = args.read_int();
			prof_p->set_suppression( level );
		}
		else
		if( strcmp( tip_cmd, "select" ) == 0 )	{
			int level = -1;
			int n = args.calc_num_tokens();
			if( n ) level = args.read_int();
			prof_p->set_selection( level );
		}
		else
		if( strcmp( tip_cmd, "abs" ) == 0 )	{
			float delta = args.read_float();
			prof_p->set_abs_threshold( (double)delta );
		}
		else
		if( strcmp( tip_cmd, "rel" ) == 0 )	{
			float factor = args.read_float();
			prof_p->set_rel_threshold( (double)factor );
		}
		else
		if( strcmp( tip_cmd, "fix" ) == 0 )	{
			int n = args.calc_num_tokens();
			if( n ) {
				char *opt = args.read_token();
				if( strcmp( opt, "abs" ) == 0 )	{
					prof_p->set_dynamic_abs( false );
				}
				else
				if( strcmp( opt, "rel" ) == 0 )	{
					prof_p->set_dynamic_rel( false );
				}
				else	{
					SmartBody::util::log("ERROR: Unknown command option '%s'", opt );
					return( CMD_NOT_FOUND );
				}
			}
			else	{
				prof_p->set_dynamic_abs( false );
				prof_p->set_dynamic_rel( false );
			}
		}
		else
		if( strcmp( tip_cmd, "dyn" ) == 0 )	{
			int n = args.calc_num_tokens();
			if( n ) {
				char *opt = args.read_token();
				if( strcmp( opt, "abs" ) == 0 )	{
					prof_p->set_dynamic_abs( true );
				}
				else
				if( strcmp( opt, "rel" ) == 0 )	{
					prof_p->set_dynamic_rel( true );
				}
				else	{
					SmartBody::util::log("ERROR: Unknown command option '%s'", opt );
					return( CMD_NOT_FOUND );
				}
			}
			else	{
				prof_p->set_dynamic_abs( true );
				prof_p->set_dynamic_rel( true );
			}
		}
		else
		if( strcmp( tip_cmd, "sniff" ) == 0 )	{
			float value = args.read_float();
			prof_p->set_sniff( (double)value );
		}
		else
		if( strcmp( tip_cmd, "avoid" ) == 0 )	{
			float value = args.read_float();
			prof_p->set_avoid( (double)value );
		}
		else
		if( strcmp( tip_cmd, "decaying" ) == 0 )	{
			float sm = args.read_float();
			prof_p->set_decaying( (double)sm );
		}
		else
		if( strcmp( tip_cmd, "rolling" ) == 0 )	{
			int c = args.read_int();
			prof_p->set_rolling( c );
		}
		else
		if( strcmp( tip_cmd, "test" ) == 0 )	{
			int reps = args.read_int();
			prof_p->test_clock( reps );
		}
		else
		if( strcmp( tip_cmd, "test2" ) == 0 )	{
			int reps = args.read_int();
			prof_p->test_clock( reps );
		}
		else {
			return( CMD_NOT_FOUND );
		}
		return( CMD_SUCCESS );
	}
	*/
	return( CMD_FAILURE );
}



int set_controller_timing(
	MeController* ctrl_p, 
	float indt, 
	float outdt, 
	float empht
)	{
	ctrl_p->inoutdt( indt, outdt );
	ctrl_p->emphasist( empht );
	return( CMD_SUCCESS );
}

int query_controller(
	MeController* ctrl_p
)	{
	SmartBody::util::log( "MCU QUERY: MeController '%s':\n", ctrl_p->getName().c_str());
	SmartBody::util::log( "  type... %s\n", ctrl_p->controller_type().c_str() );
	SmartBody::util::log( "  indt... %.3f\n", ctrl_p->indt() );
	SmartBody::util::log( "  outdt.. %.3f\n", ctrl_p->outdt() );
	float emph = ctrl_p->emphasist();
	if( emph < 0.0 )	{
		SmartBody::util::log( "  emph... UNK\n");
	}
	else	{
		SmartBody::util::log( "  emph... %.3f\n", emph );
	}
	double dur = ctrl_p->controller_duration();
	if( dur < 0.0 )	{
		SmartBody::util::log( "  dur.... UNK\n" );
	}
	else	{
		SmartBody::util::log( "  dur.... %.3f\n", dur );
	}
//	SmartBody::util::log( "> " );
	return( CMD_SUCCESS );
}

/*

	ctrl <> handle <handle-name>
	ctrl <> query
	ctrl <> timing <ease-in> <ease-out> [<emph>]
	ctrl <> record motion <operation_type> [max <num-frames>]
	ctrl <> record bvh <operation_type> [max <num-frames>]
#	ctrl <> record pose <full-file-prefix>

	ctrl <> debugger on	
	ctrl <> debugger off

#	ctrl <> interp <dur> <ctrl-a> <ctrl-b> linear|sine
#	ctrl <> interp <dur> <ctrl-a> <ctrl-b> curve <curve-name>
#	ctrl <> point <x y z>

	ctrl <> gaze [<key-fr> <key-to>]

	ctrl <> motion <motion-file>
	ctrl <> snod <char-name>

	ctrl <> quickdraw <quickdraw-motion-name> [<reholster-motion>]

	ctrl <> lifecycle <child-controller-name>

#	ctrl <> sched <char-name> [<skel-subset>]
	ctrl <> sched <char-name>

*/

int mcu_controller_func( srArgBuffer& args, SmartBody::SBScene& scene )	{
	
	 
	char *ctrl_name = args.read_token();
	char *ctrl_cmd = args.read_token();
	
	if( strcmp(ctrl_cmd, "passthrough" ) == 0 || strcmp(ctrl_cmd, "enable" ) == 0)
	{
		bool reverseMeaning = false;
		if (strcmp(ctrl_cmd, "passthrough" ) == 0)
			reverseMeaning = true;

		bool allControllers = false;
		if (strcmp(ctrl_name, "*") == 0)
			allControllers = true;

		char* value = args.read_token();
		bool passThroughValue = true;
		bool toggleValue = false;
		bool checkStatus = false;
		if (strlen(value) > 0)
		{
			if (strcmp("true", value) == 0)
			{
				passThroughValue = true;
			}
			else if (strcmp("false", value) == 0)
			{
				passThroughValue = false;
			}
			else if (strcmp("status", value) == 0)
			{
				checkStatus = true;
			}
			else
			{
				SmartBody::util::log("Use syntax: ctrl %s enable <true|false|status>, or ctrl %s enable ", ctrl_name, ctrl_name);
				return CMD_FAILURE;
			}
		}
		else
		{
			toggleValue = true;
		}

		if (reverseMeaning)
			passThroughValue = !passThroughValue;

		
		int numControllersAffected = 0;
		const std::vector<std::string>& characterNames = scene.getCharacterNames();
		for (const auto & characterName : characterNames)
		{
			SmartBody::SBCharacter* character = scene.getCharacter(characterName);
			auto& controllerTree = character->ct_tree_p;
			int numControllers = controllerTree->count_controllers();
			
			for (int c = 0; c < numControllers; c++)
			{
				if (checkStatus)
				{
					std::string passThroughStr = (controllerTree->controller(c)->isEnabled())? " true " : " false";							
					SmartBody::util::log("[%s] %s = %s", character->getName().c_str(), controllerTree->controller(c)->getName().c_str(), passThroughStr.c_str());
					numControllersAffected = numControllers;	// just so it won't generate error
				}
				else if (allControllers)
				{
					if (toggleValue)
						controllerTree->controller(c)->setEnable(!controllerTree->controller(c)->isEnabled());
					else
						controllerTree->controller(c)->setEnable(passThroughValue);
					numControllersAffected++;
				}
				else if (controllerTree->controller(c)->getName() == ctrl_name)
				{
					if (toggleValue)
						controllerTree->controller(c)->setEnable(!controllerTree->controller(c)->isEnabled());
					else
						controllerTree->controller(c)->setEnable(passThroughValue);
					numControllersAffected++;
				}
			}
		}
		if (numControllersAffected > 0)
			return CMD_SUCCESS;
		else
			return CMD_FAILURE;
	}
	else
	{
		// Non-initializing controllers need an actual instance
		MeController* ctrl_p = nullptr;// removed lookup_ctrl function 1/26/13 mcu.lookup_ctrl( string( ctrl_name ), "ERROR: ctrl <controller name>: " );
		if( ctrl_p==nullptr ) {
			// should have printed error from above function
			return CMD_FAILURE;
		}
		//example:	ctrl ~doctor/motion_sched record skm(or bvh) start [max 500]
		//			ctrl ~doctor/motion_sched record skm(or bvh) write file_prefix
		//			ctrl ~doctor/motion_sched record skm(or bvh) stop

		/*
		if( strcmp( ctrl_cmd, "record" ) == 0 )
		{
			char *record_type = args.read_token();
			char *operation = args.read_token();
			int max_num_of_frames = 0;
			if( strcmp( operation, "start" ) == 0 )
			{
				char *optional = args.read_token();
				if( strcmp( optional, "max" ) == 0 )
					max_num_of_frames = args.read_int();
				if(max_num_of_frames<0)	max_num_of_frames = 0;
					
				if( strcmp( record_type, "skm" ) == 0 )	{
					ctrl_p->record_motion( max_num_of_frames );
					return( CMD_SUCCESS );
				}
				if( strcmp( record_type, "bvh" ) == 0 )	{
					if( mcu.timer_p )	{
						double sim_dt = mcu.timer_p->get_sim_dt();
						if( sim_dt > 0.0 )	{
							ctrl_p->record_bvh( max_num_of_frames, sim_dt );
							return( CMD_SUCCESS );
						}
						else	{
							SmartBody::util::log( "mcu_controller_func ERR: BVH recording requires 'time simfps|simdt' set\n" );
							return( CMD_FAILURE );
						}
					}
					else	{
						SmartBody::util::log( "mcu_controller_func ERR: BVH recording requires registered TimeRegulator\n" );
						return( CMD_FAILURE );
					}
				}
			}
			if( strcmp( operation, "write" ) == 0 )	{
				char *full_prefix = args.read_token();
				ctrl_p->record_write(full_prefix);
				return( CMD_SUCCESS );
			}
			if( strcmp( operation, "stop" ) == 0 )	{
				ctrl_p->record_stop();
				return( CMD_SUCCESS );
			}
			if( strcmp( operation, "clear" ) == 0 )	{
				ctrl_p->record_clear();
				return( CMD_SUCCESS );
			}
		}
		*/
		else
		if( strcmp( ctrl_cmd, "debugger" ) == 0 )	{
			char *operation = args.read_token();
			if( strcmp( operation, "on" ) == 0 ) {
				ctrl_p->record_buffer_changes(true);
				return( CMD_SUCCESS );
			} else if (strcmp( operation, "off" ) == 0 ) {
				ctrl_p->record_buffer_changes(false);
				return( CMD_SUCCESS );
			}
		}
		else
		if( strcmp( ctrl_cmd, "timing" ) == 0 )	{
			int n = args.calc_num_tokens();
			if( n )	{
				float indt = args.read_float();
				float outdt = args.read_float();
				float empht = -1.0;
				if( n > 2 )	{
					empht = args.read_float();
				}
				return(
					set_controller_timing( ctrl_p, indt, outdt, empht )
				);
			}
			return( CMD_FAILURE );
		}
		else
		if( strcmp( ctrl_cmd, "query" ) == 0 )	{
			return(
				query_controller( ctrl_p )
			);
		}
		else
		if( strcmp( ctrl_cmd, "handle" ) == 0 )	{
			char *handle = args.read_token();
			ctrl_p->handle( handle );
			return( CMD_SUCCESS );
		}
		else
		return( CMD_NOT_FOUND );
	}
	return( CMD_FAILURE );
}

/////////////////////////////////////////////////////////////


int mcu_gaze_limit_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	int n = args.calc_num_tokens();
	int keyIndex = -1;
	if (n > 0)
	{
		char *key_label = args.read_token();
		keyIndex = MeCtGaze::key_index( key_label );
		if (keyIndex == -1)
		{
				SmartBody::util::log("%s is not a valid key.", key_label);
				return CMD_FAILURE;
		}
	}
		
	if( n > 3 )
	{
		float p_up = args.read_float();
		float p_dn = args.read_float();
		float h = args.read_float();
		float r = args.read_float();
			
		MeCtGaze::set_all_limits(keyIndex, p_up, p_dn, h, r );
	}
	else if (n == 1 || n == 0)
	{
		SmartBody::util::log("To set limits, use: gazelimit <lumbar|thorax|cervical|cranial|optical|back|chest|neck|head|eyes> pitchup pitchdown heading roll");
		int start = 0; 
		int finish = MeCtGaze::GAZE_KEY_EYES;
		if (n == 1)
			start = finish = keyIndex;
		for (int x = start; x <= finish; x++)
		{
			char* label = MeCtGaze::key_label(x);
			// show the limits for that particular key
			SmartBody::util::log("%s Pitch up  : %f ", label, MeCtGaze::DEFAULT_LIMIT_PITCH_UP[x]);
			SmartBody::util::log("%s Pitch down: %f", label, MeCtGaze::DEFAULT_LIMIT_PITCH_DOWN[x]);
			SmartBody::util::log("%s Heading   : %f", label, MeCtGaze::DEFAULT_LIMIT_HEADING[x]);
			SmartBody::util::log("%s Roll      : %f", label, MeCtGaze::DEFAULT_LIMIT_ROLL[x]);
		}
	}
	return( CMD_SUCCESS );
}


/////////////////////////////////////////////////////////////



/*

   net reset function -
   helpful in resyncing TCP socket connection to unreal if it breaks
	<EMF>
*/


int mcu_net_reset( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager ) {
	bool ret = CMD_SUCCESS;
	bonebusManager.getBoneBus().CloseConnection();
	bonebusManager.setEnable(true);
	bonebusManager.getBoneBus().UpdateAllCharacters();
	return (CMD_SUCCESS);
}

int mcu_net_check( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager ) {

	if (!bonebusManager.getBoneBus().IsOpen())
	{
		return mcu_net_reset(args, bonebusManager);
	}
	else
	{
		return CMD_SUCCESS;
	}
}
/*

   net boneupdates <0|1>
   net worldoffsetupdates <0|1>

*/

int mcu_net_func( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager ) {


    char * command = args.read_token();

    if ( _stricmp( command, "boneupdates" ) == 0 )
    {
        // turns on/off sending character bone information across the network
        // global setting that affects all characters and bones

        int enable = args.read_int();
        bonebusManager.setEnable(enable != 0);
        return CMD_SUCCESS;
    }
    else if ( _stricmp( command, "worldoffsetupdates" ) == 0 )
    {
        // turns on/off sending character world offset information across the network
        // global setting that affects all characters

        int enable = args.read_int();
		bonebusManager.setEnable(enable != 0);
        return CMD_SUCCESS;
    }

    return CMD_NOT_FOUND;
  
}


/*

   PlaySound <sound_file> <character_id>  // if sound_file starts with '<drive>:' - uses absolute path
                                          // if not - uses relative path, prepends absolute path of top-level saso directory to string
                                          // character_id can be used to position the sound where a character is in the world
*/

int mcu_play_sound_func( srArgBuffer& args, CommandContext commandContext )
{
    char * remainder = args.read_remainder_raw();
    string sArgs = remainder;
    vector< string > splitArgs;
	SmartBody::util::tokenize( sArgs, splitArgs );

    if ( splitArgs.size() >= 2 )
    {
        string soundFile = splitArgs[ 0 ];
        string characterName = splitArgs[ 1 ];
		
		SmartBody::util::log("Sound to play = %s", soundFile.c_str());
        // check for double-quotes around sound file
        if ( soundFile.length() > 1 )
        {
			if ( soundFile[ 0 ] == '\"' )
			{
				size_t first = sArgs.find_first_of( "\"" );
				if ( first == string::npos )
				{
					SmartBody::util::log( "Error parsing PlaySound message ''", sArgs.c_str() );
					return CMD_FAILURE;
				}

				size_t second = sArgs.find_first_of( "\"", first + 1 );
				if ( second == string::npos )
				{
					SmartBody::util::log( "Error parsing PlaySound message ''", sArgs.c_str() );
					return CMD_FAILURE;
				}

				soundFile = sArgs.substr( first + 1, second - first - 1 );
				characterName = sArgs.substr( second + 2 );
			}
        }

        bool absolutePath = false;

        if ( soundFile.length() > 1 )
        {
			if ( soundFile[ 0 ] == '\\' ||
					soundFile[ 0 ] == '/' ||
					soundFile[ 1 ] == ':' )
			{
				absolutePath = true;
			}
        }

        if ( !absolutePath )
        {
			std::string soundCacheDir = "../../../../..";
			std::string soundDir = commandContext.scene.getStringAttribute("speechRelaySoundCacheDir");
			if (!soundDir.empty())
				soundCacheDir = soundDir;

			boost::filesystem::path p( soundCacheDir );
			boost::filesystem::path abs_p = boost::filesystem::absolute( p );

	//            char full[ _MAX_PATH ];
	//            if ( _fullpath( full, "..\\..\\..\\..\\..", _MAX_PATH ) != nullptr )
			if ( boost::filesystem::exists( abs_p ) )
			{
				//soundFile = string( full ) + string( "/" ) + soundFile;
				p  /= soundFile;
				if (boost::filesystem::exists(p))
					soundFile = abs_p.string() + string("/") + soundFile;
			}
        }

		// send the sound event
		std::stringstream strstr;
		strstr << soundFile << " " << characterName;
		SmartBody::SBCharacter* character = commandContext.scene.getCharacter(characterName);
		std::string characterObjectName = commandContext.scene.getStringFromObject(character);
		float soundDuration = 0.0f;

#ifndef SB_NO_VHCL_AUDIO
		// if internal audio is on, use the internal sound player
        if (commandContext.scene.getBoolAttribute("internalAudio"))
        {
          SmartBody::util::log("Play AudioFile = %s", soundFile.c_str() );
          soundDuration = AUDIO_Play( soundFile.c_str() );
		  if (soundDuration != 0.0f)
		  {		  
			std::stringstream strstr2;
			strstr2 << R"(sb scene.getEventManager().handleEventRemove(scene.getEventManager().createEvent("sound", ")" << strstr.str() << " stop\", \"" << characterObjectName << "\"))";
			commandContext.scene.commandAt(soundDuration, strstr2.str());
		  }
        }
        else
        {
			if (commandContext.boneBusManager) {
				commandContext.boneBusManager->getBoneBus().SendPlaySound(soundFile.c_str(), characterName.c_str());
			}
        }


#endif
		strstr << " start " << soundDuration;
		SmartBody::SBEvent* sbevent = commandContext.scene.getEventManager()->createEvent("sound", strstr.str(), characterObjectName);
		commandContext.scene.getEventManager()->handleEvent(sbevent);
		delete sbevent;
        return CMD_SUCCESS;
    }

    return CMD_NOT_FOUND;
}


/*

   StopSound <sound_file>  // if sound_file starts with '<drive>:' - uses absolute path
                           // if not - uses relative path, prepends absolute path of top-level saso directory to string

*/

int mcu_stop_sound_func( srArgBuffer& args, CommandContext commandContext )
{
    char * remainder = args.read_remainder_raw();
    string sArgs = remainder;
    vector< string > splitArgs;
	SmartBody::util::tokenize( sArgs, splitArgs );

    if ( splitArgs.size() >= 2 )
    {
        string soundFile = splitArgs[ 0 ];
        string characterName = splitArgs[ 1 ];

        // check for double-quotes around sound file
        if ( soundFile.length() > 1 )
        {
        if ( soundFile[ 0 ] == '\"' )
        {
            size_t first = sArgs.find_first_of( '\"' );
            if ( first == string::npos )
            {
                SmartBody::util::log( "Error parsing StopSound message ''", sArgs.c_str() );
                return CMD_FAILURE;
            }

            size_t second = sArgs.find_first_of( '\"', first + 1 );
            if ( second == string::npos )
            {
                SmartBody::util::log( "Error parsing StopSound message ''", sArgs.c_str() );
                return CMD_FAILURE;
            }

            soundFile = sArgs.substr( first + 1, second - first - 1 );
            characterName = sArgs.substr( second + 2 );
        }
        }

        bool absolutePath = false;

        if ( soundFile.length() > 1 )
        {
        if ( soundFile[ 0 ] == '\\' ||
                soundFile[ 0 ] == '/' ||
                soundFile[ 1 ] == ':' )
        {
            absolutePath = true;
        }
        }

        if ( !absolutePath )
        {
		boost::filesystem::path p( "../../../../.." );
		boost::filesystem::path abs_p = boost::filesystem::absolute( p );

        if ( boost::filesystem::exists( abs_p ) )

        {
			p  /= soundFile;
            soundFile = abs_p.string();
        }
        }

#if 0
        if ( !absolutePath )
        {
        char full[ _MAX_PATH ];
        if ( _fullpath( full, "..\\..\\..\\..\\..", _MAX_PATH ) != nullptr )
        {
            soundFile = string( full ) + string( "\\" ) + soundFile;
        }
        }
#endif

#ifndef SB_NO_VHCL_AUDIO
      if (commandContext.scene.getBoolAttribute("internalAudio"))
      {
        AUDIO_Stop(soundFile.c_str());
      }
      else
      {
      	if (commandContext.boneBusManager) {
			commandContext.boneBusManager->getBoneBus().SendStopSound( soundFile.c_str() );
		}
      }
#endif
      return CMD_SUCCESS;
    }

  return CMD_NOT_FOUND;
}


/*
   uscriptexec <uscript command>  - Passes command straight through to Unreal where it executes the given script command
                                  - This function existed in dimr, and is only supplied because other groups were using this command
                                  - and wish to keep using it, but don't want to have to run dimr (because of license issues).
*/

int mcu_uscriptexec_func( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager)
{
    int num = args.calc_num_tokens();

    if ( num > 0 )
    {
        string command;

        command += args.read_token();

        int i = 0;
        while ( ++i < num )
        {
        command += " ";
        command += args.read_token();
        }

        //SendWinsockExecScript( command.c_str() );
        bonebusManager.getBoneBus().ExecScript( command.c_str() );

        return CMD_SUCCESS;
    }
   return CMD_FAILURE;
}


/*
   EDF - Temporary CommAPI hooks into the CommAPI.

   CommAPI setcameraposition <x> <y> <z>
      Sets the camera's position in the world.
      x,y,z = Unreal world coordinates to set the object to.

   CommAPI setcamerarotation <x> <y> <z>
      Sets the camera's rotation in the world.
      x,y,z = Orientation in degrees.  (default coord system would match x,y,z to r,h,p
*/

int mcu_commapi_func( srArgBuffer& args, SmartBody::SBBoneBusManager& bonebusManager )
{
    char * command = args.read_token();

    if ( _stricmp( command, "setcameraposition" ) == 0 )
    {
        float x = args.read_float();
        float y = args.read_float();
        float z = args.read_float();

		bonebusManager.getBoneBus().SetCameraPosition( x, y, z );

        return CMD_SUCCESS;
    }
    else if ( _stricmp( command, "setcamerarotation" ) == 0 )
    {
        float x = args.read_float();
        float y = args.read_float();
        float z = args.read_float();

        gwiz::quat_t q = gwiz::euler_t( x, y, z );

		bonebusManager.getBoneBus().SetCameraRotation( (float)q.w(), (float)q.x(), (float)q.y(), (float)q.z() );

        return CMD_SUCCESS;
    }


    return CMD_NOT_FOUND;
}



/*
   vrKillComponent sbm
   vrKillComponent all
      Kills the sbm process
*/

int mcu_vrKillComponent_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBVHMsgManager& vhmsgManager )
{
    char * command = args.read_token();
	
	
    if ( _stricmp( command, "sb" ) == 0 ||
		_stricmp( command, "sbm" ) == 0 ||
        _stricmp( command, "all" ) == 0 )
    {
		scene.getSimulationManager()->stop();
		std::stringstream strstr;
		strstr << "vrProcEnd " << command;
		vhmsgManager.send( strstr.str().c_str() );
	    return CMD_SUCCESS;
    }
	return CMD_SUCCESS;
}

/*
   vrAllCall
     In response to this message, send out vrComponent to indicate that this component is running
*/

int mcu_vrAllCall_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager, SmartBody::SBScene& scene )
{
	
	
    vhmsgManager.send( "vrComponent sbm" );
 
    // EDF - For our reply, we're going to send one vrComponent 
    //       message for each agent loaded
	const std::vector<std::string>& characterNames = scene.getCharacterNames();
    for (const auto & characterName : characterNames)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
        string message = "sbm ";
		message += character->getName();
        vhmsgManager.send2( "vrComponent", message.c_str() );
    }
	return CMD_SUCCESS;
}

/*
 * Callback function for vrPerception message
 * vrPerception kinect/gavam ...
 * vrPerception pml-nvbg <pml>
 */
int mcu_vrPerception_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBVHMsgManager& vhmsgManager )
{
	char * command = args.read_token();
		
	//only considering kinect or gavam or both
	if (_stricmp(command, "kinect") == 0 || _stricmp(command, "gavam") == 0)
	{
		SmartBody::util::log("coordinates received");
		//if the message contains kinect data
		if (_stricmp(command, "kinect") == 0)
		{
			string stringId = args.read_token();
			int kinectId = args.read_int();
			string stringFailed = args.read_token();
			int kinectFailed = args.read_int();
			string stringUsers = args.read_token();
			int kinectUsers = args.read_int();

			for (int indexUsers = 0; indexUsers < kinectUsers; ++indexUsers)
			{
				string stringPosition = args.read_token();
				float x = args.read_float();
				float y = args.read_float();
				float z = args.read_float();
			}

			//currently not doing anything with the Kinect data
			command = args.read_token();

			//if message contains gavam after kinect
			//ideally  this parsing should be a separate method so that the message can contain gavam / kinect in any order
			if (_stricmp( command, "gavam" ) == 0)
			{
				SrVec pos;
				SrVec rot;

				//parse float data
				pos[0] = args.read_float();
				pos[1] = args.read_float();
				pos[2] = args.read_float();
				rot[0] = args.read_float();
				rot[1] = args.read_float();
				rot[2] = args.read_float();

				//For converting data from euler to quaternion

				float cos_z_2 = cosf((float)0.5*rot[2]);
				float cos_y_2 = cosf((float)0.5*rot[1]);
				float cos_x_2 = cosf((float)0.5*rot[0]);

				float sin_z_2 = sinf((float)0.5*rot[2]);
				float sin_y_2 = sinf((float)0.5*rot[1]);
				float sin_x_2 = sinf((float)0.5*rot[0]);

				// and now compute quaternion
				float quatW = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
				float quatX = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
				float quatY = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
				float quatZ = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;

				// Log the Gavam data
				SmartBody::util::log("Gavam Coordinates - %f %f %f %f %f %f", pos[0], pos[1], pos[2], rot[0], rot[1], rot[2]);
				char *messg = new char[1024];
				#ifdef WIN32
				sprintf_s(messg, 1024, "receiver skeleton brad generic rotation skullbase %f %f %f %f", quatW, quatX, quatY, quatZ);
				#else
				snprintf(messg, 1024, "receiver skeleton brad generic rotation skullbase %f %f %f %f", quatW, quatX, quatY, quatZ);
				#endif
				//log message before sending it
				//SmartBody::util::log(messg);
				//sending temp mesg just to make sure it works 
				//commandContext.scene.getVHMsgManager()->send("test", messg);
				//send the receiver message to orient the skullbase of brad as per the user's head orientation as detected by Gavam
				vhmsgManager.send2("sbm", messg);
				//after sending the message, send a test message as confirmation
				//commandContext.scene.getVHMsgManager()->send("testconfirmed", messg);
			}
		}

		else if (_stricmp( command, "gavam" ) == 0)
		{
			//this is only when only gavam is detected (which is not the case right now)
			//right now vrPerception send kinect and the gavam coordinates.
		}
	}
	else if (_stricmp(command, "pml-nvbg") == 0)
	{
		std::string pml = args.read_remainder_raw();

		// all characters should be receiving the perception message
		

		const std::vector<std::string>& characterNames = scene.getCharacterNames();
		for (const auto & characterName : characterNames)
		{
			SmartBody::SBCharacter* character = scene.getCharacter(characterName);
	
			SmartBody::Nvbg* nvbg = character->getNvbg();
			if (!nvbg)
				continue;

			bool ok = nvbg->execute(character->getName(), "", "", pml);
			if (!ok)
			{
				SmartBody::util::log("NVBG for perception did not handle message %s.", pml.c_str());
			}
		}			
		return CMD_SUCCESS;	
	}
	else
	{
		SmartBody::util::log("mcu_vrPerception_func ERR: message not recognized!");
		return CMD_FAILURE;
	}

	return CMD_FAILURE;
}

/*
 * Callback function for vrBCFeedback message
 * vrBCFeedback <feedback agent name> <xml>
 */
int mcu_vrBCFeedback_func( srArgBuffer& args, SmartBody::SBScene& scene )
{
	std::string cName = args.read_token();
	std::string xml = args.read_remainder_raw();
	SmartBody::SBCharacter* character = scene.getCharacter(cName);
	if (character)
	{
		SmartBody::Nvbg* nvbg = character->getNvbg();
		if (!nvbg)
		{
			SmartBody::util::log("mcu_vrBCFeedback_func ERR: character %s doesn't has NVBG attached", cName.c_str());
			return CMD_FAILURE;
		}
		bool ok = nvbg->execute(cName, "", "", xml);
		if (!ok)
		{
			SmartBody::util::log("NVBG for perception did not handle message %s.", xml.c_str());
			return CMD_FAILURE;
		}			
	}
	else
	{
		SmartBody::util::log("mcu_vrBCFeedback_func ERR: character %s not found.", cName.c_str());
		return CMD_FAILURE;
	}
	return CMD_SUCCESS;
}

/*
 * Callback function for vrSpeech message
 * vrSpeech start id speaker
 * vrSpeech finish-speaking id
 * This message goes into NVBG
 */
int mcu_vrSpeech_func( srArgBuffer& args, SmartBody::SBScene& scene )
{
	
	std::string status = args.read_token();
	std::string id = args.read_token();
	std::string speaker = args.read_token();

	// all characters should be receiving the perception message
	const std::vector<std::string>& characterNames = scene.getCharacterNames();
	for (const auto & characterName : characterNames)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		SmartBody::Nvbg* nvbg = character->getNvbg();
		if (!nvbg)
			continue;

		bool ok = nvbg->executeSpeech(character->getName(), status, id, speaker);
		if (!ok)
		{
			SmartBody::util::log("NVBG cannot handle vrSpeech message");
		}
	}			
	return CMD_SUCCESS;
}


/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

int mcu_syncpolicy_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
    int num = args.calc_num_tokens();

    if ( num > 0 )
    {
        string command = args.read_token();
		if (command == "delay")
		{
			scene.setBoolAttribute("delaySpeechIfNeeded", true);
		}
		else if (command == "nodelay")
		{
		scene.setBoolAttribute("delaySpeechIfNeeded", false);
		}
		else
		{
		SmartBody::util::log("Usage: syncpolicy <delay|nodelay>");
		return CMD_FAILURE;
		}
 
        return CMD_SUCCESS;
    }
	else
	{
		if (scene.getBoolAttribute("delaySpeechIfNeeded"))
		{
			SmartBody::util::log("Behavior policy is 'delay'. Behaviors will be offset to a future time to make sure that all behaviors are executed in full.");
		}
		else
		{
			SmartBody::util::log("Behavior policy is 'nodelay'. Behaviors that are calculated to start in the past will be partially executed.");
		}
		return CMD_SUCCESS;
	}
   return CMD_FAILURE;
}

// Functions:
// 1. connecting the motion with the character, print out the channels inside the motion
// 2. examine whether these channels exist inside the skeleton
// 3. If frame number exist, this will output which channels are affected 
// check motion/skeleton <character name> <motion name> [frame number]
// frame number is optional
int mcu_check_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
	char* operation = args.read_token();
	char* charName = args.read_token();
	char* motionName = args.read_token();
	char* frameNumString = args.read_token();
	int frameNum = -1;
	if (strcmp(frameNumString, "") != 0)	frameNum = atoi(frameNumString);
	int mode = -1;
	int chanSize = 0;
	SkChannel chan;

	if (strcmp(operation, "motion") == 0)	mode = 1;
	if (strcmp(operation, "skeleton") == 0)	mode = 2;
	if (mode == -1)
	{
		SmartBody::util::log("mcu_check_func ERR: Mode %s not FOUND!", operation);
		return CMD_FAILURE;
	}

	SmartBody::SBCharacter* character = scene.getCharacter(charName);
	SmartBody::SBMotion* motion = scene.getAssetManager()->getMotion(motionName);
	if (!motion)
	{
		SmartBody::util::log("mcu_check_func ERR: Motion %s NOT EXIST!", motionName);
		return CMD_FAILURE;
	}
	if (character)
	{
		int numValidChannels = motion->connect(character->getSkeleton().get());	// connect and check for the joints
		SkChannelArray& mChanArray = motion->channels();
		int mChanSize = mChanArray.size();
		SkChannelArray& skelChanArray = character->getSkeleton()->channels();
		int skelChanSize = skelChanArray.size();
		if (mode == 1)
		{
			chanSize = mChanSize;
			SmartBody::util::log("Channels in motion %s's channel matching %s's skeleton are preceeded with '+'", motionName, charName);
			SmartBody::util::log("motion %s's Channel Info:", motionName);
		}
		if (mode == 2)
		{
			chanSize = skelChanSize;
			SmartBody::util::log("Channels in skeleton %s's channel matching motion %s's channel are preceeded with '+'", charName, motionName);
			SmartBody::util::log("character %s's Channel Info:", charName);
		}
		SmartBody::util::log("Channel Size: %d", chanSize);
		for (int i = 0; i < chanSize; i++)
		{				
			std::stringstream outputInfo;
			std::string channelName;
			if (mode == 1)
			{
				chan = mChanArray[i];
				channelName = mChanArray.name(i);
			}
			if (mode == 2)
			{
				chan = skelChanArray[i];
				channelName = skelChanArray.name(i);
			}

			if (!chan.joint)
			{
				SmartBody::util::log("  %d: (No mathing joint)", i);
				continue;
			}

			std::string jointName = chan.joint->jointName();
			int	chanType = chan.type;
			std::string chanTypeString;
			switch (chanType)
			{
				case 0:
					chanTypeString = "XPos";
					break;
				case 1:	
					chanTypeString = "YPos";
					break;
				case 2:
					chanTypeString = "ZPos";
					break;
				case 6:
					chanTypeString = "Quat";
					break;
				default:
					chanTypeString = "Others";
			}
			int pos = 0;
			if (mode == 1)
			{
				pos = skelChanArray.search(chan.joint->jointName(), chan.type);
				//pos = skelChanArray.linear_search(chan.joint->name(), chan.type);
			}
			if (mode == 2)	
			{
				pos = mChanArray.search(chan.joint->jointName(), chan.type);
				//pos = mChanArray.linear_search(chan.joint->name(), chan.type);
			}
			if (pos != -1)
				outputInfo << "+ ";
			if (pos == -1)	
				outputInfo << "  ";
			outputInfo << i << ": " << jointName.c_str() << " (" << chanTypeString << ")";
			SmartBody::util::log("%s", outputInfo.str().c_str());
		}
			
		// check the non-zero channel
		if (frameNum >= 0 && frameNum < motion->frames())
		{
			float* frameData = motion->posture(frameNum);
			int channelCounter = 0;
			for (int c = 0; c < mChanArray.size(); c++)
			{
				int curIndex = channelCounter;
				SkChannel& channel = mChanArray.get(c);
				std::string jointName = mChanArray.name(c);
				int chanSize = channel.size();
				int numZeroFrames = 0;
				int	chanType = channel.type;
				if (chanType == SkChannel::Quat)
				{
					if (fabs(frameData[channelCounter] - 1.0) > .0001 ||
						fabs(frameData[channelCounter + 1]) > .0001 ||
						fabs(frameData[channelCounter + 2]) > .0001 ||
						fabs(frameData[channelCounter + 3]) > .0001)
					{
						numZeroFrames = 0;
					}
					else
					{
						numZeroFrames = 1;
					}
					channelCounter += 4;
				}
				else
				{
					for (int x = 0; x < chanSize; x++)
					{
						if (fabs(frameData[channelCounter]) < .0001)
							numZeroFrames++;
						channelCounter++;
					}
							
				}
										
				if (numZeroFrames == 0)
				{
					if (chanType == SkChannel::XPos || chanType == SkChannel::YPos || chanType == SkChannel::ZPos ||
						chanType == SkChannel::XRot || chanType == SkChannel::YRot || chanType == SkChannel::ZRot || 
						chanType == SkChannel::Twist)
						SmartBody::util::log("Channel %s/%s has non-zero value: %f.", jointName.c_str(), channel.type_name(), frameData[curIndex]);
					else if (chanType == SkChannel::Swing)
						SmartBody::util::log("Channel %s/%s has non-zero value: %f %f.", jointName.c_str(), channel.type_name(), frameData[curIndex], frameData[curIndex + 1]);
					else if (chanType == SkChannel::Quat)
						SmartBody::util::log("Channel %s/%s has non-zero value: %f %f %f %f.", jointName.c_str(), channel.type_name(), frameData[curIndex], frameData[curIndex+ 1], frameData[curIndex + 2], frameData[curIndex + 3]);
				}
			}
			motion->disconnect();
		}
		else
		{
			if (strcmp(frameNumString, "") != 0)
			{
				SmartBody::util::log("mcu_check_func ERR: Frame Number not Exist!");
				motion->disconnect();
				return CMD_FAILURE;			
			}
		}
	}
	else
	{
		SmartBody::util::log("mcu_check_func ERR: Character %s NOT EXIST!", charName);
		return CMD_FAILURE;
	}

	return CMD_FAILURE;
}


int mcu_python_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
	std::string command = args.read_remainder_raw();
	bool val = scene.run(command);
	if (val)
		return CMD_SUCCESS;
	else
		return CMD_FAILURE;
}

int mcu_pythonscript_func( srArgBuffer& args, SmartBody::SBScene& scene )
{
	std::string scriptName = args.read_token();	
	return scene.runScript(scriptName);
}


int mcu_steer_func( srArgBuffer& args, SmartBody::SBScene& scene, SmartBody::SBSteerManager& steerManager )
{
	std::string command = args.read_token();
	if (command == "help")
	{
		SmartBody::util::log("Use: steer <start|stop>");
		SmartBody::util::log("     steer move <character> <mode> <x> <y> <z>");
		return CMD_SUCCESS;
	}
	else if (command == "unit")
	{
		SmartBody::util::log("Unit no longer supported. Use scene.setScale()");
		return CMD_SUCCESS;
	}
	else if (command == "start")
	{
		steerManager.setEnable(true);
		return CMD_SUCCESS;
	}
	else if (command == "stop")
	{
		steerManager.setEnable(false);
		return CMD_SUCCESS;
	}
	else if (command == "move")
	{
		//SmartBody::util::log("run steer move command");
		std::string characterName = args.read_token();
		std::string mode = args.read_token();
		int num = args.calc_num_tokens();
		if (num % 3 != 0)
		{
			SmartBody::util::log("Syntax: steer move <character> <mode> <x1> <y1> <z1> <x2> <y2> <z2> ...");
			return CMD_FAILURE;
		}
		if (steerManager.getEngineDriver()->isInitialized())
		{
			SbmCharacter* character = scene.getCharacter(characterName);
			if (character)
			{
				SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
				if (steerAgent)
				{
					PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
					character->trajectoryGoalList.clear();
					auto& steerPath = ppraiAgent->steerPath;
					float pathRadius = 1.f;												
					float x, y, z;
					float yaw, pitch, roll;
					character->get_world_offset(x, y, z, yaw, pitch, roll);
					character->trajectoryGoalList.emplace_back(x, y, z);

					if (mode == "normal")
					{

						if (ppraiAgent->getAgent())
						{
							const SteerLib::AgentGoalInfo& curGoal = ppraiAgent->getAgent()->currentGoal();
							ppraiAgent->getAgent()->clearGoals();
						}
						if (!ppraiAgent->goalList.empty())
						{
							ppraiAgent->sendLocomotionEvent("interrupt");
						}
						ppraiAgent->goalList.clear();
						//SmartBody::util::log("steer move : add agent goals. Num goal = %d",num);
						for (int i = 0; i < (num + 2); i += 3)
						{
							auto posX = args.read_float();
							auto poxY = args.read_float();
							auto posZ = args.read_float();
							SrVec pos(posX, poxY, posZ);
							ppraiAgent->goalList.emplace_back(pos);
							character->trajectoryGoalList.emplace_back(pos);
						}
					}
					else if (mode == "additive")
					{
						for (int i = 0; i < (num + 2); i += 3)
						{
							auto posX = args.read_float();
							auto poxY = args.read_float();
							auto posZ = args.read_float();
							SrVec pos(posX, poxY, posZ);
							ppraiAgent->goalList.emplace_back(pos);
							character->trajectoryGoalList.emplace_back(pos);
						}
					}
					else
						SmartBody::util::log("steer move mode not recognized!");

					auto& trajList = character->trajectoryGoalList;
					std::vector<SrVec> trajPtList;
					// add steering path
					trajPtList.reserve(trajList.size());
					for (auto & i : trajList)
					{
						trajPtList.emplace_back(i);
					}
					steerPath.clearPath();
					steerPath.initPath(trajPtList,pathRadius);
				}
			}
		}
		return CMD_SUCCESS;
	}
	else if (command == "proximity")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			if (steerAgent)
			{
				ppraiAgent->distThreshold = (float)args.read_double() / scene.getScale();
				return CMD_SUCCESS;
			}
			else
			{
				SmartBody::util::log("No steering agent for character '%s'.", character->getName().c_str());
				return CMD_FAILURE;
			}
		}
	}
	else if (command == "fastinitial")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			std::string fastinitialString = args.read_token();
			if (fastinitialString == "true")
				ppraiAgent->fastInitial = true;
			else
				ppraiAgent->fastInitial = false;
			return CMD_SUCCESS;
		}
	}
	else if (command == "speed")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			if (character->locomotion_type != character->Procedural)
			{
				ppraiAgent->desiredSpeed = (float)args.read_double();
				return CMD_SUCCESS;
			}
		}		
	}
	else if (command == "stateprefix")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			character->statePrefix = args.read_token();
			return CMD_SUCCESS;
		}
		else
			return CMD_FAILURE;
	}
	else if (command == "type")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			std::string type = args.read_token();
			if (type == "example")
			{
				if (steerManager.checkExamples(*character))
					character->locomotion_type = character->Example;
				else
					character->locomotion_type = character->Basic;
				/*
				if (character->param_animation_ct)
					character->param_animation_ct->set_pass_through(false);
				if (character->locomotion_ct)
					character->locomotion_ct->set_pass_through(true);
				if (character->basic_locomotion_ct)
					character->basic_locomotion_ct->set_pass_through(true);
				*/
				return CMD_SUCCESS;
			}
			if (type == "procedural")
			{
				character->locomotion_type = character->Procedural;
				SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
				if (steerAgent)
				{
					PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
					ppraiAgent->desiredSpeed = 1.6f;
				}
						
				/*
				if (character->param_animation_ct)
					character->param_animation_ct->set_pass_through(true);
				if (character->locomotion_ct)
					character->locomotion_ct->set_pass_through(false);
				if (character->basic_locomotion_ct)
					character->basic_locomotion_ct->set_pass_through(true);
				*/

				return CMD_SUCCESS;
			}
			if (type == "basic")
			{
				character->locomotion_type = character->Basic;
				/*
				if (character->param_animation_ct)
					character->param_animation_ct->set_pass_through(true);
				if (character->locomotion_ct)
					character->locomotion_ct->set_pass_through(true);
				if (character->basic_locomotion_ct)
					character->basic_locomotion_ct->set_pass_through(false);
				*/
				return CMD_SUCCESS;
			}
			if (type == "new")
			{
				character->locomotion_type = character->New;
				return CMD_SUCCESS;
			}
		}
	}
	else if (command == "facing")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			ppraiAgent->facingAngle = (float)args.read_double();
			return CMD_SUCCESS;
		}				
	}
	else if (command == "braking")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			ppraiAgent->brakingGain = (float)args.read_double();
			if (ppraiAgent->brakingGain < 1.0f)
				ppraiAgent->brakingGain = 1.0f;
			if (ppraiAgent->brakingGain > 4.5f)
				ppraiAgent->brakingGain = 4.5f;
			return CMD_SUCCESS;
		}				
	}
	else if (command == "test")
	{
		std::string characterName = args.read_token();
		SmartBody::SBCharacter* character = scene.getCharacter(characterName);
		if (character)
		{
			SmartBody::SBSteerAgent* steerAgent = steerManager.getSteerAgent(character->getName());
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			ppraiAgent->startParameterTesting();
			return CMD_SUCCESS;
		}
	}
	return CMD_FAILURE;
}

int syncpoint_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
	std::string motionStr = args.read_token();

	if (motionStr.empty())
	{
		SmartBody::util::log("Usage: syncpoint <motion>");
		SmartBody::util::log("Usage: syncpoint <motion> <start> <ready> <strokestart> <stroke> <strokeend> <relax> <end>");
	}

	SmartBody::SBMotion* motion = scene.getAssetManager()->getMotion(motionStr);	
	if (motion)
	{
		int num = args.calc_num_tokens();
		if (num == 0)
		{
			// show the current sync point timings
			double start = motion->time_start();
			double ready = motion->time_ready();
			double strokeStart = motion->time_stroke_start();
			double stroke = motion->time_stroke_emphasis();
			double strokeEnd = motion->time_stroke_end();
			double relax = motion->time_relax();
			double end = motion->time_stop();
			SmartBody::util::log("%f %f %f %f %f %f %f", start, ready, strokeStart, stroke, strokeEnd, relax, end);
			return CMD_SUCCESS;
		}
		else if (num != 7)
		{
			SmartBody::util::log("Usage: syncpoint <motion> <start> <ready> <strokestart> <stroke> <strokeend> <relax> <end>");
			return CMD_FAILURE;
		}
		else
		{
			double start = args.read_float();
			double ready = args.read_float();
			double strokeStart = args.read_float();
			double stroke = args.read_float();
			double strokeEnd = args.read_float();
			double relax = args.read_float();
			double end = args.read_float();
			motion->synch_points.set_time(start, ready, strokeStart, stroke, strokeEnd, relax, end);
			return CMD_SUCCESS;
		}

	}

	return CMD_SUCCESS;
}

#ifdef USE_GOOGLE_PROFILER
int startprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{	
	SmartBody::util::log("Starting the CPU Profiler...");
	ProfilerStart("/tmp/smartbodyprofile");
	return CMD_SUCCESS;
}

int stopprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	SmartBody::util::log("Stopping the CPU Profiler...");
	ProfilerStop();
	return CMD_SUCCESS;
}
int startheapprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	SmartBody::util::log("Starting the heap Profiler...");
	HeapProfilerStart("/tmp/smartbodyheapprofile");
	return CMD_SUCCESS;
}

int stopheapprofile_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	SmartBody::util::log("Stopping the heap Profiler...");
	HeapProfilerStop();
	HeapProfilerDump("/tmp/smartbodyheapprofile");
	return CMD_SUCCESS;
}
#endif

// Usage:
// receiver echo <content>
// receiver enable
// receiver skeleton <skeletonName> <emitterType> position <joint-index/joint-name> <x> <y> <z>	
// receiver skeleton <skeletonName> <emitterType> positions <x1> <y1> <z1> <x2> <y2> <z2> ...							24 Joints in total if emitterType == "kinect"
// receiver skeleton <skeletonName> <emitterType> rotation <joint-index/joint-name> <q.w> <q.x> <q.y> <q.z>
// receiver skeleton <skeletonName> <emitterType> rotations <q1.w> <q1.x> <q1.y> <q1.z> <q2.w> <q2.x> <q2.y> <q2.z>...	20 Joints in total if emitterType == "kinect"
// 
// p.s. 
// currently position is for global and rotation is for local
int mcu_joint_datareceiver_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
	//SmartBody::util::log("in data receiver function");
	std::string operation = args.read_token();
	if (operation == "echo")
		SmartBody::util::log("%s", args.read_remainder_raw());
	if (operation == "skeleton")
	{		
		std::string skelName = args.read_token();
		const std::vector<std::string>& characterNames = scene.getCharacterNames();
		std::vector<SmartBody::SBCharacter*> controlledCharacters;
		for (const auto & characterName : characterNames)
		{
			SmartBody::SBCharacter* character = scene.getCharacter(characterName);
			std::string receiverName = character->getStringAttribute("receiverName");
			if (receiverName == skelName || character->getName() == skelName)
			{
				controlledCharacters.emplace_back(character);
			}
		}

		//if (controlledCharacters.size() == 0)
		//	return CMD_SUCCESS;


		std::string emitterName = args.read_token();
		
		float scale = 1.0f;
		if (emitterName == "kinect" ||
			emitterName == "kinect2")
		{
			//scale = .1f;
			scale = 100.f;
		}

		std::string skeletonType = args.read_token();
		if (skeletonType == "position")
		{
			std::string jName;
			if (emitterName == "kinect" ||
				emitterName == "kinect2")
				jName = scene.getKinectProcessor()->getSBJointName(args.read_int());
			else
				jName = args.read_token();
			float x = args.read_float();
			float y = args.read_float();
			float z = args.read_float();

			for (auto character : controlledCharacters)
			{
					double characterScale = character->getDoubleAttribute("receiverScale");
				float scaledX = x * characterScale;
				float scaledY = y * characterScale;
				float scaledZ = z * characterScale;
				SrVec vec(scaledX, scaledY, scaledZ);
				SrVec outVec;
				scene.getKinectProcessor()->processRetargetPosition(character->getSkeleton()->getName(),vec, outVec);
				
				if (emitterName == "kinect" ||
					emitterName == "kinect2")
					character->datareceiver_ct->setGlobalPosition(jName, outVec);
				else
					character->datareceiver_ct->setLocalPosition(jName, outVec);
			}
			
			
		}
		else if (skeletonType == "positions")
		{
				// TODO: add support to joint global positions
		}
		else if (skeletonType == "rotation")
		{
			std::string jName;
			if (emitterName == "kinect"||
				emitterName == "kinect2")
				jName = scene.getKinectProcessor()->getSBJointName(args.read_int());
			else
				jName = args.read_token();

			SrQuat quat;
			quat.w = args.read_float();
			quat.x = args.read_float();
			quat.y = args.read_float();
			quat.z = args.read_float();
			for (auto character : controlledCharacters)
			{
					character->datareceiver_ct->setLocalRotation(jName, quat);
			}
		}
		else if (skeletonType == "norotation")
		{
			std::string jName;
			if (emitterName == "kinect" ||
				emitterName == "kinect2")
				jName = scene.getKinectProcessor()->getSBJointName(args.read_int());
			else
				jName = args.read_token();

			for (auto character : controlledCharacters)
			{
					character->datareceiver_ct->removeLocalRotation(jName);
			}
		}
		else if (skeletonType == "noposition")
		{
			std::string jName;
			if (emitterName == "kinect" ||
				emitterName == "kinect2")
				jName = scene.getKinectProcessor()->getSBJointName(args.read_int());
			else
				jName = args.read_token();

			for (auto character : controlledCharacters)
			{
					character->datareceiver_ct->removeLocalPosition(jName);
			}
		}
		else if (skeletonType == "rotations")
		{
			if (emitterName == "kinect" ||
				emitterName == "kinect2")
			{
				int numRemainTokens = args.calc_num_tokens();
				if (numRemainTokens < 25*4)
				{
					SmartBody::util::log("Kinect skeleton %s rotation data is not valid.", skelName.c_str());
					return CMD_FAILURE;
				}
				std::vector<SrQuat> quats;
				for (int i = 0; i < 25; i++)
				{
					SrQuat quat;
					quat.w = args.read_float();
					quat.x = args.read_float();
					quat.y = args.read_float();
					quat.z = args.read_float();
					quat.normalize();
					quats.emplace_back(quat);

					// converte rotation axis for kinect
					quats[i].z *= -1.0f;
					quats[i].x *= -1.0f;					
				}
				//KinectProcessor::processGlobalRotation(quats);
				scene.getKinectProcessor()->filterRotation(quats);
				std::vector<SrQuat> retargetQuats;
				SmartBody::SBRetargetManager* retargetManager = scene.getRetargetManager();
				for (auto character : controlledCharacters)
				{
						scene.getKinectProcessor()->processRetargetRotation(character->getSkeleton()->getName(),quats, retargetQuats);
					if (retargetQuats.size() >= 25)
					{
						for (int i = 0; i < 25; i++)
						{
							if (fabs(retargetQuats[i].w) > .0001)
							{
								const std::string& mappedJointName = scene.getKinectProcessor()->getSBJointName(i);
								if (!mappedJointName.empty())
									character->datareceiver_ct->setLocalRotation(mappedJointName, retargetQuats[i]);
							}
						}
					}					
				}
			}
		}
		else if (skeletonType == "initsk")
		{
			if (emitterName == "kinect" ||
				emitterName == "kinect2")
			{
				SmartBody::SBAssetManager* assetManager = scene.getAssetManager();
				auto kinectSk = assetManager->getSkeleton("kinect.sk");
				int numRemainTokens = args.calc_num_tokens();
				if (numRemainTokens < 25*7)
				{
					SmartBody::util::log("Kinect skeleton %s rotation and position data is not valid.", skelName.c_str());
					return CMD_FAILURE;
				}
				std::vector<SrQuat> quats;
				std::vector<SrVec> trans;
				for (int i = 0; i < 25; i++)
				{
					SrQuat quat;
					quat.w = args.read_float();
					quat.x = args.read_float();
					quat.y = args.read_float();
					quat.z = args.read_float();
					quat.normalize();
					quats.emplace_back(quat);
					
					SrVec tran;
					tran.x = args.read_float() * scale;
					tran.y = args.read_float() * scale;
					tran.z = args.read_float() * scale;
					trans.emplace_back(tran);
				}
				if (!kinectSk)
					scene.getKinectProcessor()->initKinectSkeleton(trans, quats);
			}
		}
		
	}
	return CMD_SUCCESS;
}

int mcu_character_breathing( const char* name, srArgBuffer& args, SmartBody::SBScene& scene) //Celso: Summer 2008
{
	SmartBody::SBCharacter* char_p = scene.getCharacter( name );
	if( !char_p )	
	{
		SmartBody::util::log( "mcu_character_breathing ERR: Character '%s' NOT FOUND\n", name ); 
		return( CMD_FAILURE );
	}

	auto& breathing_p = char_p->breathing_p;
	if (!breathing_p)
	{
		SmartBody::util::log("Character '%s' has no breathing controller.", name);
		return CMD_FAILURE;
	}

	char *breathing_cmd = args.read_token();
	
	if( strcmp( breathing_cmd, "useblendchannels" ) == 0 )	
	{
		std::string token = args.read_token();
		if (token == "true" || token.empty())
		{
			breathing_p->setUseBlendChannels(true);
		}
		else if (token == "false")
		{
			breathing_p->setUseBlendChannels(false);
		}
		else
		{
			SmartBody::util::log("Usage: char %s breathing usechannels true|false", char_p->getName().c_str());
			return CMD_FAILURE;
		}
		return CMD_SUCCESS;
	}
	else if( strcmp( breathing_cmd, "bpm" ) == 0 )	
	{
		float bpm = args.read_float();
		bool smooth = true;
		if(args.calc_num_tokens() > 0)
		{
			char *smooth_str = args.read_token();
			smooth = (strcmp(smooth_str, "true") == 0);
		}
		breathing_p->breaths_per_minute(bpm, smooth);
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "min" ) == 0 )	
	{
		float min = args.read_float();
		float transition = 1.0f;
		if(args.calc_num_tokens() > 0)
			transition = args.read_float();
		if((strcmp(breathing_p->current_breath_layer()->cycle->type(), "LinearBreathCycle") == 0)
			|| (strcmp(breathing_p->current_breath_layer()->cycle->type(), "SineBreathCycle") == 0))
			((MinMaxBreathCycle*)breathing_p->current_breath_layer()->cycle.get())->min(min, transition);
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "max" ) == 0 )	
	{
		float max = args.read_float();
		float transition = 1.0f;
		if(args.calc_num_tokens() > 0)
			transition = args.read_float();
		if((strcmp(breathing_p->current_breath_layer()->cycle->type(), "LinearBreathCycle") == 0)
			|| (strcmp(breathing_p->current_breath_layer()->cycle->type(), "SineBreathCycle") == 0))
			((MinMaxBreathCycle*)breathing_p->current_breath_layer()->cycle.get())->max(max, transition);
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "motion" ) == 0 )	
	{
		char* name = args.read_token();
		SmartBody::SBMotion* sbmotion = scene.getAssetManager()->getMotion(name);
		
		if( sbmotion == nullptr ) {
			printf( "Breathing motion '%s' NOT FOUND in motion map\n", name ); 
			return( CMD_FAILURE );
		}
		breathing_p->motion(sbmotion);
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "incremental" ) == 0 )
	{
		char *incremental_str = args.read_token();
		bool incremental = (strcmp(incremental_str, "true") == 0);
		breathing_p->incremental(incremental);
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "push" ) == 0 )	
	{
		char *type = args.read_token();

		if( strcmp(type, "linear") == 0)
			//The default cycle is linear, so just pop all others.
			breathing_p->clear_breath_layers();
			//breathing_p->push_breath_layer(breathing_p->default_breath_cycle());
		else if( strcmp(type, "sine") == 0)
			breathing_p->push_breath_layer(std::make_unique<SineBreathCycle>(
			breathing_p->default_breath_cycle()->min(), breathing_p->default_breath_cycle()->max()));
		else if( strcmp(type, "keyframe") == 0)
		{
			int args_count = args.calc_num_tokens();
			if((args_count % 2) != 0)
			{
				printf( "Number of arguments should be even. Try 'breathing <name> help'."); 
				return( CMD_FAILURE );
			}
			
			auto cycle = std::make_unique<KeyframeBreathCycle>();
			for(int i=0; i<args_count; i = i+2)
			{
				float time = args.read_float();
				float value = args.read_float();
				cycle->keyframes.emplace_back(KeyframeBreathCycle::Keyframe{value, time});
			}
			cycle->update();

			breathing_p->push_breath_layer(std::move(cycle));
		}
		else if( strcmp(type, "spline") == 0)
		{
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
			int args_count = args.calc_num_tokens();
			if((args_count % 2) != 0)
			{
				printf( "Number of arguments should be even. Try 'breathing <name> help'."); 
				return( CMD_FAILURE );
			}

			auto spline = std::make_unique<MeSpline1D>();
			for(int i=0; i<args_count; i = i+2)
			{
				float x = args.read_float();
				float y = args.read_float();
				spline->make_smooth(x, y, 0, 0, 0);
			}
			breathing_p->push_breath_layer(std::make_unique<SplineBreathCycle>(std::move(spline)));
#endif
		}
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "pop" ) == 0 )
	{
		breathing_p->pop_breath_layer();
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "print" ) == 0 )
	{
		//breathing_p->print_state(0);
		return( CMD_SUCCESS );
	}
	else if( strcmp( breathing_cmd, "help" ) == 0 )
	{
		printf("Syntax: breathing <controller_name> <options>\n"
			"Options:\n"
			" help\n"
			"\tSets the breaths-per-minute rate.\n"
			"\tSet 'isSmooth' to 'true' or 'false' to define whether to apply a\n"
			"\tsmooth transition. Defaults to 'true'.\n"

			" min <value> [transition]\n"
			"\tSets the minimum respiratory volume. Use with the linear and sine\n"
			"\tbreath cycles only.\n"
			"\tSet 'transition' to the transition duration in seconds. Default\n"
			"\tvalue is 1.0s.\n"
			
			" max <value> [transition]\n"
			"\tSets the maximum respiratory volume. Use with the linear and sine\n"
			"\tbreath cycles only.\n"
			"\tSet 'transition' to the transition duration in seconds. Default\n"
			"\tvalue is 1.0s.\n"
			
			" motion <name> [isSmooth]\n"
			"\tSets the breathing motion.\n"

			" incremental <bool>\n"
			"\tSets whether the breathing motion is additive.\n"

			" push linear\n"
			"\tPushes to the top of the stack the (default) linear breathing cycle.\n"

			" push sine\n"
			"\tPushes to the top of the stack a sine breathing cycle.\n"
			
			" push keyframe <time1> <value1> <time2> <value2> ...\n"
			"\tPushes to the top of the stack a custom keyframe breathing cycle.\n"
			
			" push spline knot1_x knot1_y knot2_x knot2_y ...\n"
			"\tPushes to the top of the stack a spline breathing cycle.\n"
			
			" pop\n"
			"\tPops the breathing cycle from the top of the stack.\n"
			
			" print\n"
			"\tPrints the controller's state\n"
			);
					
		return( CMD_SUCCESS );
	}


	return( CMD_FAILURE );
}


int mcu_vrExpress_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
	if (args.calc_num_tokens() < 4)
	{
		return CMD_SUCCESS;
	}

	std::string actor = args.read_token();
	std::string to = args.read_token();
	std::string messageId = args.read_token();
	std::string xml = args.read_remainder_raw();

	// get the NVBG process for the character, if available
	SmartBody::SBCharacter* character = scene.getCharacter(actor);
	if (!character)
		return CMD_SUCCESS;

	SmartBody::Nvbg* nvbg = character->getNvbg();
	if (!nvbg)
		return CMD_SUCCESS;

	bool ok = nvbg->execute(actor, to, messageId, xml);

	if (!ok)
	{
		SmartBody::util::log("NVBG for character %s to %s did not handle message %s.", actor.c_str(), to.c_str(), messageId.c_str());
	}
	return CMD_SUCCESS;
}

int mcu_vhmsg_connect_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager)
{
	char* vhmsgServerVar = getenv( "VHMSG_SERVER" );
	char* vhmsgPortVar = getenv("VHMSG_PORT");
	char* vhmsgScopeVar =  getenv("VHMSG_SCOPE");

	std::string vhmsgServer = "localhost";
	if (vhmsgServerVar)
		vhmsgServer = vhmsgServerVar;

	std::string vhmsgPort = "61616";
	if (vhmsgPortVar)
		vhmsgPort = vhmsgPortVar;

	std::string vhmsgScope = "DEFAULT_SCOPE";

	if (args.calc_num_tokens() > 0)
	{
		vhmsgServer = args.read_token();
	}

	if (args.calc_num_tokens() > 0)
	{
		vhmsgPort = args.read_token();
	}

	if (args.calc_num_tokens() > 0)
	{
		vhmsgScope = args.read_token();
	}

	// disconnect first in case we are already connected
	vhmsgManager.setServer(vhmsgServer);
	vhmsgManager.setPort(vhmsgPort);
	vhmsgManager.setScope(vhmsgScope);
	bool success = vhmsgManager.connect();

	if (success)
	{
		return CMD_SUCCESS;
	}
	else
	{
		SmartBody::util::log("Could not connect to %s:%s", vhmsgServer.c_str(), vhmsgPort.c_str());
		vhmsgManager.setEnable(false);
		return CMD_FAILURE;
	}

}

int mcu_vhmsg_disconnect_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager )
{
#ifndef SB_NO_VHMSG

	if (!vhmsgManager.isEnable())
	{
		SmartBody::util::log("VHMSG is not connected.");
		return CMD_FAILURE;
	}
	int ret = vhmsg::ttu_close();
	if (ret == vhmsg::TTU_ERROR)
	{
		SmartBody::util::log("Problem disconnecting VHMSG.");
		return CMD_FAILURE;
	}
	else
	{
		vhmsgManager.setEnable(false);
		SmartBody::util::log("VHMSG has been disconnected.");
		return CMD_SUCCESS;
	}
#else
	SmartBody::util::log("VHMSG is disabled.");
	return CMD_FAILURE;
#endif
	
}

//void mcu_vhmsg_callback( const char *op, const char *args, void * user_data )
//{
//	if (commandContext.scene.isRemoteMode())
//	{
//		commandContext.scene.getDebuggerClient()->ProcessVHMsgs(op, args);
//		return;
//	}
//	else
//		commandContext.scene.getDebuggerServer()->ProcessVHMsgs(op, args);
//
//	switch(cmdMgr->execute( op, (char *)args ) ) {
//        case CMD_NOT_FOUND:
//            SmartBody::util::log("SmartBody error: command NOT FOUND: '%s' + '%s'", op, args );
//            break;
//        case CMD_FAILURE:
//            SmartBody::util::log("SmartBody error: command FAILED: '%s' + '%s'", op, args );
//            break;
//    }
//}

int mcuFestivalRemoteSpeechCmd_func( srArgBuffer& args, SmartBody::SBScene& scene)
{
	//FestivalSpeechRelayLocal* speechRelay = mcu.festivalRelay();
	SpeechRelayLocal* speechRelay = scene.getSpeechManager()->cereprocRelay();
	if (!speechRelay) return 0;
	const char* message = args.read_remainder_raw();
	speechRelay->processSpeechMessage(message);
	//processSpeechMessage(
	return 0;
}


int vhmsglog_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager)
{

	if (args.calc_num_tokens() == 0)
	{
		if (vhmsgManager.isEnableLogging())
		{
			SmartBody::util::log("VHMSG logging is on");
		}
		else
		{
			SmartBody::util::log("VHMSG logging is off");
		}
		return CMD_SUCCESS;
	}

	std::string token = args.read_token();
	if (token == "on")
	{
		if (vhmsgManager.isEnableLogging())
		{
			SmartBody::util::log("VHMSG logging is already on");
			return CMD_SUCCESS;
		}
		else
		{
			vhmsgManager.setEnableLogging(true);
			SmartBody::util::log("VHMSG logging is now on");
			return CMD_SUCCESS;
		}
	}
	else if (token == "off")
	{
		if (!vhmsgManager.isEnableLogging())
		{
			SmartBody::util::log("VHMSG logging is already off");
			return CMD_SUCCESS;
		}
		else
		{
			vhmsgManager.setEnableLogging(false);
			SmartBody::util::log("VHMSG logging is now off");
			return CMD_SUCCESS;
		}
	}
	else
	{
		SmartBody::util::log("Usage: vhmsglog <on|off>");
		return CMD_FAILURE;
	}

}

int mcu_echo_func( srArgBuffer& args, SmartBody::SBSimulationManager& simMgr)
{
	std::stringstream timeStr;
	timeStr << simMgr.getTime();
	std::string echoStr = args.read_remainder_raw();
	int pos = echoStr.find("$time");
	if (pos != std::string::npos)
		echoStr = boost::replace_all_copy(echoStr, "$time", timeStr.str());
	
    SmartBody::util::log("%s ", echoStr.c_str() );
	return( CMD_SUCCESS );
}

int sb_main_func( srArgBuffer & args, SmartBody::SBScene& scene)
{
   
   const char * token = args.read_token();
   if ( strcmp( token, "id" ) == 0 )
   {  // Process specific
      token = args.read_token(); // Process id
	  const char * process_id = scene.getProcessId().c_str();
      if( ( scene.getProcessId().empty() )         // If process id unassigned
         || strcmp( token, process_id ) !=0 ) // or doesn't match
         return CMD_SUCCESS;                  // Ignore.
      token = args.read_token(); // Sub-command
   }

   const char * args_raw = args.read_remainder_raw();
   std::stringstream strstr;
   strstr << token;
   if (args_raw)
	   strstr << " " << args_raw;
   bool resultOk = scene.run( strstr.str() );
   if (resultOk)
   {
	   return CMD_SUCCESS;
   }
   else
   {
	     SmartBody::util::log( "Python error: command FAILED: '%s %s'> ", token, args_raw );
		 return  CMD_FAILURE;
   }

}

int sbm_main_func( srArgBuffer & args, CommandContext commandContext)
{
   const char * token = args.read_token();
   if ( strcmp( token, "id" ) == 0 )
   {  // Process specific
      token = args.read_token(); // Process id
      const char * process_id = commandContext.scene.getProcessId().c_str();
      if( ( commandContext.scene.getProcessId().empty() )         // If process id unassigned
         || strcmp( token, process_id ) !=0 ) // or doesn't match
         return CMD_SUCCESS;                  // Ignore.
      token = args.read_token(); // Sub-command
   }

   const char * args_raw = args.read_remainder_raw();
   srArgBuffer arg_buf( args_raw );
   int result = commandContext.commandManager.execute( token, arg_buf );
   switch( result )
   {
      case CMD_NOT_FOUND:
         SmartBody::util::log( "SmartBody error: command NOT FOUND: '%s %s'> ", token, args_raw );
         break;
      case CMD_FAILURE:
         SmartBody::util::log( "SmartBody error: command FAILED: '%s %s'> ", token, args_raw );
         break;
      case CMD_SUCCESS:
         break;
      default:
         break;
   }

   return CMD_SUCCESS;
}

int sbm_vhmsg_send_func( srArgBuffer& args, SmartBody::SBVHMsgManager& vhmsgManager)
{
	char* cmdName = args.read_token();
	char* cmdArgs = args.read_remainder_raw();
	return vhmsgManager.send2( cmdName, cmdArgs );
}

int mcu_triggerevent_func(srArgBuffer& args, SmartBody::SBScene& scene)
{
	SmartBody::SBEventManager* eventManager = scene.getEventManager();

	char* eventName = args.read_token();
	char* eventParameters = args.read_remainder_raw();
	SmartBody::SBEvent* e = eventManager->createEvent(eventName, eventParameters, "");
	eventManager->handleEvent(e);
	delete e;

	SmartBody::util::log("TRIGGERING EVENT %s %s AT TIME %f", eventName, eventParameters, scene.getSimulationManager()->getTime());
	return CMD_SUCCESS;
}

