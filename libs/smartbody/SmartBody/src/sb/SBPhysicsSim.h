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

#ifndef _SBPhysicsSIM_H_ 
#define _SBPhysicsSIM_H_ 

#include <sb/SBTypes.h>
#include <deque>
#include "SBColObject.h"
#include <sb/SBObject.h>
#include <sb/SBJoint.h>

namespace SmartBody
{
class SBCharacter;
class SBPhysicsSim;

class SBPhysicsObjInterface : public SBTransformObjInterface
{
protected:
	bool          bHasPhysicsSim;
	bool          bHasCollisionSim;	
	SBTransform  globalTransform;	
	SBTransform  refTransform;

public:	
	SBAPI SBTransform& getGlobalTransform() override { return globalTransform; }
	SBAPI void setGlobalTransform(SBTransform& rt) override;

	SBAPI void setGlobalTransform(const SrMat& gmat);
	SBAPI SBTransform& getRefTransform() { return refTransform; }
	SBAPI void setRefTransform(const SrMat& gmat);
	SBAPI void setRefTransform(SBTransform& rt);

	SBAPI virtual void enablePhysicsSim(bool bPhy) = 0;
	SBAPI virtual void enableCollisionSim(bool bCol) = 0;

	SBAPI bool hasPhysicsSim() const { return bHasPhysicsSim; }
	SBAPI bool hasCollisionSim() const { return bHasCollisionSim; }
	SBAPI void hasPhysicsSim(bool phySim) { bHasPhysicsSim = phySim; }
	SBAPI void hasCollisionSim(bool colSim) { bHasCollisionSim = colSim; }
	SBAPI unsigned long getID();	
};

class SBPhysicsObj : public SBPhysicsObjInterface, public SmartBody::SBObject// abstraction for rigid objects in the physics engine
{
protected:
	SBPhysicsSim& physicsSim;
	std::unique_ptr<SBGeomObject> colObj;
	float         objDensity;
	SrVec         externalForce, externalTorque;
	SrVec         linearVel, angularVel;	
	bool          initGeom;
public:
	SBAPI explicit SBPhysicsObj(SBPhysicsSim& physicsSim);
	SBAPI ~SBPhysicsObj();

	SBAPI void enablePhysicsSim(bool bPhy) override;
	SBAPI void enableCollisionSim(bool bCol) override;
	SBAPI virtual void updateSbmObj();
	SBAPI virtual void updatePhySim();

	SBAPI void setGeometry(std::unique_ptr<SBGeomObject> obj);
	SBAPI void changeGeometry(std::string& geomType, const SrVec& extends);

	SBAPI SBGeomObject* getColObj() { return colObj.get(); }
	SBAPI float         getMass();	
	SBAPI void          setMass(float mass);
	SBAPI float         getDensity() const { return objDensity; }
	SBAPI void          setDensity(float density) { objDensity = density; }

	SBAPI SrVec& getExternalTorque() { return externalTorque; }
	SBAPI void   setExternalTorque(const SrVec& val) { externalTorque = val; }
	SBAPI SrVec& getExternalForce() { return externalForce; }
	SBAPI void   setExternalForce(const SrVec& val) { externalForce = val; }

	SBAPI SrVec getLinearVel();
	SBAPI void  setLinearVel(const SrVec& val);
	SBAPI SrVec getAngularVel();
	SBAPI void  setAngularVel(const SrVec& val);

	SBAPI void notify(SBSubject* subject) override;
	SBAPI virtual void handleCollision(SrVec contactPt, SBPhysicsObj* colObj);
};

class SbmJointObj;
class SBPhysicsCharacter;

class SBPhysicsJoint : public SmartBody::SBObject
{
protected:
	SBPhysicsSim& physicsSim;
	SBJoint* sbmJoint;
	SbmJointObj* parentObj;	
	SbmJointObj* childObj;
	SrQuat refQuat;	
	SrVec  refAngularVel;	
	SrVec jointTorque;
	float totalSupportMass; // all 	
public:
	SBAPI SBPhysicsJoint(SBPhysicsSim& physicsSim, SBJoint* joint);
	SBAPI ~SBPhysicsJoint();
	SBAPI SBJoint* getSBJoint() { return sbmJoint; }
	SBAPI SrQuat& getRefQuat() { return refQuat; }
	SBAPI void setRefQuat(const SrQuat& val) { refQuat = val; }
	SBAPI SrVec& getRefAngularVel() { return refAngularVel; }
	SBAPI void setRefAngularVel(const SrVec& val) { refAngularVel = val; }

