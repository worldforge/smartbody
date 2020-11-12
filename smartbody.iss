; -- SmartBody.iss --

[Setup]
AppName=SmartBody
AppVerName=SmartBody
DefaultDirName={userdocs}\SmartBody
DefaultGroupName=SmartBody
UninstallDisplayIcon={app}\SmartBody_48x48.ico
Compression=lzma
SolidCompression=yes
OutputDir=.
AppPublisher=USC Institute for Creative Technologies
AppPublisherURL=http://smartbody.ict.usc.edu
AppVersion=r6750
OutputBaseFilename=SmartBody
WizardImageFile=".\sdk\SmartBody_splash.bmp"
RestartIfNeededByRun=yes

[Files]
; documentation
Source: ".\SmartBodyManual.pdf"; DestDir: "{app}"
Source: ".\SmartBodyPythonAPI.html"; DestDir: "{app}"
Source: ".\3rd party licenses.txt"; DestDir: "{app}"

; executables
Source: ".\core\smartbody\sbgui\bin\sbgui.exe"; DestDir: "{app}\bin"
Source: ".\core\smartbody\sbgui\bin\sbguid.exe"; DestDir: "{app}\bin"
Source: ".\core\smartbody\sbgui\bin\*.dll"; DestDir: "{app}\bin"
Source: ".\core\smartbody\sbgui\bin\*.pyd"; DestDir: "{app}\bin"
Source: ".\sdk\.smartbodysettings"; DestDir: "{app}\bin"
;Source: ".\core\smartbody\sbgui\visualc9\SmartBody_48x48.ico"; DestDir: "{app}\bin"

; supporting libraries - Python 2.7
Source: ".\core\smartbody\Python27\*"; DestDir: "{app}\python27"; Excludes: "site-packages, .svn"; Flags: recursesubdirs
Source: ".\core\smartbody\Python27\Lib\site-packages\numpy\*"; DestDir: "{app}\python27\Lib\site-packages\numpy"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\Python27\Lib\site-packages\pyke\*"; DestDir: "{app}\python27\Lib\site-packages\pyke"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\Python27\Lib\site-packages\pyxb\*"; DestDir: "{app}\python27\Lib\site-packages\pyxb"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\Python27\Lib\site-packages\scipy\*"; DestDir: "{app}\python27\Lib\site-packages\scipy"; Excludes: ".svn"; Flags: recursesubdirs
  
; supporting libraries - Ogre 1.8.1
Source: ".\lib\OgreSDK\bin\*"; DestDir: "{app}\OgreSDK\bin";  Excludes: ".svn";  Flags: recursesubdirs
Source: ".\lib\OgreSDK\Docs\*"; DestDir: "{app}\OgreSDK\Docs"; Excludes: ".svn";  Flags: recursesubdirs
Source: ".\lib\OgreSDK\media\*"; DestDir: "{app}\OgreSDK\media"; Excludes: ".svn"; Flags: recursesubdirs

Source: ".\lib\OgreSDK\include\*"; DestDir: "{app}\OgreSDK\include"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\lib\OgreSDK\lib\*"; DestDir: "{app}\OgreSDK\lib"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\lib\OgreSDK\samples\*"; DestDir: "{app}\OgreSDK\samples"; Excludes: ".svn"; Flags: recursesubdirs

; supporting libraries - Irrlicht 1.8.3
Source: ".\lib\irrlicht-1.8.3\*"; DestDir: "{app}\irrlicht-1.8.3";  Excludes: ".svn";  Flags: recursesubdirs
Source: ".\core\irrlichtsmartbody\media\*"; DestDir: "{app}\src\irrlichtsmartbody\media";  Excludes: ".svn";  Flags: recursesubdirs
Source: ".\core\irrlichtsmartbody\src\*"; DestDir: "{app}\src\irrlichtsmartbody";  Excludes: ".svn";  Flags: recursesubdirs


Source: ".\core\smartbody\SmartBody\src\external\protobuf\lib\Release\libprotobuf.pdb"; DestDir: "{app}\lib"

; supporting libraries - CEGUI 0.8.2



; supplementary executables
;Source: ".\core\TtsRelay\bin\x86\Release\*.exe"; DestDir: "{app}\bin"
;Source: ".\core\TtsRelay\bin\x86\Release\*.dll"; DestDir: "{app}\bin"

