
#include "SBPython.h"
#include "SBPythonClass.h"
#include "controllers/me_ct_scheduler2.h"
#include "controllers/me_ct_gaze.h"
#include "controllers/me_ct_eyelid.h"
#include "controllers/me_ct_curve_writer.hpp"
#include "sb/SBFaceDefinition.h"
#include <sb/nvbg.h>
#include "sb/SBBehavior.h"
#include <sb/SBParseNode.h>

#include <sb/SBScene.h>
#include <sb/SBScript.h>
#include <sb/SBService.h>
#include <sb/SBServiceManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBMotionBlendBase.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBSteerManager.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBReach.h>
#include <sb/SBReachManager.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBParser.h>
#include <sb/SBBoneBusManager.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBRealtimeManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBHandConfiguration.h>
#include <sb/SBEvent.h>
#include <sb/SBSceneListener.h>
#include "SBUtilities.h"
#include <cstdlib>
#include <controllers/me_ct_motion.h>
#include <controllers/me_ct_new_locomotion.h>
#include "sbm/xercesc_utils.hpp"

#include <boost/filesystem/operations.hpp>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>

#include "SBPythonInternal.h"

typedef std::map<std::string, SrQuat> QuatMap;
typedef std::map<std::string, SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;
typedef std::vector<SrVec> VecArray;


#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER == 190024210 || _MSC_FULL_VER == 190024215)
namespace boost
{
	template<> const volatile SmartBody::SBSubject* get_pointer(const volatile SmartBody::SBSubject* p) { return p; }
	template<> const volatile SmartBody::SBObserver* get_pointer(const volatile SmartBody::SBObserver* p) { return p; }
	template<> const volatile SmartBody::SBObject* get_pointer(const volatile SmartBody::SBObject* p) { return p; }
	template<> const volatile SmartBody::SBService* get_pointer(const volatile SmartBody::SBService* p) { return p; }
	template<> const volatile SmartBody::SBServiceManager* get_pointer(const volatile SmartBody::SBServiceManager* p) { return p; }
	template<> const volatile SmartBody::SBBmlProcessor* get_pointer(const volatile SmartBody::SBBmlProcessor* p) { return p; }
	template<> const volatile SmartBody::SBSteerManager* get_pointer(const volatile SmartBody::SBSteerManager* p) { return p; }
	template<> const volatile SmartBody::SBCollisionManager* get_pointer(const volatile SmartBody::SBCollisionManager* p) { return p; }
	template<> const volatile SmartBody::SBBoneBusManager* get_pointer(const volatile SmartBody::SBBoneBusManager* p) { return p; }
	template<> const volatile SmartBody::SBPhonemeManager* get_pointer(const volatile SmartBody::SBPhonemeManager* p) { return p; }
	template<> const volatile SmartBody::SBDiphone* get_pointer(const volatile SmartBody::SBDiphone* p) { return p; }
	template<> const volatile SmartBody::SBBehaviorSet* get_pointer(const volatile SmartBody::SBBehaviorSet* p) { return p; }
	template<> const volatile SmartBody::SBBehaviorSetManager* get_pointer(const volatile SmartBody::SBBehaviorSetManager* p) { return p; }
	template<> const volatile SmartBody::SBRetargetManager* get_pointer(const volatile SmartBody::SBRetargetManager* p) { return p; }
	template<> const volatile SmartBody::SBRetarget* get_pointer(const volatile SmartBody::SBRetarget* p) { return p; }
	template<> const volatile SmartBody::SBEvent* get_pointer(const volatile SmartBody::SBEvent* p) { return p; }
	template<> const volatile SmartBody::SBEventManager* get_pointer(const volatile SmartBody::SBEventManager* p) { return p; }
	template<> const volatile SmartBody::SBJointMapManager* get_pointer(const volatile SmartBody::SBJointMapManager* p) { return p; }
	template<> const volatile SmartBody::SBAssetManager* get_pointer(const volatile SmartBody::SBAssetManager* p) { return p; }
	template<> const volatile SmartBody::SBAsset* get_pointer(const volatile SmartBody::SBAsset* p) { return p; }
	template<> const volatile SmartBody::CharacterListenerWrap* get_pointer(const volatile SmartBody::CharacterListenerWrap* p) { return p; }
	template<> const volatile SmartBody::SBSceneListener* get_pointer(const volatile SmartBody::SBSceneListener* p) { return p; }
	template<> const volatile SmartBody::TransitionRuleWrap* get_pointer(const volatile SmartBody::TransitionRuleWrap* p) { return p; }
	template<> const volatile SmartBody::PythonControllerWrap* get_pointer(const volatile SmartBody::PythonControllerWrap* p) { return p; }
	template<> const volatile SmartBody::SBEventHandler* get_pointer(const volatile SmartBody::SBEventHandler* p) { return p; }
	template<> const volatile SmartBody::SBScriptWrap* get_pointer(const volatile SmartBody::SBScriptWrap* p) { return p; }
	template<> const volatile SmartBody::NvbgWrap* get_pointer(const volatile SmartBody::NvbgWrap* p) { return p; }
	template<> const volatile SmartBody::Nvbg* get_pointer(const volatile SmartBody::Nvbg* p) { return p; }
	template<> const volatile SmartBody::SBParser* get_pointer(const volatile SmartBody::SBParser* p) { return p; }
	template<> const volatile SmartBody::SBParseNode* get_pointer(const volatile SmartBody::SBParseNode* p) { return p; }
	template<> const volatile SmartBody::SBParserListener* get_pointer(const volatile SmartBody::SBParserListener* p) { return p; }
	template<> const volatile SmartBody::SBParserListenerWrap* get_pointer(const volatile SmartBody::SBParserListenerWrap* p) { return p; }
	template<> const volatile SmartBody::SBJointMap* get_pointer(const volatile SmartBody::SBJointMap* p) { return p; }
	template<> const volatile SmartBody::SBGestureMap* get_pointer(const volatile SmartBody::SBGestureMap* p) { return p; }
	template<> const volatile SmartBody::SBGestureMapManager* get_pointer(const volatile SmartBody::SBGestureMapManager* p) { return p; }
	template<> const volatile SmartBody::SBController* get_pointer(const volatile SmartBody::SBController* p) { return p; }
	template<> const volatile SmartBody::SBSteerAgent* get_pointer(const volatile SmartBody::SBSteerAgent* p) { return p; }
	template<> const volatile SmartBody::SBVHMsgManager* get_pointer(const volatile SmartBody::SBVHMsgManager* p) { return p; }
	template<> const volatile SmartBody::SBPhysicsManager* get_pointer(const volatile SmartBody::SBPhysicsManager* p) { return p; }
	template<> const volatile SmartBody::SBRealtimeManager* get_pointer(const volatile SmartBody::SBRealtimeManager* p) { return p; }
	template<> const volatile SmartBody::SBDebuggerServer* get_pointer(const volatile SmartBody::SBDebuggerServer* p) { return p; }
	template<> const volatile SmartBody::SBScript* get_pointer(const volatile SmartBody::SBScript* p) { return p; }
	template<> const volatile SmartBody::SBFaceDefinition* get_pointer(const volatile SmartBody::SBFaceDefinition* p) { return p; }
	template<> const volatile SmartBody::SBBehavior* get_pointer(const volatile SmartBody::SBBehavior* p) { return p; }
	template<> const volatile DeformableMesh* get_pointer(const volatile DeformableMesh* p) { return p; }
	template<> const volatile SkMotion* get_pointer(const volatile SkMotion* p) { return p; }

}
#endif

std::vector<std::function<void()>> pythonExtraModuleDeclarations;


namespace {
// decode a Python exception into a string
std::string handle_pyerror() {
	using namespace boost::python;
	using namespace boost;

	PyObject * exc, *val, *tb;
	object formatted_list, formatted;
	PyErr_Fetch(&exc, &val, &tb);
	PyErr_NormalizeException(&exc, &val, &tb);
	handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
	object traceback(import("traceback"));
	if (!tb) {
		object format_exception_only(traceback.attr("format_exception_only"));
		formatted_list = format_exception_only(hexc, hval);
	} else {
		object format_exception(traceback.attr("format_exception"));
		formatted_list = format_exception(hexc, hval, htb);
	}
	formatted = str("\n").join(formatted_list);
	return extract<std::string>(formatted);
}

//boost.python can't handle std::unique_ptrs in a good way, so we'll provide this version which does away with the result
void SBAssetStore_loadAsset(SmartBody::SBAssetStore* store, const std::string& assetPath) {
	store->loadAsset(assetPath);
}
}

