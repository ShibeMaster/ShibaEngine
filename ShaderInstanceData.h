#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <map>

struct Uniform {
	std::string name;
	GLenum type;
	GLint size;
};
/// <summary>
/// although this looks attrocious, this will be the way that we store the data for shader uniforms in the mesh / sprite renderer
/// </summary>
struct ShaderUniformData {
	std::map<std::string, glm::vec4> vec4s;
	std::map<std::string, glm::vec3> vec3s;
	std::map<std::string, glm::vec2> vec2s;
	std::map<std::string, float> floats;
	std::map<std::string, bool> bools;
	std::map<std::string, int> ints;
};