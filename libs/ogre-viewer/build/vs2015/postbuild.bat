@setlocal
@pushd ..
@call postbuild.bat %*
@REM Result code pass through
@set ERRORLEVEL=%ERRORLEV%
@popd
