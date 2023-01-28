echo Running Postbuild.bat

@setlocal

@REM Configure for a VHuman project directory
set OGRE_HOME=..\..\..\lib\OgreSDK

@REM Initialize variables according to build type
if (%1)==(DEBUG) goto DEBUG_VARS
goto RELEASE_VARS


:RELEASE_VARS
set BIN_DIR=..\bin\win32
set PLUGINS_FILE=..\src\plugins-win32.cfg
set OGRE_BIN=%OGRE_HOME%\bin9\release
set FINAL_BIN_DIR=..\..\..\bin\ogre-viewer\bin\win32
set FINAL_MEDIA_DIR=..\..\..\bin\ogre-viewer\media

copy /Y %PLUGINS_FILE% %BIN_DIR%\plugins.cfg

copy /Y %OGRE_BIN%\OgreMain.dll %BIN_DIR%
copy /Y %OGRE_BIN%\OIS.dll %BIN_DIR%
copy /Y %OGRE_BIN%\RenderSystem_Direct3D9.dll %BIN_DIR%
copy /Y %OGRE_BIN%\RenderSystem_GL.dll %BIN_DIR%

copy /Y ..\..\..\core\smartbody\SmartBody\lib\SmartBody.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\openal\libs9\Win32\alut.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\libsndfile\bin\libsndfile-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\openal\libs9\Win32\OpenAL32.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\openal\libs9\Win32\wrap_oal.dll %BIN_DIR%
copy /Y ..\..\..\lib\xerces-c\bin9\xerces-c_3_1.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_filesystem-vc90-mt-1_51.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_system-vc90-mt-1_51.dll %BIN_DIR
copy /Y ..\..\..\lib\boost\lib\boost_regex-vc90-mt-1_51.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_python-vc90-mt-1_51.dll %BIN_DIR%
copy /Y ..\..\..\core\smartbody\steersuite-1.3\build\win32\Release\steerlib.dll %BIN_DIR%
copy /Y ..\..\..\core\smartbody\steersuite-1.3\build\win32\Release\pprAI.dll %BIN_DIR%
copy /Y ..\..\..\core\smartbody\python27\libs9\python27.dll %BIN_DIR%
copy /Y ..\..\..\lib\pthreads\lib9\pthreadVSE2.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\apr\apr\lib9\libapr-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\apr\apr-iconv\lib9\libapriconv-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\apr\apr-util\lib9\libaprutil-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\activemq-cpp\vs2008-build\ReleaseDLL\activemq-cpp.dll %BIN_DIR%

if not exist ..\media\packs   mkdir ..\media\packs
if not exist ..\media\models  mkdir ..\media\models
copy /Y %OGRE_HOME%\media\packs\cubemap.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\cubemapsJS.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\dragon.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\fresneldemo.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\ogredance.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\ogretestmap.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\SdkTrays.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\Sinbad.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\skybox.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\models\cube.mesh    ..\media\models
copy /Y %OGRE_HOME%\media\models\sphere.mesh  ..\media\models

copy /Y %OGRE_BIN%\resources.cfg  %BIN_DIR%
copy /Y ..\src\config.ini %BIN_DIR%

xcopy /Y /Q /E /I /D %BIN_DIR% %FINAL_BIN_DIR%
xcopy /Y /Q /E /I /D ..\media %FINAL_MEDIA_DIR%

@goto END


:DEBUG_VARS
set BIN_DIR=..\bin\win32_debug
set PLUGINS_FILE=..\src\plugins-win32_debug.cfg
set OGRE_BIN=%OGRE_HOME%\bin9\debug
set FINAL_BIN_DIR=..\..\..\bin\ogre-viewer\bin\win32_debug
set FINAL_MEDIA_DIR=..\..\..\bin\ogre-viewer\media

copy /Y %PLUGINS_FILE% %BIN_DIR%\plugins.cfg

copy /Y %OGRE_BIN%\OgreMain_d.dll %BIN_DIR%
copy /Y %OGRE_BIN%\OIS_d.dll %BIN_DIR%
copy /Y %OGRE_BIN%\RenderSystem_Direct3D9_d.dll %BIN_DIR%
copy /Y %OGRE_BIN%\RenderSystem_GL_d.dll %BIN_DIR%

copy /Y ..\..\..\core\smartbody\SmartBody\lib\SmartBody_d.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\openal\libs9\Win32\alut.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\libsndfile\bin\libsndfile-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\openal\libs9\Win32\OpenAL32.dll %BIN_DIR%
copy /Y ..\..\..\lib\vhcl\openal\libs9\Win32\wrap_oal.dll %BIN_DIR%
copy /Y ..\..\..\lib\xerces-c\bin9\xerces-c_3_1D.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_filesystem-vc90-mt-gd-1_51.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_system-vc90-mt-gd-1_51.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_regex-vc90-mt-gd-1_51.dll %BIN_DIR%
copy /Y ..\..\..\lib\boost\lib\boost_python-vc90-mt-gd-1_51.dll %BIN_DIR%
copy /Y ..\..\..\core\smartbody\steersuite-1.3\build\win32\Debug\steerlibd.dll %BIN_DIR%
copy /Y ..\..\..\core\smartbody\steersuite-1.3\build\win32\Debug\pprAId.dll %BIN_DIR%
copy /Y ..\..\..\core\smartbody\python27\libs9\python27.dll %BIN_DIR%
copy /Y ..\..\..\lib\pthreads\lib9\pthreadVSE2.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\apr\apr\lib9\libapr-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\apr\apr-iconv\lib9\libapriconv-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\apr\apr-util\lib9\libaprutil-1.dll %BIN_DIR%
copy /Y ..\..\..\lib\activemq\activemq-cpp\vs2008-build\DebugDLL\activemq-cppd.dll %BIN_DIR%

if not exist ..\media\packs   mkdir ..\media\packs
if not exist ..\media\models  mkdir ..\media\models
copy /Y %OGRE_HOME%\media\packs\cubemap.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\cubemapsJS.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\dragon.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\fresneldemo.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\ogredance.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\ogretestmap.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\SdkTrays.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\Sinbad.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\packs\skybox.zip  ..\media\packs
copy /Y %OGRE_HOME%\media\models\cube.mesh    ..\media\models
copy /Y %OGRE_HOME%\media\models\sphere.mesh  ..\media\models

copy /Y %OGRE_BIN%\resources.cfg  %BIN_DIR%\resources.cfg
copy /Y ..\src\config.ini %BIN_DIR%

xcopy /Y /Q /E /I /D %BIN_DIR% %FINAL_BIN_DIR%
xcopy /Y /Q /E /I /D ..\media %FINAL_MEDIA_DIR%

@goto END


:ERROR
@echo ERROR in postbuild.bat: %ERROR_MSG%
@set ERRORLEV=1


:END
