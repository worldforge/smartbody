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

#include "PythonSetup.h"
#include "Session.h"
#include "SBPython.h"
#include "sb/SBSimulationManager.h"
#include "sb/SBAssetManager.h"

#include <boost/python.hpp>
void setupPython(Session& session)
{

	boost::python::object module = boost::python::import("__main__");
	boost::python::object dict  = module.attr("__dict__");
	auto smartBodyModule = boost::python::import("SmartBody");

	auto& scene = session.scene;
	scene.setCommandRunner(SBPython::commandRunner);

	scene.setScriptRunner(SBPython::scriptRunner);

	executeSafe([&](){
#ifdef PYLOG
		#if defined(__ANDROID__) || defined(SB_IPHONE)
		const char* pyfilename = "/sdcard/sbmmedia/pylog.txt";
#else
		const char* pyfilename = "C:\\SbmAndroid\\android\\pylog.txt";
#endif
		FILE* file = fopen(pyfilename,"rt");
		if (file)
		{
			SmartBody::util::log("Open file success\n");
			PyRun_SimpleFile(file,pyfilename);
			PyRun_SimpleString("logwriter = WritableObject()");
			//#ifndef __ANDROID__
			PyRun_SimpleString("sys.stdout = logwriter");
			PyRun_SimpleString("sys.stderr = logwriter");
		}
		else
		{
			SmartBody::util::log("Open File Fail!!!\n");
		}
#else
		boost::python::exec(
				"import sys\n"
				"class WritableObject:\n\tdef __init__(self):\n\t\tself.content = []\n\tdef write(self, string):\n\t\tprintlog(string)\n\n"
				"logwriter = WritableObject()\n"
				"sys.stdout = logwriter\n"
				"sys.stderr = logwriter\n"
				,dict);
#endif

		if (PyErr_Occurred())
			PyErr_Print();

		dict["scene"] = boost::python::ptr(&scene);
		dict["sim"] = boost::python::ptr(scene.getSimulationManager());
		dict["assets"] = boost::python::ptr(scene.getAssetManager());
		dict["steerManager"] = boost::python::ptr(&session.steerManager);
		dict["bml"] = boost::python::ptr(&session.bmlProcessor);
		boost::python::exec(
				"from os import *\n"
				"from SmartBody import *",
				dict);
	});

}
