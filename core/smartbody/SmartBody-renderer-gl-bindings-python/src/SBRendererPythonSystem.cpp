

#include "sbm/SceneExporter.h"

#include <boost/python.hpp>

namespace SmartBody
{

void pythonFuncsSystem()
{
	boost::python::def("save", &save, "Saves the SmartBody configuration. Returns a string containing Python commands representing the configuration.");
	boost::python::def("exportScene", &exportScene, "Saves the entire SmartBody configuration, including assets paths as a python script into a given file location.");
	boost::python::def("exportScenePackage", &exportScenePackage, "Saves the entire SmartBody configuration and all assets into a given folder, or into a .zip archive");
	boost::python::def("exportCharacter", &exportCharacter, "Saves the character's skeleton, skinned mesh and all other assets into a given folder in Collada format. ");


}
}


