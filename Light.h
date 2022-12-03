#pragma once
#include "Component.h"
class Light : public Component {
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 colour = glm::vec3(1.0f);
	float ambientStrength = 0.1f;

	static void DrawGUI(unsigned int selectedEntity) {

		if (Engine::HasComponent<Light>(selectedEntity)) {
			bool lightExists = true;
			if (ImGui::CollapsingHeader("Light", &lightExists)) {
				auto& light = Engine::GetComponent<Light>(selectedEntity);
				ImGui::InputFloat("Ambient Strength", &light.ambientStrength);
				ImGui::InputFloat3("Position", &light.position[0]);
				ImGui::ColorEdit3("Colour", &light.colour[0]);
			}

			if (!lightExists)
				Engine::RemoveComponent<Light>(selectedEntity);
		}
	}
};