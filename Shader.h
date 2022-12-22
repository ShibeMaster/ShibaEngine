#pragma once
#include <GL/glew.h>
#include "ShaderInstanceData.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
class Shader {
public:
	GLuint id;
	std::vector<Uniform> uniforms;


	Shader(){}
	Shader(const char* vertexSource, const char* fragmentSource) {

		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSource, NULL);
		glCompileShader(vertex);
		int vertexStatus;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &vertexStatus);
		std::cout << "vertex status: " << vertexStatus << std::endl;

		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);
		int fragmentStatus;
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &fragmentStatus);
		std::cout << "fragment status: " << fragmentStatus << std::endl;

		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		LoadUniforms();
	}
	void LoadUniforms() {
		int count;
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
		for (int i = 0; i < count; i++) {
			GLsizei length;
			GLchar name[16];
			Uniform uniform;
			glGetActiveUniform(id, (GLuint)i, (GLsizei)16, &length, &uniform.size, &uniform.type, name);
			uniform.name = std::string(name);
			// just going to manually check whether the name is equal to any reserved uniforms as there isn't all too many
			if (uniform.name != "lightColour" && uniform.name != "view" && uniform.name != "projection" && uniform.name != "model" && uniform.name != "viewPos" && uniform.name != "lightPos" && uniform.name != "hasTexture")
				uniforms.push_back(uniform);
		}
	}
	void Use() {
		glUseProgram(id);
	}
	void SetBool(const std::string& name, const bool& value) {
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}
	void SetFloat(const std::string& name, const float& value) {
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}
	void SetInt(const std::string& name, const int& value) {
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}
	void SetVec2(const std::string& name, const glm::vec2& value) {
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetVec3(const std::string& name, const glm::vec3& value) {
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetVec4(const std::string& name, const glm::vec4& value) {
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetMat4(std::string name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}
};