#pragma once
#include "Scene.h"
#include "Engine.h"
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>


class SceneLoader {
private:
	static void SerializeSceneInfo(const Scene& scene, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("name");
		json->String(scene.name.c_str());
		// Add more general scene info here
	}
	static void SerializeSceneHierachyNode(SceneItem* item, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->StartObject();
		json->Key("name");
		json->String(item->name.c_str());
		json->Key("hasParent");
		json->Bool(item->hasParent);
		Engine::SerializeEntityComponents(item->entity, json);
		if (item->children.size() > 0) {
			json->Key("Children");
			json->StartArray();
			for (auto& child : item->children) {
				SerializeSceneHierachyNode(child, json);
			}
			json->EndArray();
		}
		json->EndObject();
	}
	static void SerializeSceneHierachy(const Scene& scene, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("Hierachy");
		json->StartArray();
		for (auto& item : scene.hierachy) {
			SerializeSceneHierachyNode(item, json);
		}
		json->EndArray();
	}

public:
	static Scene LoadScene(const std::string& path) {

	}
	static void SaveScene(const Scene& scene) {
		rapidjson::Document doc;
		rapidjson::StringBuffer str;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> json = rapidjson::PrettyWriter(str);
		json.StartObject();
		SerializeSceneInfo(scene, &json);
		SerializeSceneHierachy(scene, &json);
		json.EndObject();
		std::cout << str.GetString() << std::endl;
	}
};