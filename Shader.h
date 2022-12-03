#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
class Shader {
public:
	GLuint id;
	
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
	void SetVec3(std::string name, const glm::vec3& value) {
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetVec4(std::string name, const glm::vec4& value) {
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetMat4(std::string name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}
};