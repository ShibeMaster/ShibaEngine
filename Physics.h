#pragma once
#include "Component.h"
#include "Time.h"
#include "Raycast.h"
#include <glm/glm.hpp>
class Physics : public Component {
public:
	glm::vec3 velocity = glm::vec3(0.0f);
	bool useGravity = true;
	glm::vec3 gravityDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	float gravity = 9.8f;
	bool useDrag = true;
	float drag = 9.0f;
	bool isGrounded;

	static void DrawGUI(unsigned int selectedEntity) {
		if (Engine::HasComponent<Physics>(selectedEntity)) {
			bool physicsExists = true;
			if (ImGui::CollapsingHeader("Physics", &physicsExists)) {
				Physics& physics = Engine::GetComponent<Physics>(selectedEntity);
				ImGui::Checkbox("Apply Drag", &physics.useDrag);
				if (physics.useDrag)
					ImGui::DragFloat("Drag", &physics.drag);
				ImGui::Checkbox("Apply Gravity", &physics.useGravity);
				if (physics.useGravity) {
					ImGui::InputFloat3("Gravity Direction", &physics.gravityDirection[0]);
					ImGui::DragFloat("Gravity", &physics.gravity);
				}
				ImGui::DragFloat3("Velocity", &physics.velocity[0], 0.1f);
			}

			if (!physicsExists)
				Engine::RemoveComponent<Physics>(selectedEntity);
		}

	}
	void ApplyDrag() {

		glm::vec3 vector = glm::vec3(velocity.x, 0.0f, velocity.z);
		float multiplier = 1.0f - drag * Time::deltaTime;
		if (multiplier < 0.0f) multiplier = 0.0f;
		vector *= multiplier;
		vector.y = velocity.y;
		velocity = vector;
	}
	void ApplyGravity() {
		velocity += gravityDirection * gravity * Time::deltaTime;
	}
	void Update(bool inRuntime) {
		if (inRuntime) {
			if (useDrag)
				ApplyDrag();
			if (useGravity && !isGrounded)
				ApplyGravity();

			if (velocity.y < -20.0f)
				velocity.y = -20.0f;

			auto& transform = Engine::GetComponent<Transform>(entity);
			auto& boundingBox = Engine::GetComponent<MeshCollisionBox>(entity);
			std::vector<unsigned int> outHits;
			isGrounded = Raycast(transform.position, glm::vec3(0.0f, -1.0f, 0.0f), abs(boundingBox.min.y) + 0.3f, &outHits, entity);
			if (isGrounded && velocity.y < 0.0f) {
				velocity.y = 0.0f;
				std::cout << "grounded" << std::endl;
			}
			transform.position += velocity * Time::deltaTime;


		}
	}
	void GetObject(ClassInstance* instance) {
		instance->SetFieldValue<glm::vec3>("velocity", velocity);
		instance->SetFieldValue<bool>("useGravity", useGravity);
		instance->SetFieldValue<float>("gravity", gravity);
		instance->SetFieldValue<glm::vec3>("gravityDirection", gravityDirection);
		instance->SetFieldValue<bool>("useDrag", useDrag);
		instance->SetFieldValue<float>("drag", drag);
	}
	void SetObject(ClassInstance* instance) {
		velocity = instance->GetFieldValue<glm::vec3>("velocity");
		useGravity = instance->GetFieldValue<bool>("useGravity");
		gravity = instance->GetFieldValue<float>("gravity");
		gravityDirection = instance->GetFieldValue<glm::vec3>("gravityDirection");
		useDrag = instance->GetFieldValue<bool>("useDrag");
		drag = instance->GetFieldValue<float>("drag");
	}

};