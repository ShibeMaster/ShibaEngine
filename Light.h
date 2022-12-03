#pragma once
#include "Component.h"
#include "Sprite.h"
#include "ModelLoader.h"
class Light : public Component {
public:
	Sprite icon;
	glm::vec3 colour = glm::vec3(1.0f);
	float ambientStrength = 0.1f;
	Light() {
		icon = ModelLoader::LoadSprite("Sprite_Light_Icon.png");
	}
	static void DrawGUI(unsigned int selectedEntity) {

		if (Engine::HasComponent<Light>(selectedEntity)) {
			bool lightExists = true;
			if (ImGui::CollapsingHeader("Light", &lightExists)) {
				auto& light = Engine::GetComponent<Light>(selectedEntity);
				ImGui::ColorEdit3("Colour", &light.colour[0]);
			}

			if (!lightExists)
				Engine::RemoveComponent<Light>(selectedEntity);
		}
	}
};