#pragma once
#include "Component.h"
#include "Engine.h"
#include "SpriteRenderer.h"
#include "SceneManager.h"
#include "Renderer.h"
class ScreenLayer : public Component {
public:
	bool fullscreen = true;
	glm::vec2 dimensions = glm::vec2(1000.0f);
	void Start() {

	}
	static void DrawGUI(unsigned int selectedEntity) {
		auto& layer = Engine::GetComponent<ScreenLayer>(selectedEntity);
		ImGui::Checkbox("Fullscreen", &layer.fullscreen);
		ImGui::InputFloat2("Dimensions", &layer.dimensions[0]);
	}
	void RenderUIElement(SceneItem* item) {
		Engine::GetComponent<SpriteRenderer>(item->entity).RenderUI();


		for (auto& child : item->children) {
			if (Engine::HasComponent<SpriteRenderer>(child->entity))
				RenderUIElement(child);
		}
	}
	void GetObject(ClassInstance* instance) {
		instance->SetFieldValue<bool>("fullscreen", fullscreen);
		instance->SetFieldValue<glm::vec2>("dimensions", dimensions);
	}
	void SetObject(ClassInstance* instance) {
		fullscreen = instance->GetFieldValue<bool>("fullscreen");
		dimensions = instance->GetFieldValue<glm::vec2>("dimensions");
	}
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("fullscreen");
		json->Bool(fullscreen);
		json->Key("dimension");
		SerializationUtils::SerializeVec2(dimensions, json);
	}
	void Deserialize(rapidjson::Value& obj) {
		fullscreen = obj["fullscreen"].GetBool();
		dimensions = SerializationUtils::DeserializeVec2(obj["dimensions"]);
	}
	void Render() {
		glDisable(GL_DEPTH_TEST);
		Renderer::ChangeShader("ShibaEngine_Sprite");
		ShaderManager::shader->SetMat4("projection", glm::ortho(0.0f, dimensions.x, -dimensions.y, 0.0f, -1.0f, 1.0f));
		ShaderManager::shader->SetVec2("dimensions", dimensions);
		for (auto& item : SceneManager::activeScene->items[entity].children) {
			if (Engine::HasComponent<SpriteRenderer>(item->entity))
				RenderUIElement(item);
		}
		glEnable(GL_DEPTH_TEST);
	}
};