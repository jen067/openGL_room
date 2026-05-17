#define _CRT_SECURE_NO_WARNINGS
#include "CObjModel.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

CObjModel::CObjModel(const std::string& filepath) {
    loadObj(filepath);
    setupVertexAttributes();
}

CObjModel::~CObjModel() {
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
    if (_points) delete[] _points;
    if (_idx)    delete[] _idx;
}

void CObjModel::draw() {
    glUseProgram(_shaderProg);
    updateMatrix();
    glBindVertexArray(_vao);
    glUniform1i(_shadingModeLoc, _uShadingMode);
    if (_bObjColor)
        glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CObjModel::drawRaw() {
    updateMatrix();
    glBindVertexArray(_vao);
    glUniform1i(_shadingModeLoc, _uShadingMode);
    if (_bObjColor)
        glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CObjModel::loadObj(const std::string& filepath) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_uvs;

    struct VertexIndex {
        unsigned int v, t, n;
    };

    std::vector<VertexIndex> vertexIndices;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            glm::vec3 v;
            iss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        }
        else if (prefix == "vt") {
            glm::vec2 vt;
            iss >> vt.x >> vt.y;
            temp_uvs.push_back(vt);
        }
        else if (prefix == "vn") {
            glm::vec3 vn;
            iss >> vn.x >> vn.y >> vn.z;
            temp_normals.push_back(vn);
        }
        else if (prefix == "f") {
            VertexIndex idx[3];
            for (int i = 0; i < 3; i++) {
                std::string vtn;
                iss >> vtn;
                sscanf(vtn.c_str(), "%d/%d/%d", &idx[i].v, &idx[i].t, &idx[i].n);
                vertexIndices.push_back(idx[i]);
            }
        }
    }

    _vtxAttrCount = 11;
    _vtxCount = vertexIndices.size();
    _idxCount = _vtxCount;

    _points = new GLfloat[_vtxCount * _vtxAttrCount];
    _idx = new GLuint[_idxCount];

    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        auto vi = vertexIndices[i];

        glm::vec3 pos = temp_vertices[vi.v - 1];
        glm::vec2 uv = temp_uvs[vi.t - 1];
        glm::vec3 norm = temp_normals[vi.n - 1];
        glm::vec3 color = glm::vec3(1.0f); // 預設白色

        _points[i * _vtxAttrCount + 0] = pos.x;
        _points[i * _vtxAttrCount + 1] = pos.y;
        _points[i * _vtxAttrCount + 2] = pos.z;
        _points[i * _vtxAttrCount + 3] = color.r;
        _points[i * _vtxAttrCount + 4] = color.g;
        _points[i * _vtxAttrCount + 5] = color.b;
        _points[i * _vtxAttrCount + 6] = norm.x;
        _points[i * _vtxAttrCount + 7] = norm.y;
        _points[i * _vtxAttrCount + 8] = norm.z;
        _points[i * _vtxAttrCount + 9] = uv.x;
        _points[i * _vtxAttrCount + 10] = uv.y;

        _idx[i] = i;
    }
}

void CObjModel::update(float dt)
{
    if (_isAnimating) {
        updateMotion(dt);
    }
}

void CObjModel::setMotionEnabled() {

    if (_isAnimating) return;
    _isAnimating = true;
    _clock = 0.0f;

}

void CObjModel::updateMotion(float dt)
{
    const float duration = 4.0f;
    _clock += dt;

    if (_clock >= duration) {
        _clock = duration;
    }

    float t = _clock / duration;
    float startAngle = _isOpen ? 90.0f : 0.0f;
    float endAngle = _isOpen ? 0.0f : 90.0f;
    float angle = glm::mix(startAngle, endAngle, t);

    setRotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));

    if (_clock >= duration) {
        _isAnimating = false;
        _isOpen = !_isOpen;
        _clock = 0.0f;
    }
}
