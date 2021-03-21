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


#include "SBPhysicsManager.h"
#include "SBPawn.h"
#include "SBCharacter.h"
#include "SBSkeleton.h"
#include "SBPhysicsSim.h"
#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"
#include <chrono>

#include <utility>

#if defined(__ANDROID__) || defined(SB_IPHONE)
#define USE_PHYSICS_CHARACTER 0
#elif __native_client__
#define USE_PHYSICS_CHARACTER 0
#else
#define USE_PHYSICS_CHARACTER 1
#endif

namespace SmartBody {

SBPhysicsManager::SBPhysicsManager(SBScene& scene, std::unique_ptr<SBPhysicsSim> physicsSim)
		: SBService(scene), _physicsSim(std::move(physicsSim)) {
	setName("physics");

	physicsTime = 0;
}

SBPhysicsManager::~SBPhysicsManager() = default;

SBPhysicsSim* SBPhysicsManager::getPhysicsEngine() {
	if (_physicsSim) {
		return _physicsSim.get();
	} else {
		return nullptr;
	}
}


bool SBPhysicsManager::isEnable() {
	if (_physicsSim) {
		return _physicsSim->getBoolAttribute("enable");
	} else {
		return false;
	}
}

void SBPhysicsManager::setEnable(bool enable) {
	if (_physicsSim) {
		if (enable) {
			// ...
			physicsTime = _scene.getSimulationManager()->getTime();
		} else {
			// ...
		}
		_physicsSim->setBoolAttribute("enable", enable);
	}

}

void SBPhysicsManager::start() {
}

void SBPhysicsManager::beforeUpdate(double time) {

}

void SBPhysicsManager::update(double time) {
	if (_physicsSim) {
		SBPhysicsSim* physicsEngine = getPhysicsEngine();

		static double prevTime = -1;
		if (isEnable()) {
			float dt = (float) physicsEngine->getDoubleAttribute("dT");//timeStep*0.03f;
			float simLimit = (float) physicsEngine->getDoubleAttribute("MaxSimTime");
			double timeDiff = time - physicsTime;
			double timeElapse = 0.0;

			while (physicsTime < time && timeElapse < simLimit) {
				auto start = std::chrono::steady_clock::now();
				physicsEngine->updateSimulation(dt);
				physicsTime += dt;
				timeElapse += std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count();
			}
			physicsTime = time;
		} else {
			physicsTime = time;
		}

		// update character
		const std::vector<std::string>& characterNames = _scene.getCharacterNames();
		for (const auto& characterName : characterNames) {
			SmartBody::SBCharacter* character = _scene.getCharacter(characterName);
			//character->updateJointPhyObjs(isEnable());
			updatePhysicsCharacter(character->getName());
		}

		const std::vector<std::string>& pawns = _scene.getPawnNames();
		for (const auto& pawnIter : pawns) {
			SBPawn* pawn = _scene.getPawn(pawnIter);
			updatePhysicsPawn(pawn->getName());
		}
	}
}

void SBPhysicsManager::afterUpdate(double time) {
}

void SBPhysicsManager::stop() {
}

SmartBody::SBObject* SBPhysicsManager::getPhysicsSimulationEngine() {
	return getPhysicsEngine();
}

SmartBody::SBObject* SBPhysicsManager::getPhysicsCharacter(const std::string& charName) {
	if (_physicsSim) {
		return _physicsSim->getPhysicsCharacter(charName);
	} else {
		return nullptr;
	}

}

SmartBody::SBObject* SBPhysicsManager::getPhysicsJoint(const std::string& charName, const std::string& jointName) {
	if (_physicsSim) {
		SBPhysicsCharacter* phyChar = _physicsSim->getPhysicsCharacter(charName);
		SBPhysicsJoint* phyJoint = nullptr;
		if (phyChar) {
			phyJoint = phyChar->getPhyJoint(jointName);
		}
		return phyJoint;
	} else {
		return nullptr;
	}
}

SmartBody::SBObject* SBPhysicsManager::getJointObj(const std::string& charName, const std::string& jointName) {
	if (_physicsSim) {
		SBPhysicsCharacter* phyChar = _physicsSim->getPhysicsCharacter(charName);
		SbmJointObj* jointObj = nullptr;
		if (phyChar) {
			jointObj = phyChar->getJointObj(jointName);
		}
		return jointObj;
	} else {
		return nullptr;
	}
}

void SBPhysicsManager::applyForceToPawn(const std::string& pawnName, const SrVec& force) {
	if (_physicsSim) {
		SBPhysicsObj* phyObj = _physicsSim->getPhysicsPawn(pawnName);
		if (phyObj) {
			SmartBody::util::log("Find phyobj, name = %s", pawnName.c_str());
			phyObj->setExternalForce(force);
		}
	}
}

void SBPhysicsManager::applyForceToCharacter(const std::string& charName, const std::string& jointName, const SrVec& force) {
	SBPhysicsObj* phyObj = dynamic_cast<SBPhysicsObj*>(getJointObj(charName, jointName));
	if (phyObj) {
		phyObj->setExternalForce(force);
	}
}

SmartBody::SBObject* SBPhysicsManager::getPhysicsPawn(const std::string& pawnName) {
	if (_physicsSim) {
		return getPhysicsEngine()->getPhysicsPawn(pawnName);
	} else {
		return nullptr;
	}
}


SmartBody::SBObject* SBPhysicsManager::createPhysicsPawn(const std::string& pawnName, const std::string& geomType, const SrVec& geomSize) {
	if (_physicsSim) {
		SmartBody::SBPawn* pawn = _scene.getPawn(pawnName);
		if (!pawn) return nullptr;
		SBPhysicsObj* phyObj = pawn->getPhysicsObject();//new SBPhysicsObj();
		if (phyObj) getPhysicsEngine()->removePhysicsObj(phyObj); // remove existing physics object

		phyObj = new SBPhysicsObj(*_physicsSim);
		phyObj->setName(pawnName);
		auto geomObj = SBGeomObject::createGeometry(geomType, geomSize);
		phyObj->setGeometry(std::move(geomObj));
		//phyObj->changeGeometry(geomType,geomSize);
		getPhysicsEngine()->addPhysicsObj(phyObj);
		getPhysicsEngine()->updatePhyObjGeometry(phyObj);
		updatePhysicsPawn(pawnName);
		return phyObj;
	} else {
		return nullptr;
	}
}

SmartBody::SBObject* SBPhysicsManager::createPhysicsCharacter(const std::string& charName) {
	if (_physicsSim) {
		SmartBody::SBCharacter* sbmChar = _scene.getCharacter(charName);
		if (!sbmChar) return nullptr; // no character with the name
		auto phyChar = _physicsSim->getPhysicsCharacter(charName);
		if (phyChar)
			return phyChar; // physics character of the same name already exist

		// create physics character
		auto& joints = sbmChar->getSkeleton()->joints();
		//printf("init physics obj\n");
		phyChar = new SBPhysicsCharacter(*_physicsSim, *sbmChar);
		std::queue<SkJoint*> tempJointList;
		std::vector<std::string> jointNameList;
		std::set<std::string> excludeNameList;
		excludeNameList.insert("r_wrist");
		excludeNameList.insert("l_wrist");
		excludeNameList.insert("spine5");
		excludeNameList.insert("l_forefoot");
		excludeNameList.insert("r_forefoot");
		excludeNameList.insert("l_ankle");
		excludeNameList.insert("r_ankle");

		SkJoint* rootJoint = sbmChar->getSkeleton()->root();
		tempJointList.push(rootJoint->child(0));

		while (!tempJointList.empty()) {
			SkJoint* joint = tempJointList.front();
			tempJointList.pop();
			std::string jName = joint->getMappedJointName();
			if (joint->num_children() == 0) // don't process leaves
				continue;
			jointNameList.emplace_back(jName);
			if (excludeNameList.find(jName) != excludeNameList.end())
				continue;
			for (int i = 0; i < joint->num_children(); i++) {
				SkJoint* cj = joint->child(i);
				if (std::find_if(joints.begin(), joints.end(), [cj](const std::unique_ptr<SkJoint>& entry){ return entry.get() == cj;}) != joints.end())
					tempJointList.push(cj);
			}
		}
		phyChar->initPhysicsCharacter(jointNameList, true);
#if USE_PHYSICS_CHARACTER
		_physicsSim->addPhysicsCharacter(phyChar);
#endif
		return phyChar;
	} else {
		return nullptr;
	}
}

void SBPhysicsManager::updatePhysicsPawn(const std::string& pawnName) {
	if (_physicsSim) {
		SmartBody::SBPhysicsSim* phyEngine = getPhysicsEngine();
		SBPhysicsObj* phyObj = phyEngine->getPhysicsPawn(pawnName);
		SmartBody::SBPawn* pawn = _scene.getPawn(pawnName);
		if (!phyObj || !pawn) return;

		bool pawnPhySim = (phyEngine->getBoolAttribute("enable") && pawn->getBoolAttribute("enablePhysics"));
		if (pawnPhySim) {
			phyObj->updateSbmObj();
			pawn->setWorldOffset(phyObj->getGlobalTransform().gmat());
		} else {
			SRT newWorldState;
			//newWorldState.gmat(pawn->get_world_offset_joint()->gmat());
			newWorldState.gmat(pawn->get_world_offset());
			phyObj->setRefTransform(phyObj->getGlobalTransform()); // save previous transform
			phyObj->setGlobalTransform(newWorldState);
			phyObj->updatePhySim();
		}
	}
}

void SBPhysicsManager::updatePhysicsCharacter(const std::string& charName) {
	if (_physicsSim) {
		SBPhysicsSim* phyEngine = getPhysicsEngine();
		SBPhysicsCharacter* phyChar = phyEngine->getPhysicsCharacter(charName);
		if (!phyChar) return;

		bool charPhySim = (phyEngine->getBoolAttribute("enable") && phyChar->getBoolAttribute("enable"));
		std::map<std::string, SbmJointObj*> jointPhyObjMap = phyChar->getJointObjMap();
		for (auto& mi : jointPhyObjMap) {
			SbmJointObj* phyObj = mi.second;
			SBJoint* joint = phyObj->getSBJoint();
			const std::string& jointName = joint->getMappedJointName();
			bool kinematicRoot = (jointName == "base" || jointName == "JtPelvis") && phyChar->getBoolAttribute("kinematicRoot");
#if USE_PHYSICS_CHARACTER
			bool constraintObj = false;
			SBPawn* constraintPawn = _scene.getPawn(phyObj->getStringAttribute("constraintTarget"));
			if (charPhySim && constraintPawn && phyObj->getBoolAttribute("constraint")) {
				phyObj->enablePhysicsSim(false);
				phyObj->setRefTransform(phyObj->getGlobalTransform()); // save previous transform
				phyObj->setGlobalTransform(constraintPawn->get_world_offset());
				phyObj->setAngularVel(phyObj->getPhyJoint()->getRefAngularVel());
				phyObj->updatePhySim();
			} else if (charPhySim && phyObj->getBoolAttribute("constraint")) {
				SrMat tranMat;
				tranMat.translation(joint->getLocalCenter());
				phyObj->enablePhysicsSim(false);
				SrMat gmat = tranMat * phyObj->getRefTransform().gmat();
				phyObj->setRefTransform(phyObj->getGlobalTransform()); // save previous transform
				phyObj->setGlobalTransform(gmat);
				phyObj->setAngularVel(phyObj->getPhyJoint()->getRefAngularVel());
				phyObj->updatePhySim();
			} else if (charPhySim && !kinematicRoot && !constraintObj) {
				phyObj->enablePhysicsSim(true);
				phyObj->updateSbmObj();
			} else {
				SrMat tranMat;
				tranMat.translation(joint->getLocalCenter());
				phyObj->enablePhysicsSim(false);
				SrMat gmat = tranMat * joint->gmat();
				phyObj->setRefTransform(phyObj->getGlobalTransform()); // save previous transform
				phyObj->setGlobalTransform(gmat);
				phyObj->setAngularVel(phyObj->getPhyJoint()->getRefAngularVel());
				phyObj->updatePhySim();
			}
#else
			{
				SrMat tranMat; tranMat.translation(joint->getLocalCenter());
				SrMat gmat = tranMat*joint->gmat();
				phyObj->setGlobalTransform(gmat);
			}
#endif
		}
	}
}


}