; library includes
Source: ".\core\smartbody\steersuite-1.3\steerlib\include\*"; DestDir: "{app}\include\steersuite";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\steersuite-1.3\pprAI\include\*"; DestDir: "{app}\include\steersuite";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\steersuite-1.3\external\tinyxml\*"; DestDir: "{app}\include\steersuite\tinyxml";  Excludes: ".svn"; 
Source: ".\core\smartbody\steersuite-1.3\external\mersenne\*"; DestDir: "{app}\include\steersuite\mersenne"; Excludes: ".svn"
Source: ".\core\smartbody\SmartBody\src\external\protobuf\include\*"; DestDir: "{app}\include\"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\lib\vhcl\include\*"; DestDir: "{app}\include\vhcl"
Source: ".\lib\vhmsg\vhmsg-c\include\*"; DestDir: "{app}\include\vhmsg"
Source: ".\lib\bonebus\include\*"; DestDir: "{app}\include\bonebus"
;Source: ".\lib\assimp-3.1.1\include\assimp\*"; DestDir: "{app}\include\assimp-3.1.1\assimp"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\sbgui\external\fltk-1.3.4-1\FL\*"; DestDir: "{app}\include\FL"
Source: ".\core\smartbody\sbgui\external\cegui-0.8.7\include\*"; DestDir: "{app}\include\CEGUI";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\sbgui\external\Pinocchio\*.h"; DestDir: "{app}\include\Pinocchio";
Source: ".\core\smartbody\sbgui\external\polyvox\library\PolyVoxCore\include\*"; DestDir: "{app}\include\polyvox";   Excludes: ".svn"; Flags: recursesubdirs

; libary header files
Source: ".\lib\xerces-c\include\xercesc\*"; DestDir: "{app}\include\xercesc";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\lib\boost\boost\*"; DestDir: "{app}\include\boost";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\SmartBody\ode\include\*"; DestDir: "{app}\include";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\lib\boostnumeric\*"; DestDir: "{app}\include\boostnumeric";  Excludes: ".svn"; Flags: recursesubdirs

; applications
Source: ".\core\smartbody\SmartBody\src\*"; DestDir: "{app}\src\SmartBody";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\sbgui\src\*"; DestDir: "{app}\src\sbgui";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\simplesmartbody\simplesmartbody.cpp"; DestDir: "{app}\src\simplesmartbody"; Flags:

; tts relay
Source: ".\bin\TtsRelay\bin\x86\Release\TtsRelayGui.exe"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\Apache.NMS.ActiveMQ.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\Apache.NMS.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\Ionic.Zlib.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\pthreadVSE.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\rhetrapi.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\TtsRelay.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\Apache.NMS.ActiveMQ.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\vhmsg-net.dll"; DestDir: "{app}\bin"; 
Source: ".\bin\TtsRelay\bin\x86\Release\FestivalDLL.dll"; DestDir: "{app}\bin"; 

; build
Source: ".\core\smartbody\SmartBody\SmartBody.vcxproj"; DestDir: "{app}\build"; Flags:
Source: ".\core\smartbody\SmartBody\SmartBody.vcxproj.filters"; DestDir: "{app}\build"; Flags:
Source: ".\core\smartbody\sbgui\visualc9\sbgui.vcxproj"; DestDir: "{app}\build"; Flags:
Source: ".\core\smartbody\sbgui\visualc9\sbgui.vcxproj.filters"; DestDir: "{app}\build"; Flags:
Source: ".\core\smartbody\simplesmartbody\simplesmartbody.vcxproj"; DestDir: "{app}\build"; Flags:


; libraries
Source: ".\core\smartbody\SmartBody\lib\*.lib"; DestDir: "{app}\lib"
;Source: ".\core\smartbody\SmartBody\lib\*.pdb"; DestDir: "{app}\lib"

Source: ".\core\smartbody\sbgui\external\fltk-1.3.4-1\lib\*.pdb"; DestDir: "{app}\lib"

Source: ".\core\smartbody\SmartBody\src\external\glew\glew32.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\SmartBody\src\external\protobuf\lib\Release\*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\SmartBody\src\external\protobuf\lib\Debug\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\vhcl\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\vhmsg\vhmsg-c\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\bonebus\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\bonebus\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\clapack\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\activemq\activemq-cpp\vs2015-build\ReleaseDLL\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\activemq\activemq-cpp\vs2015-build\DebugDLL\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\xerces-c\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\boost\lib\*vc140-mt-1_59.lib"; DestDir: "{app}\lib"
Source: ".\lib\boost\lib\*vc140-mt-gd-1_59.lib"; DestDir: "{app}\lib"
Source: ".\lib\vhcl\openal\libs\Win32\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\vhcl\libsndfile\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\pthreads\lib\pthreadVSE2.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\ode\lib\ode*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\steersuite-1.3\build\win32\Release\*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\steersuite-1.3\build\win32\Debug\*.lib"; DestDir: "{app}\lib"
Source: ".\lib\pthreads\lib\pthreadVSE2.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\sbgui\external\fltk-1.3.4-1\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\sbgui\external\cegui-0.8.7\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\sbgui\external\Pinocchio\lib\*.lib"; DestDir: "{app}\lib"
Source: ".\core\smartbody\sbgui\external\polyvox\build\lib\*.lib"; DestDir: "{app}\lib"
;Source: ".\lib\assimp-3.1.1\Release\assimp.lib"; DestDir: "{app}\lib"
;Source: ".\lib\assimp-3.1.1\Debug\assimpd.lib"; DestDir: "{app}\lib"


; data

