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

#ifndef _SBPAWN_H_
#define _SBPAWN_H_

#include <sb/SBTypes.h>
#include <sb/sbm_pawn.hpp>

namespace SmartBody {

class SBSkeleton;
class SBPhysicsSim;

class SBPawn : public SbmPawn
{
	public:
		SBAPI SBPawn();
		SBAPI explicit SBPawn( const char* name );
		SBAPI ~SBPawn() override;

		SBAPI void setName(const std::string& name) override;

		SBAPI virtual boost::intrusive_ptr<SmartBody::SBSkeleton> getSkeleton() const;
		SBAPI virtual void setSkeleton(boost::intrusive_ptr<SmartBody::SBSkeleton> skel);

		SBAPI SrVec getPosition();
		SBAPI SrQuat getOrientation();
		SBAPI void setPosition(const SrVec& pos);
		SBAPI void setPositionSmooth(const SrVec& pos, float smoothTime);
		SBAPI void setOrientation(const SrQuat& quat);
		SBAPI void setHPR(const SrVec& hpr);
		SBAPI void setHPRSmooth(const SrVec& hpr, float smoothTime);
		SBAPI SrVec getHPR();

		SBAPI SBPhysicsObj* getPhysicsObject();
//		SBAPI DeformableMeshInstance* getActiveMesh();
		SBAPI void createMeshFromCollisionSurface(std::string name, SrVec color);

		SBAPI void afterUpdate(double time) override;
		SBAPI void notify(SBSubject* subject) override;
		SBAPI virtual void copy(SBPawn* orignalPawn);

	private:
		SmartBody::DoubleAttribute* _posX;
		SmartBody::DoubleAttribute* _posY;
		SmartBody::DoubleAttribute* _posZ;
		SmartBody::DoubleAttribute* _rotX;
		SmartBody::DoubleAttribute* _rotY;
		SmartBody::DoubleAttribute* _rotZ;
		SrVec initialHPR, initialPos;
		SrVec targetHPR, targetPos;
		float hprStartTime, posStartTime;
		float hprEndTime, posEndTime;		
		bool  smoothTargetHPR, smoothTargetPos;
};

};

#endif
