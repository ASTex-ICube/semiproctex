-----------------------------------------------------------------------------------------------------------------
Technical paper:
Semi-Procedural Textures Using Point Process Texture Basis Functions
P. Guehl , R. AllEgre , J.-M. Dischler, B. Benes , and E. Galin
Computer Graphics Forum (EGSR 2020 special issue)
-----------------------------------------------------------------------------------------------------------------

NOTE : It works on Windows systems for the moment. (Linux port soon)

-----------------------------------------
[1] - Compile 3rd party dependencies
-----------------------------------------

- go to 3rdParty directory

---------------
---- hview ----

- go to Externals directory

- launch script build_hview.bat
  => BEWARE: choose your CMake generator in the script, exemple: cmake -G "Visual Studio 15 2017 Win64" ..\hview
- go to hview-build directory
- open and compile hview.sln Visual Studio solution
=> NOTE : its better and simpler to use in Visual Studio menu : "Build" then "Batch Build" then check boxes for all "Debug" and "Release" projects to build all in Debug and Release modes
- the "hview" library is automatically installed in the "3rdParty\hview" directory

-----------------------------------------
[2] - Compile the project
-----------------------------------------

- launch script cmake_generateProjects.bat
- go to Generated directory
- open and compile SemiProcTexProject.sln Visual Studio solution
- all code header, lib and executable are copied to Install\SemiProcTexProject directory
=> ex: the semi-procedural texture synthesizer SptSemiProcTex.exe is in Install\SemiProcTexProject\bin directory

-----------------------------------------------------------------------------------------------------------------
Tool(s)
-----------------------------------------------------------------------------------------------------------------

There are two software/CPU versions of our semi-procedural texture synthesizers:
- BASIC (AVAILABLE)
- FULL (WORK IN PROGRESS...)

Currently, switch is done in the "main.cpp" file with the help of the macro:
#define _USE_BASIC_SYNTHESIZER_

-------------------------------------------------------
---- [1] BASIC Semi-procedural texture synthesizer ----
-------------------------------------------------------

---- Launching the synthesizer in command line ---- 

MANUAL PROCESSING

The SemiProcTexProject.exe software works by providing a semi-procedural texture file as parameter, such as :
SemiProcTexProject.exe textureName GUIDE STRENGTH INITLEVEL BLOCSIZE INITERR INDEXWEIGHT
BEWARE: required data MUST be in same directory than parameter file :
- xxx_scrop.png (input exemplar)
- xxx_seg_scrop.png (binary structure of input exemplar)
- xxx_seg_scrop_pptbf_params.txt (PPTBF parameter file of input exemplar)

Example:
SemiProcTexProject.exe cracked_asphalt_160796 0.9 0.5 2 64 100 0.0

------------------------------------------------------
---- [2] FULL Semi-procedural texture synthesizer ----
------------------------------------------------------

WORK IN PROGRESS...

---- Launching the synthesizer in command line ---- 

MANUAL PROCESSING

The SemiProcTexProject.exe software works by providing a semi-procedural texture file as parameter, such as :
C:\PPTBF\Bin\SemiProcTexProject.exe C:\PPTBF\Data\Matching_structures\cells\TexturesCom_FloorsRounded0112_S_v2_scrop_synthesis_params.txt
BEWARE: required data MUST be in same directory than parameter file :
- xxx_scrop.png (input exemplar)
- xxx_seg_scrop.png (binary structure of input exemplar)
- xxx_scrop_semiproctex_pptbf_params.txt (PPTBF parameter file of input exemplar)
- xxx_scrop_synthesis_params.txt (semi-procedural texture parameter file of input exemplar)
