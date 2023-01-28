
#include "vhcl.h"

#include "QtCrtDbgOff.h"
#include <QApplication>
#include <QDialog>
#include "QtCrtDbgOn.h"

#include "SbmDebuggerForm.h"

#include "vhmsg-tt.h"


int main(int argc, char *argv[])
{
   vhcl::Crash::EnableExceptionHandling(true);
   //vhcl::Memory::EnableDebugFlags();   // not working due to ttu_open(), VH-345
   vhcl::Log::g_log.AddListener( new vhcl::Log::DebuggerListener() );
   vhcl::Log::g_log.AddListener( new vhcl::Log::StdoutListener() );


#if WIN32_BUILD
   // Get the exe's path and manually set the working dir.
   // We do this so that the .exe can find the asset path correctly,
   // no matter where the .exe was launched from.
   // We normally handle this with a .bat file that does a pushd/popd,
   // but if the .exe is on a network share, you can't pushd to a network share
   wchar_t lpFilename[MAX_PATH];
   GetModuleFileName(NULL, lpFilename, MAX_PATH);
   std::wstring path;
   std::wstring file;
   vhcl::StripPath(std::wstring(lpFilename), path, file);
   SetCurrentDirectory(path.c_str());
#endif


   QApplication app(argc, argv);
   QFont font;
   printf("%s", font.rawName().toStdString().c_str());
   SbmDebuggerForm sbmDebugger;

   return app.exec();
}
