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


#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif

#include "MonitorConnectWindow.h"
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include "SBPythonClass.h"
#include <sb/SBVHMsgManager.h>
#include "Session.h"
#include "SBUtilities.h"

MonitorConnectWindow::MonitorConnectWindow(int x, int y, int w, int h, const char* label, bool quickConnect, SmartBody::SBDebuggerClient& debuggerClient)
: Fl_Double_Window(x, y, w, h, label),
mDebuggerClient(debuggerClient)
{
	set_modal();
	this->label("Monitor Connect");
	this->begin();

	_inputServerName = new Fl_Input(40, 10, 200, 20, "Server");
	_inputServerName->value("localhost");
//	_inputPort = new Fl_Input(40, 40, 200, 20, "Port");
//	_inputPort->value("61616");
//	_inputScope = new Fl_Input(40, 70, 200, 20, "Scope");
//	_inputScope->value("DEFAULT_SCOPE");

	_buttonLoad = new Fl_Button(40, 40, 100, 20, "Load");
	_buttonLoad->callback(OnFefreshCB, this);

	_browserSBProcesses = new Fl_Hold_Browser(10, 70, 300, 200, "Process");
	_buttonOk = new Fl_Button(10, 300, 100, 20, "OK");
	_buttonOk->callback(OnConfirmCB, this);
	_buttonCancel = new Fl_Button(120, 300, 100, 20, "Cancel");
	_buttonCancel->callback(OnCancelCB, this);
	this->end();

   if (quickConnect)
   {
      OnFefreshCB(nullptr, this);
      
	   if (_browserSBProcesses->value() <= 0)
	   {
		   fl_alert("There are currently no running SmartBody processed to connect to");
		   return;
	   }
      else
      {
         // there is an actively running sb process. Connect to it
         std::string processId = _browserSBProcesses->text(_browserSBProcesses->value());
         OnConfirmCB(nullptr, this);
      }
   }
}

MonitorConnectWindow::~MonitorConnectWindow() = default;

void MonitorConnectWindow::show()
{
	Fl_Double_Window::show();
}

void MonitorConnectWindow::hide()
{
	Fl_Double_Window::hide();
}

void MonitorConnectWindow::loadProcesses()
{
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	SmartBody::SBScene::getScene()->setRemoteMode(true);

	SmartBody::SBDebuggerServer& s = Session::current->debuggerServer;
	mDebuggerClient.QuerySbmProcessIds();
	vhcl::Sleep(2);
	Session::current->vhmMsgManager.poll();
	_browserSBProcesses->clear();
	std::vector<std::string> ids = mDebuggerClient.GetSbmProcessIds();
	for (auto & id : ids)
	{
		if (s.getStringAttribute("id")  != id)
			_browserSBProcesses->add(id.c_str());
	}

   if (!ids.empty())
   {
      _browserSBProcesses->select(1);
   }
}

void MonitorConnectWindow::OnConfirmCB(Fl_Widget* widget, void* data)
{
#ifndef SB_NO_VHMSG
	MonitorConnectWindow* monitorConnectWindow = (MonitorConnectWindow*) data;
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	SmartBody::SBDebuggerClient& c = monitorConnectWindow->mDebuggerClient;

	if (monitorConnectWindow->_browserSBProcesses->value() <= 0)
		return;

	std::string processId = monitorConnectWindow->_browserSBProcesses->text(monitorConnectWindow->_browserSBProcesses->value());
	if (processId.empty())
	{
		fl_alert("Please a process.");
		return;
	}

	c.Disconnect();
	c.Connect(processId);
	vhcl::Sleep(2);
	vhmsg::ttu_poll();
	if (c.GetConnectResult())
	{
		SmartBody::util::log("Connect succeeded to id: %s\n", processId.c_str());
	}
	c.Init();
	c.StartUpdates(sbScene->getSimulationManager()->getTimeDt());
	monitorConnectWindow->hide();
#else
	fl_alert("VHMSG has been disabled.");
#endif

}

void MonitorConnectWindow::OnCancelCB(Fl_Widget* widget, void* data)
{
	MonitorConnectWindow* monitorConnectWindow = (MonitorConnectWindow*) data;
	monitorConnectWindow->hide();
}


void MonitorConnectWindow::OnFefreshCB(Fl_Widget* widget, void* data)
{
	MonitorConnectWindow* monitorConnectWindow = (MonitorConnectWindow*) data;
	
	std::string command = "vhmsgconnect " + std::string(monitorConnectWindow->_inputServerName->value());
	//command = " " + std::string(monitorConnectWindow->_inputPort->value());
	//command = " " + std::string(monitorConnectWindow->_inputPort->value());
	SmartBody::SBScene::getScene()->command(command);
	monitorConnectWindow->loadProcesses();	
}
