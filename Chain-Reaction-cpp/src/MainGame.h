#pragma once
#ifndef MAINGAME_H
#define MAINGAME_H

#include "PrecompiledHeader.h" //precompiled header
#include "AppGUIFrame.h"
#include "Random.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"
#include "VertexLayout.h"
#include "Shaders.h"
#include "IcoSphere.h"
#include "Utilities.h"
#include "AudioFile.h"
#include "SoundSystem.h"

class MainGame{
public:
	/*
	* @brief Draws the current board of the game and animates a blast if there are any.
	* @param {unsigned long} deltaTime: average time (in milliseconds) taken to render each frame.
	* @returns {bool} true if the current game has ended (i.e., all orbs in the board are of same color), false otherwise.
	*/
	bool drawBoard(unsigned long deltaTime);
	/**
	* @brief Converts a 2D mouse coordinate to board coordinate.
	* @param {double} mouseX: x-coordinate of the mouse position.
	* @param {double} mouseY: y-coordinate of the mouse position.
	* @returns {vec2} value of the board coordinate if the mouse position falls within the game board, {-1, -1} otherwise.
	*/
	glm::vec2 getBoardCoordinates(double mouseX, double mouseY);
	inline AppGUIFrame* getFrame() const { return this->_mainframe; }
	inline glm::vec3 getGlCompatibleCoordinate(float radius, float latitudeDegree, float longitudeDegree) const { return glm::vec3({ radius * sin(glm::radians(longitudeDegree)) * cos(glm::radians(latitudeDegree)), radius * sin(glm::radians(latitudeDegree)), radius * cos(glm::radians(longitudeDegree)) * cos(glm::radians(latitudeDegree)) }); }
	inline unsigned int getWinnerIndex() const { return this->_winnerIndex; }
	inline const char* getWinnerName() const { return this->_winnerName.c_str(); }
	inline bool isBlastAnimationRunning() const { return this->_blastAnimationRunning; }
	/*
	* @brief Checks whether or not a given player is eliminated from the game.
	* @param {string} player: color of the player that is being searched for.
	* @returns {bool} true if player is eliminated, false otherwise.
	*/
	bool isEliminated(std::string player) const;
	/*
	* @brief Checks whether or not a given coordinate in the game board contains an explosive orb.
	* @param {pair<uint32_t, uint32_t>} boardCoordinate: coordinate in the game board that is being tested.
	* @returns {bool} true if the given boardCoordinate contains an explosive, false otherwise.
	*/
	bool isExplosive(std::pair<uint32_t, uint32_t> boardCoordinate) const;
	/*
	* @brief Default constructor for the game object.
	* @param {const char*} soundSourceName: path to the audio source file. Supported audio formats are '.wav', '.aiff'.
	* @param {const char*} gridShaderVertexSource: raw source code of the vertex shader for rendering grid-lines in the game board. If none is given, a default hard-coded shader is used.
	* @param {const char*} gridShaderFragmentSource: raw source code of the fragment shader for rendering grid-lines in the game board. If none is given, a default hard-coded shader is used.
	* @param {const char*} orbShaderVertexSource: raw source code of the vertex shader for rendering orbs in the game board. If none is given, a default hard-coded shader is used.
	* @param {const char*} orbShaderFragmentSource: raw source code of the fragment shader for rendering orbs in the game board. If none is given, a default hard-coded shader is used.
	*/
	MainGame(const char* soundSourceName, const char* gridShaderVertexSource = nullptr, const char* gridShaderFragmentSource = nullptr, const char* orbShaderVertexSource = nullptr, const char* orbShaderFragmentSource = nullptr);
	/*
	* @brief Default constructor for the game object.
	* @param {const void*} soundSource: a pointer to the audio source file stored in memory. Supported audio formats are '.wav', '.aiff'.
	* @param {size_t} size: size of the audio source (in bytes).
	* @param {const char*} gridShaderVertexSource: raw source code of the vertex shader for rendering grid-lines in the game board. If none is given, a default hard-coded shader is used.
	* @param {const char*} gridShaderFragmentSource: raw source code of the fragment shader for rendering grid-lines in the game board. If none is given, a default hard-coded shader is used.
	* @param {const char*} orbShaderVertexSource: raw source code of the vertex shader for rendering orbs in the game board. If none is given, a default hard-coded shader is used.
	* @param {const char*} orbShaderFragmentSource: raw source code of the fragment shader for rendering orbs in the game board. If none is given, a default hard-coded shader is used.
	*/
	MainGame(const void* soundSource, size_t size, const char* gridShaderVertexSource = nullptr, const char* gridShaderFragmentSource = nullptr, const char* orbShaderVertexSource = nullptr, const char* orbShaderFragmentSource = nullptr);
	/*
	* @brief Default destructor of the game object.
	*/
	~MainGame(void);
	/*
	* @brief Processes players input in the game board. Inputs must be provided in the board coordinate system (lower-left square is (0,0). x-coordinate (y-coordinate) increases in the right (top) direction.
	* @param {uint32_t} boardCoordinateX: x-coordinate of the input square in board coordinate system.
	* @param {uint32_t} boardCoordinateY: y-coordinate of the input square in board coordiante system.
	* @returns {bool} true if the input is successfully processed by the game, false otherwise.
	*/
	bool processPlayerInput(uint32_t boardCoordinateX, uint32_t boardCoordinateY);
	/*
	* @brief Resets state variables of the game object.
	*/
	void resetGameVariables(void);
	/*
	* @brief Configures the board dimension and set of players before the start of a game.
	* @param {unsigned int} numberOfRows: number of rows in the game board.
	* @param {unsigned int} numberOfColumns: number of columns in the game board.
	* @param {vector<string>} playerList: the set of player colors that are obtained from the GUI control.
	*/
	void setAttribute(unsigned int numbeOfRows, unsigned int numberOfColumns, const std::vector<std::string>& playerList);
	/*
	* @brief Stores the resolution of the canvas object in member variables.
	* @param {unsigned int} width: width of the canvas.
	* @param {unsigned int} height: height of the canvas.
	*/
	void setCanvasSize(unsigned int width, unsigned int height);
	inline void setPixelScaling(double correctionFactor) {this->_pixelCorrectionFactor = correctionFactor;}
	/**
	* @brief Sets the frame object (which handles graphical user inputs) of the game instance.
	* @param {AppGUIFrame*} frame: pointer to the frame object.
	*/
	void setFrame(AppGUIFrame* frame);
	inline void setGridShaderVertexSource(std::string source) { this->_gridShaderVertexSource = source; }
	inline void setGridShaderFragmentSource(std::string source) { this->_gridShaderFragmentSource = source; }
	inline void setOrbShaderVertexSource(std::string source) { this->_orbShaderVertexSource = source; }
	inline void setOrbShaderFragmentSource(std::string source) { this->_orbShaderFragmentSource = source; }
	/*
	* @brief Configures the camera settings, e.g., updates projection matrix, modelview matrix, camera position, eye direction etc.
	*/
	void setupCamera();
	/*
	* @brief Undos the move of the last player and updates the game board accordingly.
	*/
	void undo(void);
	/*
	* @brief Updates the color of the gridlines and the player label in GUI in accordance with the current player.
	*/
	void updateTurn(void);
private:
	//*********************** Declaration of private member variables *************************
	double _angleOfRotation;
	ALuint _audioID, _audioSourceID;
	bool _areShadersInitialized = false;
	std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue> _BOARD;
	bool _blastAnimationRunning;
	double _blastDisplacement;
	double _BLAST_TIME;
	glm::vec3 _cameraPosition = {0.0f, 0.0f, 0.0f};
	glm::vec3 _cameraTarget = { 0.0f, 0.0f, 1.0f };
	glm::vec3 _cameraUp = { 0.0f, 1.0f, 0.0f };
	unsigned int _COL_DIV;
	static const std::unordered_map<std::string, glm::vec4> _colorMap;
	unsigned int _CUBE_WIDTH;
	std::unordered_map<GameUtilities::BoardKey, uint32_t> _currentAllNeighbours;
	std::vector<std::pair<GameUtilities::BoardKey, GameUtilities::BoardValue>> _currentBombs;
	unsigned int _DISPLAY[2]; //Stores the current dimension of the OpenGL canvas.
	std::vector<std::string> _eliminated; //Stores the colors (in lowercase) of the elminiated players.
	bool _gameHasEnded = false;
	IndexBufferObject _GRID_IBO_ID[5][5]; //Stores index_buffer_object for (row, col) grid data. Row and col range from 6 to 10, index 0 maps to 6.
	//Default fragment shader for rendering grid lines in the game board.
	std::string _gridShaderFragmentSource = "#version 400\r\n"
		"in vec4 fragmentColor;\r\n"
		"out vec4 color;\r\n"
		"uniform vec4 backsideColor;\r\n"
		"uniform vec4 frontsideColor;\r\n"
		"void main() {\r\n"
		"	color = clamp(fragmentColor.r * backsideColor + (1.0 - fragmentColor.r) * frontsideColor, 0.0, 1.0);\r\n"
		"}";
	Shaders _gridShaderProgram; //Compiled shader program that handles rendering of the grid lines in the game board.
	//Default vertex shader for rendering grid lines in the game board.
	std::string _gridShaderVertexSource = "#version 400\r\n"
		"layout(location = 0) in vec3 vertexPosition;\r\n"
		"layout(location = 1) in vec3 vertexNormal;\r\n"
		"layout(location = 2) in vec2 texCoord;\r\n"
		"layout(location = 3) in vec4 vertexColor;\r\n"
		"out vec4 fragmentColor;\r\n"
		"uniform mat4 transform;\r\n"
		"void main() {\r\n"
		"	gl_Position = transform * vec4(vertexPosition, 1.0);\r\n"
		"	fragmentColor = vertexColor;\r\n"
		"}";
	std::unordered_map<GameUtilities::BoardKey, VertexArrayObject*> _GRID_VAO_ID; //Stores vao_id for (row x column) grid data. Row and column range from 6 to 10, index 0 maps to 6.
	VertexBufferObject _GRID_VBO_ID[5][5]; //Stores vertex_buffer_object for (row, col) grid data. Row and col range from 6 to 10, index 0 maps to 6.
	float _lightDirLatitude = 0.0f;
	float _lightDirLongitude = 0.0f;
	double _lowerleft[2];
	AppGUIFrame* _mainframe = nullptr;
	glm::mat4 _modelview; //Stores the current modelview matrix.
	//Default fragment shader for rendering orbs in the game board.
	std::string _orbShaderFragmentSource = "#version 400\r\n"
		"in vec3 normalWorld;\r\n"
		"in vec4 vPosition;\r\n"
		"out vec4 outputColor;\r\n"
		"uniform vec3 cameraPosition;\r\n" //Current camera position
		"uniform vec4 orbColor;\r\n"
		"uniform vec3 vDiffuse;\r\n" //diffusive color property specification.
		"uniform vec3 vSpecular;\r\n" //specular color property specification.
		"uniform float vSpecularExponent;\r\n" //specular exponent property specification.
		"uniform vec3 lightDirection;\r\n" //only considers directional light;
		"void main(){\r\n"
		"	vec3 viewDir = normalize(cameraPosition - vec3(vPosition.xyz));\r\n"
		"	vec3 sunDir = normalize(lightDirection);\r\n"
		"	vec3 reflectionVector =  normalize(2 * dot(normalWorld, -sunDir) * normalWorld + sunDir);\r\n"
		"	float brightness = clamp(1.1 * dot(normalWorld, -sunDir), 0.0f, 1.0f);\r\n"
		"	vec3 diffuseComponent = vec3(orbColor.xyz);\r\n"
		"	vec3 specularComponent = vSpecular * pow(dot(reflectionVector, viewDir), vSpecularExponent);\r\n"
		"	vec3 color = clamp(diffuseComponent + specularComponent, 0.0f, 1.0f);\r\n"
		"	outputColor = vec4(color * brightness, 1.0f);\r\n"
		"}";
	Shaders _orbShaderProgram; //Compiled shader program that handles rendering of the orbs in the game.
	//Default vertex shader for rendering grid orbs in the game board.
	std::string _orbShaderVertexSource = "#version 400\r\n"
		"layout(location = 0) in vec3 vertexPosition;\r\n"
		"layout(location = 1) in vec3 vertexNormal;\r\n"
		"layout(location = 2) in vec2 texCoord;\r\n" //Texture coordinates are not used for now.
		"layout(location = 3) in vec4 vertexColor;\r\n"
		"out vec3 normalWorld;\r\n"
		"out vec4 vPosition;\r\n"
		"uniform mat4 projectionView;\r\n"
		"uniform mat4 modelTransform;\r\n"
		"void main() {\r\n"
		"	vPosition = modelTransform * vec4(vertexPosition, 1.0);\r\n"
		"	gl_Position = projectionView * vPosition;\r\n"
		"	normalWorld = normalize(vec3(modelTransform * vec4(vertexNormal, 0)));\r\n"
		"}";
	double _pixelCorrectionFactor = 1.0;
	std::vector<std::string> _players; //List of colors (in lowercase) of the current players.
	glm::mat4 _projection; //Stores the current projection (i.e., perspective) matrix.
	unsigned int _ROTATION_SPEED, _ROW_DIV;
	//UvSphere _sphere;
	IcoSphere _sphere;
	const MeshData* _sphereMeshData;
	unsigned int _turn;
	std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue> _undoBoard; //Stores the game-board configuration before the last input.
	std::vector<std::string> _undoEliminated; //Stores the list of (colors in lowercase) players that were eliminated before the last input.
	unsigned int _undoTurn; //Stores the value of the turn variable before the last input.
	unsigned int _winnerIndex = 0;
	std::string _winnerName = "";

