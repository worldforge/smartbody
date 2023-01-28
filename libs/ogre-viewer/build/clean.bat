@REM OgreViewer clean.bat
@REM Carefully removes files created by build scripts
@REM Assumes is it is run from ogre-viewer/build directory.

@setlocal

if not exist ..\bin (goto END)

:CLEAN_WIN32_RELEASE
@REM Binary directory for 
if not exist ..\bin\win32 (goto CLEAN_WIN32_DEBUG)
pushd ..\bin\win32
del OgreViewer.exe OgreViewer.pdb OgreViewer.ilk
del OgreMain.dll OIS.dll
del RenderSystem_Direct3D9.dll RenderSystem_GL.dll
del resources.cfg plugins.cfg
rmdir /S /Q media
popd
@REM attempt to remove directory if empty
rmdir ..\bin\win32

:CLEAN_WIN32_DEBUG
if not exist ..\bin\win32_debug (goto END)
pushd ..\bin\win32_debug
del OgreViewer_d.exe OgreViewer_d.pdb OgreViewer_d.ilk
del OgreMain_d.dll OIS_d.dll
del RenderSystem_Direct3D9_d.dll RenderSystem_GL_d.dll
del resources.cfg plugins.cfg
rmdir /S /Q media
popd
@REM attempt to remove directory if empty
rmdir ..\bin\win32_debug

:CLEAN_BIN
rmdir ..\bin

:END
