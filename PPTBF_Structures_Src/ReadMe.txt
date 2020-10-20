-----------------------------------------------------------------------------------------------------------------
Technical paper:
Semi-Procedural Textures Using Point Process Texture Basis Functions
P. Guehl , R. AllEgre , J.-M. Dischler, B. Benes , and E. Galin
Computer Graphics Forum (EGSR 2020 special issue)
-----------------------------------------------------------------------------------------------------------------

NOTE : It works on Windows systems for the moment. (Linux port soon)
We have tested the softwares with the following graphics cards :
- NVidia GeForce 1060 GTX (6Go)
- NVidia RTX 2070 (8 Go)

On Windows, we rely on: CMake, Visual Studio, 7-zip file archiver and ".bat" scripts. Furthermore, the GLAD depency (see below) requires Python (CMake seems to find Python to process files).

-----------------------------------------
[1] - Compile 3rd party dependencies
-----------------------------------------

- go to 3rdParty directory

-------------------------
---- GLAD, GLFW, GLM ----

- go to Externals directory

- launch script build_glad.bat
  => BEWARE: choose your CMake generator in the script, exemple: cmake -G "Visual Studio 15 2017 Win64" ..\glad
  depending on your settings, the generator G could be replaced by: "Visual Studio 16 2019 Win64", "Visual Studio 14 2015 Win64", ...
  for more info, check CMake website (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#cmake-generators-7)
- go to glad-build directory
- open and compile gladProject.sln Visual Studio solution
=> NOTE : its better and simpler to use in Visual Studio menu : "Build" then "Batch Build" then check boxes for all "Debug" and "Release" projects to build all in Debug and Release modes, and install all files directly
=> we rely on CMake "ExternalProject_Add" feature to download libraries on-demand at compilation stage from their online github repositories

- Do the same for the scripts : build_glfw.bat and build_glad.bat

---------------
---- ImGui ----

- go back to 3rdParty directory
- unzip (here) the ImGui.7z
  => it requires the 7zip tool (available here: https://www.7-zip.org/)
- launch script cmake_generateProjects.bat
- go to Generated directory
- open and compile ImGui.sln Visual Studio solution

-----------------------------------------
[2] - Compile the PPTBF project (dll + tools)
-----------------------------------------

- go back to project root directory
- launch script cmake_generateProjects.bat
- go to Generated directory
- open and compile PPTBFProject.sln Visual Studio solution
- all code header, lib and executable are copied to Install\PPTBFProject directory
=> ex: the PPTBF viewer PtViewer.exe is in Install\PPTBFProject\bin directory
BEWARE: you MUST launch it with by prefexing name with current fullpath directory
=> ex: the script tool_launchViewer.bat at project root directory is made for that
=> instead, in command line, type for example : F:\PPTBF\Code\Install\PPTBFProject\bin\PtViewer.bat

-----------------------------------------------------------------------------------------------------------------
Tool(s)
-----------------------------------------------------------------------------------------------------------------

--------------------------------------
---- [1] Interactive PPTBF Viewer ----
--------------------------------------

The PtViewer.exe software enables to visualize and edit the PPTBF parameters at real-time.
=> Launch the script "tool_launchViewer.bat" at root directory

Scene objects:
- a) PPTBF
- b) visual structure is the thresholded version of the continuous PPTBF accrding to given threshold (use GUI widget "Binary Structure Map")
- c) 3D mesh onto which the PPTBF is mapped (PPTBF or thresholded version)

---- NAVIGATION control in 3D view ----

[1] CONTROL button
+ LEFT mouse   : object rotation
+ RIGHT mouse  : object translation
+ SCROLL mouse : object zoom/unzoom

[2] SHIFT button
+ LEFT mouse   : PPTBF rotation
+ RIGHT mouse  : PPTBF translation
+ SCROLL mouse : PPTBF zoom/unzoom

[3] 'R' key : reset 3D view

-----------------------------
---- [2] PPTBF GENERATOR ----
-----------------------------

---- Launching the PPTBF command line generator  ---- 

MANUAL PROCESSING

The PtBDDGenerator.exe software works by providing a PPTBF file as parameter and PPTBF width and height, such as :
C:\PPTBF\Bin\PtBDDGenerator.exe C:\PPTBF\Data\Matching_structures\cells\111357_seg_scrop\111357_seg_scrop_pptbf_params.txt 400 400
BEWARE: you MUST prefix the exe and file by full path
