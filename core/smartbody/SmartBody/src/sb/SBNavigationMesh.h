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

#ifndef _SBNAVIGATIONMESH_H_
#define _SBNAVIGATIONMESH_H_

#include <sb/SBTypes.h>
#include <sr/sr_model.h>
#include <sb/SBAsset.h>
#include <sb/SBObject.h>
#include <string>
#include <vector>
#include <map>

class dtNavMesh;
class dtNavMeshQuery;

namespace SmartBody {

class SBNavigationMesh : public SBObject
{
	public:
		SBAPI SBNavigationMesh();		
		SBAPI ~SBNavigationMesh();	
		//SBAPI bool buildNavigationMesh(std::string meshName);
		SBAPI bool buildNavigationMeshFromModel(SrModel& inMesh);			
		SBAPI SrModel* getRawMesh();
		SBAPI SrModel* getNavigationMesh();
		SBAPI float queryFloorHeight(SrVec pos, SrVec searchSize);
		SBAPI SrVec queryMeshPointByRayCast(SrVec& p1, SrVec& p2);
		SBAPI std::vector<SrVec> findPath(SrVec& spos, SrVec& epos);

		SBAPI void notify(SBSubject* subject) override;
	protected:
		void cleanUp();
	protected:
		float m_cellSize;
		float m_cellHeight;
		float m_agentHeight;
		float m_agentRadius;
		float m_agentMaxClimb;
		float m_agentMaxSlope;
		float m_regionMinSize;
		float m_regionMergeSize;
		bool m_monotonePartitioning;
		float m_edgeMaxLen;
		float m_edgeMaxError;
		float m_vertsPerPoly;
		float m_detailSampleDist;
		float m_detailSampleMaxError;
		SrModel* rawMesh; // the orignal model
		SrModel* naviMesh; // the navigation mesh built from original model		

		dtNavMesh* m_navMesh;
		dtNavMeshQuery* m_navQuery;
};

}


#endif