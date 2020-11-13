# Overview

Here you can find information to compile source code from scratch.

# Requirements

## OS [Operating Systems]

Currently, it works on ***Windows*** operating systems. (Linux port soon).

NOTE: For running all the *.bat* scripts, you can either double-click on the file or use the *Command Prompt* on Windows OS. On the command line in the respective folder, where the file was found, just type the file name and execute it.

## Hardware Requirements

We have tested the softwares with the following graphics cards :
- NVIDIA: GeForce GTX 1060, GeForce GTX 1080, RTX 2070 and Quadro P5000.

## Software Requirements

On Windows, we rely on: CMake, Git, Visual Studio (e.g 2015, 2017, 2019), 7-zip file archiver and ".bat" scripts.

Links for installation information of required dependencies can be found below :

### File archiver

- **7zip**: file archiver with a high compression ratio
  - https://www.7-zip.org/
  - on webpage, download the *64-bit x64* installer for instance: *7z1900-x64.exe*. This will install the software and add path into the Windows PATH environment variable (select this choice of adding to PATH if it asks).

### Build system generator

- **CMake**: cross-platform, compiler-independent build system generator
  - https://cmake.org/download/
  - on webpage, search for *Release Candidate*, *Binary distributions*, *Platform* then download the *Windows win64-x64 Installer*, for instance: *cmake-3.19.0-rc3-win64-x64.msi*. This will install the software and add path into the Windows PATH environment variable (select this choice of adding to PATH if it asks).
  
- **Git**: free and open source revision control software
  - NOTE: We rely on CMake "ExternalProject_Add" feature to download all dependency libraries (GLAD, GLFW and GLM) on-demand at compilation stage by cloning from their online github repositories. So, one needs to install Git on Windows otherwise there will be an error during their cloning step. 
  - https://git-scm.com/download/win
  - on webpage, download the *64-bit version of Git for Windows* installer, for instance: *Git-2.29.2.2-64-bit.exe*. This will install the software and add path into the Windows PATH environment variable (select this choice of adding to PATH if it asks).

### Compiler, IDE (Integrated development environment)

