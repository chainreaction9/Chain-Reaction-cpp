#ifndef VBO_H
#define VBO_H

#include "PrecompiledHeader.h"

class VertexBufferObject{
public:
	VertexBufferObject();
	VertexBufferObject(const void *data, unsigned int size);
	~VertexBufferObject();

	void bind() const;
	void unbind() const;
	void deleteBuffer();
	bool isEmpty();
	inline unsigned int getSize() { return _size; }
	void loadNewBuffer(const void* data, unsigned int size); //delete old data and then store
private:
	GLuint _vboID;
	unsigned int _size;
};
#endif //!VBO_H