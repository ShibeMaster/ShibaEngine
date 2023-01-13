#pragma once
#include "Scene.h"
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

class SceneLoader {
private:
	static void SerializeSceneInfo(const Scene& scene, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static void DeserializeSceneHierachy(Scene* scene, rapidjson::GenericArray<false, rapidjson::Value> document);
	static void SerializeSceneHierachy(const Scene& scene, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
public:
	static void SerializeSceneHierachyNode(SceneItem* item, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static SceneItem* DeserializeSceneHierachyNode(Scene* scene, rapidjson::Value& obj, SceneItem* parent = nullptr);
	static void DeserializeSceneHierachyNodeScripts(Scene* scene, SceneItem* item, rapidjson::Value& obj);
	static void LoadScene(const std::string& path);
	static void SaveScene(const Scene& scene, const std::string& path);
};