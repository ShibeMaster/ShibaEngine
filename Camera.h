#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Sprite.h"
#include "ModelLoader.h"

class Camera : public Component
{
public:
	glm::vec3 worldUp;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	float yaw;
	float pitch;

	float speed;
	float sensitivity;

	Sprite icon;
	Camera() {}
	void Initialize() {
		Component::Initialize();
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.0f;
		pitch = 0.0f;
		speed = 10.0f;
		sensitivity = 0.1f;
		UpdateCameraVectors();
		icon = ModelLoader::LoadSprite("Sprite_Camera_Icon.png");
	}
	Camera(Transform* transform, glm::vec3 wUp = glm::vec3(0.0f, 1.0f, 0.0f), float y = -90.0f, float p = 0.0f) : speed(10.0f), sensitivity(0.1f) {
		this->transform = transform;
		worldUp = wUp;
		yaw = y;
		pitch = p;
		UpdateCameraVectors();
	}
	static void DrawGUI(unsigned int selectedEntity) {
		auto& camera = Engine::GetComponent<Camera>(selectedEntity);
		ImGui::InputFloat("Speed", &camera.speed);
		ImGui::InputFloat("Sensitivity", &camera.sensitivity);
	}
	void UpdateCameraVectors() {
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = direction;

		right = glm::normalize(glm::cross(forward, worldUp));
		up = glm::normalize(glm::cross(right, forward));
	}
	void ProcessCameraMouse() {
		yaw += InputManager::mouse.xOffset * sensitivity;
		pitch += InputManager::mouse.yOffset * sensitivity;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		UpdateCameraVectors();
	}
	glm::mat4 GetViewMatrix() {
		return glm::lookAt(transform->position, transform->position + forward, up);
	}
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("Speed");
		json->Double(speed);
		json->Key("Sensitivity");
		json->Double(sensitivity);
	}
};