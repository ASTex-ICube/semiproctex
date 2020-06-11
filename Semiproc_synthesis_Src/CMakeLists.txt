#----------------------------------------------------------------
# SemiProcTex PROJECT CMake file
# Main user file
#----------------------------------------------------------------

# Check CMAKE version
cmake_minimum_required( VERSION 3.0 )

#----------------------------------------------------------------
# Project
#----------------------------------------------------------------

# Invite message
message( STATUS "--------------------------------------------" )
message( STATUS "- PROJECT GENERATION : SemiProcTex Library -" )
message( STATUS "--------------------------------------------" )
message( STATUS "" )

# Project name
project( SemiProcTexProject )

set( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Install/SemiProcTexProject" )

# Add PATH variables to be retrieved in source code
set( PT_PROJECT_PATH "${CMAKE_SOURCE_DIR}/.." )
set( PT_DATA_PATH "${PT_PROJECT_PATH}/Data" )
set( PT_IMAGE_PATH "${PT_DATA_PATH}/Images" )
set( PT_SHADER_PATH "${PT_DATA_PATH}/Shaders" )
get_filename_component( PT_PROJECT_PATH "${CMAKE_SOURCE_DIR}/.." ABSOLUTE )
get_filename_component( PT_DATA_PATH "${PT_PROJECT_PATH}/Data" ABSOLUTE )
get_filename_component( PT_IMAGE_PATH "${PT_DATA_PATH}/Images" ABSOLUTE )
get_filename_component( PT_SHADER_PATH "${PT_DATA_PATH}/Shaders" ABSOLUTE )
add_definitions( -DPT_PROJECT_PATH="${PT_PROJECT_PATH}" )
add_definitions( -DPT_DATA_PATH="${PT_DATA_PATH}" )
add_definitions( -DPT_IMAGE_PATH="${PT_IMAGE_PATH}" )
add_definitions( -DPT_SHADER_PATH="${PT_SHADER_PATH}" )

#----------------------------------------------------------------
# Add Library/Tools path
#----------------------------------------------------------------

# SDK / API
add_subdirectory( "${CMAKE_SOURCE_DIR}/SptSynthesizer" )
