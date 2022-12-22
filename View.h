#pragma once
#include <glm/glm.hpp>
#include "Camera.h"
#include "FrameBuffer.h"
class View {
public:
	glm::vec2 position;
	glm::vec2 dimensions;
	bool hasCamera;
	FrameBuffer framebuffer;
	Camera* camera;

	View(){}
	View(glm::vec2 position, glm::vec2 dimensions) {
		this->position = position;
		this->dimensions = dimensions;
		this->hasCamera = false;
	}
	View(glm::vec2 position, glm::vec2 dimensions, Camera* camera) {
		this->position = position;
		this->dimensions = dimensions;
		this->camera = camera;
		this->hasCamera = true;
	}
};