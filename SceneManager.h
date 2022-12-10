#pragma once
#include "Scene.h"
class SceneManager {
public:
	static std::vector<Scene> scenes;
	static Scene* activeScene;
	static void AddScene() {
		scenes.push_back(Scene());
	}
	static void ChangeScene(int index) {
		activeScene = &scenes[index];
	}
	static void AddEntity(unsigned int entity) {
		activeScene->entities.push_back(entity);
	}
};