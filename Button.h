#pragma once
#include "Sprite.h"
#include "Renderer.h"
#include "Component.h"
#include "Project.h"
#include "ModelLoader.h"
#include "ViewManager.h"
#include "UIManager.h"
#include "GUIExtensions.h"
#include "InputManager.h"
#include "Collisions.h"
class Button : public Component {
public:
	ProjectItem spriteItem = { "Sprite Item" };
	bool hasSprite = false;
	Sprite sprite;
	glm::vec2 screenLayerDimensions;
	Box2d box;
	static void DrawGUI(unsigned int selectedEntity) {
		Button& button = Engine::GetComponent<Button>(selectedEntity);

		ImGui::Button(button.spriteItem.name.c_str());
		ProjectItem item;
		if (GUIExtensions::CreateProjectItemDropField({ ".png" }, &item)) {
			button.spriteItem = item;
			button.ReloadSprite();
		}
	}
	void ReloadSprite() {
		sprite = ModelLoader::LoadSprite(spriteItem.path);
		hasSprite = true;
	}
	void GenerateBox(glm::mat4 projection, glm::vec2 screenDimensions) {
		if (hasSprite) {
			glm::vec2 displayDim = glm::vec2(Display::width, Display::height);
			glm::vec2 max = sprite.mesh.vertices[0].position;
			glm::vec2 posMax = glm::vec2(transform->GetMatrix() * glm::vec4(max.x, max.y, 0.0f, 1.0));
			posMax.x += screenDimensions.x / 2;
			posMax.y += -screenDimensions.y / 2;
			posMax = glm::vec2(projection * glm::vec4(posMax.x, posMax.y, 0.0f, 1.0f));
			posMax *= displayDim * (UIManager::viewportFrame.sceneViewportDimensions / displayDim);
			posMax += UIManager::viewportFrame.sceneViewportPosition;


			glm::vec2 min = sprite.mesh.vertices[2].position;
			glm::vec2 posMin = glm::vec2(transform->GetMatrix() * glm::vec4(min.x, min.y, 0.0f, 1.0));
			posMin.x += screenDimensions.x / 2;
			posMin.y += -screenDimensions.y / 2;
			posMin = glm::vec2(projection * glm::vec4(posMin.x, posMin.y, 0.0f, 1.0f));
			posMin *= displayDim * (UIManager::viewportFrame.sceneViewportDimensions / displayDim);
			posMin += UIManager::viewportFrame.sceneViewportPosition;

			this->box = { posMin, posMax };

		}
	}
	void OnMouseClick() {
		if (Collisions::Collides(box, { InputManager::mouse.position - UIManager::viewportFrame.sceneViewportDimensions/glm::vec2(2.0f), InputManager::mouse.position - UIManager::viewportFrame.sceneViewportDimensions / glm::vec2(2.0f) })) {
			std::cout << "pressed" << std::endl;
		}
		else {
			std::cout << "not pressed" << std::endl;
		}
	}
	void RenderUI() {
		if (hasSprite) {
			glm::mat4 model = transform->GetMatrix();
			Renderer::SetModel(model);
			sprite.Render();
		}
	}
};