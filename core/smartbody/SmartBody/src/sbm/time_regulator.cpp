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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sb/SBUtilities.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "sbm/time_regulator.h"

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX        /* Don't defined min() and max() */
#endif
#include <windows.h>
#include <mmsystem.h>
#if SBM_REPORT_MEMORY_LEAKS
#include <malloc.h>
#include <crtdbg.h>
#endif
#endif

#ifdef WIN32_LEAN_AND_MEAN

#else
#include <sys/time.h>
#endif

#define ENABLE_QPF_TIME 	(1)

///////////////////////////////////////////////////////////////////////////////////

double SBM_get_real_time_nodelay( void ) {
#ifdef WIN32
#if ENABLE_QPF_TIME

	static int once = 1;
	static double inv_freq;
	static LONGLONG ref_quad;
	LARGE_INTEGER c;
	
	if( once )	{
		once = 0;
		LARGE_INTEGER f;
		QueryPerformanceFrequency( &f );
		inv_freq = 1.0 / (double)f.QuadPart;

//SmartBody::util::log( "SBM_get_real_time: dt:%.18f fps: %.3f\n", inv_freq, (double)f.QuadPart );

		QueryPerformanceCounter( &c );
		ref_quad = c.QuadPart;
	}
	QueryPerformanceCounter( &c );
	LONGLONG diff_quad = c.QuadPart - ref_quad;
	if( diff_quad < 0 ) {
		diff_quad = 0;
	}
	return( (double)diff_quad * inv_freq );
	
#else
	return( (double)timeGetTime() / 1000.0 );
#endif
#else
	struct timeval tv;
	gettimeofday( &tv, NULL );
	return( tv.tv_sec + ( tv.tv_usec / 1000000.0 ) );
#endif
}

double SBM_get_real_time( void ) {

	static double t = -1.0;
	double next_t = 0.0;
	do	{
		next_t = SBM_get_real_time_nodelay();
	}
	while( next_t == t );
	t = next_t;
	return( t );
}

void SBM_sleep_msec( int msec )	{
#ifdef WIN32
	static int once = 1;
	if( once )	{
		once = 0;
		timeBeginPeriod( 1 ); // millisecond resolution
	}
	Sleep( msec );
#else
	sleep(msec * .001);
#endif
}

double SBM_sleep_wait( double prev_time, double target_dt, bool verbose )	{ // sleep to reach target loop rate

	if( target_dt > 0.0 )	{
		
		int passes = 0;
		while( 1 )	{
		
			double curr_time = SBM_get_real_time();
			double elapse_dt = curr_time - prev_time;

			if( elapse_dt < target_dt )	{

				double diff = target_dt - elapse_dt;
				int wait_msec = (int)( diff * 1000.0 + 1.0 );
				
				if( wait_msec > 0 ) {
					if( verbose && passes )	{
						SmartBody::util::log( "SBM_sleep_wait NOTICE: slipped %f seconds == %d msec", 
							diff, wait_msec 
						);
					}
					
					SBM_sleep_msec( wait_msec );
					
				}
				passes++;
			}
			else	{
				return( curr_time );
			}
		}
	}
	return( SBM_get_real_time() );
}

///////////////////////////////////////////////////////////////////////////////////

void TimeRegulator::start( double in_time ) {

	if( in_time < 0.0 )	{
		start_time = SBM_get_real_time();
		extern_src = false;
	}
	else	{
		start_time = in_time;
		extern_src = true;
	}
	
	clock_time = start_time;
	prev_loop_time = 0.0;
	started = true;
}

void TimeRegulator::stop(  ) {
	pause();
}

void TimeRegulator::reset( double in_time ) {

	double reset_val;
	if( in_time < 0.0 )	{
		reset_val = 0.0;
	}
	else	{
		reset_val = in_time;
	}
	
	out_time = reset_val;
	eval_wait = 0.0;
}

