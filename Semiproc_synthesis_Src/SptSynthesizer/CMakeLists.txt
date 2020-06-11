# Invite message
message( STATUS "----------------------------------------------" )
message( STATUS "- PROJECT GENERATION : SptSemiProcTex Module -" )
message( STATUS "----------------------------------------------" )
message( STATUS "" )

##################################################################################
# Copy 3rd party dependencies
##################################################################################

##################################################################################
# Copy resources
##################################################################################

##################################################################################
# Project
##################################################################################

# Project name
project( SptSemiProcTex )

# Set parallel build mode
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )

# Remove some compilation warnings
add_definitions( -D_CRT_SECURE_NO_WARNINGS ) 

##################################################################################
# Package Management
##################################################################################

##################################################################################
# Include directories
##################################################################################

# stb
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/stb" )

# hview
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/hview/include" )

##################################################################################
# Program
##################################################################################

# Retrieve source files
file( GLOB incList "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB inlList "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" )
file( GLOB srcList "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
# - shaders
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.vert" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.frag" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.geom" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.tese" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.tesc" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.comp" )

# Group files in IDE (Visual Studio)
source_group( "Include" FILES ${incList} )
source_group( "Inline" FILES ${inlList} )
source_group( "Source" FILES ${srcList} )
# - shaders
source_group( "Shaders" FILES ${shaderList} )

# Target program
# - source files
set( resList ${resList} ${incList} )
set( resList ${resList} ${inlList} )
set( resList ${resList} ${shaderList} )
# target
add_executable( ${PROJECT_NAME} ${srcList} ${resList} )

#####################################################
set( CUSTOM_DEBUG_POSTFIX "d" )
set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CUSTOM_DEBUG_POSTFIX} )
#####################################################

##################################################################################
# Linked libraries
##################################################################################

# Project property
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11 )

##################################################################################
# Custom commands
##################################################################################

# Copy library
#add_custom_command(TARGET ${PROJECT_NAME}
#           POST_BUILD
#           #COMMAND ${CMAKE_COMMAND} -E echo "Copy DLL"		   
#		   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../PtViewer/$<CONFIG>
#)

# Executables
install( TARGETS ${PROJECT_NAME} DESTINATION bin )
