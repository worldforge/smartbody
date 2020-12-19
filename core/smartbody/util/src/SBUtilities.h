#pragma once
#include "SBABI.h"

#include <vector>
#include <string>
#include <list>
#include <cstdarg>

namespace SmartBody {

	namespace util {

		SBAPI void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");
		SBAPI std::string toLower(const std::string& str);
		SBAPI std::string toUpper(const std::string& str);
		SBAPI std::string replace(std::string str, const std::string& from, const std::string& to);
		SBAPI float toFloat(const std::string& str);
		SBAPI double toDouble(const std::string& str);
		SBAPI int toInt(const std::string& str);
		SBAPI float degToRad(float deg);
		SBAPI float radToDeg(float rad);
		SBAPI std::string format(const char * fmt, ...);
		SBAPI std::wstring format(const wchar_t * fmt, ...);
		SBAPI std::string vFormat(const char * fmt, va_list argPtr);
		SBAPI std::wstring vFormat(const wchar_t * fmt, va_list argPtr);

		class Listener
		{
		public:
			SBAPI virtual ~Listener();
			SBAPI virtual void OnMessage(const std::string & message) = 0;
		};


		class FileListener : public Listener
		{
		private:
			std::string m_filename;

		public:
			SBAPI explicit FileListener(const char * filename);
			SBAPI ~FileListener() override;

			SBAPI void OnMessage(const std::string & message) override;
		};


		class DebuggerListener : public Listener
		{
		public:
			SBAPI DebuggerListener();
			SBAPI ~DebuggerListener() override;

			SBAPI void OnMessage(const std::string & message) override;
		};


#ifdef __ANDROID__
		class AndroidListener : public Listener
		{
		public:
			std::list<std::string> logList;

		public:
			AndroidListener();
			virtual ~AndroidListener();

			virtual void OnMessage(const std::string & message);
			std::string getLogs();
		};
#endif
        
#ifdef SB_IPHONE
        class IOSListener : public Listener
        {
        public:
            std::list<std::string> logList;
            
        public:
            IOSListener();
            virtual ~IOSListener();
            
            virtual void OnMessage(const std::string & message);
            std::string getLogs();
        };
#endif


		class StdoutListener : public Listener
		{
		public:
			SBAPI StdoutListener();
			SBAPI ~StdoutListener() override;

			SBAPI void OnMessage(const std::string & message) override;
		};


		class Logger
		{
		private:
			std::vector< Listener * > m_listeners;

		public:
			SBAPI Logger();
			SBAPI virtual ~Logger();

			SBAPI void AddListener(Listener * listener);
			SBAPI void RemoveListener(Listener * listener);
			SBAPI void RemoveAllListeners();

			SBAPI bool IsEnabled() const;


			SBAPI void Log(const char * message, ...);

			SBAPI void vLog(const char * message, va_list argPtr);
			SBAPI void vLogSimple(const char * message);
		};


		// global instance of a logger
		SBAPI extern Logger g_log;

		SBAPI void log(const char * message, ...);
		SBAPI void logSimple(const char * message);
	}

};
