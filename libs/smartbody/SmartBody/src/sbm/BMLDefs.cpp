#include "sbm/BMLDefs.h"

namespace BML
{

XMLCh* BMLDefs::TAG_BML = nullptr;
XMLCh* BMLDefs::ATTR_ID = nullptr;
XMLCh* BMLDefs::ATTR_TYPE = nullptr;
XMLCh* BMLDefs::ATTR_NAME = nullptr;
XMLCh* BMLDefs::ATTR_LEVEL = nullptr;
XMLCh* BMLDefs::ATTR_HANDLE = nullptr;
XMLCh* BMLDefs::ATTR_LEXEME = nullptr;
XMLCh* BMLDefs::ATTR_FILTERED = nullptr;

XMLCh* BMLDefs::ATTR_START = nullptr;
XMLCh* BMLDefs::ATTR_READY = nullptr;
XMLCh* BMLDefs::ATTR_STROKE_START = nullptr;
XMLCh* BMLDefs::ATTR_STROKE = nullptr;
XMLCh* BMLDefs::ATTR_STROKE_END = nullptr;
XMLCh* BMLDefs::ATTR_RELAX = nullptr;
XMLCh* BMLDefs::ATTR_END = nullptr;

XMLCh* BMLDefs::ATTR_PRESTROKE_HOLD = nullptr;
XMLCh* BMLDefs::ATTR_POSTSTROKE_HOLD = nullptr;
XMLCh* BMLDefs::ATTR_PRESTROKE_HOLD_IDLEMOTION = nullptr;
XMLCh* BMLDefs::ATTR_POSTSTROKE_HOLD_IDLEMOTION = nullptr;
XMLCh* BMLDefs::ATTR_SCALE = nullptr;
XMLCh* BMLDefs::ATTR_FREQUENCY = nullptr;
XMLCh* BMLDefs::ATTR_PRIORITY = nullptr;
XMLCh* BMLDefs::ATTR_GROUP = nullptr;
XMLCh* BMLDefs::ATTR_EMOTION = nullptr;
XMLCh* BMLDefs::ATTR_LAYER = nullptr;

XMLCh* BMLDefs::TM_START = nullptr;
XMLCh* BMLDefs::TM_READY = nullptr;
XMLCh* BMLDefs::TM_STROKE_START = nullptr;
XMLCh* BMLDefs::TM_STROKE = nullptr;
XMLCh* BMLDefs::TM_STROKE_END = nullptr;
XMLCh* BMLDefs::TM_RELAX = nullptr;
XMLCh* BMLDefs::TM_END = nullptr;

XMLCh* BMLDefs::TAG_SBM_ANIMATION = nullptr;
XMLCh* BMLDefs::ATTR_SPEED = nullptr;
XMLCh* BMLDefs::ATTR_PVALUE = nullptr;

XMLCh* BMLDefs::TAG_REF 				= nullptr;
XMLCh* BMLDefs::TAG_CURVE			= nullptr;
XMLCh* BMLDefs::TAG_NUM_KEYS			= nullptr;
XMLCh* BMLDefs::TAG_LIPS				= nullptr;
XMLCh* BMLDefs::TAG_ARTICULATION		= nullptr;
XMLCh* BMLDefs::TAG_SYNC 			= nullptr;
XMLCh* BMLDefs::TAG_TIME 			= nullptr;

XMLCh* BMLDefs::TAG_BODYREACH = nullptr;

XMLCh* BMLDefs::TAG_CONSTRAINT = nullptr;

XMLCh* BMLDefs::TAG_SBM_EVENT = nullptr;
XMLCh* BMLDefs::ATTR_MESSAGE = nullptr;

XMLCh* BMLDefs::TAG_FACE = nullptr;
	
XMLCh* BMLDefs::TAG_GAZE = nullptr;

XMLCh* BMLDefs::TAG_PARAM = nullptr;

XMLCh* BMLDefs::TAG_INTERRUPT = nullptr;

XMLCh* BMLDefs::TAG_LOCOTMOTION = nullptr;

XMLCh* BMLDefs::TAG_QUICKDRAW = nullptr;
XMLCh* BMLDefs::TAG_REACH = nullptr;
XMLCh* BMLDefs::TAG_NOISE = nullptr;

XMLCh* BMLDefs::TAG_SPEECH = nullptr;
XMLCh* BMLDefs::TAG_SBM_SPEECH = nullptr;
XMLCh* BMLDefs::ATTR_TARGET	= nullptr;


	// XMLStrings (utf-16 character arrays) for parsing vrSpeak's XML
XMLCh* BMLDefs::TAG_ACT = nullptr;
XMLCh* BMLDefs::TAG_BODY  = nullptr;
XMLCh* BMLDefs::TAG_GESTURE = nullptr;
XMLCh* BMLDefs::TAG_TORSO  = nullptr;
XMLCh* BMLDefs::TAG_REQUIRED = nullptr;
XMLCh* BMLDefs::TAG_HEAD = nullptr;
XMLCh* BMLDefs::TAG_TM  = nullptr;
XMLCh* BMLDefs::TAG_MARK = nullptr;


XMLCh* BMLDefs::TAG_SBM_COMMAND = nullptr;

