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

#ifndef _NVBG_H
#define _NVBG_H

#include <sb/SBObject.h>
#include <string>

namespace SmartBody 
{

class Nvbg : public SmartBody::SBObject
{
public:
		Nvbg();
		virtual ~Nvbg();

		virtual void objectEvent(std::string character, std::string name, bool isAnimate, SrVec charPosition, SrVec charVelocity, SrVec objPosition, SrVec objVelocity, SrVec relativePosition, SrVec relativeVelocity);
		virtual bool execute(std::string character, std::string to, std::string messageId, std::string xml);
		virtual bool executeEvent(std::string character, std::string messageId, std::string state);
		virtual bool executeSpeech(std::string character, std::string speechStatus, std::string speechId, std::string speaker);
		virtual bool executeSpeechRequest(std::vector<std::string> behaviors, std::vector<std::string> types, std::vector<float> times, std::vector<std::string> targets, std::vector<std::string> info);
		void notify(SmartBody::SBSubject* subject) override;
		virtual void notifyLocal(std::string name);

		virtual void notifyAction(std::string name);
		virtual void notifyBool(std::string name, bool val);
		virtual void notifyInt(std::string name, int val);
		virtual void notifyDouble(std::string name, double val);
		virtual void notifyString(std::string name, std::string val);
		virtual void notifyVec3(std::string name, SrVec val);
		virtual void notifyMatrix(std::string name, SrMat val);

};

}

#endif