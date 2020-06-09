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
message( STATUS "- PROJECT GENERATION : GLFW 3rdParty -" )
message( STATUS "--------------------------------------" )
message( STATUS "" )

# Project name
project( glfwProject )

# Automatic cross-plateform compilation of glfw

include( ExternalProject )

SET( BUILD_SHARED_LIBS ON )

SET(
	glfw_CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_DEBUG_POSTFIX=d
	-DBUILD_SHARED_LIBS=ON
	-DGLFW_BUILD_EXAMPLES=OFF
	-DGLFW_BUILD_TESTS=OFF
	-DGLFW_BUILD_DOCS=OFF
)

ExternalProject_Add(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.3
	GIT_SHALLOW ON
	INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../glfw"
	BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/glfw-build"
	CMAKE_ARGS ${CMAKE_ARGS} ${glfw_CMAKE_ARGS}
)

