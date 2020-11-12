#ifndef SBM_DEBUGGER_CLIENT_H_
#define SBM_DEBUGGER_CLIENT_H_
#include <string>
#include <vector>

#include "SBDebuggerCommon.h"
#include "NetRequest.h"

class SBDebuggerClient
{
public:
   typedef void (*OnClose)(void* data);

private:
   //vhmsg::Client m_vhmsg;
   std::vector<std::string> m_processIdList;
   std::string m_sbmId;
   bool m_connectResult;

   Scene m_scene;
   NetRequestManager m_netRequestManager;
   OnClose m_OnCloseCB;
   void* m_pCloseCBData;

public:
   SBDebuggerClient();
   virtual ~SBDebuggerClient();

   Scene* GetScene() { return &m_scene; }

   void QuerySbmProcessIds();
   const std::vector<std::string> & GetSbmProcessIds() const { return m_processIdList; }

   void Connect(const std::string & id);
   void Disconnect();
   bool GetConnectResult() { return m_connectResult; }

   void Init();
   void Update();
   void StartUpdates(double updateFrequencyS);
   void EndUpdates();
   void AddOnCloseCallback(OnClose onClose, void* closeData) { m_OnCloseCB = onClose; m_pCloseCBData = closeData; }

   void GetResourcePaths();

   void SendSBMCommand(int requestId, const std::string & command);
   void SendSBMCommand(int requestId, const std::string & returnValue, const std::string & functionNameandParams,
      NetRequest::RequestCallback cb, void* callbackOwner = NULL);

   void ProcessVHMsgs(const char * op, const char * args);
};

#endif