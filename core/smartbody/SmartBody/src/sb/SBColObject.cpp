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

#include "SBColObject.h"
#include "SBPhysicsSim.h"
#include "gwiz_math.h"
#include <ode/collision.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBScene.h>

SrVec SBTransform::localToGlobal( const SrVec& vLocal )
{
	return vLocal*rot + tran;	
}

SrVec SBTransform::globalToLocal( const SrVec& vGlobal )
{
	return (vGlobal-tran)*rot.inverse();
}

SrMat SBTransform::gmat() const
{
	SrMat mat;
	mat = rot.get_mat(mat);
	mat.setl4(tran);
	return mat;
}

void SBTransform::gmat(const SrMat& inMat )
{
	tran = inMat.get_translation();
	rot  = SrQuat(inMat);
}

SBTransform SBTransform::diff( const SBTransform& r1, const SBTransform& r2 )
{
	SBTransform rout;
	rout.tran = r2.tran - r1.tran;
	rout.rot  = r1.rot.inverse()*r2.rot;	
	rout.rot.normalize();
	return rout;
}

SBTransform SBTransform::mult( const SBTransform& r1, const SBTransform& r2 )
{
	SBTransform rout;
	SrMat g1 = r1.gmat();
	SrMat g2 = r2.gmat();
	rout.gmat(g1*g2);
	//rout.tran = r1.tran*r2.rot + r2.tran;
	//rout.rot  = r1.rot*r2.rot;
	//rout.rot.normalize();
	return rout;
}


SBTransform SBTransform::blend( SBTransform& r1, SBTransform& r2, float weight )
{
	SBTransform rout;
	rout.tran = r1.tran*(1.f-weight) + r2.tran*weight;
	rout.rot  = slerp( r1.rot, r2.rot, weight );
	rout.rot.normalize();
	return rout;
}

SBTransform& SBTransform::operator=( const SBTransform& rt )
{
	tran = rt.tran;
	rot  = rt.rot;
	return *this;
}

float SBTransform::dist( const SBTransform& r1, const SBTransform& r2 )
{
	SBTransform diffT = diff(r1,r2);
	return diffT.tran.norm();
}

void SBTransform::add( const SBTransform& delta )
{
	tran = tran + delta.tran;//curEffectorPos + offset;
	rot  = rot*delta.rot;//rotOffset;
	rot.normalize();
}

SBTransform::SBTransform()
{
	rot = SrQuat();
	tran = SrVec(0,0,0);
}

SBGeomObject::SBGeomObject(void)
{
	color.set(1.0f, 0.0f, 0.0f, 1.0f);
}

SBGeomObject::~SBGeomObject(void)
{
	
}

SrVec SBGeomObject::getCenter()
{
	return getCombineTransform().tran;
}

void SBGeomObject::attachToObj(SBTransformObjInterface* phyObj)
{
	attachedObj = phyObj;	
}

SBTransformObjInterface* SBGeomObject::getAttachObj()
{
	return attachedObj;
}

SBTransform& SBGeomObject::getCombineTransform()
{
	if (attachedObj)
		combineTransform = SBTransform::mult(localTransform,attachedObj->getGlobalTransform());
	else
		combineTransform = SBTransform::mult(localTransform,globalTransform);
	return combineTransform;	
}

std::unique_ptr<SBGeomObject> SBGeomObject::createGeometry(const std::string& type, SrVec size, const SrVec& from, const SrVec& to)
{
	if (type == "sphere")
	{
		return std::make_unique<SBGeomSphere>(size[0]);
	}
	else if (type == "box")
	{
		return std::make_unique<SBGeomBox>(SrVec(size[0],size[1],size[2]));
	}
	else if (type == "capsule")
	{	
		SrVec p1,p2;
		if (from == to && from == SrVec())
		{
			p1 = SrVec(0,-size[0],0); p2 = SrVec(0,size[0],0);
			return std::make_unique<SBGeomCapsule>(size[0]*2.f,size[1]);//new SBGeomCapsule(p1,p2,size[1]);//new SBGeomCapsule(size[0]*2.f,size[1]);
		}
		else
		{
			return std::make_unique<SBGeomCapsule>(from, to, size[1]);
		}				
	}	
	else
	{
		return std::make_unique<SBGeomNullObject>();
	}
}

