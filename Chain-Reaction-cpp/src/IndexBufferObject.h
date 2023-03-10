#ifndef IBO_H
#define IBO_H

#include "PrecompiledHeader.h"

class IndexBufferObject {
public:
	IndexBufferObject();
	IndexBufferObject(const GLuint* data, unsigned int count);
	~IndexBufferObject();

	void bind() const;
	void unbind() const;
	void deleteBuffer();
	bool isEmpty();
	inline unsigned int getCount() const { return _count; }
	void loadNewBuffer(const GLuint* data, unsigned int count); //delete old data and then store
private:
	GLuint _iboID;
	unsigned int _count;
};

#endif