CMake uses *generators* (with ***-G*** option) to select a compiler to generate a project environment. For instance, it could be a *makefile* on Linux or a Visual Studio *solution*. For more info on *generators*, check CMake website (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#cmake-generators-7).
- for instance, to select *Visual Studio 2017 x64 version*, one can use: *cmake -G "Visual Studio 15 2017 Win64" ..\glad*. But, by default, not using a dedicated generator CMake will find the first available generator in the PATH: *cmake -G ..\glad*

- **Visual Studio**: IDE
  - https://visualstudio.microsoft.com/fr/downloads/
  - this will install the software and add path into the Windows PATH environment variable (select this choice of adding to PATH if it asks).
  - IMPORTANT: we tried the 2015, 2017 and 2019 versions. By default, the 2017 is selected in the CMake command in our *.bat* scripts files (change it to correct value or remove the *-G* option and the name of the generator just after). We use this because CMake sometimes uses the x86 of Visual Studio instead of the x64 version. For instance, in our *.bat* script files select your generator (we use commenting with the *rem* DOS command):
```
  rem set GS_COMPILER="Visual Studio 14 2015 Win64"
  set GS_COMPILER="Visual Studio 15 2017 Win64"
  rem set GS_COMPILER="Visual Studio 16 2019 Win64"
  cmake -G %GS_COMPILER% ..
```
  - as mentioned, removing the *-G* option and its generator name *should work*, and CMake will look at your (latest installed) compiler/IDE in PATH environment variable:
```
  cmake ..
```

### Other dependency

- **Python**: through **Anaconda** installer
  - https://www.anaconda.com/products/individual
  - Anaconda is a really simple option on Windows, with which you can install several Python version localy.
  - IMPORTANT: try to not install it first. It seems that CMake, while compiling GLAD, try to use Python to modify some GLAD text files. If you have an error while compiling GLAD. Check console errors or log window then install Python only if required.

# Compilation

## [Step 1] Compile 3rd party dependencies

First, go to ***3rdParty*** directory

### GLAD, GLFW, GLM

- go to ***Externals*** directory

- launch ***script build_glad.bat***
  - BEWARE: choose your CMake generator in the script, exemple: cmake -G "Visual Studio 15 2017 Win64" ..\glad
  depending on your settings, the generator G could be replaced by: "Visual Studio 16 2019 Win64", "Visual Studio 14 2015 Win64", ...
  for more info, check CMake website (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#cmake-generators-7)
- go to ***glad-build*** directory
- open and compile ***gladProject.sln*** Visual Studio solution
  - NOTE: its better and simpler to use in Visual Studio menu : "Build" then "Batch Build" then check boxes for all "Debug" and "Release" projects to build all in Debug and Release modes, and install all files directly

- Do the same for the scripts : ***build_glfw.bat*** and ***build_glm.bat***

### ImGui

- go back to ***3rdParty*** directory
- unzip (here) the ***ImGui.7z***
  - it requires the 7zip tool (available here: https://www.7-zip.org/)
- launch script ***cmake_generateProjects.bat***
  - BEWARE: choose your CMake generator in the script, exemple: cmake -G "Visual Studio 15 2017 Win64" ..
  depending on your settings, the generator G could be replaced by: "Visual Studio 16 2019 Win64", "Visual Studio 14 2015 Win64", ...
  for more info, check CMake website (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#cmake-generators-7)
- go to ***Generated*** directory
- open and compile ***ImGui.sln*** Visual Studio solution
  - NOTE: its better and simpler to use in Visual Studio menu : "Build" then "Batch Build" then check boxes for all "Debug" and "Release" projects to build all in Debug and Release modes, and install all files directly

## [Step 2] Compile the PPTBF project (dll + tools)

- go back to the *project root directory*
- launch script ***cmake_generateProjects.bat***
  - BEWARE: choose your CMake generator in the script, exemple: cmake -G "Visual Studio 15 2017 Win64" ..
  depending on your settings, the generator G could be replaced by: "Visual Studio 16 2019 Win64", "Visual Studio 14 2015 Win64", ...
  for more info, check CMake website (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#cmake-generators-7)
- go to ***Generated*** directory
- open and compile ***PPTBFProject.sln*** Visual Studio solution
  - NOTE: its better and simpler to use in Visual Studio menu : "Build" then "Batch Build" then check boxes for all "Debug" and "Release" projects to build all in Debug and Release modes, and install all files directly
- all code header, lib and executable are copied to ***Install\PPTBFProject*** directory, for instance: the PPTBF viewer ***PtViewer.exe*** is in ***Install\PPTBFProject\bin*** directory
- BEWARE: you MUST launch it with by prefexing name with current fullpath directory
  - ex: the script ***tool_launchViewer.bat*** at project root directory is made for that
  - instead, in command line, type for example : F:\PPTBF\Code\Install\PPTBFProject\bin\PtViewer.bat

# Tools

## [1] Interactive PPTBF Viewer

The ***PtViewer.exe*** software enables to visualize and edit the PPTBF parameters at real-time ((PPTBF is a model of stochastic procedural structures).

Launch the script ***tool_launchViewer.bat*** at root directory.

Info on displayed scene objects in the viewer:
- a) PPTBF (model of stochastic procedural structures in grayscale)
- b) visual structure is the thresholded version of the continuous PPTBF according to given threshold (use GUI widget "Binary Structure Map")
- c) 3D mesh onto which the PPTBF is mapped (PPTBF or thresholded version)

There are 3 widgets to play with :
- PPTBF Designer
- Rendering Parameters
- Binary Structure Map

NOTE: ImGui GUI system uses a ***imgui.ini*** parameter file located in the same directory than the ***PtViewer.exe*** software. It contains the location of each of the 2D widgets on the screen. If one of the widget has disappeared, just delete the ***imgui.ini*** file to reinitialize the default state. If one of the widget is located behind an overlay window info and cannot be clicked anymore, delete the ***imgui.ini*** file. For instance, for the *Rendering Parameters* widget, you could have for its 2D position *Pos*:
- [Rendering Parameters]
- Pos=301,20

### WIDGET: PPTBF Designer

This widget is used to edit the PPTBF parameters.

#### Global Settings

You can choose the resolution of the PPTBF image used for computing. By default 512x512 pixels.

#### PPTBF PARAMETERS edition

Below, you can find short videos showing PPTBF parameters edition at real-time with the ***PtViewer.exe*** software:

##### Point Process
- tilling types: https://vimeo.com/434859580
- jittering: https://vimeo.com/434859687

##### Window Function
- non-overlapping windows: https://vimeo.com/434859823
- overlapping windows: https://vimeo.com/434860045

##### Feature Function
- anisotropic stringed Gabor kernels: https://vimeo.com/434860110
- bombing & voronoise: https://vimeo.com/434860370

##### Model Transform and Deformations
- spatial trasformations (zoom, rotation, stretch), deformation and navigation (translation): https://vimeo.com/434869887

### WIDGET: Binary Structure Map

This widget is used to edit the thresholded version of the PPTBF which creates a binary structure map (i.e. visual structure).

- *BinaryHistograms* checkbox is used to activate the thresholding of the PPTBF and display a binary version of the PPTBF at the bottom right of the screen.
- *Nb bins*: is the precision of the histogram of the values of the PPTBF image (number of histogram bins).
- *Threshold* slider is a value between 0% and 100% whose real associated PPTBF value is displayed below near the *Binary structure map* text. PPTBF values are in [0;1]. Every value greater or equal than the thresholded value is in the binary structure.

Below, you can find a short video showing edition at real-time with the ***PtViewer.exe*** software:
- rendering binary map: https://vimeo.com/434869996
  - NOTE: for the provided code, but not the video, it seems that the thresholded version of the PPTBF (displayed at the bottom right of the viewer window) is not affected by the thresholded value but the 3D model onto which texture is mapped is OK.

### WIDGET: Rendering Parameters

This widget is used to edit the 3D model onto which the PPTBF is mapped.

- Mesh Settings: choose a 3D model (grid, wave, cylinder, torus or sphere)
- Rendering Mode: either display the PPTBF is grayscale or its binary version (after applygin current *threshold* value)

### NAVIGATION control in 3D view

[1] CONTROL button
+ LEFT mouse   : object rotation
+ RIGHT mouse  : object translation
+ SCROLL mouse : object zoom/unzoom

[2] SHIFT button
+ LEFT mouse   : PPTBF rotation
+ RIGHT mouse  : PPTBF translation
+ SCROLL mouse : PPTBF zoom/unzoom

[3] 'R' key : reset 3D view

## [2] PPTBF GENERATOR

The ***PtBDDGenerator.exe*** software enables to launching the PPTBF command line generator.

### MANUAL PROCESSING

The ***PtBDDGenerator.exe*** software works by providing the desierd PPTBF width and height then a PPTBF file as parameter, such as :
- **```program width height xxx_pptbf_params.txt```**
- ```C:\PPTBF\Bin\PtBDDGenerator.exe 400 400 C:\PPTBF\Data\Matching_structures\cells\111357_seg_scrop\111357_seg_scrop_pptbf_params.txt```
- BEWARE: you MUST prefix the exe and file by full path

### BATCH PROCESSING

The following batch file is used to generate all the PPTBF images and their binary versions:
- ***replicate_pptbfResults_matchingStructures.bat***
- for all PPTBF parameter files located in the ***PPTBF_Structures_Src/Install/PPTBFProject/bin/Data/Matching_structures/*** directory, classified by types (cells, dots, lines, networks, etc...), the generated images will be saved near the parameter files (same directory).
