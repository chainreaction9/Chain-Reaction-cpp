# CMakeList.txt : CMake project for Chain-Reaction-cpp, include source and define
# project specific logic here.
#

# TODO: Add tests and install targets if needed.
set(TARGET_NAME ${PROJECT_NAME})
add_executable (${TARGET_NAME} WIN32)

# Add source to this project's executable.
MESSAGE(STATUS "Including source directories ${EXTRA_INCLUDE_DIRS} to the target ${TARGET_NAME}.")
target_include_directories(${TARGET_NAME} 
	PRIVATE ${EXTRA_INCLUDE_DIRS}
	PRIVATE "src"
)
MESSAGE(STATUS "Including library directories ${EXTRA_LIB_DIRS} to the target ${TARGET_NAME}.")
target_link_directories(${TARGET_NAME}
	PRIVATE ${EXTRA_LIB_DIRS}
)
MESSAGE(STATUS "Linking libraries ${EXTRA_LINKS} to the target ${TARGET_NAME}.")
target_link_libraries(${TARGET_NAME}
	PRIVATE ${EXTRA_LINKS}
)
###################### Add source files to the target ####################
list(APPEND SOURCE_FILES "src/AppGLCanvas.cpp")
list(APPEND SOURCE_FILES "src/AppGUIFrame.cpp")
list(APPEND SOURCE_FILES "src/AudioFile.h")
list(APPEND SOURCE_FILES "src/AudioLoader.cpp")
if (NOT WIN32)
	list(APPEND SOURCE_FILES "src/icon.xpm")
endif()
list(APPEND SOURCE_FILES "src/IcoSphere.cpp")
list(APPEND SOURCE_FILES "src/IndexBufferObject.cpp")
list(APPEND SOURCE_FILES "src/InputDialogs.cpp")
list(APPEND SOURCE_FILES "src/main.cpp")
list(APPEND SOURCE_FILES "src/MainGame.cpp")
list(APPEND SOURCE_FILES "src/MeshGenerator.cpp")
list(APPEND SOURCE_FILES "src/ObjLoader.h")
list(APPEND SOURCE_FILES "src/PrecompiledHeader.cpp")
list(APPEND SOURCE_FILES "src/Random.h")
if (WIN32)
	list(APPEND SOURCE_FILES "src/Resource.h")
	list(APPEND SOURCE_FILES "src/Chain-Reaction.rc")
endif()
list(APPEND SOURCE_FILES "src/Shaders.cpp")
list(APPEND SOURCE_FILES "src/SoundSystem.cpp")
list(APPEND SOURCE_FILES "src/Text.cpp")
list(APPEND SOURCE_FILES "src/Utilities.cpp")
list(APPEND SOURCE_FILES "src/UvSphere.cpp")
list(APPEND SOURCE_FILES "src/VertexArrayObject.cpp")
list(APPEND SOURCE_FILES "src/VertexBufferObject.cpp")
list(APPEND SOURCE_FILES "src/VertexLayout.cpp")

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
endif()
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
install(TARGETS ${TARGET_NAME}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/Debug)
install(TARGETS ${TARGET_NAME}
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/Release)
