#ifndef _ME_CT_DATA_RECEIVER_H_
#define _ME_CT_DATA_RECEIVER_H_

#include <sb/SBController.h>
#include <sk/sk_skeleton.h>
#include <map>
#include "sr/sr_shared_ptr.hpp"
class MeCtDataReceiver : public SmartBody::SBController
{
	public:
		static std::string CONTROLLER_TYPE;

	private:
		double			_prevTime;
		double			_dt;
		float 			_duration;
		SkChannelArray	_channels;
		boost::intrusive_ptr<SkSkeleton> 	_skeleton;
		bool			_valid;
		std::map<std::string, SrVec>	_posMap;				// global position
		std::map<std::string, SrVec>	_startingPos;			// starting position
		std::map<std::string, SrQuat>	_quatMap;				// local rotation
		std::map<std::string, SrVec>	_localPosMap;			// local position

	public:
		explicit MeCtDataReceiver(SmartBody::SBPawn& pawn, boost::intrusive_ptr<SkSkeleton> skel);
		~MeCtDataReceiver() override;

		bool getValid()						{return _valid;}
		void setValid(bool v)				{_valid = v;}

		void setLocalPosition(const std::string& jName, SrVec& pos);
		void setGlobalPosition(const std::string& jName, SrVec& pos);
		void setLocalRotation(const std::string& jName, SrQuat& q);
		void removeLocalRotation(const std::string& jName);
		void removeLocalPosition(const std::string& jName);

	private:
		virtual bool controller_evaluate(double t, MeFrameData& frame);
		virtual SkChannelArray& controller_channels()	{ return(_channels); }
		virtual double controller_duration()			{ return((double)_duration); }
		virtual const std::string& controller_type() const		{ return(CONTROLLER_TYPE); }
};

#endif