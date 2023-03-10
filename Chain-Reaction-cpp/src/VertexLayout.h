#ifndef LAYOUT_H
#define LAYOUT_H

#include "PrecompiledHeader.h"

typedef struct Layout {
	Layout(GLenum _type, unsigned int _count, GLboolean _normalized);
	GLenum type;
	unsigned int count;
	GLboolean normalize;
	static unsigned int GetSize(GLenum type);
	Layout(): type(GL_FLOAT), count(0), normalize(GL_FALSE) {}
} Layout;

class VertexLayout {
public:
	VertexLayout();
	~VertexLayout();
	void addLayout(GLenum type, unsigned int count, GLboolean normalize);
	void addLayout(Layout layout);
	inline unsigned int getStride() { return _stride; }
	inline unsigned int getNumLayout() { return _allLayouts.size(); }
	inline std::vector<Layout> getLayouts() { return _allLayouts; }

private:
	std::vector<Layout> _allLayouts;
	unsigned int _stride;
};

#endif //!LAYOUT_H
