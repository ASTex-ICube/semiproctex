# Invite message
message( STATUS "-----------------------------------------------------" )
message( STATUS "- PROJECT GENERATION : PtBDDGenerator Tool -" )
message( STATUS "-----------------------------------------------------" )
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
project( PtBDDGenerator )

# Set parallel build mode
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )

# Remove some compilation warnings
add_definitions( -D_CRT_SECURE_NO_WARNINGS ) 

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
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../PtModel" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../PtGraphics" )

# # glm
# include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/glm" )

# # stb
# include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/stb" )

# # glad
# include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/glad/include" )

# # GLFW
# set( glfw3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/GLFW/lib/cmake/glfw3" )
# #find_package( glfw3 3.2 CONFIG REQUIRED PATH_SUFFIXES "/lib/cmake/glfw3" )
# find_package( glfw3 3.2 CONFIG REQUIRED )
# file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/GLFW/lib/glfw3.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug )
# file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/GLFW/lib/glfw3.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release )

# glad
set( glad_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/glad/lib/cmake/glad" )
find_package( glad CONFIG REQUIRED )

# glm
set( glm_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/glm/lib/cmake/glm" )
find_package( glm CONFIG REQUIRED )
include_directories( ${GLM_INCLUDE_DIRS} )

# stb
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/stb" )

# glfw
set( glfw3_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/glfw/lib/cmake/glfw3 )
find_package( glfw3 CONFIG REQUIRED )

# flann
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/flann/include" )

##################################################################################
# Program
##################################################################################

# Retrieve source files
file( GLOB incList "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )
file( GLOB inlList "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" )
file( GLOB srcList "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
# - shaders
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders/*.vert" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders/*.frag" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders/*.geom" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders/*.tese" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders/*.tesc" )
file( GLOB shaderList "${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders/*.comp" )

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

# Graphics
target_link_libraries( ${PROJECT_NAME} ${OPENGL_gl_LIBRARY} )

# PtDataModel
target_link_libraries( ${PROJECT_NAME} PtModel )
target_link_libraries( ${PROJECT_NAME} PtGraphics )

# GLFW
target_link_libraries( ${PROJECT_NAME} glfw )

## glad
##set( GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/glad" )
##add_library( "glad" SHARED "${GLAD_DIR}/src/glad.c" )
##target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
##target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
##target_link_libraries( ${PROJECT_NAME} glad "${CMAKE_DL_LIBS}" )
#find_library( PT_GLAD_LIBRARY
#    NAMES glad
#    HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/Externals/lib"
#)
##message( STATUS ${PT_GLAD_LIBRARY} )
#target_compile_definitions( ${PROJECT_NAME} PUBLIC GLAD_GLAPI_EXPORT )
#target_link_libraries( ${PROJECT_NAME} ${PT_GLAD_LIBRARY} "${CMAKE_DL_LIBS}" )

# glad
target_link_libraries( ${PROJECT_NAME} glad::glad )

# ImGui
set( ImGui_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/ImGui" )
file( GLOB imguiIncList "${ImGui_DIR}/*.h" )
file( GLOB imguiSrcList "${ImGui_DIR}/*.cpp" )
file( GLOB addons1imguiIncList "${ImGui_DIR}/addons/imguifilesystem/*.h" )
file( GLOB addons1imguiSrcList "${ImGui_DIR}/addons/imguifilesystem/*.cpp" )
file( GLOB addons2imguiIncList "${ImGui_DIR}/addons/imguiyesaddons/imguitinyfiledialogs.h" )
file( GLOB addons2imguiSrcList "${ImGui_DIR}/addons/imguiyesaddons/imguitinyfiledialogs.cpp" )
#add_library( "ImGui" ${imguiSrcList} ${imguiIncList} ${addons1imguiSrcList} ${addons1imguiIncList} ${addons2imguiSrcList} ${addons2imguiIncList} )
target_include_directories( "ImGui" PRIVATE "${ImGui_DIR}" )
target_include_directories( ${PROJECT_NAME} PRIVATE "${ImGui_DIR}" )
target_link_libraries( ${PROJECT_NAME} "ImGui" )

# Project property
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11 )

##################################################################################
# Custom commands
##################################################################################

# Libraries
install( TARGETS ${PROJECT_NAME} DESTINATION bin )

# Copy library
#add_custom_command(TARGET ${PROJECT_NAME}
 #          POST_BUILD
  #         COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/../x64
	#	   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/x64/.
		  # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_LINKER_FILE:${PROJECT_NAME}> ${GS_RELEASE_LIB_DIR}/.
		  # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${GS_RELEASE_BIN_DIR}/.
#)