; behaviorsets
Source:  ".\data\behaviorsets\BehaviorSetCommon.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\default-behavior-sets.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetFemaleGestures.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetGestures.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetJumping.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetKicking.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetMaleGestures.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetMaleLocomotion.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetMaleMocapLocomotion.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetMocapReaching.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 
Source:  ".\data\behaviorsets\BehaviorSetReaching.py"; DestDir: "{app}\data\behaviorsets";  Excludes: ".svn"; 

Source: ".\data\behaviorsets\FemaleGestures\*"; DestDir: "{app}\data\behaviorsets\FemaleGestures";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\Gestures\*"; DestDir: "{app}\data\behaviorsets\Gestures";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\Gestures2\*"; DestDir: "{app}\data\behaviorsets\Gestures2";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\jumping\*"; DestDir: "{app}\data\behaviorsets\jumping";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\kicking\*"; DestDir: "{app}\data\behaviorsets\kicking";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\MaleGestures\*"; DestDir: "{app}\data\behaviorsets\MaleGestures";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\MaleLocomotion\*"; DestDir: "{app}\data\behaviorsets\MaleLocomotion";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\MaleMocapLocomotion\*"; DestDir: "{app}\data\behaviorsets\MaleMocapLocomotion";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\MocapReaching\*"; DestDir: "{app}\data\behaviorsets\MocapReaching";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\behaviorsets\reaching\*"; DestDir: "{app}\data\behaviorsets\reaching";  Excludes: ".svn"; Flags: recursesubdirs

Source: ".\data\examples\*"; DestDir: "{app}\data\examples"; Excludes: "Physics, Terrain, .svn";Flags: recursesubdirs
Source: ".\data\fonts\*"; DestDir: "{app}\data\fonts";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\ChrBrad\*"; DestDir: "{app}\data\ChrBrad";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\ChrMaarten\*"; DestDir: "{app}\data\ChrMaarten";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\ChrRachel\*"; DestDir: "{app}\data\ChrRachel";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\Sinbad\*"; DestDir: "{app}\data\Sinbad";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\mesh\ChrBrad\*"; DestDir: "{app}\data\mesh\ChrBrad"
Source: ".\data\mesh\ChrRachel\*"; DestDir: "{app}\data\mesh\ChrRachel"
Source: ".\data\mesh\ChrMaarten\*"; DestDir: "{app}\data\mesh\ChrMaarten"
Source: ".\data\mesh\Sinbad\*"; DestDir: "{app}\data\mesh\Sinbad"
Source: ".\data\scripts\*"; DestDir: "{app}\data\scripts";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\Hand\*"; DestDir: "{app}\data\Hand"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\cegui\datafiles\*"; DestDir: "{app}\data\cegui\datafiles";  Excludes: ".svn"; Flags: recursesubdirs
Source: ".\data\perceptionneuron\*"; DestDir: "{app}\data\perceptionneuron"; Excludes: ".svn, *.bvh";Flags: recursesubdirs
Source: ".\data\shaders\*"; DestDir: "{app}\data\shaders"; Excludes: ".svn"; Flags: recursesubdirs
Source: ".\core\smartbody\SmartBody\src\sbm\GPU\shaderFiles\*"; DestDir: "{app}\data\shaders"; Excludes: ".svn"; Flags: recursesubdirs

Source: "e:\smartbody\externalprograms\vs2015\vc_redist.x86.exe"; DestDir: "{app}"

; sdk build
Source: ".\sdk\README.txt"; DestDir: "{app}\"
Source: ".\sdk\resources.cfg"; DestDir: "{app}\"
Source: ".\sdk\win32\build\*"; DestDir: "{app}\build"
Source: ".\sdk\win32\bin\*"; DestDir: "{app}\bin"
Source: ".\sdk\win32\src\*"; DestDir: "{app}\src";  Excludes: ".svn"; Flags: recursesubdirs

; SB Kinect viewer
Source: ".\lib\SBKinectViewer\*.exe"; DestDir: "{app}\lib\SBKinectViewer"
Source: ".\lib\SBKinectViewer\*.dll"; DestDir: "{app}\lib\SBKinectViewer"



[Registry]

[Icons]
Name: "{group}\SmartBody"; Filename: "{app}\bin\sbgui.exe"; WorkingDir: "{app}"
Name: "{group}\SmartBody Manual"; Filename: "{app}\SmartBodyManual.pdf"; WorkingDir: "{app}"
Name: "{group}\SmartBody Python API"; Filename: "{app}\SmartBodyPythonAPI.html"; WorkingDir: "{app}"

[Run]
Filename: {app}\vcredist_x86.exe; Description: "Visual Studio 2015 Redistributable"; Parameters: "/q:a /c:""install /l """; WorkingDir: {tmp}; Flags: postinstall runascurrentuser ; StatusMsg: "Installing ""Microsoft Visual C++ 2015 Redistributable Package"" if needed. This can take several minutes..."
Filename: {app}\bin\sbgui.exe ; Flags: postinstall ; Description: "Run SmartBody"




