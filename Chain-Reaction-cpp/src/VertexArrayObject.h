#ifndef VAO_H
#define VAO_H

#include "PrecompiledHeader.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"
#include "VertexLayout.h"

class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();
	void CreateArray();
	inline void bind() const { glBindVertexArray(_vaoID); }
	inline void unbind() const { glBindVertexArray(0); }
	inline GLuint numIndices() const { return _numIndices; }
	inline GLuint numAttr() const { return _numAttributes; }
	void setVertexBuffer(const VertexBufferObject& vb, VertexLayout layout);
	void setIndexBuffer(const IndexBufferObject& ib);
private:
	GLuint _vaoID;
	GLuint _numAttributes, _numIndices;
};
#endif