void SBGeomObject::setLocalTransform( SBTransform& newLocalTran )
{
	localTransform = newLocalTran;
}

SBTransform& SBGeomObject::getGlobalTransform()
{
	if (attachedObj) 
		return attachedObj->getGlobalTransform();
	else
		return globalTransform;
}

void SBGeomObject::setGlobalTransform( SBTransform& newGlobalTran )
{
	if (attachedObj) 
		attachedObj->setGlobalTransform(newGlobalTran);
	else
		globalTransform = newGlobalTran;

}

bool SBGeomNullObject::estimateHandPosture( const SrQuat& naturalRot, SrVec& outHandPos, SrQuat& outHandRot, float offsetDist )
{
	outHandPos = getCenter(); outHandRot = naturalRot; return false;
}

SrBox SBGeomNullObject::getBoundingBox()
{
	SrVec point = getCombineTransform().tran;
	SrBox box(point);

	return box;
}

/************************************************************************/
/* Sphere collider                                                      */
/************************************************************************/

bool SBGeomSphere::isInside( const SrVec& gPos, float offset )
{
	SrVec lpos = getCombineTransform().globalToLocal(gPos);
	if (lpos.norm() < radius + offset)
		return true;
	return false;
}

SBGeomSphere::~SBGeomSphere()
{

}

bool SBGeomSphere::isIntersect( const SrVec& gPos1, const SrVec& gPos2, float offset)
{
	SrVec p1 = getCombineTransform().globalToLocal(gPos1);
	SrVec p2 = getCombineTransform().globalToLocal(gPos2);

	if ( (p1).norm() < radius || (p2).norm() < radius + offset )
		return true;

	SrVec p2p1 = p2-p1;
	SrVec p3p1 = -p1;
	float u = dot(p2p1,p3p1)/dot(p2p1,p2p1);
	if (u > 0.f && u < 1.f && (p1+p2p1*u).norm() < radius + offset)
		return true;

	return false;
}

bool SBGeomSphere::estimateHandPosture( const SrQuat& naturalRot, SrVec& outHandPos, SrQuat& outHandRot, float offsetDist )
{
	outHandPos = getCenter() + SrVec(0,radius+offsetDist,0)*naturalRot;
	outHandRot = naturalRot;
	return true;
}

float SBGeomSphere::getRadius()
{
	return radius;
}

void SBGeomSphere::setRadius(float val)
{
	radius = val;
}

SBGeomSphere::SBGeomSphere( float r )
{
	radius = r;
}

SrBox SBGeomSphere::getBoundingBox()
{
	SrBox box;

	
	SrVec center = getCenter();
	box.set(SrVec(center.x - radius, center.y - radius, center.z - radius), SrVec(center.x + radius, center.y + radius, center.z + radius));

	return box;
}

/************************************************************************/
/* Box collider                                                         */
/************************************************************************/

SBGeomBox::SBGeomBox( const SrVec& ext )
{
	extent = ext;	
}

SBGeomBox::SBGeomBox( SrBox& bbox )
{
	extent = (bbox.b - bbox.a)*0.5f;	
}

SBGeomBox::~SBGeomBox()
{

}

bool SBGeomBox::isInside( const SrVec& gPos, float offset )
{
	SrVec lpos = getCombineTransform().globalToLocal(gPos);
	
	if (lpos.x > -extent.x - offset && lpos.x < extent.x + offset && 
		lpos.y > -extent.y - offset && lpos.y < extent.y + offset &&
		lpos.z > -extent.z - offset && lpos.z < extent.z + offset)
		return true;

	return false;
}

bool SBGeomBox::isIntersect( const SrVec& gPos1, const SrVec& gPos2, float offset)
{
	SrVec p1 = getCombineTransform().globalToLocal(gPos1);
	SrVec p2 = getCombineTransform().globalToLocal(gPos2);

	SrVec d = (p2 - p1)*0.5f;    
	SrVec e = extent + SrVec(offset,offset,offset);    
	SrVec c = p1 + d ;    
	SrVec ad = d; 	ad.abs();
	// Returns same vector with all components positive    
	if (fabsf(c[0]) > e[0] + ad[0])        return false;    
	if (fabsf(c[1]) > e[1] + ad[1])        return false;    
	if (fabsf(c[2]) > e[2] + ad[2])        return false;      
	if (fabsf(d[1] * c[2] - d[2] * c[1]) > e[1] * ad[2] + e[2] * ad[1] + gwiz::epsilon10())        return false;    
	if (fabsf(d[2] * c[0] - d[0] * c[2]) > e[2] * ad[0] + e[0] * ad[2] + gwiz::epsilon10())        return false;    
	if (fabsf(d[0] * c[1] - d[1] * c[0]) > e[0] * ad[1] + e[1] * ad[0] + gwiz::epsilon10())        return false;                
	return true;
}

bool SBGeomBox::estimateHandPosture( const SrQuat& naturalRot, SrVec& outHandPos, SrQuat& outHandRot, float offsetDist )
{	
	SrVec yAxis = SrVec(0,1,0);
	yAxis = yAxis*naturalRot;
	SrVec ly = yAxis*getCombineTransform().rot.inverse();//worldState.globalToLocal(yAxis);
	SrVec axis[6] = {SrVec(1,0,0), SrVec(0,1,0), SrVec(0,0,1),SrVec(-1,0,0), SrVec(0,-1,0), SrVec(0,0,-1) };
	SrVec graspAxis;
	float minAngle = 100.f;	
	float objSize = -1;
	for (int i=0;i<6;i++)
	{
		float rotAngle = acosf(dot(ly,axis[i]));
		if (fabs(rotAngle) < minAngle)
		{
			minAngle = rotAngle;
			graspAxis = axis[i];
			objSize = extent[i%3];//dot(graspAxis,extent);
		}		
	}
	//sr_out << "minAngle = " << minAngle << "  , grasp axis = " << graspAxis << srnl;
	graspAxis = graspAxis*getCombineTransform().rot;
	SrVec rotAxis = cross(yAxis,graspAxis); rotAxis.normalize();
	SrQuat alignRot = SrQuat(rotAxis,minAngle);
	outHandRot = alignRot*naturalRot;	
	outHandPos = getCenter() + SrVec(0,objSize+offsetDist,0)*outHandRot;
	return true;
}

SrBox SBGeomBox::getBoundingBox()
{
	SrBox box;

	SrVec min(-extent[0] , -extent[1] , -extent[2] );
	SrVec max(extent[0] , extent[1] , extent[2] );


	SrVec finalMin = globalTransform.localToGlobal(min);
	SrVec finalMax = globalTransform.localToGlobal(max);
	//SmartBody::util::log("finalMin = %.3f %.3f %.3f, finalMax = %.3f %.3f %.3f", finalMin[0],finalMin[1],finalMin[2], finalMax[0],finalMax[1],finalMax[2]);
	if (this->attachedObj)
	{
		SrVec min = this->attachedObj->getGlobalTransform().localToGlobal(finalMin);
		SrVec max = this->attachedObj->getGlobalTransform().localToGlobal(finalMax);
		//SmartBody::util::log("min = %.3f %.3f %.3f, max = %.3f %.3f %.3f", min[0],min[1],min[2], max[0],max[1],max[2]);

		box.set(min, max);
	}
	else
	{
		box.set(finalMin, finalMax);
	}


	

	return box;
}

/************************************************************************/
/* Capsule collider                                                     */
/************************************************************************/

static float findPointDistOnLineSegment(const SrVec& Point, SrVec e[2], SrVec& closePt)
{
	SrVec vDiff = Point - e[0];
	SrVec vDir = e[1]-e[0]; 
	float fDist = vDir.norm(); vDir.normalize();
	float fDot = dot(vDiff,vDir);
	if (fDot < 0)
	{
		closePt = e[0];
	}
	else if (fDot > fDist)
	{
		closePt = e[1];
	}
	else
	{
		closePt = e[0] + vDir*fDot;
	}

	float fPointDist = (Point-closePt).norm();
	return fPointDist;
}

static float findLineSegmentDistOnLineSegment(SrVec e1[2], SrVec e2[2], SrVec& closePt)
{		
	SrVec u = e1[1]-e1[0];
	SrVec v = e2[1]-e2[0];
	SrVec w0 = e1[0]-e2[0];
	float a = dot(u,u);
	float b = dot(u,v);
	float c = dot(v,v);
	float d = dot(u,w0);
	float e = dot(v,w0);

	float sd,td;

	sd = td = (a*c-b*b);	
	float sc = (b*e - c*d);
	float tc = (a*e - b*d);

	if (sc < 0.0) {       // sc < 0 => the s=0 edge is visible
        sc = 0.0;
        tc = e;
        td = c;
    }
    else if (sc > sd) {  // sc > 1 => the s=1 edge is visible
            sc = sd;
            tc = e + b;
            td = c;
    }
    

    if (tc < 0.0) {           // tc < 0 => the t=0 edge is visible
        tc = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sc = 0.0;
        else if (-d > a)
            sc = sd;
        else {
            sc = -d;
            sd = a;
        }
    }
    else if (tc > td) {      // tc > 1 => the t=1 edge is visible
        tc = td;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sc = 0;
        else if ((-d + b) > a)
            sc = sd;
        else {
            sc = (-d + b);
            sd = a;
        }
    }

	float so = sc/sd;
	float to = tc/td;
	float dline = (w0 + u*so - v*to).norm();	
	return dline;
}

SBGeomCapsule::SBGeomCapsule( float len, float r )
{
	extent = len*0.5f;
	radius = r;
	endPts[0] = SrVec(0,0,-extent);
	endPts[1]   = SrVec(0,0,extent);
}

SBGeomCapsule::SBGeomCapsule( const SrVec& p1, const SrVec& p2, float r )
{
	extent = (p2-p1).norm()*0.5f;
	SrVec zAxis = SrVec(0,0,1);
	SrVec capAxis = (p2-p1); capAxis.normalize();
	if (capAxis.len() < gwiz::epsilon10())
		capAxis = SrVec(0,0,1);
	SrVec pos = (p2+p1)*0.5f;
	SrQuat rot = SrQuat(zAxis,capAxis);

	rot.normalize();
	localTransform.rot = rot;
	localTransform.tran = pos;

	//endPts[0] = SrVec(0,0,-extent);
	//endPts[1] = SrVec(0,0,extent);
	endPts[0] = p1*localTransform.gmat().inverse();//SrVec(0,0,-extent);
	endPts[1] = p2*localTransform.gmat().inverse();//SrVec(0,0,extent);
	radius = r;		
	//updateGlobalTransform(SrMat::id);
}

SBGeomCapsule::~SBGeomCapsule() = default;

void SBGeomCapsule::setGeomSize(SrVec& size) 
{ 
	extent = size[0]; radius = size[1]; 
	endPts[0] = SrVec(0,0,-extent);
	endPts[1]   = SrVec(0,0,extent);
}

bool SBGeomCapsule::isInside( const SrVec& gPos, float offset)
{
	SrVec lpos = getCombineTransform().globalToLocal(gPos);
	SrVec cPts;
	float dist = findPointDistOnLineSegment(lpos,endPts,cPts);	
	if (dist < radius + offset )
	{		
		return true;
	}
	return false;
}

