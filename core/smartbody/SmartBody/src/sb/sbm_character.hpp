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

#ifndef SBM_CHARACTER_HPP
#define SBM_CHARACTER_HPP

#include <sb/SBTypes.h>
#include <cfloat>
#include <iostream>
#include <string>
#include <set>
#include <map>

#include <sb/SBPawn.h>
#include <sb/SBFaceDefinition.h>
#define MeCtSchedulerClass MeCtScheduler2


#include "sbm_pawn.hpp"

#include <sb/nvbg.h>
#include <sb/MiniBrain.h>
#include <sb/SBReach.h>

//#include <controllers/me_spline_1d.hpp>
class MeCtMotionPlayer;
class MeCtPAnimation;
class MeCtParamAnimation;
class MeCtSaccade;
class MeCtBasicLocomotion;
class MeCtNewLocomotion;
class MeCtDataReceiver;
class SBPhysicsCharacter;
class MeCtPhysicsController;
class MeCtNoiseController;
class MeCtMotionPlayer;
class MeCtMotionRecorder;
class MeCtSchedulerClass;
class MeCtEyeLidRegulator;
class MeCtFace;
class MeCtEyeLid;
class MeCtBreathing;
class MeCtBasicLocomotion;
class MeCtReachEngine;
class MeCtPosePostProcessing;
class MeCtMotionGraph;
class MeCtGenericHand;
class RealTimeLipSyncController;
class ActionUnit;
class SkMotion;
struct GeneralParam;

namespace SmartBody
{
	class SpeechInterface;
}

class SbmCharacter : public SmartBody::SBPawn	{

	// Locomotion + steering
public:
	std::list<SrVec> trajectoryBuffer;
	std::vector<float> trajectoryGoalList;
	static const int trajectoryLength = 1000;
	
	bool _reachTarget;
	bool _lastReachStatus;
	//SBPhysicsCharacter* phyChar;

	enum LocomotionType {Basic, Example, Procedural, New};
	
	LocomotionType								locomotion_type;
	std::string									statePrefix;
	//bool checkExamples();

public:
	// Static Constants
	std::map<std::string, GeneralParam*>*   param_map;

	std::vector<SrVec> footStepList[2];	
	int footStepIdx[2];
	SrVec          curFootIKPos[2];
	float flightTime[2];

protected:
	// The implementation to be used for speech (nullptr if unset)
	SmartBody::SpeechInterface* speech_impl;
	// The voice code used by the implementation (empty string if unset) 
	std::string 			voice_code;

	SmartBody::SpeechInterface* speech_impl_backup;
	std::string 			voice_code_backup;

	// Evaluation time face data
	SkMotion*				face_neutral;

	std::map< std::string, SkMotion* > 		viseme_map;

	//MeCtLocomotionAnalysis* locomotion_ct_analysis;


	// The term "viseme" in the following variables is a misnomer,
	// and may also refer to an action unit or other face shape.
	// They are all controlled by the "character .. viseme .." command.
	// Following patch can be removed after the other parts are fullfilled
	// viseme name patch
	// in case there's a mis-match between audio file&remote speech viseme name and Bonebus&SBM viseme name
	// e.g. Oh <-> oh, OW <-> oh
	// also doing this for bilateral au, e.g. mapping au_1 to au_1_left and au_1_right
	// in this way, au_1 can control both au_1_left and au_1_right
	std::map<std::string, std::vector<std::string> > viseme_name_patch;

	// associate each joint with a physics object
	// currently, we assume the vis-geo as the geometry ( or an capsule to the child if vis-geo is not available )
	//std::map<std::string, SBPhysicsObj*> jointPhyObjMap; 	


	// Viseme Curve Info
	bool	use_viseme_curve;
	float	viseme_time_offset;
	float	viseme_sound_offset;
	float	viseme_magnitude;
	int 	viseme_channel_count;
	std::string _diphoneSetName;

	std::unique_ptr<SmartBody::SBFaceDefinition> _faceDefinition;

	bool _isControllerPruning;

	SmartBody::Nvbg* _nvbg;
	std::unique_ptr<SmartBody::MiniBrain> _miniBrain;
public:
	//  Methods
	//SbmCharacter();
	explicit SbmCharacter( const char * char_name );
	SbmCharacter( const char* character_name, std::string type);
	~SbmCharacter() override;
	
	int init( boost::intrusive_ptr<SkSkeleton> skeleton_p,
			  SmartBody::SBFaceDefinition* faceDefinition,
			  std::map< std::string, GeneralParam * >* param_map,
			  const char* classType);

	int setup() override;

	virtual void createStandardControllers();
	//virtual void createMinimalControllers();
	//* Overrides SbmPawn::prune_controller_tree()
	int prune_controller_tree() override;

	void addFootStep(int iLeg, SrVec& footPos, bool Update = false);
	SBAPI std::vector<SrVec>& getFootSteps(int ileg);

