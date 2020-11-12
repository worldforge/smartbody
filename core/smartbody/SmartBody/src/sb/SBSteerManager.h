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

#ifndef _STEERMANAGER_H_
#define _STEERMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBService.h>

class SteerSuiteEngineDriver;
namespace SteerLib
{
	class BoxObstacle;
}

namespace SmartBody {

class SBSteerAgent;

class SBSteerManager : public SmartBody::SBService
{
	public:
		SBAPI SBSteerManager();
		SBAPI ~SBSteerManager();
		
		SBAPI virtual void setEnable(bool enable);
		SBAPI virtual void start();
		SBAPI virtual void beforeUpdate(double time);
		SBAPI virtual void update(double time);
		SBAPI virtual void afterUpdate(double time);
		SBAPI virtual void stop();

		SBAPI virtual void onCharacterDelete(SBCharacter* character);

		SBAPI SteerSuiteEngineDriver* getEngineDriver();

		SBAPI SBSteerAgent* createSteerAgent(std::string name);
		SBAPI void removeSteerAgent(std::string name);
		SBAPI int getNumSteerAgents();
		SBAPI SBSteerAgent* getSteerAgent(std::string name);
		SBAPI std::vector<std::string> getSteerAgentNames();
		SBAPI std::map<std::string, SBSteerAgent*>& getSteerAgents();

	protected:
		std::map<std::string, SBSteerAgent*> _steerAgents;
		std::vector<SteerLib::BoxObstacle*> _boundaryObstacles;

		SteerSuiteEngineDriver* _driver;
		double _maxUpdateFrequency;


};

}

#endif 