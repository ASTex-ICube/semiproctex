@Echo off

rem ***************************************
rem CURRENT WORKING DIRECTORY
rem ***************************************

rem - working directory
cd Install\PPTBFProject\bin

rem - data path
SET PT_PROJECT_BIN_PATH=%~dp0
SET PT_PROJECT_PATH=%PT_PROJECT_BIN_PATH%/..
SET PT_DATA_PATH=%PT_PROJECT_PATH%/Data
SET PT_IMAGE_PATH=%PT_DATA_PATH%/Images
SET PT_SHADER_PATH=%PT_DATA_PATH%/Shaders

rem ***************************************
rem USER CUSTOMIZABLE PARAMETERS
rem ***************************************

rem ***************************************
rem LAUNCH PROGRAM
rem ***************************************

@Echo off

%~dp0Install/PPTBFProject/bin/PtViewerd.exe

@Echo off

rem ***************************************
rem CURRENT WORKING DIRECTORY
rem ***************************************

cd ..\..\..
