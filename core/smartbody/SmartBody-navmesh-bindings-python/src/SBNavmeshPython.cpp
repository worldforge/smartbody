/*
 Copyright (C) 2020 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "SBNavmeshPython.h"
#include "sb/SBNavigationMesh.h"
#include "sb/SBNavigationMeshManager.h"
#include <boost/python.hpp>

using namespace SmartBody;

void initPythonNavmesh() {
	boost::python::class_<SBNavigationMesh>("SBNavigationMesh")
//.def("buildNavigationMesh", &SBNavigationMesh::buildNavigationMesh, "Build the navigation mesh based on input mesh name")
			.def("findPath", &SBNavigationMesh::findPath, boost::python::return_value_policy<boost::python::return_by_value>(), "Find a path from start position to end position.")
			.def("queryMeshPointByRayCast", &SBNavigationMesh::queryMeshPointByRayCast, boost::python::return_value_policy<boost::python::return_by_value>(), "Query the position on navigation mesh by ray casting.")
			.def("queryFloorHeight", &SBNavigationMesh::queryFloorHeight, boost::python::return_value_policy<boost::python::return_by_value>(), "Query the height of floor on navigation mesh given a position and search radius.");

	boost::python::class_<SBNavigationMeshManager>("SBNavigationMeshManager")
			.def("createNavigationMesh", &SBNavigationMeshManager::createNavigationMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a navigation mesh")
			.def("getNavigationMesh", &SBNavigationMeshManager::getNavigationMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the navigation mesh based on its name.")
			.def("getNavigationMeshNames", &SBNavigationMeshManager::getNavigationMeshNames, "Get the names of all navigation meshes.")
			;
}