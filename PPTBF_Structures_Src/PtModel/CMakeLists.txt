# Invite message
message( STATUS "-------------------------------------------" )
message( STATUS "- PROJECT GENERATION : PtModel Module -" )
message( STATUS "-------------------------------------------" )
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
project( PtModel )

# Set parallel build mode
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )

# Remove some compilation warnings
add_definitions( -D_CRT_SECURE_NO_WARNINGS ) 

# Shared library
add_definitions( -DPTMODEL_MAKEDLL )

##################################################################################
# Package Management
##################################################################################

##################################################################################
# Include directories
##################################################################################

# glm
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glm" )

# stb
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/stb" )

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
add_library( ${PROJECT_NAME} SHARED ${srcList} ${resList} )

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
install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib )
IF (WIN32)
ELSE()
install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION bin )
ENDIF()
install( FILES ${resList} DESTINATION include/PtModel )
#install( FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL )

#MACRO(testPascal pFile pPath)
#    copy_if_different ${pFile} ${pPath}
#endMACRO()
#set(no_copy $<CONFIG:Debug>)
#add_custom_command(TARGET ${PROJECT_NAME}
#           POST_BUILD
#		   COMMAND "${CMAKE_COMMAND}" -E
#		   # do nothing for non-Release build
#		   $#<${no_copy}:echo>    $<${no_copy}:"copy omitted for non-release build, command would have been ">
#		   # copy file for release build
#		   $<${no_copy}:copy_if_different> $<${no_copy}:testPascal($<TARGET_PDB_FILE:${PROJECT_NAME}> "D:/PhD/Projects/PPTBF")
#)

#set(no_copy $<CONFIG:Release>)
#add_custom_command(TARGET myDLL POST_BUILD
#    COMMAND "${CMAKE_COMMAND}" -E
#    # do nothing for non-Release build
#    $<${no_copy}:echo>    $<${no_copy}:"copy omitted for non-release build, command would have been ">
#    # copy file for release build
#    copy_if_different $<TARGET_FILE:myDLL> ${DIR_FOR_DLL})