bool SBGeomCapsule::isIntersect( const SrVec& gPos1, const SrVec& gPos2, float offset)
{
	SrVec lpos[2];
	SrVec cpt;
	lpos[0] = getCombineTransform().globalToLocal(gPos1);
	lpos[1] = getCombineTransform().globalToLocal(gPos2);
	
	float dist = findLineSegmentDistOnLineSegment(lpos,endPts,cpt);

	//printf("test capsule intersection\n");
	//printf("dist=%f\n",dist);
	if (dist < offset + radius)
	{
		//printf("intersect dist=%f\n",dist);
		return true;
	}
	return false;
}

bool SBGeomCapsule::estimateHandPosture( const SrQuat& naturalRot, SrVec& outHandPos, SrQuat& outHandRot, float offsetDist )
{
	SrVec capAxis = (endPts[1]-endPts[0]); capAxis.normalize();
	capAxis = capAxis*getCombineTransform().rot;
	SrVec handAxis = SrVec(0,1,0)*naturalRot; handAxis.normalize();
	SrVec handXAxis = SrVec(-1,0,0)*naturalRot;	
	SrVec orienAxis = cross(handXAxis,capAxis); orienAxis.normalize();	

	SrVec crossHand = cross(handAxis,orienAxis); crossHand.normalize();
	if (dot(crossHand,handXAxis) < 0.0 && dot(handAxis,orienAxis) < 0.7)
		orienAxis = -orienAxis;
	
	SrQuat rot = SrQuat(handAxis,orienAxis);
	outHandRot = rot*naturalRot;//naturalRot*rot;
	outHandPos = getCenter() + SrVec(0,radius+offsetDist,0)*outHandRot;
	return true;
}

SrBox SBGeomCapsule::getBoundingBox()
{
	SrBox box;

	box.set(SrVec(), SrVec());

	return box;
}



SBGeomContact& SBGeomContact::operator=( const SBGeomContact& rt ) = default;


bool SBCollisionUtil::checkIntersection( SBGeomObject* obj1, SBGeomObject* obj2 )
{
	if (dynamic_cast<SBGeomSphere*>(obj1))
	{
		auto* sph = dynamic_cast<SBGeomSphere*>(obj1);
		return obj2->isInside(obj1->getCombineTransform().tran,sph->radius);
	}
	else if (dynamic_cast<SBGeomCapsule*>(obj1))
	{
		auto* cap = dynamic_cast<SBGeomCapsule*>(obj1);
		SrVec g1,g2;
		g1 = cap->endPts[0]*cap->getCombineTransform().gmat();
		g2 = cap->endPts[1]*cap->getCombineTransform().gmat();
		return obj2->isIntersect(g1,g2,cap->radius);		
	}
	else if (dynamic_cast<SBGeomBox*>(obj1))
	{
		return false;
	}
	else
	{
		return false;
	}
}



/************************************************************************/
/* Collision Space                                                      */
/************************************************************************/

SBCollisionSpace::SBCollisionSpace() = default;

SBCollisionSpace::~SBCollisionSpace() = default;

void SBCollisionSpace::addCollisionObjects( const std::string& objName )
{	
	SmartBody::SBCollisionManager* colManager = SmartBody::SBScene::getScene()->getCollisionManager();
	SBGeomObject* obj = colManager->getCollisionObject(objName);
	// remove the old one if it exists
	if (collsionObjMap.find(objName) != collsionObjMap.end())
	{
		removeCollisionObjects(objName);		
	}	
	collsionObjMap[objName] = obj;
}

void SBCollisionSpace::removeCollisionObjects( const std::string& objName  )
{
	SmartBody::SBCollisionManager* colManager = SmartBody::SBScene::getScene()->getCollisionManager();
	SBGeomObject* obj = colManager->getCollisionObject(objName);
	if (collsionObjMap.find(objName) != collsionObjMap.end())
	{
		collsionObjMap.erase(objName);					
	}		
}

void SBCollisionSpace::addExcludePair( const std::string& objName1, const std::string& objName2 )
{
	
}