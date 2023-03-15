Chain Reaction Cpp
==================

This open source MIT-licensed project is a cross-platform implementation of the android game [Chain Reaction](https://brilliant.org/wiki/chain-reaction-game/) in **C++**. The project makes heavy usage of the GUI library [wxWidgets](https://www.wxwidgets.org/) and combines it with the [OpenGL](https://www.opengl.org/) API to render 3D graphics. It is designed in such a way that the application has the potential to simulate any other graphics-based game.

![Application image](/chain-reaction.png)
### About the Chain Reaction game.
_________________________________
This is a strategy based board game between two or more players. Each player gets a turn to occupy board-cells by either capturing an empty cell or by invading cells occupied by other players. The goal of the game is to eliminate all other players by capturing their cells. 

An invasion is caused by filling an already occupied cell beyond its critical mass. As per the common rule of this game, the critical mass of each board-cell is equal to one less than the number of its neighbours. A chain reaction is created if enough neighbouring cells reach their critical mass. More details on the rules can be found [here](https://chainserver.pythonanywhere.com) which is a webapp hosting this game.

Source building
---------------
The project can be built with [CMake](https://cmake.org/) (version >= 3.20). In order for the build to be successful the following dependencies must be available in the dev environment.
### Dependency
1. [wxWidgets](https://www.wxwidgets.org/)
2. [GLEW](https://glew.sourceforge.net/) or [glew-cmake](https://github.com/Perlmint/glew-cmake)
3. [OpenGL Mathematics (GLM)](https://glm.g-truc.net/0.9.9/)
4. [OpenAL-Soft](https://github.com/kcat/openal-soft)

The four default cmake options ```WXWIDGETS_AUTO_DOWNLOAD```, ```GLM_AUTO_DOWNLOAD```, ```GLEW_AUTO_DOWNLOAD```, ```OPENAL_AUTO_DOWNLOAD``` defined in ```DefaultOptions.cmake``` are set, so that if the dependencies are not available, they will automatically be downloaded with a ``` FetchContent_MakeAvailable``` call in the configuration time and be built from scratch. One can configure the a build with the follwing cmake commands from the project root directory:

- **Debug** build: 

```bash
cmake -S . -B destination_folder -DCMAKE_BUILD_TYPE=Debug
```

- **Release** build : 

```bash
cmake -S . -B destination_folder -DCMAKE_BUILD_TYPE=Release
```

Assuming the configuration went well, you can build the binary executable by running the command 

```bash
cmake --build destination_folder
```

### Resource files
The project depends on the resource files (*.obj, shaders, *.wav, *mtl, etc.) in the [resource](/Chain-Reaction-cpp/src/Resources/resource) directory. On Windows platform [resource.zip](/Chain-Reaction-cpp/src/Resources/resource.zip) containing these files will automatically be embedded into the binary executable with the compilation of the source file [Chain-Reaction.rc](/Chain-Reaction-cpp/src/Chain-Reaction.rc). On other platforms the user will need to manually specify the zip file at the start of the application.
