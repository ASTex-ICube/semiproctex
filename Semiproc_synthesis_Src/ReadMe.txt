-----------------------------------------------------------------------------------------------------------------
Technical paper:
Semi-Procedural Textures Using Point Process Texture Basis Functions
P. Guehl , R. AllEgre , J.-M. Dischler, B. Benes , and E. Galin
Computer Graphics Forum (EGSR 2020 special issue)
-----------------------------------------------------------------------------------------------------------------

NOTE : It works on Windows systems for the moment. (Linux port soon)

-----------------------------------------
[1] - Compile the project
-----------------------------------------

- launch script cmake_generateProjects.bat
- go to Generated directory
- open and compile SemiProcTexProject.sln Visual Studio solution
- all code header, lib and executable are copied to Install\SemiProcTexProject directory
=> ex: the semi-procedural texture synthesizer SptSemiProcTex.exe is in Install\SemiProcTexProject\bin directory

-----------------------------------------------------------------------------------------------------------------
Tool(s)
-----------------------------------------------------------------------------------------------------------------

-------------------------------------------------
---- [1] Semi-procedural texture synthesizer ----
-------------------------------------------------

---- Launching the synthesizer in command line ---- 

MANUAL PROCESSING

The SemiProcTexProject.exe software works by providing a semi-procedural texture file as parameter, such as :
C:\PPTBF\Bin\SemiProcTexProject.exe C:\PPTBF\Data\Matching_structures\cells\TexturesCom_FloorsRounded0112_S_v2_scrop_synthesis_params.txt
BEWARE: required data MUST be in same directory than parameter file :
- xxx_scrop.png (input exemplar)
- xxx_seg_scrop.png (binary structure of input exemplar)
- xxx_scrop_semiproctex_pptbf_params.txt (PPTBF parameter file of input exemplar)
- xxx_scrop_synthesis_params.txt (semi-procedural texture parameter file of input exemplar)
