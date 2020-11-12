
#include "vhcl.h"
#include "vhcl_socket.h"

#include "SBDebuggerClient.h"

#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif

#include "SBDebuggerCommon.h"


using std::string;
using std::vector;


SBDebuggerClient::SBDebuggerClient()
{
}

SBDebuggerClient::~SBDebuggerClient()
{
}

void SBDebuggerClient::QuerySbmProcessIds()
{
   m_processIdList.clear();
   vhmsg::ttu_notify1("sbmdebugger queryids");
}






vhcl::socket_t m_sockTCP = NULL;


void SBDebuggerClient::Connect(const string & id)
{
   m_sbmId = id;
   m_connectResult = false;
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s connect", m_sbmId.c_str()).c_str());




   // id == host:port:friendlyName
   vector<string> idSplit;
   vhcl::Tokenize(id, idSplit, ":");
   const char * server = idSplit[0].c_str();
   int port = vhcl::ToInt(idSplit[1]);


   bool ret = vhcl::SocketStartup();
   if (!ret)
   {
      printf("SocketStartup() failed\n");
   }


   m_sockTCP = vhcl::SocketOpenTcp();
   if (m_sockTCP == NULL)
   {
      printf( "SocketOpenTcp() failed\n" );
      vhcl::SocketShutdown();
      return;
   }

   ret = vhcl::SocketConnect(m_sockTCP, server, port);
   if (!ret)
   {
      printf( "SocketConnect() failed\n" );
      vhcl::SocketClose(m_sockTCP);
      vhcl::SocketShutdown();
      return;
   }
}

void SBDebuggerClient::Disconnect()
{
   if ( m_sockTCP )
   {
      vhcl::SocketClose(m_sockTCP);
      m_sockTCP = NULL;
   }

   vhcl::SocketShutdown();


   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s disconnect", m_sbmId.c_str()).c_str());
   m_processIdList.clear();
   m_sbmId = "";
   m_connectResult = false;
   m_scene.m_pawns.clear();
   m_scene.m_characters.clear();
   m_scene.m_camera = DebuggerCamera();
   m_scene.m_rendererIsRightHanded = true;
}

void SBDebuggerClient::Init()
{
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s send_init", m_sbmId.c_str()).c_str());
}






string m_tcpData;
int m_tcpDataCount = 0;

