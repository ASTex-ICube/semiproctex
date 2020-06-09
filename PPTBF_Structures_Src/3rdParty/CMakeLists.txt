#----------------------------------------------------------------
# OmniScale PROJECT CMake file
# Main user file
#----------------------------------------------------------------

# Check CMAKE version
cmake_minimum_required( VERSION 3.0 )

#----------------------------------------------------------------
# Project
#----------------------------------------------------------------

# Invite message
message( STATUS "-------------------------------------------" )
message( STATUS "- PROJECT GENERATION : PPTBF 3rdParty -" )
message( STATUS "-------------------------------------------" )
message( STATUS "" )

# Project name
project( PPTBF3rdPartyProject )

set( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Externals" )
set( CUSTOM_DEBUG_POSTFIX "d" )

#----------------------------------------------------------------
# Add Library/Tools path
#----------------------------------------------------------------

# ImGui
project( ImGui )
set( ImGui_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ImGui" )
file( GLOB imguiIncList "${ImGui_DIR}/*.h" )
file( GLOB imguiSrcList "${ImGui_DIR}/*.cpp" )
file( GLOB addons1imguiIncList "${ImGui_DIR}/addons/imguifilesystem/*.h" )
file( GLOB addons1imguiSrcList "${ImGui_DIR}/addons/imguifilesystem/*.cpp" )
file( GLOB addons2imguiIncList "${ImGui_DIR}/addons/imguiyesaddons/imguitinyfiledialogs.h" )
file( GLOB addons2imguiSrcList "${ImGui_DIR}/addons/imguiyesaddons/imguitinyfiledialogs.cpp" )
file( GLOB addons3imguiIncList "${ImGui_DIR}/addons/imguinodegrapheditor/*.h" )
file( GLOB addons3imguiSrcList "${ImGui_DIR}/addons/imguinodegrapheditor/*.cpp" )
add_library( ImGui ${imguiSrcList} ${imguiIncList} ${addons1imguiSrcList} ${addons1imguiIncList} ${addons2imguiSrcList} ${addons2imguiIncList} ${addons3imguiSrcList} ${addons3imguiIncList} )
set_target_properties( ImGui PROPERTIES DEBUG_POSTFIX ${CUSTOM_DEBUG_POSTFIX} )
target_include_directories( ImGui PRIVATE "${ImGui_DIR}" )
install( TARGETS ImGui DESTINATION lib )