bool TimeRegulator::update( double in_time ) {
	bool abort = false;
	
	if( !started ) start( in_time );
	
	if( in_time < 0.0 )	{
		if( extern_src )	{
			if( verbose ) SmartBody::util::log( "TimeRegulator::update NOTICE: switch to internal" );
			start( in_time );
			abort = true;
		}
		clock_time = SBM_sleep_wait( clock_time, sleep_dt, verbose );
	}
	else	{
		if( !extern_src )	{
			if( verbose ) SmartBody::util::log( "TimeRegulator::update NOTICE: switch to external" );
			start( in_time );
			abort = true;
		}
		clock_time = in_time;
	}

	double loop_time = clock_time - start_time;
	double loop_dt = loop_time - prev_loop_time;
	prev_loop_time = loop_time;


	if( !abort ) {
		if( loop_dt < 0.0 ) {
			if( extern_src )	{
				if( verbose ) SmartBody::util::log( "TimeRegulator::update ERR: negative external increment: %f", loop_dt );
			}
			else
				SmartBody::util::log( "TimeRegulator::update ERR: negative internal increment!!!!: %f", loop_dt );
			abort = true;
		}
		else
		if( loop_dt == 0.0 )	{
			if( extern_src )	{
				if( verbose ) SmartBody::util::log( "TimeRegulator::update NOTICE: zero external increment" );
				if( ( ( sleep_dt == 0.0 )&&( eval_dt == 0.0 )&&( sim_dt > 0.0 ) ) == false )	{
					abort = true;
				}
			}
			else	{
				SmartBody::util::log( "TimeRegulator::update ERR: zero internal increment!!!!" );
				abort = true;
			}
		}
	}
	if( abort ) {
		out_dt = 0.0;
		if( verbose ) SmartBody::util::log( "TimeRegulator::update ABORT" );
		return( false );
	}
	
	bool eval_sim = false;
	eval_wait += loop_dt;

	if( eval_dt > 0.0 )	{
		if( eval_wait >= eval_dt )	{
			eval_sim = true;
			eval_wait = 0.0;
		}
	}
	else	{
		eval_sim = true;
	}

	real_time += loop_dt;

	if( eval_sim )	{

		real_dt = real_time - prev_real_time;
		prev_real_time = real_time;

		if( do_pause )	{
			if( verbose ) SmartBody::util::log( "TimeRegulator::update PAUSE" );
			do_pause = false;
			paused = true;
		}
		if( do_steps )	{
			do_resume = true;
		}
		if( do_resume )	{
			if( verbose ) SmartBody::util::log( "TimeRegulator::update RESUME" );
			do_resume = false;
			paused = false;
		}
		if( do_steps )	{
			if( verbose ) SmartBody::util::log( "TimeRegulator::update STEP" );
			do_steps--;
			do_pause = true;
		}
		if( !paused )	{
			if( sim_dt > 0.0 )	{
				out_dt = sim_dt;
			}
			else	{
				out_dt = real_dt * speed;
			}
			out_time += out_dt;
			update_perf();
			return( true );
		}
	}
	
	out_dt = 0.0;
	if( verbose ) SmartBody::util::log( "TimeRegulator::update SKIP" );
	return( false );
}

void TimeRegulator::update_perf( void )	{

	if( perf_interval <= 0.0 ) return;

	perf_real_sum += real_dt;
	perf_sim_sum += sim_dt;
	perf_count++;

	if( perf_real_sum >= perf_interval )	{
		double avg = perf_real_sum / perf_count;

		if( perf_sim_sum > 0.0 ) {
			double sim_avg = perf_sim_sum / perf_count;
			SmartBody::util::log( "PERF: REAL dt:%.3f fps:%.1f ~ SIM dt:%.3f fps:%.1f", 
//			SmartBody::util::log( "PERF: REAL dt:%f fps:%f ~ SIM dt:%f fps:%f\n", 
				avg, 1.0 / avg,
				sim_avg, 1.0 / sim_avg
			);
		}
		else	{
			SmartBody::util::log( "PERF: dt:%.3f fps:%.1f", avg, 1.0 / avg );
//			SmartBody::util::log( "PERF: dt:%f fps:%f\n", avg, 1.0 / avg );
		}
		perf_real_sum = 0.0;
		perf_sim_sum = 0.0;
		perf_count = 0;
	}
}

