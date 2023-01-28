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

void executeSafe(const std::function<void()>& fn);

extern std::vector<std::function<void()>> pythonExtraModuleDeclarations;

struct SBPython {
	static std::function<bool(SmartBody::SBScene& scene, const std::string&)> scriptRunner;
	static std::function<bool(SmartBody::SBScene& scene, const std::string&)> commandRunner;
};

#endif