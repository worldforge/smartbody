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

#include "SBCommandManager.h"
#include <sb/SBScene.h>
#include <sb/SBAttribute.h>
#include <sb/SBAssetManager.h>
#include <sb/SBSimulationManager.h>
#include "SBUtilities.h"
#include <sstream>
#include <sbm/sbm_test_cmds.hpp>
#include <sbm/text_speech.h>
#include <sbm/local_speech.h>


SequenceManager::SequenceManager() = default;

SequenceManager::~SequenceManager()
{
	clear();
}

void SequenceManager::clear()
{
	for (auto & _sequence : _sequences)
	{
		delete _sequence.second;
	}

	_sequenceSet.clear();
	_sequences.clear();
}

bool SequenceManager::addSequence(const std::string& seqName, srCmdSeq* seq)
{
	if (_sequenceSet.find(seqName) != _sequenceSet.end())
		return false;

	_sequenceSet.insert(seqName);
	_sequences.emplace_back(std::pair<std::string, srCmdSeq*>(seqName, seq));
	return true;
}

bool SequenceManager::removeSequence(const std::string& seqName, bool deleteSequence)
{
	auto iter = _sequenceSet.find(seqName);
	if (iter == _sequenceSet.end())
		return false;

	_sequenceSet.erase(iter);

	for (auto & _sequence : _sequences)
	{
		if (_sequence.first == seqName)
		{
			/*
			if (deleteSequence)
				delete (*iter).second;
			_sequences.erase(iter);
			*/
			// need to mark the sequences as invalid for later removal
			_sequence.second->setValid(false);

			return true;
		}
	}

	SmartBody::util::log("Could not find sequence in active sequence queue. Please check code - this should not happen.");
	return false;
}

srCmdSeq* SequenceManager::getSequence(const std::string& name)
{
	for (auto & _sequence : _sequences)
	{
		if (_sequence.first == name)
		{
			return _sequence.second;
		}
	}

	return nullptr;
}

srCmdSeq* SequenceManager::getSequence(int num, std::string& name)
{
	if (num < (int) _sequences.size())
	{
		name = _sequences[num].first;
		return _sequences[num].second;
	}
	else
	{
		return nullptr;
	}
}

int SequenceManager::getNumSequences()
{
	return _sequences.size();
}

void SequenceManager::cleanupMarkedSequences()
{
	bool hasInvalidSequences = true;
	while (hasInvalidSequences)
	{
		hasInvalidSequences = false;
		for (auto iter = _sequences.begin();
			iter != _sequences.end();
			iter++)
		{
			if (!(*iter).second->isValid())
			{
				srCmdSeq* seq = (*iter).second;
				delete seq;
				_sequences.erase(iter);
				hasInvalidSequences = true;
				break;
			}
		}
	}
}




