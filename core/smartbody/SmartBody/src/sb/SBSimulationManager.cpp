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

#include "SBSimulationManager.h"
#include <sb/SBAttribute.h>
#include <sb/SBScene.h>
#include <sb/SBScript.h>
#include "SBUtilities.h"
#include <sbm/time_regulator.h>
#include <sbm/time_profiler.h>


namespace SmartBody {

SBProfiler::SBProfiler()
: profiler_p(std::make_unique<TimeIntervalProfiler>())
{
	setName("Profiler");

	setAttributeGroupPriority("Settings", 200);
	createIntAttribute("suppression", -1,true,"Settings",10,false,false,false,"");
	createIntAttribute("selection", -1,true,"Settings",20,false,false,false,"");
	createDoubleAttribute("absThreshold", 0.0,true,"Settings",30,false,false,false,"");
	createDoubleAttribute("relThreshold",0.0,true,"Settings",40,false,false,false,"");
	createBoolAttribute("dynamicAbs",false,true,"Settings",50,false,false,false,"");
	createBoolAttribute("dynamicRel",false,true,"Settings",60,false,false,false,"");
	createDoubleAttribute("sniff", 0.9,true,"Settings",70,false,false,false,"");
	createDoubleAttribute("avoid", 1.5,true,"Settings",80,false,false,false,"");
	createDoubleAttribute("decaying", .95,true,"Settings",90,false,false,false,"");
	createIntAttribute("rolling", 1 ,true,"Settings",100,false,false,false,"");

	setEnable(false);
}

SBProfiler::~SBProfiler() = default;

void SBProfiler::setEnable(bool val)
{
	SBService::setEnable(true);

	profiler_p->bypass(!val);
}

bool SBProfiler::isEnable()
{
	return SBService::isEnable();
}

void SBProfiler::printLegend()
{
	profiler_p->print_legend();
}

void SBProfiler::printStats()
{
	profiler_p->print();
}

void SBProfiler::printReport()
{
	profiler_p->print();
}

void SBProfiler::setupProfiler()
{
	profiler_p = std::make_unique<TimeIntervalProfiler>();
}

void SBProfiler::mark( const char* group_name, int level, const char* label )	{
	if( profiler_p ) profiler_p->mark( group_name, level, label );
}


void SBProfiler::mark_time( const char* group_name, int level, const char* label, double time )
{
	if (profiler_p)
		profiler_p->mark_time(group_name, level, label, time);
}


int SBProfiler::mark( const char* group_name )	{
	if( profiler_p ) return( profiler_p->mark( group_name ) );
	return( 0 );
}

void SBProfiler::updateProfiler( double in_time )
{
	if( profiler_p )	{
		profiler_p->update(in_time);
	}
}

void SBProfiler::notify( SBSubject* subject )
{
	SBService::notify(subject);

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);

	if (attribute)
	{
		std::string name = attribute->getName();
		if (name == "suppression")
		{
			IntAttribute* intAttr = dynamic_cast<IntAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_suppression(intAttr->getValue());
		}
		else if (name == "selection")
		{
			IntAttribute* intAttr = dynamic_cast<IntAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_selection(intAttr->getValue());
		}
		else if (name == "absThreshold")
		{
			DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_abs_threshold(doubleAttr->getValue());
		}
		else if (name == "relThreshold")
		{
			DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_rel_threshold(doubleAttr->getValue());
		}
		else if (name == "dynamicAbs")
		{
			BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_dynamic_abs(boolAttr->getValue());
		}
		else if (name == "dynamicRel")
		{
			BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_dynamic_rel(boolAttr->getValue());
		}
		else if (name == "sniff")
		{
			DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_sniff(doubleAttr->getValue());
		}
		else if (name == "avoid")
		{
			DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_avoid(doubleAttr->getValue());
		}
		else if (name == "decaying")
		{
			DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_decaying(doubleAttr->getValue());
		}
		else if (name == "rolling")
		{
			IntAttribute* intAttr = dynamic_cast<IntAttribute*>(attribute);
			if (profiler_p)
				profiler_p->set_rolling(intAttr->getValue());
		}
	}
}

SBSimulationManager::SBSimulationManager()
: _simStarted(false),
  _simPlaying(false),
  _simStopped(false),
  time(0.0),
  time_dt(0.16)
{
}

SBSimulationManager::~SBSimulationManager() = default;

void SBSimulationManager::printInfo()
{
	if (timer_p)	
		timer_p->print();
	else	
	{
		SmartBody::util::log( "TIME:%.3f ~ DT:%.3f %.2f:FPS\n",
			time,
			time_dt,
			1.0 / time_dt
		);
	}
}

