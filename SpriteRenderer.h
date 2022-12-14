#pragma once
#include "Component.h"
#include "ProjectItem.h"
#include "Engine.h"
#include "GUIExtensions.h"
#include "ModelLoader.h"
#include "Sprite.h"
#include "Renderer.h"
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
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("Sprite");
		json->String(spriteItem.path.c_str());
	}
	void Deserialize(rapidjson::Value& obj) {
		spriteItem = ProjectManager::activeProject.GetItem(obj["Sprite"].GetString());
		ReloadSprite();
	}
	void GetObject(ClassInstance* instance) {
		instance->SetFieldValue<MonoString>("spritePath", mono_string_new(Scripting::data.appDomain, spriteItem.path.c_str()));
	}
	void SetObject(ClassInstance* instance) {
		spriteItem = ProjectManager::activeProject.GetItem(mono_string_to_utf8(instance->GetFieldValue<MonoString*>("spritePath")));
		ReloadSprite();
	}
	void RenderUI() {
		if (hasSprite) {
			glm::mat4 model = transform->GetMatrix();
			Renderer::SetModel(model);
			sprite.Render();
		}
	}
};