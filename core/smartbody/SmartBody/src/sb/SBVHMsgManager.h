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

#ifndef _SBVHMSGMANAGER_H_
#define _SBVHMSGMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include "SBUtilities.h"
#include <string>

namespace vhcl {
	namespace Log {
		class Listener;
	}
}

namespace SmartBody {

class SBVHMsgManager : public SBService
{
	public:
		SBAPI SBVHMsgManager();
		SBAPI ~SBVHMsgManager();

		SBAPI virtual void setEnable(bool val);
		SBAPI virtual bool isEnable();

		SBAPI bool isConnected();
		SBAPI bool connect();
		SBAPI void disconnect();

		SBAPI void stop();

		SBAPI int sendOpMessage(const std::string& op, const std::string& message);
		SBAPI int sendMessage(const std::string& message);
		SBAPI int send2(const char *op, const char* message);
		SBAPI int send(const char* message);

		SBAPI int poll();

		SBAPI void setPort(const std::string& port);
		SBAPI const std::string& getPort();
		SBAPI void setServer(const std::string& server);
		SBAPI const std::string& getServer();
		SBAPI void setScope(const std::string& scope);
		SBAPI const std::string& getScope();

		SBAPI virtual void setEnableLogging(bool val);
		SBAPI virtual bool isEnableLogging();

		void onCharacterDelete(SBCharacter* character) override;



	protected:
		static void vhmsgCallback( const char *op, const char *args, void * user_data );

		std::string _port;
		std::string _server;
		std::string _scope;
		SmartBody::util::Listener* _logListener;
};

}

#endif