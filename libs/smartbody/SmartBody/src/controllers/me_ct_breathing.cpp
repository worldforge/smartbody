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

#include <controllers/me_ct_breathing.h>
#include "gwiz_math.h"
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"

std::string MeCtBreathing::type_name = "Breathing";

MeCtBreathing::MeCtBreathing (SmartBody::SBPawn& pawn) : SmartBody::SBController(pawn)
{
	_motion = nullptr;
	_last_apply_frame = 0;

	_local_time_offset = 0;

	_default_breath_cycle = new LinearBreathCycle();
	_breath_layers.emplace_front(std::make_unique<BreathLayer>(std::unique_ptr<BreathCycle>(_default_breath_cycle), -1));

	_pending_breath_layer = nullptr;
	_pending_pop = false;

	_previous_breath_is_inspiring = current_breath_layer()->cycle->is_inspiring();
	
	_pending_motion = nullptr;
	_bpm = 15;
	_pending_bpm = -1;
	_expiratory_reserve_volume_threshold = 0;

	_incremental = true;
	_blendChannelBreathingMotion = nullptr;

	// breathing settings
	setDefaultAttributeGroupPriority("Breathing", 450);
	addDefaultAttributeString("breathing.motion", "", "Breathing");
	addDefaultAttributeBool("breathing.useBlendChannels", false, "Breathing");
	addDefaultAttributeDouble("breathing.bpm", 15, "Breathing");
	addDefaultAttributeDouble("breathing.min", 15, "Breathing");
	addDefaultAttributeDouble("breathing.max", 15, "Breathing");
	addDefaultAttributeBool("breathing.incremental", false, "Breathing");
}

MeCtBreathing::~MeCtBreathing () = default;

void MeCtBreathing::setUseBlendChannels(bool val)
{
	if (_useBlendChannels && !val)
	{
		setMotion(nullptr);
	}

	_useBlendChannels = val;

	if (_useBlendChannels && !_blendChannelBreathingMotion)
	{
		// blend channels are being turned on, so
		// create a motion containing simulated breathing
		_blendChannelBreathingMotion = new SmartBody::SBMotion(getScene());
		SkChannelArray breathingChannels;
		breathingChannels.add("breathX", SkChannel::XPos);
		breathingChannels.add("breathY", SkChannel::XPos);
		_blendChannelBreathingMotion->init(breathingChannels);
#if 0
		double dt = 0.016;
		double totalTime = 6.0; // time for a breathing cycle
		int counter = 0;
		for (double t = 0.0; t < totalTime; t += dt)
		{
			_blendChannelBreathingMotion->insert_frame(counter, (float) t);
			float* frame = _blendChannelBreathingMotion->posture(counter);
			frame[0] = (float) sin( 2 * M_PI * t / totalTime + M_PI ) + 1.0f;
			frame[1] = (float) (t / totalTime);
			counter++;
		}
		_blendChannelBreathingMotion->synch_points.set_time(0, .25 * totalTime, totalTime / 2,  totalTime / 2, totalTime / 2, .75 * totalTime, totalTime);
#else
		
		double dt = 1.0f / 30.0f;
		double totalTime = 1.0f / 30.0f * 50.0f; // time for a breathing cycle
		double cycle[] = {-314.076,-314.822,-306.105,-287.696,-266.611,-254.156,-221.962,-204.197,-171.479,-131.456,-97.085,-62.244,-34.1823,27.1129,48.913,104.886,128.478,
			               163.525,196.806,227.198,257.267,281.45,300.513,303.802,300.411,302.249,304.403,301.314,293.514,272.392,258.719,219.968,211.476,164.294,141.273,
						   94.0298,62.7389,13.6275,-15.886,-65.0322,-94.177,-138.845,-175.796,-198.208,-232.152,-248.494,-285.866,-290.819,-300.595,-305.594,-314.547};
		for (int counter = 0; counter < 51; counter++)
		{
			double time = ((float) counter) * dt;
			_blendChannelBreathingMotion->insert_frame(counter, (float) time);
			float* frame = _blendChannelBreathingMotion->posture(counter);
			frame[0] = (float) cycle[counter];
			frame[1] = (float) (time);
		}
		_blendChannelBreathingMotion->synch_points.set_time(0, 0, totalTime / 2,  totalTime / 2, totalTime / 2, totalTime, totalTime);
#endif
		setMotion(_blendChannelBreathingMotion);
	}
	
}

