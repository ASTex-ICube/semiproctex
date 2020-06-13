@Echo off

rem SptBasicSynthesizer is a basic synthesizer that only requires 3 files:
rem - an input exemplar name xxx_scrop.png
rem - an input segmented exemplar name xxx_seg_scrop.png
rem - a pptbf parameters file xxx_seg_scrop_pptbf_params.txt
rem where xxx is texture name.

rem ***************************************
rem CURRENT WORKING DIRECTORY
rem ***************************************

rem - working directory
cd Install\SemiProcTexProject\bin

rem - data path
SET PT_PROJECT_BIN_PATH=%~dp0
SET PT_PROJECT_PATH=%PT_PROJECT_BIN_PATH%/..
SET PT_DATA_PATH=%PT_PROJECT_PATH%/Data
SET PT_IMAGE_PATH=%PT_DATA_PATH%/Images
SET PT_SHADER_PATH=%PT_DATA_PATH%/Shaders

rem ***************************************
rem USER CUSTOMIZABLE PARAMETERS
rem ***************************************

rem Input exemplar texture name
SET TEXURE_NAME=cracked_asphalt_160796

rem Semi-procedural texture synthesis parameter
SET GUIDE=0.9
SET STRENGTH=0.5
SET INITLEVEL=2
SET BLOCSIZE=64
SET INITERR=100
SET INDEXWEIGHT=0

rem ***************************************
rem LAUNCH PROGRAM
rem ***************************************

@Echo off

rem %~dp0Install/SemiProcTexProject/bin/SptSemiProcTex.exe %TEXURE_NAME% %GUIDE% %STRENGTH% %INITLEVEL% %BLOCSIZE% %INITERR% %INDEXWEIGHT%
SptSemiProcTex.exe %TEXURE_NAME% %GUIDE% %STRENGTH% %INITLEVEL% %BLOCSIZE% %INITERR% %INDEXWEIGHT%

@Echo off

rem ***************************************
rem CURRENT WORKING DIRECTORY
rem ***************************************

cd ..\..\..
