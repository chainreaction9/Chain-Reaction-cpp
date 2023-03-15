#include "PrecompiledHeader.h" //precompiled header
#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject(): _vaoID(0), _numAttributes(1), _numIndices(0) {
	glGenVertexArrays(1, &_vaoID);
	wxLogDebug(wxString::Format("[Chain-Reaction] Created OpenGL vertex array %d at address %p ...", _vaoID, &_vaoID));
}
VertexArrayObject::~VertexArrayObject() {
	if (_vaoID != 0) {
		wxLogDebug(wxString::Format("[Chain-Reaction] Freed memory allocated to OpenGL vertex array %d at address %p ...", _vaoID, &_vaoID));
		glDeleteVertexArrays(1, &_vaoID);
	}
}
void VertexArrayObject::CreateArray() {
	if (_vaoID == 0) glGenVertexArrays(1, &_vaoID);
}
void VertexArrayObject::setVertexBuffer(const VertexBufferObject& vb, VertexLayout layout) {
	bind();
	vb.bind();
	_numAttributes = layout.getNumLayout();
	const auto& layoutList = layout.getLayouts();
	GLuint64 offset = 0;
	for (GLuint i = 0; i < layoutList.size(); i++) {
		Layout l = layoutList[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, l.count, l.type, l.normalize, layout.getStride(), (void *) offset);
		offset += l.count * Layout::GetSize(l.type);
		glDisableVertexAttribArray(i);
	}
	unbind();
	vb.unbind();
}
void VertexArrayObject::setIndexBuffer(const IndexBufferObject& ib) {
	bind();
	ib.bind();
	unbind();
	ib.unbind();
	_numIndices = ib.getCount();

}