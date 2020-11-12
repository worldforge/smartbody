
#include "vhcl.h"
#include "vhcl_audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if defined(WIN_BUILD)
#include <conio.h>
#include <windows.h>
#endif


using std::string;
using std::vector;


class TestException : public std::exception
{
   public:
      TestException() {}
      virtual const char * what() const throw() { return "EXCEPTION: Caught TestException!"; }
};


// prototype, found in testempty.cpp
void testCrashEmpty();

void testCrash3()
{
   testCrashEmpty();
   //return;


   // use this to test null pointer exception report
#if 0
   volatile int * n = 0x0;
   int crash = *n;
#endif


   // use this to test C++ exception report
#if 0
   throw TestException();
#endif
}

void testCrash2()
{
   testCrash3();
}

void testCrash1()
{
   testCrash2();

   try
   {
      testCrash2();
   }
   catch ( std::exception & )
   {
      //vhcl::Crash::SendExceptionReport( e );
      throw;
   }
}


class TestCrashCallback : public vhcl::Crash::CrashCallback
{
   public:
      virtual void OnCrash()
      {
         LOG( "CRASH: Testing TestCrashCallback" );
      }
};


void testAssert()
{
   // testing vhcl_assert

   bool isTrue = true;

   ASSERT( isTrue );

   // to trigger the assert
   //ASSERT( !isTrue );
}


void testAudio()
{
   // testing vhcl_audio

   const string audioFile = "gong.au";

   vhcl::Audio audio;
   audio.Open();

   vhcl::Sound * sound = audio.CreateSound( "gong.au", "gong" );
   //vhcl::Sound * sound = audio.CreateSoundLibSndFile( "test2.aiff", "test" );
   sound->Play();

   LOG( "AUDIO: Playing %s - Sleep() until finished", audioFile.c_str() );

   int i = 0;
   while ( i++ < 50 )
   {
      //audio.SetListenerPos( 0, 0, 3 * ( i % 2 ? -1 : 1 ) );
      //sound->SetPosition( 0, 0, 3 * ( i % 2 ? -1 : 1 ) );
      audio.Update( i / 10.0f );

      vhcl::Sleep( 0.1 );
   }

   audio.Close();

   LOG( "" );
}


void testCrash()
{
   // testing vhcl_crash  (with stack trace)

   testCrash1();
}


void testLog()
{
   // testing vhcl_log
   vhcl::Log::g_log.AddListener( new vhcl::Log::DebuggerListener() );
   vhcl::Log::g_log.AddListener( new vhcl::Log::FileListener( "test.log" ) );
   vhcl::Log::g_log.AddListener( new vhcl::Log::StdoutListener() );
   //vhcl::Log::g_log.UseFileConfig( "log.props" );

   LOG( "LOG: Should see 3 log messages following this one" );
   vhcl::Log::g_log.Log( "LOG: Log Test" );

   LOG( "LOG: Log Test using LOG()" );
   LOG( "LOG: Log Test printf style: %d %s %5.3f", 42, vhcl::Format( "'hello world %d'", 42 ).c_str(), sqrt( 42.0 ) );
   LOG( "" );
}


void testMemory()
{
   LOG( "MEMORY: Should see 2 memory leak if run through the debugger" );
   LOG( "MEMORY: new()/delete() currently doesn't show line numbers, but malloc()/free() does" );
   LOG( "" );
   string * testLeak = new string( "leaktest" );
   int * testLeak2 = (int *)malloc(sizeof(int) );
}


void testPlatform()
{
   // Predefined Macros - http://msdn.microsoft.com/en-us/library/b0084kay(v=VS.90).aspx

#if defined(WIN32_BUILD)
   LOG("PLATFORM: Windows 32-bit platform detected" );
#endif

#if defined(WIN64_BUILD)
   LOG("PLATFORM: Windows 64-bit platform detected.  Note, _WIN32 is defined on Win64 for backward compatibility, but not WIN32_BUILD." );
#endif

#if defined(WIN_CONSOLE_BUILD)
   LOG("PLATFORM: Windows console executable detected." );
#endif

#if defined(LINUX_BUILD)
   LOG("PLATFORM: Linux platform detected." );
#endif

#if defined(ANDROID_BUILD)
   LOG("PLATFORM: Android platform detected." );
#endif

#if defined(IPHONE_BUILD)
   LOG("PLATFORM: ios platform detected." );
#endif

#if defined(MAC_BUILD)
   LOG("PLATFORM: Mac/OSX platform detected." );
#endif

#if defined(DEBUG_BUILD)
   LOG("PLATFORM: Debug build detected." );
#endif

#if defined(RELEASE_BUILD)
   LOG("PLATFORM: Release build detected." );
#endif

   LOG( "" );
}


