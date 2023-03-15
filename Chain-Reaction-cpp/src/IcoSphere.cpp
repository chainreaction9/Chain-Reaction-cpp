/*
This file is a refactored version of the typescript code available at https://github.com/mourner/icomesh.git
*/
#include "PrecompiledHeader.h"
#include "IcoSphere.h"

IcoSphere::IcoSphere(){
    this->_vao = nullptr;
    this->_iboForLevelOne = this->_iboForLevelTwo = this->_iboForLevelThree = nullptr;
    this->_elementCount = 0;
}

IcoSphere::~IcoSphere(){
    if (this->_vao != nullptr) {
        delete this->_vao;
        this->_vao = nullptr;
    }
    if (this->_iboForLevelOne != nullptr) {
        delete this->_iboForLevelOne;
        this->_iboForLevelOne = nullptr;
    }
    if (this->_iboForLevelTwo != nullptr) {
        delete this->_iboForLevelTwo;
        this->_iboForLevelTwo = nullptr;
    }
    if (this->_iboForLevelThree != nullptr) {
        delete this->_iboForLevelThree;
        this->_iboForLevelThree = nullptr;
    }
}
void IcoSphere::init(glm::vec3 center, float radius, uint32_t order, bool generateTextureCoord, glm::vec4 color){
    this->_center = center;
    this->_radius = radius;
    this->_order = order;
    std::vector<double> vertices;
    std::vector<double> textures;
    std::vector<uint32_t> indices;
    if (!generateTextureCoord) {
        this->_generateData(_order, vertices, indices, textures, false);
    }
    else this->_generateData(_order, vertices, indices, textures, generateTextureCoord);
    std::vector<VertexStructure::Vertex> interleavedVertices; //contains vertices of a level 1 sphere. We use this to construct other two level of spheres.
    uint32_t numVertices = vertices.size() / 3.0;
    for (uint32_t i = 0; i < numVertices; i++) {
        VertexStructure::Vertex v;
        v.position.x = this->_center.x + this->_radius * vertices[3 * i];
        v.position.y = this->_center.y + this->_radius * vertices[3 * i + 1];
        v.position.z = this->_center.z + this->_radius * vertices[3 * i + 2];
        v.color.r = color.r; v.color.g = color.g; v.color.b = color.b; v.color.a = color.a;
        v.normal.x = v.position.x; v.normal.y = v.position.y; v.normal.z = v.position.z;
        if (generateTextureCoord) {
            v.texCoord.x = textures[2 * i];
            v.texCoord.y = textures[2 * i + 1];
        }
        else { v.texCoord.x = 0; v.texCoord.y = 0; }
        interleavedVertices.push_back(v);
    }
    this->_elementCount = interleavedVertices.size();
    this->_iboForLevelOne = new IndexBufferObject();
    this->_iboForLevelOne->loadNewBuffer(&indices[0], indices.size());
    
    // level 2 sphere
    std::vector<VertexStructure::Vertex> interleavedSecondVertices; // contains vertices of a level 2 sphere.
    float displacement = (2 * this->_radius) / 3.0f;
    glm::mat4 translateLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-displacement, 0, 0));
    glm::mat4 translateRight = glm::translate(glm::mat4(1.0f), glm::vec3(displacement, 0, 0));

    for (uint32_t i = 0; i != interleavedVertices.size(); i++) {
        VertexStructure::Vertex a = interleavedVertices[i];
        VertexStructure::Position pos = a.position;
        glm::vec4 leftPosition = translateLeft * glm::vec4(pos.x, pos.y, pos.z, 1);
        a.position = VertexStructure::Position(leftPosition.x, leftPosition.y, leftPosition.z);
        interleavedSecondVertices.insert(interleavedSecondVertices.begin() + i, a);
        glm::vec4 rightPosition = translateRight * glm::vec4(pos.x, pos.y, pos.z, 1);
        a.position = VertexStructure::Position(rightPosition.x, rightPosition.y, rightPosition.z);
        interleavedSecondVertices.push_back(a);
    }
    std::vector<uint32_t> secondIndices; //contains indices for level 2 sphere
    for (int j = 0; j < 2; j++) {
        for (uint32_t i = 0; i < indices.size(); i++) {
            secondIndices.push_back(indices[i] + (j + 1) * this->_elementCount);
        }
    }
    this->_iboForLevelTwo = new IndexBufferObject();
    this->_iboForLevelTwo->loadNewBuffer(&secondIndices[0], secondIndices.size());
    // level 3 sphere
    std::vector<VertexStructure::Vertex> interleavedThirdVertices; // contains vertices of a level 2 sphere.
    glm::mat4 translateTop = glm::translate(glm::mat4(1.0f), glm::vec3(0, this->_radius, 0));
    translateLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-this->_radius * cos(M_PI / 6), -0.5 * this->_radius, 0));
    translateRight = glm::translate(glm::mat4(1.0f), glm::vec3(this->_radius * cos(M_PI / 6), -0.5 * this->_radius, 0));

    for (uint32_t i = 0; i != interleavedVertices.size(); i++) {
        VertexStructure::Vertex a = interleavedVertices[i];
        VertexStructure::Position pos = a.position;
        glm::vec4 leftPosition = translateLeft * glm::vec4(pos.x, pos.y, pos.z, 1);
        a.position = VertexStructure::Position(leftPosition.x, leftPosition.y, leftPosition.z);
        interleavedThirdVertices.insert(interleavedThirdVertices.begin() + i, a);
        glm::vec4 rightPosition = translateRight * glm::vec4(pos.x, pos.y, pos.z, 1);
        a.position = VertexStructure::Position(rightPosition.x, rightPosition.y, rightPosition.z);
        interleavedThirdVertices.push_back(a);
    }
    for (uint32_t i = 0; i != interleavedVertices.size(); i++) {
        VertexStructure::Vertex a = interleavedVertices[i];
        glm::vec4 topPosition = translateTop * glm::vec4(a.position.x, a.position.y, a.position.z, 1);
        a.position = VertexStructure::Position(topPosition.x, topPosition.y, topPosition.z);
        interleavedThirdVertices.push_back(a);
    }
    std::vector<uint32_t> thirdIndices; //contains indices for level 2 sphere
    for (int j = 0; j < 3; j++) {
        for (uint32_t i = 0; i < indices.size(); i++) {
            thirdIndices.push_back(indices[i] + (j + 3) * this->_elementCount);
        }
    }
    this->_iboForLevelThree = new IndexBufferObject();
    this->_iboForLevelThree->loadNewBuffer(&thirdIndices[0], thirdIndices.size());

    //*********************************************************************************************
    std::vector<VertexStructure::Vertex> allVertices; //contains vertices for all vertex data in one buffer
    allVertices.assign(interleavedVertices.begin(), interleavedVertices.end());
    allVertices.insert(allVertices.end(), interleavedSecondVertices.begin(), interleavedSecondVertices.end());
    allVertices.insert(allVertices.end(), interleavedThirdVertices.begin(), interleavedThirdVertices.end());

    this->_vao = new VertexArrayObject();
    //All references to vbo and the vbo layout are saved in the currently bound vertex array _vaoID
    this->_vbo.loadNewBuffer(&allVertices[0], (uint32_t)allVertices.size() * sizeof(VertexStructure::Vertex));
    VertexLayout layout; //layout: position, normal, color
    layout.addLayout(GL_FLOAT, 3, GL_FALSE);
    layout.addLayout(GL_FLOAT, 3, GL_FALSE);
    layout.addLayout(GL_FLOAT, 2, GL_FALSE);
    layout.addLayout(GL_UNSIGNED_BYTE, 4, GL_TRUE);
    this->_vao->setVertexBuffer(this->_vbo, layout);
}
void IcoSphere::draw(uint8_t level) {
    if (level == 1) this->_vao->setIndexBuffer(*this->_iboForLevelOne);
    else if (level == 2) this->_vao->setIndexBuffer(*this->_iboForLevelTwo);
    else this->_vao->setIndexBuffer(*this->_iboForLevelThree);
    this->_vao->bind();
    for (GLuint i = 0; i < _vao->numAttr(); i++) {
        glEnableVertexAttribArray(i);
    }
    glDrawElements(GL_TRIANGLES, this->_vao->numIndices(), GL_UNSIGNED_INT, nullptr); //This does not utilise index buffer. Hence the draw call is glDrawArrays instead of glDrawElements
    for (GLuint i = 0; i < _vao->numAttr(); i++) {
        glDisableVertexAttribArray(i);
    }
    _vao->unbind();
}
void IcoSphere::_generateData(uint32_t order, std::vector<double>& verticesDestination, std::vector<uint32_t>& trianglesDestination, std::vector<double>& textureDestination, bool generateTextureCoord) {
    assert(order <= 10);

    // set up an icosahedron (12 vertices / 20 triangles)
    const double f = (1 + sqrt(5)) / 2;
    const uint64_t T = pow(4, order);

    const uint64_t numVertices = 10 * T + 2;
    uint64_t numDuplicates = 0;
    if (generateTextureCoord) {
        if (order == 0) numDuplicates = 3;
        else numDuplicates = pow(2, order) * 3 + 9;
    }
    std::vector<double> vertices;
    vertices.assign({
        -1, f, 0, 1, f, 0, -1, -f, 0, 1, -f, 0,
        0, -1, f, 0, 1, f, 0, -1, -f, 0, 1, -f,
        f, 0, -1, f, 0, 1, -f, 0, -1, -f, 0, 1
        });
    vertices.resize((numVertices + numDuplicates) * 3);
    std::vector<uint32_t> triangles = {
        0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
        11, 10, 2, 5, 11, 4, 1, 5, 9, 7, 1, 8, 10, 7, 6,
        3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
        9, 8, 1, 4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7
    };

    uint32_t v = 12;
    std::unordered_map<uint32_t, uint32_t> midCache;

    std::vector<uint32_t> trianglesPrev = std::vector<uint32_t>(triangles);
    std::vector<uint32_t> IndexArray;

    for (uint32_t i = 0; i < order; i++) { // repeatedly subdivide each triangle into 4 triangles
        uint32_t prevLen = trianglesPrev.size();
        triangles.resize(prevLen * 4);

        for (uint32_t k = 0; k < prevLen; k += 3) {
            const uint32_t v1 = trianglesPrev[k + 0];
            const uint32_t v2 = trianglesPrev[k + 1];
            const uint32_t v3 = trianglesPrev[k + 2];
            const uint32_t a = this->_addMidPoint(v1, v2, v, midCache, vertices);
            const uint32_t b = this->_addMidPoint(v2, v3, v, midCache, vertices);
            const uint32_t c = this->_addMidPoint(v3, v1, v, midCache, vertices);
            uint32_t t = k * 4;
            triangles[t++] = v1; triangles[t++] = a; triangles[t++] = c;
            triangles[t++] = v2; triangles[t++] = b; triangles[t++] = a;
            triangles[t++] = v3; triangles[t++] = c; triangles[t++] = b;
            triangles[t++] = a;  triangles[t++] = b; triangles[t++] = c;
        }
        trianglesPrev = triangles;
    }

    // normalize vertices
    for (uint32_t i = 0; i < numVertices * 3; i += 3) {
        const double v1 = vertices[i + 0];
        const double v2 = vertices[i + 1];
        const double v3 = vertices[i + 2];
        const double m = 1 / sqrt(v1 * v1 + v2 * v2 + v3 * v3);
        vertices[i + 0] *= m;
        vertices[i + 1] *= m;
        vertices[i + 2] *= m;
    }

    if (!generateTextureCoord) {
        verticesDestination.assign(vertices.begin(),vertices.end());
        trianglesDestination.assign(triangles.begin(),triangles.end());
        return;
    }

    // uv mapping
    std::vector<double> uv = std::vector<double>((numVertices + numDuplicates) * 2);
    for (uint32_t i = 0; i < numVertices; i++) {
        uv[2 * i + 0] = atan2(vertices[3 * i + 2], vertices[3 * i]) / (2 * M_PI) + 0.5;
        uv[2 * i + 1] = asin(vertices[3 * i + 1]) / M_PI + 0.5;
    }

    std::unordered_map<uint32_t, uint32_t> duplicates;

    for (uint32_t i = 0; i < triangles.size(); i += 3) {
        const uint32_t a = triangles[i + 0];
        const uint32_t b = triangles[i + 1];
        const uint32_t c = triangles[i + 2];
        double ax = uv[2 * a];
        double bx = uv[2 * b];
        double cx = uv[2 * c];
        const double ay = uv[2 * a + 1];
        const double by = uv[2 * b + 1];
        const double cy = uv[2 * c + 1];

        // uv fixing code; don't ask me how I got here
        if (bx - ax >= 0.5 && ay != 1) bx -= 1;
        if (cx - bx > 0.5) cx -= 1;
        if (ax > 0.5 && ax - cx > 0.5 || ax == 1 && cy == 0) ax -= 1;
        if (bx > 0.5 && bx - ax > 0.5) bx -= 1;

        if (ay == 0 || ay == 1) {
            ax = (bx + cx) / 2;
            if (ay == bx) uv[2 * a] = ax;
            else triangles[i + 0] = this->_addDuplicate(a, ax, ay, false, v, duplicates,vertices, uv);

        }
        else if (by == 0 || by == 1) {
            bx = (ax + cx) / 2;
            if (by == ax) uv[2 * b] = bx;
            else triangles[i + 1] = this->_addDuplicate(b, bx, by, false, v, duplicates, vertices, uv);

        }
        else if (cy == 0 || cy == 1) {
            cx = (ax + bx) / 2;
            if (cy == ax) uv[2 * c] = cx;
            else triangles[i + 2] = this->_addDuplicate(c, cx, cy, false, v, duplicates, vertices, uv);
        }
        if (ax != uv[2 * a] && ay != 0 && ay != 1) triangles[i + 0] = this->_addDuplicate(a, ax, ay, true, v, duplicates, vertices, uv);
        if (bx != uv[2 * b] && by != 0 && by != 1) triangles[i + 1] = this->_addDuplicate(b, bx, by, true, v,duplicates, vertices, uv);
        if (cx != uv[2 * c] && cy != 0 && cy != 1) triangles[i + 2] = this->_addDuplicate(c, cx, cy, true, v, duplicates, vertices, uv);
    }

    verticesDestination.assign(vertices.begin(), vertices.end());
    trianglesDestination.assign(triangles.begin(), triangles.end());
    textureDestination.assign(uv.begin(), uv.end());
}
uint32_t IcoSphere::_addMidPoint(uint32_t a, uint32_t b, uint32_t& v, std::unordered_map<uint32_t, uint32_t>& midCache, std::vector<double>& vertices) {
    const uint32_t key = floor(((a + b) * (a + b + 1) / 2) + std::min(a, b)); // Cantor's pairing function
    auto index = midCache.find(key);
    if (index != midCache.end()){
        const uint32_t i = index->second;
        midCache.erase(index); // midpoint is only reused once, so we delete it for performance
        return i;
    }
    midCache[key] = v;
    vertices[3 * v + 0] = (vertices[3 * a + 0] + vertices[3 * b + 0]) * 0.5;
    vertices[3 * v + 1] = (vertices[3 * a + 1] + vertices[3 * b + 1]) * 0.5;
    vertices[3 * v + 2] = (vertices[3 * a + 2] + vertices[3 * b + 2]) * 0.5;
    return v++;
}
uint32_t IcoSphere::_addDuplicate(uint32_t i, double uvx, double uvy, bool cached, uint32_t& v, std::unordered_map<uint32_t, uint32_t>& duplicates, std::vector<double>& vertices, std::vector<double>& uv) {
    if (cached) {
        auto index = duplicates.find(i);
        if (index != duplicates.end()) {
            return index->second;
        }
    }
    vertices[3 * v + 0] = vertices[3 * i + 0];
    vertices[3 * v + 1] = vertices[3 * i + 1];
    vertices[3 * v + 2] = vertices[3 * i + 2];
    uv[2 * v + 0] = uvx;
    uv[2 * v + 1] = uvy;
    if (cached) duplicates[i] = v;
    return v++;
}