void SBSimulationManager::printPerf(float v)
{
	if (timer_p)
	{
		if (v > 0.0) 
			timer_p->set_perf(v);
		else	
			timer_p->set_perf(10.0);	
	}
	else
		SmartBody::util::log("Time regulator does not exist!");
}

double SBSimulationManager::getTime()
{
	return time;
}

double SBSimulationManager::queryTime()
{
	if (timer_p)
		return timer_p->get_time();
	else
		return time;
}

double SBSimulationManager::getTimeDt()
{
	return time_dt;
}

void SBSimulationManager::setTime(double time)
{
	updateTimer(time);	
}

void SBSimulationManager::update()
{
	if (timer_p)
	{
		bool doUpdate = updateTimer();
		if (!doUpdate)
			return;
	}
}

bool SBSimulationManager::isStarted()
{
	if (timer_p)
		return timer_p->isStarted();
	else
	{
		if (_simStarted)
			return true;
		else
			return false;
	}
}

bool SBSimulationManager::isRunning()
{
	if (timer_p)
		return timer_p->isRunning();
	else
	{
		if (_simPlaying)
			return true;
		else
			return false;
	}
}


bool SBSimulationManager::isStopped()
{
	if (timer_p)
		return !timer_p->isRunning();
	return _simStopped;
}

void SBSimulationManager::reset()
{
	if (timer_p)	
		timer_p->reset();
	else
	{
		return;
	}
}

void SBSimulationManager::start()
{
	// run the start scripts
	std::map<std::string, SBScript*>& scripts = SmartBody::SBScene::getScene()->getScripts();
	for (auto & script : scripts)
	{
		script.second->start();
	}
	

	if (timer_p)	
	{
		timer_p->start();
	}
	else
	{
		_simStarted = true;
	}
}

void SBSimulationManager::stop()
{
	// run the stop scripts
	std::map<std::string, SBScript*>& scripts = SmartBody::SBScene::getScene()->getScripts();
	for (auto & script : scripts)
	{
		script.second->stop();
	}
	
	if (timer_p)	
	{
		timer_p->stop();
	}
	else
	{
		_simStarted = false;
		_simPlaying = false;
		_simStopped = true;
	}
}

void SBSimulationManager::pause()
{
	if (timer_p)
	{
		timer_p->pause();
	}
	else
	{
		_simPlaying = false;
	}
}

void SBSimulationManager::resume()
{
	if (timer_p)
	{
		timer_p->resume();
	}
	else
	{
		_simPlaying = true;
	}
}

void SBSimulationManager::step(int numSteps)
{
	if (timer_p)
	{
		timer_p->step(numSteps);
	}
}

void SBSimulationManager::setSleepFps(float v)
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator not exist!");
		return;
	}
	timer_p->set_sleep_fps(v);
}

void SBSimulationManager::setEvalFps(float v)
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator does not exist!");
		return;
	}
	timer_p->set_eval_fps(v);
}

void SBSimulationManager::setSimFps(float v)
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator does not exist!");
		return;
	}
	timer_p->set_sim_fps(v);
}

void SBSimulationManager::setSleepDt(float v)
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator not exist!");
		return;
	}
	timer_p->set_sleep_dt(v);
}

void SBSimulationManager::setEvalDt(float v)
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator does not exist!");
		return;
	}
	timer_p->set_eval_dt(v);
}

void SBSimulationManager::setSimDt(float v)
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator does not exist!");
		return;
	}
	timer_p->set_sim_dt(v);
}

void SBSimulationManager::setSpeed(float v)
{
	if (timer_p)	
		timer_p->set_speed(v);
	else
		SmartBody::util::log("Time regulator does not exist!");
}

void SBSimulationManager::setupTimer()
{

	register_timer( std::make_unique<TimeRegulator>() );
}

void SBSimulationManager::setSleepLock()
{
	if (!timer_p)	
	{
		SmartBody::util::log("Time regulator does not exist!");
		return;
	}
	timer_p->set_sleep_lock();
}

void SBSimulationManager::set_perf(float val)
{
	if (timer_p)
		timer_p->set_perf(val);
}

void SBSimulationManager::register_timer( std::unique_ptr<TimeRegulator> time_reg )	{
	timer_p = std::move(time_reg);
}


bool SBSimulationManager::updateTimer( double in_time)
{
	if( timer_p )	{
		bool ret = timer_p->update( in_time );
		time = timer_p->get_time();
		time_dt = timer_p->get_dt();
		return( ret );
	}
	double prev = time;
	time = in_time;
	time_dt = time - prev;
	return( true );
}

SBAPI void SBSimulationManager::stepDt( double dt )
{
	double newTime = time + dt;
	updateTimer(newTime);
	//time_dt = dt;	
}

}

