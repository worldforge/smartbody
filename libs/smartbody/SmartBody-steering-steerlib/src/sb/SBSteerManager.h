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
#include <boost/noncopyable.hpp>
#include <memory>
#include "sb/sbm_pawn.hpp"
#include "sb/SBSceneListener.h"

class SteerSuiteEngineDriver;
class SbmCharacter;
namespace SteerLib
{
	class BoxObstacle;
}

namespace SmartBody {

class SBSteerAgent;
class SBScene;

struct PawnObstacle {
	SbmPawn& pawn;
	std::unique_ptr<SteerLib::BoxObstacle> obstacle;
	SrVec steeringSpaceObjSize;
};

class SBSteerManager : public SmartBody::SBService
{
	public:
		SBAPI explicit SBSteerManager(SBScene& scene);
		SBAPI ~SBSteerManager() override;

		SBAPI void setEnable(bool enable) override;
		SBAPI void start() override;
		SBAPI void beforeUpdate(double time) override;
		SBAPI void update(double time) override;
		SBAPI void afterUpdate(double time) override;
		SBAPI void stop() override;

		SBAPI void onPawnDelete(SBPawn* pawn) override;
		void onCharacterCreate(SBCharacter *character) override;
		SBAPI void onCharacterDelete(SBCharacter* character) override;

		SBAPI SteerSuiteEngineDriver* getEngineDriver();

		SBAPI SBSteerAgent* createSteerAgent(const std::string& name);
		SBAPI void removeSteerAgent(const std::string& name);
		SBAPI int getNumSteerAgents();
		SBAPI SBSteerAgent* getSteerAgent(const std::string& name);
		SBAPI std::vector<std::string> getSteerAgentNames();
//		SBAPI std::map<std::string, std::unique_ptr<SBSteerAgent>>& getSteerAgents();
		SBAPI std::map<std::string, PawnObstacle>& getObstacles() {
			return _pawnObstacles;
		}
		SBAPI const std::map<std::string, PawnObstacle>& getObstacles() const {
			return _pawnObstacles;
		}

		PawnObstacle* createObstacleForPawn(const std::string& name);

		void applyPawnObstacle(PawnObstacle& pawnObstacle);

		bool checkExamples(SbmCharacter& character);
		bool checkExamples(SBSteerAgent& steerAgent);

protected:
		struct CharacterObserver : public SBObserver {
			SBCharacter& character;
			SBSteerManager& steerManager;
			CharacterObserver(SBCharacter& character_, SBSteerManager& steerManager_);
			~CharacterObserver();
			void notify(SBSubject* subject) override;
		};

		struct SteerAgentEntry {
			std::unique_ptr<SBSteerAgent> agent;
			CharacterObserver observer;
		};

		std::map<std::string, SteerAgentEntry> _steerAgents;
		std::vector<std::unique_ptr<SteerLib::BoxObstacle>> _boundaryObstacles;
		std::map<std::string, PawnObstacle> _pawnObstacles;

		std::unique_ptr<SteerSuiteEngineDriver> _driver;
		double _maxUpdateFrequency;

		void updatePawnObstacle(PawnObstacle& pawnObstacle);


};

}

#endif 