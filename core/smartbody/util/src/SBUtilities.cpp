#include "SBUtilities.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <iostream>
#ifndef WIN_BUILD
#include <wchar.h>
#endif

// from: http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
void SmartBody::util::tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

std::string SmartBody::util::toLower(const std::string& str)
{
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), tolower);
	return lowerStr;
}


std::string SmartBody::util::toUpper(const std::string& str)
{
	std::string upperStr = str;
	std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), toupper);
	return upperStr;
}

std::string SmartBody::util::replace(std::string str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return str;
}

float SmartBody::util::toFloat(const std::string& str)
{
	return (float)atof(str.c_str());
}

double SmartBody::util::toDouble(const std::string& str)
{
	return atof(str.c_str());
}

int SmartBody::util::toInt(const std::string& str)
{
	return (int)atoi(str.c_str());
}

float SmartBody::util::degToRad(float deg)
{
	return deg * 3.14159f / 180.0f;
}

float SmartBody::util::radToDeg(float rad)
{
	return rad * 180.0f / 3.14159f;
}


std::string SmartBody::util::format(const char * fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	std::string result = vFormat(fmt, argList);
	va_end(argList);

	return result;
}


std::wstring SmartBody::util::format(const wchar_t * fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	std::wstring result = vFormat(fmt, argList);
	va_end(argList);

	return result;
}


std::string SmartBody::util::vFormat(const char * fmt, va_list argPtr)
{
	// If the string is less than a certain amount of characters,
	// allocate it on the stack because this saves the malloc/free time.
	const int32_t bufSize = 512;
	char stackBuffer[bufSize];

	int32_t numChars = vsnprintf(stackBuffer, bufSize, fmt, argPtr);
	if (numChars < bufSize)
	{
		// Got it on the first try.
		return std::string(stackBuffer);
	}

  // Now use the heap.
	// Define a given max size for the longest a string can be
  // if the string is longer than this, well, tough luck...
	const int32_t maxSize = 32768;
  std::unique_ptr<char[]> buffer(new char[maxSize]);
  // should be using vasprintf(), but it's a GNU extension.
  return vsnprintf(buffer.get(), maxSize, fmt, argPtr) > 0
  ? std::string(buffer.get()) : "";
}

std::wstring SmartBody::util::vFormat(const wchar_t * fmt, va_list argPtr)
{
	// TODO: linux
#if defined(WIN_BUILD)
	// If the string is less than a certain amount of characters,
	// allocate it on the stack because this saves the malloc/free time.
	const int32_t bufSize = 512;
	wchar_t stackBuffer[bufSize];

	int32_t attemptedSize = bufSize - 1;

	int32_t numChars = _vsnwprintf(stackBuffer, attemptedSize, fmt, argPtr);
	if (numChars >= 0)
	{
		// if we had exactly the number of characters, then the null doesn't automatically get appended
		stackBuffer[attemptedSize] = '\0';

		// Got it on the first try.
		return wstring(stackBuffer);
	}

	// Define a given max size for the longest a string can be
	const int32_t maxSize = 32768;

	// Now use the heap.
	wchar_t * heapBuffer = NULL;

	while ((numChars == -1) && (attemptedSize < maxSize))
	{
		// Try a bigger size
		attemptedSize *= 2;
		delete[] heapBuffer;
		heapBuffer = new wchar_t[attemptedSize + 1];
		numChars = _vsnwprintf(heapBuffer, attemptedSize, fmt, argPtr);
		heapBuffer[attemptedSize] = '\0';  // make sure there's a null at the end, doesn't always get appended
	}

	wstring result = wstring(heapBuffer);

	delete[] heapBuffer;
	heapBuffer = NULL;

	return result;
#else
	// If the string is less than a certain amount of characters,
	// allocate it on the stack because this saves the malloc/free time.
	const int32_t bufSize = 512;
	wchar_t stackBuffer[bufSize];

	int32_t attemptedSize = bufSize - 1;

	int32_t numChars = vswprintf(stackBuffer, attemptedSize, fmt, argPtr);
	if (numChars >= 0)
	{
		// if we had exactly the number of characters, then the null doesn't automatically get appended
		stackBuffer[attemptedSize] = '\0';

		// Got it on the first try.
		return std::wstring(stackBuffer);
	}

	// Define a given max size for the longest a string can be
	const int32_t maxSize = 32768;

	// Now use the heap.
	wchar_t * heapBuffer = NULL;

	while ((numChars == -1) && (attemptedSize < maxSize))
	{
		// Try a bigger size
		attemptedSize *= 2;
		delete[] heapBuffer;
		heapBuffer = new wchar_t[attemptedSize + 1];
		numChars = vswprintf(heapBuffer, attemptedSize, fmt, argPtr);
		heapBuffer[attemptedSize] = '\0';  // make sure there's a null at the end, doesn't always get appended
	}

	std::wstring result = std::wstring(heapBuffer);

	delete[] heapBuffer;
	heapBuffer = NULL;

	return result;
#endif
}


