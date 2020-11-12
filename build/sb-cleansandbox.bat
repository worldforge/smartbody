
@setlocal

@rem look for python executable.  First look in the path, then look in standard locations.  Override is available.
where /q python.exe
if %ERRORLEVEL% EQU 0 set PY_EXE=python.exe
where /q mayapy.exe
if %ERRORLEVEL% EQU 0 set PY_EXE=mayapy.exe
if not defined PY_EXE set PY_EXE="%ProgramW6432%\Autodesk\Maya2012\bin\mayapy.exe"
if defined MAYAINSTALLPATH2012_64 set PY_EXE="%MAYAINSTALLPATH2012_64%\bin\mayapy.exe"

%PY_EXE% sb-automated-build-prepare.py svn://svn.code.sf.net/p/smartbody/code/trunk/build

%PY_EXE% scripts/sb-automated-build.py cleansandbox

@endlocal