	/**
	 *  Sets the character's speech implementation.
	 */
	int set_speech_impl(SmartBody::SpeechInterface *speech_impl); //set speech returns CMD_SUCCESS  

	/**
	 *  Sets the character's backup speech implementation.
	 */
	int set_speech_impl_backup(SmartBody::SpeechInterface *speech_impl); //set speech returns CMD_SUCCESS  

	/**
	 *  Gets the character's speech implementation.
	 */
	SmartBody::SpeechInterface* get_speech_impl() const; //returns speech implementation if set or nullptr if not
	
	/**
	 *  Gets the character's backup speech implementation.
	 */
	SmartBody::SpeechInterface* get_speech_impl_backup() const; //returns speech implementation if set or nullptr if not
	
	/**
	 *  Sets the character's voice code string.
	 */
	int set_voice_code(const std::string& voice_code); //set Voice returns CMD_SUCCESS  

	/**
	 *  Sets the character's backup voice code string which will be activated if the primary voice fails.
	 */
	int set_voice_code_backup(const std::string& voice_code); //set Voice returns CMD_SUCCESS  


	/**
	 *  Gets the character's voice code string.
	 */
	const std::string& get_voice_code() const; //returns voice if exists or nullptr if not

	/**
	 *  Gets the character's backup voice code string.
	 */
	const std::string& get_voice_code_backup() const; //returns voice if exists or nullptr if not


	// Prioritized Schedules for behaviors (known as "blocking" in manual animation)
	// TODO: Rename by body part, rather than controller type
	boost::intrusive_ptr<MeCtSchedulerClass>	posture_sched_p; // legs / stance / posture
	boost::intrusive_ptr<MeCtSchedulerClass>	motion_sched_p;  // full body motions
	boost::intrusive_ptr<MeCtSchedulerClass>	gaze_sched_p;    // back / chest / spine
	// TODO: Arms
	boost::intrusive_ptr<MeCtSchedulerClass>    constraint_sched_p;
	boost::intrusive_ptr<MeCtSchedulerClass>	reach_sched_p; // reaching
	boost::intrusive_ptr<MeCtSchedulerClass>	grab_sched_p; // grabbing (temp)
	boost::intrusive_ptr<MeCtSchedulerClass>	head_sched_p; // neck / head orientation
	boost::intrusive_ptr<MeCtSchedulerClass>	param_sched_p; // general parameters
	boost::intrusive_ptr<MeCtSchedulerClass>	overlayMotion_sched_p;  // full body motions

	boost::intrusive_ptr<MeCtEyeLidRegulator>	eyelid_reg_ct_p;
	boost::intrusive_ptr<MeCtFace>				face_ct;
	boost::intrusive_ptr<MeCtEyeLid>				eyelid_ct;

	boost::intrusive_ptr<MeCtPhysicsController> physics_ct;
	boost::intrusive_ptr<MeCtDataReceiver>	datareceiver_ct;
	boost::intrusive_ptr<MeCtParamAnimation> param_animation_ct;			// layer0
	boost::intrusive_ptr<MeCtParamAnimation> param_animation_ct_layer1;	// layer1
	boost::intrusive_ptr<MeCtParamAnimation> param_animation_ct_layer2;	// layer2
	boost::intrusive_ptr<MeCtParamAnimation> head_param_anim_ct;

	boost::intrusive_ptr<MeCtMotionPlayer>	motionplayer_ct;
	boost::intrusive_ptr<MeCtSaccade>		saccade_ct;
	boost::intrusive_ptr<MeCtNoiseController> noise_ct;
	boost::intrusive_ptr<MeCtMotionRecorder>  record_ct;
	boost::intrusive_ptr<MeCtBreathing>			breathing_p;
	boost::intrusive_ptr<MeCtBasicLocomotion>	basic_locomotion_ct;
	boost::intrusive_ptr<MeCtNewLocomotion>		new_locomotion_ct;
	boost::intrusive_ptr<MeCtPosePostProcessing> postprocess_ct;
	boost::intrusive_ptr<MeCtMotionGraph>        motiongraph_ct;
	boost::intrusive_ptr<MeCtGenericHand>		generic_hand_ct;
	boost::intrusive_ptr<RealTimeLipSyncController> realTimeLipSyncCt;

	std::vector<float>	viseme_history_arr;
	int 	viseme_channel_start_pos;
	int 	viseme_channel_end_pos;

	void schedule_viseme_curve( 
		const char* viseme, 
		double start_time, 
		float* curve_info, int num_keys, int num_key_params, 
		float ramp_in, float ramp_out 
	);
	void schedule_viseme_trapezoid( 
		const char* viseme,
		double start_time,
		float weight,
		float duration,
		float ramp_in, 
		float ramp_out 
	);
	void schedule_viseme_blend_curve( 
		const char* viseme, 
		double start_time, 
		float weight, 
		float* curve_info, int num_keys, int num_key_params 
	);
	void schedule_viseme_blend_ramp( 
		const char* viseme, 
		double start_time, 
		float weight, 
		float rampin_duration
	);
	void forward_visemes( double curTime);
	void forward_parameters( double curTime);

