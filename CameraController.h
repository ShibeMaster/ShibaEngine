#pragma once
#include "Camera.h"
#include "GUIExtensions.h"
#include "SceneManager.h"
class CameraController : public Component {
public:
	bool hasCamera;
	Camera* camera;
	
	float sensitivity;
	void ProcessMouse() {
		if (hasCamera) {

			camera->transform->rotation.x += InputManager::mouse.xOffset * sensitivity;
			camera->transform->rotation.y += InputManager::mouse.yOffset * sensitivity;

			if (camera->transform->rotation.y > 89.0f)
				camera->transform->rotation.y = 89.0f;
			if (camera->transform->rotation.y < -89.0f)
				camera->transform->rotation.y = -89.0f;

			if (camera->transform->rotation.x > 360.0f)
				camera->transform->rotation.x = camera->transform->rotation.x - 360.0f;
			if (camera->transform->rotation.x < 0.0f)
				camera->transform->rotation.x = camera->transform->rotation.x + 360.0f;

			camera->UpdateCameraVectors();
		}
	}
	static void DrawGUI(unsigned int selectedEntity) {
		auto& camera = Engine::GetComponent<CameraController>(selectedEntity);
		unsigned int entity;
		ImGui::Button(camera.hasCamera ? SceneManager::activeScene->items[camera.camera->entity].name.c_str() : "Undefined Camera");
		if (GUIExtensions::CreateDragDropTarget<unsigned int>("Entity", &entity)) {
			if (Engine::HasComponent<Camera>(entity)) {
				camera.camera = Engine::GetComponentPointer<Camera>(entity);
				camera.hasCamera = true;
			}
		}
		if (camera.hasCamera) {
			ImGui::InputFloat("Sensitivity", &camera.sensitivity);
		}
	}
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("Sensitivity");
		json->Double((double)sensitivity);
	}
	void Deserialize(rapidjson::Value& obj) {
		sensitivity = (float)obj["Sensitivity"].GetDouble();
	}
	void GetObject(ClassInstance* instance) {
		instance->SetFieldValue<float>("sensitivity", sensitivity);
		instance->SetFieldValue<bool>("hasCamera", hasCamera);
		if(hasCamera)
			instance->SetFieldValue<MonoObject>("cameraInstance", Scripting::data.entities[camera->entity].instance.instance);
	}
	void SetObject(ClassInstance* instance) {
		sensitivity = instance->GetFieldValue<float>("sensitivity");
	}

};