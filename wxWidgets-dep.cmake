
include(FetchContent)

#################### WxWidgets Dependency ####################
#################################################################################
set(WXWIDGETS_PROJECT_NAME "wxWidgets")
#wxWidget options
set(wxBUILD_INSTALL OFF)
set(wxBUILD_SHARED OFF)
set(wxUSE_UNICODE ON)
set(wxUSE_OPENGL ON)
set(wxUSE_GLCANVAS_EGL OFF) #To support GLEW (glew is built with OpenGL backend) with wxGLCanvas
set(wxWidgets_USE_STATIC ON)
set(wxBUILD_SAMPLES OFF)
set(wxBUILD_COMPATIBILITY 3.0)
#################################################################################
MESSAGE(STATUS "Looking for dependency ${WXWIDGETS_PROJECT_NAME} ...")
find_package(${WXWIDGETS_PROJECT_NAME} CONFIG NAMES wxWidgets)
if (${WXWIDGETS_PROJECT_NAME}_FOUND)
	SET(${WXWIDGETS_PROJECT_NAME}_POPULATED TRUE)
endif()
FetchContent_Declare(
	${WXWIDGETS_PROJECT_NAME}
	USES_TERMINAL_DOWNLOAD TRUE
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	URL "https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.0/wxWidgets-3.2.0.zip"
	URL_HASH SHA1=29f70fb17f42345a281375b4219fa0b8e8c5dfcd
)
# This will try calling find_package() first for dependencies. If the dependency is not found, then it will download the dependency from github and add it to the project for further build.
if (WXWIDGETS_AUTO_DOWNLOAD)
	MESSAGE(STATUS "WXWIDGETS_AUTO_DOWNLOAD option is turned on. The dependency ${WXWIDGETS_PROJECT_NAME} will be automatically downloaded if needed ...")
	if (NOT ${WXWIDGETS_PROJECT_NAME}_POPULATED)
		MESSAGE(DEBUG "find_package could not locate dependency ${WXWIDGETS_PROJECT_NAME} in config mode.\n -- The source files are being downloaded for build purpose ...")
		FetchContent_MakeAvailable(${WXWIDGETS_PROJECT_NAME})
		FetchContent_GetProperties(${WXWIDGETS_PROJECT_NAME}
			POPULATED ${WXWIDGETS_PROJECT_NAME}_POPULATED
		)
	endif()
endif()
FetchContent_GetProperties(
	${WXWIDGETS_PROJECT_NAME}
	SOURCE_DIR ${WXWIDGETS_PROJECT_NAME}_SOURCE_DIR
	BINARY_DIR ${WXWIDGETS_PROJECT_NAME}_BINARY_DIR
)
if(NOT ${WXWIDGETS_PROJECT_NAME}_POPULATED)
	MESSAGE(FATAL_ERROR "Required dependency ${WXWIDGETS_PROJECT_NAME} is neither found by find_package nor has been downloaded with FetchContent_MakeAvailable.\n -- Aborting configuration step.")
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
		MESSAGE(STATUS "Dependency ${WXWIDGETS_PROJECT_NAME} was not found by find_package.\n -- Using brute-force build and linking of source files made available by FetchContent_MakeAvailable.")
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
		list(APPEND EXTRA_LINKS wxrichtext wxadv wxgl wxcore wxbase)
	endif()
endif()
