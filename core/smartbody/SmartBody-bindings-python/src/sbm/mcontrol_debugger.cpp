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

#include "mcontrol_debugger.h"
#include "sb/SBScene.h"
#include "sb/SBVHMsgManager.h"
#include <boost/python.hpp>

int mcu_sbmdebugger_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	std::string instanceId = args.read_token();
	// make sure this instance id matches
	// ...
	// ...

	std::string messageId = args.read_token();
	std::string type = args.read_token();
	if (type == "response")
		return CMD_SUCCESS;

	if (type != "request")
		return CMD_SUCCESS;

	std::string returnType = args.read_token();
	std::string code = args.read_token();
	//SmartBody::util::log("mcu_sbmdebugger_func code: %s", code.c_str());
	if (code.empty())
	{
		std::stringstream strstr;
		strstr << instanceId << " " << messageId << " response-fail ";
		strstr << "No Python code sent.";
		SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
		return CMD_FAILURE;
	}

	boost::python::object module = boost::python::import("__main__");
	boost::python::object mainDict  = module.attr("__dict__");

	PyObject* pyResult = nullptr;
	if (returnType == "void")
	{
		try {
			boost::python::exec(code.c_str(), mainDict);
		} catch (...) {
			PyErr_Print();
		}
		return CMD_SUCCESS;
	}
	else if (returnType == "bool")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			bool result = boost::python::extract<bool>(mainDict["ret"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			strstr << result;
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else if (returnType == "int")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			int result = boost::python::extract<int>(mainDict["ret"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			strstr << result;
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else if (returnType == "float")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			float result = boost::python::extract<float>(mainDict["ret"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			strstr << result;
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else if (returnType == "string")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			std::string result = boost::python::extract<std::string>(mainDict["ret"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			strstr << result;
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else if (returnType == "int-array")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			boost::python::object obj2 = boost::python::exec("size = len(ret)", mainDict);
			int size =  boost::python::extract<int>(mainDict["size"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			for (int x = 0; x < size; x++)
			{
				int val =  boost::python::extract<int>(mainDict["ret"][x]);
				strstr << " " << val;
			}
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else if (returnType == "float-array")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			boost::python::object obj2 = boost::python::exec("size = len(ret)", mainDict);
			int size =  boost::python::extract<int>(mainDict["size"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			for (int x = 0; x < size; x++)
			{
				float val =  boost::python::extract<float>(mainDict["ret"][x]);
				strstr << " " << val;
			}
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else if (returnType == "string-array")
	{
		try {
			boost::python::object obj = boost::python::exec(code.c_str(), mainDict);
			boost::python::object obj2 = boost::python::exec("size = len(ret)", mainDict);
			int size =  boost::python::extract<int>(mainDict["size"]);
			std::stringstream strstr;
			strstr << instanceId << " " << messageId << " response ";
			for (int x = 0; x < size; x++)
			{
				std::string val =  boost::python::extract<std::string>(mainDict["ret"][x]);
				strstr << " " << val;
			}
			SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
			return CMD_SUCCESS;
		} catch (...) {
			PyErr_Print();
		}
	}
	else
	{
		std::stringstream strstr;
		strstr << instanceId << " " << messageId << " response-fail ";
		strstr << "Unknown return type: " << returnType;
		SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );
		return CMD_FAILURE;
	}

	std::stringstream strstr;
	strstr << instanceId << " " << messageId << " response-fail ";
	strstr << "Problem executing code." << returnType;
	SmartBody::SBScene::getScene()->getVHMsgManager()->send2( "sbmdebugger", strstr.str().c_str() );

	return CMD_FAILURE;
}

namespace SmartBody {

void installDebuggerCommand(SmartBody::SBCommandManager& commandManager) {
	commandManager.insert( "sbmdebugger", mcu_sbmdebugger_func );
}
}

