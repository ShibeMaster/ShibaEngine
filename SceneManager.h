#pragma once
#include "Scene.h"

class SceneManager {
private:
	static std::map<std::string, Scene> scenes;

public:
	static Scene* activeScene;
	static void AddScene(const std::string& path);
	static void OnEntityDestroyed(unsigned int entity);
	static bool IsSceneLoaded(const std::string& path);
	static void ChangeScene(const std::string& path);
	static void AddEntity(unsigned int entity);
};