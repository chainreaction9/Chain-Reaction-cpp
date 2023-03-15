#include "PrecompiledHeader.h" //precompiled header
#include "IndexBufferObject.h"

IndexBufferObject::IndexBufferObject() : _iboID(0), _count(0) {}
IndexBufferObject::IndexBufferObject(const GLuint* data, unsigned int count) : _iboID(0), _count(0) {
	glGenBuffers(1, &_iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), (const void *) data, GL_STATIC_DRAW);
	_count = count;
	wxLogDebug(wxString::Format("[Chain-Reaction] Created OpenGL index buffer %d at address %p ...", _iboID, &_iboID));
}
IndexBufferObject::~IndexBufferObject() {
	if (_iboID != 0) {
		wxLogDebug(wxString::Format("[Chain-Reaction] Freed memory allocated to OpenGL index buffer %d at address %p ...", _iboID, &_iboID));
		glDeleteBuffers(1, &_iboID);
	}
}

void IndexBufferObject::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID);
}
void IndexBufferObject::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void IndexBufferObject::deleteBuffer() {
	if (_iboID != 0) {
		glDeleteBuffers(1, &_iboID);
		_iboID = 0;
		_count = 0;
	}
}
bool IndexBufferObject::isEmpty() {
	if (_iboID != 0) return false;
	else return true;
}
void IndexBufferObject::loadNewBuffer(const GLuint* data, unsigned int count) {
	glDeleteBuffers(1, &_iboID);
	glGenBuffers(1, &_iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), (const void*) data, GL_STATIC_DRAW);
	_count = count;
}