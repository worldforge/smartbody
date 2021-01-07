#include "controllers/me_ct_noise_controller.h"
#include <sr/sr_euler.h>

#include <sb/SBSkeleton.h>

std::string MeCtNoiseController::CONTROLLER_TYPE = "NoiseController";

MeCtNoiseController::MeCtNoiseController(SbmCharacter* character) : SmartBody::SBController(*character)
{
	_character = character;
	_valid = true;
	_prevTime = 0.0;
	_duration = 0.0;
	perlinScale = 0.02f;
	perlinFrequency = 0.03f;
	perlinDuration = 1.f/perlinFrequency;

	_fadeStartTime = -1.0;
	_fadeInInterval = 0.3f;
	_fadeOutInterval = 0.3f;
	_fadeType = 0;	// fade type: 0 - no fade, 1 - fade in, 2 - fade out
}

MeCtNoiseController::~MeCtNoiseController() = default;

void MeCtNoiseController::setValid(bool v)
{
	_fadeStartTime = _prevTime;
	if (v)
	{
		_valid = v;
		_fadeType = 1;
	}
	else
	{
		_fadeType = 2;
	}
	//SmartBody::util::log("fade for noise controller %d", _fadeType);
}

void MeCtNoiseController::setJointNoise( std::vector<std::string>& jointNames, float scale, float frequency )
{
	perlinMap.clear();
	for (auto jname : jointNames)
	{
			SmartBody::SBJoint* joint = _character->getSkeleton()->getJointByName(jname);
		if (joint) // joint actually exist 
		{
			perlinMap[jname] = Perlin();
			perlinMap[jname].init(); // reinitialize perlin noise for the joint
		}
	}
	if (scale > 0.f)
		perlinScale = scale;
	if (frequency > 0.f)
	{
		perlinFrequency = frequency;
		perlinDuration = 1.f/frequency;
	}	
}

float MeCtNoiseController::getNormalizeTime( float t, float offset )
{
	int nT = int(t/perlinDuration);
	float normalizeTime = (t - perlinDuration*nT)/perlinDuration;
	return normalizeTime;
}

bool MeCtNoiseController::controller_evaluate(double t, MeFrameData& frame)
{
	
	if (_prevTime == 0)
		_dt = 0.016;
	else
	{
		_dt = t - _prevTime;
	}	
	_prevTime = t;
	float invDt = 1.f/0.016;
	if (_dt > 1e-6)
		invDt = 1.f/(float)_dt;	
#if 1
	if (_valid && _context )
	{
		double fadeWeight = -1;
		// use linear fade
		if (_fadeStartTime >= 0 && _fadeType != 0)
		{
			double _fadeInterval = _fadeInInterval;
			if (_fadeType == 1)
				_fadeInterval = _fadeInInterval;
			if (_fadeType == 2)
				_fadeInterval = _fadeOutInterval;
			double ratio = (t - _fadeStartTime) / _fadeInterval;
			if (ratio <= 1)
			{
				if (_fadeType == 1)
					fadeWeight = ratio;
				if (_fadeType == 2)
					fadeWeight = 1 - ratio;
				//SmartBody::util::log("fade %d weight %f, time %f, start %f", _fadeType, fadeWeight, t, _fadeStartTime);
			}
			if (_fadeType == 2 && ratio > 1)
			{
				_valid = false;
				return true;
			}
		}

		SrVec normalizeTime;
		for (int k=0;k<3;k++)
		{			
			normalizeTime[k] = getNormalizeTime((float)t, perlinDuration*0.3f*k);
		}
		
		std::map<std::string, Perlin>::iterator mi;
		for ( mi  = perlinMap.begin();
			  mi != perlinMap.end();
			  mi++)
		{
			SrQuat oldQuat, newQuat;
			SrVec oldPos;
			std::string jname = mi->first;
			Perlin& perlinNoise = mi->second;
			getJointChannelValues(jname, frame, oldQuat, oldPos);
			SrVec noiseVec;
			newQuat = oldQuat;
			for (int k=0;k<3;k++)
			{
				noiseVec[k] = perlinNoise.noise1(normalizeTime[k])*perlinScale + newQuat.getData(k+1);
				newQuat.setData(k+1,noiseVec[k]);
			}
			newQuat.normalize();
			if (fadeWeight >= 0 && _fadeType != 0)
			{
				SrQuat weightedQuat;
				if (_fadeType == 1)
					weightedQuat = slerp(oldQuat, newQuat, (float)fadeWeight);
				if (_fadeType == 2)
					weightedQuat = slerp(oldQuat, newQuat, (float)fadeWeight);
				weightedQuat.normalize();
				//SmartBody::util::log("fade %d weight %f, time %f, start %f, Original %f %f %f %f, weighted %f %f %f %f, new %f %f %f %f", _fadeType, fadeWeight, t, _fadeStartTime, oldQuat.w, oldQuat.x, oldQuat.y, oldQuat.z, weightedQuat.w, weightedQuat.x, weightedQuat.y, weightedQuat.z, newQuat.w, newQuat.x, newQuat.y, newQuat.z);
				setJointChannelQuat(jname, frame, weightedQuat);
			}
			else
				setJointChannelQuat(jname, frame, newQuat);
		}
	}	
#endif
	return true;
}

