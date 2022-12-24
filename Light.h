#pragma once
#include "Component.h"
#include "Sprite.h"
#include "ModelLoader.h"
class Light : public Component {
public:
	Sprite icon;
	glm::vec3 colour = glm::vec3(1.0f);
	Light() {
		icon = ModelLoader::LoadSprite("Icons\\Sprite_Light_Icon.png");
		icon.shader = "ShibaEngine_Billboard";
	}
	static void DrawGUI(unsigned int selectedEntity) {
		auto& light = Engine::GetComponent<Light>(selectedEntity);
		ImGui::ColorEdit3("Colour", &light.colour[0]);
	}
	void GetObject(ClassInstance* instance) {
		instance->SetFieldValue<glm::vec3>("colour", colour);
	}
	void SetObject(ClassInstance* instance) {
		colour = instance->GetFieldValue<glm::vec3>("colour");
	}
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		SerializationUtils::SerializeVec3("colour", colour, json);
	}
	void Deserialize(rapidjson::Value& obj) {
		colour = SerializationUtils::DeserializeVec3(obj["colour"]);
	}
};