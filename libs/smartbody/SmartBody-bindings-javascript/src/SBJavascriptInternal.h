#ifndef _SBMPYTHONINTERNAL_
#define _SBMPYTHONINTERNAL_

#include <string>
#include <sb/SBScript.h>
#include <sb/SBSceneListener.h>
#include <sr/sr_vec.h>
#include <sb/SBJavascriptClass.h>
#if defined(EMSCRIPTEN)
#include <emscripten/bind.h>
#include <emscripten.h>

namespace SmartBody 
{
#ifndef SB_NO_JAVASCRIPT
	//this part contains functions that allow to be overided in javascript
	struct NvbgWrapper :  public emscripten::wrapper<Nvbg>
	{
		EMSCRIPTEN_WRAPPER(NvbgWrapper);
		void objectEvent(std::string character, std::string name, bool isAnimate, SrVec charPosition, SrVec charVelocity, SrVec objPosition, SrVec objVelocity, SrVec relativePosition, SrVec relativeVelocity)
		{
			return call<void>("objectEvent", character, name, isAnimate, charPosition, charVelocity, objPosition, objVelocity, relativePosition, relativeVelocity);
		}

		bool execute(std::string character, std::string to, std::string messageId, std::string xml)
		{
			return call<bool>("execute", character, to, messageId, xml);
		}

	    bool executeEvent(std::string character, std::string messageId, std::string state)
		{
			return call<bool>("executeEvent", character, messageId, state);	
		}

		bool executeSpeech(std::string character, std::string speechStatus, std::string speechId, std::string speaker)
		{
			return call<bool>("executeSpeech", character, speechStatus, speechId, speaker);
		}

		bool executeSpeechRequest(std::vector<std::string> behaviors, std::vector<std::string> types, std::vector<float> times, std::vector<std::string> targets, std::vector<std::string> info)
		{
			return call<bool>("executeSpeechRequest", behaviors, types, times, targets, info);
		}

		virtual void notifyAction(std::string name)
		{
			return call<void>("notifyLocal", name);
		}

		virtual void notifyBool(std::string name, bool val)
		{
			return call<void>("notifyLocal", name);
		}

		virtual void notifyInt(std::string name, int val)
		{
			return call<void>("notifyLocal", name);
		}

		virtual void notifyDouble(std::string name, double val)
		{
			return call<void>("notifyLocal", name);
		}

		virtual void notifyString(std::string name, std::string val)
		{
			return call<void>("notifyLocal", name);
		}

		virtual void notifyVec3(std::string name, SrVec val)
		{
			return call<void>("notifyLocal", name);
		}

		virtual void notifyMatrix(std::string name, SrMat val)
		{
			return call<void>("notifyLocal", name);
		}

	};

	struct SBScriptWrapper :  emscripten::wrapper<SBScript>
	{
		EMSCRIPTEN_WRAPPER(SBScriptWrapper);
		void start()
		{
			return call<void>("start");
		}

		void stop()
		{
			return call<void>("stop");
		}

		void beforeUpdate(double time)
		{
			return call<void>("beforeUpdate", time);
		}

		void update(double time)
		{
			return call<void>("update", time);
		}

		void afterUpdate(double time)
		{
			return call<void>("afterUpdate", time);
		}
	};


	struct SBEventHandlerWrapper :  emscripten::wrapper<SBEventHandler>
	{
		EMSCRIPTEN_WRAPPER(SBEventHandlerWrapper);
		void executeAction(SBEvent* event)
		{
			return call<void>("executeAction", event);
		}
	};


	struct JavascriptControllerWrapper : emscripten::wrapper<SmartBody::JavascriptController>
	{
		EMSCRIPTEN_WRAPPER(JavascriptControllerWrapper);
		void start()
		{
			return call<void>("start");
		}

		void init()
		{
			return call<void>("init");
		}

		void evaluate()
		{
			return call<void>("evaluate");
		}

		void stop()
		{
			return call<void>("stop");
		}
	};

	struct TransitionRuleWrapper : emscripten::wrapper<SmartBody::SBAnimationTransitionRule>
	{
		EMSCRIPTEN_WRAPPER(TransitionRuleWrapper);
		bool check(SmartBody::SBCharacter* character, SmartBody::SBAnimationBlend* blend)
		{
			return call<bool>("check", character, blend);
		}
	};


	struct CharacterListenerWrapper : emscripten::wrapper<SmartBody::SBSceneListener>
	{
		EMSCRIPTEN_WRAPPER(CharacterListenerWrapper);
		void OnCharacterCreate( const std::string & name, const std::string & objectClass )
		{
			return call<void>("OnCharacterCreate", name, objectClass);
		}
		void OnCharacterDelete( const std::string & name )
		{
			return call<void>("OnCharacterDelete", name);

		}
		void OnCharacterUpdate( const std::string & name)
		{
			return call<void>("OnCharacterUpdate", name);
		}
		void OnPawnCreate( const std::string & name )
		{
			return call<void>("OnPawnCreate", name);
		}
		void OnPawnDelete( const std::string & name )
		{
			return call<void>("OnPawnDelete", name);
		}
		void OnEvent( const std::string & type, const std::string & params )
		{
			return call<void>("OnEvent", type, params);
		}
		void OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime )
		{
			return call<void>("OnViseme", name, visemeName, weight, blendTime);
		}
		void OnChannel( const std::string & name, const std::string & channelName, const float value)
		{
			return call<void>("OnChannel", name, channelName, value);
		}
		void OnLogMessage( const std::string & message)
		{
			return call<void>("OnLogMessage", message);
		}

	};
#endif
}

// wrapper for std::map
template<class T>
struct map_item
{
	typedef typename T::key_type K;
	typedef typename T::mapped_type V;
	static V get(T const& x, K const& i)
	{
		V temp;
		if( x.find(i) != x.end() ) 
			return x.find(i)->second;
		EM_ASM(console.error("Key not found"));
		return temp;		
	}
	static void set(T & x, K const& i, V const& v)
	{
		x[i]=v; // use map autocreation feature
	}
	static void del(T & x, K const& i)
	{
		if( x.find(i) != x.end() ) x.erase(i);
		else EM_ASM(console.error("Key not found"));
	}
};

#endif
#endif
