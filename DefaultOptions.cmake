
set(THIRDLIBS_BUILD_TYPE "debug")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE(STATUS "Building project in debug mode ...")
    set(THIRDLIBS_BUILD_TYPE "debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	MESSAGE(STATUS "Building project in release mode ...")
    set(THIRDLIBS_BUILD_TYPE "release")
endif()

#Update policy CMP0077 to NEW behaviour so that option() honors normal variable.
SET(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

option(BUILD_SHARED_LIBS "Build external shared/static libraries" OFF)
###  If the following options are turned off, the targetted dependencies won't be downloaded.
###  However, they are mandatory requirements for this project.
###  So please make sure that the dependencies can be found by a find_package call in config mode
###  before turning the options off. Git is also a mandatory requirement for some (glew and OpenAL)
###  of the downloads to work.
option(GLM_AUTO_DOWNLOAD "Download glm dependency via FetchContent_MakeAvailable" ON)
option(GLEW_AUTO_DOWNLOAD "Download glew dependency via FetchContent_MakeAvailable" ON)
option(WXWIDGETS_AUTO_DOWNLOAD "Download wxWidgets dependency via FetchContent_MakeAvailable" ON)
option(OPENAL_AUTO_DOWNLOAD "Download OpenAL dependency via FetchContent_MakeAvailable" ON)