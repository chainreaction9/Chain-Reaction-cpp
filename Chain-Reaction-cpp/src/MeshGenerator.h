#pragma once
#ifndef MESH_GENERATOR_SHADER_SUPPORTED_H
#define MESH_GENERATOR_SHADER_SUPPORTED_H

#include "PrecompiledHeader.h"
#include "Shaders.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"
#include "VertexLayout.h"
#include "ObjLoader.h"

class MeshData {
public:
	MeshData();
	/**
	* @brief Destructor method for the MeshData class.
	* @brief Deletes the MeshEntry instances contained in the private variable <_meshList>.
	*/
	~MeshData();
	/**
	* @brief Loads .obj data from byte stream.
	* @brief Does not support material processing.
	* @param {const BYTE*} rawData: pointer to the byte stream containing .obj data.
	* @param {size_t} size: size of the obj data in byte length.
	* @returns {bool} true if the mesh data was loaded successfully, false otherwise.
	*/
	bool LoadFromMemory(const BYTE* source, size_t size);
	/**
	* @brief Loads .obj data from a source file.
	* @brief Supports material processing.
	* @param {const char*} fileName: Path to the source file.
	* @returns {bool} true if the mesh data was loaded successfully, false otherwise.
	*/
	bool LoadFromFile(const char* filename);
	/**
	* @brief Updates the default material property of MeshEntries in MeshData from material source specified by materialList.
	* @param {unordered_map<string, objl::Material>} materialList: a map to store all material objects with their names as the keys.
	* @returns true if any one of the material properties of MeshEntries is updated, false otherwise.
	*/
	bool updateDefaultMaterials(const std::unordered_map<std::string, objl::Material>& materialList);
	void render(Shaders& shaderProgram);
private:
	struct MeshEntry {
		VertexArrayObject* _vao;
		VertexBufferObject* _vbo;
		IndexBufferObject* _ibo;
		objl::Material _defaultMaterial;
		MeshEntry(const objl::Mesh& mesh);
		/**
		* @brief Destructor method for the MeshEntry struct.
		* @brief Deletes the OpenGL buffers allocated while creating this instance.
		*/
		~MeshEntry();
		/**
		* @brief Renders the MeshEntry instance which is a part of the MeshData class.
		**/
		void render(Shaders& shaderProgram);
	};
	objl::Loader _objLoader;
	std::vector<MeshEntry*> _meshList;
};
#endif //!MESH_GENERATOR_SHADER_SUPPORTED_H