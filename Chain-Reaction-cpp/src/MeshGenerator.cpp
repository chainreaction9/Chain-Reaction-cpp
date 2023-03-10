#include "PrecompiledHeader.h" //precompiled header
#include "MeshGenerator.h"

MeshData::MeshData() {

}
MeshData::~MeshData() {
	for (auto entry : this->_meshList) {
		delete entry;
	}
	this->_meshList.clear();
}
MeshData::MeshEntry::MeshEntry(const objl::Mesh& mesh) {
	this->_vao = new VertexArrayObject();
	this->_vbo = new VertexBufferObject();
	this->_ibo = new IndexBufferObject();
	this->_defaultMaterial = mesh.MeshMaterial;
	this->_vbo->loadNewBuffer(&mesh.Vertices[0], mesh.Vertices.size() * sizeof(objl:: Vertex));
	VertexLayout layout;
	layout.addLayout(GL_FLOAT, 3, GL_FALSE); // Position Coordinates
	layout.addLayout(GL_FLOAT, 3, GL_FALSE); // Normal Coordinates
	layout.addLayout(GL_FLOAT, 2, GL_FALSE); // Texture Coordinates
	this->_vao->setVertexBuffer(*_vbo, layout);
	this->_ibo->loadNewBuffer(&mesh.Indices[0], mesh.Indices.size());
	this->_vao->setIndexBuffer(*_ibo);
}
MeshData::MeshEntry::~MeshEntry() {
	delete this->_vao;
	delete this->_vbo;
	delete this->_ibo;
}
void MeshData::MeshEntry::render(Shaders& shaderProgram) {
	shaderProgram.applyMaterial(this->_defaultMaterial);
	this->_vao->bind();
	for (GLuint i = 0; i < this->_vao->numAttr(); i++) {
		glEnableVertexAttribArray(i);
	}
	glDrawElements(GL_TRIANGLES, this->_vao->numIndices(), GL_UNSIGNED_INT, nullptr);
	for (GLuint i = 0; i < this->_vao->numAttr(); i++) {
		glDisableVertexAttribArray(i);
	}
	this->_vao->unbind();
}
bool MeshData::LoadFromFile(const char* fileName) {
	bool success = this->_objLoader.LoadFile(fileName);
	if (success) {
		for (const auto& rawEntry : this->_objLoader.LoadedMeshes) {
			MeshEntry* entry = new MeshEntry(rawEntry);
			this->_meshList.push_back(entry);
		}
		return success;
	}
	return false;
}
bool MeshData::LoadFromMemory(const BYTE* source, size_t size) {
	bool success = this->_objLoader.LoadFromMemory(source, size);
	if (success) {
		for (const auto& rawEntry : this->_objLoader.LoadedMeshes) {
			MeshEntry* entry = new MeshEntry(rawEntry);
			this->_meshList.push_back(entry);
		}
		return true;
	}
	return false;
}
bool MeshData::updateDefaultMaterials(const std::unordered_map<std::string, objl::Material>& materialList) {
	bool success = false;
	for (auto meshEntry : this->_meshList) {
		for (const auto& dataEntry : materialList) {
			std::string meshName = meshEntry->_defaultMaterial.name;
			if (!meshName.empty() && meshName[meshName.size() - 1] == '\r')
				meshName.erase(meshName.size() - 1);
			if (!(dataEntry.first.compare(meshName))) {
				meshEntry->_defaultMaterial = dataEntry.second;
				success = true;
				break;
			}
		}
	}
	return success;
}
void MeshData::render(Shaders& shaderProgram) {
	for (const auto& entry : this->_meshList) {
		entry->render(shaderProgram);
	}
}