
include(FetchContent)

#################################################################################
set(GLM_PROJECT_NAME "glm")
set (GLM_SOURCE_URL "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip")
#GLM options
set(BUILD_TESTING OFF)

#################################################################################
find_package(${GLM_PROJECT_NAME} NAMES glm GLM CONFIG PATH_SUFFIXES "cmake" "cmake/glm")
if (${GLM_PROJECT_NAME}_FOUND)
	SET(${GLM_PROJECT_NAME}_POPULATED TRUE)
endif()
FetchContent_Declare(
	${GLM_PROJECT_NAME}
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	USES_TERMINAL_DOWNLOAD TRUE
	URL "${GLM_SOURCE_URL}"
)
# This will try calling find_package() first for dependencies. If the dependency is not found, then it will download the dependency from github and add it to the project for further build.
MESSAGE(STATUS "Looking for dependency ${GLM_PROJECT_NAME} ...")
if (GLM_AUTO_DOWNLOAD)
	MESSAGE(STATUS "GLM_AUTO_DOWNLOAD option is turned on. The dependency ${GLM_PROJECT_NAME} will be automatically downloaded if needed ...")
	if (NOT ${GLM_PROJECT_NAME}_POPULATED)
		MESSAGE(DEBUG "find_package could not locate dependency ${GLM_PROJECT_NAME} in config mode.\n -- The source files are being downloaded for build purpose ...")
		FetchContent_MakeAvailable(${GLM_PROJECT_NAME})
		FetchContent_GetProperties(${GLM_PROJECT_NAME}
			POPULATED ${GLM_PROJECT_NAME}_POPULATED
		)
	endif()
endif()
FetchContent_GetProperties(
	${GLM_PROJECT_NAME}
	SOURCE_DIR ${GLM_PROJECT_NAME}_SOURCE_DIR
	BINARY_DIR ${GLM_PROJECT_NAME}_BINARY_DIR
)
if(NOT ${GLM_PROJECT_NAME}_POPULATED)
	### get wxwidget from github
	MESSAGE(FATAL_ERROR "Required dependency ${GLM_PROJECT_NAME} is neither found by find_package nor has been downloaded with FetchContent_MakeAvailable.\n -- Aborting configuration step.")
else()
	MESSAGE(STATUS "Dependency ${GLM_PROJECT_NAME} has been populated.")
	if (${GLM_PROJECT_NAME}_SOURCE_DIR)
		MESSAGE(STATUS "-- Source directory of ${GLM_PROJECT_NAME}: ${${GLM_PROJECT_NAME}_SOURCE_DIR}")
	endif()
	if (${GLM_PROJECT_NAME}_BINARY_DIR)
		MESSAGE(STATUS "-- Binary directory of ${GLM_PROJECT_NAME}: ${${GLM_PROJECT_NAME}_BINARY_DIR}")
	endif()
	if(${GLM_PROJECT_NAME}_FOUND)
		MESSAGE(STATUS "Dependency ${GLM_PROJECT_NAME} is found by find_package via CONFIG file located at ${${GLM_PROJECT_NAME}_DIR}.")
		MESSAGE(STATUS "Adding ${GLM_PROJECT_NAME} include and library directories to the target.")
		# and for each of your dependent executable/library targets:
		MESSAGE(STATUS "Adding ${GLM_PROJECT_NAME} include and library directories to the target.")
		# and for each of your dependent executable/library targets:
		if (${GLM_PROJECT_NAME}_INCLUDE_DIRS)
			MESSAGE(STATUS "-- Include path ${${GLM_PROJECT_NAME}_INCLUDE_DIRS} is added to the target.")
			list(APPEND EXTRA_INCLUDE_DIRS "${${GLM_PROJECT_NAME}_INCLUDE_DIRS}")
		endif()
		if (${GLM_PROJECT_NAME}_LIBRARY_DIRS)
			MESSAGE(STATUS "-- Library path ${${GLM_PROJECT_NAME}_LIBRARY_DIRS} is added to the target.")
			list(APPEND EXTRA_LIB_DIRS "${${GLM_PROJECT_NAME}_LIBRARY_DIRS}")
		endif()
		if (${GLM_PROJECT_NAME}_LIBRARIES)
			MESSAGE(STATUS "-- Library components ${${GLM_PROJECT_NAME}_LIBRARIES} are added to the target.")
			list(APPEND EXTRA_LINKS "${${GLM_PROJECT_NAME}_LIBRARIES}")
		endif()
		list(APPEND EXTRA_LINKS glm)
	else()
		MESSAGE(STATUS "Dependency ${GLM_PROJECT_NAME} was not found by find_package.\n -- Using brute-force build and linking of source files made available by FetchContent_MakeAvailable.")
		MESSAGE(STATUS "Adding ${GLM_PROJECT_NAME} include directory '${${GLM_PROJECT_NAME}_SOURCE_DIR}/glm' to the target.")
		list(APPEND EXTRA_INCLUDE_DIRS "${${GLM_PROJECT_NAME}_SOURCE_DIR}/glm")
		MESSAGE(STATUS "Adding ${GLM_PROJECT_NAME} library paths and linking libraries against the target.")
		list(APPEND EXTRA_LIB_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib")
		list(APPEND EXTRA_LINKS glm)
	endif()
endif()
