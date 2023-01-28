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

#ifndef _SBPHYSICSMANAGER_H_
#define _SBPHYSICSMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <boost/noncopyable.hpp>
#include <memory>

namespace SmartBody {

class SBPhysicsSim;

class SBPhysicsManager : public SBService
{		
	public:
		SBAPI explicit SBPhysicsManager(SBScene& scene, std::unique_ptr<SBPhysicsSim> physicsSim);
		SBAPI ~SBPhysicsManager();

		SBAPI void setEnable(bool enable) override;
		SBAPI bool isEnable() override;
		
		SBAPI void start() override;
		SBAPI void beforeUpdate(double time) override;
		SBAPI void update(double time) override;
		SBAPI void afterUpdate(double time) override;
		SBAPI void stop() override;

		SBAPI SBPhysicsSim* getPhysicsEngine();
				
		SBAPI SmartBody::SBObject* createPhysicsCharacter(const std::string& charName);
		SBAPI SmartBody::SBObject* createPhysicsPawn(const std::string& pawnName, const std::string& geomType, const SrVec& geomSize);

		SBAPI SmartBody::SBObject* getPhysicsSimulationEngine();
		SBAPI SmartBody::SBObject* getPhysicsCharacter(const std::string& charName);
		SBAPI SmartBody::SBObject* getPhysicsJoint(const std::string& charName, const std::string& jointName);
		SBAPI SmartBody::SBObject* getJointObj(const std::string& charName, const std::string& jointName);
		SBAPI SmartBody::SBObject* getPhysicsPawn(const std::string& pawnName);

		SBAPI void applyForceToPawn(const std::string& pawnName, const SrVec& force);
		SBAPI void applyForceToCharacter(const std::string& charName, const std::string& jointName, const SrVec& force);


	protected:
		std::unique_ptr<SBPhysicsSim> _physicsSim;

		double            physicsTime;
		void updatePhysicsCharacter(const std::string& charName);
		void updatePhysicsPawn(const std::string& pawnName);
;};

}

#endif
