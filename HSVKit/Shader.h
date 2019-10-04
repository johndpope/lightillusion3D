// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
//#include <GL/glew.h>
#include <string>
#include "Math.h"
//#include"common.h"
#include <GLFW/gg.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
class Shader
{
public:
	Shader();
	~Shader();
	// Load the vertex/fragment shaders with the given names
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// Set this as the active shader program
	void SetActive();
	// Sets a Matrix uniform
	void SetMatrixUniform(const char* name, const glm::mat4 matrix);
	// Sets a Vector3 uniform
	void SetVectorUniform(const char* name, const glm::vec3& vector);
	// Sets a float uniform
	void SetFloatUniform(const char* name, float value);

	void SetTextureUniform(const char* name, GLint value);

	void printProgram();
private:
	// Tries to compile the specified shader
	bool CompileShader(const std::string& fileName,
		GLenum shaderType,
		GLuint& outShader);

	// Tests whether shader compiled successfully
	bool IsCompiled(GLuint shader);
	// Tests whether vertex/fragment programs link
	bool IsValidProgram();
private:
	// Store the shader object IDs
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
};
