# Check CMAKE version
cmake_minimum_required( VERSION 3.0 )

# Invite message
message( STATUS "-------------------------------------" )
message( STATUS "- PROJECT GENERATION : hview Module -" )
message( STATUS "-------------------------------------" )
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
project( hview )

set( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../../hview" )

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

##################################################################################
# Program
##################################################################################

# Retrieve source files
file( GLOB incList "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB srcList "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )

# Group files in IDE (Visual Studio)
source_group( "Include" FILES ${incList} )
source_group( "Source" FILES ${srcList} )

# Target program
# - source files
set( resList ${resList} ${incList} )
# target
add_library( ${PROJECT_NAME} STATIC ${srcList} ${resList} )

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
#install( TARGETS ${PROJECT_NAME} DESTINATION bin )
# Libraries
install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib )
install( FILES ${resList} DESTINATION include )
#install( FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL )

