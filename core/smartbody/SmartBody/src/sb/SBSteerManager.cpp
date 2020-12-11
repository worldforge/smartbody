/*************************************************************
Copyright (C) 2017 University of Southern California

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

#include "SBSteerManager.h"

#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBAttribute.h>
#include <sb/SBSteerAgent.h>
#include "SBUtilities.h"
#include <sbm/PPRAISteeringAgent.h>
#include <sbm/SteerSuiteEngineDriver.h>
#include <SteerLib.h>

namespace SmartBody {

SBSteerManager::SBSteerManager(SBScene& scene)
		: SBService(),
		  _scene(scene),
		  _driver(std::make_unique<SteerSuiteEngineDriver>()),
		  _maxUpdateFrequency(1)
{
	setName("steering");
#ifdef WIN32
			createStringAttribute("aimodule", "pprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif
#ifdef __linux__
			createStringAttribute("aimodule", "libpprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif
#ifdef __APPLE__
			createStringAttribute("aimodule", "libpprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif
#if defined(EMSCRIPTEN)
			createStringAttribute("aimodule", "pprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif

	createStringAttribute("engineOptions.testCaseSearchPath", "../../../../core/smartbody/steersuite-1.3/testcases/", true, "Basic", 60, false, false, false, "Path to find test cases");
	createStringAttribute("engineOptions.moduleSearchPath", SMARTBODY_LIBDIR "/", true, "Basic", 60, false, false, false, "Path to find agent shared libraries");
	createDoubleAttribute("gridDatabaseOptions.gridSizeX", 35, true, "Basic", 60, false, false, false, "Size of grid in x dimension.");
	createDoubleAttribute("gridDatabaseOptions.gridSizeZ", 35, true, "Basic", 60, false, false, false, "Size of grid in z dimension.");
	createIntAttribute("gridDatabaseOptions.numGridCellsX", 70, true, "Basic", 60, false, false, false, "Number of grid cells in x dimension.");
	createIntAttribute("gridDatabaseOptions.numGridCellsZ", 70, true, "Basic", 60, false, false, false, "Number of grid cells in z dimension.");
	createIntAttribute("gridDatabaseOptions.maxItemsPerGridCell", 7, true, "Basic", 60, false, false, false, "Max units per grid cell. If agent density is high, make sure increase this value.");
	createDoubleAttribute("initialConditions.radius", 0.4, true, "Basic", 60, false, false, false, "Initial radius of agents in meters.");
	createBoolAttribute("addBoundaryWalls", true, true, "Basic", 60, false, false, false, "Adds boundaries around the perimeter of the grid to prevent agents from leaving grid area.");
	createBoolAttribute("useEnvironmentCollisions", true, true, "Basic", 60, false, false, false, "Determines whether to include the environment (pawns) when determining steering path. If set to false, objects in the environment will be ignored.");
	createDoubleAttribute("maxUpdateFrequency", 60.0, true, "Basic", 60, false, false, false, "Maximum frequency of steering updates.");	
}

SBSteerManager::~SBSteerManager() = default;

SteerSuiteEngineDriver* SBSteerManager::getEngineDriver()
{
	return _driver.get();
}

void SBSteerManager::setEnable(bool enable)
{
	SBService::setEnable(enable);
	if (enable)
		start();
	else
		stop();
	SmartBody::SBAttribute* attribute = getAttribute("enable");
	if (attribute)
	{
		auto* enableAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		enableAttribute->setValueFast(enable);
	}
}

void SBSteerManager::beforeUpdate(double time)
{
	
}

void SBSteerManager::update(double time)
{
	if (getEngineDriver()->isInitialized())
	{
		if (!getEngineDriver()->isDone())
		{

			if (getEngineDriver()->getStartTime() == 0.0)
			{
				getEngineDriver()->setStartTime(_scene.getSimulationManager()->getTime());
				getEngineDriver()->setLastUpdateTime(_scene.getSimulationManager()->getTime() - _maxUpdateFrequency - .01);
			}

			double maxFrequency = getDoubleAttribute("maxUpdateFrequency");
			if (maxFrequency != 0.0)
				_maxUpdateFrequency = 1.0 / maxFrequency;
			else
				_maxUpdateFrequency = .016;
			double timeDiff = _scene.getSimulationManager()->getTime() - getEngineDriver()->getLastUpdateTime();
			const std::vector<std::string>& characterNames = _scene.getCharacterNames();
			for (const auto & characterName : characterNames)
			{
				SmartBody::SBCharacter* character = _scene.getCharacter(characterName);
				SmartBody::SBSteerAgent* steerAgent = getSteerAgent(character->getName());
				if (steerAgent)
					steerAgent->evaluate(timeDiff);
			}

			for (auto& entry: _pawnObstacles) {
				updatePawnObstacle(entry.second);
			}

			if (timeDiff >= _maxUpdateFrequency)
			{ // limit steering to 60 fps
				getEngineDriver()->setLastUpdateTime(_scene.getSimulationManager()->getTime());
			
				bool running = getEngineDriver()->_engine->update(false, true, (float) (_scene.getSimulationManager()->getTime() - getEngineDriver()->getStartTime()));
				if (!running)
					getEngineDriver()->setDone(true);
			}

		}
	}
}

void SBSteerManager::afterUpdate(double time)
{
}

void SBSteerManager::start()
{

	if (getEngineDriver()->isInitialized())
	{
		SmartBody::util::log("STEERSIM ALREADY STARTED");
		return;
	}

	auto* steerOptions = new SteerLib::SimulationOptions();
	steerOptions->moduleOptionsDatabase["testCasePlayer"]["testcase"] = "3-way-confusion-1.xml";
	std::string ai = dynamic_cast<SmartBody::StringAttribute*>( getAttribute("aimodule") )->getValue();
	//Zengrui:this is not useful, casue the attribute is nullptr, will cause null ptr error
	if (ai.empty()){
		return;
	}
	steerOptions->moduleOptionsDatabase["testCasePlayer"]["ai"] = ai;
	steerOptions->engineOptions.startupModules.insert("testCasePlayer");
	std::string testCases = dynamic_cast<SmartBody::StringAttribute*>( getAttribute("engineOptions.testCaseSearchPath") )->getValue();
	steerOptions->engineOptions.testCaseSearchPath = testCases;
	std::string moduleSearchPath = dynamic_cast<SmartBody::StringAttribute*>( getAttribute("engineOptions.moduleSearchPath") )->getValue();
	steerOptions->engineOptions.moduleSearchPath = moduleSearchPath;
	double gridSizeX = dynamic_cast<SmartBody::DoubleAttribute*>( getAttribute("gridDatabaseOptions.gridSizeX") )->getValue();
	double gridSizeZ = dynamic_cast<SmartBody::DoubleAttribute*>( getAttribute("gridDatabaseOptions.gridSizeZ") )->getValue();
	steerOptions->gridDatabaseOptions.gridSizeX = float(gridSizeX);
    steerOptions->gridDatabaseOptions.gridSizeZ = float(gridSizeZ);
	int numGridCellsX = dynamic_cast<SmartBody::IntAttribute*> (getAttribute("gridDatabaseOptions.numGridCellsX"))->getValue();
	int numGridCellsZ = dynamic_cast<SmartBody::IntAttribute*> (getAttribute("gridDatabaseOptions.numGridCellsZ"))->getValue();
	int maxItemsPerGridCell = dynamic_cast<SmartBody::IntAttribute*> (getAttribute("gridDatabaseOptions.maxItemsPerGridCell"))->getValue();
	//SmartBody::util::log("max Items per grid cell = %d",maxItemsPerGridCell);
	steerOptions->gridDatabaseOptions.numGridCellsX = numGridCellsX;
	steerOptions->gridDatabaseOptions.numGridCellsZ = numGridCellsZ;
	steerOptions->gridDatabaseOptions.maxItemsPerGridCell = maxItemsPerGridCell;

	bool setBoundaries = getBoolAttribute("addBoundaryWalls");
	if (setBoundaries)
	{
		for (auto & _boundaryObstacle : _boundaryObstacles)
		{
			getEngineDriver()->_engine->removeObstacle(_boundaryObstacle.get());
			getEngineDriver()->_engine->getSpatialDatabase()->removeObject(_boundaryObstacle.get(), _boundaryObstacle->getBounds());
		}
		_boundaryObstacles.clear();
		auto* top = new SteerLib::BoxObstacle((float) -gridSizeX / 2.0f, (float) gridSizeX / 2.0f, 0.0f,  1.0f, (float) -gridSizeZ / 2.0f, (float) -gridSizeZ / 2.0f + 1.0f);
		_boundaryObstacles.emplace_back(top);
		auto* bottom = new SteerLib::BoxObstacle((float) -gridSizeX / 2.0f, (float) gridSizeX / 2.0f, 0.0f,  1.0f, (float) gridSizeZ / 2.0f - 1.0f, (float) gridSizeZ / 2.0f);
		_boundaryObstacles.emplace_back(bottom);
		auto* left = new SteerLib::BoxObstacle((float) -gridSizeX / 2.0f, (float) -gridSizeX / 2.0f + 1.0f, 0.0f,  1.0f, (float) -gridSizeZ / 2.0f, (float) gridSizeZ / 2.0f);
		_boundaryObstacles.emplace_back(left);
		auto* right = new SteerLib::BoxObstacle((float) gridSizeX / 2.0f - 1.0f, (float) gridSizeX / 2.0f, 0.0f,  1.0f, (float) -gridSizeZ / 2.0f, (float) gridSizeZ / 2.0f);
		_boundaryObstacles.emplace_back(right);
	}

	//	customize the item per grid cell
	//	steerOptions->gridDatabaseOptions.maxItemsPerGridCell = maxItemPerCell;


	//SmartBody::util::log("INIT STEERSIM");
	try {
		getEngineDriver()->init(steerOptions);
	} catch (Util::GenericException& ge) {
		SmartBody::util::log("Problem starting steering engine: %s", ge.what()); 
		getEngineDriver()->finish();
		delete steerOptions;
		return;
	} catch (std::exception& e) {
		if (e.what())
			SmartBody::util::log("Problem starting steering engine: %s", e.what()); 
		else
			SmartBody::util::log("Unknown problem starting steering engine: %s", e.what()); 

		getEngineDriver()->finish();
		delete steerOptions;
		return;
	}

	//SmartBody::util::log("LOADING STEERSIM");
	getEngineDriver()->loadSimulation();

	int numSetup = 0;
	// create an agent based on the current characters and positions
	SteerLib::ModuleInterface* pprAIModule = getEngineDriver()->_engine->getModule(ai);
	const std::vector<std::string>& characterNames = _scene.getCharacterNames();
	for (const auto & characterName : characterNames)
	{
		SmartBody::SBCharacter* character = _scene.getCharacter(characterName);
		SmartBody::SBSteerManager* steerManager = _scene.getSteerManager();
		SmartBody::SBSteerAgent* steerAgent = steerManager->getSteerAgent(character->getName());
		if (!steerAgent)
		{
			SmartBody::util::log("No steering agent for character %s", character->getName().c_str());
			continue;
		}

		float x, y, z;
		float yaw, pitch, roll;
		character->get_world_offset(x, y, z, yaw, pitch, roll);
		SteerLib::AgentInitialConditions initialConditions;
		initialConditions.position = Util::Point( x * _scene.getScale(), 0.0f, z * _scene.getScale() );
		Util::Vector orientation = Util::rotateInXZPlane(Util::Vector(0.0f, 0.0f, 1.0f), yaw * 3.14159f / 180.0f);
		initialConditions.direction = orientation;
		double initialRadius = dynamic_cast<SmartBody::DoubleAttribute*>( getAttribute("initialConditions.radius") )->getValue();
		if (initialRadius == 0.0)
			initialConditions.radius = 0.3f;//0.2f;//0.4f;
		else
			initialConditions.radius = (float) initialRadius;
		initialConditions.speed = 0.0f;
		initialConditions.goals.clear();
		initialConditions.name = character->getName();		
		SteerLib::AgentInterface* agent = getEngineDriver()->_engine->createAgent( initialConditions, pprAIModule );			
		auto* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
		ppraiAgent->setAgent(agent);
		agent->reset(initialConditions, dynamic_cast<SteerLib::EngineInterface*>(pprAIModule));
		SmartBody::util::log("Setting up steering agent for character %s", character->getName().c_str());
		numSetup++;
	}
	if (numSetup == 0)
	{
		//SmartBody::util::log("No characters set up with steering. Steering will need to be restarted when new characters are available.");
	}

	bool useEnvironment = getBoolAttribute("useEnvironmentCollisions");

	if (useEnvironment)
	{
		// adding obstacles to the steering space
		const std::vector<std::string>& pawns = _scene.getPawnNames();
		for (const auto & pawnName : pawns)
		{
			SBPawn* pawn = _scene.getPawn(pawnName);
			auto* character = dynamic_cast<SBCharacter*>(pawn);
			if (character) continue; // do not set obstacle for the character, it will mess up the steering
// 			if ((*iter).second->getGeomObject())
// 				(*iter).second->initSteeringSpaceObject();
			if (pawn && pawn->getGeomObject() && pawn->getGeomObject()->geomType() != "null")
				createObstacleForPawn(pawnName);
		}

	}

	// add any boundary walls, if applicable
	for (auto & _boundaryObstacle : _boundaryObstacles)
	{
		getEngineDriver()->_engine->addObstacle(_boundaryObstacle.get());
		getEngineDriver()->_engine->getSpatialDatabase()->addObject(_boundaryObstacle.get(), _boundaryObstacle->getBounds());
	}

	//SmartBody::util::log("STARTING STEERSIM");
	getEngineDriver()->startSimulation();
	getEngineDriver()->setStartTime(0.0f);

	double maxFrequency = getDoubleAttribute("maxUpdateFrequency");
	if (maxFrequency != 0.0)
		_maxUpdateFrequency = 1.0 / maxFrequency;
	else
		_maxUpdateFrequency = .016;
}

void SBSteerManager::stop()
{
	
	if (getEngineDriver()->isInitialized())
	{
		getEngineDriver()->stopSimulation();
		getEngineDriver()->unloadSimulation();
		getEngineDriver()->finish();

		const std::vector<std::string>& characterNames = _scene.getCharacterNames();
		for (const auto & characterName : characterNames)
		{
			SmartBody::SBCharacter* character = _scene.getCharacter(characterName);
			SmartBody::SBSteerAgent* steerAgent = getSteerAgent(character->getName());
		
			if (steerAgent)
			{
				auto* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setAgent(nullptr);
			}
				
		}

		//No need to remove from engine as that's reset anyway.
		_pawnObstacles.clear();
		_boundaryObstacles.clear();
	}
}

void SBSteerManager::updatePawnObstacle(PawnObstacle& pawnObstacle) {
	float steerScale = 1.0f / _scene.getScale();
	auto pawn = pawnObstacle.pawn;
	// get the size of the steering object
	float x, y, z, h, p, r;
	pawn.get_world_offset(x, y, z, h, p, r);
	gwiz::quat_t q = gwiz::euler_t(p,h,r);
	SrQuat pawnQ = SrQuat((float)q.w(), (float)q.x(), (float)q.y(), (float)q.z());
	SrVec size = this->getVec3Attribute("collisionShapeScale");
	size = size*pawnQ;
	pawnObstacle.steeringSpaceObjSize.x = fabs(size.x);
	pawnObstacle.steeringSpaceObjSize.y = fabs(size.y);
	pawnObstacle.steeringSpaceObjSize.z = fabs(size.z);

	float xmin = (x - pawnObstacle.steeringSpaceObjSize.x) / steerScale;
	float xmax = (x + pawnObstacle.steeringSpaceObjSize.x) / steerScale;
	float ymin = (y - pawnObstacle.steeringSpaceObjSize.y) / steerScale;
	float ymax = (y + pawnObstacle.steeringSpaceObjSize.y) / steerScale;
	float zmin = (z - pawnObstacle.steeringSpaceObjSize.z) / steerScale;
	float zmax = (z + pawnObstacle.steeringSpaceObjSize.z) / steerScale;

	const Util::AxisAlignedBox& box = pawnObstacle.obstacle->getBounds();
	if (fabs(box.xmax - xmax) > .0001 ||
		fabs(box.xmin - xmin) > .0001 ||
		fabs(box.ymax - ymax) > .0001 ||
		fabs(box.ymin - ymin) > .0001 ||
		fabs(box.zmax - zmax) > .0001 ||
		fabs(box.zmin - zmin) > .0001)
	{
		getEngineDriver()->_engine->getSpatialDatabase()->removeObject(pawnObstacle.obstacle.get(), pawnObstacle.obstacle->getBounds());
		auto mutableBox = const_cast<Util::AxisAlignedBox&>(box);
		mutableBox.xmax = xmax;
		mutableBox.xmin = xmin;
		mutableBox.ymax = ymax;
		mutableBox.ymin = ymin;
		mutableBox.zmax = zmax;
		mutableBox.zmin = zmin;
		getEngineDriver()->_engine->getSpatialDatabase()->addObject(pawnObstacle.obstacle.get(), pawnObstacle.obstacle->getBounds());
	}
}

void SBSteerManager::applyPawnObstacle(PawnObstacle& pawnObstacle) {
	getEngineDriver()->_engine->getSpatialDatabase()->removeObject(pawnObstacle.obstacle.get(), pawnObstacle.obstacle->getBounds());
	getEngineDriver()->_engine->getSpatialDatabase()->addObject(pawnObstacle.obstacle.get(), pawnObstacle.obstacle->getBounds());

}



PawnObstacle* SBSteerManager::createObstacleForPawn(const std::string& name) {
	if (_pawnObstacles.find(name) != _pawnObstacles.end()) {
		return nullptr;
	}

	auto pawn = _scene.getPawn(name);
	if (pawn) {
		float steerScale = 1.0f / _scene.getScale();
		// get the size of the steering object
		float x, y, z, h, p, r;
		pawn->get_world_offset(x, y, z, h, p, r);
		gwiz::quat_t q = gwiz::euler_t(p,h,r);
		SrQuat pawnQ = SrQuat((float)q.w(), (float)q.x(), (float)q.y(), (float)q.z());
		SrVec size = this->getVec3Attribute("collisionShapeScale");
		size = size*pawnQ;
		SrVec			steeringSpaceObjSize;
		steeringSpaceObjSize.x = fabs(size.x);
		steeringSpaceObjSize.y = fabs(size.y);
		steeringSpaceObjSize.z = fabs(size.z);

		float xmin = (x - steeringSpaceObjSize.x) / steerScale;
		float xmax = (x + steeringSpaceObjSize.x) / steerScale;
		float ymin = (y - steeringSpaceObjSize.y) / steerScale;
		float ymax = (y + steeringSpaceObjSize.y) / steerScale;
		float zmin = (z - steeringSpaceObjSize.z) / steerScale;
		float zmax = (z + steeringSpaceObjSize.z) / steerScale;

		auto obstacle = std::make_unique<SteerLib::BoxObstacle>(xmin, xmax, ymin, ymax, zmin, zmax);
		getEngineDriver()->_engine->addObstacle(obstacle.get());
		getEngineDriver()->_engine->getSpatialDatabase()->addObject(obstacle.get(), obstacle->getBounds());

		auto result = _pawnObstacles.emplace(name, PawnObstacle{*pawn, std::move(obstacle), steeringSpaceObjSize});
		if (result.second) {
			return &result.first->second;
		}
	}
	return nullptr;
}

SBSteerAgent* SBSteerManager::createSteerAgent(const std::string& name)
{
	
	auto iter = _steerAgents.find(name);
	if (iter != _steerAgents.end())
	{
		SmartBody::util::log("Steer agent with name %s already exists.", name.c_str());
		return iter->second.get();
	}
	SBCharacter* character = _scene.getCharacter(name);
	if (!character)
	{
		SmartBody::util::log("Character named '%s' does not exist, steering agent cannot be constructed.", name.c_str());
		return nullptr;
	}
	SBSteerAgent* agent = new PPRAISteeringAgent(character);
	_steerAgents.insert(std::pair<std::string, SBSteerAgent*>(name, agent));
	return agent;
}

void SBSteerManager::removeSteerAgent(const std::string& name)
{
	auto iter = _steerAgents.find(name);
	if (iter != _steerAgents.end())
	{
		_steerAgents.erase(iter);
		return;
	}
	SmartBody::util::log("Steer agent with name %s does not exist.", name.c_str());
}

int SBSteerManager::getNumSteerAgents()
{
	return _steerAgents.size();
}

SBSteerAgent* SBSteerManager::getSteerAgent(const std::string& name)
{
	auto iter = _steerAgents.find(name);
	if (iter == _steerAgents.end())
		return nullptr;
	else
		return (*iter).second.get();
}

std::vector<std::string> SBSteerManager::getSteerAgentNames()
{
	std::vector<std::string> steerAgentNames;

	for (auto & _steerAgent : _steerAgents)
	{
		steerAgentNames.emplace_back(_steerAgent.first);
	}

	return steerAgentNames;
}

std::map<std::string, std::unique_ptr<SBSteerAgent>>& SBSteerManager::getSteerAgents()
{
	return _steerAgents;
}

void SBSteerManager::onPawnDelete(SBPawn* pawn)  {
	auto I = _pawnObstacles.find(pawn->getName());
	if (I != _pawnObstacles.end()) {
		// first delete pawn from underlying grid...
		getEngineDriver()->_engine->getSpatialDatabase()->removeObject(I->second.obstacle.get(), I->second.obstacle->getBounds());
		// ..., then delete obstacle itself, so that the neighborhood map can be recomputed with the remaining obstacles
		getEngineDriver()->_engine->removeObstacle(I->second.obstacle.get());
		_pawnObstacles.erase(I);
	}
}


void SBSteerManager::onCharacterDelete(SBCharacter* character)
{
	removeSteerAgent(character->getName());
}

}
