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

#ifndef __COMMAND_LINE_ENGINE_DRIVER_H__
#define __COMMAND_LINE_ENGINE_DRIVER_H__

/// @file SteerSuiteEngineDriver.h
/// @brief Declares the SteerSuiteEngineDriver class
#include <sb/SBTypes.h>
#include <SteerLib.h>
#include <PPRAgent.h>
#include <sb/SBObject.h>

class SteerSuiteEngineDriver :  public SmartBody::SBObject, public SteerLib::EngineControllerInterface
{
public:
	SteerSuiteEngineDriver();
	~SteerSuiteEngineDriver();


	void init(SteerLib::SimulationOptions * options);
	SBAPI void finish();
	void run();

	SBAPI bool isInitialized();
	bool isDone();
	void setDone(bool val);
	void setStartTime(double time);
	double getStartTime();
	void setLastUpdateTime(double time);
	double getLastUpdateTime();
	float collisionPenetration( SrVec pos, float radius, SteerLib::AgentInterface* agent );	

	/// @name The EngineControllerInterface
	/// @brief The CommandLineEngineDriver does not support any of the engine controls.
	//@{
	virtual bool isStartupControlSupported() { return false; }
	virtual bool isPausingControlSupported() { return false; }
	virtual bool isPaused() { return false; }
	virtual void loadSimulation();
	virtual void unloadSimulation();
	virtual void startSimulation();
	virtual void stopSimulation();
	virtual void pauseSimulation() { throw Util::GenericException("CommandLineEngineDriver does not support pauseSimulation()."); }
	virtual void unpauseSimulation() { throw Util::GenericException("CommandLineEngineDriver does not support unpauseSimulation()."); }
	virtual void togglePausedState() { throw Util::GenericException("CommandLineEngineDriver does not support togglePausedState()."); }
	virtual void pauseAndStepOneFrame() { throw Util::GenericException("CommandLineEngineDriver does not support pauseAndStepOneFrame()."); }
	//@}

	SteerLib::SimulationEngine * _engine;

		// These functions are kept here to protect us from mangling the instance.
	// Technically the CommandLineEngineDriver is not a singleton, though.
//	SteerSuiteEngineDriver(const SteerSuiteEngineDriver & );  // not implemented, not copyable
//	SteerSuiteEngineDriver& operator= (const SteerSuiteEngineDriver & );  // not implemented, not assignable

protected:
	bool _alreadyInitialized;
	bool _done;
	double _startTime;
	double _lastUpdateTime;
	SteerLib::SimulationOptions * _options;


};


#endif