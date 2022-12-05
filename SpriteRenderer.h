#pragma once
#include "Component.h"
#include "ProjectItem.h"
#include "Engine.h"
#include "GUIExtensions.h"
#include "ModelLoader.h"
#include "Sprite.h"
#include <imgui.h>
class SpriteRenderer : public Component {
public:
	ProjectItem spriteItem = {"Sprite Item"};
	bool hasSprite = false;
	Sprite sprite;
	void ReloadSprite() {
		sprite = ModelLoader::LoadSprite(spriteItem.path);
		hasSprite = sprite.height != 0;
	}
	static void DrawGUI(unsigned int selectedEntity) {
		SpriteRenderer& renderer = Engine::GetComponent<SpriteRenderer>(selectedEntity);
		ImGui::Button(renderer.spriteItem.name.c_str());
		ProjectItem item;
		if (GUIExtensions::CreateProjectItemDropField({ ".png" }, &item)) {
			renderer.spriteItem = item;
			std::cout << renderer.spriteItem.name << " || " << renderer.spriteItem.path << std::endl;
			renderer.ReloadSprite();
		}
	}
	void Update(bool inRuntime) {
		if (hasSprite) {
			glm::mat4 model = transform->GetMatrix();
			Shaders::activeShader.SetMat4("model", model);
			sprite.Render();
		}
	}
};