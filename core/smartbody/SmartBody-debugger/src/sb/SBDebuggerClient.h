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

#ifndef SBM_DEBUGGER_CLIENT_H_
#define SBM_DEBUGGER_CLIENT_H_

#include <sb/SBTypes.h>
#include <vhcl_socket.h>
#include <string>
#include <vector>
#include <sb/SBScene.h>
#include "SBDebuggerUtility.h"

#include "sb/NetRequest.h"

namespace SmartBody {

class SBDebuggerClient
{
private:
   //vhmsg::Client m_vhmsg;
   std::vector<std::string> m_processIdList;
   std::string m_sbmId;
   bool m_connectResult;
   bool m_initFinish;
   vhcl::socket_t m_sockTCP_client;

   NetRequestManager m_netRequestManager;

   SBDebuggerUtility m_debuggerUtility;

public:
   SBAPI SBDebuggerClient();
   SBAPI virtual ~SBDebuggerClient();

   SBAPI void QuerySbmProcessIds();
   SBAPI const std::vector<std::string> & GetSbmProcessIds() const { return m_processIdList; }

   SBAPI void Connect(const std::string & id);
   SBAPI void Disconnect();
   SBAPI bool GetConnectResult() { return m_connectResult; }

   SBAPI void Init();
   SBAPI void Update();
   SBAPI void StartUpdates(double updateFrequencyS);
   SBAPI void EndUpdates();

   void attachToScene(SBScene& scene);

   SBAPI void SendSBMCommand(int requestId, const std::string & command);
   SBAPI void SendSBMCommand(int requestId, const std::string & returnValue, const std::string & functionNameandParams,
      NetRequest::RequestCallback cb, void* callbackOwner = nullptr);

   SBAPI void ProcessVHMsgs(const char * op, const char * args);
};

}


#endif
