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

#ifndef SB_NO_ODE_PHYSICS
#ifndef _ODEPhysicsSim_H_
#define _ODEPhysicsSim_H_

#include <ode/ode.h>
#include <ode/common.h>
#include <sb/SBPhysicsSim.h>

class ODEObj
{
public:
	dBodyID bodyID;
	dGeomID geomID;
	dMass   odeMass;
	dTriMeshDataID meshdataID; // optional mesh data ID
	SmartBody::SBPhysicsObj* physicsObj; // rigid body
public:
	ODEObj();
	~ODEObj();
	void cleanGeometry();
};

class ODEJoint
{
public:
	dJointID jointID;
	dJointID aMotorID;
	dBodyID  parentID, childID;
	SmartBody::SBPhysicsJoint* joint;
public:
	ODEJoint();
	~ODEJoint();
};

typedef std::map<unsigned long,ODEObj*> ODEObjMap;
typedef std::map<unsigned long, ODEJoint*> ODEJointMap;

class ODEPhysicsSim :
	public SmartBody::SBPhysicsSim
{
public:
	dGeomID  groundID;
protected:
	dWorldID worldID;
	dSpaceID spaceID;		
	dJointGroupID contactGroupID;
	bool   hasInit;	
	ODEObjMap odeObjMap;
	ODEJointMap odeJointMap;
public:
	ODEPhysicsSim();
	~ODEPhysicsSim() override;
	dWorldID getWorldID() { return worldID; }
	dSpaceID getSpaceID() { return spaceID; }	
	dJointGroupID getContactGroupID() { return contactGroupID; }
	bool   systemIsInit() const { return hasInit; }
public:	
	void initSimulation() override;
	
	void addPhysicsObj(SmartBody::SBPhysicsObj* obj) override;
	void removePhysicsObj(SmartBody::SBPhysicsObj* obj) override;
	void addPhysicsCharacter(SmartBody::SBPhysicsCharacter* phyChar) override;
	void removePhysicsCharacter(SmartBody::SBPhysicsCharacter* phyChar) override;

	void updateSimulationInternal(float timeStep) override;
	SmartBody::SBPhysicsObj* createPhyObj() override;
	SrVec getJointConstraintPos(SmartBody::SBPhysicsJoint* joint) override;
	SrVec getJointRotationAxis(SmartBody::SBPhysicsJoint* joint, int axis) override;
	void updatePhysicsJoint(SmartBody::SBPhysicsJoint* phyJoint) override; // update joint parameters
	void updatePhyObjGeometry(SmartBody::SBPhysicsObj* obj) override;
	void enablePhysicsSim(SmartBody::SBPhysicsObj* obj, bool bSim) override;
	void enableCollisionSim(SmartBody::SBPhysicsObj* obj, bool bCol) override;
	void writeToPhysicsObj(SmartBody::SBPhysicsObj* obj) override; // write sim data to colObj
	void readFromPhysicsObj(SmartBody::SBPhysicsObj* obj) override; // read sim data from colObj

public:
	static dGeomID createODERawGeometry(SBGeomObject* geomObj); 	
	static void updateODEGeometryTransform(SBGeomObject* geomObj, dGeomID geomID);
	static ODEPhysicsSim* getODESim();
protected:
	static void updateODEMass(dMass& odeMass, SBGeomObject* geomObj, float mass);
	static void nearCallBack(void *data, dGeomID o1, dGeomID o2);
	ODEObj* getODEObj(SmartBody::SBPhysicsObj* obj);
	ODEJoint* getODEJoint(SmartBody::SBPhysicsJoint* joint);
	dGeomID createODEGeometry(SmartBody::SBPhysicsObj* obj, float mass);
	void linkJointObj(SmartBody::SbmJointObj* obj);
};

class ODECollisionSpace : public SBCollisionSpace
{
protected:
	dSpaceID spaceID;	
	std::map<std::string,dGeomID> odeGeomMap;
	std::map<dGeomID, std::string> odeGeomNameMap;	
	SbmCollisionPairList curCollisionPairs;
public:
	ODECollisionSpace();
	~ODECollisionSpace();

	void addCollisionObjects(const std::string& objName) override;
	void removeCollisionObjects(const std::string& objName) override;
	void getPotentialCollisionPairs(SbmCollisionPairList& collisionPairs) override;
	static  void collisionSpaceNearCallBack(void *data, dGeomID o1, dGeomID o2);
	std::string getODEGeomName(dGeomID geomID);
	SbmCollisionPairList& getCurrentCollisionPairList();
	void collisionDetection(SBGeomObject* obj1, SBGeomObject* obj2, std::vector<SBGeomContact>& contactPts);
protected:
	dGeomID getODEGeomID(const std::string& geomName);	
};

#endif
#endif