	// Deprecated behavior tags
XMLCh* BMLDefs::TAG_ANIMATION = nullptr;
XMLCh* BMLDefs::TAG_EVENT = nullptr;

XMLCh* BMLDefs::TAG_PANIMATION = nullptr;

	// XMLStrings (utf-16 character arrays) for parsing vrSpeak's XML
XMLCh* BMLDefs::ATTR_SPEAKER = nullptr;
XMLCh* BMLDefs::ATTR_ADDRESSEE = nullptr;
XMLCh* BMLDefs::ATTR_CONTENTTYPE = nullptr;
XMLCh* BMLDefs::ATTR_LANG  = nullptr;
XMLCh* BMLDefs::ATTR_TID = nullptr;
XMLCh* BMLDefs::ATTR_POSTURE = nullptr;
XMLCh* BMLDefs::ATTR_REPEATS = nullptr;
XMLCh* BMLDefs::ATTR_AMOUNT  = nullptr;
XMLCh* BMLDefs::ATTR_VELOCITY  = nullptr;
XMLCh* BMLDefs::ATTR_ANGLE  = nullptr;
XMLCh* BMLDefs::ATTR_DIRECTION = nullptr;
XMLCh* BMLDefs::ATTR_ROLL = nullptr;
XMLCh* BMLDefs::ATTR_SMOOTH = nullptr;

