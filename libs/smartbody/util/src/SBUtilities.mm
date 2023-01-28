#include "SBTypes.h"
#include "SBUtilities.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <stdlib.h>

#if defined(SB_IPHONE)  // IOS_BUILD


#import <Foundation/Foundation.h>

SmartBody::util::Logger SmartBody::util::g_log;

void IOSLog(const char * message, ...)
{
	va_list argList;
	va_start(argList, message);
	//__android_log_vprint(ANDROID_LOG_INFO, "SBM", message, argList);
    NSLogv([NSString stringWithUTF8String:message], argList) ;
	va_end(argList);
}


SmartBody::util::Listener::~Listener()
{
}



SmartBody::util::FileListener::FileListener(const char * filename)
{

}


SmartBody::util::FileListener::~FileListener()
{
}


void SmartBody::util::FileListener::OnMessage(const std::string & message)
{
	// we're now letting log4cxx handle the message
}



SmartBody::util::IOSListener::IOSListener()
{

}


SmartBody::util::IOSListener::~IOSListener()
{
}


void SmartBody::util::IOSListener::OnMessage(const std::string & message)
{
	// we're now letting log4cxx handle the message
	while (logList.size() > 100)
		logList.pop_front();
	logList.emplace_back(message);
}

std::string SmartBody::util::IOSListener::getLogs()
{
	std::string strLog;
	std::list<std::string>::iterator li;
	for (li = logList.begin();
		li != logList.end();
		li++)
	{
		strLog += (*li);
		strLog += "\n";
	}
	return strLog;
}


SmartBody::util::DebuggerListener::DebuggerListener()
{

}


SmartBody::util::DebuggerListener::~DebuggerListener()
{
}


void SmartBody::util::DebuggerListener::OnMessage(const std::string & message)
{
	// we're now letting log4cxx handle the message
}


SmartBody::util::StdoutListener::StdoutListener()
{

}


SmartBody::util::StdoutListener::~StdoutListener()
{
}


void SmartBody::util::StdoutListener::OnMessage(const std::string & message)
{
	// we're now letting log4cxx handle the message
}


SmartBody::util::Logger::Logger()
{

}


SmartBody::util::Logger::~Logger()
{
	RemoveAllListeners();
}


void SmartBody::util::Logger::AddListener(Listener * listener)
{
	m_listeners.emplace_back(listener);
}


void SmartBody::util::Logger::RemoveListener(Listener * listener)
{
	std::vector<Listener *>::iterator iter = m_listeners.end();
	for (iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
	{
		if ((*iter) == listener)
			break;
	}

	if (iter != m_listeners.end())
		m_listeners.erase(iter);
}


void SmartBody::util::Logger::RemoveAllListeners()
{
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		// don't call delete until we more formally describe the ownership policy
		//delete m_listeners[ i ];
	}
	m_listeners.clear();
}


bool SmartBody::util::Logger::IsEnabled() const
{
	if (m_listeners.empty())
		return false;

	return true;
}

#include "vhcl.h"

void SmartBody::util::Logger::Log(const char * message, ...)
{
	if (!IsEnabled())
		return;

	va_list argList;
	va_start(argList, message);
	std::string result = vhcl::vFormat(message, argList);
	va_end(argList);

	std::string s = result;
	s.append("\n");

	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		m_listeners[i]->OnMessage(s);
	}
}



void SmartBody::util::Logger::vLog(const char * message, va_list argPtr)
{
	if (!IsEnabled())
		return;

	std::string result = vhcl::vFormat(message, argPtr);

	std::string s = result;
	s.append("\n");

	IOSLog(s.c_str());
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		m_listeners[i]->OnMessage(s);
	}
}

void SmartBody::util::Logger::vLogSimple(const char * message)
{
	if (!IsEnabled())
		return;

	std::string s = message;
	s.append("\n");

	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		m_listeners[i]->OnMessage(s);
	}
}

#endif  // IOS_BUILD


