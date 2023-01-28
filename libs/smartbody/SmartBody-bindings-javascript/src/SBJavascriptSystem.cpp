
#include "SBJavascript.h"
#include "SBJavascriptClass.h"
#include <sr/sr_camera.h>
#include <sb/SBScene.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBEvent.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <functional>

#if defined(EMSCRIPTEN)
#ifndef SB_NO_JAVASCRIPT
#include <sb/SBVersion.hpp>
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
	void javascriptFuncsSystem()
	{
		// viewers
		emscripten::function("getCamera", &getCamera, emscripten::allow_raw_pointers());
		emscripten::function("getViewer", &getViewer, emscripten::allow_raw_pointers());

		// system
		emscripten::function("reset", &reset);
		emscripten::function("quit", &quitSbm);	
		emscripten::function("getScene", &SBScene::getScene, emscripten::allow_raw_pointers());
		emscripten::function("getVersion", &SmartBody::getVersion);

	}
#endif
}
#endif
