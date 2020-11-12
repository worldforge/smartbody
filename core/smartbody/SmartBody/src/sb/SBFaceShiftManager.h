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

#ifndef _SBFACESHIFTMANAGER_H_
#define _SBFACESHIFTMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <sr/sr_quat.h>
#include <string>
#include <sb/SBSubject.h>

#ifdef WIN32
#include <WinSock2.h>
namespace SmartBody {


class SBFaceShiftManager : public SBService
{
	public:
		SBAPI SBFaceShiftManager();
		SBAPI ~SBFaceShiftManager();

		SBAPI virtual void setEnable(bool val);
		SBAPI virtual bool isEnable();
		SBAPI virtual void start();
		SBAPI virtual void stop();

		SBAPI void notify(SBSubject* subject);

		SBAPI void initConnection();
		SBAPI void stopConnection();
		SBAPI virtual void update(double time);
		SBAPI double getCoeffValue(const std::string& blendName);
		SBAPI SrQuat getHeadRotation();
		SBAPI std::vector<std::string>& getShapeNames();


	protected:
		SOCKET connectSocket;	
		std::vector<std::string> blendShapeNames;
		std::map<std::string, double> coeffTable;
		SrQuat headRotation;
		std::vector<std::string> shapeNames;
};

}

#else // no support for FaceShift in non-Windows build
namespace SmartBody {


	class SBFaceShiftManager : public SBService
	{
	public:
		SBAPI SBFaceShiftManager() {}
		SBAPI ~SBFaceShiftManager() {}

		SBAPI virtual void setEnable(bool val) {}
		SBAPI virtual bool isEnable() { return false; }
		SBAPI virtual void start() {}
		SBAPI virtual void stop() {}

		SBAPI void notify(SBSubject* subject) {}

		SBAPI void initConnection() {}
		SBAPI void stopConnection() {}
		SBAPI virtual void update(double time) {}
		SBAPI double getCoeffValue(const std::string& blendName) { return 0.0; }
		SBAPI SrQuat getHeadRotation() { return SrQuat(); }

	};

}
#endif

#endif
