#pragma once
#include "Component.h"
#include "EngineTime.h"
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
	void Update() {
		transform->position += velocity * Time::deltaTime;

		if (useDrag)
			ApplyDrag();
		if (useGravity && !isGrounded)
			ApplyGravity();

		if (velocity.y < -20.0f)
			velocity.y = -20.0f;
	}
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("Use Gravity");
		json->Bool(useGravity);
		SerializationUtils::SerializeVec3("Gravity Direction", gravityDirection, json);
		json->Key("Gravity");
		json->Double(gravity);
		json->Key("Use Drag");
		json->Bool(useDrag);
		json->Key("Drag");
		json->Double(drag);
	}
	void Deserialize(rapidjson::Value& obj) {
		useGravity = obj["Use Gravity"].GetBool();
		gravityDirection = SerializationUtils::DeserializeVec3(obj["Gravity Direction"]);
		gravity = (float)obj["Gravity"].GetDouble();
		useDrag = obj["Use Drag"].GetBool();
		drag = (float)obj["Drag"].GetDouble();
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