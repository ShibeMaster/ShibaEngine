#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};
class Mesh {
public:
	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//std::vector<Texture> textures;

	Mesh(){}
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies/*, std::vector<Texture> textures*/) {
		this->vertices = vertices;
		this->indices = indicies;
		// this->textures = textures;


		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);


		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);


		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
	void Render() {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};