SBAPI void SmartBody::util::log(const char * message, ...)
{
  va_list argList;
  va_start(argList, message);
  g_log.vLog(message, argList);
  va_end(argList);
}

SBAPI void SmartBody::util::logSimple(const char * message)
{
	g_log.vLogSimple(message);
}

#if !defined(__ANDROID__) //&& !defined(SB_IPHONE)


// global instance of a logger
SmartBody::util::Logger SmartBody::util::g_log;


SmartBody::util::Listener::~Listener()
{
}



SmartBody::util::FileListener::FileListener(const char * filename)
{
	m_filename = filename;

	// wipe the file before starting
	FILE * file = fopen(m_filename.c_str(), "w");
	if (file)
	{
		fclose(file);
	}
};


SmartBody::util::FileListener::~FileListener()
{
}


void SmartBody::util::FileListener::OnMessage(const std::string & message)
{
	// we open the file, write the message, then close the file.
	// this makes sure the file is properly flushed and released each time a message is received.
	// otherwise, the last output may not be received before a crash.
	// also, if it crashes in a wierd way or debugging is stopped manually, the file may still be locked after exit
	// also, viewing the file while it's open may not show the full contents of the file (due to delayed writing, etc)
	// also, the file is locked during a run, which may be an issue if you want to copy/move the file while the program is running.

	FILE * file = fopen(m_filename.c_str(), "a");
	if (file)
	{
		fprintf(file, "%s", message.c_str());
		fclose(file);
	}
}


SmartBody::util::DebuggerListener::DebuggerListener()
{
}


SmartBody::util::DebuggerListener::~DebuggerListener()
{
}


void SmartBody::util::DebuggerListener::OnMessage(const std::string & message)
{
#if defined(WIN_BUILD)
	OutputDebugString(message.c_str());
#endif
}


SmartBody::util::StdoutListener::StdoutListener()
{
}


SmartBody::util::StdoutListener::~StdoutListener()
{
}


void SmartBody::util::StdoutListener::OnMessage(const std::string & message)
{
	printf("%s", message.c_str());
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
	m_listeners.push_back(listener);
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


void SmartBody::util::Logger::Log(const char * message, ...)
{
	if (!IsEnabled())
		return;

	va_list argList;
	va_start(argList, message);
	std::string result = vFormat(message, argList);
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

	std::string result = vFormat(message, argPtr);

	std::string s = result;
	s.append("\n");

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


#else  // ANDROID_BUILD


#include <android/log.h>


SmartBody::util::Logger SmartBody::util::g_log;

void AndroidLOG(const char * message, ...)
{
	va_list argList;
	va_start(argList, message);
	__android_log_vprint(ANDROID_LOG_INFO, "SBM", message, argList);
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



SmartBody::util::AndroidListener::AndroidListener()
{

}


SmartBody::util::AndroidListener::~AndroidListener()
{
}


void SmartBody::util::AndroidListener::OnMessage(const std::string & message)
{
	// we're now letting log4cxx handle the message
	while (logList.size() > 100)
		logList.pop_front();
	logList.push_back(message);
}

std::string SmartBody::util::AndroidListener::getLogs()
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
	m_listeners.push_back(listener);
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

	AndroidLOG(s.c_str());
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

#endif  // ANDROID_BUILD


