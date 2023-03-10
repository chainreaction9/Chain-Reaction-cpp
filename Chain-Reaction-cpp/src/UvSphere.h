#ifndef UVSPHERE_H
#define UVSPHERE_H

#include "PrecompiledHeader.h"
#include "Utilities.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"
#include "VertexLayout.h"
#include "MeshGenerator.h"

class UvSphere
{
public:
	UvSphere();
	~UvSphere();
	void init(float center_x, float center_y, float center_z, float radius, VertexStructure::Color color = VertexStructure::Color(255, 255, 255, 255), unsigned int lats = 32, unsigned int longs = 32);
	void draw(unsigned int level = 1); //level ranges from 1 to 3.
	inline float getRadius() const { return this->_radius; }
	inline glm::vec3 getCenter() const { return glm::vec3(this->_x, this->_y, this->_z); }
	inline const char* getColor() const { return _currentColor.c_str(); }
	inline void setColor(const char* color) { _currentColor = std::string(color); }
private:
	float _x, _y, _z, _radius;
	std::string _currentColor;
	unsigned int _elementCount; //number of vertices of a level 1 sphere
	VertexArrayObject* _vao; ////contains vertex array data for 3 level of spheres
	VertexBufferObject _vbo; //contains vertex buffer data for 3 level of spheres
};
#endif
