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

#ifndef _SIMULATIONMANAGER_H
#define _SIMULATIONMANAGER_H

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <boost/noncopyable.hpp>
#include <memory>
class TimeRegulator;
class TimeIntervalProfiler;

namespace SmartBody {

class SBProfiler : public SBService
{
	public:
		SBAPI SBProfiler();
		SBAPI ~SBProfiler();

		SBAPI void setEnable(bool val) override;
		SBAPI bool isEnable() override;

		SBAPI void setupProfiler();
		SBAPI void updateProfiler(double in_time = -1.0 );
		void mark( const char* group_name, int level, const char* label );
		void mark_time( const char* group_name, int level, const char* label, double time );
		int mark( const char* group_name );

		SBAPI void printLegend();
		SBAPI void printStats();
		SBAPI void printReport();

		void notify( SBSubject* subject ) override;

	protected:
		std::unique_ptr<TimeIntervalProfiler> profiler_p;
};



class SBSimulationManager : public SBObject
{
	public:
		SBAPI SBSimulationManager();
		SBAPI ~SBSimulationManager();

		SBAPI bool isStarted();
		SBAPI bool isRunning();
		SBAPI bool isStopped();

		SBAPI void printInfo();
		SBAPI void printPerf(float v);
		SBAPI double getTime();
		SBAPI double queryTime();
		SBAPI double getTimeDt();
		SBAPI void setTime(double time);
		SBAPI void stepDt(double dt);
		SBAPI void start() override;
		SBAPI void stop() override;
		SBAPI void reset();
		SBAPI void pause();
		SBAPI void resume();
		SBAPI void update();
		SBAPI void step(int numSteps);

		SBAPI void setupTimer();
		SBAPI void setSleepFps(float v);
		SBAPI void setEvalFps(float v);
		SBAPI void setSimFps(float v);
		SBAPI void setSleepDt(float v);
		SBAPI void setEvalDt(float v);
		SBAPI void setSimDt(float v);
		SBAPI void setSpeed(float v);
		SBAPI void setSleepLock();
		
		void set_perf(float val);

		void register_timer( std::unique_ptr<TimeRegulator> time_reg );
		SBAPI bool updateTimer( double in_time = -1.0 );

	protected:
		bool _simStarted;
		bool _simPlaying;
		bool _simStopped;

		std::unique_ptr<TimeRegulator> timer_p;
		
		double			time;
		double			time_dt;

		SBProfiler* _profiler;
};

}

#endif