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
#include <sbm/sr_arg_buff.h>
#include <sbm/sbm_constants.h>
#include "vhcl_socket.h"


#include "vhmsg-tt.h"


struct VHMsgLogger : public SmartBody::util::Listener
{

	SmartBody::SBScene& _scene;
		explicit VHMsgLogger(SmartBody::SBScene& scene) : SmartBody::util::Listener(), _scene(scene)
		{
		}
        
		~VHMsgLogger() override = default;

        void OnMessage( const std::string & message ) override
		{
			_scene.sendVHMsg2("sbmlog", message);
		}
};

namespace SmartBody {

SBVHMsgManager::SBVHMsgManager(SBScene& scene) : SBService(scene)
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

	if (vhmsg::ttu_is_open())
		vhmsg::ttu_close();

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
	if (_scene.isRemoteMode())
		return true;
	
	return SBService::isEnable();
}

bool SBVHMsgManager::isConnected()
{
	return vhmsg::ttu_is_open();

}

bool SBVHMsgManager::connect()
{
	if (vhmsg::ttu_is_open())
		vhmsg::ttu_close();

	if (vhmsg::ttu_open(_server.c_str(), _scope.c_str(), _port.c_str()) == vhmsg::TTU_SUCCESS)
	{
		vhmsg::ttu_set_client_callback( &SBVHMsgManager::vhmsgCallback, this );
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
}

void SBVHMsgManager::disconnect()
{

	if (vhmsg::ttu_is_open())
		vhmsg::ttu_close();
}

void SBVHMsgManager::stop()
{
	disconnect();
}

int SBVHMsgManager::send2( const char *op, const char* message )
{

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
		if( _scene.getCommandManager()->hasCommand( op ) ) {
			// Append to command queue
			std::ostringstream command;
			command << op << " " << message;
			_scene.getCommandManager()->execute_later( command.str().c_str() );
		}
	}

	return( CMD_SUCCESS );

}

int SBVHMsgManager::send( const char* message )
{
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
		if( _scene.getCommandManager()->hasCommand( token ) ) {
			// Append to command queue
			_scene.getCommandManager()->execute_later( message );
		}
	}

	return( CMD_SUCCESS );
}

int SBVHMsgManager::poll()
{

	if( isEnable() )
	{
		int ret = vhmsg::ttu_poll();
		if ( ret == vhmsg::TTU_SUCCESS )
			return CMD_SUCCESS;
		else
			return CMD_FAILURE;
	}
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
	auto vhmsgManager = static_cast<SBVHMsgManager*>(user_data);
	if (vhmsgManager->getBoolAttribute("showMessages"))
		SmartBody::util::log("[%s] %s", op, args);
	//FIXME: expose the callback so that the debugger can listen to it
//	if (_scene.isRemoteMode())
//	{
//		_scene.getDebuggerClient()->ProcessVHMsgs(op, args);
//		return;
//	}
//	else
//	{
//		_scene.getDebuggerServer()->ProcessVHMsgs(op, args);
//	}

	//SmartBody::util::log("Get VHMSG , op = %s, args = %s", op, args);
	switch( vhmsgManager->_scene.getCommandManager()->execute( op, (char *)args ) )
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
		_logListener = new VHMsgLogger(_scene);
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

