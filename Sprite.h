#pragma once
#include "ShaderManager.h"
#include "Mesh.h"
class Sprite {
public:
	int width;
	int height;
	int channels;
	unsigned int texture;
	Mesh mesh;
	std::string shader;
	void Render() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		mesh.Render();
	}
};