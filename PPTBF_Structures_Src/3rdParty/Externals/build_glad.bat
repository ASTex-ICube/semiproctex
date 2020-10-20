mkdir glad-build
cd glad-build
rem Call the script selecting the CMAKE compiler (Visual Studio 2010, 2012, etc...)
rem "Visual Studio 14 2015 Win64"
rem "Visual Studio 15 2017 Win64"
rem "Visual Studio 16 2019 Win64"
cmake -G "Visual Studio 15 2017 Win64" ..\glad