bool MeCtBreathing::getUseBlendChannels()
{
	return _useBlendChannels;
}

void MeCtBreathing::setMotion ( SkMotion* m ) 
{
	immediate_motion(m);

	_default_breath_cycle->min(_expiratory_reserve_volume_threshold);
	_default_breath_cycle->max(1.0f);

}

SkMotion* MeCtBreathing::getMotion () 
{
	return _motion;
}

void MeCtBreathing::push_breath_layer(std::unique_ptr<BreathCycle> cycle, int cyclesRemaining, bool smoothTransition)
{
	if(smoothTransition)
		_pending_breath_layer = std::make_unique<BreathLayer>(std::move(cycle), cyclesRemaining);
	else
		immediate_push_breath_layer(std::make_unique<BreathLayer>(std::move(cycle), cyclesRemaining));
}
void MeCtBreathing::pop_breath_layer()
{
	_pending_pop = true;
}
void MeCtBreathing::clear_breath_layers()
{
	//Keep the default layer
	while (_breath_layers.size() > 1) {
		_breath_layers.pop_front();
	}
}
BreathLayer* MeCtBreathing::current_breath_layer()
{
	if(_breath_layers.empty())
		return nullptr;
	else
		return _breath_layers.front().get();
}
void MeCtBreathing::breaths_per_minute(float bpm, bool smooth_transition) 
{ 
	if(smooth_transition)
		_pending_bpm = bpm; 
	else
		immediate_breaths_per_minute(bpm);
}

void MeCtBreathing::immediate_push_breath_layer(std::unique_ptr<BreathLayer> layer)
{
	_breath_layers.emplace_front(std::move(layer));
}
void MeCtBreathing::immediate_pop_breath_layer()
{
	//Never pop the base layer
	if (_breath_layers.size() == 1) {
		return;
	}
	_breath_layers.pop_front();
}
void MeCtBreathing::immediate_breaths_per_minute(float bpm)
{
	_bpm = bpm;
	_pending_bpm = -1;
}
void MeCtBreathing::immediate_motion(SkMotion* motion)
{
	if (!motion)
		return;

	if ( _motion ) {
		if( motion == _motion ) {
			// Minimal init()
			_last_apply_frame = 0;
			return;
		}
		// else new motion
	}

	_motion = motion;
	_last_apply_frame = 0;

	_motion->move_keytimes ( 0 ); // make sure motion starts at 0

	if( _context ) {
		// Notify _context of channel change.
		_context->child_channels_updated( this );
	}

	_expiratory_reserve_volume_threshold = (float) _motion->time_ready() / (float) _motion->time_stroke_emphasis();

	// ?????
	//_motion->set_incremental_frames(_motion->time_ready());
}
void MeCtBreathing::controller_map_updated() 
{
	// Map motion channel index to context float buffer index
	if (!_motion)
		return;
	SkChannelArray& mChannels = _motion->channels();
	const int size = mChannels.size();

	_mChan_to_buff.size( size );

	if( _context ) {
		SkChannelArray& cChannels = _context->channels();

		for( int i=0; i<size; ++i ) {
			int chanIndex = cChannels.search( mChannels.name(i), mChannels.type(i) );
			_mChan_to_buff[ i ] = _context->toBufferIndex( chanIndex );
		}
	} else {
		_mChan_to_buff.setall( -1 );
	}
}

void MeCtBreathing::motion( SkMotion* motion )
{
	if (!_motion) 
		immediate_motion(motion);
	else
		_pending_motion = motion;
}

bool MeCtBreathing::controller_evaluate ( double t, MeFrameData& frame ) 
{
	if (!_motion)
		return true;

	float frameTime;
	bool isFrameTimeSet = false;
	while(!isFrameTimeSet)
	{
		if(current_breath_layer() == nullptr)
			return true; //No breathing, but the controller never finishes

		float desiredBreathsPerSecond = _bpm / 60.0f;
		float currentBreathsPerSecond = 1.0f / current_breath_layer()->cycle->duration();
		float speed = desiredBreathsPerSecond / currentBreathsPerSecond;

		double localTime = t - _local_time_offset;

		double cycleTime = fmod(localTime*speed, double(current_breath_layer()->cycle->duration()));
		frameTime = (float) _motion->time_stroke_emphasis() * current_breath_layer()->cycle->value(localTime, cycleTime);
		
		isFrameTimeSet = true;
		if(!_previous_breath_is_inspiring && current_breath_layer()->cycle->is_inspiring()) //New cycle is starting
		{
			if(_pending_bpm != -1)
			{
				_bpm = _pending_bpm;
				_pending_bpm = -1;
				_local_time_offset = t;
				isFrameTimeSet = false;
			}
			if(_pending_motion != nullptr)
			{
				immediate_motion(_pending_motion);
				_pending_motion = nullptr;
				isFrameTimeSet = false;
			}
			
			if(current_breath_layer()->cycles_remaining != -1)
			{
				current_breath_layer()->cycles_remaining--;
				if(current_breath_layer()->cycles_remaining == 0)
					pop_breath_layer();
				isFrameTimeSet = false;
			}
			if(_pending_pop)
			{
				immediate_pop_breath_layer();
				_pending_pop = false;
				_local_time_offset = t;
				isFrameTimeSet = false;
			}
			if(_pending_breath_layer != nullptr)
			{
				immediate_push_breath_layer(std::move(_pending_breath_layer));
				_local_time_offset = t;
				isFrameTimeSet = false;
			}

			if(current_breath_layer() != nullptr)
				_previous_breath_is_inspiring = current_breath_layer()->cycle->is_inspiring();
		}
	}
	
	_previous_breath_is_inspiring = current_breath_layer()->cycle->is_inspiring();
	if(_incremental)
	{
		_motion->apply( 
			frameTime, &(frame.buffer()[0]), &_mChan_to_buff, SkMotion::Linear, &_last_apply_frame, true );
	}
	else
	{
		_motion->apply( 
			frameTime, &(frame.buffer()[0]), &_mChan_to_buff, SkMotion::Linear, &_last_apply_frame );
	}

	return true;
}

SkChannelArray& MeCtBreathing::controller_channels ()
{
	if (_motion)
		return _motion->channels();
	else
		return _channels;
}

const std::string& MeCtBreathing::controller_type () const
{
	return type_name;
}

void MeCtBreathing::notify(SBSubject* subject)
{
	SmartBody::SBController::notify(subject);

	addDefaultAttributeString("breathing.motion", "", "Breathing");
	addDefaultAttributeBool("breathing.useBlendChannels", false, "Breathing");
	addDefaultAttributeFloat("breathing.bpm", 15, "Breathing");

	auto* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		const std::string& name = attribute->getName();
		if (name == "breathing.motion")
		{
			auto* attr = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			
			SmartBody::SBMotion* motion = getScene().getMotion(attr->getValue());
			if (!motion)
			{
				SmartBody::util::log("No motion named '%s' found.", attr->getValue().c_str());
			}
			else
			{
				this->motion(motion);
			}
		}
		else if (name == "breathing.useBlendChannels")
		{
			auto* attr = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			setUseBlendChannels(attr->getValue());		
		}
		else if (name == "breathing.bpm")
		{
			auto* attr = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
			breaths_per_minute((float) attr->getValue());
		}
	}
}

