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

#include "SBCollisionManager.h"
#include <sb/SBAttribute.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBSkeleton.h>
#include <sb/SBEvent.h>
#include "SBUtilities.h"
#include <boost/lexical_cast.hpp>
#include <sr/sr_color.h>


namespace SmartBody {

SBCollisionManager::SBCollisionManager(SBScene& scene, std::unique_ptr<SBCollisionSpace> collisionSpace)
: SBService(scene), _collisionSpace(std::move(collisionSpace))
{
	setName("collision");

	createIntAttribute("maxNumIterations", 5, true, "Basic", 60, false, false, false, "Max number collision response iterations.");

	std::vector<std::string> collisionHandles;
	collisionHandles.emplace_back("none");
	collisionHandles.emplace_back("default");
	SmartBody::StringAttribute* collisionHandleAttr = createStringAttribute("collisionResolutionType", "none", true, "Physics", 350, false, false, false, "Set the collision resolution method");
	collisionHandleAttr->setValidValues(collisionHandles);

	createBoolAttribute("singleChrCapsuleMode", false, true, "basic", 5, false, false, false, "Create single collision capsule for each character.");
	createDoubleAttribute("jointBVLenRadRatio", 4.0f, true, "Basic", 60, false, false, false, "Joint BV capsule length-radius ratio.");

	_maxIterations = 5;

	_singleChrCapsuleMode = false;
	_jointBVLenRadRatio = 4.0f;
}

SBCollisionManager::~SBCollisionManager() = default;

void SBCollisionManager::setEnable(bool enable)
{
	SBService::setEnable(enable);
	if (enable)
		start();
	else
		stop();
	SmartBody::SBAttribute* attribute = getAttribute("enable");
	if (attribute)
	{
		auto* enableAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		enableAttribute->setValueFast(enable);
	}
}

void SBCollisionManager::start()
{
	_singleChrCapsuleMode = getBoolAttribute("singleChrCapsuleMode");
	float jointBVLenRadRatio = (float)(getDoubleAttribute("jointBVLenRadRatio"));

	SR_CLIP(jointBVLenRadRatio, 0.001f, 1000.0f);
	_jointBVLenRadRatio = jointBVLenRadRatio;

	_positions.clear();
	_velocities.clear();
//	if (!collisionSpace)
//	{
//		collisionSpace = new ODECollisionSpace();
//
//	}
	const std::vector<std::string>& characterNames = _scene.getCharacterNames();
	for (const auto & characterName : characterNames)
	{
		_positions.insert(std::pair<std::string, SrVec>(characterName, SrVec()));
		_velocities.insert(std::pair<std::string, SrVec>(characterName, SrVec()));
		SBCharacter* character = _scene.getCharacter(characterName);
		if (!character->getGeomObject() || character->getGeomObject()->geomType() == "null") // no collision geometry setup for the character
		{
			if(_singleChrCapsuleMode)
			{
				//SBGeomObject* obj = new SBGeomCapsule()			
				SrBox bbox = character->getBoundingBox();
				auto radius = std::max(bbox.getSize().x / 2.0, bbox.getSize().y / 2.0);
				float yoffset = bbox.getMinimum().y - character->get_world_offset().get_translation().y;
				SrVec size = SrVec(0,radius,0);
				SBGeomObject* obj = createCollisionObject(character->getGeomObjectName(),"capsule",size,SrVec(0,yoffset,0),SrVec(0,yoffset+character->getHeight(),0));
				obj->attachToObj(character);
				addObjectToCollisionSpace(character->getGeomObjectName());
				//new SBGeomCapsule(SrVec(0,yoffset,0),SrVec(0,yoffset+character->getHeight(),0),_characterRadius);
				//character->setGeomObject(obj);
				//collisionSpace->addCollisionObjects(obj);
			}
			else // create collision capsules based on skel bones
			{
				SmartBody::util::log(character->getName().c_str());
				auto sk = character->getSkeleton();
				auto& origJnts = sk->joints();
				sk->update_global_matrices();
				std::vector<SkJoint*> jnt_excld_list;
				for(auto& joint : origJnts)
				{
					auto j = joint.get();
						SrString jname(j->jointName().c_str());
					if(jname.search("world_offset")>=0) { jnt_excld_list.emplace_back(j); continue; } // skip world_offset
					if(jname.search("face")>=0) { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("brow")>=0) { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("eye")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("nose")>=0) { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("lid")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("jaw")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("tongue")>=0) { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("lip")>=0)    { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("cheek")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("finger")>=0) { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("thumb")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("index")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("middle")>=0) { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("pinky")>=0)  { jnt_excld_list.emplace_back(j); continue; }
					if(jname.search("ring")>=0)   { jnt_excld_list.emplace_back(j); continue; }
				}
				std::string chrName = character->getGeomObjectName();
				float chrHeight = character->getHeight();
				for(auto& j : origJnts)
				{
						if(isJointExcluded(j.get(), jnt_excld_list)) continue;
					SrString jname(j->jointName().c_str());
					for(int k=0; k<j->num_children(); k++)
					{
						SkJoint* j_ch = j->child(k);
						if(isJointExcluded(j_ch, jnt_excld_list)) continue;
						const SrVec& offset = j_ch->offset();
						float offset_len = offset.norm();
						float radius = offset_len / jointBVLenRadRatio;
						if(offset_len < 0.03*chrHeight) continue; // skip short bones
						std::string colObjName = chrName + ":" + j->jointName();
						if(k>0) colObjName = colObjName + ":" + boost::lexical_cast<std::string>(k);
						SBGeomObject* obj = createCollisionObject(colObjName,"capsule",SrVec(0, radius, 0),SrVec::null,offset);
						SmartBody::util::log("SBColMan: col primitive added: %s, len: %f, radius: %f", colObjName.c_str(), offset_len, radius);
						obj->attachToObj(dynamic_cast<SBTransformObjInterface*>(j.get()));
						addObjectToCollisionSpace(colObjName);
					}
				}
			}
		}
	}

	const std::vector<std::string>& pawnNames = _scene.getPawnNames();
	for (const auto & pawnName : pawnNames)
	{
		SBPawn* pawn = _scene.getPawn(pawnName);
		if(pawn->getGeomObject()->geomType() != "null")
		{
			//SBGeomObject* obj = pawn->getGeomObject();
			SBGeomObject* obj = createCollisionObject(pawn->getGeomObjectName(),pawn->getGeomObject()->geomType(),pawn->getGeomObject()->getGeomSize(),SrVec::null,SrVec::null);
			obj->attachToObj(pawn);

			addObjectToCollisionSpace(pawn->getGeomObjectName());
		}
	}
}

bool SBCollisionManager::isJointExcluded(SkJoint* j, const std::vector<SkJoint*>& jnt_excld_list)
{
	for(auto i : jnt_excld_list)
		if(i == j)
			return true;
	return false;
}

void SBCollisionManager::beforeUpdate(double time)
{
}

void SBCollisionManager::update(double time)
{
}

void SBCollisionManager::afterUpdate(double time)
{	
	// determine if any of the characters are currently in collision
	// horribly inefficient n^2 implementation for characters only
	double timeDt = _scene.getSimulationManager()->getTimeDt();
	if (timeDt == 0.0)
		timeDt = .016;
	
	const std::vector<std::string>& characters = _scene.getCharacterNames();
	for (const auto & iter : characters)
	{
		SBCharacter* character =  _scene.getCharacter(iter);
		SrVec position = character->getPosition();
		position[1] = 0.0;
		SrVec& oldPosition = _positions[iter];
		_velocities[iter] = (position - oldPosition) / (float) timeDt;
		_positions[iter] = position;
	}

	int curIteration = 0;
	bool needMoreIterations = true;	

	SBEventManager* eventManager = _scene.getEventManager();	

	const std::string& collisionResMethod = getStringAttribute("collisionResolutionType");	

	while (needMoreIterations && curIteration < _maxIterations)
	{
		needMoreIterations = false;
		SbmCollisionPairList potentialCollisions;		
		_collisionSpace->getPotentialCollisionPairs(potentialCollisions);

		for (auto & potentialCollision : potentialCollisions)
		{
			// unfiltered
			//SmartBody::util::log("Collision Pair = %s %s",potentialCollisions[i].first.c_str(), potentialCollisions[i].second.c_str());
			SBGeomObject* g1 = getCollisionObject(potentialCollision.first);
			SBGeomObject* g2 = getCollisionObject(potentialCollision.second);

			// skip self-collision within skeleton/character's joints
			if(!_singleChrCapsuleMode) 
			{
			SBJoint* j1 = dynamic_cast<SBJoint*>(g1->getAttachObj());
			SBJoint* j2 = dynamic_cast<SBJoint*>(g2->getAttachObj());
			if(j1 && j2)
				if(j1->skeleton() == j2->skeleton()) continue; // belong to same character
			}
			
			// filtered, exclude all collisions within each character/skeleton
			//SmartBody::util::log("Potential Collision Pair: %s %s",potentialCollisions[i].first.c_str(), potentialCollisions[i].second.c_str());

			std::vector<SBGeomContact> contactPts;
			_collisionSpace->collisionDetection(g1,g2,contactPts);
			if (!contactPts.empty())
			{
				// collision handling here
				if(_singleChrCapsuleMode)
				{
					SBCharacter* c1 = dynamic_cast<SBCharacter*>(g1->getAttachObj());
					SBCharacter* c2 = dynamic_cast<SBCharacter*>(g2->getAttachObj());
					if (c1 && c2)
					{
						SBEvent* collisionEvent = eventManager->createEvent("collision",c1->getName()+"/"+c2->getName(), _scene.getStringFromObject(c1));
						eventManager->handleEvent(collisionEvent);
						//SmartBody::util::log("Collision detected between character %s and character %s",c1->getName().c_str(), c2->getName().c_str());
						delete collisionEvent; // free the memory

						// collision resolution
						if (collisionResMethod == "default")
						{						
							SrVec v1 = _velocities[c1->getName()];
							v1[1] = 0.0;
							SrVec v2 = _velocities[c2->getName()];
							v2[1] = 0.0;
							//SmartBody::util::log("v1 len = %f, v2 len = %f",v1.len(),v2.len());
							SBCharacter* cMove = (v1.len() > v2.len()) ? c1 : c2;				
							SBGeomContact& contact = contactPts[0];
							SrVec normalDir = (v1.len() > v2.len()) ? contact.contactNormal : -contact.contactNormal;
							normalDir[1] = 0.0;

							SrVec newPos = cMove->getPosition() + normalDir*contact.penetrationDepth;
							cMove->setPosition(newPos);
						}
						else if (collisionResMethod == "none")
						{
							// no collision resolution
						}
						else
						{
							// default ? 
						}					
					}
					needMoreIterations = true;
				}
				else 
				{
					// character joint <==> character joint (capsule) collision only
					//bool createEvent = true;
					//std::string obj1, obj2;
					//SBJoint* j1 = dynamic_cast<SBJoint*>(g1->getAttachObj());
					//if(j1) obj1 = j1->getName();
					//else
					//{
					//	SBCharacter* c1 = dynamic_cast<SBCharacter*>(g1->getAttachObj());
					//	if(c1) obj1 = c1->getName();
					//	else
					//		createEvent = false;
					//}
					//SBJoint* j2 = dynamic_cast<SBJoint*>(g2->getAttachObj());
					//if(j2) obj2 = j2->getName();
					//else
					//{
					//	SBCharacter* c2 = dynamic_cast<SBCharacter*>(g2->getAttachObj());
					//	if(c2) obj2 = c2->getName();
					//	else
					//		createEvent = false;
					//}
					//if(createEvent)
					//{
					//	Event* collisionEvent = eventManager->createEvent("collision",obj1+"/"+obj2);
					//	eventManager->handleEvent(collisionEvent);
					//	SmartBody::util::log("Collision detected between %s and %s", obj1.c_str(), obj2.c_str());
					//	delete collisionEvent; // free the memory
					//}

					const std::string& obj1 = potentialCollision.first;
					const std::string& obj2 = potentialCollision.second;

					//SmartBody::util::log("Collision detected between %s and %s", obj1.c_str(), obj2.c_str());

					SBEvent* collisionEvent = eventManager->createEvent("collision",obj1+"/"+obj2, potentialCollision.first);
					eventManager->handleEvent(collisionEvent);
					delete collisionEvent; // free the memory
				}
			}
		}
		curIteration++;
	}

		

#if 0
		for (std::vector<std::string>::iterator iter = characters.begin();
			 iter != characters.end();
			 iter++)
		{
			SBCharacter* character1 =  _scene.getCharacter((*iter));
			SrVec position1 = character1->getPosition();
			position1[1] = 0.0;
			
			for (std::vector<std::string>::iterator iter2 = iter + 1;
			 iter2 != characters.end();
			 iter2++)
			{
				SBCharacter* character2 =  _scene.getCharacter((*iter2));
				// determine if the two characters are in collision
				std::vector<SBGeomContact> contactPts;
				SBCollisionUtil::collisionDetection(character1->getGeomObject(),character2->getGeomObject(),contactPts);
				
				if (contactPts.size() > 0)
				{
					//SmartBody::util::log("Collision between character %s and character %s",character1->getName().c_str(), character2->getName().c_str());

				}

				
				SrVec position2 = character2->getPosition();
				position2[1] = 0.0;
				float distance = dist(position1, position2);
				if (distance < _characterRadius * 2)
				{
					float penetration = _characterRadius * 2 - distance;
					needMoreIterations = true;
					curIteration++;
					// collision resolution
					// who is moving faster?
					SrVec& velocity1 = _velocities[(*iter)];
					SrVec& velocity2 = _velocities[(*iter2)];
					float magnitude1 = velocity1.len();
					float magnitude2 = velocity2.len();
					// move the object with greater velocity
					if (magnitude1 > magnitude2)
					{
						SrVec diff = position1 - position2;
						diff.normalize();
						// move character1 in that direction
						position1 += diff * penetration;
						_positions[(*iter)] = position1;
						SrVec curPosition = character1->getPosition();
						curPosition.x = position1[0];
						curPosition.z = position1[2];
						character1->setPosition(curPosition);
					}
					else
					{
						SrVec diff = position2 - position1;
						diff.normalize();
						// move character1 in that direction
						position2 += diff * penetration;
						_positions[(*iter2)] = position2;
						SrVec curPosition = character2->getPosition();
						curPosition.x = position2[0];
						curPosition.z = position2[2];
						character2->setPosition(curPosition);
					}
				}				
				
			}			
		}
#endif
	

}

void SBCollisionManager::stop()
{
}

void SBCollisionManager::notify(SBSubject* subject)
{
	SBService::notify(subject);

// 	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
// 	if (!attribute)
// 	{
// 		return;
// 	}
// 
// 	const std::string& name = attribute->getName();
// 	if (name == "maxNumIterations")
// 	{
// 		_maxIterations = getIntAttribute("maxNumIterations");
// 		return;
// 	}
}

SBGeomObject* SBCollisionManager::createCollisionObject( const std::string& geomName, const std::string& geomType, SrVec size, SrVec from, SrVec to )
{	
	auto newObj = SBGeomObject::createGeometry(geomType,size,from,to);
	if (newObj)
	{
		removeCollisionObject(geomName); // remove existing one
		newObj->color.set(1.0f, 1.0f, 0.0f);
		auto result = geomObjectMap.emplace(geomName, std::move(newObj));
		if (result.second) {
			return result.first->second.get();
		}
	}
	return nullptr;
}

SBGeomObject* SBCollisionManager::getCollisionObject( const std::string& geomName )
{
	auto I = geomObjectMap.find(geomName);
	if (I != geomObjectMap.end()) {
		return I->second.get();
	}
	return nullptr;
}

bool SBCollisionManager::removeCollisionObject( const std::string& geomName )
{
	auto I = geomObjectMap.find(geomName);
	if (I != geomObjectMap.end()) {
		if (_collisionSpace) {
			_collisionSpace->removeCollisionObjects(*this, geomName);
		}
		geomObjectMap.erase(I);
		return true;
	}
	return false;
}

bool SBCollisionManager::addObjectToCollisionSpace( const std::string& geomName )
{
	SBGeomObject* geomObj = getCollisionObject(geomName);
	if (geomObj)
	{
		_collisionSpace->addCollisionObjects(*this, geomName);
		return true;
	}
	return false;

}

bool SBCollisionManager::removeObjectFromCollisionSpace( const std::string& geomName )
{
	SBGeomObject* geomObj = getCollisionObject(geomName);
	if (geomObj)
	{
		if (_collisionSpace)
			_collisionSpace->removeCollisionObjects(*this, geomName);
		return true;
	}
	return false;
}

std::map<std::string, std::unique_ptr<SBGeomObject>>& SBCollisionManager::getAllCollisionObjects()
{
	return geomObjectMap;
}


void SBCollisionManager::onCharacterDelete(SBCharacter* character)
{
	SBCollisionManager::removeObjectFromCollisionSpace(character->getGeomObjectName());
}

void SBCollisionManager::onPawnDelete(SBPawn* pawn)
{
	SBCollisionManager::removeObjectFromCollisionSpace(pawn->getGeomObjectName());
}

}


