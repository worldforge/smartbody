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

#ifndef _SBCOLLISIONMANAGER_H_
#define _SBCOLLISIONMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <sb/SBSubject.h>
#include <sb/SBColObject.h>
#include <sk/sk_joint.h>
#include <memory>
#include <boost/noncopyable.hpp>
namespace SmartBody {

class SBCollisionManager : public SBService
{
	public:
		SBAPI explicit SBCollisionManager(SBScene& scene, std::unique_ptr<SBCollisionSpace> collisionSpace);
		SBAPI ~SBCollisionManager();

		SBAPI void setEnable(bool enable) override;
		SBAPI void start() override;
		SBAPI virtual bool getJointCollisionMode() { return _singleChrCapsuleMode; }
		SBAPI void beforeUpdate(double time) override;
		SBAPI void update(double time) override;
		SBAPI void afterUpdate(double time) override;
		SBAPI void stop() override;
		SBAPI void notify(SBSubject* subject) override;

		SBAPI void onCharacterDelete(SBCharacter* character) override;
		SBAPI void onPawnDelete(SBPawn* character) override;

		SBAPI SBGeomObject* createCollisionObject(const std::string& geomName, const std::string& geomType, SrVec size, SrVec from = SrVec(), SrVec to = SrVec());	
		SBAPI SBGeomObject* getCollisionObject(const std::string& geomName);
		SBAPI bool           removeCollisionObject(const std::string& geomName);
		SBAPI bool           addObjectToCollisionSpace(const std::string& geomName);
		SBAPI bool           removeObjectFromCollisionSpace(const std::string& geomName);
		SBAPI std::map<std::string, std::unique_ptr<SBGeomObject>>& getAllCollisionObjects();

	protected:
		std::unique_ptr<SBCollisionSpace> _collisionSpace;
		int _maxIterations;
		std::map<std::string, SrVec> _velocities;
		std::map<std::string, SrVec> _positions;
		std::map<std::string, std::unique_ptr<SBGeomObject>> geomObjectMap;

		bool _singleChrCapsuleMode;
		float _jointBVLenRadRatio;
		bool isJointExcluded(SkJoint* j, const std::vector<SkJoint*>& jnt_excld_list);
};


}
#endif