#pragma once
#include "Scene.h"
#include "Engine.h"
#include <fstream>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <cstdio>
#include "SerializationUtils.h"
#include <rapidjson/filereadstream.h>

class SceneLoader {
private:
	static void SerializeSceneInfo(const Scene& scene, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key("Name");
		json->String(scene.name.c_str());
		// Add more general scene info here
	}
	static void DeserializeSceneHierachy(Scene* scene, rapidjson::GenericArray<false, rapidjson::Value> document) {
		for (auto& item : document) {
			SceneItem* sceneItem = DeserializeSceneHierachyNode(scene, item);
			scene->hierachy.push_back(sceneItem);
		}
	}
	static SceneItem* DeserializeSceneHierachyNode(Scene* scene, rapidjson::Value& obj, SceneItem* parent = nullptr) {
		SceneItem item;
		item.name = obj["name"].GetString();
		item.hasParent = obj["hasParent"].GetBool();
		if (item.hasParent)
			item.parent = parent;
		item.entity = Engine::CreateEntity();
		scene->entities.push_back(item.entity);
		scene->items[item.entity] = item;
		Engine::DeserializeEntityComponents(item.entity, obj);
		if (obj.HasMember("Children")) {
			for (auto& child : obj["Children"].GetArray()) {
				scene->items[item.entity].children.push_back(DeserializeSceneHierachyNode(scene, child, &scene->items[item.entity]));
			}
		}
		return &scene->items[item.entity];
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
	static void LoadScene(const std::string& path) {
		rapidjson::Document doc;
		SceneManager::AddScene(path);
		SceneManager::ChangeScene(path);
		doc.Parse(SerializationUtils::ReadFile(path).c_str());
		SceneManager::activeScene->name = doc["Name"].GetString();
		SceneManager::activeScene->path = path;
		DeserializeSceneHierachy(SceneManager::activeScene, doc["Hierachy"].GetArray());
		SceneManager::activeScene->LoadSkybox();
	}
	static void SaveScene(const Scene& scene, const std::string& path) {
		rapidjson::StringBuffer str;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> json = rapidjson::PrettyWriter(str);
		json.StartObject();
		SerializeSceneInfo(scene, &json);
		SerializeSceneHierachy(scene, &json);
		json.EndObject();
		auto file = std::ofstream(path, std::ofstream::out | std::ofstream::trunc);
		file << str.GetString();
		file.close();
	}
};