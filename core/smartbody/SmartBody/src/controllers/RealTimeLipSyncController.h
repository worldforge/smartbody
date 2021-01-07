#pragma once

#include <sb/SBController.h>
#include <sb/SBCharacter.h>
#include <sb/SBMotion.h>
#include <sbm/sr_linear_curve.h>

class RealTimeLipSyncController : public SmartBody::SBController
{
	public:
		explicit RealTimeLipSyncController(SmartBody::SBPawn& pawn);
		~RealTimeLipSyncController() override;

		bool controller_evaluate ( double t, MeFrameData& frame ) override;

		void notify(SmartBody::SBSubject* subject) override;

	protected:
		void updateLipSyncChannels();
		void setup();
		void smoothCurve(std::vector<float>& c, float windowSize);

		double _lastPhonemeTime;
		std::string _lastPhoneme;
		std::vector<double> _currentPhonemeTimings;
		std::vector<std::string> _currentPhonemes;
		double _startTime;
		SkChannelArray  _channels;

		std::vector<std::pair<std::string, srLinearCurve*> > _currentCurves;
};
		
