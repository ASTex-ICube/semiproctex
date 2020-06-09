# Invite message
message( STATUS "------------------------------------------" )
message( STATUS "- PROJECT GENERATION : PtGraphics Module -" )
message( STATUS "------------------------------------------" )
message( STATUS "" )

##################################################################################
# Copy 3rd party dependencies
##################################################################################

# Graphics
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/bin/GsGraphicsd.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug )
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/bin/GsGraphics.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel )
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/bin/GsGraphics.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release )
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/bin/GsGraphics.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo )

##################################################################################
# Copy resources
##################################################################################

# Shaders
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/data/Shaders/*.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug )
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/data/Shaders/*.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel )
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/data/Shaders/*.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release )
#file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GsGraphics/data/Shaders/*.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo )

##################################################################################
# Project
##################################################################################

# Project name
project( PtGraphics )

# Set parallel build mode
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )

# Remove some compilation warnings
add_definitions( -D_CRT_SECURE_NO_WARNINGS ) 

# Shared library
add_definitions( -DPTGRAPHICS_MAKEDLL )

# GLFW
add_definitions( -DGLFW_DLL )

##################################################################################
# Package Management
##################################################################################

# OpenGL
find_package( OpenGL REQUIRED )

##################################################################################
# Include directories
##################################################################################

# PtDataModel
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../PtModel" )

# glad
#include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glad/include" )
set( glad_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glad/lib/cmake/glad" )
find_package( glad CONFIG REQUIRED )

# glm
#include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glm" )
set( glm_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glm/lib/cmake/glm" )
find_package( glm CONFIG REQUIRED )
include_directories( ${GLM_INCLUDE_DIRS} )

# stb
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/stb" )

# # GLFW
# set( glfw3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GLFW/lib/cmake/glfw3" )
# #find_package( glfw3 3.2 CONFIG REQUIRED PATH_SUFFIXES "/lib/cmake/glfw3" )
# find_package( glfw3 3.2 CONFIG REQUIRED )
# file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GLFW/lib/glfw3.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug )
# file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/GLFW/lib/glfw3.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release )
# glfw
set( glfw3_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glfw/lib/cmake/glfw3 )
find_package( glfw3 CONFIG REQUIRED )

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

# Graphics
target_link_libraries( ${PROJECT_NAME} ${OPENGL_gl_LIBRARY} )

# PtDataModel
target_link_libraries( ${PROJECT_NAME} PtModel )

# GLFW
target_link_libraries( ${PROJECT_NAME} glfw )

# glad
# set( GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glad" )
# add_library( "glad" SHARED "${GLAD_DIR}/src/glad.c" )
# target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
# target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
# target_compile_definitions( glad PUBLIC GLAD_GLAPI_EXPORT PRIVATE GLAD_GLAPI_EXPORT_BUILD )
# target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")
#target_link_libraries( ${PROJECT_NAME} glad "${CMAKE_DL_LIBS}" )
#target_link_libraries( ${PROJECT_NAME} debug "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/Externals/lib/MultiScaleLabelMapExtractiond.lib" optimized "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/MultiScaleLabelMapExtraction/Release/lib/MultiScaleLabelMapExtraction.lib" )

# find_library( PT_GLAD_LIBRARY
    # NAMES glad
    # HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/Externals/lib"
# )
# #message( STATUS ${PT_GLAD_LIBRARY} )
# target_compile_definitions( ${PROJECT_NAME} PUBLIC GLAD_GLAPI_EXPORT )
# target_link_libraries( ${PROJECT_NAME} ${PT_GLAD_LIBRARY} "${CMAKE_DL_LIBS}" )

# glad
target_link_libraries( ${PROJECT_NAME} glad::glad )

# dlopen/dlclose
target_link_libraries( ${PROJECT_NAME} ${CMAKE_DL_LIBS} )

# Project property
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11 )

##################################################################################
# Custom commands
##################################################################################

# Libraries
install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib ) 
install( FILES ${resList} DESTINATION include/PtGraphics )

# Copy library
#add_custom_command(TARGET ${PROJECT_NAME}
#           POST_BUILD
#           #COMMAND ${CMAKE_COMMAND} -E echo "Copy DLL"		   
#		   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../PtViewer/$<CONFIG>
#)

# Copy library
#add_custom_command(TARGET ${PROJECT_NAME}
 #          POST_BUILD
  #         COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/../x64
	#	   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/x64/.
		  # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_LINKER_FILE:${PROJECT_NAME}> ${GS_RELEASE_LIB_DIR}/.
		  # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${GS_RELEASE_BIN_DIR}/.
#)
