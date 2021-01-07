#ifndef		_ME_CT_PHYSICS_CONTROLLER_H_
#define _ME_CT_PHYSICS_CONTROLLER_H_

#include <sb/SBController.h>
#include <sb/sbm_character.hpp>
#include <map>

class MeCtPhysicsController : public SmartBody::SBController
{
	public:
		static std::string CONTROLLER_TYPE;

	private:
		double			_prevTime;
		double			_dt;
		float 			_duration;
		SkChannelArray	_channels;
		SbmCharacter* 	_character;
		boost::intrusive_ptr<SmartBody::SBSkeleton>     _skeletonCopy;
		bool			_valid;	
	public:
		explicit MeCtPhysicsController(SbmCharacter* character);
		~MeCtPhysicsController() override;

		bool getValid() const						{return _valid;}
		void setValid(bool v)				{_valid = v;}		
	private:
		bool controller_evaluate(double t, MeFrameData& frame) override;
		SkChannelArray& controller_channels() override	{ return(_channels); }
		double controller_duration() override			{ return((double)_duration); }
		const std::string& controller_type() const override		{ return(CONTROLLER_TYPE); }
};

#endif