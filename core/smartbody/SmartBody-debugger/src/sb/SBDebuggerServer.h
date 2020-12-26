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

#ifndef SBM_DEBUGGER_SERVER_H_
#define SBM_DEBUGGER_SERVER_H_

#include <vhcl.h>
#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <sb/SBService.h>
#include <functional>

namespace SmartBody {

class SBJoint;
class SBScene; 
class SBRenderScene;

class SBDebuggerServer : public SBService
{
	public:
		SBAPI explicit SBDebuggerServer(SBRenderScene& renderScene, std::function<void(std::ostream&, bool)> exportFn);
		SBAPI virtual ~SBDebuggerServer();

		SBAPI void setEnable(bool val) override;

		SBAPI void afterUpdate(double time) override;

		SBAPI void notify(SBSubject* subject) override;

	   void Update();

	   void GenerateInitHierarchyMsg(SmartBody::SBJoint * root, std::string & msg, int tab);

	   SBAPI void ProcessVHMsgs(const char * op, const char * args);

		vhcl::Vector3 m_cameraPos;
		vhcl::Vector3 m_cameraLookAt;
		vhcl::Vector4 m_cameraRot;
		double m_cameraFovY;
		double m_cameraAspect;
		double m_cameraZNear;
		double m_cameraZFar;
		bool m_rendererIsRightHanded;

	protected:
		void Init();
		void Close();


	private:
		SBRenderScene& m_renderScene;
		vhcl::Timer m_timer;
		std::vector<std::string> m_processIdList;
		std::string m_sbmFriendlyName;
		std::string m_hostname;
		int m_port;
		std::string m_fullId;
		bool m_connectResult;
		double m_updateFrequencyS;
		double m_lastUpdate;
		std::function<void(std::ostream&, bool)> m_exportFn;
public:


};

}
#endif

