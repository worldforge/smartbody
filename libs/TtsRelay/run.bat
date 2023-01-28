
@rem unzip festival voices
if not exist bin\lib\festival\festival\lib\voices\english goto runTTS
pushd bin\lib\festival\festival\lib\voices\english
call prepare.bat
popd


:runTTS
pushd bin\x86\Release
start /MIN TtsRelayGui.exe %1 %2 %3 %4 %5 %6 %7 %8
popd
