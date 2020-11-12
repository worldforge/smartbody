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

#pragma once
#include <vector>
#include <sr/sr_vec.h>

class SteerPathBase
{
public:
	virtual SrVec closestPointOnPath(const SrVec& pt, SrVec& tangent, float& dist) = 0;
	virtual SrVec pathPoint(float length) = 0;
	virtual SrVec pathTangent(float length) = 0;	
	virtual float pathDistance(const SrVec& pt) = 0;
	virtual float pathLength() = 0;	

};

class SteerPath : public SteerPathBase// polyline path
{
protected:	
	std::vector<SrVec> pathPts;
	std::vector<SrVec> pathSegDir;
	std::vector<float> pathSegLength;
	float              pathRadius;
	unsigned int       currentGoal;
public:
	SteerPath(void);
	~SteerPath(void);
	
	void initPath(const std::vector<SrPnt>& pts, float radius);	
	void clearPath();
		
	virtual SrVec closestPointOnPath(const SrVec& pt, SrVec& tangent, float& dist);	
	virtual SrVec pathPoint(float length);
	virtual SrVec pathTangent(float length);
	virtual float pathDistance(const SrVec& pt);
	virtual float pathLength();
	virtual float pathCurvature(float start, float end);
	SrVec closestPointOnNextGoal(const SrVec& pt, SrVec& tangent, float& dist);		
	SrVec pathGoalPoint();
	bool atLastGoal();
	void advanceToNextGoal(float length);
};
