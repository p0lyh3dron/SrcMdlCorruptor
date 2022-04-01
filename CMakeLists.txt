cmake_minimum_required( VERSION 3.15 FATAL_ERROR )

project( SourceMDLCorrupter )

set( SRC_FILES
  src/main.cpp
  src/mdl.cpp
  src/mdl.h
  
  src/util.cpp
  src/util.h
  src/commandline.cpp
  src/commandline.h
)

include( core.cmake )

add_executable( src_mdl_corrupt ${SRC_FILES} )

