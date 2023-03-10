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

#include "vhcl.h"
#include "vhcl_socket.h"

#include "SBDebuggerClient.h"
#include "sb/SBSimulationManager.h"
#include "SBUtilities.h"
#include "sb/SBSceneListener.h"
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>

#include "vhmsg-tt.h"

#include <sb/SBScene.h>

using std::string;
using std::vector;

namespace SmartBody {

SBDebuggerClient::SBDebuggerClient(SmartBody::SBScene& scene)
: SBSceneOwned(scene), m_debuggerUtility(*this)
{
	m_sockTCP_client = 0;
	m_initFinish = false;
}

SBDebuggerClient::~SBDebuggerClient() = default;

void SBDebuggerClient::QuerySbmProcessIds()
{
   m_processIdList.clear();
   vhmsg::ttu_notify1("sbmdebugger queryids");
}





void SBDebuggerClient::Connect(const string & id)
{
   m_sbmId = id;
   m_connectResult = false;
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s connect", m_sbmId.c_str()).c_str());

   // id == host:port:friendlyName
   vector<string> idSplit;
   SmartBody::util::tokenize(id, idSplit, ":");
   const char * server = idSplit[0].c_str();
   int port = SmartBody::util::toInt(idSplit[1]);


   bool ret = vhcl::SocketStartup();
   if (!ret)
   {
      printf("SocketStartup() failed\n");
   }


   m_sockTCP_client = vhcl::SocketOpenTcp();
#ifdef WIN32
   if (m_sockTCP_client == nullptr)
#else
   if (m_sockTCP_client == -1)
#endif
   {
      printf( "SocketOpenTcp() failed\n" );
      vhcl::SocketShutdown();
      return;
   }

   ret = vhcl::SocketConnect(m_sockTCP_client, server, port);
   if (!ret)
   {
      printf( "SocketConnect() failed\n" );
      vhcl::SocketClose(m_sockTCP_client);
      vhcl::SocketShutdown();
      return;
   }
}

void SBDebuggerClient::Disconnect()
{
   if ( m_sockTCP_client )
   {
      vhcl::SocketClose(m_sockTCP_client);
      m_sockTCP_client = 0;
   }

   vhcl::SocketShutdown();


   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s disconnect", m_sbmId.c_str()).c_str());
   m_processIdList.clear();
   m_sbmId = "";
   m_connectResult = false;
}

void SBDebuggerClient::Init()
{
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s send_init", m_sbmId.c_str()).c_str());
}






string m_tcpData;
int m_tcpDataCount = 0;

void SBDebuggerClient::Update()
{	
   //return;
   {
      vhcl::socket_t s = m_sockTCP_client;

      int tcpDataPending;
      tcpDataPending = vhcl::SocketIsDataPending(s);

      while ( tcpDataPending )
      {
         tcpDataPending = 0;

         // TODO - preallocate this
         char str[ 1000 ] = { 0 };

         int bytesReceived = vhcl::SocketReceive(s, str, sizeof( str ) - 1);
         if ( bytesReceived > 0 )
         {
            m_tcpData.append(str);
            m_tcpDataCount += m_tcpData.length();           

            tcpDataPending = vhcl::SocketIsDataPending(s);

			//SmartBody::util::log("TCP Data Count %d, tcpDataPending = %d\n", m_tcpDataCount, tcpDataPending);
         }

         // sanity check for spamming too much data
         if (m_tcpData.length() > 500000)
         {
            static int i = 0;
            SmartBody::util::log("TCP Overflow %d, length = %d\n", i++,m_tcpData.length());
            tcpDataPending = 0;
         }
      }


      // if m_tcpData contains a ;, then we have a full command, extract it and process it

      size_t marker = m_tcpData.find_first_of(';');
      while (marker != string::npos)
      {
         string command = m_tcpData.substr(0, marker);
         m_tcpData.erase(0, marker + 1);


         vector< string > msgTokens;
         vhcl::Tokenize( command, msgTokens, "|" );

         vector<string> split;
         vhcl::Tokenize(command, split, " \t\n");

         if ( msgTokens.size() > 0 )
         {
            if ( msgTokens[ 0 ] == "CreateActor" )
            {
               if ( msgTokens.size() > 4 )
               {
                  string charIdStr = msgTokens[ 1 ];
                  int charId       = atoi( charIdStr.c_str() );
                  string uClass    = msgTokens[ 2 ];
                  string name      = msgTokens[ 3 ];
                  int skeletonType = atoi( msgTokens[ 4 ].c_str() );

                  //if ( m_onCreateCharacterFunc )
                  {
                     //m_onCreateCharacterFunc( charId, uClass, name, skeletonType, m_onClientConnectUserData );
                  }
               }
            }
            else if (split[2] == "update" && m_initFinish)
            {
               // handle updates
               if (split.size() > 3)
               {
                  if (split[3] == "character")
                  {
                     if (split.size() > 4)
                     {
                        string character_string = split[3];
                        string name = split[4];
                     }

                    if (split[5] == "bones")
                       {
                          if (split.size() > 6)
                          {
                             int numBones = vhcl::ToInt(split[6]);
                          }
// 						  if (split[4] == "ChrBackovicPrefab")						  
// 						  {							  
// 							  static double prevTime = 0.0;
// 							  float dt = _scene.getSimulationManager()->getTime() - prevTime;
// 							  SmartBody::util::log("Update ChrBackovic, time = %f, num bones = %d", dt,split.size() - 7);
// 							  prevTime = _scene.getSimulationManager()->getTime();
// 						  }
                          for (size_t i = 7; i < split.size(); i++)
                          {
							 if (i +9 <= split.size() - 1)
							 {
								 string name = split[i];
								 string pos  = split[i + 1];
								 float posX = SmartBody::util::toFloat(split[i + 2]);
								 float posY = SmartBody::util::toFloat(split[i + 3]);
								 float posZ = SmartBody::util::toFloat(split[i + 4]);
								 string rot  = split[i + 5];
								 float rotX = SmartBody::util::toFloat(split[i + 6]);
								 float rotY = SmartBody::util::toFloat(split[i + 7]);
								 float rotZ = SmartBody::util::toFloat(split[i + 8]);
								 float rotW = SmartBody::util::toFloat(split[i + 9]);
								 i += 9;

								m_debuggerUtility.updateCharacter(split[4], name, posX, posY, posZ, rotX, rotY, rotZ, rotW);
							 }
						  }
                       }
                     }
                  else if (split[3] == "pawn")
                  {
					// sbmdebugger <sbmid> update pawn <name> pos <x y z> rot <x y z w> geom <s> size <s> 

						float posX = SmartBody::util::toFloat(split[6]);
						float posY = SmartBody::util::toFloat(split[7]);
						float posZ = SmartBody::util::toFloat(split[8]);
						float rotX = SmartBody::util::toFloat(split[10]);
						float rotY = SmartBody::util::toFloat(split[11]);
						float rotZ = SmartBody::util::toFloat(split[12]);
						float rotW = SmartBody::util::toFloat(split[13]);
					  m_debuggerUtility.updatePawn(split[4], posX, posY, posZ, rotX, rotY, rotZ, rotW);
                  }
                  else if (split[3] == "camera")
                  {
                     int i = 4;
                     string pos  = split[i];
                     float posX = SmartBody::util::toFloat(split[i + 1]);
                     float posY = SmartBody::util::toFloat(split[i + 2]);
                     float posZ = SmartBody::util::toFloat(split[i + 3]);
                     string rot  = split[i + 4];
                     float rotX = SmartBody::util::toFloat(split[i + 5]);
                     float rotY = SmartBody::util::toFloat(split[i + 6]);
                     float rotZ = SmartBody::util::toFloat(split[i + 7]);
                     float rotW = SmartBody::util::toFloat(split[i + 8]);
                     string persp  = split[i + 9];
                     float fovY   = SmartBody::util::toFloat(split[i + 10]);
                     float aspect = SmartBody::util::toFloat(split[i + 11]);
                     float zNear  = SmartBody::util::toFloat(split[i + 12]);
                     float zFar   = SmartBody::util::toFloat(split[i + 13]);
					       string center  = split[i + 14];
					       float centerX = SmartBody::util::toFloat(split[i + 15]);
					       float centerY = SmartBody::util::toFloat(split[i + 16]);
					       float centerZ = SmartBody::util::toFloat(split[i + 17]);
					  m_debuggerUtility.updateCamera(posX, posY, posZ, centerX, centerY, centerZ, fovY, aspect, zNear, zFar);
                  }
               }


               //printf("Update Received\n");
               //printf(message.c_str());
               //printf("\n");
            }

         }

         marker = m_tcpData.find_first_of(';');
      }
	}
}

void SBDebuggerClient::StartUpdates(double updateFrequencyS)
{
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s start_update %f", m_sbmId.c_str(), updateFrequencyS).c_str());
}

void SBDebuggerClient::EndUpdates()
{
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s end_update", m_sbmId.c_str()).c_str());
}

void SBDebuggerClient::SendSBMCommand(int requestId, const std::string & command)
{
   // send a void command, not expecting a return
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s %d request void \"ret = %s\"", m_sbmId.c_str(), 42, command.c_str()).c_str());

   m_netRequestManager.CreateNetRequest(requestId, nullptr, nullptr);
}

