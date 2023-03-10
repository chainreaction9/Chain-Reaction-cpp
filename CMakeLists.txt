# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20 FATAL_ERROR)
project ("Chain-Reaction-cpp" VERSION 1.0)

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

## the os must support OpenGL
find_package(OpenGL REQUIRED)

include(DefaultOptions.cmake)
## include external cmake project dependencies
include(Dependencies.cmake)
# Include sub-projects.
add_subdirectory ("${PROJECT_NAME}")