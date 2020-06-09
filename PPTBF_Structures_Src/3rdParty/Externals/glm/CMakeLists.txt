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
message( STATUS "-------------------------------------" )
message( STATUS "- PROJECT GENERATION : GLM 3rdParty -" )
message( STATUS "-------------------------------------" )
message( STATUS "" )

# Project name
project( glmProject )

# Automatic cross-plateform compilation of glm

include( ExternalProject )

SET(
	glm_CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_DEBUG_POSTFIX=d
	-DBUILD_SHARED_LIBS=ON
	-DGLM_TEST_ENABLE=OFF
)

ExternalProject_Add(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 0.9.9.5
	GIT_SHALLOW ON
	INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../glm"
	BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/glm-build"
	CMAKE_ARGS ${CMAKE_ARGS} ${glm_CMAKE_ARGS}
	LOG_DOWNLOAD ON
    LOG_INSTALL ON
)
