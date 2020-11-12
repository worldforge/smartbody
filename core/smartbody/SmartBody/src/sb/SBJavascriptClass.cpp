
#include "SBJavascriptClass.h"
#include "controllers/me_ct_reach.hpp"


#include <sk/sk_skeleton.h>
#include <sk/sk_joint.h>
#include <sb/sbm_character.hpp>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <sbm/sbm_test_cmds.hpp>
#include <controllers/me_ct_param_animation.h>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_gaze.h>
#include <controllers/me_ct_eyelid.h>
#include <controllers/me_ct_face.h>
#include <controllers/me_ct_curve_writer.hpp>
#if defined(EMSCRIPTEN)
namespace SmartBody 
{
#ifndef SB_NO_JAVASCRIPT
	SrViewer* getViewer()
	{
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		if (!scene->getViewer())
		{

			if (scene->getViewerFactory())
			{
				scene->setViewer(scene->getViewerFactory()->create(0, 0, 1024, 768));
				scene->getViewer()->label_viewer("Visual Debugger");
				SmartBody::SBScene::getScene()->createCamera("cameraDefault");
			}
		}
		
		return scene->getViewer();
	}


	std::string JsLogger::strBuffer = "";


	void quitSbm()
	{
		SmartBody::SBScene::getScene()->getSimulationManager()->stop();
	}

	void reset()
	{
		SmartBody::SBScene::destroyScene();
	}

	void printlog(const std::string& message)
	{
		string s = message;
		if (s[s.size() - 1] == '\n')
			s.erase(s.length() - 1);
		if (s.length() > 0)
			SmartBody::util::log(message.c_str());
	}



	SBController* createController(std::string controllerType, std::string controllerName)
	{
		SBController* controller = NULL;

		if (controllerType == "schedule")
		{
			controller = new MeCtScheduler2();

		}
		else if (controllerType == "gaze")
		{
			controller = new MeCtGaze();
		}
		else if (controllerType == "eyelid")
		{
			controller = new MeCtEyeLidRegulator();
		}
		else if (controllerType == "face")
		{
			controller = new MeCtFace();
		}
		else if (controllerType == "paramanimation")
		{
			controller = new MeCtParamAnimation();
		}
		else if (controllerType == "curvewriter")
		{
			controller = new MeCtCurveWriter();
		}

		if (controller)
			controller->setName(controllerName);

		return controller;
	}

	SrCamera* getCamera()
	{
		return SmartBody::SBScene::getScene()->getActiveCamera();
	}


	////////////////////////////////////////////

	void JsLogger::pa()
	{
		strBuffer += "a";		
	}

	void JsLogger::pb()
	{
		strBuffer += "b";		
	}
	void JsLogger::pc()
	{
		strBuffer += "c";		
	}

	void JsLogger::pd()
	{
		strBuffer += "d";		
	}
	void JsLogger::pe()
	{
		strBuffer += "e";		
	}

	void JsLogger::pf()
	{
		strBuffer += "f";		
	}
	void JsLogger::pg()
	{
		strBuffer += "g";		
	}

	void JsLogger::ph()
	{
		strBuffer += "h";		
	}
	void JsLogger::pi()
	{
		strBuffer += "i";		
	}

	void JsLogger::pj()
	{
		strBuffer += "j";		
	}
	void JsLogger::pk()
	{
		strBuffer += "k";		
	}

	void JsLogger::pl()
	{
		strBuffer += "l";		
	}

	void JsLogger::pm()
	{
		strBuffer += "m";		
	}
	void JsLogger::pn()
	{
		strBuffer += "n";		
	}

	void JsLogger::po()
	{
		strBuffer += "o";		
	}
	void JsLogger::pp()
	{
		strBuffer += "p";		
	}

	void JsLogger::pq()
	{
		strBuffer += "q";		
	}
	void JsLogger::pr()
	{
		strBuffer += "r";		
	}

	void JsLogger::ps()
	{
		strBuffer += "s";		
	}
	void JsLogger::pt()
	{
		strBuffer += "t";		
	}

	void JsLogger::pu()
	{
		strBuffer += "u";		
	}
	void JsLogger::pv()
	{
		strBuffer += "v";		
	}

	void JsLogger::pw()
	{
		strBuffer += "w";		
	}
	void JsLogger::px()
	{
		strBuffer += "x";		
	}

	void JsLogger::py()
	{
		strBuffer += "y";		
	}
	void JsLogger::pz()
	{
		strBuffer += "z";		
	}
	void JsLogger::pspace()
	{
		strBuffer +=" ";
	}

	void JsLogger::p1()
	{
		strBuffer +="1";
	}
	void JsLogger::p2()
	{
		strBuffer +="2";
	}
	void JsLogger::p3()
	{
		strBuffer +="3";
	}
	void JsLogger::p4()
	{
		strBuffer +="4";
	}
	void JsLogger::p5()
	{
		strBuffer +="5";
	}
	void JsLogger::p6()
	{
		strBuffer +="6";
	}
	void JsLogger::p7()
	{
		strBuffer +="7";
	}
	void JsLogger::p8()
	{
		strBuffer +="8";
	}
	void JsLogger::p9()
	{
		strBuffer +="9";
	}
	void JsLogger::p0()
	{
		strBuffer +="0";
	}




	void JsLogger::pnon()
	{
		strBuffer +=".";
	}

	void JsLogger::outSmartBody::util::log()
	{
		if (strBuffer.size() > 1)
		{
			SmartBody::util::log("pyLog : %s",strBuffer.c_str());
			strBuffer = "";
		}	
	}
#endif

}
#endif