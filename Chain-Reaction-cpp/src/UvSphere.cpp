#include "PrecompiledHeader.h" //precompiled header
#include "UvSphere.h"

UvSphere::UvSphere() : _x(0), _y(0), _z(0), _radius(1), _elementCount(0), _vao(nullptr), _currentColor("white"){

}

UvSphere::~UvSphere() {
	if (this->_vao!= nullptr) delete this->_vao;
	this->_vao = nullptr;
}

void UvSphere::init(float center_x, float center_y, float center_z, float radius, VertexStructure::Color color, unsigned int lats, unsigned int longs) {
	this->_x = center_x;
	this->_y = center_y;
	this->_z = center_z;
	this->_radius = radius;
	std::vector<VertexStructure::Vertex> vertices; //contains vertices of a level 1 sphere. We use this to construct other two level of spheres.
	unsigned int i, j;
	for (i = 0; i <= lats; i++) {
		double lat0 =  M_PI * (-0.5f + (double) (i - 1) / lats);
		float z0 = sin(lat0);
		float zr0 = cos(lat0);

		double lat1 =  M_PI * (-0.5f + (double) i / lats);
		float z1 = sin(lat1);
		float zr1 = cos(lat1);

		for (j = 0; j <= longs; j++) {
			double lng = 2 *  M_PI * (double) (j - 1) / longs;
			float x = cos(lng);
			float y = sin(lng);

			VertexStructure::Vertex a;
			a.position = VertexStructure::Position(_x + radius * x * zr1, _y + radius * y * zr1, _z + radius * z1);
			a.normal = VertexStructure::Position(x * zr1, y * zr1, z1);
			a.color = color;
			vertices.push_back(a);

			a.position = VertexStructure::Position(_x + radius * x * zr0, _y + radius * y * zr0, _z + radius * z0);
			a.normal = VertexStructure::Position(x * zr0, y * zr0, z0);
			vertices.push_back(a);
		}
	}
	this->_elementCount = vertices.size();


	// level 2 sphere
	std::vector<VertexStructure::Vertex> secondVertices; // contains vertices of a level 2 sphere.
	double displacement = (2 * radius) / 3.0f;
	glm::mat4 translateLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-displacement, 0.0f, 0.0f));
	glm::mat4 translateRight = glm::translate(glm::mat4(1.0f), glm::vec3(displacement, 0.0f, 0.0f));
	
	for (unsigned int i = 0; i != vertices.size(); i++) {
		VertexStructure::Vertex v = vertices[i];
		VertexStructure::Position pos = v.position;
		glm::vec4 leftPosition = translateLeft * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
		v.position = VertexStructure::Position(leftPosition.x, leftPosition.y, leftPosition.z);
		secondVertices.insert(secondVertices.begin() + i, v);
		glm::vec4 rightPosition = translateRight * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
		v.position = VertexStructure::Position(rightPosition.x, rightPosition.y, rightPosition.z);
		secondVertices.push_back(v);
	}

	// level 3 sphere
	std::vector<VertexStructure::Vertex> thirdVertices; // contains vertices of a level 2 sphere.
	glm::mat4 translateTop = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, radius, 0.0f));
	translateLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-radius * cos(M_PI / 6), -0.5f * radius, 0.0f));
	translateRight = glm::translate(glm::mat4(1.0f), glm::vec3(radius * cos(M_PI / 6), -0.5f * radius, 0.0f));

	for (unsigned int i = 0; i != vertices.size(); i++) {
		VertexStructure::Vertex v = vertices[i];
		VertexStructure::Position pos = v.position;
		glm::vec4 leftPosition = translateLeft * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
		v.position = VertexStructure::Position(leftPosition.x, leftPosition.y, leftPosition.z);
		thirdVertices.insert(thirdVertices.begin() + i, v);
		glm::vec4 rightPosition = translateRight * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
		v.position = VertexStructure::Position(rightPosition.x, rightPosition.y, rightPosition.z);
		thirdVertices.push_back(v);
	}
	for (unsigned int i = 0; i != vertices.size(); i++) {
		VertexStructure::Vertex v = vertices[i];
		VertexStructure::Position pos = v.position;
		glm::vec4 topPosition = translateTop * glm::vec4(pos.x, pos.y, pos.z, 1);
		v.position = VertexStructure::Position(topPosition.x, topPosition.y, topPosition.z);
		thirdVertices.push_back(v);
	}

	std::vector<VertexStructure::Vertex> allVertices; //contains all vertex data in one buffer
	allVertices.assign(vertices.begin(), vertices.end());
	allVertices.insert(allVertices.end(), secondVertices.begin(), secondVertices.end());
	allVertices.insert(allVertices.end(), thirdVertices.begin(), thirdVertices.end());

	this->_vao = new VertexArrayObject();
	//All references to vbo and the vbo layout are saved in the currently bound vertex array _vaoID
	this->_vbo.loadNewBuffer(&allVertices[0], (unsigned int) allVertices.size() * sizeof(VertexStructure::Vertex));
	VertexLayout layout; //layout: position, normal, color
	layout.addLayout(GL_FLOAT, 3, GL_FALSE);
	layout.addLayout(GL_FLOAT, 3, GL_FALSE);
	layout.addLayout(GL_FLOAT, 2, GL_FALSE);
	layout.addLayout(GL_UNSIGNED_BYTE, 4, GL_TRUE);
	this->_vao->setVertexBuffer(this->_vbo, layout);
}
void UvSphere::draw(unsigned int level) {
	GLint offsets = 0;
	if (level == 2) offsets = _elementCount;
	else if (level == 3) offsets = 3 * _elementCount;
	this->_vao->bind();
	for (unsigned int i = 0; i < this->_vao->numAttr(); i++) {
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, offsets, _elementCount * level); //This does not utilise index buffer. Hence the draw call is glDrawArrays instead of glDrawElements
	for (unsigned int i = 0; i < this->_vao->numAttr(); i++) {
		glDisableVertexAttribArray(i);
	}
	this->_vao->unbind();
}