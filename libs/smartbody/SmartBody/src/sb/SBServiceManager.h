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

#ifndef _SBSERVICEMANAGER_H_
#define _SBSERVICEMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBObject.h>
#include <string>
#include <map>

namespace SmartBody {

class SBService;

class SBServiceManager : public SBObject
{
	public:
		SBAPI SBServiceManager();
		SBAPI ~SBServiceManager();

		SBAPI int getNumServices();
		SBAPI std::vector<std::string> getServiceNames();

		SBAPI SBService* getService(const std::string& name);

		template<typename T>
		SBAPI T* getServiceByType(const std::string& name) {
			auto service = getService(name);
			return dynamic_cast<T*>(service);
		}
		SBAPI void addService(SBService* service);
		SBAPI void removeService(const std::string& name);

		SBAPI std::map<std::string, SBService*>& getServices();
		
	protected:
		std::map<std::string, SBService*> _services;

};

}


#endif