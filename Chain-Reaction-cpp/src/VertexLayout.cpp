#include "PrecompiledHeader.h" //precompiled header
#include "VertexLayout.h"

Layout::Layout(GLenum _type, unsigned int _count, GLboolean _normalize) {
	type = _type;
	count = _count;
	normalize = _normalize;
};
unsigned int Layout::GetSize(GLenum type) {
	switch (type) {
		case GL_FLOAT:			return sizeof(GLfloat);
		case GL_UNSIGNED_INT:	return sizeof(GLuint);
		case GL_UNSIGNED_BYTE:	return sizeof(GLubyte);
		default:				return sizeof(GLfloat);
	}
}

VertexLayout::VertexLayout() : _stride(0) {

}
VertexLayout::~VertexLayout() {

}

void VertexLayout::addLayout(GLenum type, unsigned int count, GLboolean normalize) {
	Layout l(type, count, normalize);
	_allLayouts.push_back(l);
	_stride += count * Layout::GetSize(type);
}
void VertexLayout::addLayout(Layout layout) {
	_allLayouts.push_back(layout);
	_stride += layout.count * Layout::GetSize(layout.type);
}