namespace SmartBody {


void pythonFuncsAnimation();

void pythonFuncsAttribute();

void pythonFuncsCharacter();

void pythonFuncsMath();

void pythonFuncsMotion();

void pythonFuncsScene();

void pythonFuncsSimulation();

void pythonFuncsSkeleton();

void pythonFuncsMesh();

void pythonFuncsSystem();


BOOST_PYTHON_MODULE (SmartBody) {
	boost::python::def("printlog", printLog, "Write to the log. \n Input: message string \n Output: NULL");
	boost::python::docstring_options local_docstring_options(true, true, false);

	boost::python::class_<std::vector<std::string> >("StringVec")
			.def(boost::python::vector_indexing_suite<std::vector<std::string> >());

	boost::python::class_<std::vector<float> >("FloatVec")
			.def(boost::python::vector_indexing_suite<std::vector<float> >());

	boost::python::class_<std::vector<SrVec> >("VecArray")
			.def(boost::python::vector_indexing_suite<std::vector<SrVec> >());

	boost::python::class_<std::vector<int> >("IntVec")
			.def(boost::python::vector_indexing_suite<std::vector<int> >());

	boost::python::class_<std::vector<double> >("DoubleVec")
			.def(boost::python::vector_indexing_suite<std::vector<double> >());

	boost::python::class_<std::vector<SBAsset*> >("AssetVec")
			.def(boost::python::vector_indexing_suite<std::vector<SBAsset*> >());


	boost::python::class_<QuatMap>("QuatMap")
			.def("__len__", &QuatMap::size)
			.def("clear", &QuatMap::clear)
			.def("__getitem__", &map_item<QuatMap>::get,
				 boost::python::return_value_policy<boost::python::return_by_value>())
			.def("__setitem__", &map_item<QuatMap>::set,
				 boost::python::with_custodian_and_ward<1, 2>()) // to let container keep value
			.def("__delitem__", &map_item<QuatMap>::del);

	boost::python::class_<VecMap>("VecMap")
			.def("__len__", &VecMap::size)
			.def("clear", &VecMap::clear)
			.def("__getitem__", &map_item<VecMap>::get,
				 boost::python::return_value_policy<boost::python::return_by_value>())
			.def("__setitem__", &map_item<VecMap>::set,
				 boost::python::with_custodian_and_ward<1, 2>()) // to let container keep value
			.def("__delitem__", &map_item<VecMap>::del);

	boost::python::class_<StringMap>("StringMap")
			.def("__len__", &StringMap::size)
			.def("clear", &StringMap::clear)
			.def("__getitem__", &map_item<StringMap>::get,
				 boost::python::return_value_policy<boost::python::return_by_value>())
			.def("__setitem__", &map_item<StringMap>::set,
				 boost::python::with_custodian_and_ward<1, 2>()) // to let container keep value
			.def("__delitem__", &map_item<StringMap>::del);



	//boost::python::numeric::array::set_module_and_type("numpy", "ndarray");


	// characters

//#if !defined(__ANDROID__)



	boost::python::def("createController", createController, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a new controller given a controller type and a controller name.");





	//#endif

#if 0
	boost::python::def("pa", &PyLogger::pa, "Prints an a");
	boost::python::def("pb", &PyLogger::pb, "Prints an b");
	boost::python::def("pc", &PyLogger::pc, "Prints an c");
	boost::python::def("pd", &PyLogger::pd, "Prints an d");
	boost::python::def("pe", &PyLogger::pe, "Prints an e");
	boost::python::def("pf", &PyLogger::pf, "Prints an f");
	boost::python::def("pg", &PyLogger::pg, "Prints an g");
	boost::python::def("ph", &PyLogger::ph, "Prints an h");
	boost::python::def("pi", &PyLogger::pi, "Prints an i");
	boost::python::def("pj", &PyLogger::pj, "Prints an j");
	boost::python::def("pk", &PyLogger::pk, "Prints an k");
	boost::python::def("pl", &PyLogger::pl, "Prints an l");
	boost::python::def("pm", &PyLogger::pm, "Prints an m");
	boost::python::def("pn", &PyLogger::pn, "Prints an n");
	boost::python::def("po", &PyLogger::po, "Prints an o");
	boost::python::def("pp", &PyLogger::pp, "Prints an p");
	boost::python::def("pq", &PyLogger::pq, "Prints an q");
	boost::python::def("pr", &PyLogger::pr, "Prints an r");
	boost::python::def("ps", &PyLogger::ps, "Prints an s");
	boost::python::def("pt", &PyLogger::pt, "Prints an t");
	boost::python::def("pu", &PyLogger::pu, "Prints an u");
	boost::python::def("pv", &PyLogger::pv, "Prints an v");
	boost::python::def("pw", &PyLogger::pw, "Prints an w");
	boost::python::def("px", &PyLogger::px, "Prints an x");
	boost::python::def("py", &PyLogger::py, "Prints an y");
	boost::python::def("pz", &PyLogger::pz, "Prints an z");
	boost::python::def("p1", &PyLogger::p1, "Prints an 1");
	boost::python::def("p2", &PyLogger::p2, "Prints an 2");
	boost::python::def("p3", &PyLogger::p3, "Prints an 3");
	boost::python::def("p4", &PyLogger::p4, "Prints an 4");
	boost::python::def("p5", &PyLogger::p5, "Prints an 5");
	boost::python::def("p6", &PyLogger::p6, "Prints an 6");
	boost::python::def("p7", &PyLogger::p7, "Prints an 7");
	boost::python::def("p8", &PyLogger::p8, "Prints an 8");
	boost::python::def("p9", &PyLogger::p9, "Prints an 9");
	boost::python::def("p0", &PyLogger::p0, "Prints an 0");

	boost::python::def("openparen", &PyLogger::openparen, "Prints an (");
	boost::python::def("closeparen", &PyLogger::closeparen, "Prints a )");
	boost::python::def("openbracket", &PyLogger::openbracket, "Prints an [");
	boost::python::def("closebracket", &PyLogger::closebracket, "Prints a ]");
	boost::python::def("openbrace", &PyLogger::openbrace, "Prints an {");
	boost::python::def("closebrace", &PyLogger::closebrace, "Prints an }");
	boost::python::def("plus", &PyLogger::plus, "Prints a +");
	boost::python::def("minus", &PyLogger::minus, "Prints a -");
	boost::python::def("aster", &PyLogger::aster, "Prints an *");
	boost::python::def("slash", &PyLogger::slash, "Prints a /");
	boost::python::def("backslash", &PyLogger::backslash, "Prints a \\");
	boost::python::def("comma", &PyLogger::comma, "Prints a ,");
	boost::python::def("colon", &PyLogger::colon, "Prints a :");
	boost::python::def("semicolon", &PyLogger::semicolon, "Prints a ;");
	boost::python::def("equal", &PyLogger::equal, "Prints an =");
	boost::python::def("less", &PyLogger::less, "Prints a ");
	boost::python::def("more", &PyLogger::more, "Prints a >");

	boost::python::def("pspace", &PyLogger::pspace, "Returns the number of pawns.\n Input: NULL \nOutput: number of pawns.");
	boost::python::def("pnon", &PyLogger::pnon, "Returns the number of pawns.\n Input: NULL \nOutput: number of pawns.");
	boost::python::def("outlog", &PyLogger::outlog, "Returns the number of pawns.\n Input: NULL \nOutput: number of pawns.");
#endif

	//boost::python::def("testGetNDArray",testGetNDArray, "Test ND Array");
	// potential APIs that can be useful:
	// 1. getPawnNames
	// 2. removePawn
	// 3. createPawn 
	// etc. 
	// 


	pythonFuncsSystem();



	// class interface
/*	boost::python::class_<Script>("Script")
		.def("printInfo", &Script::print, "Print the content inside this script, this only works for seq script. ")
		.def("run", &Script::run, "Run the script immediately. ")
		.def("abort", &Script::abort, "Abort this running script, this only works for seq script. ")
		;
*/


	boost::python::class_<SBSubject>("SBSubject")
			.def("notifyObservers", &SBSubject::notifyObservers, "Notifies all observers of this subject.")
			.def("registerObserver", &SBSubject::registerObserver, "Registers an observer to this subject.")
			.def("unregisterObserver", &SBSubject::unregisterObserver, "Unregisters an observer from this subject.");

	boost::python::class_<SBObserver>("SBObserver")
			.def("addDependency", &SBObserver::addDependency, "Adds a dependency on a subject.")
			.def("removeDependency", &SBObserver::removeDependency, "Removes a dependency on a subject.")
			.def("hasDependency", &SBObserver::hasDependency, "Returns True if there is a dependency on the given subject.")
			.def("notify", &SBObserver::notify, "Notifies the observer of the subject.");


	pythonFuncsAttribute();


	boost::python::class_<SBObject, boost::noncopyable>("SBObject", boost::python::no_init)
			.def("getName", &SBObject::getName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the name of the object.")
			.def("setName", &SBObject::setName, "Sets the name of the object.")
			.def("getNumAttributes", &SBObject::getNumAttributes, "Returns the number of attributes associated with this object.")
			.def("getAttributeNames", &SBObject::getAttributeNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the attributes names associated with this object.")
			.def("getAttribute", &SBObject::getAttribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns an attribute of a given name")
			.def("clearAttributes", &SBObject::clearAttributes, "Clear all the attributes associated with this object.")
			.def("createBoolAttribute", &SBObject::createBoolAttribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a boolean attribute.")
			.def("createVec3Attribute", &SBObject::createVec3Attribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a vec3 attribute.")
			.def("createIntAttribute", &SBObject::createIntAttribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates an integer attribute.")
			.def("createDoubleAttribute", &SBObject::createDoubleAttribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a double attribute.")
			.def("createStringAttribute", &SBObject::createStringAttribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a string attribute.")
			.def("createActionAttribute", &SBObject::createActionAttribute, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a action attribute.")
			.def("setBoolAttribute", &SBObject::setBoolAttribute, "Sets a boolean attribute of a given name to the given value.")
			.def("setIntAttribute", &SBObject::setIntAttribute, "Sets an integer attribute of a given name to the given value.")
			.def("setDoubleAttribute", &SBObject::setDoubleAttribute, "Sets a floating point attribute of a given name to the given value.")
			.def("setStringAttribute", &SBObject::setStringAttribute, "Sets a string attribute of a given name to the given value.")
			.def("setVec3Attribute", &SBObject::setVec3Attribute, "Sets a vector attribute of a given name to the given value.")
			.def("setMatrixAttribute", &SBObject::setMatrixAttribute, "Sets a matrix attribute of a given name to the given value.")
			.def("setActionAttribute", &SBObject::setActionAttribute, "Sets a action attribute of a given name.")
			.def("setBool", &SBObject::setBoolAttribute, "Sets a boolean attribute of a given name to the given value.")
			.def("setInt", &SBObject::setIntAttribute, "Sets an integer attribute of a given name to the given value.")
			.def("setDouble", &SBObject::setDoubleAttribute, "Sets a floating point attribute of a given name to the given value.")
			.def("setString", &SBObject::setStringAttribute, "Sets a string attribute of a given name to the given value.")
			.def("setVec3", &SBObject::setVec3Attribute, "Sets a vector attribute of a given name to the given value.")
			.def("setMatrix", &SBObject::setMatrixAttribute, "Sets a matrix attribute of a given name to the given value.")
			.def("setAction", &SBObject::setActionAttribute, "Sets a action attribute of a given name.")
			.def("getBool", &SBObject::getBoolAttribute, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the value of a boolean attribute of a given name to the given value.")
			.def("getInt", &SBObject::getIntAttribute, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the value of an integer attribute of a given name to the given value.")
			.def("getDouble", &SBObject::getDoubleAttribute, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the value of a floating point attribute of a given name to the given value.")
			.def("getString", &SBObject::getStringAttribute, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the value of a string attribute of a given name to the given value.")
			.def("getVec3", &SBObject::getVec3Attribute, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the value of a string attribute of a given name to the given value.")
			.def("getMatrix", &SBObject::getMatrixAttribute, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the value of a matrix attribute of a given name to the given value.");


	boost::python::class_<SBService, boost::python::bases<SBObject>, boost::noncopyable>("SBService", boost::python::no_init)
			.def("setEnable", &SBService::setEnable, "Enables or disables the service.")
			.def("isEnable", &SBService::isEnable, "Is the service enabled?");

	boost::python::class_<SBServiceManager, boost::noncopyable>("SBServiceManager", boost::python::no_init)
			.def("addService", &SBServiceManager::addService, "Adds a service to the service manager.")
			.def("removeService", &SBServiceManager::removeService, "Removes a service to the service manager.")
			.def("getServiceNames", &SBServiceManager::getServiceNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of services available.")
			.def("getNumServices", &SBServiceManager::getNumServices, "Returns the number of services present.")
			.def("getService", &SBServiceManager::getService, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a service by name.");


	pythonFuncsSimulation();


	pythonFuncsAnimation();


	boost::python::class_<SBSteerManager, boost::python::bases<SBService>, boost::noncopyable>("SBSteerManager", boost::python::no_init)
			.def("createSteerAgent", &SBSteerManager::createSteerAgent, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a steer agent.")
			.def("removeSteerAgent", &SBSteerManager::removeSteerAgent, "Remove a steer agent.")
			.def("getNumSteerAgents", &SBSteerManager::getNumSteerAgents, "Return number of steer agents.")
			.def("getSteerAgent", &SBSteerManager::getSteerAgent, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return steer agent given its name.")
			.def("getSteerAgentNames", &SBSteerManager::getSteerAgentNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Return steer agent names.")
			.def("start", &SBSteerManager::start, "Start the steer simulation.")
			.def("stop", &SBSteerManager::stop, "Stop the steer simulation.");

	boost::python::class_<SBCollisionManager, boost::python::bases<SBService>, boost::noncopyable>("SBCollisionManager", boost::python::no_init)
			.def("start", &SBCollisionManager::start, "Starts the collision manager.")
			.def("stop", &SBCollisionManager::stop, "Stops the collision manager.");

	boost::python::class_<SBRealtimeManager, boost::python::bases<SBService>, boost::noncopyable>("SBRealtimeManager", boost::python::no_init)
			.def("setChannelNames", &SBRealtimeManager::setChannelNames, "Sets the names of the channels that will be used.")
			.def("getChannelNames", &SBRealtimeManager::getChannelNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Sets the names of the channels that will be used.")
			.def("setData", &SBRealtimeManager::setData, "Sets a stream of data that matches the channel names and types.")
			.def("getData", &SBRealtimeManager::getData, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets data from a particular channel.")
			.def("getDataQuat", &SBRealtimeManager::getDataQuat, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets data from a particular channel interpreted as a quaternion.")
			.def("getDataVec", &SBRealtimeManager::getDataVec, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets data from a particular channel interpreted as a vector.")
			.def("getDataMat", &SBRealtimeManager::getDataMat, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets data from a particular channel interpreted as a matrix.")
			.def("getDataDouble", &SBRealtimeManager::getDataDouble, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets data from a particular channel interpreted as a double.")
			.def("getDataInt", &SBRealtimeManager::getDataInt, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets data from a particular channel interpreted as an integer.")

			.def("setChannelMetadata", &SBRealtimeManager::setChannelMetadata, "Sets metadata describing the channels - name and data quantity.")
			.def("setDataFrame", &SBRealtimeManager::setDataFrame, "Sets data per frame based on the channel metadata.");

//	boost::python::class_<SBDebuggerServer, boost::python::bases<SBService> >("SBDebuggerServer")
//		;

	boost::python::class_<SBPhysicsManager, boost::python::bases<SBService>, boost::noncopyable>("SBPhysicsManager", boost::python::no_init)
			.def("createPhysicsCharacter", &SBPhysicsManager::createPhysicsCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a physics character.")
			.def("createPhysicsPawn", &SBPhysicsManager::createPhysicsPawn, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a physics rigid body and attach it to the pawn.")
			.def("getPhysicsEngine", &SBPhysicsManager::getPhysicsSimulationEngine, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return the current physics engine")
			.def("getPhysicsCharacter", &SBPhysicsManager::getPhysicsCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a physics-based character")
			.def("getPhysicsJoint", &SBPhysicsManager::getPhysicsJoint, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a physics-based joint")
			.def("getJointObj", &SBPhysicsManager::getJointObj, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a physics-based body link")
			.def("getPhysicsPawn", &SBPhysicsManager::getPhysicsPawn, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a rigid body pawn")
			.def("applyForceToPawn", &SBPhysicsManager::applyForceToPawn, "Apply force to a physics pawn")
			.def("applyForceToCharacter", &SBPhysicsManager::applyForceToCharacter, "Apply force to a joint of the physics character")
			.def("start", &SBPhysicsManager::start, "Start the physics simulation.(not implemented yet)")
			.def("stop", &SBPhysicsManager::stop, "Stop the physics simulation.(not implemented yet)");

	boost::python::class_<SBBoneBusManager, boost::python::bases<SBService>, boost::noncopyable>("SBBoneBusManager", boost::python::no_init);

	boost::python::class_<SBSteerAgent, boost::noncopyable>("SBSteerAgent", boost::python::no_init)
			.def("setSteerStateNamePrefix", &SBSteerAgent::setSteerStateNamePrefix, "Set the animation state name prefix used for steering, only applies to steering type locomotion.")
			.def("getSteerStateNamePrefix", &SBSteerAgent::getSteerStateNamePrefix, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the animation state name prefix used for steering, only applies to steering type locomotion.")
			.def("setSteerType", &SBSteerAgent::setSteerType, "Sets the type of steering locomotion, can be one of the following: basic, example, procedural")
			.def("getSteerType", &SBSteerAgent::getSteerType, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the type of steering locomotion, is one of the following: basic, example, procedural")
			.def("getCharacter", &SBSteerAgent::getCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return SBCharacter that SBSteerAgent is attached to.");


	boost::python::class_<SBPhonemeManager, boost::python::bases<SBService>, boost::noncopyable>("SBPhonemeManager", boost::python::no_init)
			.def("createDiphone", &SBPhonemeManager::createDiphone, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a diphone.")
			.def("getDiphones", &SBPhonemeManager::getDiphones, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get diphones given diphone set name.")
			.def("getDiphone", &SBPhonemeManager::getDiphone, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get diphone given from phoneme, to phoneme and diphone set name.")
			.def("getNumDiphoneMap", &SBPhonemeManager::getNumDiphoneMap, "Return number of diphone set.")
			.def("getNumDiphones", &SBPhonemeManager::getNumDiphones, "Return number of diphones given the diphone set name.")
			.def("getDiphoneMapNames", &SBPhonemeManager::getDiphoneMapNames, "Returns the names of all the diphone sets.")
			.def("deleteDiphoneSet", &SBPhonemeManager::deleteDiphoneSet, "Delete all the diphones given diphone sets.")
			.def("loadDictionary", &SBPhonemeManager::loadDictionary, "Loads a dictionary for a given language.")
			.def("addDictionaryWord", &SBPhonemeManager::addDictionaryWord, "Adds a word and its phoneme parts to a dictionary for a given language.")
			.def("getDictionaryWord", &SBPhonemeManager::getDictionaryWord, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the phonemes associated with a word for a given language.")
			.def("clearPhonemesRealtime", &SBPhonemeManager::clearPhonemesRealtime, "Removes all realtime phonemes associated with the given character.")
			.def("setPhonemesRealtime", &SBPhonemeManager::setPhonemesRealtime, "Sets the phonemes associated with the given character for realtime.")
			.def("getPhonemesRealtime", &SBPhonemeManager::getPhonemesRealtime, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the realtime phonemes associated with the given character.")
			.def("getPhonemesRealtimeTimings", &SBPhonemeManager::getPhonemesRealtimeTimings, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the realtime phoneme timings associated with the given character.")
			.def("saveLipSyncAnimation", &SBPhonemeManager::saveLipSyncAnimation, "Saves the lip sync animation to a .dae (COLLADA) file.")
			.def("exportForService", &SBPhonemeManager::exportForService, "Exports the data")
			.def("createFastMap", &SBPhonemeManager::createFastMap, "Optimizes the phoneme pair search.");


	boost::python::class_<SBDiphone>("SBDiphone")
			.def("addKey", &SBDiphone::addKey, "add key to the diphone.")
			.def("setKey", &SBDiphone::setKey, "sets keys to the diphone.")
			.def("getKeys", &SBDiphone::getKeys, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return the keys given viseme name.")
			.def("getVisemeNames", &SBDiphone::getVisemeNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Return the viseme names.")
			.def("getNumVisemes", &SBDiphone::getNumVisemes, "Return the number of viseme.")
			.def("getFromPhonemeName", &SBDiphone::getFromPhonemeName, boost::python::return_value_policy<boost::python::return_by_value>(), "Return FROM phoneme name.")
			.def("getToPhonemeName", &SBDiphone::getToPhonemeName, boost::python::return_value_policy<boost::python::return_by_value>(), "Return TO phoneme name.");

	boost::python::class_<std::vector<SBDiphone*> >("SBDiphoneVec")
			.def(boost::python::vector_indexing_suite<std::vector<SBDiphone*> >());

	boost::python::class_<SBBehaviorSetManager>("SBBehaviorSetManager")
			.def("createBehaviorSet", &SBBehaviorSetManager::createBehaviorSet, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a behavior set.")
			.def("getBehaviorSets", &SBBehaviorSetManager::getBehaviorSets, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets all the behavior sets.")
			.def("getBehaviorSet", &SBBehaviorSetManager::getBehaviorSet, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets a behavior set with a given name.")
			.def("getNumBehaviorSets", &SBBehaviorSetManager::getNumBehaviorSets, "Returns the number of behavior sets.")
			.def("removeBehaviorSet", &SBBehaviorSetManager::removeBehaviorSet, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Removes a behavior set with a given name.")
			.def("removeAllBehaviorSets", &SBBehaviorSetManager::removeAllBehaviorSets, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Removes all the behavior sets.");

	boost::python::class_<SBBehaviorSet>("SBBehaviorSet")
			.def("setName", &SBBehaviorSet::setName, "Sets the name of the behavior set.")
			.def("getName", &SBBehaviorSet::getName, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the name of the behavior set.")
			.def("setScript", &SBBehaviorSet::setScript, "Sets the name of the script to be run for this behavior set.")
			.def("getScript", &SBBehaviorSet::getScript, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the name of the script to be run for this behavior set.");

	boost::python::class_<SBRetargetManager>("SBRetargetManager")
			.def("createRetarget", &SBRetargetManager::createRetarget, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a retarget instance for the source/target skeleton pair.")
			.def("getRetarget", &SBRetargetManager::getRetarget, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the retarget instance for the source/target skeleton pair.")
			.def("getRetargetNames", &SBRetargetManager::getRetargetNames, "Get the names of all retarget instances.");


	boost::python::class_<SBRetarget>("SBRetarget")
			.def("initRetarget", &SBRetarget::initRetarget, "Initialize retarget instance.")
			.def("applyRetargetJointRotation", &SBRetarget::applyRetargetJointRotation, "Apply retargeting on the joint rotation.")
			.def("applyRetargetJointTranslation", &SBRetarget::applyRetargetJointTranslation, "Apply retargeting on the joint translation.")
			.def("getEndJointNames", &SBRetarget::getEndJointNames, "Get the names of the joints that will be skipped during retargeting.")
			.def("getRelativeJointNames", &SBRetarget::getRelativeJointNames, "Get the names of the joints that not be aligned during retargeting.")
			.def("addJointRotOffset", &SBRetarget::addJointRotOffset, "As an additional offset to the joint to be applied during the retargeting.");


	/*
		boost::python::class_<Viseme>("Viseme")
			.def("getName", &Viseme::getVisemeName, boost::python::return_value_policy<boost::python::return_by_value>(), "Get the viseme name. \n Input: NULL \n Output: viseme name")
			.def("getCharacterName", &Viseme::getCharName, boost::python::return_value_policy<boost::python::return_by_value>(), "Get the character name. \n Input: NULL \n Output: character name")
			.def("setWeight", &Viseme::setWeight, "Set the weight for current viseme in non-curve mode. \n Input: a list including weight, duration, rampup time, rampdown time e.g. [1,3,1,1] \n Output: NULL")
			.def("setCurve", &Viseme::setCurve, "Set the curve for current viseme in curve mode. \n Input: number of keys, a list of keys e.g. (3, [1,0,3,1,5,0]) \n Output: NULL")
			;
	*/

	boost::python::class_<SBFaceDefinition, boost::noncopyable>("SBFaceDefinition", boost::python::no_init)
			.def("getName", &SBFaceDefinition::getName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns name of the face definition.")
			.def("getNumVisemes", &SBFaceDefinition::getNumVisemes, "Returns the number of visemes.")
			.def("getVisemeNames", &SBFaceDefinition::getVisemeNames, "Returns the names of the visemes.")
			.def("setViseme", &SBFaceDefinition::setViseme, "Sets a viseme to a particular motion name.")
			.def("getVisemeMotion", &SBFaceDefinition::getVisemeMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a motion associated with a particular viseme.")
			.def("getNumAUs", &SBFaceDefinition::getNumAUs, "Returns the number of Action Units.")
			.def("getAUNumbers", &SBFaceDefinition::getAUNumbers, "Returns the numbers of the Action Units.")
			.def("getAUSide", &SBFaceDefinition::getAUSide, "Returns LEFT, RIGHT, or BOTH, depending on which side is used for the Action Unit.")
			.def("getAUMotion", &SBFaceDefinition::getAUMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the motion associated with a given Action Unit side: LEFT, RIGHT, or BOTH.")
			.def("setAU", &SBFaceDefinition::setAU, "Sets an Action Unit of a given number to a side and a motion.")
			.def("setFaceNeutral", &SBFaceDefinition::setFaceNeutral, "Sets the neutral face to a particular motion name.")
			.def("save", &SBFaceDefinition::save, "Save face definition to a file.")
			.def("setDefaultFacePose", &SBFaceDefinition::setDefaultFacePose, "Sets the default face poses, accepts a vector of face poses and a vector of values.");


	pythonFuncsMotion();


	boost::python::class_<SBController, boost::python::bases<SBObject> >("SBController", boost::python::no_init)
			.def("addChannel", &SBController::addChannel, "Adds a channel to this controller's context.")
			.def("getType", &SBController::getType, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the type for this controller.")
			.def("setIgnore", &SBController::setIgnore, "Ignore this controller when evaluating controller tree.")
			.def("isIgnore", &SBController::isIgnore, "Will the controller be ignored during evaluation.")
			.def("setDebug", &SBController::setDebug, "Sets the debug state for this controller.")
			.def("isDebug", &SBController::isDebug, "Is the controller in a debug state?")
			.def("getDuration", &SBController::getDuration, "Gets the controller's duration.")
			.def("getChannelValue", &SBController::getChannelValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the channel's value.")
			.def("getChannelPos", &SBController::getChannelPos, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the channel's translation value.")
			.def("getChannelQuat", &SBController::getChannelQuat, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the channel's rotation value.")
			.def("setChannelValue", &SBController::setChannelValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Sets the channel's value.")
			.def("setChannelPos", &SBController::setChannelPos, boost::python::return_value_policy<boost::python::return_by_value>(), "Sets the channel's translation value.")
			.def("setChannelQuat", &SBController::setChannelQuat, boost::python::return_value_policy<boost::python::return_by_value>(), "Sets the channel's rotation value.")
			.def("setChannelQuatGlobal", &SBController::setChannelQuatGlobal, boost::python::return_value_policy<boost::python::return_by_value>(), "Sets the channel's rotation value.")
			.def("convertChannelQuatGlobal", &SBController::convertChannelQuatGlobal, boost::python::return_value_policy<boost::python::return_by_value>(), "convert the channel's rotation value.")
			.def("getCharacterName", &SBController::getCharacterName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the controlling character's name.")
			.def("getPawn", &SBController::getPawn, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Attaches the attached pawn to the controller.")
			//.def("setPawn", &SBController::setPawn, "Attaches the pawn to the controller.")
			;

	boost::python::class_<MeCtNewLocomotion, boost::python::bases<SBController, SBObject>, boost::noncopyable >("SBNewLocomotion", boost::python::no_init)
			.def("reset", &MeCtNewLocomotion::reset, "Resets.")
			.def("setDesiredHeading", &MeCtNewLocomotion::setDesiredHeading, "Sets the desired heading")
			.def("loopMotion", &MeCtNewLocomotion::loopMotion, "Checks a loop");

	boost::python::class_<MeCtScheduler2, boost::python::bases<SBController> >("SchedulerController", boost::python::no_init)
			.def("getNumTracks", &MeCtScheduler2::count_children, "Returns the number of children/tracks.");

	boost::python::class_<MeCtMotion, boost::python::bases<SBController> >("MotionController", boost::python::no_init)
		//	.def("getMotion", &MeCtMotion::motion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the motion associated with this motion controller.")
			;

	boost::python::class_<MeCtEyeLid, boost::python::bases<SBController> >("EyelidController", boost::python::no_init);

	boost::python::class_<MeCtGaze, boost::python::bases<SBController> >("GazeController", boost::python::no_init);

	//boost::python::class_<MeCtReach, boost::python::bases<SBController> > ("ReachController")	;

	boost::python::class_<MeCtCurveWriter, boost::python::bases<SBController> >("CurveWriterController", boost::python::no_init);


	pythonFuncsSkeleton();

	pythonFuncsMesh();


	boost::python::class_<SBBehavior, boost::python::bases<SBObject> >("SBBehavior")
			//.def(boost::python::init<std::string, std::string>())
			.def("getType", &SBBehavior::getType, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the type of the behavior.");

	boost::python::class_<GazeBehavior, boost::python::bases<SBBehavior> >("GazeBehavior")
			//.def(boost::python::init<std::string, std::string>())
			.def("getGazeTarget", &GazeBehavior::getGazeTarget, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the name of the gaze target.")
			.def("isFadingIn", &GazeBehavior::isFadingIn, "Returns true if the gaze controller is currently fading in.")
			.def("isFadingOut", &GazeBehavior::isFadingOut, "Returns true if the gaze controller is currently fading out.")
			.def("isFadedOut", &GazeBehavior::isFadedOut, "Returns true if the gaze controller is currently faded out and thus not active.")
			.def("getHandle", &GazeBehavior::getHandle, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the handle name of the gaze, or an empty string if it has no handle.");

	boost::python::class_<LocomotionBehavior, boost::python::bases<SBBehavior> >("LocomotionBehavior")
			//.def(boost::python::init<std::string, std::string>())
			.def("getLocomotionTarget", &LocomotionBehavior::getLocomotionTarget, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the locomotion target as a vector.");

	boost::python::class_<PostureBehavior, boost::python::bases<SBBehavior> >("PostureBehavior")
			//.def(boost::python::init<std::string, std::string>())
			.def("getPosture", &PostureBehavior::getPosture, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the current posture.");

	boost::python::class_<SpeechBehavior, boost::python::bases<SBBehavior> >("SpeechBehavior")
			//.def(boost::python::init<std::string, std::string>())
			.def("getUtterance", &SpeechBehavior::getUtterance, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the current utterance.");


	pythonFuncsCharacter();


	boost::python::class_<SBReach>("SBReach")
			.def("getCharacter", &SBReach::getCharacter, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the character associated with this reach engine.")
			.def("copy", &SBReach::copy, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Copies the reach engine.")
			.def("addMotion", &SBReach::addMotion, "Adds a motion to the reach engine.")
			.def("removeMotion", &SBReach::removeMotion, "Removes a motion from the reach engine.")
			.def("getNumMotions", &SBReach::getNumMotions, "Returns the number of motions in the reach engine.")
			.def("getMotionNames", &SBReach::getMotionNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the names of the motions used in the reach engine.")
			.def("build", &SBReach::build, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Constructs the reach engine using the exisiting motions.")
			.def("setGrabHandMotion", &SBReach::setGrabHandMotion, "Sets the hand pose to be used during grasping.")
			.def("getGrabHandMotion", &SBReach::getGrabHandMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the hand pose to be used during grasping.")
			.def("setReleaseHandMotion", &SBReach::setReleaseHandMotion, "Sets the hand pose to be used when releasing an object.")
			.def("getReleaseHandMotion", &SBReach::getReleaseHandMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the hand pose to be used when releasing an object.")
			.def("setReachHandMotion", &SBReach::setReachHandMotion, "Sets the hand pose to be used when reaching for an object.")
			.def("getReachHandMotion", &SBReach::getReachHandMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the hand pose to be used when reaching for an object.")
			.def("setPointHandMotion", &SBReach::setPointHandMotion, "Sets the hand pose to be used when reaching for an object.")
			.def("getPointHandMotion", &SBReach::getPointHandMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the hand pose to be used when reaching for an object.")
			.def("setInterpolatorType", &SBReach::setInterpolatorType, "Set the interpolation type when building the reach engine")
			.def("getInterpolatorType", &SBReach::getInterpolatorType, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the interpolation type used building the reach engine")
			.def("isPawnAttached", &SBReach::isPawnAttached, boost::python::return_value_policy<boost::python::return_by_value>(), "Return True is the pawn is currently attached to the character's hand.");

	boost::python::class_<SBReachManager, boost::noncopyable>("SBReachManager", boost::python::no_init)
			.def("createReach", &SBReachManager::createReach, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a reach engine for a character.")
			.def("createReachWithTag", &SBReachManager::createReachWithTag, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a reach engine for a character with specified tag.")
//			.def("removeReach", &SBReachManager::removeReach, "Removes a reach engine for a character")
			.def("getNumReaches", &SBReachManager::getNumReaches, "Returns the number of reach engines present.")
			.def("getReach", &SBReachManager::getReach, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a reach engine for a given character.");


	boost::python::class_<SBGestureMap::GestureInfo>("GestureInfo")
			.def("getAnimation", &SBGestureMap::GestureInfo::getAnimation, boost::python::return_value_policy<boost::python::return_by_value>(), "")
			.def("getLexeme", &SBGestureMap::GestureInfo::getLexeme, boost::python::return_value_policy<boost::python::return_by_value>(), "")
			.def("getType", &SBGestureMap::GestureInfo::getType, boost::python::return_value_policy<boost::python::return_by_value>(), "")
			.def("getHand", &SBGestureMap::GestureInfo::getHand, boost::python::return_value_policy<boost::python::return_by_value>(), "")
			.def("getStyle", &SBGestureMap::GestureInfo::getStyle, boost::python::return_value_policy<boost::python::return_by_value>(), "")
			.def("getPosture", &SBGestureMap::GestureInfo::getPosture, boost::python::return_value_policy<boost::python::return_by_value>(), "");

	boost::python::class_<SBGestureMap, boost::noncopyable>("SBGestureMap", boost::python::no_init)
			.def("addGestureMapping", &SBGestureMap::addGestureMapping, "Add a gesture mapping. Input: name of the animation/state, type, posture, hand. Output: null")
			.def("getGestureByInfo", &SBGestureMap::getGestureByInfo, "Return a gesture given the type and hand of the gesture. Input: type, hand, style. Output: corresponding gesture name")
			.def("getGestureByIndex", &SBGestureMap::getGestureByIndex, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a gesture given the index inside the map.")
			.def("getGestureAnimation", &SBGestureMap::getGestureAnimation, "Return the animation name associated with the gesture index")
			.def("getNumMappings", &SBGestureMap::getNumMappings, "Return a number of entries inside the map.")
			.def("validate", &SBGestureMap::validate, "Gives feedback about gestures in the gesture map; reports missing idle poses or motions.");

	boost::python::class_<SBGestureMapManager, boost::noncopyable>("SBGestureMapManager", boost::python::no_init)
			.def("createGestureMap", &SBGestureMapManager::createGestureMap, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a gesture map for a character.")
			.def("removeGestureMap", &SBGestureMapManager::removeGestureMap, "Remove a gesture map for a character given character name.")
			.def("getNumGestureMaps", &SBGestureMapManager::getNumGestureMaps, "Return number of gesture maps in the scene.")
			.def("getGestureMap", &SBGestureMapManager::getGestureMap, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return gesture map given character name.");

	boost::python::class_<SBHandConfigurationManager, boost::noncopyable>("SBHandConfigurationManager", boost::python::no_init)
			.def("createHandConfiguration", &SBHandConfigurationManager::createHandConfiguration, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Create a hand configuration.")
			.def("removeHandConfiguration", &SBHandConfigurationManager::removeHandConfiguration, "Remove a hand configuration")
			.def("getNumHandConfiguration", &SBHandConfigurationManager::getNumHandConfigurations, "Get the number of hand configuration")
			.def("printHandConfiguration", &SBHandConfigurationManager::printHandConfiguration, "Print the hand configuration.");

	boost::python::class_<SBHandConfiguration, boost::noncopyable>("SBHandConfiguration", boost::python::no_init)
			.def("addMotion", &SBHandConfiguration::addMotion, "Add a new motion to the configuration")
			.def("removeMotion", &SBHandConfiguration::removeMotion, "Remove the motion from the configuration")
			.def("getNumMotions", &SBHandConfiguration::getNumMotions, "Get the number of motions")
			.def("getMotionNames", &SBHandConfiguration::getMotionNames, "Returns a list of motion names from the hand configuration.");

	boost::python::class_<SBJointMap, boost::noncopyable>("SBJointMap", boost::python::no_init)
			.def("setMapping", &SBJointMap::setMapping, "Sets the mapping from one joint name to another.")
			.def("removeMapping", &SBJointMap::removeMapping, "Removes a mapping from a given joint to whichever joint is mapped.")
			.def("getMapTarget", &SBJointMap::getMapTarget, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the target joint for a given mapping.")
			.def("getMapSource", &SBJointMap::getMapSource, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the source joint for a given mapping.")
			.def("getNumMappings", &SBJointMap::getNumMappings, "Returns the number of joint mappings.")
			.def("getSource", &SBJointMap::getSource, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the source joint of the nth mapping.")
			.def("getTarget", &SBJointMap::getTarget, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the target joint of the nth mapping.")
			.def("applyMotion", &SBJointMap::applyMotion, "Applies the current joint mapping to a motion.")
			.def("applyMotionRecurse", &SBJointMap::applyMotionRecurse, "Applies the current joint mapping to all motions that originally came from the given directories.")
			.def("applySkeleton", &SBJointMap::applySkeleton, "Applies the current skeleton mapping to a motion.")
			.def("applyMotionInverse", &SBJointMap::applyMotionInverse, "Applies the inverse joint mapping to a motion.")
			.def("applySkeletonInverse", &SBJointMap::applySkeletonInverse, "Applies the inverse skeleton mapping to a motion.")
			.def("guessMapping", &SBJointMap::guessMapping, "Automatic joint name matching to standard SmartBody names.")
//			.def("getMappedMotions", &SBJointMap::getMappedMotions, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of the motions that have been mapped by this joint map.")
//			.def("getMappedSkeletons", &SBJointMap::getMappedSkeletons, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of the skeletons that have been mapped by this joint map.")
			;

	boost::python::class_<SBJointMapManager, boost::noncopyable>("SBJointMapManager", boost::python::no_init)
			.def("getJointMap", &SBJointMapManager::getJointMap, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the joint map associated with a given name.")
			.def("createJointMap", &SBJointMapManager::createJointMap, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a joint map with a given name. Returns null is the map already exists.")
			.def("getJointMapNames", &SBJointMapManager::getJointMapNames, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the names of all joint maps.")
			.def("removeJointMap", &SBJointMapManager::removeJointMap, "Removes a joint map of a given name.")
			.def("removeAllJointMaps", &SBJointMapManager::removeAllJointMaps, "Removes all the joint maps.");

	boost::python::class_<SBEvent>("SBEvent")
			.def(boost::python::init<>())
			.def("getType", &SBEvent::getType, "Returns the event type.")
			.def("setType", &SBEvent::setType, "Sets the event type.")
			.def("getParameters", &SBEvent::getParameters, "Returns the event parameters.")
			.def("setParameters", &SBEvent::setParameters, "Sets the event parameters.")
			.def("getSource", &SBEvent::getSource, "Returns the event source object string.")
			.def("setSource", &SBEvent::setSource, "Set the event source object string.");

	boost::python::class_<SBEventManager, boost::noncopyable>("SBEventManager", boost::python::no_init)
			.def("handleEvent", &SBEventManager::handleEvent, "Processes an event by the appropriate event handler.")
			.def("handleEventRemove", &SBEventManager::handleEventRemove, "Processes an event by the appropriate event handler then remove the event.")
			.def("addEventHandler", &SBEventManager::addEventHandler, "Returns the event type.")
			.def("removeEventHandler", &SBEventManager::removeEventHandler, "Returns the event type.")
			.def("getNumHandlers", &SBEventManager::getNumEventHandlers, "Gets the number of event handlers.")
			.def("getEventHandler", &SBEventManager::getEventHandler, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets an event handler by name.")
			.def("createEvent", &SBEventManager::createEvent, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates an event of a given type with given parameters.");

	boost::python::class_<SBParseNode>("SBParseNode")
			.def("getWord", &SBParseNode::getWord, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the word, empty word if not a terminal node.")
			.def("getTerm", &SBParseNode::getTerm, boost::python::return_value_policy<boost::python::return_by_value>(), "Gets the terminal, empty word if not a terminal node.")
			.def("isTerminal", &SBParseNode::isTerminal, "Is this node a terminal node.")
			.def("getNumChildren", &SBParseNode::getNumChildren, "Deletes parse tree.")
			.def("getChild", &SBParseNode::getChild, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a child node.");

	boost::python::class_<SBParser>("SBParser")
			.def("parse", &SBParser::parse, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Parses an utterance using the Charniak parser.")
			.def("initialize", &SBParser::initialize, "Initializes the Charniak parser with parameters.")
			.def("cleanUp", &SBParser::cleanUp, "Deletes parse tree.")
			.def("isInitialized", &SBParser::isInitialized, "Return boolean telling if Charniak parser is initialized.")
			.def("parseUtterance", &SBParser::parseUtterance, "Parse utterance and send output to listeners.");


	pythonFuncsMath();

	boost::python::class_<SBParserListenerWrap, boost::noncopyable, boost::shared_ptr<SBParserListenerWrap> >("SBParserListener")
			.def("onWord", &SBParserListener::onWord, &SBParserListenerWrap::default_onWord, "Callback for word in utterance.")
			.def("onPartOfSpeech", &SBParserListener::onPartOfSpeech, &SBParserListenerWrap::default_onPartOfSpeech, "Callback for part of speech in utterance.")
			.def("resetBML", &SBParserListener::resetBML, "Restores the BML to Callback for word in utterance.")
			.def("addBML", &SBParserListener::addBML, "Adds a BML instruction to the current BML construct.")
			.def("getCurrentBML", &SBParserListener::getCurrentBML, "Returns the current BML construct.");

	boost::python::class_<NvbgWrap, boost::python::bases<SBObject>, boost::noncopyable>("Nvbg")
			.def("objectEvent", &Nvbg::objectEvent, &NvbgWrap::default_objectEvent, "An event indicating that an object of interest is present.")
			.def("execute", &Nvbg::execute, &NvbgWrap::default_execute, "Execute the xml vrX message.")
			.def("executeEvent", &Nvbg::executeEvent, &NvbgWrap::default_executeEvent, "Execute the vrAgent message.")
			.def("executeSpeech", &Nvbg::executeSpeech, &NvbgWrap::default_executeSpeech, "Execute the vrSpeech message.")
			.def("executeSpeechRequest", &Nvbg::executeSpeechRequest, &NvbgWrap::default_executeSpeechRequest, "Execute the listening feedback from virtual agents.")
			.def("notifyAction", &Nvbg::notifyAction, &NvbgWrap::default_notifyAction, "Notifies NVBG processor of a bool attribute.")
			.def("notifyBool", &Nvbg::notifyBool, &NvbgWrap::default_notifyBool, "Notifies NVBG processor of a bool attribute")
			.def("notifyInt", &Nvbg::notifyInt, &NvbgWrap::default_notifyInt, "Notifies NVBG processor of an int attribute")
			.def("notifyDouble", &Nvbg::notifyDouble, &NvbgWrap::default_notifyDouble, "Notifies NVBG processor of a double attribute")
			.def("notifyString", &Nvbg::notifyString, &NvbgWrap::default_notifyString, "Notifies NVBG processor of a string attribute")
			.def("notifyVec3", &Nvbg::notifyVec3, &NvbgWrap::default_notifyVec3, "Notifies NVBG processor of a vec3 attribute.")
			.def("notifyMatrix", &Nvbg::notifyMatrix, &NvbgWrap::default_notifyMatrix, "Notifies NVBG processor of a matrix attribute.");

	boost::python::class_<SBScriptWrap, boost::noncopyable, boost::shared_ptr<SBScriptWrap> >("SBScript")
			.def(boost::python::init<>())
			.def("start", &SBScript::start, &SBScriptWrap::default_start, "Script start.")
			.def("beforeUpdate", &SBScript::beforeUpdate, &SBScriptWrap::default_beforeUpdate, "Script before update step.")
			.def("update", &SBScript::update, &SBScriptWrap::default_update, "Script updates.")
			.def("afterUpdate", &SBScript::afterUpdate, &SBScriptWrap::default_afterUpdate, "Script after update step.")
			.def("stop", &SBScript::stop, &SBScriptWrap::default_stop, "Script stop.");

	boost::python::class_<SBEventHandlerWrap, boost::noncopyable>("SBEventHandler")
			.def("executeAction", &SBEventHandler::executeAction, &SBEventHandlerWrap::default_executeAction, "Execute the event handler.");

	boost::python::class_<PythonControllerWrap, boost::noncopyable, boost::python::bases<SBController, SBObject>, boost::shared_ptr<PythonControllerWrap> >("PythonController", boost::python::no_init)
			.def("start", &PythonController::start, &PythonControllerWrap::default_start, "start.")
			.def("stop", &PythonController::stop, &PythonControllerWrap::default_stop, "stop.")
			.def("evaluate", &PythonController::evaluate, &PythonControllerWrap::default_evaluate, "evaluate.");

	boost::python::class_<TransitionRuleWrap, boost::noncopyable>("TransitionRule")
			.def("check", &SBAnimationTransitionRule::check, &TransitionRuleWrap::default_check, "Determines if the transition rule should be triggered.");

	boost::python::class_<SBSceneListener>("SBSceneListener")
			.def("OnCharacterCreate", &SBSceneListener::OnCharacterCreate, "Triggered when a character is created.")
			.def("OnCharacterDelete", &SBSceneListener::OnCharacterDelete, "Triggered when a character is deleted.")
			.def("OnCharacterUpdate", &SBSceneListener::OnCharacterUpdate, "Triggered when a character is updated.")
			.def("OnPawnCreate", &SBSceneListener::OnPawnCreate, "Triggered when a pawn is created.")
			.def("OnPawnDelete", &SBSceneListener::OnPawnDelete, "Triggered when a pawn is deleted.")
			.def("OnViseme", &SBSceneListener::OnViseme, "Visemes sent every frame.")
			.def("OnEvent", &SBSceneListener::OnEvent, "Triggered when an event occurs.");

	boost::python::class_<CharacterListenerWrap, boost::noncopyable>("CharacterListener")
			.def("OnCharacterCreate", &SBSceneListener::OnCharacterCreate, &CharacterListenerWrap::default_OnCharacterCreate, "Triggered when a character is created.")
			.def("OnCharacterDelete", &SBSceneListener::OnCharacterDelete, &CharacterListenerWrap::default_OnCharacterDelete, "Triggered when a character is deleted.")
			.def("OnCharacterUpdate", &SBSceneListener::OnCharacterUpdate, &CharacterListenerWrap::default_OnCharacterUpdate, "Triggered when a character is updated.")
			.def("OnPawnCreate", &SBSceneListener::OnPawnCreate, &CharacterListenerWrap::default_OnPawnCreate, "Triggered when a pawn is created.")
			.def("OnPawnDelete", &SBSceneListener::OnPawnDelete, &CharacterListenerWrap::default_OnPawnDelete, "Triggered when a pawn is deleted.")
			.def("OnViseme", &SBSceneListener::OnViseme, &CharacterListenerWrap::default_OnViseme, "Triggered when a viseme is triggered.")
			.def("OnChannel", &SBSceneListener::OnChannel, &CharacterListenerWrap::default_OnChannel, "Channel data.")
			.def("OnLogMessage", &SBSceneListener::OnLogMessage, &CharacterListenerWrap::default_OnLogMessage, "Log message.");

	boost::python::class_<SBAssetStore, boost::noncopyable>("SBAssetStore", boost::python::no_init)
			.def("addAssetPath", &SBAssetStore::addAssetPath, "Add path resource given path type and actual path string. \n Input: type(can be seq|me|ME), path \n Output: NULL")
			.def("removeAssetPath", &SBAssetStore::removeAssetPath, "Removes a  path resource given path type and actual path string. \n Input: type(can be cript|motion|audio), path \n Output: NULL")
			.def("removeAllAssetPaths", &SBAssetStore::removeAllAssetPaths, "Removes all paths resource given path type and actual path string. \n Input: type(can be script|motion|audio), path \n Output: NULL")
			.def("getAssetPaths", &SBAssetStore::getAssetPaths, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of all path names for a given type: seq, me, audio, mesh.")
			.def("getLocalAssetPaths", &SBAssetStore::getLocalAssetPaths, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns a list of all path names for a given type excluding the media path: seq, me, audio, mesh.")
			.def("loadAsset", &SBAssetStore_loadAsset, "Loads the skeletons and motions from the file.")
			.def("loadAssets", &SBAssetStore::loadAssets, "Loads the skeletons and motions from the asset paths.")
			.def("loadAssetsFromPath", &SBAssetStore::loadAssetsFromPath, "Loads the skeletons and motions from a given path. The path will not be stored for later use.");


	boost::python::class_<SBAssetManager, boost::python::bases<SBObject>, boost::noncopyable>("SBAssetManager", boost::python::no_init)
			.def("addSkeletonDefinition", &SBAssetManager::addSkeletonDefinition, boost::python::return_value_policy<boost::python::return_by_value>(), "Creates a new skeleton given a name.")
			.def("removeSkeletonDefinition", &SBAssetManager::removeSkeletonDefinition, "Removes a skeleton with a given name.")
			.def("createMotion", &SBAssetManager::createMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a new motion given a name.")
			.def("createSkeleton", &SBAssetManager::createSkeleton, boost::python::return_value_policy<boost::python::return_by_value>(), "Creates a new skeleton given a skeleton definition.")
			.def("getSkeleton", &SBAssetManager::getSkeleton, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the skeleton object given its name. \n Input: skeleton name \nOutput: skeleton object")
					//.def("addMotion", &SBAssetManager::addMotion, "Adds a motion to the scene.")
			.def("addMotionDefinition", &SBAssetManager::addMotionDefinition, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Adds a motion definition with blank frames.")
			.def("removeMotion", &SBAssetManager::removeMotion, "Removes a motion from the scene.")
//		.def("addMotions", &SBAssetManager::addMotions, "Add motion resource given filepath and recursive flag. \n Input: path, recursive flag(boolean variable indicating whether to tranverse all the children directories) \n Output: NULL")
			.def("getMotion", &SBAssetManager::getMotion, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a the motion of given name.")
			.def("getNumMotions", &SBAssetManager::getNumMotions, "Returns the number of motions available.")
			.def("getMotionNames", &SBAssetManager::getMotionNames, "Returns the names of motions available.")
			.def("getNumSkeletons", &SBAssetManager::getNumSkeletons, "Returns the number of skeletons available.")
			.def("getSkeletonNames", &SBAssetManager::getSkeletonNames, "Returns a list of all skeleton names.\n Input: NULL \nOutput: list of skeleton names")
//		.def("addMesh", &SBAssetManager::addMesh, "Adds a mesh to the scene.")
//		.def("removeMesh", &SBAssetManager::removeMesh, "Removes a mesh from the scene.")
//		.def("removeDeformableMesh", &SBAssetManager::removeDeformableMesh, "Removes a mesh from the scene.")
//		.def("getMeshNames", &SBAssetManager::getMeshNames, "Returns a list of all mesh names.\n Input: NULL \nOutput: list of skeleton names")
//		.def("getDeformableMesh", &SBAssetManager::getDeformableMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a deformable mesh given name")
//		.def("getMesh", &SBAssetManager::getDeformableMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a deformable mesh given name")
//		.def("createMeshFromBlendMasks", &SBAssetManager::createMeshFromBlendMasks, "Writes out a mesh and texture based on: neutralShapeFile, neutralTextureFile, expressiveShapeFile, expressiveTextureFile, maskTextureFile, outputMeshFile, outputTextureFile")
//		.def("addModelToMesh", &SBAssetManager::addModelToMesh, "Adds a submesh to a model.")
//		.def("addBlendshapeToModel", &SBAssetManager::addBlendshapeToModel, "Adds a blendshape to a mesh, or replaces the blendshape within the mesh.")
//		.def("handlePenetrations", &SBAssetManager::handlePenetrations, "Handles penetrations within a mesh given a base model and a penetrative model.")
			;


	boost::python::class_<SBVHMsgManager, boost::python::bases<SBService>, boost::noncopyable>("SBVHMsgManager", boost::python::no_init)
			.def("connect", &SBVHMsgManager::connect, "Connects to the VH Message (ActiveMQ) server")
			.def("disconnect", &SBVHMsgManager::disconnect, "Disconnects from the VH Message (ActiveMQ) server")
			.def("sendMessage", &SBVHMsgManager::sendMessage, "Sends a VH message.")
			.def("sendOpMessage", &SBVHMsgManager::sendOpMessage, "Sends a VH message with OP.")
			.def("send", &SBVHMsgManager::send, "Sends a VH message.")
			.def("poll", &SBVHMsgManager::poll, "Queries the VHMSG bus for incoming messages.")
			.def("setPort", &SBVHMsgManager::setPort, "Sets the port on which the VH Message (ActiveMQ) server is running.")
			.def("getPort", &SBVHMsgManager::getPort, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the VH Message (ActiveMQ) server port.")
			.def("setServer", &SBVHMsgManager::setServer, "Sets the VH Message (ActiveMQ) server name or IP address.")
			.def("getServer", &SBVHMsgManager::getServer, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the VH Message (ActiveMQ) server name or IP address.")
			.def("setScope", &SBVHMsgManager::setScope, "Sets the ActiveMQ scope for VH messaging.")
			.def("getScope", &SBVHMsgManager::getScope, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the ActiveMQ scope used for VH messaging.")
			.def("setEnableLogging", &SBVHMsgManager::setEnableLogging, "Enables logging over the VH Message bus. Messages will start with the prefix 'vhlog'")
			.def("isEnableLogging", &SBVHMsgManager::isEnableLogging, "Disables logging over the VH Message bus.");

	pythonFuncsScene();

	for (auto& func : pythonExtraModuleDeclarations) {
		func();
	}

}


}

#if defined(__ANDROID__) || defined(SB_IPHONE)
//#if 0
extern "C" {
	extern void initpyexpat(void);
	extern void init_functools(void);
	extern void inititertools(void);
	extern void initcStringIO(void);
	extern void initcPickle(void);
	extern void initcmath(void);
	extern void initmath(void);
	extern void inittime(void);
	extern void initdatetime(void);
	extern void init_collections(void);
	extern void initoperator(void);
	extern void init_md5(void);
	extern void init_sha(void);
	extern void init_sha256(void);
	extern void init_sha512(void);
	extern void initselect(void);
	extern void initfcntl(void);
	extern void init_struct(void);
	//extern void init_ctypes_test(void);
	extern void init_weakref(void);
	extern void initarray(void);
	extern void initstrop(void);
	//extern void initfuture_builtins(void);
	extern void init_random(void);
	extern void init_bisect(void);
	//extern void init_heapq(void);
	//extern void init_fileio(void);
	extern void init_bytesio(void);
	extern void init_functools(void);
	//extern void init_json(void);
	//extern void init_testcapi(void);
	//extern void init_hotshot(void);
	//extern void init_lsprof(void);
	//extern void initunicodedata(void);
	//extern void init_locale(void);
	extern void initparser(void);
	//extern void initmmap(void);
	//extern void initsyslog(void);
	//extern void initaudioop(void);
	//extern void initimageop(void);
	//extern void init_csv(void);
	//extern void inittermios(void);
	//extern void initresource(void);
	extern void initbinascii(void);
	//extern void init_multibytecodec(void);
	//extern void init_codecs_kr(void);
	//extern void init_codecs_jp(void);
	//extern void init_codecs_cn(void);
	//extern void init_codecs_tw(void);
	//extern void init_codecs_hk(void);
	//extern void init_codecs_iso2022(void);
	//extern void init_multiprocessing(void);
	extern void init_elementtree(void);
	//extern void init_ctypes(void);
	extern void init_hashlib(void);
}
#endif

void appendPythonModule(const char* moduleName, PyObject* (* initfunc)()) {
	int result = PyImport_AppendInittab(moduleName, initfunc);
	SmartBody::util::log("initialize module %s, result = %d", moduleName, result);
}


void initPython() {
	XMLPlatformUtils::Initialize();

	Py_NoSiteFlag = 1;

#if defined(__ANDROID__) || defined(SB_IPHONE)
	//#if 0
		//appendPythonModule("pyexpat", initpyexpat);
		appendPythonModule("_functools", init_functools);
		appendPythonModule("itertools", inititertools);
		appendPythonModule("cStringIO", initcStringIO);
		appendPythonModule("cPickle", initcPickle);
		appendPythonModule("cmath", initcmath);
		appendPythonModule("math", initmath);
		appendPythonModule("time", inittime);
		appendPythonModule("datetime", initdatetime);
		appendPythonModule("_collections", init_collections);
		appendPythonModule("operator", initoperator);
		appendPythonModule("_md5", init_md5);
		appendPythonModule("_sha", init_sha);
		appendPythonModule("_sha", init_sha);
		appendPythonModule("_sha256", init_sha256);
		appendPythonModule("_sha512", init_sha512);
		appendPythonModule("select", initselect);
		appendPythonModule("fcntl", initfcntl);
		appendPythonModule("_struct", init_struct);
		//appendPythonModule("_ctypes_test", init_ctypes_test);
		appendPythonModule("_weakref", init_weakref);
		appendPythonModule("array", initarray);
		appendPythonModule("strop", initstrop);
		//appendPythonModule("future_builtins", initfuture_builtins);
		appendPythonModule("_random", init_random);
		appendPythonModule("_bisect", init_bisect);
		//appendPythonModule("_heapq", init_heapq);
		//appendPythonModule("_fileio", init_fileio);
		appendPythonModule("_functools", init_functools);
		//appendPythonModule("_json", init_json);

		//appendPythonModule("_testcapi", init_testcapi);
		//appendPythonModule("_hotshot", init_hotshot);
		//appendPythonModule("_lsprof", init_lsprof);
		//appendPythonModule("_locale", init_locale);
		//appendPythonModule("_csv", init_csv);
		//appendPythonModule("unicodedata", initunicodedata);
		appendPythonModule("parser", initparser);
		//appendPythonModule("mmap", initmmap);
		//appendPythonModule("syslog", initsyslog);
		//appendPythonModule("audioop", initaudioop);
		//appendPythonModule("imageop", initimageop);
		//appendPythonModule("termios", inittermios);
		//appendPythonModule("resource", initresource);
		appendPythonModule("binascii", initbinascii);
		//appendPythonModule("_multibytecodec", init_multibytecodec);
		//appendPythonModule("_codecs_kr", init_codecs_kr);
		//appendPythonModule("_codecs_jp", init_codecs_jp);
		//appendPythonModule("_codecs_cn", init_codecs_cn);
		//appendPythonModule("_codecs_tw", init_codecs_tw);
		//appendPythonModule("_codecs_hk", init_codecs_hk);
		//appendPythonModule("_codecs_iso2022", init_codecs_iso2022);
		//appendPythonModule("_multiprocessing", init_multiprocessing);
		appendPythonModule("_elementtree", init_elementtree);
		//appendPythonModule("_ctypes", init_ctypes);
		//appendPythonModule("_hashlib", init_hashlib);
#endif

	PyImport_AppendInittab("SmartBody", &SmartBody::PyInit_SmartBody);

	Py_InitializeEx(0);
	if (PyErr_Occurred())
		PyErr_Print();

//	try {
//
//		if (PyErr_Occurred())
//			PyErr_Print();
//
//		setupPython();
//	} catch (...) {
//		if (PyErr_Occurred())
//			PyErr_Print();
//	}
}


void executeSafe(const std::function<void()>& fn) {
	try {
		fn();
	} catch (...) {

		if (PyErr_Occurred()) {
			auto msg = handle_pyerror();
			SmartBody::util::log("Python error: '%s'", msg.c_str());
			PyErr_Clear();
		}
	}
}

std::function<bool(SmartBody::SBScene& scene, const std::string&)> SBPython::scriptRunner = [](SmartBody::SBScene& scene, const std::string& script) -> bool {
	boost::python::object module = boost::python::import("__main__");
	boost::python::object dict = module.attr("__dict__");

	// add the .seq extension if necessary
	std::string candidateSeqName = script;
	if (candidateSeqName.find(".py") == std::string::npos) {
		candidateSeqName.append(".py");
	}

	std::string curFilename = scene.getAssetManager()->findFileName("script", candidateSeqName);
	SmartBody::util::log("script name = '%s', curFilename = '%s'", script.c_str(), curFilename.c_str());
	if (!curFilename.empty()) {
		try {
			// save the last directory so that a script path can be used as a relative pathing for asset loading or other use
			boost::filesystem::path scriptPath = curFilename;
			boost::filesystem::path scriptDir = scriptPath.parent_path();
			scene.setLastScriptDirectory(scriptDir.string());

			boost::python::exec_file(curFilename.c_str(), dict, dict);
			return true;
		} catch (...) {
			if (PyErr_Occurred()) {
				PyErr_Print();
			}
			auto event = scene.getEventManager()->createEvent("error", script, scene.getStringFromObject(&scene));
			scene.getEventManager()->handleEvent(event);
			delete event;
			return false;
		}
	}

	SmartBody::util::log("Could not find Python script '%s'", script.c_str());
	return false;
};
std::function<bool(SmartBody::SBScene& scene, const std::string&)> SBPython::commandRunner = [](SmartBody::SBScene& scene, const std::string& command) -> bool {
	try {
		boost::python::object module = boost::python::import("__main__");
		boost::python::object dict = module.attr("__dict__");
		//SmartBody::util::log("executePython = %s",command.c_str());
		boost::python::exec(command.c_str(), dict, dict);


		return true;
	} catch (...) {
		if (PyErr_Occurred()) {
			PyErr_Print();
		}
		auto event = scene.getEventManager()->createEvent("error", command, scene.getStringFromObject(&scene));
		scene.getEventManager()->handleEvent(event);
		delete event;
		return false;
	}
};