void TimeRegulator::print( void )	{
	SmartBody::util::log( "TimeRegulator( %.3f ): ", real_time );
	SmartBody::util::log( "   status: %s", paused ? "PAUSED" : ( do_steps ? "STEPPING" : "RUNNING" ) );
	SmartBody::util::log( "    speed: %.3f", speed );
	SmartBody::util::log( "    sleep: %.4f : %.2f fps", sleep_dt, ( sleep_dt > 0.0 )? ( 1.0 / sleep_dt ): 0.0 );
	SmartBody::util::log( "     eval: %.4f : %.2f fps", eval_dt, ( eval_dt > 0.0 )? ( 1.0 / eval_dt ): 0.0 );
	SmartBody::util::log( "      sim: %.4f : %.2f fps", sim_dt, ( sim_dt > 0.0 )? ( 1.0 / sim_dt ): 0.0 );
	SmartBody::util::log( "   out dt: %.4f : %.2f fps", out_dt, ( out_dt > 0.0 )? ( 1.0 / out_dt ): 0.0 );
	SmartBody::util::log( " out time: %.3f", out_time );
}

void TimeRegulator::print_update( int id, double in_time ) {

	bool res = update( in_time );
	SmartBody::util::log( "[%d]:(%d): in:%f time:%f dt:%f", id, res, in_time, get_time(), get_dt() );
}

///////////////////////////////////////////////////////////////////////////////////

//#include <stdlib.h>

void test_time_regulator( void )	{
	TimeRegulator tr;
	int c = 0;
	
	tr.start();
	tr.set_verbose();
	
	SmartBody::util::log( "=====================================================\n" );

#if 0
	tr.print_update( c++ );
	tr.print_update( c++ );
	tr.print_update( c++ );
	tr.print_update( c++ );
	tr.print_update( c++ );

	SmartBody::util::log( "=====================================================\n" );
	
	tr.print_update( c++, 1.0 );
	tr.print_update( c++, 1.1 );
	tr.print_update( c++, 2.0 );
	tr.print_update( c++, 2.2 );
	tr.print_update( c++, 2.2 );

	SmartBody::util::log( "=====================================================\n" );

	tr.print_update( c++ );
	tr.print_update( c++ );
	tr.print_update( c++ );
	tr.print_update( c++ );
	tr.print_update( c++ );

	SmartBody::util::log( "=====================================================\n" );
	
	tr.print_update( c++, 1.0 );
	tr.print_update( c++, 2.0 );
	tr.print_update( c++, 3.0 );
	tr.print_update( c++, 0.0 );
	tr.print_update( c++, 4.0 );

	SmartBody::util::log( "=====================================================\n" );
	tr.print();
#endif
	
	tr.start();
//	tr.set_speed( 10.0 );
	tr.set_sleep_fps( 10.0 );
	tr.set_eval_fps( 10.0 );
//	tr.set_sim_fps( 10.0 );

#if 0
	for( int i = 0; i<20; i++ )	{
		double r = (double)rand() / (double)RAND_MAX;
		tr.print_update( i, r );
	}
	SmartBody::util::log( "=====================================================\n" );
#endif

#if 0
	tr.set_perf( 1.0 );
	for( int i = 0; i<100; i++ )	{
		tr.update();
	}
	SmartBody::util::log( "=====================================================\n" );
	tr.set_perf( 0.0 );
#endif

#if 1
	tr.set_sleep_fps( 0.0 );
	tr.set_eval_fps( 0.0 );
	tr.set_sim_fps( 10.0 );
	tr.print();
	for( int i = 0; i<30; i++ )	{
		if( i == 10 ) tr.start();
		if( i == 20 ) tr.reset();

		tr.print_update( i, 0.0 );
	}
	SmartBody::util::log( "=====================================================\n" );
#endif

#if 0
	for( int i = 0; i<100; i++ )	{
		if( i == 10 ) tr.pause();
		if( i == 20 ) tr.step();
		if( i == 30 ) tr.resume();
//		if( i == 40 ) tr.pause();
		if( i == 50 ) tr.step( 10 );
		if( i == 70 ) tr.resume();
		if( i == 80 ) tr.start();
//		if( i == 90 ) tr.reset();

		tr.print_update( i );
	}
	SmartBody::util::log( "=====================================================\n" );
#endif
}
















