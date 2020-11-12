
cd files
for %%i in (*.txt) do (
	echo %%i
	..\Tcl80\bin\tclsh80.exe ..\fa\gen_pronun.tcl ..\files\%%~ni.txt %%~ni.ntp
	..\Tcl80\bin\tclsh80.exe ..\fa\fa_new_multichan.tcl ..\files\%%~ni.wav %%~ni.ntp ..\files\%%~ni.txt1
	move /Y %%~ni.ntp ..\files
)
cd..
cd converter
.\converter ..\files ..\mapping.txt
cd..