void SBDebuggerClient::SendSBMCommand(int requestId, const std::string & returnValue, const std::string & functionNameandParams,
                                       NetRequest::RequestCallback cb,  void* callbackOwner)
{
   // send a void com
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s %d request %s \"ret = %s\"", m_sbmId.c_str(),
      requestId, returnValue.c_str(), functionNameandParams.c_str()).c_str());

   m_netRequestManager.CreateNetRequest(requestId, cb, callbackOwner);
}

void SBDebuggerClient::ProcessVHMsgs(const char * op, const char * args)
{
   string message = string(op) + " " + string(args);
   vector<string> split;
   // get first line of the string
   size_t nl = message.find_first_of('\n');//std::find( message.begin(), message.end(), '\n' ) ;
   std::string line1 = message;
   std::string rest = "";
   if (nl != std::string::npos)
   {
	   line1 = message.substr(0, nl );
	   rest = message.substr(nl);
   }  
   SmartBody::util::tokenize(line1, split, " \t\n");
   //SmartBody::util::tokenize(message, lines, "\n"); // separate lines
   if (split.size() > 0)
   {
      if (split[0] == "sbmdebugger")
      {
         if (split.size() > 1)
         {
            if (split.size() > 2)
            {
               if (split[2] == "id")
               {
                  m_processIdList.emplace_back(split[1]);
               }
            }

            if (split[1] == m_sbmId)
            {
               if (split.size() >= 4 && split[3] == "response")
               {
                  NetRequest::RequestId rid = (NetRequest::RequestId)atoi(split[2].c_str());
                  split.erase(split.begin(), split.begin() + 4); // erase first 4 elements
                  m_netRequestManager.ProcessRequest(rid, split);
               }
               else if (split.size() >= 5 && split[3] == "response-fail")
               {
                  NetRequest::RequestId rid = (NetRequest::RequestId)atoi(split[2].c_str());
                  m_netRequestManager.RemoveRequest(rid);
                  printf("Error: response-fail on on net request %d. Reason: %s", (int)rid, split[4].c_str());
               }

               if (split.size() > 2)
               {
                  if (split[2] == "connect_success")
                  {
                     m_connectResult = true;					
					 m_initFinish = false;
					  m_debuggerUtility.initScene();
					  m_debuggerUtility.queryResources();
                  }
                  else if (split[2] == "init")
                  {
					  if (split.size() > 3)
					  {
						  if (split[3] == "scene")
						  {
							  //initialize the rest of scene
							  //SmartBody::util::log("test = %s",rest.c_str());
// 							  SmartBody::util::log("rest string size = %d",rest.size());
// 							  FILE* fp = fopen("e:/scene.py","wt");
// 							  fprintf(fp,"%s",rest.c_str());
// 							  fclose(fp);

							  _scene.run(rest);
							  m_initFinish = true;
							  //_scene.runScript("e:/scene.py");
						  }
					  }
#if 0
                     if (split.size() > 3)
                     {
                        if (split[3] == "character-skeleton")
						{
							string name = split[4];
							string skeletonName = split[5];
							_scene.getDebuggerUtility()->initCharacter(name, skeletonName);
						}
						else if (split[3] == "character-face_definition")
						{
							string name = split[4];
							string faceDefName = split[5];
							_scene.getDebuggerUtility()->initCharacterFaceDefinition(name, faceDefName, message);

						}
						else if (split[3] == "skeleton")
						{
							std::string skFileName = split[4];
							std::string command = "# " + message;
							_scene.getDebuggerUtility()->initSkeleton(skFileName, command);
						}
                        else if (split[3] == "pawn")
                        {
							_scene.getDebuggerUtility()->initPawn(split[4]);
                        }
						else if (split[3] == "face_definition")
						{
							std::string command = "# " + message;
							_scene.getDebuggerUtility()->runPythonCommand(command);						
						}
						else if (split[3] == "blend")
						{
							std::string command = "# " + message;
							_scene.getDebuggerUtility()->runPythonCommand(command);						
						}
						else if (split[3] == "transition")
						{
							std::string command = "# " + message;
							_scene.getDebuggerUtility()->runPythonCommand(command);						
						}
                     }
#endif
                     //printf("Init Received\n");
                     //printf(message.c_str());
                     //printf("\n");
                  }
               }
            }
         }
      }
      else if (split[0] == "sbmlog")
      {
         if (split[1] == "MotionFile")
         {
            //split[split.size() - 1];
            //_scene.addAssetPath();
         }
      }
      else if (split[0] == "vrAllCall")
      {
         vhmsg::ttu_notify1("vrComponent sbmdebugger");
      }
      else if (split[0] == "vrKillComponent")
      {
         if (split.size() > 1 && split[1] == "sbmdebugger")
         {
            
         }
      }
      else if (split[0] == "vrKillAll")
      {
         
      }
   }
}

void SBDebuggerClient::attachToScene(SBScene& scene) {
	struct Listener : public SBSceneListener {
		SBDebuggerClient& _client;
		Listener(SBDebuggerClient& client) : _client(client) {}
		void OnSimulationUpdate() override {
			_client.Update();
		}

	};

	scene.addSceneListener(new Listener(*this));
}

}
