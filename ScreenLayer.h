#pragma once
#include "Component.h"
#include "Engine.h"
#include "SpriteRenderer.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "Button.h"
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
		if(Engine::HasComponent<SpriteRenderer>(item->entity))
			Engine::GetComponent<SpriteRenderer>(item->entity).RenderUI();

		if (Engine::HasComponent<Button>(item->entity)) {
			Engine::GetComponent<Button>(item->entity).RenderUI();
		}

		for (auto& child : item->children) {
			if (Engine::HasComponent<SpriteRenderer>(child->entity) || Engine::HasComponent<Button>(child->entity))
				RenderUIElement(child);
		}
	}
	void Render() {
		glDisable(GL_DEPTH_TEST);
		Renderer::ChangeShader("ShibaEngine_Sprite");
		ShaderManager::shader->SetMat4("projection", glm::ortho(0.0f, dimensions.x, -dimensions.y, 0.0f, -1.0f, 1.0f));
		ShaderManager::shader->SetVec2("dimensions", dimensions);
		for (auto& item : SceneManager::activeScene->items[entity].children) {
			if (Engine::HasComponent<SpriteRenderer>(item->entity) || Engine::HasComponent<Button>(item->entity))
				RenderUIElement(item);
		}
		glEnable(GL_DEPTH_TEST);
	}
};