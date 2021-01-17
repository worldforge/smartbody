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

#include "SteerSuiteEngineDriver.h"
#include "sr/sr_vec.h"
#include <iostream>

using namespace std;
using namespace SteerLib;
using namespace Util;

//
// constructor
//
SteerSuiteEngineDriver::SteerSuiteEngineDriver() : SBObject()
{
	_alreadyInitialized = false;
	_engine = nullptr;
	_done = false;
	_startTime = 0;


}

SteerSuiteEngineDriver::~SteerSuiteEngineDriver()
{
	if (_engine) {
		_engine->postprocessSimulation();
		_engine->cleanupSimulation();
		_engine->finish();
	}
}

bool SteerSuiteEngineDriver::isInitialized()
{
	return _alreadyInitialized;
}

bool SteerSuiteEngineDriver::isDone()
{
	return _done;
}
void SteerSuiteEngineDriver::setDone(bool val)
{
	_done = val;
}

void SteerSuiteEngineDriver::setStartTime(double time)
{
	_startTime = time;
}
double SteerSuiteEngineDriver::getStartTime()
{
	return _startTime;
}

void SteerSuiteEngineDriver::setLastUpdateTime(double time)
{
	_lastUpdateTime = time;
}
double SteerSuiteEngineDriver::getLastUpdateTime()
{
	return _lastUpdateTime;
}


//
// init()
//
void SteerSuiteEngineDriver::init(std::unique_ptr<SteerLib::SimulationOptions> options)
{
	if (_alreadyInitialized) {
		throw GenericException("SteerSuiteEngineDriver::init() - should not call this function twice.\n");
	}

	_alreadyInitialized = true;
	_done = false;

	_engine = std::make_unique<SimulationEngine>();
	//Move ownership over to the Steerlib Engine
	_engine->init(options.release(), this);
}


//
// run() - never returns, will end the program properly when appropriate
//
void SteerSuiteEngineDriver::run()
{
	/*
	bool verbose = true;  // TODO: make this a user option...
	bool done = false;

	if (verbose) std::cout << "\rInitializing...\n";
	_engine->initializeSimulation();

	if (verbose) std::cout << "\rPreprocessing...\n";
	_engine->preprocessSimulation();

	// loop until the engine tells us its done
	while (!done) {
		if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber();
		done = !_engine->update(false);
	}

	if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber() << std::endl;

	if (verbose) std::cout << "\rPostprocessing...\n";
	_engine->postprocessSimulation();

	if (verbose) std::cout << "\rCleaning up...\n";
	_engine->cleanupSimulation();

	if (verbose) std::cout << "\rDone.\n";
	*/
}

//
// finish() - cleans up.
//
void SteerSuiteEngineDriver::finish()
{
	_engine->finish();
	_engine.reset();
	_alreadyInitialized = false;
}


void SteerSuiteEngineDriver::loadSimulation()
{
	_engine->initializeSimulation();
	std::cout << "Simulation loaded.\n";
}

void SteerSuiteEngineDriver::startSimulation()
{
	std::cout << "Simulation started.\n";
	_engine->preprocessSimulation();
}

void SteerSuiteEngineDriver::stopSimulation()
{
	_engine->postprocessSimulation();
	std::cout << "Simulation stopped.\n";
}

void SteerSuiteEngineDriver::unloadSimulation()
{
	_engine->cleanupSimulation();
	std::cout << "Simulation unloaded.\n";
}

float SteerSuiteEngineDriver::collisionPenetration( SrVec pos, float radius, SteerLib::AgentInterface* agent )
{
	std::set<SpatialDatabaseItemPtr> neighborList;
	std::set<SpatialDatabaseItemPtr>::iterator neighbor;
	// exclude the current agent
	_engine->getSpatialDatabase()->getItemsInRange(neighborList, pos.x-radius,pos.x+radius,pos.z-radius,pos.z+radius, agent);
	Util::Point steerPt = Util::Point(pos.x , 0.0f, pos.z);
	float maxPenetration = 0.f;
	for (neighbor = neighborList.begin(); neighbor != neighborList.end(); ++neighbor) {

		// this way, collisionKey will be unique across all objects in the spatial database.
		//unsigned int collisionKey = reinterpret_cast<unsigned int>((*neighbor));
		SpatialDatabaseItemPtr item = (*neighbor);
		int* collisionKey = reinterpret_cast<int*>(item);
		float penetration = 0.0f;
		penetration = (*neighbor)->computePenetration(steerPt, radius);
		if (penetration > maxPenetration)
		{
			maxPenetration = penetration;						
		}
	}
	return maxPenetration;
}


