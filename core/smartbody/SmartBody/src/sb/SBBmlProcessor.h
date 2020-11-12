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

#ifndef _SBBMLPROCESSOR_H
#define _SBBMLPROCESSOR_H

#include <sb/SBTypes.h>
#include <string>
#include <ostream>
#include <sb/SBObject.h>

namespace BML {
	class Processor;
}

class BMLObject;

namespace SmartBody {

class SBBMLBlock : public SBObserver
{
	public:
		SBAPI SBBMLBlock();
		SBAPI ~SBBMLBlock();

		SBAPI virtual void notify(SBSubject* subject);

};

class SBTrigger : public SBSubject
{
	public:
		SBTrigger();
		~SBTrigger();

		void setOffset(double val);
		double getOffset();

		void setStartTag(std::string tag);
		std::string getStartTag();

		void setEndTag(std::string tag);
		std::string getEndTag();


	protected:
		std::string _name;
		double _offset;
		std::string _startTag;
		std::string _endTag;
};

class SBSyncPoint : public SBSubject
{
	public:
		SBSyncPoint(const std::string& name);
		~SBSyncPoint();

	protected:
		std::string _name;
};

class SBBMLSchedule
{
	public:
		SBAPI SBBMLSchedule();
		SBAPI ~SBBMLSchedule();

		SBAPI void setCharacter(std::string name);
		SBAPI std::string getCharacter();
		SBAPI void remove();

	protected:
		std::string _characterName;
};


class SBBmlProcessor
{
	public:
		SBAPI SBBmlProcessor();
		SBAPI ~SBBmlProcessor();

		SBAPI void vrSpeak(std::string agent, std::string recip, std::string msgId, std::string msg);
		SBAPI void vrAgentBML(std::string op, std::string agent, std::string msgId, std::string msg);
		
		SBAPI std::string execBML(std::string character, std::string bml);
		SBAPI std::string execBMLFile(std::string character, std::string filename);
		SBAPI std::string execBMLFileId(std::string character, std::string filename, std::string id);
		SBAPI std::string execXML(std::string character, std::string xml);
		SBAPI std::string execXMLFile(std::string character, std::string filename);
		SBAPI std::string execXMLFileId(std::string character, std::string filename, std::string id);
		SBAPI void execBMLAt(double time, std::string character, std::string bml);
		SBAPI void execBMLFileAt(double time, std::string character, std::string filename);
		SBAPI void execXMLAt(double time, std::string character, std::string xml);

		SBAPI void interruptCharacter(const std::string& character, double seconds);
		SBAPI void interruptBML(const std::string& character, const std::string& id, double seconds);

		SBAPI BML::Processor* getBMLProcessor();

	protected:
		std::vector<BMLObject*> parseBML(const std::string& bml);
		void scheduleBML(std::vector<BMLObject*>& behaviors);
		void processBML(double time);

		std::string build_vrX(std::ostringstream& buffer, const std::string& cmd, const std::string& char_id, const std::string& recip_id, const std::string& content, bool for_seq, std::string candidateMsgId = "" );
		std::string send_vrX( const char* cmd, const std::string& char_id, const std::string& recip_id,
			const std::string& seq_id, bool echo, bool send, const std::string& bml, std::string candidateMsgId = "");

		BML::Processor* _bmlProcessor;
		std::map<std::string, BMLObject*> _bmlHandlers;
		std::map<std::string, SBBMLSchedule*> _bmlSchedules;
};

}

#endif