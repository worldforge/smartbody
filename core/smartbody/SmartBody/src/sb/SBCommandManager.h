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

#ifndef _SBCOMMANDMANAGER_H_
#define _SBCOMMANDMANAGER_H_

#include <sb/SBObject.h>
#include <sbm/sr_cmd_map.h>
#include <sbm/sr_cmd_seq.h>

#include <functional>
#include "SBSceneOwned.h"


class SequenceManager
{
	public:
		SBAPI SequenceManager();
		SBAPI ~SequenceManager();

		SBAPI bool addSequence(const std::string& seqName, srCmdSeq* seq);
		SBAPI bool removeSequence(const std::string& seqName, bool deleteSequence);
		SBAPI srCmdSeq* getSequence(const std::string& name);
		SBAPI srCmdSeq* getSequence(int num, std::string& name);
		SBAPI int getNumSequences();
		SBAPI void cleanupMarkedSequences();

		SBAPI void clear();

	protected:
		std::set<std::string> _sequenceSet;
		std::vector<std::pair<std::string, srCmdSeq*> > _sequences;
};


namespace SmartBody {

class SBCommandManager : public SBObject, public SBSceneOwned
{
	public:
		typedef std::function<int(srArgBuffer&)> Callback;
		SBAPI explicit SBCommandManager(SBScene& scene);
		SBAPI ~SBCommandManager();

		SBAPI void registerCallbacks();

		bool hasCommand(const std::string& command);

		SBAPI int execute( const char *key, srArgBuffer& args );
		SBAPI int execute( const char *key, char* strArgs );
		SBAPI int execute( char *cmd );
		SBAPI int execute_seq( srCmdSeq *seq );
		SBAPI int execute_seq( srCmdSeq *seq, const char* seq_name );
		SBAPI int execute_seq_chain( const std::vector<std::string>& seq_names, const char* error_prefix = nullptr );
		SBAPI int execute_later( const char* command, float seconds );
		SBAPI int execute_later( const char* command );
		SBAPI srCmdSeq* lookup_seq( const char* );
		SBAPI int abortSequence( const char* name );
		SBAPI int deleteSequence( const char* name );

		SBAPI int insert_set_cmd( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp );
		SBAPI int insert_set_cmd( std::string key, Callback callback );
		SBAPI int insert_print_cmd( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp );
		SBAPI int insert_print_cmd( std::string key, Callback callback );
		SBAPI int insert_test_cmd( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp );
		SBAPI int insert_test_cmd( std::string key, Callback callback );
		SBAPI int insert( const char *key, srCmdMap<SBCommandManager>::sr_cmd_callback_fp fp, char* description = nullptr );
		SBAPI int insert( std::string key, Callback callback );

		SBAPI SequenceManager* getPendingSequences();
		SBAPI SequenceManager* getActiveSequences();

//		/*! Executes a variable setting sub-command.   See mcuCBHandle::insert_set_cmd(..). */
//		static int mcu_set_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
//		/*! Executes a variable printing/data debugging sub-command.   See mcuCBHandle::insert_print_cmd(..). */
//		static int mcu_print_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );
//		/*! Executes a test sub-command.   See mcuCBHandle::insert_test_cmd(..). */
//		static int mcu_test_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr );


	protected:

		SequenceManager	pendingSequences;
		SequenceManager activeSequences;

		srCmdMap<SBCommandManager>		set_cmd_map;
		srCmdMap<SBCommandManager>		print_cmd_map;
		srCmdMap<SBCommandManager>		test_cmd_map;

	std::map<std::string, Callback> _cmd_map;
	std::map<std::string, Callback> _set_cmd_map;
	std::map<std::string, Callback> _print_cmd_map;
	std::map<std::string, Callback> _test_cmd_map;
};

}

#endif