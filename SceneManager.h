#pragma once
#include "Scene.h"

class SceneManager {
public:
	static std::unordered_map<std::string, Scene> scenes;
	static Scene* activeScene;
	static void AddScene(const std::string& path) {
		scenes[path] = Scene();
	}
	static void OnEntityDestroyed(unsigned int entity) {
		activeScene->OnDestroyEntity(entity);
	}
	static bool IsSceneLoaded(const std::string& path) {
		return scenes.find(path) != scenes.end();
	}
	static void ChangeScene(const std::string& path) {
		activeScene = &scenes[path];
	}
	static void AddEntity(unsigned int entity) {
		activeScene->entities.push_back(entity);
	}
};