	////// XML Direction constants
	// Angular (gaze) and orienting (head)
XMLCh* BMLDefs::DIR_RIGHT = nullptr;
XMLCh* BMLDefs::DIR_LEFT = nullptr;
XMLCh* BMLDefs::DIR_UP = nullptr;
XMLCh* BMLDefs::DIR_DOWN = nullptr;
	// Angular only
XMLCh* BMLDefs::DIR_UPRIGHT = nullptr;
XMLCh* BMLDefs::DIR_UPLEFT = nullptr;
XMLCh* BMLDefs::DIR_DOWNRIGHT = nullptr;
XMLCh* BMLDefs::DIR_DOWNLEFT = nullptr;
XMLCh* BMLDefs::DIR_POLAR = nullptr;
	// Orienting only
XMLCh* BMLDefs::DIR_ROLLRIGHT = nullptr;
XMLCh* BMLDefs::DIR_ROLLLEFT = nullptr;

////// XML Tags
XMLCh* BMLDefs::TAG_DESCRIPTION = nullptr;

////// BML Description Type
XMLCh* BMLDefs::DTYPE_SBM = nullptr;

////// XML ATTRIBUTES
XMLCh* BMLDefs::ATTR_ROOT = nullptr;
XMLCh* BMLDefs::ATTR_ROOTJOINT = nullptr;
XMLCh* BMLDefs::ATTR_EFFECTOR = nullptr;
XMLCh* BMLDefs::ATTR_CONSTRAINT_TYPE = nullptr;
XMLCh* BMLDefs::ATTR_EFFECTOR_ROOT = nullptr;
XMLCh* BMLDefs::ATTR_FADE_OUT = nullptr;
XMLCh* BMLDefs::ATTR_FADE_IN = nullptr;

XMLCh* BMLDefs::ATTR_OFFSET_ROTX = nullptr;
XMLCh* BMLDefs::ATTR_OFFSET_ROTY = nullptr;
XMLCh* BMLDefs::ATTR_OFFSET_ROTZ = nullptr;
//XMLCh* BMLDefs::ATTR_OFFSET_POS[]         = L"offset-pos";

XMLCh* BMLDefs::ATTR_RAMPUP = nullptr;
XMLCh* BMLDefs::ATTR_RAMPDOWN = nullptr;
XMLCh* BMLDefs::ATTR_DURATION = nullptr;

XMLCh* BMLDefs::ATTR_X = nullptr;
XMLCh* BMLDefs::ATTR_Y = nullptr;
XMLCh* BMLDefs::ATTR_Z = nullptr;

XMLCh* BMLDefs::ATTR_POSX = nullptr;
XMLCh* BMLDefs::ATTR_POSY = nullptr;
XMLCh* BMLDefs::ATTR_POSZ = nullptr;

XMLCh* BMLDefs::ATTR_ROTX = nullptr;
XMLCh* BMLDefs::ATTR_ROTY = nullptr;
XMLCh* BMLDefs::ATTR_ROTZ = nullptr;

////// XML ATTRIBUTES
XMLCh* BMLDefs::ATTR_TARGET_POS = nullptr;
XMLCh* BMLDefs::ATTR_REACH_VELOCITY = nullptr;
XMLCh* BMLDefs::ATTR_REACH_FINISH = nullptr;
XMLCh* BMLDefs::ATTR_REACH_TYPE = nullptr;
//XMLCh* BMLDefs::ATTR_APEX_DURATION[] = L"sbm:apex-duration";

XMLCh* BMLDefs::ATTR_ROTATION = nullptr;
XMLCh* BMLDefs::ATTR_ENABLE = nullptr;

XMLCh* BMLDefs::TAG_VISEME = nullptr;
XMLCh* BMLDefs::TAG_SOUND  = nullptr;
XMLCh* BMLDefs::ATTR_AU = nullptr;
XMLCh* BMLDefs::ATTR_SIDE = nullptr;

////// XML ATTRIBUTES
XMLCh* BMLDefs::ATTR_SBM_ROLL = nullptr;
XMLCh* BMLDefs::ATTR_JOINT_RANGE = nullptr;
XMLCh* BMLDefs::ATTR_JOINT_SPEED = nullptr;
XMLCh* BMLDefs::ATTR_TIME_HINT = nullptr;
XMLCh* BMLDefs::ATTR_JOINT_SMOOTH = nullptr;
XMLCh* BMLDefs::ATTR_PITCH = nullptr;
XMLCh* BMLDefs::ATTR_HEADING = nullptr;
XMLCh* BMLDefs::ATTR_BLEND = nullptr;
XMLCh* BMLDefs::ATTR_INTERPOLATE_BIAS = nullptr;

XMLCh* BMLDefs::ATTR_PRIORITY_JOINT = nullptr;
XMLCh* BMLDefs::ATTR_PITCH_MIN = nullptr;
XMLCh* BMLDefs::ATTR_PITCH_MAX = nullptr;

XMLCh* BMLDefs::VALUE_TEXT_PLAIN = nullptr;
XMLCh* BMLDefs::VALUE_SSML = nullptr;

XMLCh* BMLDefs::start_id = nullptr;
XMLCh* BMLDefs::end_id   = nullptr;

XMLCh* BMLDefs::ATTR_REACH_ARM = nullptr;
XMLCh* BMLDefs::ATTR_USE_EXAMPLE = nullptr;
XMLCh* BMLDefs::ATTR_BUILD_EXAMPLE = nullptr;
XMLCh* BMLDefs::ATTR_EXAMPLE_DIST = nullptr;
XMLCh* BMLDefs::ATTR_RESAMPLE_SIZE = nullptr;

XMLCh* BMLDefs::ATTR_ANIM = nullptr;
XMLCh* BMLDefs::ATTR_TRACK_DUR = nullptr;
XMLCh* BMLDefs::ATTR_GUNDRAW_DUR = nullptr;
XMLCh* BMLDefs::ATTR_HOLSTER_DUR = nullptr;
XMLCh* BMLDefs::ATTR_AIM_OFFSET = nullptr;


XMLCh* BMLDefs::ATTR_ANIM1 = nullptr;
XMLCh* BMLDefs::ATTR_ANIM2 = nullptr;
XMLCh* BMLDefs::ATTR_LOOP = nullptr;
XMLCh* BMLDefs::ATTR_WRAPMODE = nullptr;
XMLCh* BMLDefs::ATTR_SCHEDULEMODE = nullptr;
XMLCh* BMLDefs::ATTR_BLENDMODE = nullptr;
XMLCh* BMLDefs::ATTR_TRANSITION_LENGTH = nullptr;
XMLCh* BMLDefs::ATTR_DIRECTPLAY = nullptr;
XMLCh* BMLDefs::ATTR_OVERLAY = nullptr;


////// XML ATTRIBUTES
XMLCh* BMLDefs::ATTR_VALUE = nullptr;

XMLCh* BMLDefs::ATTR_TWARP = nullptr;

XMLCh* BMLDefs::ATTR_TRUE = nullptr;
XMLCh* BMLDefs::ATTR_FALSE = nullptr;
XMLCh* BMLDefs::ATTR_RESET = nullptr;

XMLCh* BMLDefs::ATTR_SHAKE  = nullptr;
XMLCh* BMLDefs::ATTR_TOSS   = nullptr;
XMLCh* BMLDefs::ATTR_ORIENT   = nullptr;
XMLCh* BMLDefs::ATTR_NOD   = nullptr;


XMLCh* BMLDefs::ATTR_ARMLEFT = nullptr;
XMLCh* BMLDefs::ATTR_ARMRIGHT = nullptr;
XMLCh* BMLDefs::ATTR_FACS = nullptr;

XMLCh* BMLDefs::ATTR_RPS = nullptr;
XMLCh* BMLDefs::ATTR_GRPS = nullptr;
XMLCh* BMLDefs::ATTR_LRPS = nullptr;

XMLCh* BMLDefs::ATTR_EYEBROWS = nullptr;
XMLCh* BMLDefs::ATTR_EYELIDS = nullptr;
XMLCh* BMLDefs::ATTR_MOUTH = nullptr;

XMLCh* BMLDefs::TAG_GRAB = nullptr;
XMLCh* BMLDefs::ATTR_CONS_JOINT = nullptr;
XMLCh* BMLDefs::ATTR_CONS_TARGET = nullptr;

XMLCh* BMLDefs::ATTR_WRIST = nullptr;
XMLCh* BMLDefs::ATTR_GRAB_SPEED = nullptr;
XMLCh* BMLDefs::ATTR_GRAB_FINISH = nullptr;
XMLCh* BMLDefs::ATTR_GRAB_TYPE = nullptr;
XMLCh* BMLDefs::ATTR_APEX_DURATION = nullptr;
XMLCh* BMLDefs::ATTR_OBSTACLE = nullptr;
XMLCh* BMLDefs::ATTR_GRAB_STATE = nullptr;
XMLCh* BMLDefs::ATTR_REACH_ACTION = nullptr;
XMLCh* BMLDefs::ATTR_REACH_DURATION = nullptr;

XMLCh* BMLDefs::ATTR_PROXIMITY = nullptr;
XMLCh* BMLDefs::ATTR_MANNER = nullptr;
XMLCh* BMLDefs::ATTR_FACING = nullptr;
XMLCh* BMLDefs::ATTR_FOLLOW = nullptr;
XMLCh* BMLDefs::ATTR_NUM_STEPS = nullptr;
XMLCh* BMLDefs::ATTR_SPD = nullptr;
XMLCh* BMLDefs::ATTR_SOURCE_JOINT = nullptr;
XMLCh* BMLDefs::ATTR_ATTACH_PAWN = nullptr;
XMLCh* BMLDefs::ATTR_RELEASE_PAWN = nullptr;
XMLCh* BMLDefs::ATTR_FOOT_IK = nullptr;
XMLCh* BMLDefs::ATTR_USE_LOCOMOTION = nullptr;
XMLCh* BMLDefs::TAG_SACCADE = nullptr;
XMLCh* BMLDefs::ATTR_MAGNITUDE = nullptr;
XMLCh* BMLDefs::ATTR_MODE = nullptr;
XMLCh* BMLDefs::ATTR_ANGLELIMIT = nullptr;
XMLCh* BMLDefs::ATTR_FINISH = nullptr;
XMLCh* BMLDefs::ATTR_STEERACCEL = nullptr;
XMLCh* BMLDefs::ATTR_STEERANGLEACCEL = nullptr;
XMLCh* BMLDefs::ATTR_STEERSCOOTACCEL = nullptr;
XMLCh* BMLDefs::ATTR_NODAXIS = nullptr;
XMLCh* BMLDefs::ATTR_NODACCEL = nullptr;
XMLCh* BMLDefs::ATTR_NODPERIOD = nullptr;
XMLCh* BMLDefs::ATTR_NODWARP = nullptr;
XMLCh* BMLDefs::ATTR_NODPITCH = nullptr;
XMLCh* BMLDefs::ATTR_NODDECAY = nullptr;
XMLCh* BMLDefs::ATTR_WIGGLE = nullptr;
XMLCh* BMLDefs::ATTR_WAGGLE = nullptr;
XMLCh* BMLDefs::ATTR_PARAMETERIZED = nullptr;

XMLCh* BMLDefs::TAG_STATES = nullptr;
XMLCh* BMLDefs::TAG_BLEND = nullptr;
XMLCh* BMLDefs::ATTR_STARTINGNOW = nullptr;
XMLCh* BMLDefs::ATTR_ADDITIVE = nullptr;
XMLCh* BMLDefs::ATTR_PARTIALJOINT = nullptr;

XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN0 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN45 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN90 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN135 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN180 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN225 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN270 = nullptr;
XMLCh* BMLDefs::ATTR_PERCENTAGE_BIN315 = nullptr;
XMLCh* BMLDefs::ATTR_SACCADE_INTERVAL_MEAN = nullptr;
XMLCh* BMLDefs::ATTR_SACCADE_INTERVAL_VARIANT = nullptr;

// OpenCollada
XMLCh* BMLDefs::ATTR_SID = nullptr;
XMLCh* BMLDefs::ATTR_STRIDE = nullptr;
XMLCh* BMLDefs::ATTR_COUNT = nullptr;
XMLCh* BMLDefs::ATTR_INPUTSTR = nullptr;
XMLCh* BMLDefs::ATTR_SEMANTIC = nullptr;
XMLCh* BMLDefs::ATTR_VCOUNT = nullptr;
XMLCh* BMLDefs::ATTR_SOURCE = nullptr;
XMLCh* BMLDefs::ATTR_URL = nullptr;
XMLCh* BMLDefs::ATTR_OFFSET = nullptr;
XMLCh* BMLDefs::ATTR_TRIM = nullptr;
XMLCh* BMLDefs::ATTR_SYMBOL = nullptr;
XMLCh* BMLDefs::ATTR_MATERIAL = nullptr;
XMLCh* BMLDefs::ATTR_OPAQUE = nullptr;
XMLCh* BMLDefs::ATTR_TEXTURE = nullptr;



// locomotion
XMLCh* BMLDefs::ATTR_BRAKING = nullptr;

XMLCh* BMLDefs::ATTR_STYLE = nullptr;

XMLCh* BMLDefs::ATTR_STATENAME = nullptr;

XMLCh* BMLDefs::ATTR_POLICY = nullptr;
XMLCh* BMLDefs::ATTR_PROCID = nullptr;

XMLCh* BMLDefs::OGRE_BONE = nullptr;
XMLCh* BMLDefs::OGRE_PARENT = nullptr;
XMLCh* BMLDefs::OGRE_ID = nullptr;
XMLCh* BMLDefs::OGRE_VERTEX_COUNT = nullptr;
XMLCh* BMLDefs::OGRE_NAME = nullptr;
XMLCh* BMLDefs::OGRE_MATERIAL = nullptr;
XMLCh* BMLDefs::OGRE_LENGTH = nullptr;
XMLCh* BMLDefs::OGRE_U = nullptr;
XMLCh* BMLDefs::OGRE_V = nullptr;
XMLCh* BMLDefs::OGRE_V1 = nullptr;
XMLCh* BMLDefs::OGRE_V2 = nullptr;
XMLCh* BMLDefs::OGRE_V3 = nullptr;
XMLCh* BMLDefs::OGRE_T1 = nullptr;
XMLCh* BMLDefs::OGRE_T2 = nullptr;
XMLCh* BMLDefs::OGRE_T3 = nullptr;
XMLCh* BMLDefs::OGRE_X = nullptr;
XMLCh* BMLDefs::OGRE_Y = nullptr;
XMLCh* BMLDefs::OGRE_Z = nullptr;
XMLCh* BMLDefs::OGRE_ANGLE = nullptr;
XMLCh* BMLDefs::OGRE_AXIS = nullptr;
XMLCh* BMLDefs::OGRE_ANIMATION = nullptr;
XMLCh* BMLDefs::OGRE_TRACKS = nullptr;
XMLCh* BMLDefs::OGRE_TRACK = nullptr;
XMLCh* BMLDefs::OGRE_KEYFRAMES = nullptr;
XMLCh* BMLDefs::OGRE_KEYFRAME = nullptr;
XMLCh* BMLDefs::OGRE_TRANSLATE = nullptr;
XMLCh* BMLDefs::OGRE_ROTATE = nullptr;
XMLCh* BMLDefs::OGRE_TIME = nullptr;
XMLCh* BMLDefs::OGRE_VERTEX_INDEX = nullptr;
XMLCh* BMLDefs::OGRE_BONE_INDEX = nullptr;
XMLCh* BMLDefs::OGRE_WEIGHT = nullptr;

std::list<XMLPtr> BMLDefs::sAttributes;

void BMLDefs::shutdown() {
	sAttributes.clear();
}

BMLDefs::BMLDefs()
{

	auto add = [](const std::string& attr){
		XMLPtr ptr(XMLString::transcode(attr.c_str()));
		sAttributes.emplace_back(std::move(ptr));
		return sAttributes.back().get();
	};
	
	ATTR_ID = add("id");
	ATTR_TYPE = add("type");
	ATTR_NAME = add("name");
	ATTR_LEVEL = add("level");
	ATTR_HANDLE = add("sbm:handle");
	ATTR_LEXEME = add("lexeme");
	ATTR_FILTERED = add("filtered");

	ATTR_START = add("start");
	ATTR_READY = add("ready");
	ATTR_STROKE_START = add("stroke_start");
	ATTR_STROKE = add("stroke");
	ATTR_STROKE_END = add("stroke_end");
	ATTR_RELAX = add("relax");
	ATTR_END = add("end");

	ATTR_PRESTROKE_HOLD = add("prestroke_hold");
	ATTR_PRESTROKE_HOLD_IDLEMOTION = add("prestroke_hold_idlemotion");
	ATTR_POSTSTROKE_HOLD = add("poststroke_hold");
	ATTR_POSTSTROKE_HOLD_IDLEMOTION = add("poststroke_hold_idlemotion");
	ATTR_SCALE = add("sbm:scale");
	ATTR_FREQUENCY = add("sbm:frequency");
	ATTR_PRIORITY = add("priority");
	ATTR_GROUP = add("group");
	ATTR_EMOTION = add("emotion");
	ATTR_LAYER = add("layer");

	TM_START = add("start");
	TM_READY = add("ready");
	TM_STROKE_START = add("stroke_start");
	TM_STROKE = add("stroke");
	TM_STROKE_END = add("stroke_end");
	TM_RELAX = add("relax");
	TM_END = add("end");

	TAG_SBM_ANIMATION		 = add("sbm:animation");
	ATTR_TWARP       		 = add("ME:twarp");
	ATTR_PVALUE				 = add("sbm:value");

	TAG_REF 				 = add("ref");
	TAG_CURVE				 = add("curve");
	TAG_NUM_KEYS			 = add("num_keys");
	TAG_LIPS				 = add("lips");
	TAG_ARTICULATION		 = add("articulation");
	TAG_SYNC				 = add("sync");
	TAG_TIME				 = add("time");

	TAG_BODYREACH			 = add("sbm:reach");

	TAG_CONSTRAINT			 = add("sbm:constraint");

	//TAG_SBM_EVENT			 = add("sbm:event");
	//ATTR_MESSAGE			 = add("message");

	TAG_FACE				 = add("face");
	
	TAG_GAZE				 = add("gaze");

	TAG_PARAM				 = add("param");

	TAG_INTERRUPT			 = add("sbm:interrupt");

	TAG_LOCOTMOTION			 = add("locomotion");

	TAG_QUICKDRAW			 = add("sbm:quickdraw");

	TAG_SACCADE				 = add("saccade");
	TAG_REACH				 = add("sbm:reach");
	TAG_NOISE    			 = add("sbm:noise");

	TAG_SPEECH				 = add("speech");     // Original tag, here for backward compatibility
	TAG_SBM_SPEECH			 = add("sbm:speech");
	ATTR_TARGET				 = add("target");

	TAG_SBM_EVENT			 = add("sbm:event");
	ATTR_MESSAGE			 = add("message");

	// XMLStrings (utf-16 character arrays) for parsing vrSpeak's XML
	TAG_ACT		 = add("act");
	TAG_BML       = add("bml");
	TAG_BODY      = add("body");
	TAG_TORSO      = add("torso");
	TAG_REQUIRED  = add("required");
	TAG_GESTURE = add("gesture");
	TAG_HEAD      = add("head");
	TAG_TM        = add("tm");
	TAG_MARK      = add("mark");


	TAG_SBM_COMMAND = add("sbm:command");

	// Deprecated behavior tags
	TAG_ANIMATION = add("animation");
	TAG_EVENT     = add("event");

	TAG_PANIMATION = add("panimation");
	//TAG_REACH = add("sbm:reach_old");

	// XMLStrings (utf-16 character arrays) for parsing vrSpeak's XML
	ATTR_SPEAKER      = add("speaker");
	ATTR_ADDRESSEE    = add("addressee");
	ATTR_CONTENTTYPE  = add("contenttype");
	ATTR_LANG         = add("lang");
	ATTR_TID          = add("tid");
	ATTR_POSTURE      = add("posture");
	ATTR_REPEATS      = add("repeats");
	ATTR_AMOUNT       = add("amount");
	ATTR_VELOCITY     = add("velocity");
	ATTR_ANGLE        = add("angle");
	ATTR_DIRECTION    = add("direction");
	//ATTR_ROLL         = add("sbm:roll");
	ATTR_SMOOTH       = add("sbm:smooth");

	////// XML Direction constants
	// Angular (gaze) and orienting (head)
	DIR_RIGHT        = add("RIGHT");
	DIR_LEFT         = add("LEFT");
	DIR_UP           = add("UP");
	DIR_DOWN         = add("DOWN");
	// Angular only
	DIR_UPRIGHT      = add("UPRIGHT");
	DIR_UPLEFT       = add("UPLEFT");
	DIR_DOWNRIGHT    = add("DOWNRIGHT");
	DIR_DOWNLEFT     = add("DOWNLEFT");
	DIR_POLAR        = add("POLAR");
	// Orienting only
	DIR_ROLLRIGHT    = add("ROLLRIGHT");
	DIR_ROLLLEFT     = add("ROLLLEFT");


	////// XML Tags
	TAG_DESCRIPTION  = add("description");

	////// BML Description Type
	DTYPE_SBM   = add("ICT.SBM");

	////// XML ATTRIBUTES
	ATTR_ROOTJOINT  = add("sbm:root-joint");
	ATTR_EFFECTOR  = add("effector");
	ATTR_CONSTRAINT_TYPE  = add("sbm:constraint-type");
	ATTR_EFFECTOR_ROOT  = add("sbm:effector-root");
	ATTR_FADE_OUT		 = add("sbm:fade-out");
	ATTR_FADE_IN		 = add("sbm:fade-in");

	ATTR_OFFSET_ROTX         = add("rot-x");
	ATTR_OFFSET_ROTY       = add("rot-y");
	ATTR_OFFSET_ROTZ          = add("rot-z");
	//ATTR_OFFSET_POS          = add("offset-pos";
	
	ATTR_RAMPUP 	 = add("sbm:rampup");
	ATTR_RAMPDOWN	 = add("sbm:rampdown");
	ATTR_DURATION	 = add("sbm:duration");

	//ATTR_X      = add("pos-x");
	//ATTR_Y      = add("pos-y");
	//ATTR_Z      = add("pos-z");

	////// XML ATTRIBUTES
	ATTR_TARGET_POS  = add("sbm:target-pos");
	ATTR_REACH_VELOCITY  = add("sbm:reach-velocity");
	ATTR_REACH_DURATION  = add("sbm:reach-duration");
	ATTR_REACH_FINISH  = add("sbm:reach-finish");
	ATTR_REACH_TYPE  = add("sbm:reach-type");
	//ATTR_APEX_DURATION  = add("sbm:apex-duration";

	ATTR_ROTATION      = add("rotation");
	ATTR_ENABLE      = add("enable");
	ATTR_SPEED      = add("speed");

	ATTR_X      = add("x");
	ATTR_Y      = add("y");
	ATTR_Z      = add("z");

	ATTR_POSX      = add("pos-x");
	ATTR_POSY      = add("pos-y");
	ATTR_POSZ      = add("pos-z");

	ATTR_ROTX      = add("rot-x");
	ATTR_ROTY      = add("rot-y");
	ATTR_ROTZ      = add("rot-z");


	TAG_VISEME  = add("viseme");
	TAG_SOUND  = add("soundFile"); //this tag is used to rename the soundFile by Remote speech process
	ATTR_AU      = add("au");
	ATTR_SIDE    = add("side");	

	////// XML ATTRIBUTES
	ATTR_SBM_ROLL      = add("sbm:roll");
	ATTR_JOINT_RANGE   = add("sbm:joint-range");
	ATTR_JOINT_SPEED   = add("sbm:joint-speed");
	ATTR_TIME_HINT	 = add("sbm:time-hint");
	ATTR_JOINT_SMOOTH  = add("sbm:speed-smoothing");
	ATTR_PITCH         = add("pitch");
	ATTR_HEADING       = add("heading");
	ATTR_ROLL          = add("roll");
	ATTR_BLEND         = add("blend");
	ATTR_INTERPOLATE_BIAS  = add("interpolate-bias");

	ATTR_PRIORITY_JOINT  = add("sbm:priority-joint");
	ATTR_PITCH_MIN	 = add("pitch-min");
	ATTR_PITCH_MAX	 = add("pitch-max");

//	TAG_MARK       = add("mark");

	VALUE_TEXT_PLAIN  = add("text/plain");
	VALUE_SSML        = add("application/ssml+xml");

	start_id  = add("bml:start");
	end_id    = add("bml:end");

	////// XML ATTRIBUTES
	ATTR_REACH_ARM  = add("reach-arm");
	ATTR_USE_EXAMPLE  = add("use-example");
	ATTR_BUILD_EXAMPLE  = add("build-example");
	ATTR_EXAMPLE_DIST  = add("example-dist"); // minimal distances between pose examples
	ATTR_RESAMPLE_SIZE  = add("resample-size"); // minimal distances between pose examples
	////// XML ATTRIBUTES
	ATTR_ANIM          = add("anim");
	ATTR_TRACK_DUR     = add("track-duration");
	ATTR_GUNDRAW_DUR    = add("gundraw-duration");
	ATTR_HOLSTER_DUR    = add("holster-duration");
	ATTR_AIM_OFFSET     = add("aim-offset");


	ATTR_ANIM1  = add("anim1");
	ATTR_ANIM2  = add("anim2");
	ATTR_LOOP  = add("loop");
	ATTR_WRAPMODE = add("sbm:wrap-mode");
	ATTR_BLENDMODE = add("sbm:blend-mode");
	ATTR_SCHEDULEMODE = add("sbm:schedule-mode");
	ATTR_TRANSITION_LENGTH = add("sbm:transition-length");
	ATTR_DIRECTPLAY = add("sbm:direct-play");
	ATTR_OVERLAY = add("overlay");

	////// XML ATTRIBUTES
	ATTR_VALUE   = add("value");

	ATTR_TRUE   = add("true");
	ATTR_FALSE   = add("false");
	ATTR_RESET   = add("reset");

	ATTR_SHAKE  = add("shake");
	ATTR_TOSS   = add("toss");
	ATTR_ORIENT   = add("orient");
	ATTR_NOD   = add("nod");
	ATTR_WIGGLE   = add("wiggle");
	ATTR_WAGGLE   = add("waggle");
	ATTR_PARAMETERIZED = add("parameterized");

	ATTR_ARMLEFT   = add("left");
	ATTR_ARMRIGHT   = add("right");

	ATTR_FACS   = add("facs");
	ATTR_RPS = add("rps");
	ATTR_GRPS = add("grps");
	ATTR_LRPS = add("lrps");

	ATTR_EYEBROWS = add("eyebrows");
	ATTR_EYELIDS = add("eyelids");
	ATTR_MOUTH = add("mouth");

	ATTR_ROOT = add("sbm:root");

	TAG_GRAB = add("sbm:grab");
	//ATTR_REACH_FINISH = add("sbm:reach-finish");
	ATTR_CONS_JOINT = add("sbm:cons-joint");
	ATTR_CONS_TARGET = add("sbm:cons-target");
	ATTR_APEX_DURATION = add("sbm:apex-duration");
	ATTR_WRIST = add("sbm:wrist");
	ATTR_GRAB_SPEED = add("sbm:grab-speed");
	ATTR_GRAB_FINISH = add("sbm:grab-finish");
	ATTR_GRAB_TYPE = add("sbm:grab-type");
	ATTR_OBSTACLE = add("sbm:obstacle");
	ATTR_GRAB_STATE = add("sbm:grab-state");
	ATTR_REACH_ACTION = add("sbm:action");
	ATTR_PROXIMITY = add("proximity");
	ATTR_MANNER = add("manner");
	ATTR_FACING = add("facing");
	ATTR_FOLLOW = add("sbm:follow");
	ATTR_NUM_STEPS = add("sbm:numsteps");
	ATTR_SPD = add("spd");
	ATTR_SOURCE_JOINT = add("sbm:source-joint");
	ATTR_ATTACH_PAWN = add("sbm:attach-pawn");
	ATTR_RELEASE_PAWN = add("sbm:release-pawn");
	ATTR_FOOT_IK = add("sbm:foot-ik");
	ATTR_USE_LOCOMOTION = add("sbm:use-locomotion");

	ATTR_MAGNITUDE = add("magnitude");
	ATTR_MODE = add("mode");
	ATTR_ANGLELIMIT = add("angle-limit");
	ATTR_FINISH = add("finish");

	ATTR_STEERACCEL = add("sbm:accel");
	ATTR_STEERANGLEACCEL = add("sbm:angleaccel");
	ATTR_STEERSCOOTACCEL = add("sbm:scootaccel");

	ATTR_NODAXIS = add("sbm:axis");
	ATTR_NODACCEL = add("sbm:accel");
	ATTR_NODPERIOD = add("sbm:period");
	ATTR_NODWARP = add("sbm:warp");
	ATTR_NODPITCH = add("sbm:pitch");
	ATTR_NODDECAY = add("sbm:decay");

	TAG_STATES = add("sbm:states");
	TAG_BLEND = add("blend");
	ATTR_STARTINGNOW = add("sbm:startnow");
	ATTR_ADDITIVE = add("sbm:additive");
	ATTR_PARTIALJOINT = add("sbm:partial-joint");

	ATTR_PERCENTAGE_BIN0 = add("sbm:bin0");
	ATTR_PERCENTAGE_BIN45 = add("sbm:bin45");
	ATTR_PERCENTAGE_BIN90 = add("sbm:bin90");
	ATTR_PERCENTAGE_BIN135 = add("sbm:bin135");
	ATTR_PERCENTAGE_BIN180 = add("sbm:bin180");
	ATTR_PERCENTAGE_BIN225 = add("sbm:bin225");
	ATTR_PERCENTAGE_BIN270 = add("sbm:bin270");
	ATTR_PERCENTAGE_BIN315 = add("sbm:bin315");
	ATTR_SACCADE_INTERVAL_MEAN = add("sbm:mean");
	ATTR_SACCADE_INTERVAL_VARIANT = add("sbm:variant");

	// OpenCollada
	ATTR_SID = add("sid");
	ATTR_STRIDE = add("stride");
	ATTR_COUNT = add("count");
	ATTR_INPUTSTR = add("input");
	ATTR_SEMANTIC = add("semantic");
	ATTR_VCOUNT = add("vcount");
	ATTR_SOURCE = add("source");
	ATTR_URL = add("url");
	ATTR_OFFSET = add("offset");
	ATTR_TRIM = add("trim");
	ATTR_SYMBOL = add("symbol");
	ATTR_MATERIAL = add("material");
	ATTR_OPAQUE = add("opaque");
	ATTR_TEXTURE = add("texture");

	// locomotion
	ATTR_BRAKING = add("sbm:braking");

	ATTR_STYLE = add("sbm:style");

	ATTR_STATENAME = add("sbm:state-name");

	ATTR_POLICY = add("policy");
	ATTR_PROCID = add("procid");

	OGRE_BONE = add("bone");
	OGRE_PARENT = add("parent");
	OGRE_ID = add("id");
	OGRE_VERTEX_COUNT = add("vertexcount");
	OGRE_NAME = add("name");
	OGRE_MATERIAL = add("material");
	OGRE_LENGTH = add("length");
	OGRE_X = add("x");
	OGRE_Y = add("y");
	OGRE_Z = add("z");
	OGRE_U = add("u");
	OGRE_V = add("v");
	OGRE_V1 = add("v1");
	OGRE_V2 = add("v2");
	OGRE_V3 = add("v3");
	OGRE_T1 = add("t1");
	OGRE_T2 = add("t2");
	OGRE_T3 = add("t3");
	OGRE_ANGLE = add("angle");
	OGRE_AXIS = add("axis");
	OGRE_ANIMATION = add("animation");
	OGRE_TRACKS = add("tracks");
	OGRE_TRACK = add("track");
	OGRE_KEYFRAMES = add("keyframes");
	OGRE_KEYFRAME = add("keyframe");
	OGRE_TRANSLATE = add("translate");
	OGRE_ROTATE = add("rotate");
	OGRE_TIME = add("time");
	OGRE_VERTEX_INDEX = add("vertexindex");
	OGRE_BONE_INDEX = add("boneindex");
	OGRE_WEIGHT = add("weight");

}

BMLDefs::~BMLDefs() = default;

}
