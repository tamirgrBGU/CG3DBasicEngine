cmake_minimum_required(VERSION 3.1)
project(libigl_tutorials)

### Compile libigl in header-only mode for Python bindings
option(LIBIGL_USE_STATIC_LIBRARY "Use LibIGL as static library" OFF)

### Adding libIGL: choose the path to your local copy libIGL
if(NOT TARGET igl_common)
  include(libigl)
else()
  include(LibiglDownloadExternal)
endif()

include(engine)

# Store location of tutorial/shared directory
#set(TUTORIAL_SHARED_PATH ${LIBIGL_EXTERNAL}/../tutorial/data CACHE PATH "location of shared tutorial resources")
#add_library(tutorials INTERFACE)
#target_compile_definitions(tutorials INTERFACE "-DTUTORIAL_SHARED_PATH=\"${TUTORIAL_SHARED_PATH}\"")
#target_include_directories(tutorials INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


#######################
if(NOT (LIBIGL_WITH_OPENGL AND LIBIGL_WITH_OPENGL_GLFW) )
  message(WARNING "Most tutorial executables depend on OpenGL and glfw. Use `cmake ../ -DLIBIGL_WITH_OPENGL=ON -DLIBIGL_WITH_OPENGL_GLFW=ON`")
  return()
endif()

add_subdirectory(Game)

add_subdirectory(Assignment1)

add_subdirectory(Assignment2)

add_subdirectory(Assignment3)

add_subdirectory(Project)
