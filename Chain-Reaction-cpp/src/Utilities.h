#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include "Text.h"

namespace OnlineUtilities {
	std::string len_to_string(int length);
	int string_to_len(std::string str);
	std::vector<std::string> break_string(std::string stream);
	std::string add_string(std::vector<std::string> list);
};
namespace VertexStructure {
	typedef struct Position {
		Position(): x(0), y(0), z(0) {}
		Position(GLfloat _x, GLfloat _y, GLfloat _z);
		GLfloat x;
		GLfloat y;
		GLfloat z;
	} Position;
	typedef struct Position2D {
		Position2D(): x(0), y(0) {}
		Position2D(GLfloat _x, GLfloat _y);
		GLfloat x;
		GLfloat y;
	} Position2D;
	/*
	* @brief A structure to contain RGBA components of a pixel.
	*/
	typedef struct Color {
		GLubyte r;
		GLubyte g;
		GLubyte b;
		GLubyte a;
		Color(): r(0), g(0), b(0), a(0) {}
		Color(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a);
	} Color;
	/*
	* @brief A structure to contain vertex attributes such as the 3D position, 3D normal, 2D texture coordinates, RGBA color, etc.
	* @brief It has a layout: vec3, vec3, vec2, vec4; Total byte size of a vertex: (3 + 3 + 2) * 4 + 4 = 36 bytes.
	*/
	typedef struct Vertex {
		Position position, normal;
		Position2D texCoord;
		Color color;
		Vertex() : position(0, 0, 0), normal(0, 1, 0), texCoord(0, 0), color(255, 255, 255, 255) {};
	} Vertex;
}
namespace ResourceUtilities {
	/**
	* @brief A struct object to store binary data of a file.
	* @brief Members of the struct:
	* @brief 1. {string} name: Name of the file associated with this data.
	* @brief 2. {unsigned char*} data: a pointer to the raw binary data.
	* @brief 3. {size_t} size: size of the data in bytes.
	*/
	typedef struct FileData {
		std::string name; // Name of the file
		unsigned char* data; // Pointer to the raw binary data.
		size_t size; // Size of the data in byte lengths.
		FileData() : name(""), data(nullptr), size(0) {};
	} FileData;
	/*
	* @brief A structure for storing resource data (e.g., 3d object, default material properties etc.)
	* which can be passed on to an OpenGL canvas object for further processing.
	* @brief Member of the struct:
	* @brief 1. {std::vector<FileData*>} listOfObjectFiles: list containing object data of 3D models for further processing.
	* @brief 2. {std::unordered_map<std::string, objl::Material>} listOfAllAvailableMaterials: list containing all available materials that can be used to render a 3D model.
	* @brief 3. {Material} defaultSphereMaterial: a Material object to specify default material properties in case a mesh data does not contain any.
	* @brief 4. {const char*} particleShaderVertexSource: raw source code of the vertex shader for rendering the brownian particles. If none is given, a hard-coded default shader is used.
	* @brief 5. {const char*} particleShaderFragmentSource: raw source code of the fragment shader for rendering the brownian particles. If none is given, a hard-coded default shader is used.
	* @brief 6. {const char*} textShaderVertexSource: raw source code of the vertex shader for rendering 3D text objects. If none is given, a hard-coded default shader is used.
	* @brief 7. {const char*} textShaderFragmentSource: raw source code of the fragment shader for rendering 3D text objects. If none is given, a hard-coded default shader is used.
	*/
	typedef struct ResourceData {
		std::vector<FileData*> listOfObjectFiles;
		std::unordered_map<std::string, objl::Material> listOfAllAvailableMaterials;
		objl::Material defaultSphereMaterial; //Default material used for rendering brownian particles.
		const char* particleShaderVertexSource = nullptr;
		const char* particleShaderFragmentSource = nullptr;
		const char* textShaderVertexSource = nullptr;
		const char* textShaderFragmentSource = nullptr;
	} ResourceData;
	/*
	* @brief Extracts zip source contained in rawData to the storageLocation.
	* @param {const void *} rawData: A pointer to the raw data that is stored in memory.
	* @param {size_t} sizeOfData: Size (in bytes) of the data stored in rawData.
	* @param {vector<FileData>} storageLocation: A vector of data type 'FileData' which is used as a storage container for extracting the zip data specified by rawData.
	* @param {set<string>} fileNameList: A set of data type 'string' that contains the file names in the zip.
	* @returns {bool} true if the zip extraction was successful, false otherwise.
	*/
	bool extractZipResource(const void* rawData, size_t sizeOfData, std::vector<FileData*>& storageLocation, const std::set<std::string>& fileNameList, wxProgressDialog* progressDialog = nullptr);
	/*
	* @brief Extracts a zip file.
	* @param {const char *} pathToZipFile: Path to the zip file.
	* @param {vector<FileData>} storageLocation: A vector of data type 'FileData' which is used as a storage container for extracting the zip data specified by rawData.
	* @param {set<string>} fileNameList: A set of data type 'string' that contains the file names in the zip.
	* @returns {bool} true if the zip extraction was successful, false otherwise.
	*/
	bool extractZipResource(const char* pathToZipFile, std::vector<FileData*>& storageLocation, const std::set<std::string>& fileNameList, wxProgressDialog* progressDialog = nullptr);
	/*
	* @brief Deletes the data contained in storage.
	* @param {vector<FileData*>} storage: A reference to a vector containing all the file data extracted from a zip.
	*/
	void clearZipStorage(std::vector<FileData*>& storage);
	/*
	* @brief Deletes the MeshData objects contained in dataSet.
	* @param {unordered_map<string, MeshData>} dataSet: An unordered map that points each mesh name to its raw data.
	*/
	void clearMeshStorage(std::unordered_map<std::string, MeshData*>& dataSet);
}
namespace GameUtilities {
	typedef uint64_t BoardKey;
	typedef struct BoardValue {
		unsigned int level;
		std::string color;
		glm::vec3 rotationAxes;
		std::pair<uint32_t, uint32_t> boardCoordinate; //stores the board coordinate to which this value belongs.
		BoardValue() : level(1), color(""), rotationAxes(0.0f, 0.0f, 0.0f), boardCoordinate(0, 0) {};
	} BoardValue;
	/**
	* @file AppGLCanvas.h
	* @brief A structure for storing runtime game information.
	* @brief Member variables:
	* @brief	1. {bool} game_started: possible values: true or false. Value false indicates an idle state with no game running, true indicates the start of a game.
	* @brief	2. {bool} game_ended: possible values: false or true. Value false indicates that a game has yet not started or is still running. While true indicates that a running game has just ended.
	* @brief	3. {bool} mouse_data_processed: possible values false or true. Value false indicates that mouse data obtained from user input is not yet processed. Value true indicates mouse data is already processed and is ready to process new data.
	* @brief	4. {bool} new_game: possible values false or true. Value true indicates that user wants to stop a running game and start a new one. Default value is false.
	* @brief	5. {int} go_back_main: possible values {0, 1, 2, 3}. Value 1 indicates that user wants to stop a running game and go back to the main menu. Default value is 0. Other two values are relevant for the option of online play.
	* @brief	6. {int} network_status: Possible values are defined in NETWORK_STAT enum. It is used for communicating with online game server.
	* @brief	7. {bool} online: boolean value. true indicates that the current game is online, false indicates an offline game.
	* @brief	8. {void *} game: pointer to the object associated with an offline game.
	* @brief	9. {void *} online_game: pointer to the object associated with an online game.
	*/
	typedef struct GameState {
		bool game_started;
		bool game_ended;
		bool mouse_data_processed;
		bool new_game;
		int go_back_main; //must be of integer type instead of boolean as it has more than 2 possible values.
		int network_status;
		bool isOnline;
		void *game;
		void *online_game;
		GameState() :game_started(0), game_ended(1), mouse_data_processed(0), new_game(0), go_back_main(0), network_status(0), isOnline(false), game(nullptr), online_game(nullptr) {};
	} GameState;
	/**
	* @brief A structure for storing properties of a brownian particle.
	* @brief Member variables:
	* @brief	1. {vec3} position: position of the (center of mass) particle in 3d world.
	* @brief	2. {vec3} color: color of the particle.
	* @brief	3. {uint8_t} level: level of the particle. Possible values: {1,2,3}.
	* @brief	4. {bool} isFlickering: boolean flag to determine whether the particle should change color over time.
	*/
	typedef struct Particle {
		glm::vec3 position;
		glm::vec3 color;
		uint8_t level;
		bool isFlickering;
		Particle() : position(0, 0, 0), color(1.0f, 1.0f, 1.0f), level(1), isFlickering(false) {};
	} Particle;
}
#endif // !UTILITIES_H
