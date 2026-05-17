#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "CPlane.h"
#include <iostream>

CPlane::CPlane() : CShape()
{
	_vtxCount = 4; _vtxAttrCount = 11; _idxCount = 6;

	_points = new GLfloat[_vtxCount * _vtxAttrCount]{

		// Bottom face (柔和青, y = -0.5, 法向量 (0,-1,0))
		-0.6f, -0.6f, -0.4f,  0.6f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // 20
		 0.6f, -0.6f, -0.4f,  0.6f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.5f, 0.0f,  // 21
		 0.6f, -0.6f,  0.4f,  0.6f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.5f, 1.0f,  // 22
		-0.6f, -0.6f,  0.4f,  0.6f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,   // 23

	};

	for (int i = 0; i < _vtxCount; ++i) {
		_points[0 * _vtxAttrCount + 9] = 0.0f;  // u0
		_points[0 * _vtxAttrCount + 10] = 0.0f;  // v0

		_points[1 * _vtxAttrCount + 9] = 1.0f;  // u1
		_points[1 * _vtxAttrCount + 10] = 0.0f;  // v1

		_points[2 * _vtxAttrCount + 9] = 1.0f;  // u2
		_points[2 * _vtxAttrCount + 10] = 1.0f;  // v2

		_points[3 * _vtxAttrCount + 9] = 0.0f;
		_points[3 * _vtxAttrCount + 10] = 1.0f;
	}

	_idx = new GLuint[_idxCount]{
		0, 1, 2, 2, 3, 0,
	};

}

CPlane::~CPlane()
{
	glDeleteBuffers(1, &_vbo);  //先釋放 VBO 與 EBO
	glDeleteBuffers(1, &_ebo);
	glDeleteVertexArrays(1, &_vao); //再釋放 VAO
	if (_points != NULL) delete[] _points;
	if (_idx != NULL) delete[] _idx;
}

void CPlane::drawRaw()
{
	glUseProgram(_shaderProg);  // 必須有
	updateMatrix();

	glBindVertexArray(_vao);
	//std::cout << "Wall" << _textureID << std::endl;
	if (_textureID != 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textureID);
		glUniform1i(glGetUniformLocation(_shaderProg, "uTexture"), 0);  // 將 sampler2D 接到 GL_TEXTURE0
	}

	glUniform1i(_shadingModeLoc, _uShadingMode);
	if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));

	glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CPlane::reset() {
	CShape::reset();
}