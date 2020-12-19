#ifndef _SBMPYTHON_
#define _SBMPYTHON_

#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <functional>

namespace SmartBody {
class SBScene;
}

SBAPI void initPython();

SBAPI void setupPython(SmartBody::SBScene& scene);

void executeSafe(const std::function<void()>& fn);

extern std::vector<std::function<void()>> pythonExtraModuleDeclarations;

#endif