void testString()
{
   // testing vhcl_string

   const char * test = "Hello world";
   const wchar_t * testUnicode = L"Hello world";
   const char * testUTF8 = "Hello world";
   const char * startsWith = "Hello";
   const char * endsWith = "world";
   const char * replaceTest = "XXreplaceXX";
   const char * testPath = "C:\\test\\testFolder\\test.exe";
   const char * testUnixPath = "/linux/testFolder/test";
   const char * testFilename = "test.exe";

   LOG( "STRING: testing vhcl::Format() == %s", vhcl::Format( "%d - %s", 42, test ).c_str() );
   if ( vhcl::StartsWith( (string)test, (string)startsWith ) ) LOG("STRING: '%s' does start with '%s'", test, startsWith ); else LOG( "STRING: ERROR" );
   if ( vhcl::EndsWith( (string)test, (string)endsWith ) ) LOG("STRING: '%s' does end with '%s'", test, endsWith ); else LOG( "STRING: ERROR" );
   LOG( "STRING: replacing '%s' in '%s' with '%s' == '%s'", startsWith, test, endsWith, vhcl::Replace( (string)test, (string)startsWith, (string)endsWith ).c_str() );
   LOG( "STRING: replacing 'l's' in '%s' with 'X' == '%s'", test, vhcl::Replace( (string)test, "l", "X" ).c_str() );
   LOG( "STRING: replacing 'X's' in '%s' with '' == '%s'", replaceTest, vhcl::Replace( (string)replaceTest, "X", "" ).c_str() );
   LOG( "STRING: replacing 'X's' in '%s' with '0' == '%s'", replaceTest, vhcl::Replace( (string)replaceTest, "X", "0" ).c_str() );
   LOG( "STRING: replacing 'XX's' in '%s' with '' == '%s'", replaceTest, vhcl::Replace( (string)replaceTest, "XX", "" ).c_str() );
   LOG( "STRING: replacing 'XX's' in '%s' with '0' == '%s'", replaceTest, vhcl::Replace( (string)replaceTest, "XX", "0" ).c_str() );
   LOG( "STRING: testing vhcl::ToLower() == '%s'", vhcl::ToLower( (string)test ).c_str() );
   LOG( "STRING: testing vhcl::ToUpper() == '%s'", vhcl::ToUpper( (string)test ).c_str() );

   LOG( "STRING: testing vhcl::AnsiToWString(%s) = '%s'", test, vhcl::WStringToAnsi( vhcl::AnsiToWString( test ) ).c_str() );
   LOG( "STRING: testing vhcl::WStringToAnsi(%s) = '%s'", test, vhcl::WStringToAnsi( testUnicode ).c_str() );
   LOG( "STRING: testing vhcl::Utf8ToWString(%s) = '%s'", test, vhcl::WStringToAnsi( vhcl::Utf8ToWString( testUTF8 ) ).c_str() );

   vector<string> tokens;
   vhcl::Tokenize( test, tokens, " " );
   LOG( "STRING: testing vhcl::Tokenize() results in: '%s' and '%s' (size=%d)", tokens[ 0 ].c_str(), tokens[ 1 ].c_str(), tokens.size() );

   LOG( "STRING: testing vhcl::ToFloat(%f), vhcl::ToDouble(%f)", vhcl::ToFloat( "42.42" ), vhcl::ToDouble( "42.42" ) );
   LOG( "STRING: testing vhcl::ToInt(%d), vhcl::ToInt64(%lld)", vhcl::ToInt( "42" ), vhcl::ToInt64( "42" ) );
   LOG( "STRING: testing vhcl::ToString(int) = %s", vhcl::ToString( 42 ).c_str() );

   string path;
   string file;
   vhcl::StripPath( (string)testPath, path, file );
   LOG( "STRING: testing vhcl::StripPath(%s) = '%s' and '%s'", testPath, path.c_str(), file.c_str() );
   vhcl::StripPath( (string)testUnixPath, path, file );
   LOG( "STRING: testing vhcl::StripPath(%s) = '%s' and '%s'", testUnixPath, path.c_str(), file.c_str() );

   string fileWithoutExt;
   string ext;
   vhcl::StripExt( (string)testPath, fileWithoutExt, ext );
   LOG( "STRING: testing vhcl::StripExt(%s) = '%s' and '%s'", testPath, fileWithoutExt.c_str(), ext.c_str() );
   vhcl::StripExt( (string)testFilename, fileWithoutExt, ext );
   LOG( "STRING: testing vhcl::StripExt(%s) = '%s' and '%s'", testFilename, fileWithoutExt.c_str(), ext.c_str() );

   LOG( "" );
}


void testTimer()
{
   // testing vhcl_timer
   vhcl::Timer t;
   vhcl::StopWatch s( t );

   //while ( !vhcl::kbhit() )
   {
      LOG( "TIMER: %5.5f == ~0.0 since app started", t.GetTime() );
   }

   s.Start();

   LOG( "TIMER: Test Sleep(1) - start" );
   vhcl::Sleep( 1 );
   LOG( "TIMER: Test Sleep(1) - end" );

   s.Stop();
   LOG( "TIMER: Stopwatch: %5.5f == ~1.0 since Sleep started.", s.GetTime() );

   LOG( "TIMER: %5.5f == ~1.0 since app started", t.GetTime() );

   LOG( "" );
}


int mainFull()
{
   testAssert();
   testLog();
   testPlatform();
   testString();
   testTimer();

   testCrash();
   testAudio();

   testMemory();

   return 0;
}


int main()
{
   vhcl::Crash::EnableExceptionHandling( true );
   vhcl::Crash::AddCrashCallback( new TestCrashCallback() );
   //vhcl::Crash::SetEmailServer( "noserver.ict.usc.edu" );
   //vhcl::Crash::SetSourceEmail( "noreply@ict.usc.edu" );
   //vhcl::Crash::SetDestinationEmail( "noreply@ict.usc.edu" );

   vhcl::Memory::EnableDebugFlags();

   mainFull();
}
