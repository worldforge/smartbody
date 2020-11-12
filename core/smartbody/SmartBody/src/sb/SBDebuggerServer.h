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

#include <vhcl.h>
#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <sb/SBService.h>

namespace SmartBody {

class SBJoint;
class SBScene; 

class SBDebuggerServer : public SBService
{
	public:
		SBAPI SBDebuggerServer();
		SBAPI virtual ~SBDebuggerServer();

		SBAPI virtual void setEnable(bool val);

		SBAPI virtual void afterUpdate(double time);

		SBAPI virtual void notify(SBSubject* subject);

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
		std::vector<std::string> m_processIdList;
		std::string m_sbmFriendlyName;
		std::string m_hostname;
		int m_port;
		std::string m_fullId;
		bool m_connectResult;
		double m_updateFrequencyS;
		double m_lastUpdate;
		vhcl::Timer m_timer;

public:


};

}

