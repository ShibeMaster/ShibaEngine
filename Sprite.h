#pragma once
#include "Shaders.h"
class Sprite {
public:
	int width;
	int height;
	int channels;
	unsigned int texture1;
	Mesh mesh;
	void Render() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		mesh.Render();
	}
};