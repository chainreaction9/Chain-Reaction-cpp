# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20 FATAL_ERROR)
project ("Chain-Reaction-cpp" VERSION 1.0)

# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(PROJECT_IS_TOP_LEVEL)
	set(buildTypes "Debug" "Release")
	set(defaultBuildType "Debug")
	if(NOT isMultiConfig)
		if(NOT CMAKE_BUILD_TYPE)
			message(STATUS "Setting build type to '${defaultBuildType}' as none was specified.")
			set(CMAKE_BUILD_TYPE "${defaultBuildType}" CACHE STRING "Choose the type of build." FORCE)
			set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${buildTypes}")
		elseif(NOT CMAKE_BUILD_TYPE IN_LIST buildTypes)
			message(FATAL_ERROR "Unknown build type: '${CMAKE_BUILD_TYPE}'")
		endif()
	endif()
	unset(defaultBuildType)
	unset(buildTypes)
	set_property(GLOBAL PROPERTY USE_FOLDERS YES)
endif()

if(PROJECT_IS_TOP_LEVEL)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		string(APPEND CMAKE_CXX_FLAGS " /W4")
	endif()
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 20)
elseif(CMAKE_CXX_STANDARD LESS 17 OR CMAKE_CXX_STANDARD GREATER_EQUAL 98)
	message(FATAL_ERROR "The CMAKE_CXX_STANDARD value needs to be at least 17, current value: ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(MSVC)
	# Disable warnings about standard conformant code that is not conform to Microsoft standards
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
	# Without BOM the UTF-8 encoding doesn't get detected so enforce it
	add_compile_options(/source-charset:utf-8)
	# Report the correct C++ version if supported
	if(MSVC_VERSION GREATER_EQUAL 1914)
		add_compile_options(/Zc:__cplusplus)
	endif()
endif()


## the os must support OpenGL
find_package(OpenGL REQUIRED)

include(DefaultOptions.cmake)
## include external cmake project dependencies
include(Dependencies.cmake)
# Include sub-projects.
add_subdirectory ("${PROJECT_NAME}")