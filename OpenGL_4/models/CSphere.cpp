#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "CSphere.h"
#include "../common/typedefs.h"

CSphere::CSphere(float radius, GLuint sectors, GLuint stacks , GLuint pattern) : CShape()
{
	generateSphere(radius, sectors, stacks, pattern);
}

void CSphere::setPattern(GLuint pattern) {
    glm::vec4 topColor, bottomColor;
    switch (pattern) {
    case 1:
    default:
        topColor = glm::vec4(0.00f, 0.50f, 0.70f, 1.0f);
        bottomColor = glm::vec4(0.30f, 0.80f, 0.90f, 1.0f);
        break;
    case 2:
        topColor = glm::vec4(1.00f, 0.50f, 0.20f, 1.0f);
        bottomColor = glm::vec4(1.00f, 0.80f, 0.40f, 1.0f);
        break;
    case 3:
        topColor = glm::vec4(0.50f, 0.20f, 0.70f, 1.0f);
        bottomColor = glm::vec4(0.90f, 0.40f, 0.90f, 1.0f);
        break;
    case 4: // 綠色
        topColor = glm::vec4(0.10f, 0.60f, 0.10f, 1.0f);       // 深綠
        bottomColor = glm::vec4(0.50f, 0.90f, 0.50f, 1.0f);    // 淺綠
        break;
    case 5:
        topColor = glm::vec4(0.00f, 0.50f, 0.70f, 1.0f);
        bottomColor = glm::vec4(1.00f, 0.80f, 0.40f, 1.0f);
        break;
    case 6: // 紅色
        topColor = glm::vec4(0.80f, 0.15f, 0.10f, 1.0f);       // 暗紅
        bottomColor = glm::vec4(1.00f, 0.50f, 0.40f, 1.0f);    // 淺紅
        break;
    }
}

CSphere::~CSphere()
{
	glDeleteBuffers(1, &_vbo);  //先釋放 VBO 與 EBO
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao); //再釋放 VAO
	if (_points != NULL) delete[] _points;
	if (_idx != NULL) delete[] _idx;
}

void CSphere::draw()
{
	glUseProgram(_shaderProg);
	updateMatrix();
	glBindVertexArray(_vao);
	glUniform1i(_shadingModeLoc, _uShadingMode);
	if ( _bObjColor ) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CSphere::drawRaw()
{
	updateMatrix();
	glBindVertexArray(_vao);
	glUniform1i(_shadingModeLoc, _uShadingMode);
	if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CSphere::update(float dt)
{

}

void CSphere::reset() {
	CShape::reset();
}


void CSphere::generateSphere(float radius, GLuint sectors, GLuint stacks, GLuint pattern) {
    _vtxCount = (GLuint)(stacks + 1) * (sectors + 1);
    _idxCount = (GLuint)stacks * sectors * 6;
    _vtxAttrCount = 11;

    _points = new GLfloat[_vtxCount * _vtxAttrCount];
    _idx = new GLuint[_idxCount];

    glm::vec4 topColor, bottomColor;
    switch (pattern) {
    case 1: // 橘
    default:
        topColor = glm::vec4(0.00f, 0.50f, 0.70f, 1.0f);
        bottomColor = glm::vec4(0.30f, 0.80f, 0.90f, 1.0f);
        break;
    case 2:
        topColor = glm::vec4(1.00f, 0.50f, 0.20f, 1.0f);
        bottomColor = glm::vec4(1.00f, 0.80f, 0.40f, 1.0f);
        break;
    case 3:
        topColor = glm::vec4(0.50f, 0.20f, 0.70f, 1.0f);
        bottomColor = glm::vec4(0.90f, 0.40f, 0.90f, 1.0f);
        break;
    case 4: // 綠色
        topColor = glm::vec4(0.10f, 0.60f, 0.10f, 1.0f);       // 深綠
        bottomColor = glm::vec4(0.50f, 0.90f, 0.50f, 1.0f);    // 淺綠
        break;
    case 5:
        topColor = glm::vec4(0.00f, 0.50f, 0.70f, 1.0f);
        bottomColor = glm::vec4(1.00f, 0.80f, 0.40f, 1.0f);
        break;
    case 6: // 紅色
        topColor = glm::vec4(0.80f, 0.15f, 0.10f, 1.0f);       // 暗紅
        bottomColor = glm::vec4(1.00f, 0.50f, 0.40f, 1.0f);    // 淺紅
        break;
    }

    float sectorStep = 2.0f * M_PI / sectors;
    float stackStep = M_PI / stacks;

    GLuint vi = 0;
    for (unsigned int i = 0; i <= stacks; ++i) {
        float v = float(i) / stacks;
        float stackAngle = M_PI / 2.0f - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float y = radius * sinf(stackAngle);  // Y 軸為垂直軸

        glm::vec4 color = glm::mix(topColor, bottomColor, v);

        for (unsigned int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            float x = xy * cosf(sectorAngle);
            float z = -xy * sinf(sectorAngle);  // OpenGL RHS，逆時針方向

            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 texCoords = glm::vec2((float)j / sectors, v);

            _points[vi++] = pos.x;
            _points[vi++] = pos.y;
            _points[vi++] = pos.z;
            _points[vi++] = color.r;
            _points[vi++] = color.g;
            _points[vi++] = color.b;
            _points[vi++] = normal.x;
            _points[vi++] = normal.y;
            _points[vi++] = normal.z;
            _points[vi++] = texCoords.s;
            _points[vi++] = texCoords.t;
        }
    }

    GLuint ii = 0;
    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < sectors; ++j) {
            unsigned int first = i * (sectors + 1) + j;
            unsigned int second = first + sectors + 1;

            _idx[ii++] = first;
            _idx[ii++] = second;
            _idx[ii++] = first + 1;

            _idx[ii++] = first + 1;
            _idx[ii++] = second;
            _idx[ii++] = second + 1;
        }
    }
}

void CSphere::upload() {

}