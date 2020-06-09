@echo off

rem **************************************************************************
rem USER : Choose your compiler
rem **************************************************************************

rem Call the script selecting the CMAKE compiler (Visual Studio 2010, 2012, etc...)
rem set GS_COMPILER="Visual Studio 14 2015 Win64"
set GS_COMPILER="Visual Studio 15 2017 Win64"

rem **************************************************************************
rem CMAKE GENERATION
rem **************************************************************************

set CURRENTSCRIPTPATH=%CD%

mkdir Generated
cd Generated
cmake -G %GS_COMPILER% ..

rem exit
if NOT ERRORLEVEL 0 pause
cd %CURRENTSCRIPTPATH%
pause