namespace SmartBody {



SBCommandManager::SBCommandManager(SBScene& scene) : SBSceneOwned(scene)
{
	registerCallbacks();
}

SBCommandManager::~SBCommandManager() = default;

bool SBCommandManager::hasCommand(const std::string& command)
{
	return _cmd_map.find(command) != _cmd_map.end();
}



int SBCommandManager::insert_set_cmd( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp )
{
	return( set_cmd_map.insert( key, fp ) );
}

int SBCommandManager::insert_set_cmd( std::string key, Callback callback )
{
	auto result = _set_cmd_map.emplace(std::move(key), std::move(callback));
	return result.second ? 1 : 0;}

int SBCommandManager::insert_print_cmd( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp )
{
	return( print_cmd_map.insert( key, fp ) );
}

int SBCommandManager::insert_print_cmd( std::string key, Callback callback )
{
	auto result = _print_cmd_map.emplace(std::move(key), std::move(callback));
	return result.second ? 1 : 0;}

int SBCommandManager::insert_test_cmd( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp )
{
	return( test_cmd_map.insert( key, fp ) );
}

int SBCommandManager::insert_test_cmd( std::string key, Callback callback )
{
	auto result = _test_cmd_map.emplace(std::move(key), std::move(callback));
	return result.second ? 1 : 0;}

int SBCommandManager::insert( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp, char* description )
{
	auto func = [=](srArgBuffer& args) {return fp(args, this);};
	return insert(key, func);
}

int SBCommandManager::insert( std::string key, Callback callback )
{
	auto result = _cmd_map.emplace(std::move(key), std::move(callback));
	return result.second ? 1 : 0;
}

void SBCommandManager::registerCallbacks()
{


	//  cmd prefixes "set" and "print"
	insert("set", [this](srArgBuffer& args) {
			   auto arg = args.read_token();
			   if (arg) {
				   auto I = _set_cmd_map.find(arg);
				   if (I != _set_cmd_map.end()) {
					   auto result = I->second(args);
					   if (result == CMD_NOT_FOUND) {
						   SmartBody::util::log("SmartBody error: Unknown Variable, Cannot set: '%s'\n> ", arg);  // Clarify this as a set command error
						   return CMD_SUCCESS; // Avoid multiple error messages
					   } else {
						   return result;
					   }
				   }
			   }
			   return CMD_NOT_FOUND;
		   }
	);
	insert("print", [this](srArgBuffer& args) {
			   auto arg = args.read_token();
			   if (arg) {
				   auto I = _print_cmd_map.find(arg);
				   if (I != _print_cmd_map.end()) {
					   auto result = I->second(args);
					   if (result == CMD_NOT_FOUND) {
						   SmartBody::util::log("SmartBody error: Print command NOT FOUND: '%s'\n> ", arg );  // Clarify this as a print command error
						   return CMD_SUCCESS; // Avoid multiple error messages
					   } else {
						   return result;
					   }
				   }
			   }
			   return CMD_NOT_FOUND;
		   }
	);
	insert("test", [this](srArgBuffer& args) {
			   auto arg = args.read_token();
			   if (arg) {
				   auto I = _test_cmd_map.find(arg);
				   if (I != _test_cmd_map.end()) {
					   auto result = I->second(args);
					   if (result == CMD_NOT_FOUND) {
						   SmartBody::util::log("SmartBody error: Test command NOT FOUND: '%s'\n> ", arg );  // Clarify this as a test command error
						   return CMD_SUCCESS; // Avoid multiple error messages
					   } else {
						   return result;
					   }
				   }
			   }
			   return CMD_NOT_FOUND;
		   }
	);





//	insert( "locomotion",          locomotion_cmd_func );
//	insert( "loco",                locomotion_cmd_func ); // shorthand
	insert( "bml",				   test_bml_func );

#ifdef USE_GOOGLE_PROFILER
	insert( "startprofile",			   startprofile_func );
	insert( "stopprofile",			   stopprofile_func );
	insert( "startheapprofile",			   startheapprofile_func );
	insert( "stopheapprofile",			   stopheapprofile_func );
#endif

	

	insert_test_cmd( "bml",  test_bml_func );
	insert_test_cmd( "fml",  test_fml_func );
	insert_test_cmd( "bone_pos", test_bone_pos_func );
	



	/*
	// deprecated callbacks
	insert( "help",					deprecatedMessage);
	insert( "reset",				deprecatedMessage );
	insert( "path",					deprecatedMessage );
	insert( "physics",				deprecatedMessage );	
	insert( "mirror",				deprecatedMessage);
	insert( "syncpolicy",          deprecatedMessage );
	insert( "adjustmotion",		   deprecatedMessage);
	insert( "mediapath",		   deprecatedMessage);
	insert( "addevent",			   deprecatedMessage );
	insert( "removeevent",		   deprecatedMessage );
	insert( "enableevents",	       deprecatedMessage );
	insert( "disableevents",	   deprecatedMessage );
	insert( "registerevent",       deprecatedMessage );
	insert( "unregisterevent",     deprecatedMessage );
	insert( "setmap",			   deprecatedMessage );
	insert( "motionmap",		   deprecatedMessage );
	insert( "motionmapdir",		   deprecatedMessage );
	insert( "skeletonmap",		   deprecatedMessage );
	insert( "characters",		   deprecatedMessage );
	insert( "pawns",			   deprecatedMessage );
	insert( "resetanimation",	   deprecatedMessage);
	insert( "animation",		   deprecatedMessage);
	insert( "skscale",			   deprecatedMessage);
	insert( "skmscale",			   deprecatedMessage);
	insert( "xmlcachedir",		   deprecatedMessage);
	insert( "xmlcache",			   deprecatedMessage);
	*/
}

int SBCommandManager::execute( const char *key, srArgBuffer& args )
{ 
//	std::stringstream strstr;
//	strstr << key << " " << args.peek_string();
			
	auto I = _cmd_map.find(key);
	if (I != _cmd_map.end()) {
		return I->second(args);
	} else {
		return 0;
	}
}

int SBCommandManager::execute( const char *key, char* strArgs )
{
	std::stringstream strstr;
	strstr << key << " " << strArgs;

    srArgBuffer args( strArgs );
    return execute(key, args);
}

int SBCommandManager::execute( char *cmd )
{ 
//	//SmartBody::util::log("execute cmd = %s\n",cmd);
//	// check to see if this is a sequence command
//	// if so, save the command id
//	std::string checkCmd = cmd;
//	size_t startpos = checkCmd.find_first_not_of(" \t");
//	if( std::string::npos != startpos )
//		checkCmd = checkCmd.substr( startpos );

	srArgBuffer argb( cmd );
	char *key = argb.read_token();
	return execute( key, argb);

}

int SBCommandManager::execute_seq( srCmdSeq *seq )
{
	std::ostringstream seq_id;
	SmartBody::IntAttribute* intAttr = dynamic_cast<SmartBody::IntAttribute*>(_scene.getAttribute("queuedCommandsIndex"));
	seq_id << "execute_seq-" << (intAttr->getValue());
	intAttr->setValue(intAttr->getValue() + 1);

	return execute_seq( seq, seq_id.str().c_str() );
}

int SBCommandManager::execute_seq( srCmdSeq *seq, const char* seq_name )
{
	
//	printf( "mcuCBHandle::execute_seq: id: '%s'\n", seq_id );
//	seq_p->print();

	if ( !activeSequences.addSequence( seq_name, seq ) ) {
		SmartBody::util::log("ERROR: mcuCBHandle::execute_seq(..): Failed to insert srCmdSeq \"%s\"into active_seq_map.", seq_name );
		return CMD_FAILURE;
	}	

	return CMD_SUCCESS;
}

int SBCommandManager::execute_seq_chain( const std::vector<std::string>& seq_names, const char* error_prefix )
{
	auto it  = seq_names.begin();
	auto end = seq_names.end();

	if( it == end ) {
		// No sequences -> NOOP
		return CMD_SUCCESS;
	}

	const std::string& first_seq_name = *it;  // convenience reference
	std::string fullPath;
	FILE* first_file_p = _scene.getAssetManager()->open_sequence_file( first_seq_name.c_str(), fullPath );
	if( first_file_p == nullptr ) {
		if( error_prefix )
			SmartBody::util::log("%s Cannot find sequence \"%s\". Aborting seq-chain.", error_prefix, first_seq_name.c_str());
		return CMD_FAILURE;
	}

	srCmdSeq* seq_p = new srCmdSeq();
	int parse_result = seq_p->read_file( first_file_p );
	fclose( first_file_p );
	if( parse_result != CMD_SUCCESS ) {
		if( error_prefix )
			SmartBody::util::log("%s Unable to parse sequence\"%s\".", error_prefix, first_seq_name.c_str());

		delete seq_p;
		seq_p = nullptr;

		return CMD_FAILURE;
	}

	// Test remaining sequence names, error early if invalid
	auto second = ++it;
	for( ; it != end; ++it ) {
		const std::string& next_seq = *it;  // convenience reference

		std::string fullPath;
		FILE* file = _scene.getAssetManager()->open_sequence_file( next_seq.c_str(), fullPath );
		if( file == nullptr ) {
			if( error_prefix )
				SmartBody::util::log("%s Cannot find sequence \"%s\". Aborting seq-chain.", error_prefix, next_seq.c_str() );
			return CMD_FAILURE;
		} else {
			fclose( file );
		}
	}

	if( second != end ) {  // has more than one seq_name
		// Append new seq-chian command of remaining seq_names at end of seq_p
		float time = seq_p->duration();

		// Start from second
		it = second;

		// build command
		std::ostringstream oss;
		oss << "seq-chain";
		for( ; it != end; ++it )
			oss << ' ' << (*it);

		// insert command or error with cleanup
		int result = seq_p->insert( time, oss.str().c_str() );
		if( result != CMD_SUCCESS ) {
			if( error_prefix )
				SmartBody::util::log("%s Failed to insert seq-chain command at time %f", error_prefix, time);

			delete seq_p;
			seq_p = nullptr;

			return CMD_FAILURE;
		}
	}

	execute_seq( seq_p, first_seq_name.c_str() );

	return CMD_SUCCESS;
}


//  Schedule command in some seconds
int SBCommandManager::execute_later( const char* command, float seconds )
{
	srCmdSeq *temp_seq = new srCmdSeq();
	temp_seq->insert( (float)_scene.getSimulationManager()->getTime() +seconds, command );

	std::ostringstream seqName;
	SmartBody::IntAttribute* intAttr = dynamic_cast<SmartBody::IntAttribute*>(_scene.getAttribute("queuedCommandsIndex"));
	seqName << "execute_later-" << (intAttr->getValue());
	intAttr->setValue(intAttr->getValue() + 1);

	return execute_seq( temp_seq, seqName.str().c_str() );;
}

//  Queue command for next frame
int SBCommandManager::execute_later( const char* command )
{ 
	return( execute_later( command, 0 ) ); 
}


srCmdSeq* SBCommandManager::lookup_seq( const char* name )
{
	int err = CMD_FAILURE;
	
	// Remove previous activation of sequence.
	// Anm: Why?  Need clear distrinction (and definition) between pending and active.
	abortSequence( name );

	srCmdSeq* seq = pendingSequences.getSequence( name );
	if (seq)
	{
		pendingSequences.removeSequence( name, false );
	}
	else
	{
		// Sequence not found.  Load new instance from file.
		std::string fullPath;
		FILE* file = _scene.getAssetManager()->open_sequence_file( name, fullPath );
		if( file ) {
			seq = new srCmdSeq();
			err = seq->read_file( file );
			fclose( file );

			if( err != CMD_SUCCESS ) {
				SmartBody::util::log("ERROR: mcuCBHandle::lookup_seq(..): '%s' PARSE FAILED\n", name ); 

				delete seq;
				seq = nullptr;
			}
		} else {
			SmartBody::util::log("ERROR: mcuCBHandle::lookup_seq(..): '%s' NOT FOUND\n", name ); 
		}
	}
	
	return( seq );
}

int SBCommandManager::abortSequence( const char* name ) {
	srCmdSeq* seq = activeSequences.getSequence(name);
	if( !seq )
		return CMD_FAILURE;
	
	bool success = activeSequences.removeSequence( name, true );
	

	srCmdSeq* pending = pendingSequences.getSequence( name );
	if( pending )
		success = pendingSequences.removeSequence( name, true );

	return CMD_SUCCESS; 
}

int SBCommandManager::deleteSequence( const char* name ) {
	int result = abortSequence( name );

	srCmdSeq* seq = pendingSequences.getSequence( name );
	if( seq  )
	{
		pendingSequences.removeSequence( name, true );
		result = CMD_SUCCESS;
	}

	return result;
}

SequenceManager* SBCommandManager::getPendingSequences()
{
	return &pendingSequences;
}

SequenceManager* SBCommandManager::getActiveSequences()
{
	return &activeSequences;
}

//int SBCommandManager::mcu_set_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr ) {
//    char* arg = args.read_token();
//    int result = _scene.getCommandManager()->set_cmd_map.execute( arg, args, _scene.getCommandManager() );
//	if( result == CMD_NOT_FOUND ) {
//		// TODO: Differentiate between not finding this var and subargs
//		SmartBody::util::log("SmartBody error: Unknown Variable, Cannot set: '%s'\n> ", arg );  // Clarify this as a set command error
//		return CMD_SUCCESS; // Avoid multiple error messages
//	} else {
//		return result;
//	}
//}
//
///*
//	Executes a command to print to the console some debug data.
//	See insert_print_cmd and its call in main
//*/
//
//int SBCommandManager::mcu_print_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr ) {
//    char* arg = args.read_token();
//    int result = _scene.getCommandManager()->print_cmd_map.execute( arg, args, _scene.getCommandManager() );
//	if( result == CMD_NOT_FOUND ) {
//		// TODO: Differentiate between not finding this var and subargs
//		SmartBody::util::log("SmartBody error: Print command NOT FOUND: '%s'\n> ", arg );  // Clarify this as a print command error
//		return CMD_SUCCESS; // Avoid multiple error messages
//	} else {
//		return result;
//	}
//}
//
///*
//	Executes a test sub-command.
//	See insert_test_cmd and its call in main
//*/
//
//int SBCommandManager::mcu_test_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr ) {
//    char* arg = args.read_token();
//    int result = _scene.getCommandManager()->test_cmd_map.execute( arg, args, _scene.getCommandManager() );
//	if( result == CMD_NOT_FOUND ) {
//		SmartBody::util::log("SmartBody error: Test command NOT FOUND: '%s'\n> ", arg );  // Clarify this as a test command error
//		return CMD_SUCCESS; // Avoid multiple error messages
//	} else {
//		return result;
//	}
//}

}
