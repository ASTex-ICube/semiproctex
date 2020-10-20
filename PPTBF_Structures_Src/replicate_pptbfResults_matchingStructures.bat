rem ***************************************************************
rem Reproduce visual structures from PPTBF parameters files
rem ***************************************************************

rem Data Path (user provided)
set PPTBF_DATA_PATH=Install\PPTBFProject\bin\Data\Matching_structures

rem go to current directory (for resource path)
set PPTBF_BIN_PATH=Install\PPTBFProject\bin
cd %~dp0%PPTBF_BIN_PATH%

rem Display BDD file names
rem forfiles /P %~dp0%PPTBF_DATA_PATH% /S /M *.txt /c "cmd /c echo @file"

rem Launch batch generation
rem - generated images are 400x400 pixels by default
forfiles /P %~dp0%PPTBF_DATA_PATH% /S /M *.txt /c "cmd /c %~dp0%PPTBF_BIN_PATH%\PtBDDGenerator.exe 400 400 @path"

rem go back to original ditrectory
cd %~dp0