	SBAPI SrVec& getJointTorque() { return jointTorque; }
	SBAPI void   setJointTorque(const SrVec& val) { jointTorque = val; }
	SBAPI SbmJointObj* getParentObj() const { return parentObj; }
	SBAPI void setParentObj(SbmJointObj* val) { parentObj = val; }
	SBAPI SbmJointObj* getChildObj() const { return childObj; }
	SBAPI void setChildObj(SbmJointObj* val) { childObj = val; }

	SBAPI unsigned long getID();

	SBAPI void notify(SBSubject* subject) override;

public:
	SBAPI void updateTotalSupportMass();
	SBAPI float getTotalSupportMass() const { return totalSupportMass; }
};

struct CollisionRecord
{
	SrVec collisionPt;
	SrVec momentum; 
	SbmJointObj* hitJointObj;
	SBPhysicsObj* collider;
public:
	CollisionRecord& operator= (const CollisionRecord& rt);
};

class SbmJointObj : public SBPhysicsObj 
// Modeling each body part as a SbmPhyObj
{
protected:	
	SBPhysicsCharacter* phyChar;
	SBPhysicsJoint* phyJoint;	
public:
	SBAPI explicit SbmJointObj(SBPhysicsSim& physicsSim, SBPhysicsCharacter* phyc);
	SBAPI ~SbmJointObj();
	SBAPI SrMat getRelativeOrientation();
	SBAPI SBPhysicsJoint* getPhyJoint() { return phyJoint; 	}
	SBAPI SBJoint*         getSBJoint() { return phyJoint->getSBJoint(); }
	SBAPI SBPhysicsCharacter* getPhysicsCharacter() { return phyChar; }

	SBAPI SBPhysicsJoint* getChildJoint(int i);
	SBAPI int getNumChildJoints();

	SBAPI SbmJointObj* getParentObj() { return phyJoint->getParentObj(); }
	//void setParentObj(SbmJointObj* parent) { parentObj = parent; }
	SBAPI virtual void initJoint(SBPhysicsJoint* joint);
	SBAPI void handleCollision(SrVec contactPt, SBPhysicsObj* colObj) override;
};


class SBPhysicsCharacter : public SBPhysicsObjInterface, public SmartBody::SBObject // interface for articulated dynamic character 
{
protected:
	SBPhysicsSim& physicsSim;
	SBPhysicsJoint* root;
	std::map<std::string, SBPhysicsJoint*> jointMap;
	std::map<std::string, SbmJointObj*>     jointObjMap;
	//std::map<std::string, SBGeomObject*>   jointGeometryMap;
	SBCharacter& character;
	std::vector<CollisionRecord> collisionRecords;
public:
	SBAPI SBPhysicsCharacter(SBPhysicsSim& physicsSim, SBCharacter& character);
	SBAPI void initPhysicsCharacter(std::vector<std::string>& jointNameList, bool buildGeometry = false);

	SBAPI void enablePhysicsSim(bool bPhy) override;
	SBAPI void enableCollisionSim(bool bCol) override;
	SBAPI std::vector<CollisionRecord>& getCollisionRecords() { return collisionRecords; }