	void inspect_skeleton( SkJoint* joint_p, int depth = 0 );
	void inspect_skeleton_local_transform( SkJoint* joint_p, int depth = 0 );
	void inspect_skeleton_world_transform( SkJoint* joint_p, int depth = 0 );

	/** Returns true if face controller is active on this character. */
	bool is_face_controller_enabled();


	static int character_cmd_func( srArgBuffer& args );

	static int character_init_cmd( srArgBuffer& args);


public:
	
	SBAPI void addBlendShapeChannel(std::string shapeName);
	SBAPI void addBlendShapeChannels(std::vector<std::string>& shapeNames);

	static void addTagSkMotion(int tag, SkMotion* motion, std::set<std::pair<int,SkMotion*> >& motionSet);
		
	// viseme curve related functions
	void set_viseme_curve_mode( bool mode )		{ use_viseme_curve = mode; }
	bool get_viseme_curve_mode( ) const	{ return( use_viseme_curve ); }
	void set_viseme_time_delay( float timeDelay ) { viseme_time_offset = timeDelay; }
	float get_viseme_time_delay( ) const	{ return( viseme_time_offset ); }
	void set_viseme_sound_delay( float timeDelay ) { viseme_sound_offset = timeDelay; }
	float get_viseme_sound_delay( ) const	{ return( viseme_sound_offset ); }
	void set_viseme_magnitude( float magnitude ) { viseme_magnitude = magnitude; }
	float get_viseme_magnitude( ) const		{ return( viseme_magnitude ); }

	void setSoftEyes( bool val );
	bool isSoftEyes( ) const { return _soft_eyes_enabled; }
	bool isVisemePlateau( ) const { return _visemePlateau; }
	void setVisemePlateau( bool val ) { _visemePlateau = val; }
	bool isDiphone( ) const { return _diphone; }
	void setDiphone( bool val ) { _diphone = val; }
	float getDiphoneScale( ) const { return _diphoneScale; }
	void setDiphoneScale( float val ) { _diphoneScale = val; }
	bool isDiphoneSplineCurve( ) const { return _diphoneSplineCurve; }
	void setDiphoneSplineCurve( bool val ) { _diphoneSplineCurve = val; }
	float getDiphoneSmoothWindow( ) const { return _diphoneSmoothWindow; }
	void setDiphoneSmoothWindow( float val ) { _diphoneSmoothWindow = val; }
	float getDiphoneSpeedLimit( ) const { return _diphoneSpeedLimit; }
	void setDiphoneSpeedLimit( float val ) {_diphoneSpeedLimit = val; }

	SBAPI SrVec getFacingDirection() ;

	void setMinVisemeTime(float minTime);
	float getMinVisemeTime() const;
	void notify(SBSubject* subject) override;

	virtual SmartBody::SBFaceDefinition* getFaceDefinition();
	virtual void setFaceDefinition(SmartBody::SBFaceDefinition* face);
	virtual void updateFaceDefinition();
	void removeAllFaceChannels();
	void removeAllBlendShapeChannels();

	virtual void setNvbg(SmartBody::Nvbg* nvbg);
	virtual SmartBody::Nvbg* getNvbg();

	virtual void setMiniBrain(std::unique_ptr<SmartBody::MiniBrain> mini);
	virtual SmartBody::MiniBrain* getMiniBrain();
	int writeSkeletonHierarchy(std::string file, double scale);

	virtual void copy(SbmCharacter* origChar);


private:

	bool	_soft_eyes_enabled;
	bool	_visemePlateau;	
	float	_minVisemeTime;
	bool	_diphone;
	float	_diphoneScale;
	bool	_diphoneSplineCurve;
	float	_diphoneSmoothWindow;
	float	_diphoneSpeedLimit;

protected:
	/*!
	 *   Modify skeleton, if necessary.
	 *
	 *   SbmPawn inserts world_offset joint above the existing root.
	 */
	int init_skeleton() override;

	/*!
	 *  Adds a single float channel, bounded by lower and upper limits.
	 */

	// NOTE: called for shader params, bound ( 0, 2 )
	SkJoint* add_bounded_float_channel( const std::string & name, float lower, float upper, int wo_index );


	/*!
	 *   Adds a joint of given name with XPos activated
	 *   for the channel to control an aspect of the face.
	 */

	// NOTE: called for A-units and visemes; bound ( 0, 1 )
	void add_face_channel( const std::string& name, int wo_index );

	
	void writeSkeletonHierarchyRecurse(SkJoint* joint, std::ofstream& ostream, double scale, int indentLevel);
	void indent(int num, std::ofstream& ostream);

	void addVisemeChannel(std::string visemeName, SkMotion* motion);
	void addVisemeChannel(std::string visemeName, std::string motionName);
	void addActionUnitChannel(int auNum, ActionUnit* au);

	void initData();
};

/////////////////////////////////////////////////////////////
#endif // SBM_CHARACTER_HPP
