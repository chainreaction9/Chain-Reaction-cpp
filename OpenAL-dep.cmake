
include(FetchContent)

#################### OpenAL Dependency ######################
set(OPENAL_PROJECT_NAME "OPENAL")
#OpenAL options
set(ALSOFT_UTILS OFF)
set(ALSOFT_NO_CONFIG_UTIL ON)
set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_INSTALL_CONFIG OFF)
set(ALSOFT_INSTALL_EXAMPLES OFF)
set(ALSOFT_INSTALL_UTILS OFF)
set(LIBTYPE STATIC)
#############################################################
find_package(${OPENAL_PROJECT_NAME} CONFIG
	NAMES OpenAL OpenAL32
)
if (${OPENAL_PROJECT_NAME}_FOUND)
	SET(${OPENAL_PROJECT_NAME}_POPULATED TRUE)
endif()
FetchContent_Declare(
	${OPENAL_PROJECT_NAME}
	USES_TERMINAL_DOWNLOAD TRUE
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
	GIT_TAG        "master"
	GIT_PROGRESS TRUE
)
# This will try calling find_package() first for dependencies. If the dependency is not found, then it will download the dependency from github and add it to the project for further build.
MESSAGE(STATUS "Looking for dependency ${OPENAL_PROJECT_NAME} ...")
if (OPENAL_AUTO_DOWNLOAD)
	MESSAGE(STATUS "OPENAL_AUTO_DOWNLOAD option is turned on. The dependency ${OPENAL_PROJECT_NAME} will be automatically downloaded if needed ...")
	if (NOT ${OPENAL_PROJECT_NAME}_POPULATED)
		MESSAGE(DEBUG "find_package could not locate dependency ${OPENAL_PROJECT_NAME} in config mode.\n -- The source files are being downloaded for build purpose ...")
		FetchContent_MakeAvailable(${OPENAL_PROJECT_NAME})
		FetchContent_GetProperties(${OPENAL_PROJECT_NAME}
			POPULATED ${OPENAL_PROJECT_NAME}_POPULATED
		)
	endif()
endif()
FetchContent_GetProperties(
	${OPENAL_PROJECT_NAME}
	SOURCE_DIR ${OPENAL_PROJECT_NAME}_SOURCE_DIR
	BINARY_DIR ${OPENAL_PROJECT_NAME}_BINARY_DIR
)
if(NOT ${OPENAL_PROJECT_NAME}_POPULATED)
	MESSAGE(FATAL_ERROR "Required dependency ${OPENAL_PROJECT_NAME} is neither found by find_package nor has been downloaded with FetchContent_MakeAvailable.\n -- Aborting configuration step.")
else()
	MESSAGE(STATUS "Dependency ${OPENAL_PROJECT_NAME} has been populated.")
	if (${OPENAL_PROJECT_NAME}_SOURCE_DIR)
		MESSAGE(STATUS "-- Source directory of ${OPENAL_PROJECT_NAME}: ${${OPENAL_PROJECT_NAME}_SOURCE_DIR}")
	endif()
	if (${OPENAL_PROJECT_NAME}_BINARY_DIR)
		MESSAGE(STATUS "-- Binary directory of ${OPENAL_PROJECT_NAME}: ${${OPENAL_PROJECT_NAME}_BINARY_DIR}")
	endif()
	if(${OPENAL_PROJECT_NAME}_FOUND)
		MESSAGE(STATUS "Dependency ${OPENAL_PROJECT_NAME} is found by find_package via CONFIG file located at ${${OPENAL_PROJECT_NAME}_DIR}.")
		MESSAGE(STATUS "Adding ${OPENAL_PROJECT_NAME} include and library directories to the target.")
		# and for each of your dependent executable/library targets:
		if (${OPENAL_PROJECT_NAME}_INCLUDE_DIR)
			MESSAGE(STATUS "-- Include path ${${OPENAL_PROJECT_NAME}_INCLUDE_DIR} is added to the target.")
			list(APPEND EXTRA_INCLUDE_DIRS "${${OPENAL_PROJECT_NAME}_INCLUDE_DIR}")
		endif()
		if (${OPENAL_PROJECT_NAME}_INCLUDE_DIRS)
			MESSAGE(STATUS "-- Include path ${${OPENAL_PROJECT_NAME}_INCLUDE_DIRS} is added to the target.")
			list(APPEND EXTRA_INCLUDE_DIRS "${${OPENAL_PROJECT_NAME}_INCLUDE_DIRS}")
		endif()
		if (${OPENAL_PROJECT_NAME}_LIBRARY_DIR)
			MESSAGE(STATUS "-- Library path ${${OPENAL_PROJECT_NAME}_LIBRARY_DIR} is added to the target.")
			list(APPEND EXTRA_LIB_DIRS "${${OPENAL_PROJECT_NAME}_LIBRARY_DIR}")
		endif()
		if (${OPENAL_PROJECT_NAME}_LIBRARY_DIRS)
			MESSAGE(STATUS "-- Library path ${${OPENAL_PROJECT_NAME}_LIBRARY_DIRS} is added to the target.")
			list(APPEND EXTRA_LIB_DIRS "${${OPENAL_PROJECT_NAME}_LIBRARY_DIRS}")
		endif()
		if (${OPENAL_PROJECT_NAME}_LIBRARY)
			MESSAGE(STATUS "-- Library components ${${OPENAL_PROJECT_NAME}_LIBRARY} are added to the target.")
			list(APPEND EXTRA_LINKS "${${OPENAL_PROJECT_NAME}_LIBRARY}")
		endif()
		if (${OPENAL_PROJECT_NAME}_LIBRARIES)
			MESSAGE(STATUS "-- Library components ${${OPENAL_PROJECT_NAME}_LIBRARIES} are added to the target.")
			list(APPEND EXTRA_LINKS "${${OPENAL_PROJECT_NAME}_LIBRARIES}")
		endif()
	else()
		MESSAGE(STATUS "Dependency ${OPENAL_PROJECT_NAME} was not found by find_package.\n -- Using brute-force build and linking of source files made available by FetchContent_MakeAvailable.")
		MESSAGE(STATUS "Adding ${OPENAL_PROJECT_NAME} include directory '${${OPENAL_PROJECT_NAME}_SOURCE_DIR}/include' to the target.")
		list(APPEND EXTRA_INCLUDE_DIRS "${${OPENAL_PROJECT_NAME}_SOURCE_DIR}/include")
		MESSAGE(STATUS "Adding ${OPENAL_PROJECT_NAME} library paths and linking libraries against the target.")
		list(APPEND EXTRA_LIB_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib")
		list(APPEND EXTRA_LIB_DIRS "${${OPENAL_PROJECT_NAME}_BINARY_DIR}")
		list(APPEND EXTRA_LINKS "OpenAL")
	endif()
endif()
