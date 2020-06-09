#----------------------------------------------------------------
# PCTS PROJECT CMake file
# Main user file
#----------------------------------------------------------------

#----------------------------------------------------------------
# Project
#----------------------------------------------------------------

# Invite message
message( STATUS "-------------------------------------------" )
message( STATUS "- PROJECT GENERATION : PPTBF Tools -" )
message( STATUS "-------------------------------------------" )
message( STATUS "" )

# Project name
#project( PPTBFTools )

#----------------------------------------------------------------
# Add Library/Tools path
#----------------------------------------------------------------

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/PtViewer" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/PtBDDGenerator" )
