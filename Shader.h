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
		glGetProgramiv(vertex, GL_COMPILE_STATUS, &vertexStatus);
		std::cout << "vertex status: " << vertexStatus << std::endl;

		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSource, NULL);
		glCompileShader(fragment);
		int fragmentStatus;
		glGetProgramiv(fragment, GL_COMPILE_STATUS, &fragmentStatus);
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

	void SetVec3(std::string name, glm::vec3 value) {
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetVec4(std::string name, glm::vec4 value) {
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void SetMat4(std::string name, glm::mat4 value) {
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}
};