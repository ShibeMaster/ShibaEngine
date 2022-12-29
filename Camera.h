#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Component.h"
#include "Sprite.h"
#include "ModelLoader.h"
#include "InputManager.h"

class Camera : public Component
{
public:
	glm::vec3 worldUp;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	float fov;

	Sprite icon;
	Camera() {}
	void Initialize() {
		Component::Initialize();
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		transform->rotation.x = -90.0f;
		transform->rotation.y = 0.0f;
		UpdateCameraVectors();
		icon = ModelLoader::LoadSprite("Icons\\Sprite_Camera_Icon.png");
		icon.shader = "ShibaEngine_Billboard";
	}
	Camera(Transform* transform, glm::vec3 wUp = glm::vec3(0.0f, 1.0f, 0.0f), float y = -90.0f, float p = 0.0f) : fov(45.0f) {
		this->transform = transform;
		worldUp = wUp;
		this->transform->rotation.x = y;
		this->transform->rotation.y = p;
		UpdateCameraVectors();
	}
	static void DrawGUI(unsigned int selectedEntity) {
		auto& camera = Engine::GetComponent<Camera>(selectedEntity);
		ImGui::InputFloat("Field of View", &camera.fov);
	}
	void UpdateCameraVectors() {
		forward = transform->GetForward();

		right = glm::normalize(glm::cross(forward, worldUp));
		up = glm::normalize(glm::cross(right, forward));
	}
	void ProcessCameraMouse() {
		transform->rotation.x += InputManager::mouse.xOffset * 0.1f; 
		transform->rotation.y += InputManager::mouse.yOffset * 0.1f;

		if (transform->rotation.y > 89.0f)
			transform->rotation.y = 89.0f;
		if (transform->rotation.y < -89.0f)
			transform->rotation.y = -89.0f;

		UpdateCameraVectors();
	}
	glm::mat4 GetViewMatrix() {
		return glm::lookAt(transform->position, transform->position + forward, up);
	}
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("FOV");
		json->Double((double)fov);
	}
	void Deserialize(rapidjson::Value& obj) {
		fov = (float)obj["FOV"].GetDouble();
	}
};