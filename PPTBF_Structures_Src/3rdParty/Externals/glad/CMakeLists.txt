#----------------------------------------------------------------
# PROJECT CMake file
# Main user file
#----------------------------------------------------------------

# Check CMAKE version
cmake_minimum_required( VERSION 3.0 )

#----------------------------------------------------------------
# Project
#----------------------------------------------------------------

# Invite message
message( STATUS "--------------------------------------" )
message( STATUS "- PROJECT GENERATION : GLAD 3rdParty -" )
message( STATUS "--------------------------------------" )
message( STATUS "" )

# Project name
project( gladProject )

# Automatic cross-plateform compilation of glad

include( ExternalProject )

SET( BUILD_SHARED_LIBS ON )

SET(
	glad_CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_DEBUG_POSTFIX=d
	-DGLAD_PROFILE="core"
	-DGLAD_API="gl=4.6"
	-DGLAD_ALL_EXTENSIONS=ON
	-DGLAD_INSTALL=ON
	-DGLAD_EXPORT=ON
	-DBUILD_SHARED_LIBS=ON
)

ExternalProject_Add(
	glad
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git
	#GIT_TAG v0.1.33
	GIT_SHALLOW ON
	INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../glad"
	BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/glad-build"
	CMAKE_ARGS ${CMAKE_ARGS} ${glad_CMAKE_ARGS}
)

