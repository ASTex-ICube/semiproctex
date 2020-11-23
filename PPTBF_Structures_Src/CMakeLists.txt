#----------------------------------------------------------------
# PPTBF PROJECT CMake file
# Main user file
#----------------------------------------------------------------

# Check CMAKE version
cmake_minimum_required( VERSION 3.0 )

#----------------------------------------------------------------
# Project
#----------------------------------------------------------------

# Invite message
message( STATUS "-------------------------------------------" )
message( STATUS "- PROJECT GENERATION : PPTBF Library -" )
message( STATUS "-------------------------------------------" )
message( STATUS "" )

# Project name
project( PPTBFProject )

set( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Install/PPTBFProject" )

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
add_subdirectory( "${CMAKE_SOURCE_DIR}/PtModel" )
add_subdirectory( "${CMAKE_SOURCE_DIR}/PtGraphics" )

# Tools
add_subdirectory( "${CMAKE_SOURCE_DIR}/Tools" )

# 3rdParty
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/Externals/bin
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject
    FILES_MATCHING
    PATTERN "*.dll"
)
# glad
IF (WIN32)
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glad/bin/glad.dll DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glad/bin/gladd.dll DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
ELSE()
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glad/lib/libglad.so DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
ENDIF()
# GLFW
IF (WIN32)
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw/bin/glfw3.dll DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw/bin/glfw3d.dll DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
ELSE()
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw/lib/libglfw.so DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
file(GLOB PROJECT_GLFW_LIBRARY
  "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw/lib/*so*"
)
file( COPY ${PROJECT_GLFW_LIBRARY} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Install/PPTBFProject/bin )
ENDIF()
