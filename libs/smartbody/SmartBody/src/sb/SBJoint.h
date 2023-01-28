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

#ifndef _SBJOINT_H_
#define _SBJOINT_H_

#include <sb/SBTypes.h>
#include <sk/sk_joint.h>
#include <sb/SBColObject.h>
#include <string>

namespace SmartBody {

class SBSkeleton;

class SBJoint : public SkJoint, public SBTransformObjInterface
{
	public:
		SBAPI SBJoint();
		SBAPI SBJoint( SkSkeleton* sk, SkJoint* parent, RotType rtype, int i );
		~SBJoint() override;

		SBAPI void setName(const std::string& name) override;
		SBAPI const std::string& getName() const override;

		SBAPI int getIndex();
		
		SBAPI void setParent(SBJoint* parent);
		SBAPI SBJoint* getParent();
		SBAPI int getNumChildren();
		SBAPI SBJoint* getChild(int index);
		SBAPI void addChild(SBJoint* child);
		SBAPI void removeChild(SBJoint* child);
		SBAPI std::vector<SBJoint*> getDescendants();
		SBAPI std::vector<SBJoint*> getAncestors();

		SBAPI void setSkeleton(SBSkeleton* skeleton);
		SBAPI SBSkeleton* getSkeleton();

		SBAPI SrVec getOffset();
		SBAPI void setOffset(SrVec vec);

		SBAPI SrVec getPosition();
		SBAPI SrQuat getQuaternion();
		SBAPI SrQuat getLocalRawQuaternion();
		SBAPI SrMat getMatrixGlobal();
		SBAPI SrMat getMatrixLocal();

		SBAPI void setUseRotation(bool val);
		SBAPI bool isUseRotation();
		SBAPI void setUsePosition(int index, bool val);
		SBAPI bool isUsePosition(int index);

		SBAPI void calculateLocalCenter();
		SBAPI const SrVec& getLocalCenter();

		SBAPI void setMass(float mass);
		SBAPI float getMass();

		SBAPI void setPrerotation(SrQuat& quat);
		SBAPI SrQuat getPrerotation();
		SBAPI void setPostrotation(SrQuat& quat);
		SBAPI SrQuat getPostrotation();
		SBAPI void setJointOrientation(SrQuat& quat);
		SBAPI SrQuat getJointOrientation();

		SBAPI SBTransform& getGlobalTransform() override;
		SBAPI void setGlobalTransform(SBTransform& newGlobalTransform) override;

		SBAPI void setAsfAxis(SrVec axis);
		SBAPI SrVec& getAsfAxis();
		SBAPI void setAsfChannels(std::vector<std::string>& channels);
		SBAPI std::vector<std::string>& getAsfChannels();

	protected:
		SrVec _localCenter;
		SBTransform globalTransform;
		SrVec _asfAxis;
		std::vector<std::string> _asfChannels;
};

};
#endif
