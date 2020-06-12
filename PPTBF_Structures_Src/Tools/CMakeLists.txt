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

# Real-time viewer/designer of PPTBF model
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/PtViewer" )

# Command line PPTBF synthsizer from a PPTBF parameter file
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/PtBDDGenerator" )

# Sampling the PPTBF parameter space of produce BDD for use such as PPTBF parameter estimation, learning-based approaches...
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/PtBDDParameterSpaceSampler" )
