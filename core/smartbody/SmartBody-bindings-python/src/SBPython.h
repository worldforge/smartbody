#ifndef _SBMPYTHON_
#define _SBMPYTHON_

#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <functional>

SBAPI void initPython(std::string pythonLibPath);
SBAPI void setupPython();

extern std::vector<std::function<void()>> pythonExtraModuleDeclarations;

#endif