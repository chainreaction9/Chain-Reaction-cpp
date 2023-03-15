#include "PrecompiledHeader.h" //precompiled header
#include "VertexBufferObject.h"

VertexBufferObject::VertexBufferObject() : _vboID(0), _size(0) {}
VertexBufferObject::VertexBufferObject(const void* data, unsigned int size) : _vboID(0), _size(0) {
	glGenBuffers(1, &_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	_size = size;
	wxLogDebug(wxString::Format("[Chain-Reaction] Created OpenGL vertex buffer %d at address %p ...", _vboID, &_vboID));
}
VertexBufferObject::~VertexBufferObject() {
	if (_vboID != 0) {
		wxLogDebug(wxString::Format("[Chain-Reaction] Freed memory allocated to OpenGL vertex buffer %d at address %p ...", _vboID, &_vboID));
		glDeleteBuffers(1, &_vboID);
	}
}

void VertexBufferObject::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
}
void VertexBufferObject::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VertexBufferObject::deleteBuffer() {
	if (_vboID != 0) {
		glDeleteBuffers(1, &_vboID);
		_vboID = 0;
		_size = 0;
	}
}
bool VertexBufferObject::isEmpty() {
	if (_vboID != 0) return false;
	else return true;
}
void VertexBufferObject::loadNewBuffer(const void* data, unsigned int size) {
	glDeleteBuffers(1, &_vboID);
	glGenBuffers(1, &_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	_size = size;
}