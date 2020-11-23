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

XMLCh*  BMLDefs::start_id = nullptr;
XMLCh*  BMLDefs::end_id   = nullptr;

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


BMLDefs::BMLDefs()
{
	ATTR_ID = XMLString::transcode("id");
	ATTR_TYPE = XMLString::transcode("type");
	ATTR_NAME = XMLString::transcode("name");
	ATTR_LEVEL = XMLString::transcode("level");
	ATTR_HANDLE = XMLString::transcode("sbm:handle");
	ATTR_LEXEME = XMLString::transcode("lexeme");
	ATTR_FILTERED = XMLString::transcode("filtered");

	ATTR_START = XMLString::transcode("start");
	ATTR_READY = XMLString::transcode("ready");
	ATTR_STROKE_START = XMLString::transcode("stroke_start");
	ATTR_STROKE = XMLString::transcode("stroke");
	ATTR_STROKE_END = XMLString::transcode("stroke_end");
	ATTR_RELAX = XMLString::transcode("relax");
	ATTR_END = XMLString::transcode("end");

	ATTR_PRESTROKE_HOLD =  XMLString::transcode("prestroke_hold");
	ATTR_PRESTROKE_HOLD_IDLEMOTION = XMLString::transcode("prestroke_hold_idlemotion");
	ATTR_POSTSTROKE_HOLD =  XMLString::transcode("poststroke_hold");
	ATTR_POSTSTROKE_HOLD_IDLEMOTION = XMLString::transcode("poststroke_hold_idlemotion");
	ATTR_SCALE = XMLString::transcode("sbm:scale");
	ATTR_FREQUENCY = XMLString::transcode("sbm:frequency");
	ATTR_PRIORITY = XMLString::transcode("priority");
	ATTR_GROUP = XMLString::transcode("group");
	ATTR_EMOTION = XMLString::transcode("emotion");
	ATTR_LAYER = XMLString::transcode("layer");

	TM_START = XMLString::transcode("start");
	TM_READY = XMLString::transcode("ready");
	TM_STROKE_START = XMLString::transcode("stroke_start");
	TM_STROKE = XMLString::transcode("stroke");
	TM_STROKE_END = XMLString::transcode("stroke_end");
	TM_RELAX = XMLString::transcode("relax");
	TM_END = XMLString::transcode("end");

	TAG_SBM_ANIMATION		= XMLString::transcode("sbm:animation");
	ATTR_TWARP       		= XMLString::transcode("ME:twarp");
	ATTR_PVALUE				= XMLString::transcode("sbm:value");

	TAG_REF 				= XMLString::transcode("ref");
	TAG_CURVE				= XMLString::transcode("curve");
	TAG_NUM_KEYS			= XMLString::transcode("num_keys");
	TAG_LIPS				= XMLString::transcode("lips");
	TAG_ARTICULATION		= XMLString::transcode("articulation");
	TAG_SYNC				= XMLString::transcode("sync");
	TAG_TIME				= XMLString::transcode("time");

	TAG_BODYREACH			= XMLString::transcode("sbm:reach");

	TAG_CONSTRAINT			= XMLString::transcode("sbm:constraint");

	//TAG_SBM_EVENT			= XMLString::transcode("sbm:event");
	//ATTR_MESSAGE			= XMLString::transcode("message");

	TAG_FACE				= XMLString::transcode("face");
	
	TAG_GAZE				= XMLString::transcode("gaze");

	TAG_PARAM				= XMLString::transcode("param");

	TAG_INTERRUPT			= XMLString::transcode("sbm:interrupt");

	TAG_LOCOTMOTION			= XMLString::transcode("locomotion");

	TAG_QUICKDRAW			= XMLString::transcode("sbm:quickdraw");

	TAG_SACCADE				= XMLString::transcode("saccade");
	TAG_REACH				= XMLString::transcode("sbm:reach");
	TAG_NOISE    			= XMLString::transcode("sbm:noise");

	TAG_SPEECH				= XMLString::transcode("speech");     // Original tag, here for backward compatibility
	TAG_SBM_SPEECH			= XMLString::transcode("sbm:speech");
	ATTR_TARGET				= XMLString::transcode("target");

	TAG_SBM_EVENT			= XMLString::transcode("sbm:event");
	ATTR_MESSAGE			= XMLString::transcode("message");

	// XMLStrings (utf-16 character arrays) for parsing vrSpeak's XML
	TAG_ACT		= XMLString::transcode("act");
	TAG_BML       = XMLString::transcode("bml");
	TAG_BODY      = XMLString::transcode("body");
	TAG_TORSO      = XMLString::transcode("torso");
	TAG_REQUIRED  = XMLString::transcode("required");
	TAG_GESTURE = XMLString::transcode("gesture");
	TAG_HEAD      = XMLString::transcode("head");
	TAG_TM        = XMLString::transcode("tm");
	TAG_MARK      = XMLString::transcode("mark");


	TAG_SBM_COMMAND = XMLString::transcode("sbm:command");

	// Deprecated behavior tags
	TAG_ANIMATION = XMLString::transcode("animation");
	TAG_EVENT     = XMLString::transcode("event");

	TAG_PANIMATION = XMLString::transcode("panimation");
	//TAG_REACH = XMLString::transcode("sbm:reach_old");

	// XMLStrings (utf-16 character arrays) for parsing vrSpeak's XML
	ATTR_SPEAKER      = XMLString::transcode("speaker");
	ATTR_ADDRESSEE    = XMLString::transcode("addressee");
	ATTR_CONTENTTYPE  = XMLString::transcode("contenttype");
	ATTR_LANG         = XMLString::transcode("lang");
	ATTR_TID          = XMLString::transcode("tid");
	ATTR_POSTURE      = XMLString::transcode("posture");
	ATTR_REPEATS      = XMLString::transcode("repeats");
	ATTR_AMOUNT       = XMLString::transcode("amount");
	ATTR_VELOCITY     = XMLString::transcode("velocity");
	ATTR_ANGLE        = XMLString::transcode("angle");
	ATTR_DIRECTION    = XMLString::transcode("direction");
	//ATTR_ROLL         = XMLString::transcode("sbm:roll");
	ATTR_SMOOTH       = XMLString::transcode("sbm:smooth");

	////// XML Direction constants
	// Angular (gaze) and orienting (head)
	DIR_RIGHT        = XMLString::transcode("RIGHT");
	DIR_LEFT         = XMLString::transcode("LEFT");
	DIR_UP           = XMLString::transcode("UP");
	DIR_DOWN         = XMLString::transcode("DOWN");
	// Angular only
	DIR_UPRIGHT      = XMLString::transcode("UPRIGHT");
	DIR_UPLEFT       = XMLString::transcode("UPLEFT");
	DIR_DOWNRIGHT    = XMLString::transcode("DOWNRIGHT");
	DIR_DOWNLEFT     = XMLString::transcode("DOWNLEFT");
	DIR_POLAR        = XMLString::transcode("POLAR");
	// Orienting only
	DIR_ROLLRIGHT    = XMLString::transcode("ROLLRIGHT");
	DIR_ROLLLEFT     = XMLString::transcode("ROLLLEFT");


	////// XML Tags
	TAG_DESCRIPTION  = XMLString::transcode("description");

	////// BML Description Type
	DTYPE_SBM   = XMLString::transcode("ICT.SBM");

	////// XML ATTRIBUTES
	ATTR_ROOTJOINT  = XMLString::transcode("sbm:root-joint");
	ATTR_EFFECTOR  = XMLString::transcode("effector");
	ATTR_CONSTRAINT_TYPE  = XMLString::transcode("sbm:constraint-type");
	ATTR_EFFECTOR_ROOT  = XMLString::transcode("sbm:effector-root");
	ATTR_FADE_OUT		 = XMLString::transcode("sbm:fade-out");
	ATTR_FADE_IN		 = XMLString::transcode("sbm:fade-in");

	ATTR_OFFSET_ROTX         = XMLString::transcode("rot-x");
	ATTR_OFFSET_ROTY       = XMLString::transcode("rot-y");
	ATTR_OFFSET_ROTZ          = XMLString::transcode("rot-z");
	//ATTR_OFFSET_POS          = XMLString::transcode("offset-pos";
	
	ATTR_RAMPUP 	= XMLString::transcode("sbm:rampup");
	ATTR_RAMPDOWN	= XMLString::transcode("sbm:rampdown");
	ATTR_DURATION	= XMLString::transcode("sbm:duration");

	//ATTR_X      = XMLString::transcode("pos-x");
	//ATTR_Y      = XMLString::transcode("pos-y");
	//ATTR_Z      = XMLString::transcode("pos-z");

	////// XML ATTRIBUTES
	ATTR_TARGET_POS  = XMLString::transcode("sbm:target-pos");
	ATTR_REACH_VELOCITY  = XMLString::transcode("sbm:reach-velocity");
	ATTR_REACH_DURATION  = XMLString::transcode("sbm:reach-duration");
	ATTR_REACH_FINISH  = XMLString::transcode("sbm:reach-finish");
	ATTR_REACH_TYPE  = XMLString::transcode("sbm:reach-type");
	//ATTR_APEX_DURATION  = XMLString::transcode("sbm:apex-duration";

	ATTR_ROTATION      = XMLString::transcode("rotation");
	ATTR_ENABLE      = XMLString::transcode("enable");
	ATTR_SPEED      = XMLString::transcode("speed");

	ATTR_X      = XMLString::transcode("x");
	ATTR_Y      = XMLString::transcode("y");
	ATTR_Z      = XMLString::transcode("z");

	ATTR_POSX      = XMLString::transcode("pos-x");
	ATTR_POSY      = XMLString::transcode("pos-y");
	ATTR_POSZ      = XMLString::transcode("pos-z");

	ATTR_ROTX      = XMLString::transcode("rot-x");
	ATTR_ROTY      = XMLString::transcode("rot-y");
	ATTR_ROTZ      = XMLString::transcode("rot-z");


	TAG_VISEME  = XMLString::transcode("viseme");
	TAG_SOUND  = XMLString::transcode("soundFile"); //this tag is used to rename the soundFile by Remote speech process
	ATTR_AU      = XMLString::transcode("au");
	ATTR_SIDE    = XMLString::transcode("side");	

	////// XML ATTRIBUTES
	ATTR_SBM_ROLL      = XMLString::transcode("sbm:roll");
	ATTR_JOINT_RANGE   = XMLString::transcode("sbm:joint-range");
	ATTR_JOINT_SPEED   = XMLString::transcode("sbm:joint-speed");
	ATTR_TIME_HINT	 = XMLString::transcode("sbm:time-hint");
	ATTR_JOINT_SMOOTH  = XMLString::transcode("sbm:speed-smoothing");
	ATTR_PITCH         = XMLString::transcode("pitch");
	ATTR_HEADING       = XMLString::transcode("heading");
	ATTR_ROLL          = XMLString::transcode("roll");
	ATTR_BLEND         = XMLString::transcode("blend");
	ATTR_INTERPOLATE_BIAS  = XMLString::transcode("interpolate-bias");

	ATTR_PRIORITY_JOINT  = XMLString::transcode("sbm:priority-joint");
	ATTR_PITCH_MIN	 = XMLString::transcode("pitch-min");
	ATTR_PITCH_MAX	 = XMLString::transcode("pitch-max");

//	TAG_MARK       = XMLString::transcode("mark");

	VALUE_TEXT_PLAIN  = XMLString::transcode("text/plain");
	VALUE_SSML        = XMLString::transcode("application/ssml+xml");

	start_id  = XMLString::transcode("bml:start");
	end_id    = XMLString::transcode("bml:end");

	////// XML ATTRIBUTES
	ATTR_REACH_ARM  = XMLString::transcode("reach-arm");
	ATTR_USE_EXAMPLE  = XMLString::transcode("use-example");
	ATTR_BUILD_EXAMPLE  = XMLString::transcode("build-example");
	ATTR_EXAMPLE_DIST  = XMLString::transcode("example-dist"); // minimal distances between pose examples
	ATTR_RESAMPLE_SIZE  = XMLString::transcode("resample-size"); // minimal distances between pose examples
	////// XML ATTRIBUTES
	ATTR_ANIM          = XMLString::transcode("anim");
	ATTR_TRACK_DUR     = XMLString::transcode("track-duration");
	ATTR_GUNDRAW_DUR    = XMLString::transcode("gundraw-duration");
	ATTR_HOLSTER_DUR    = XMLString::transcode("holster-duration");
	ATTR_AIM_OFFSET     = XMLString::transcode("aim-offset");


	ATTR_ANIM1  = XMLString::transcode("anim1");
	ATTR_ANIM2  = XMLString::transcode("anim2");
	ATTR_LOOP  = XMLString::transcode("loop");
	ATTR_WRAPMODE = XMLString::transcode("sbm:wrap-mode");
	ATTR_BLENDMODE = XMLString::transcode("sbm:blend-mode");
	ATTR_SCHEDULEMODE = XMLString::transcode("sbm:schedule-mode");
	ATTR_TRANSITION_LENGTH = XMLString::transcode("sbm:transition-length");
	ATTR_DIRECTPLAY = XMLString::transcode("sbm:direct-play");
	ATTR_OVERLAY = XMLString::transcode("overlay");

	////// XML ATTRIBUTES
	ATTR_VALUE   = XMLString::transcode("value");

	ATTR_TRUE   = XMLString::transcode("true");
	ATTR_FALSE   = XMLString::transcode("false");
	ATTR_RESET   = XMLString::transcode("reset");

	ATTR_SHAKE  = XMLString::transcode("shake");
	ATTR_TOSS   = XMLString::transcode("toss");
	ATTR_ORIENT   = XMLString::transcode("orient");
	ATTR_NOD   = XMLString::transcode("nod");
	ATTR_WIGGLE   = XMLString::transcode("wiggle");
	ATTR_WAGGLE   = XMLString::transcode("waggle");
	ATTR_PARAMETERIZED = XMLString::transcode("parameterized");

	ATTR_ARMLEFT   = XMLString::transcode("left");
	ATTR_ARMRIGHT   = XMLString::transcode("right");

	ATTR_FACS   = XMLString::transcode("facs");
	ATTR_RPS = XMLString::transcode("rps");
	ATTR_GRPS = XMLString::transcode("grps");
	ATTR_LRPS= XMLString::transcode("lrps");

	ATTR_EYEBROWS = XMLString::transcode("eyebrows");
	ATTR_EYELIDS = XMLString::transcode("eyelids");
	ATTR_MOUTH = XMLString::transcode("mouth");

	ATTR_ROOT = XMLString::transcode("sbm:root");

	TAG_GRAB = XMLString::transcode("sbm:grab");
	//ATTR_REACH_FINISH = XMLString::transcode("sbm:reach-finish");
	ATTR_CONS_JOINT = XMLString::transcode("sbm:cons-joint");
	ATTR_CONS_TARGET = XMLString::transcode("sbm:cons-target");
	ATTR_APEX_DURATION = XMLString::transcode("sbm:apex-duration");
	ATTR_WRIST = XMLString::transcode("sbm:wrist");
	ATTR_GRAB_SPEED = XMLString::transcode("sbm:grab-speed");
	ATTR_GRAB_FINISH = XMLString::transcode("sbm:grab-finish");
	ATTR_GRAB_TYPE = XMLString::transcode("sbm:grab-type");
	ATTR_OBSTACLE = XMLString::transcode("sbm:obstacle");
	ATTR_GRAB_STATE = XMLString::transcode("sbm:grab-state");
	ATTR_REACH_ACTION = XMLString::transcode("sbm:action");
	ATTR_PROXIMITY = XMLString::transcode("proximity");
	ATTR_MANNER = XMLString::transcode("manner");
	ATTR_FACING= XMLString::transcode("facing");
	ATTR_FOLLOW= XMLString::transcode("sbm:follow");
	ATTR_NUM_STEPS = XMLString::transcode("sbm:numsteps");
	ATTR_SPD = XMLString::transcode("spd");
	ATTR_SOURCE_JOINT = XMLString::transcode("sbm:source-joint");
	ATTR_ATTACH_PAWN = XMLString::transcode("sbm:attach-pawn");
	ATTR_RELEASE_PAWN = XMLString::transcode("sbm:release-pawn");
	ATTR_FOOT_IK = XMLString::transcode("sbm:foot-ik");
	ATTR_USE_LOCOMOTION = XMLString::transcode("sbm:use-locomotion");

	ATTR_MAGNITUDE = XMLString::transcode("magnitude");
	ATTR_MODE = XMLString::transcode("mode");
	ATTR_ANGLELIMIT = XMLString::transcode("angle-limit");
	ATTR_FINISH = XMLString::transcode("finish");

	ATTR_STEERACCEL = XMLString::transcode("sbm:accel");
	ATTR_STEERANGLEACCEL = XMLString::transcode("sbm:angleaccel");
	ATTR_STEERSCOOTACCEL = XMLString::transcode("sbm:scootaccel");

	ATTR_NODAXIS = XMLString::transcode("sbm:axis");
	ATTR_NODACCEL = XMLString::transcode("sbm:accel");
	ATTR_NODPERIOD = XMLString::transcode("sbm:period");
	ATTR_NODWARP = XMLString::transcode("sbm:warp");
	ATTR_NODPITCH = XMLString::transcode("sbm:pitch");
	ATTR_NODDECAY = XMLString::transcode("sbm:decay");

	TAG_STATES = XMLString::transcode("sbm:states");
	TAG_BLEND = XMLString::transcode("blend");
	ATTR_STARTINGNOW = XMLString::transcode("sbm:startnow");
	ATTR_ADDITIVE = XMLString::transcode("sbm:additive");
	ATTR_PARTIALJOINT = XMLString::transcode("sbm:partial-joint");

	ATTR_PERCENTAGE_BIN0 = XMLString::transcode("sbm:bin0");
	ATTR_PERCENTAGE_BIN45 = XMLString::transcode("sbm:bin45");
	ATTR_PERCENTAGE_BIN90 = XMLString::transcode("sbm:bin90");
	ATTR_PERCENTAGE_BIN135 = XMLString::transcode("sbm:bin135");
	ATTR_PERCENTAGE_BIN180 = XMLString::transcode("sbm:bin180");
	ATTR_PERCENTAGE_BIN225 = XMLString::transcode("sbm:bin225");
	ATTR_PERCENTAGE_BIN270 = XMLString::transcode("sbm:bin270");
	ATTR_PERCENTAGE_BIN315 = XMLString::transcode("sbm:bin315");
	ATTR_SACCADE_INTERVAL_MEAN = XMLString::transcode("sbm:mean");
	ATTR_SACCADE_INTERVAL_VARIANT = XMLString::transcode("sbm:variant");

	// OpenCollada
	ATTR_SID = XMLString::transcode("sid");
	ATTR_STRIDE = XMLString::transcode("stride");
	ATTR_COUNT = XMLString::transcode("count");
	ATTR_INPUTSTR = XMLString::transcode("input");
	ATTR_SEMANTIC = XMLString::transcode("semantic");
	ATTR_VCOUNT = XMLString::transcode("vcount");
	ATTR_SOURCE = XMLString::transcode("source");
	ATTR_URL = XMLString::transcode("url");
	ATTR_OFFSET = XMLString::transcode("offset");
	ATTR_TRIM = XMLString::transcode("trim");
	ATTR_SYMBOL = XMLString::transcode("symbol");
	ATTR_MATERIAL = XMLString::transcode("material");
	ATTR_OPAQUE = XMLString::transcode("opaque");
	ATTR_TEXTURE = XMLString::transcode("texture");

	// locomotion
	ATTR_BRAKING = XMLString::transcode("sbm:braking");

	ATTR_STYLE = XMLString::transcode("sbm:style");

	ATTR_STATENAME = XMLString::transcode("sbm:state-name");

	ATTR_POLICY = XMLString::transcode("policy");
	ATTR_PROCID = XMLString::transcode("procid");

	OGRE_BONE = XMLString::transcode("bone");
	OGRE_PARENT = XMLString::transcode("parent");
	OGRE_ID = XMLString::transcode("id");
	OGRE_VERTEX_COUNT = XMLString::transcode("vertexcount");
	OGRE_NAME = XMLString::transcode("name");
	OGRE_MATERIAL = XMLString::transcode("material");
	OGRE_LENGTH = XMLString::transcode("length");
	OGRE_X = XMLString::transcode("x");
	OGRE_Y = XMLString::transcode("y");
	OGRE_Z = XMLString::transcode("z");
	OGRE_U = XMLString::transcode("u");
	OGRE_V = XMLString::transcode("v");
	OGRE_V1 = XMLString::transcode("v1");
	OGRE_V2 = XMLString::transcode("v2");
	OGRE_V3 = XMLString::transcode("v3");
	OGRE_T1 = XMLString::transcode("t1");
	OGRE_T2 = XMLString::transcode("t2");
	OGRE_T3 = XMLString::transcode("t3");
	OGRE_ANGLE = XMLString::transcode("angle");
	OGRE_AXIS = XMLString::transcode("axis");
	OGRE_ANIMATION = XMLString::transcode("animation");
	OGRE_TRACKS = XMLString::transcode("tracks");
	OGRE_TRACK = XMLString::transcode("track");
	OGRE_KEYFRAMES = XMLString::transcode("keyframes");
	OGRE_KEYFRAME = XMLString::transcode("keyframe");
	OGRE_TRANSLATE = XMLString::transcode("translate");
	OGRE_ROTATE = XMLString::transcode("rotate");
	OGRE_TIME = XMLString::transcode("time");
	OGRE_VERTEX_INDEX = XMLString::transcode("vertexindex");
	OGRE_BONE_INDEX = XMLString::transcode("boneindex");
	OGRE_WEIGHT = XMLString::transcode("weight");

}

BMLDefs::~BMLDefs()
{

}

}
