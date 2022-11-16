#pragma once
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
class Mouse
{
private:
	bool firstMouseMove = true;
	float lastMouseX = 0.0f;
	float lastMouseY = 0.0f;
public:
	float xOffset = 0.0f;
	float yOffset = 0.0f;
	glm::vec2 position;
	bool inputLocked = false;

	void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouseMove) {
			lastMouseX = xpos;
			lastMouseY = ypos;
			firstMouseMove = false;
		}
		if (!inputLocked) {
			xOffset = 0.0f;
			yOffset = 0.0f;
		}
		else {

			xOffset = xpos - lastMouseX;
			yOffset = lastMouseY - ypos;
		}

		position.x = (float)xpos;
		position.y = (float)ypos;
		lastMouseX = xpos;
		lastMouseY = ypos;
	}
};