	//******************************************* Private Method declaraction ******************************
	void _applyGridShaderSettings(void);
	void _applyOrbShaderSettings(void);
	/*
	* @brief Cantor pairing function to map (injectively) two 32 bit positive intergers to a single 64 bit positive integer.
	* @brief It can be used as a hash function to encode two positive integers into a single positive integer.
	* @see MainGame::_inverseKey.
	* @param {unsigned int} i, j : input value of the two positive integers.
	* @returns {unsigned long long} output value obtained from Cantor pairing.
	*/
	inline uint64_t _boardKey(uint32_t i, uint32_t j) const { return (uint64_t)(0.5 * (i + j) * (i + j + 1) + j); }
	static std::unordered_map<std::string, glm::vec4> _createColorMap();
	void _createGridData(unsigned int numberOfRows, unsigned int numberOfColumns, VertexArrayObject* vao, VertexBufferObject* vbo, IndexBufferObject* ibo, unsigned int cubeWidth, unsigned int centerX = 0, unsigned int centerY = 0);
	void _drawGrid();
	void _drawOrb(glm::vec3 center, glm::vec3 axes, double angleOfRotation, unsigned int level, const char* colorName);
	void _eliminatePlayers(bool updateTurnVariable);
	void _getWorldCoordinates(double x, double y, double& worldCoordinateX, double& worldCoordinateY, double& worldCoordinateZ);
	std::set<std::pair<uint32_t, uint32_t> > _getNeighbours(uint32_t boardCoordinateX, uint32_t boardCoordinateY);
	std::vector<std::pair<GameUtilities::BoardKey, GameUtilities::BoardValue>> _getBombs(const std::unordered_map<GameUtilities::BoardKey, GameUtilities::BoardValue>& board);
	std::unordered_map<GameUtilities::BoardKey, uint32_t> _getBombNeighbours();
	void _initDefaults(void);
	/**
	* @brief Inverse of the Cantor pairing function to map (injectively) a single 64 bit positive integer to two 32 bit positive intergers.
	* @see MainGame::_boardKey
	* @param {unsigned int} key : input value of the 64 bit positive integer.
	* @returns {pair<unsigned long long, unsigned long long>} output value obtained from inverse Cantor pairing.
	*/
	inline std::pair<uint32_t, uint32_t> _inverseKey(uint64_t key) const {
		const auto w = floor(0.5 * (sqrt(8 * key + 1) - 1));
		const auto t = 0.5 * (w + 1) * w;
		const uint32_t y = key - t;
		const uint32_t x = w - y;
		return std::make_pair(x, y);
	}
	/**
	* @brief Performs animation of an explosion if there are any.
	* @param {unsigned long} delta_t: average time (in miliseconds) taken to render each frame.
	* @returns {bool} true if the current game has ended (i.e., all orbs in the board are of same color) due to the explosion, false otherwise.
	*/
	bool _runBlastAnimation(unsigned long deltaTime);
	void _setColorOfBacksideGrid(float colorR, float colorG, float colorB);
	void _setColorOfFrontsideGrid(float colorR, float colorG, float colorB);
	/**
	* @brief Adds an audio source at the origin in the 3D world and uploads the audio data specified by sourceName to the source for using in the game play.
	* @param {const char*} sourceName: path to the audio data that is to be uploaded.
	* @returns true if the audio source was successfully created and the audio data was properly uploaded, false otherwise.
	*/
	bool _initSound(const char *sourceName);
	/**
	* @brief Adds an audio source at the origin in the 3D world and uploads the audio data from memory to the source for using in the game play.
	* @param {const void*} sound_source: pointer to the audio data in memory.
	* @param {unsigned int} size of the audio data in bytes.
	* @returns true if the audio source was successfully created and the audio data was properly uploaded, false otherwise.
	*/
	bool _initSound(const void* soundSource, size_t size);
};
#endif

