@echo off

@REM check if we already have the tools in the environment
if exist "%VCINSTALLDIR%" (
	goto compile
)

@REM Check for Visual Studio
if exist "%VS90COMNTOOLS%" (
	set VSPATH="%VS90COMNTOOLS%"
	goto set_env
)
if exist "%VS80COMNTOOLS%" (
	set VSPATH="%VS80COMNTOOLS%"
	goto set_env
)

echo You need Microsoft Visual Studio 8 or 9 installed
pause
exit

@ setup the environment
:set_env
call %VSPATH%vsvars32.bat

:compile
bam-bin.exe -j 4 %*