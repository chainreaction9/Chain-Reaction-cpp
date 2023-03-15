/*
This file is a refactored version of the javascript code available at https://github.com/mourner/icomesh.git
*/
#pragma once
#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#include "Utilities.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"
#include "VertexLayout.h"
#include "MeshGenerator.h"

class IcoSphere
{
public:
	IcoSphere();
	~IcoSphere();
	inline float getRadius() const { return this->_radius; }
	inline glm::vec3 getCenter() const { return this->_center; }
	void init(glm::vec3 center, float radius, uint32_t order, bool generateTextureCoord = false, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	void draw(uint8_t level = 1);
	inline const char* getColor() const { return _currentColor.c_str(); }
	inline void setColor(const char* color) { _currentColor = std::string(color); }
private:
	glm::vec3 _center = { 0, 0, 0 };
	std::string _currentColor = "white";
	float _radius = 1.0f;
	uint32_t _order = 3;
	uint32_t _elementCount = 0; //number of vertices of a level 1 sphere
	VertexArrayObject* _vao = nullptr;; ////contains vertex array data for 3 level of spheres
	VertexBufferObject _vbo; //contains vertex buffer data for 3 level of spheres
	IndexBufferObject* _iboForLevelOne;
	IndexBufferObject* _iboForLevelTwo;
	IndexBufferObject* _iboForLevelThree;
	void _generateData(uint32_t order, std::vector<double>& verticesDestination, std::vector<uint32_t>& trianglesDestination, std::vector<double>& textureDestination, bool generateTextureCoord = false);
	uint32_t _addMidPoint(uint32_t a, uint32_t b, uint32_t& v, std::unordered_map<uint32_t, uint32_t>& midCache, std::vector<double>& vertices);
	uint32_t _addDuplicate(uint32_t i, double uvx, double uvy, bool cached, uint32_t &v, std::unordered_map<uint32_t, uint32_t>& duplicates, std::vector<double>& vertices, std::vector<double>& uv);
};
#endif //!ICOSPHERE_H
