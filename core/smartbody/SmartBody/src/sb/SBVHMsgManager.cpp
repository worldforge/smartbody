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

#include "SBVHMsgManager.h"
#include <sb/SBScene.h>
#include <sb/SBCommandManager.h>
#include "SBUtilities.h"

#include <iostream>
#include <sstream>
#include <sbm/sr_arg_buff.h>
#include <sbm/sbm_constants.h>
#include "vhcl_socket.h"


#ifdef __ANDROID__
#define LINK_VHMSG_CLIENT		(1)
#elif defined(__native_client__)
#else
#define LINK_VHMSG_CLIENT		(1)
#endif

#if LINK_VHMSG_CLIENT
#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#endif

class VHMsgLogger : public SmartBody::util::Listener
{
	public:
		VHMsgLogger() : SmartBody::util::Listener()
		{
		}
        
		virtual ~VHMsgLogger()
		{
		}

        virtual void OnMessage( const std::string & message )
		{
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2("sbmlog", message.c_str());
		}
};

namespace SmartBody {

SBVHMsgManager::SBVHMsgManager() : SBService()
{
	createBoolAttribute("showMessages", false, true, "Basic", 60, false, false, false, "Dumps any registered vhmsgs to the console.");

	setEnable(false);

	setName("VHMsg");

	_port = "61616";
	if (getenv( "VHMSG_SERVER" ))
		_server = getenv( "VHMSG_SERVER" );
	else
		_server = "localhost";
	if (getenv("VHMSG_SCOPE"))
		_scope = getenv("VHMSG_SCOPE");
	else
		_scope = "DEFAULT_SCOPE";
	_logListener = nullptr;
}

SBVHMsgManager::~SBVHMsgManager()
{
	if (isEnable() && isConnected()) {
		send("vrProcEnd sbm");
	}

#ifndef SB_NO_VHMSG
	if (vhmsg::ttu_is_open())
		vhmsg::ttu_close();
#endif

	if (_logListener)
	{
		SmartBody::util::g_log.RemoveListener(_logListener);
	}
	delete _logListener;
}

void SBVHMsgManager::setEnable(bool val)
{
	SBService::setEnable(val);

	if (val)
	{
		vhcl::SocketStartup();
		bool success = connect();
		if (!success)
			SBService::setEnable(false);

	}
	else
	{
		disconnect();
	}

}

bool SBVHMsgManager::isEnable()
{
	// if it's remote mode, vhmsg should be enabled no matter what
	if (SmartBody::SBScene::getScene()->isRemoteMode())
		return true;
	
	return SBService::isEnable();
}

bool SBVHMsgManager::isConnected()
{
#ifndef SB_NO_VHMSG
	return vhmsg::ttu_is_open();
#else
	return false;
#endif

}

bool SBVHMsgManager::connect()
{
#ifndef SB_NO_VHMSG
	if (vhmsg::ttu_is_open())
		vhmsg::ttu_close();

	if (vhmsg::ttu_open(_server.c_str(), _scope.c_str(), _port.c_str()) == vhmsg::TTU_SUCCESS)
	{
		vhmsg::ttu_set_client_callback( &SBVHMsgManager::vhmsgCallback );
		int err = vhmsg::TTU_SUCCESS;
		err = vhmsg::ttu_register( "sb" );
		err = vhmsg::ttu_register( "sbm" );
		err = vhmsg::ttu_register( "vrAgentBML" );
		err = vhmsg::ttu_register( "vrExpress" );
		err = vhmsg::ttu_register( "vrSpeak" );
		err = vhmsg::ttu_register( "RemoteSpeechCmd" ); // for local speech relay
		err = vhmsg::ttu_register( "RemoteSpeechReply" );
		err = vhmsg::ttu_register( "PlaySound" );
		err = vhmsg::ttu_register( "StopSound" );
		err = vhmsg::ttu_register( "CommAPI" );
		err = vhmsg::ttu_register( "object-data" );
		err = vhmsg::ttu_register( "vrAllCall" );
		err = vhmsg::ttu_register( "vrKillComponent" );
		err = vhmsg::ttu_register( "receiver" );
		err = vhmsg::ttu_register( "sbmdebugger" );
		err = vhmsg::ttu_register( "vrPerception" );
		err = vhmsg::ttu_register( "vrBCFeedback" );
		err = vhmsg::ttu_register( "vrSpeech" );
		SmartBody::util::log("VHMSG connected successfully");
		return true;
	} 
	else
	{
		SmartBody::util::log("Could not connect to %s:%s", _server.c_str(), _port.c_str());
		setEnable(false);
		return false;
	}
#else
	SmartBody::util::log("VHMSG has been disabled.");
	return false;
#endif
}

void SBVHMsgManager::disconnect()
{

#ifndef SB_NO_VHMSG
	if (vhmsg::ttu_is_open())
		vhmsg::ttu_close();
#endif
}

void SBVHMsgManager::stop()
{
	disconnect();
}

int SBVHMsgManager::send2( const char *op, const char* message )
{
#ifndef SB_NO_VHMSG
#if LINK_VHMSG_CLIENT
	
	if( isEnable() )
	{
		int err = vhmsg::ttu_notify2( op, message );
		if( err != vhmsg::TTU_SUCCESS )	{
			std::stringstream strstr;
			if (!op || !message)
				strstr << "ERROR: mcuCBHandle::vhmsg_send(..): ttu_notify2 failed on message." << std::endl;
			else
				strstr << "ERROR: mcuCBHandle::vhmsg_send(..): ttu_notify2 failed on message \"" << op << "  " << message << "\"." << std::endl;
			SmartBody::util::log(strstr.str().c_str());
		}
	}
	else
	{
		// append to command queue if header token has callback function
		srArgBuffer tokenizer( message );
		char* token = tokenizer.read_token();
		if( SmartBody::SBScene::getScene()->getCommandManager()->hasCommand( op ) ) {
			// Append to command queue
			std::ostringstream command;
			command << op << " " << message;
			SmartBody::SBScene::getScene()->getCommandManager()->execute_later( command.str().c_str() );
		}
	}
#else
	// append to command queue if header token has callback function
	srArgBuffer tokenizer( message );
	char* token = tokenizer.read_token();
	if( cmd_map.is_command( op ) ) {
		// Append to command queue
		ostringstream command;
		command << op << " " << message;
		execute_later( command.str().c_str() );
	}
#endif
#else
	// send the command locally
	std::stringstream strstr;
	strstr << op << " " << message;
	SmartBody::SBScene::getScene()->command(strstr.str());
#endif
	return( CMD_SUCCESS );

}

int SBVHMsgManager::send( const char* message )
{
#ifndef SB_NO_VHMSG
#if LINK_VHMSG_CLIENT
	if( isEnable() && vhmsg::ttu_is_open())
	{
		int err = vhmsg::ttu_notify1( message );
		if( err != vhmsg::TTU_SUCCESS )	{
			std::stringstream strstr;
			strstr << "ERROR: mcuCBHandle::vhmsg_send(..): ttu_notify1 failed on message \"" << message << "\"." << std::endl;
			SmartBody::util::log(strstr.str().c_str());
		}
	}
	else
	{
		// append to command queue if header token has callback function
		srArgBuffer tokenizer( message );
		char* token = tokenizer.read_token();
		if( SmartBody::SBScene::getScene()->getCommandManager()->hasCommand( token ) ) {
			// Append to command queue
			SmartBody::SBScene::getScene()->getCommandManager()->execute_later( message );
		}
	}
#else
	// append to command queue if header token has callback function
	srArgBuffer tokenizer( message );
	char* token = tokenizer.read_token();
	if( cmd_map.is_command( token ) ) {
		// Append to command queue
		execute_later( message );
	}
#endif
#else
	// send the command locally
	SmartBody::SBScene::getScene()->command(message);
#endif

	return( CMD_SUCCESS );
}

int SBVHMsgManager::poll()
{
#ifndef SB_NO_VHMSG
#if LINK_VHMSG_CLIENT
	if( isEnable() )
	{
		int ret = vhmsg::ttu_poll();
		if ( ret == vhmsg::TTU_SUCCESS )
			return CMD_SUCCESS;
		else
			return CMD_FAILURE;
	}
#endif
#endif
	return CMD_SUCCESS;
}

void SBVHMsgManager::setPort(const std::string& port)
{
	_port = port;
}

const std::string& SBVHMsgManager::getPort()
{
	return _port;
}

void SBVHMsgManager::setServer(const std::string& server)
{
	_server = server;
}

const std::string& SBVHMsgManager::getServer()
{
	return _server;
}

void SBVHMsgManager::setScope(const std::string& scope)
{
	_scope = scope;
}

const std::string& SBVHMsgManager::getScope()
{
	return _scope;
}

void SBVHMsgManager::vhmsgCallback( const char *op, const char *args, void * user_data )
{
	if (SmartBody::SBScene::getScene()->getVHMsgManager()->getBoolAttribute("showMessages"))
		SmartBody::util::log("[%s] %s", op, args);
	//FIXME: expose the callback so that the debugger can listen to it
//	if (SmartBody::SBScene::getScene()->isRemoteMode())
//	{
//		SmartBody::SBScene::getScene()->getDebuggerClient()->ProcessVHMsgs(op, args);
//		return;
//	}
//	else
//	{
//		SmartBody::SBScene::getScene()->getDebuggerServer()->ProcessVHMsgs(op, args);
//	}

	//SmartBody::util::log("Get VHMSG , op = %s, args = %s", op, args);
	switch( SmartBody::SBScene::getScene()->getCommandManager()->execute( op, (char *)args ) )
	{
        case CMD_NOT_FOUND:
            SmartBody::util::log("SmartBody error: command NOT FOUND: '%s' + '%s'", op, args );
            break;
        case CMD_FAILURE:
            SmartBody::util::log("SmartBody error: command FAILED: '%s' + '%s'", op, args );
            break;
    }
}

void SBVHMsgManager::setEnableLogging(bool val)
{
	if (val)
	{
		_logListener = new VHMsgLogger();
		SmartBody::util::g_log.AddListener(_logListener);
	}
}

bool SBVHMsgManager::isEnableLogging()
{
	if (_logListener)
		return true;
	else
		return false;
}

SBAPI int SBVHMsgManager::sendMessage( const std::string& message )
{
	return send(message.c_str());
}

SBAPI int SBVHMsgManager::sendOpMessage( const std::string& op, const std::string& message )
{
	return send2(op.c_str(), message.c_str());
}


void SBVHMsgManager::onCharacterDelete(SBCharacter* character) {
	std::string vrProcEnd_msg = "vrProcEnd sbm ";
	vrProcEnd_msg += getName();
	send( vrProcEnd_msg.c_str() );
}

}

