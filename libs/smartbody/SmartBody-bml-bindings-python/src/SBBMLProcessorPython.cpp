/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>

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

#include "SBBMLProcessorPython.h"
#include "sb/SBBmlProcessor.h"
#include <boost/python.hpp>

using namespace SmartBody;

void initPythonBML() {
	boost::python::class_<SBBmlProcessor, boost::noncopyable>("BmlProcessor", boost::python::no_init)
			.def("execBML", &SBBmlProcessor::execBML, boost::python::return_value_policy<boost::python::return_by_value>(), "Execute a generic BML instruction to a given character. Adds the <?xml..> and <act><bml>...</bml></act> elements.")
			.def("execBMLFile", &SBBmlProcessor::execBMLFile, boost::python::return_value_policy<boost::python::return_by_value>(), "Execute the BML instructions contained in a file for a given character.")
			.def("execBMLFileId", &SBBmlProcessor::execBMLFileId, boost::python::return_value_policy<boost::python::return_by_value>(), "Execute the BML instructions contained in a file for a given character.")
			.def("execXML", &SBBmlProcessor::execXML, boost::python::return_value_policy<boost::python::return_by_value>(), "Execute a generic XML instruction to a given character. Adds the <?xml..> header.")
			.def("execXMLFile", &SBBmlProcessor::execXMLFile, boost::python::return_value_policy<boost::python::return_by_value>(), "Execute a generic XML instructions contained in a file to a given character.")
			.def("execXMLFileId", &SBBmlProcessor::execXMLFileId, boost::python::return_value_policy<boost::python::return_by_value>(), "Execute a generic XML instructions contained in a file to a given character and pass a BML id.")
			.def("execBMLAt", &SBBmlProcessor::execBMLAt, "Execute a generic BML instruction to a given character at a time in the future. Adds the <?xml..> and <act><bml>...</bml></act> elements.")
			.def("execBMLFileAt", &SBBmlProcessor::execBMLFileAt, "Execute the BML instructions contained in a file for a given character at a time in the future..")
			.def("execXMLAt", &SBBmlProcessor::execXMLAt, "Execute a generic XML instruction to a given character at a time in the future.. Adds the <?xml..> header.")
			.def("interruptCharacter", &SBBmlProcessor::interruptCharacter, "Interrupts all BML behaviors associated with a given character at a future time in seconds (zero seconds means immediately).")
			.def("interruptBML", &SBBmlProcessor::interruptBML, "Interrupts a specific BML block behaviors associated with a given character at a future time in seconds (zero seconds means immediately).")

			;
}