/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>
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

#include "SBNavMeshHelper.h"
#include "sr/sr_model.h"
#include "sbm/sbm_deformable_mesh.h"
#include "sb/SBNavigationMesh.h"

namespace SmartBody {
SBAPI bool SBNavMeshHelper::buildNavigationMesh(SBRenderAssetManager& renderAssetManager, SBNavigationMesh& navigationMesh, std::string meshName) {
	auto mesh = renderAssetManager.getDeformableMesh(meshName);
	if (!mesh)
		return false;

	if (mesh->dMeshStatic_p.size() == 0)
		return false;
	SrModel& inMesh = mesh->dMeshStatic_p[0]->shape();
	// this is a quick hack to get the API working, but should have a more robust way
	// to build the navigation from all meshes inside a deformable model.
	return navigationMesh.buildNavigationMeshFromModel(inMesh);
}
}
