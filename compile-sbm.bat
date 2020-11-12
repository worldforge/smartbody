
setlocal


@rem Windows VS2010 build

call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
@rem vcvarsall turns echo off and doesn't turn it back on, shame on them
@echo on

devenv vs2010.sln /build Debug
devenv vs2010.sln /build Release


@rem Android build

set NDK_EXE="c:\android-ndk-r8e\ndk-build.cmd"
if defined ANDROID_NDK set NDK_EXE="%ANDROID_NDK%\ndk-build.cmd"

pushd android\vh_wrapper\jni

call %NDK_EXE% -j 8

@rem ndk-build turns echo off and doesn't turn it back on, shame on them
@echo on

popd


endlocal
