@setlocal
@pushd ..
@call postbuild9.bat %*
@REM Result code pass through
@set ERRORLEVEL=%ERRORLEV%
@popd
