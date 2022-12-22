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

	float speed;
	float sensitivity;

	Sprite icon;
	Camera() {}
	void Initialize() {
		Component::Initialize();
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		transform->rotation.x = -90.0f;
		transform->rotation.y = 0.0f;
		speed = 10.0f;
		sensitivity = 0.1f;
		UpdateCameraVectors();
		icon = ModelLoader::LoadSprite("Sprite_Camera_Icon.png");
		icon.shader = "ShibaEngine_Billboard";
	}
	Camera(Transform* transform, glm::vec3 wUp = glm::vec3(0.0f, 1.0f, 0.0f), float y = -90.0f, float p = 0.0f) : speed(10.0f), sensitivity(0.1f) {
		this->transform = transform;
		worldUp = wUp;
		this->transform->rotation.x = y;
		this->transform->rotation.y = p;
		UpdateCameraVectors();
	}
	static void DrawGUI(unsigned int selectedEntity) {
		auto& camera = Engine::GetComponent<Camera>(selectedEntity);
		ImGui::InputFloat("Speed", &camera.speed);
		ImGui::ImageButton((ImTextureID)camera.icon.texture, ImVec2((float)camera.icon.width, (float)camera.icon.height));
		ImGui::InputFloat("Sensitivity", &camera.sensitivity);
	}
	void UpdateCameraVectors() {
		forward = transform->GetForward();

		right = glm::normalize(glm::cross(forward, worldUp));
		up = glm::normalize(glm::cross(right, forward));
	}
	void ProcessCameraMouse() {
		transform->rotation.x += InputManager::mouse.xOffset * sensitivity; 
		transform->rotation.y += InputManager::mouse.yOffset * sensitivity;

		if (transform->rotation.x > 89.0f)
			transform->rotation.x = 89.0f;
		if (transform->rotation.y < -89.0f)
			transform->rotation.y = -89.0f;

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
	void Deserialize(rapidjson::Value& obj) {
		speed = (float)obj["Speed"].GetDouble();
		sensitivity = (float)obj["Sensitivity"].GetDouble();
	}
};