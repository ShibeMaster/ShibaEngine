#pragma once
#include <GL/glew.h>
#include <assimp/mesh.h>
#include <string>
#include "ShaderManager.h"
#include <iostream>
#include <glm/glm.hpp>
#include "glm/gtc/quaternion.hpp"
#include <vector>
struct Vertex {
	glm::vec3 position;
	glm::vec2 textureCoords;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
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
	std::vector<Texture> textures;
	aiMesh* mesh;
	bool hasTextures = false;
	//std::vector<Texture> textures;

	Mesh(){}
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, std::vector<Texture> textures = {}) {
		this->vertices = vertices;
		this->indices = indicies;
		this->textures = textures;

		hasTextures = textures.size() != 0;


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


		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
	void Render() {
		if (textures.size() != 0) {
			unsigned int diffuse = 1;
			unsigned int specular = 1;
			unsigned int normals = 1;
			unsigned int height = 1;
			for (unsigned int i = 0; i < textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				std::string number;
				std::string name = textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuse++);
				else if (name == "texture_specular")
					number = std::to_string(specular++);
				else if (name == "texture_normal")
					number = std::to_string(normals++);
				else if (name == "texture_height")
					number = std::to_string(height++);

				ShaderManager::shader->SetInt((name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}
		}
		ShaderManager::shader->SetBool("hasTexture", textures.size() != 0);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
};