	SBAPI std::string getPhysicsCharacterName();
	SBAPI SbmJointObj* getJointObj(const std::string& jointName); // get body part associated with this joint
	SBAPI SBPhysicsJoint* getPhyJoint(const std::string& jointName);
	SBAPI SBPhysicsJoint* getPhyJointRoot() { return root; }
	SBAPI std::vector<SbmJointObj*> getJointObjList();
	SBAPI std::vector<SBPhysicsJoint*> getPhyJointList();
	SBAPI std::map<std::string,SbmJointObj*>& getJointObjMap();
	SBAPI void notify(SBSubject* subject) override;
	SBAPI void updatePDTorque();
protected:
	void cleanUpJoints();
	std::unique_ptr<SBGeomObject> createJointGeometry(SBJoint* joint, float radius = -1);
	void updateJointAxis(SBPhysicsJoint* phyJoint);
	SrVec computePDTorque(SrQuat& q, SrQuat& qD, SrVec& w, SrVec& vD, float Ks, float Kd);
	SrVec computeSPDTorque(SrQuat& q, SrQuat& qD, SrVec& w, SrVec& vD, float Ks, float Kd, float dt, float mass);
};

//typedef std::deque<SBPhysicsObj*> SBPhysicsObjList;
typedef std::map<unsigned long, SBPhysicsJoint*> SBPhysicsJointMap;
typedef std::map<unsigned long,SBPhysicsObj*> SBPhysicsObjMap;
typedef std::map<std::string, SBPhysicsCharacter*> SBPhysicsCharacterMap;
typedef std::map<std::string, SBPhysicsObj*> SBPhysicsPawnMap;
class SBPhysicsSim : public SmartBody::SBObject
{
friend class SbmPhyObj;
protected:
	SBPhysicsObjMap physicsObjList;		
	SBPhysicsJointMap physicsJointList;
	SBPhysicsCharacterMap characterMap;
	SBPhysicsPawnMap      pawnObjMap;
public:
	SBAPI SBPhysicsSim();
	SBAPI virtual ~SBPhysicsSim();
	SBAPI void updateSimulation(float timestep);
	SBAPI void setEnable(bool enable);
	SBAPI void setGravity(float gravity);	
	SBAPI virtual bool hasPhysicsObj(SBPhysicsObj* obj);
	SBAPI virtual bool hasPhysicsCharacter(SBPhysicsCharacter* phyChar);	
	SBAPI SBPhysicsCharacterMap& getCharacterMap() { return characterMap; }
	SBAPI SBPhysicsPawnMap&       getPawnObjMap() { return pawnObjMap; }
	SBAPI SBPhysicsCharacter* getPhysicsCharacter(const std::string& charName);
	SBAPI SBPhysicsObj*       getPhysicsPawn(const std::string& pawnName);

	SBAPI virtual void addPhysicsObj(SBPhysicsObj* obj);
	SBAPI virtual void removePhysicsObj(SBPhysicsObj* obj);
	SBAPI virtual void addPhysicsCharacter(SBPhysicsCharacter* phyChar);	
	SBAPI virtual void removePhysicsCharacter(SBPhysicsCharacter* phyChar) = 0;	

	SBAPI virtual void enablePhysicsSim(SBPhysicsObj* obj, bool bSim) = 0;
	SBAPI virtual void enableCollisionSim(SBPhysicsObj* obj, bool bCol) = 0;	

	SBAPI virtual void writeToPhysicsObj(SBPhysicsObj* obj) = 0; // write sim data to colObj
	SBAPI virtual void readFromPhysicsObj(SBPhysicsObj* obj) = 0; // read sim data from colObj	

	SBAPI void updateAllPhysicsJoints();
	SBAPI virtual void updatePhysicsJoint(SBPhysicsJoint* phyJoint) = 0; // update joint parameters		
	SBAPI virtual void updatePhyObjGeometry(SBPhysicsObj* obj) = 0;

	//virtual void applyTorque(SBJoint* joint, )

	SBAPI virtual SrVec getJointConstraintPos(SBPhysicsJoint* joint) = 0;
	SBAPI virtual SrVec getJointRotationAxis(SBPhysicsJoint* joint, int axis) = 0;

	SBAPI virtual void updateSimulationInternal(float timeStep) = 0;
	SBAPI virtual SBPhysicsObj* createPhyObj() = 0;	

	SBAPI void notify(SBSubject* subject) override;
//	SBAPI static SBPhysicsSim* getPhysicsEngine();
};

}


#endif

