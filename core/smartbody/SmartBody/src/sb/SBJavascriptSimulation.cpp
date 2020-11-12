
#include "SBJavascript.h"
#include "SBJavascriptClass.h"
#include <sb/SBSimulationManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBEvent.h>

#include <sr/sr_box.h>
#include <sr/sr_camera.h>
#include <stdlib.h>
#include <sbm/GenericViewer.h>
#include <controllers/me_ct_motion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <functional>

#if defined(EMSCRIPTEN)
#ifndef SB_NO_JAVASCRIPT
#include <emscripten/bind.h>
#include <emscripten.h>
#endif


#include "SBJavascriptInternal.h"

typedef std::map<std::string,SrQuat> QuatMap;
typedef std::map<std::string,SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;

namespace SmartBody
{
#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsSimulation()
	{

		emscripten::class_<SBSimulationManager>("SBSimulationManager")
			.function("isRunning", &SBSimulationManager::isRunning)
			.function("isStarted", &SBSimulationManager::isStarted)
			.function("printInfo", &SBSimulationManager::printInfo)
			.function("printPerf", &SBSimulationManager::printPerf)
			.function("getTime", &SBSimulationManager::getTime)
			.function("getTimeDt", &SBSimulationManager::getTimeDt)
			.function("setTime", &SBSimulationManager::setTime)
			.function("start", &SBSimulationManager::start)
			.function("stop", &SBSimulationManager::stop)
			.function("reset", &SBSimulationManager::reset)
			.function("pause", &SBSimulationManager::pause)
			.function("resume", &SBSimulationManager::resume)
			.function("setupTimer", &SBSimulationManager::setupTimer)
			.function("setSleepFps", &SBSimulationManager::setSleepFps)
			.function("setEvalFps", &SBSimulationManager::setEvalFps)
			.function("setSimFps", &SBSimulationManager::setSimFps)
			.function("setSleepDt", &SBSimulationManager::setSleepDt)
			.function("setEvalDt", &SBSimulationManager::setEvalDt)
			.function("setSimDt", &SBSimulationManager::setSimDt)
			.function("setSpeed", &SBSimulationManager::setSpeed)
			;

		emscripten::class_<SBProfiler>("Profiler")
			.function("printLegend", &SBProfiler::printLegend)
			.function("printStats", &SBProfiler::printStats)
			.function("printReport", &SBProfiler::printReport)
			;


		emscripten::class_<SrViewer>("Viewer")
			.function("show", &SrViewer::show_viewer)
			.function("hide", &SrViewer::hide_viewer)
			;

		emscripten::class_<GenericViewer>("GenericViewer")
			.function("show", &GenericViewer::show_viewer)
			.function("hide", &GenericViewer::hide_viewer)
			;

	}
#endif
}
#endif
