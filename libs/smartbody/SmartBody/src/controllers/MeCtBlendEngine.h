#pragma once
#include "controllers/me_ct_data_interpolation.h"
#include "controllers/me_ct_barycentric_interpolation.h"
#include "controllers/me_ct_motion_parameter.h"
#include "controllers/me_ct_motion_profile.h"
#include "controllers/me_ct_jacobian_IK.hpp"
#include "controllers/me_ct_ccd_IK.hpp"
#include "controllers/me_ct_constraint.hpp"
#include <sb/sbm_pawn.hpp>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>

#include "sr/sr_shared_ptr.hpp"

class SbmCharacter;

class MeCtBlendEngine
{
protected:
	bool          valid;
	bool          ikInit;
	int           reachType;
	//SbmCharacter* character;
	boost::intrusive_ptr<SmartBody::SBSkeleton>   skeletonCopy, skeletonRef;
	std::string   rootJointName;
	std::vector<SkMotion*> motionData;
	SmartBody::SBMotion*             refMotion;   // reference motion for time warping
	MotionExampleSet      motionExamples;

	MeCtIKTreeScenario    ikScenario;
	vector<SmartBody::SBJoint*>      affectedJoints; // list of joints that are affected by motion interpolation & IK. 
	// for motion interpolation
	dVector               targetParameter;
	MotionParameter*      motionParameter;		
	DataInterpolator*     dataInterpolator;	
	ResampleMotion*       interpMotion; // pointer to motion interface for generating motion example

	vector<InterpolationExample*>* interpExampleData;
	vector<InterpolationExample*>* resampleData;	
	BodyMotionFrame       inputMotionFrame, outputMotionFrame, idleMotionFrame;		
	float curRefTime, du;
public:
	vector<SrVec>         examplePts,resamplePts;
	VecOfSimplex*         simplexList;
public:
	MeCtBlendEngine(boost::intrusive_ptr<SmartBody::SBSkeleton> sk, std::string rootJointName);
	virtual ~MeCtBlendEngine();
	void init(const std::string& paramFuncType);
	bool isValid() const { return valid; }
	//SbmCharacter*   getCharacter() { return character; }		
	MotionParameter* getMotionParameter() { return motionParameter; }	
	BodyMotionFrame& outputMotion() { return outputMotionFrame; }	
	void setBlendParameter(dVector& para, std::vector<double>& outWeight);	
	void getBlendParameterFromWeights(dVector& outPara, std::vector<double>& inWeight);
	void getMotionParameter(const std::string& motion, dVector& outPara);
	
	void updateBlend(float t, float dt, BodyMotionFrame& inputFrame);	
	void updateMotionExamples(SmartBody::SBScene& scene, const std::vector<SmartBody::SBMotion*>& inMotionSet, std::string interpolatorType);
	IKTreeNodeList& ikTreeNodes() { return ikScenario.ikTreeNodes; }
protected:
	void updateSkeletonCopy();		
	DataInterpolator* createInterpolator(std::string interpolatorType);
	ResampleMotion*   createInterpMotion();		
};

