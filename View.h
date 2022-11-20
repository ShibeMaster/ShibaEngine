#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Camera.h"
#include "Shaders.h"
#include "Time.h"
#include "InputManager.h"
class View {
public:
	glm::vec2 position;
	glm::vec2 dimensions;
	bool hasCamera;
	bool cameraInRuntimeOnly;
	Camera camera;

	View(){}
	View(glm::vec2 position, glm::vec2 dimensions, bool cameraInRuntimeOnly = true) {
		this->position = position;
		this->dimensions = dimensions;
		this->hasCamera = false;
		this->cameraInRuntimeOnly = cameraInRuntimeOnly;
	}
	View(glm::vec2 position, glm::vec2 dimensions, Camera camera, bool cameraInRuntimeOnly = true) {
		this->position = position;
		this->dimensions = dimensions;
		this->camera = camera;
		this->hasCamera = true;
		this->cameraInRuntimeOnly = cameraInRuntimeOnly;
	}
	void Update(bool inRuntime) {
		glViewport(position.x, position.y, dimensions.x, dimensions.y);

		Shaders::activeShader.SetMat4("projection", glm::perspective(glm::radians(45.0f), dimensions.x / dimensions.y, 0.1f, 100.0f));
		
		if (hasCamera) {
			Shaders::activeShader.SetMat4("view", camera.GetViewMatrix());

			if ((!cameraInRuntimeOnly || inRuntime)) {
				camera.position += camera.forward * InputManager::MoveVert() * Time::deltaTime;
				camera.position += camera.right * InputManager::MoveHorz() * Time::deltaTime;
				camera.position += camera.worldUp * InputManager::MoveUpDown() * Time::deltaTime;
				camera.UpdateCameraVectors();
			}
		}
	}
};