void SBDebuggerClient::Update()
{
   {
      vhcl::socket_t s = m_sockTCP;

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

            //printf("TCP %d\n", m_tcpDataCount);

            tcpDataPending = vhcl::SocketIsDataPending(s);
         }

         // sanity check for spamming too much data
         if (m_tcpData.length() > 500000)
         {
            //static int i = 0;
            //printf("TCP Overflow %d - %d\n", i++);
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
            else if (split[2] == "update")
            {
               double converter = m_scene.m_rendererIsRightHanded ? 1 : -1;
               // handle updates

               if (split.size() > 3)
               {
                  if (split[3] == "character")
                  {
                     Character * character = NULL;

                     if (split.size() > 4)
                     {
                        string character_string = split[3];
                        string name = split[4];

                        for (size_t i = 0; i < m_scene.m_characters.size(); i++)
                        {
                           Character * c = &m_scene.m_characters[i];
                           if (c->m_name == name)
                           {
                              character = c;
                           }
                        }
                     }

                     if (character)
                     {
                        if (split.size() > 5)
                        {
                           /*if (split[5] == "position")
                           {
                              if (split.size() > 12)
                              {
                                 double posX = vhcl::ToDouble(split[6]);
                                 double posY = vhcl::ToDouble(split[7]);
                                 double posZ = vhcl::ToDouble(split[8]);
                                 double rotX = vhcl::ToDouble(split[9]);
                                 double rotY = vhcl::ToDouble(split[10]);
                                 double rotZ = vhcl::ToDouble(split[11]);
                                 double rotW = vhcl::ToDouble(split[12]);

                                 character->pos.x = posX;
                                 character->pos.y = posY;
                                 character->pos.z = posZ;
                                 character->rot.x = rotX;
                                 character->rot.y = rotY;
                                 character->rot.z = rotZ;
                                 character->rot.w = rotW;
                              }
                           }
                           else*/ if (split[5] == "bones")
                           {
                              if (split.size() > 6)
                              {
                                 int numBones = vhcl::ToInt(split[6]);
                              }

                              for (size_t i = 7; i < split.size(); i++)
                              {
                                 string name = split[i];
                                 string pos  = split[i + 1];
                                 double posX = vhcl::ToDouble(split[i + 2]);
                                 double posY = vhcl::ToDouble(split[i + 3]);
                                 double posZ = vhcl::ToDouble(split[i + 4]);
                                 string rot  = split[i + 5];
                                 double rotX = vhcl::ToDouble(split[i + 6]);
                                 double rotY = vhcl::ToDouble(split[i + 7]);
                                 double rotZ = vhcl::ToDouble(split[i + 8]);
                                 double rotW = vhcl::ToDouble(split[i + 9]);
                                 i += 9;

                                 Joint * joint = character->FindJoint(name);
                                 if (joint)
                                 {
                                    joint->pos.x = posX;
                                    joint->pos.y = posY;
                                    joint->pos.z = posZ;
                                    joint->rot.x = rotX;
                                    joint->rot.y = rotY;
                                    joint->rot.z = rotZ;
                                    joint->rot.w = rotW;
                                 }
                              }
                           }
                        }
                     }
                  }
                  else if (split[3] == "pawn")
                  {
                      // sbmdebugger <sbmid> update pawn <name> pos <x y z> rot <x y z w> geom <s> size <s> 
                     Pawn* pawn = m_scene.FindPawn(split[4]);
                     if (pawn)
                     {
                         Joint* worldOffset = pawn->FindJoint("world_offset");
                         worldOffset->pos.x = vhcl::ToDouble(split[6]);
                         worldOffset->pos.y = vhcl::ToDouble(split[7]);
                         worldOffset->pos.z = vhcl::ToDouble(split[8]);
                         worldOffset->rot.x = vhcl::ToDouble(split[10]);
                         worldOffset->rot.y = vhcl::ToDouble(split[11]);
                         worldOffset->rot.z = vhcl::ToDouble(split[12]);
                         worldOffset->rot.w = vhcl::ToDouble(split[13]);
                         pawn->m_shape = Pawn::StringToPawnShape(split[15]);
                         pawn->m_size = vhcl::ToDouble(split[17]);
                     }                  
                  }
                  else if (split[3] == "camera")
                  {
                     int i = 4;
                     string pos  = split[i];
                     double posX = vhcl::ToDouble(split[i + 1]);
                     double posY = vhcl::ToDouble(split[i + 2]);
                     double posZ = vhcl::ToDouble(split[i + 3]);
                     string rot  = split[i + 4];
                     double rotX = vhcl::ToDouble(split[i + 5]);
                     double rotY = vhcl::ToDouble(split[i + 6]);
                     double rotZ = vhcl::ToDouble(split[i + 7]);
                     double rotW = vhcl::ToDouble(split[i + 8]);
                     string persp  = split[i + 9];
                     double fovY   = vhcl::ToDouble(split[i + 10]);
                     double aspect = vhcl::ToDouble(split[i + 11]);
                     double zNear  = vhcl::ToDouble(split[i + 12]);
                     double zFar   = vhcl::ToDouble(split[i + 13]);

                     m_scene.m_camera.pos.x = posX;
                     m_scene.m_camera.pos.y = posY;
                     m_scene.m_camera.pos.z = posZ;
                     m_scene.m_camera.rot.x = rotX;
                     m_scene.m_camera.rot.y = rotY;
                     m_scene.m_camera.rot.z = rotZ;
                     m_scene.m_camera.rot.w = rotW;
                     m_scene.m_camera.fovY   = fovY;
                     m_scene.m_camera.aspect = aspect;
                     m_scene.m_camera.zNear  = zNear;
                     m_scene.m_camera.zFar   = zFar;
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

void SBDebuggerClient::GetResourcePaths()
{
   // store query ids
   // send queries
   // wait for responses
   // update vectors
   int pid = 42;

   //scene.getAssetPaths("seq")
   //sbmdebugger 123 455 request void "DoSomething(42)" 

   //vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s %d request string-array \"ret = scene.getAssetPaths("seq")\"", m_sbmId.c_str(), 42).c_str());

   //NetRequest* req = new NetRequest();
}

void SBDebuggerClient::SendSBMCommand(int requestId, const std::string & command)
{
   // send a void command, not expecting a return
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s %d request void \"ret = %s\"", m_sbmId.c_str(), 42, command.c_str()).c_str());

   m_netRequestManager.CreateNetRequest(requestId, NULL, NULL);
}

void SBDebuggerClient::SendSBMCommand(int requestId, const std::string & returnValue, const std::string & functionNameandParams,
                                       NetRequest::RequestCallback cb,  void* callbackOwner)
{
   vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s %d request %s \"ret = %s\"", m_sbmId.c_str(),
      requestId, returnValue.c_str(), functionNameandParams.c_str()).c_str());

   m_netRequestManager.CreateNetRequest(requestId, cb, callbackOwner);
}


void SBDebuggerClient::ProcessVHMsgs(const char * op, const char * args)
{
   string message = string(op) + " " + string(args);
   vector<string> split;
   vhcl::Tokenize(message, split, " \t\n");

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
                  m_processIdList.push_back(split[1]);
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
                  }
                  else if (split[2] == "init")
                  {
                     if (split.size() > 3)
                     {
                        if (split[3] == "character")
                        {
                           Character character;

                           if (split.size() > 4)
                           {
                              string character_string = split[3];
                              string name = split[4];

                              character.m_name = name;
                           }

                           if (split.size() > 6)
                           {
                              string bones_string = split[5];
                              int numBones = vhcl::ToInt(split[6]);
                           }

                           Joint * parent = NULL;
                           for (size_t i = 7; i < split.size(); i++)
                           {
                              if (split[i] == "{")
                              {
                                 string name = split[i + 1];
                                 string pos  = split[i + 2];
                                 double posX = vhcl::ToDouble(split[i + 3]);
                                 double posY = vhcl::ToDouble(split[i + 4]);
                                 double posZ = vhcl::ToDouble(split[i + 5]);
                                 string prerot  = split[i + 6];
                                 double rotX = vhcl::ToDouble(split[i + 7]);
                                 double rotY = vhcl::ToDouble(split[i + 8]);
                                 double rotZ = vhcl::ToDouble(split[i + 9]);
                                 double rotW = vhcl::ToDouble(split[i + 10]);
                                 i += 10;

                                 Joint * j = new Joint();
                                 j->m_name = name;
                                 j->posOrig.x = posX;
                                 j->posOrig.y = posY;
                                 j->posOrig.z = posZ;
                                 j->rotOrig.x = rotX;
                                 j->rotOrig.y = rotY;
                                 j->rotOrig.z = rotZ;
                                 j->rotOrig.w = rotW;
                                 j->m_parent = parent;

                                 if (parent == NULL)
                                 {
                                    character.m_joints.push_back(j);
                                    parent = *(character.m_joints.end() - 1);
                                 }
                                 else
                                 {
                                    parent->m_joints.push_back(j);
                                    parent = *(parent->m_joints.end() - 1);
                                 }
                              }
                              else if (split[i] == "}")
                              {
                                 if (parent != NULL)
                                 {
                                    parent = parent->m_parent;
                                 }
                              }
                           }

                           m_scene.m_characters.push_back(character);
                        }
                        else if (split[3] == "pawn")
                        {
                           // sbmdebugger <sbmid> update pawn <name> pos <x y z> rot <x y z w> geom <s> size <s> 
                           Pawn p;
                           p.m_name = split[4];
                           Joint* worldOffset = new Joint();
                           worldOffset->m_name = "world_offset";
                           worldOffset->posOrig.x = vhcl::ToDouble(split[6]);
                           worldOffset->posOrig.y = vhcl::ToDouble(split[7]);
                           worldOffset->posOrig.z = vhcl::ToDouble(split[8]);
                           worldOffset->rotOrig.x = vhcl::ToDouble(split[10]);
                           worldOffset->rotOrig.y = vhcl::ToDouble(split[11]);
                           worldOffset->rotOrig.z = vhcl::ToDouble(split[12]);
                           worldOffset->rotOrig.w = vhcl::ToDouble(split[13]);
                           p.m_joints.push_back(worldOffset);
                           p.m_shape = Pawn::StringToPawnShape(split[15]);
                           p.m_size = vhcl::ToDouble(split[17]);
                           
                           m_scene.m_pawns.push_back(p);
                        }
                        else if (split[3] == "renderer")
                        {
                           if (split.size() > 5)
                           {
                              if (split[4] == "right_handed")
                              {
                                 string value = split[5];
                                 m_scene.m_rendererIsRightHanded = vhcl::ToInt(value) != 0;
                              }
                           }
                        }
                     }


                     //printf("Init Received\n");
                     //printf(message.c_str());
                     //printf("\n");
                  }
                  else if (split[2] == "update")
                  {
                     double converter = m_scene.m_rendererIsRightHanded ? 1 : -1;
                     // handle updates

                     if (split.size() > 3)
                     {
                        if (split[3] == "character")
                        {
                           Character * character = NULL;

                           if (split.size() > 4)
                           {
                              string character_string = split[3];
                              string name = split[4];

                              for (size_t i = 0; i < m_scene.m_characters.size(); i++)
                              {
                                 Character * c = &m_scene.m_characters[i];
                                 if (c->m_name == name)
                                 {
                                    character = c;
                                 }
                              }
                           }

                           if (character)
                           {
                              if (split.size() > 5)
                              {
                                 /*if (split[5] == "position")
                                 {
                                    if (split.size() > 12)
                                    {
                                       double posX = vhcl::ToDouble(split[6]);
                                       double posY = vhcl::ToDouble(split[7]);
                                       double posZ = vhcl::ToDouble(split[8]);
                                       double rotX = vhcl::ToDouble(split[9]);
                                       double rotY = vhcl::ToDouble(split[10]);
                                       double rotZ = vhcl::ToDouble(split[11]);
                                       double rotW = vhcl::ToDouble(split[12]);

                                       character->pos.x = posX;
                                       character->pos.y = posY;
                                       character->pos.z = posZ;
                                       character->rot.x = rotX;
                                       character->rot.y = rotY;
                                       character->rot.z = rotZ;
                                       character->rot.w = rotW;
                                    }
                                 }
                                 else*/ if (split[5] == "bones")
                                 {
                                    if (split.size() > 6)
                                    {
                                       int numBones = vhcl::ToInt(split[6]);
                                    }

                                    for (size_t i = 7; i < split.size(); i++)
                                    {
                                       string name = split[i];
                                       string pos  = split[i + 1];
                                       double posX = vhcl::ToDouble(split[i + 2]);
                                       double posY = vhcl::ToDouble(split[i + 3]);
                                       double posZ = vhcl::ToDouble(split[i + 4]);
                                       string rot  = split[i + 5];
                                       double rotX = vhcl::ToDouble(split[i + 6]);
                                       double rotY = vhcl::ToDouble(split[i + 7]);
                                       double rotZ = vhcl::ToDouble(split[i + 8]);
                                       double rotW = vhcl::ToDouble(split[i + 9]);
                                       i += 9;

                                       Joint * joint = character->FindJoint(name);
                                       if (joint)
                                       {
                                          joint->pos.x = posX;
                                          joint->pos.y = posY;
                                          joint->pos.z = posZ;
                                          joint->rot.x = rotX;
                                          joint->rot.y = rotY;
                                          joint->rot.z = rotZ;
                                          joint->rot.w = rotW;
                                       }
                                    }
                                 }
                              }
                           }
                        }
                        else if (split[3] == "camera")
                        {
                           int i = 4;
                           string pos  = split[i];
                           double posX = vhcl::ToDouble(split[i + 1]);
                           double posY = vhcl::ToDouble(split[i + 2]);
                           double posZ = vhcl::ToDouble(split[i + 3]);
                           string rot  = split[i + 4];
                           double rotX = vhcl::ToDouble(split[i + 5]);
                           double rotY = vhcl::ToDouble(split[i + 6]);
                           double rotZ = vhcl::ToDouble(split[i + 7]);
                           double rotW = vhcl::ToDouble(split[i + 8]);
                           string persp  = split[i + 9];
                           double fovY   = vhcl::ToDouble(split[i + 10]);
                           double aspect = vhcl::ToDouble(split[i + 11]);
                           double zNear  = vhcl::ToDouble(split[i + 12]);
                           double zFar   = vhcl::ToDouble(split[i + 13]);

                           m_scene.m_camera.pos.x = posX;
                           m_scene.m_camera.pos.y = posY;
                           m_scene.m_camera.pos.z = posZ;
                           m_scene.m_camera.rot.x = rotX;
                           m_scene.m_camera.rot.y = rotY;
                           m_scene.m_camera.rot.z = rotZ;
                           m_scene.m_camera.rot.w = rotW;
                           m_scene.m_camera.fovY   = fovY;
                           m_scene.m_camera.aspect = aspect;
                           m_scene.m_camera.zNear  = zNear;
                           m_scene.m_camera.zFar   = zFar;
                        }
                     }


                     //printf("Update Received\n");
                     //printf(message.c_str());
                     //printf("\n");
                  }
               }
            }
         }
      }
      else if (split[0] == "vrAllCall")
      {
         vhmsg::ttu_notify1("vrComponent sbmdebugger");
      }
      else if (split[0] == "vrKillComponent")
      {
         if (split.size() > 1)
         {
            if (split[1] == "sbmonitor" || split[1] == "all")
            {
               if (m_OnCloseCB)
                  (*m_OnCloseCB)(m_pCloseCBData);
            }
         }
      }
      else if (split[0] == "vrKillAll")
      {
         if (m_OnCloseCB)
            (*m_OnCloseCB)(m_pCloseCBData);
      }
   }
}
