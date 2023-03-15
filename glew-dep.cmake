
include(FetchContent)

#################### glew Dependency ########################
set(GLEW_PROJECT_NAME "libglew")
#Glew options
set(glew-cmake_BUILD_SHARED OFF)
set(GLEW_USE_STATIC_LIBS ON)
set(BUILD_UTILS OFF)
set(ONLY_LIBS ON)
############################################################
MESSAGE(STATUS "Looking for dependency ${GLEW_PROJECT_NAME} ...")
find_package(${GLEW_PROJECT_NAME} CONFIG NAMES libglew_static)
if (${GLEW_PROJECT_NAME}_FOUND)
	SET(${GLEW_PROJECT_NAME}_POPULATED TRUE)
endif()
FetchContent_Declare(
	${GLEW_PROJECT_NAME}
	USES_TERMINAL_DOWNLOAD TRUE
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	GIT_REPOSITORY "https://github.com/Perlmint/glew-cmake.git"
	GIT_TAG "glew-cmake-2.2.0"
	GIT_PROGRESS TRUE
)

# This will try calling find_package() first for dependencies. If the dependency is not found, then it will download the dependency from github and add it to the project for further build.
if (GLEW_AUTO_DOWNLOAD)
	MESSAGE(STATUS "GLEW_AUTO_DOWNLOAD option is turned on. The dependency ${GLEW_PROJECT_NAME} will be automatically downloaded if needed ...")
	if (NOT ${GLEW_PROJECT_NAME}_POPULATED)
		MESSAGE(DEBUG "find_package could not locate dependency ${GLEW_PROJECT_NAME} in config mode.\n -- The source files are being downloaded for build purpose ...")
		FetchContent_MakeAvailable(${GLEW_PROJECT_NAME})
		FetchContent_GetProperties(${GLEW_PROJECT_NAME}
			POPULATED ${GLEW_PROJECT_NAME}_POPULATED
		)
	endif()
endif()
FetchContent_GetProperties(
	${GLEW_PROJECT_NAME}
	SOURCE_DIR ${GLEW_PROJECT_NAME}_SOURCE_DIR
	BINARY_DIR ${GLEW_PROJECT_NAME}_BINARY_DIR
)
if(NOT ${GLEW_PROJECT_NAME}_POPULATED)
	MESSAGE(FATAL_ERROR "Required dependency ${GLEW_PROJECT_NAME} is neither found by find_package nor has been downloaded with FetchContent_MakeAvailable.\n -- Aborting configuration step.")
else()
	MESSAGE(STATUS "Dependency ${GLEW_PROJECT_NAME} has been populated.")
	if (${GLEW_PROJECT_NAME}_SOURCE_DIR)
		MESSAGE(STATUS "-- Source directory of ${GLEW_PROJECT_NAME}: ${${GLEW_PROJECT_NAME}_SOURCE_DIR}")
	endif()
	if (${GLEW_PROJECT_NAME}_BINARY_DIR)
		MESSAGE(STATUS "-- Binary directory of ${GLEW_PROJECT_NAME}: ${${GLEW_PROJECT_NAME}_BINARY_DIR}")
	endif()
	if(${GLEW_PROJECT_NAME}_FOUND)
		MESSAGE(STATUS "Dependency ${GLEW_PROJECT_NAME} is found by find_package via CONFIG file located at ${${GLEW_PROJECT_NAME}_DIR}.")
		MESSAGE(STATUS "Adding ${GLEW_PROJECT_NAME} include and library directories to the target.")
		# and for each of your dependent executable/library targets:
		if (${GLEW_PROJECT_NAME}_INCLUDE_DIRS)
			MESSAGE(STATUS "-- Include path ${${GLEW_PROJECT_NAME}_INCLUDE_DIRS} is added to the target.")
			list(APPEND EXTRA_INCLUDE_DIRS "${${GLEW_PROJECT_NAME}_INCLUDE_DIRS}")
		endif()
		if (${GLEW_PROJECT_NAME}_LIBRARY_DIRS)
			MESSAGE(STATUS "-- Library path ${${GLEW_PROJECT_NAME}_LIBRARY_DIRS} is added to the target.")
			list(APPEND EXTRA_LIB_DIRS "${${GLEW_PROJECT_NAME}_LIBRARY_DIRS}")
		endif()
		if (${GLEW_PROJECT_NAME}_LIBRARIES)
			MESSAGE(STATUS "-- Library components ${${GLEW_PROJECT_NAME}_LIBRARIES} are added to the target.")
			list(APPEND EXTRA_LINKS "${${GLEW_PROJECT_NAME}_LIBRARIES}")
		endif()
		list(APPEND EXTRA_LINKS "libglew_static")
	else()
		MESSAGE(STATUS "Dependency ${GLEW_PROJECT_NAME} was not found by find_package.\n -- Using brute-force build and linking of source files made available by FetchContent_MakeAvailable.")
		MESSAGE(STATUS "Adding ${GLEW_PROJECT_NAME} include directory '${${GLEW_PROJECT_NAME}_SOURCE_DIR}/include' to the target.")
		list(APPEND EXTRA_INCLUDE_DIRS "${${GLEW_PROJECT_NAME}_SOURCE_DIR}/include")
		MESSAGE(STATUS "Adding ${GLEW_PROJECT_NAME} library paths and linking libraries against the target.")
		list(APPEND EXTRA_LIB_DIRS "${${GLEW_PROJECT_NAME}_BINARY_DIR}/lib")
		list(APPEND EXTRA_LIB_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib")
		list(APPEND EXTRA_LINKS "libglew_static")
	endif()
endif()
