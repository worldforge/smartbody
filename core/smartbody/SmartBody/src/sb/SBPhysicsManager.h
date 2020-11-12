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

class ODEPhysicsSim;

namespace SmartBody {

class SBPhysicsSim;

class SBPhysicsManager : public SBService
{		
	public:
		SBAPI SBPhysicsManager();
		SBAPI ~SBPhysicsManager();

		SBAPI virtual void setEnable(bool enable);
		SBAPI virtual bool isEnable();
		
		SBAPI virtual void start();
		SBAPI virtual void beforeUpdate(double time);
		SBAPI virtual void update(double time);
		SBAPI virtual void afterUpdate(double time);
		SBAPI virtual void stop();

		SBAPI SBPhysicsSim* getPhysicsEngine();
				
		SBAPI SmartBody::SBObject* createPhysicsCharacter(std::string charName);
		SBAPI SmartBody::SBObject* createPhysicsPawn(std::string pawnName, std::string geomType, SrVec geomSize);

		SBAPI SmartBody::SBObject* getPhysicsSimulationEngine();
		SBAPI SmartBody::SBObject* getPhysicsCharacter(std::string charName);
		SBAPI SmartBody::SBObject* getPhysicsJoint(std::string charName, std::string jointName);
		SBAPI SmartBody::SBObject* getJointObj(std::string charName, std::string jointName);
		SBAPI SmartBody::SBObject* getPhysicsPawn(std::string pawnName);

		SBAPI void applyForceToPawn(std::string pawnName, SrVec force);
		SBAPI void applyForceToCharacter(std::string charName, std::string jointName, SrVec force);


	protected:
#ifndef SB_NO_ODE_PHYSICS
		ODEPhysicsSim* _ode;
#else
		SmartBody::SBPhysicsSim* _ode;
#endif

		double            physicsTime;
		void updatePhysicsCharacter(std::string charName);
		void updatePhysicsPawn(std::string pawnName);
;};

}

#endif
