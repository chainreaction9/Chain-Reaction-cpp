
include(FetchContent)

#################### WxWidgets Dependency ####################
#################################################################################
set(WXWIDGETS_PROJECT_NAME "wxWidgets")
#wxWidget options
set(wxBUILD_INSTALL ON)
set(wxBUILD_SHARED OFF)
set(wxUSE_UNICODE ON)
set(wxWidgets_USE_STATIC ON)
set(wxBUILD_SAMPLES OFF)
set(wxBUILD_COMPATIBILITY 3.0)
#################################################################################

FetchContent_Declare(
	${WXWIDGETS_PROJECT_NAME}
	URL "https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.0/wxWidgets-3.2.0.zip"
	URL_HASH SHA1=29f70fb17f42345a281375b4219fa0b8e8c5dfcd
	FIND_PACKAGE_ARGS NAMES wxWidgets CONFIG
)
# This will try calling find_package() first for dependencies. If the dependency is not found, then it will download the dependency from github and add it to the project for further build.
MESSAGE(STATUS "Looking for dependency ${WXWIDGETS_PROJECT_NAME} ...")
if (WXWIDGETS_AUTO_DOWNLOAD)
	FetchContent_MakeAvailable(${WXWIDGETS_PROJECT_NAME})
else()
	find_package(${WXWIDGETS_PROJECT_NAME} CONFIG
		NAMES wxWidgets
	)
	if (${WXWIDGETS_PROJECT_NAME}_FOUND)
		SET(${WXWIDGETS_PROJECT_NAME}_POPULATED TRUE)
	endif()
endif()
FetchContent_GetProperties(
		${WXWIDGETS_PROJECT_NAME}
		SOURCE_DIR ${WXWIDGETS_PROJECT_NAME}_SOURCE_DIR
		BINARY_DIR ${WXWIDGETS_PROJECT_NAME}_BINARY_DIR
		POPULATED ${WXWIDGETS_PROJECT_NAME}_POPULATED
)
if(NOT ${WXWIDGETS_PROJECT_NAME}_POPULATED)
	### get wxwidget from github
	MESSAGE(FATAL_ERROR "Required dependency ${WXWIDGETS_PROJECT_NAME} is neither found by find_package nor has been downloaded with FetchContent_MakeAvailable. Aborting configuration step.")
else()
	MESSAGE(STATUS "Dependency ${WXWIDGETS_PROJECT_NAME} has been populated.")
	if (${WXWIDGETS_PROJECT_NAME}_SOURCE_DIR)
		MESSAGE(STATUS "-- Source directory of ${WXWIDGETS_PROJECT_NAME}: ${${WXWIDGETS_PROJECT_NAME}_SOURCE_DIR}")
	endif()
	if (${WXWIDGETS_PROJECT_NAME}_BINARY_DIR)
		MESSAGE(STATUS "-- Binary directory of ${WXWIDGETS_PROJECT_NAME}: ${${WXWIDGETS_PROJECT_NAME}_BINARY_DIR}")
	endif()
	if(${WXWIDGETS_PROJECT_NAME}_FOUND)
		MESSAGE(STATUS "Dependency ${WXWIDGETS_PROJECT_NAME} is found by find_package via CONFIG file located at ${${WXWIDGETS_PROJECT_NAME}_DIR}.")
		MESSAGE(STATUS "Adding ${WXWIDGETS_PROJECT_NAME} include and library directories to the target.")
		if (${WXWIDGETS_PROJECT_NAME}_USE_FILE)
			include("${${WXWIDGETS_PROJECT_NAME}_USE_FILE}")
		endif()
		# and for each of your dependent executable/library targets:
		if (${WXWIDGETS_PROJECT_NAME}_INCLUDE_DIRS)
			MESSAGE(STATUS "-- Include path ${${WXWIDGETS_PROJECT_NAME}_INCLUDE_DIRS} is added to the target.")
			list(APPEND EXTRA_INCLUDE_DIRS "${${WXWIDGETS_PROJECT_NAME}_INCLUDE_DIRS}")
		endif()
		if (${WXWIDGETS_PROJECT_NAME}_LIBRARY_DIRS)
			MESSAGE(STATUS "-- Library path ${${WXWIDGETS_PROJECT_NAME}_LIBRARY_DIRS} is added to the target.")
			list(APPEND EXTRA_LIB_DIRS "${${WXWIDGETS_PROJECT_NAME}_LIBRARY_DIRS}")
		endif()
		if (${WXWIDGETS_PROJECT_NAME}_LIBRARIES)
			MESSAGE(STATUS "-- Library components ${${WXWIDGETS_PROJECT_NAME}_LIBRARIES} are added to the target.")
			list(APPEND EXTRA_LINKS "${${WXWIDGETS_PROJECT_NAME}_LIBRARIES}")
		endif()
	else()
		MESSAGE(STATUS "Dependency ${WXWIDGETS_PROJECT_NAME} was not found by find_package. Using brute-force build and linking of source files made available by FetchContent_MakeAvailable.")
		MESSAGE(STATUS "Adding ${WXWIDGETS_PROJECT_NAME} include directory '${${WXWIDGETS_PROJECT_NAME}_SOURCE_DIR}/include' to the target.")
		list(APPEND EXTRA_INCLUDE_DIRS "${${WXWIDGETS_PROJECT_NAME}_SOURCE_DIR}/include")
		if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
			set(custom_wxCOMPILER_PREFIX "vc")
		elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
		    set(custom_wxCOMPILER_PREFIX "gcc")
		elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
		    set(custom_wxCOMPILER_PREFIX "clang")
		endif()
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		    set(custom_wxARCH_SUFFIX "_x64")
		endif()
		if(WIN32)
			if (wxUSE_UNICODE)
				MESSAGE(STATUS "Adding include directories for <wx/setup.h>")
				list(APPEND EXTRA_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib/${custom_wxCOMPILER_PREFIX}${custom_wxARCH_SUFFIX}_lib/$<$<CONFIG:Debug>:mswud>$<$<CONFIG:Release>:mswu>$<$<CONFIG:RelWithDebInfo>:mswu>$<$<CONFIG:MinSizeRel>:mswu>")
			else()
				MESSAGE(STATUS "Adding include directories for <wx/setup.h>")
				list(APPEND EXTRA_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib/${custom_wxCOMPILER_PREFIX}${custom_wxARCH_SUFFIX}_lib/$<$<CONFIG:Debug>:mswud>$<$<CONFIG:Release>:mswu>$<$<CONFIG:RelWithDebInfo>:mswu>$<$<CONFIG:MinSizeRel>:mswu>")
			endif()
		endif()
		MESSAGE(STATUS "Adding ${WXWIDGETS_PROJECT_NAME} library paths and linking libraries against the target.")
		list(APPEND EXTRA_LIB_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib/${custom_wxCOMPILER_PREFIX}${custom_wxARCH_SUFFIX}_lib")
		list(APPEND EXTRA_LINKS wxrichtext wxgl wxcore wxbase)
	endif()
endif()