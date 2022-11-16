#pragma once
#include "Scene.h"
class SceneManager {
public:
	static Scene activeScene;
	static void ChangeScene(Scene scene) {
		activeScene = scene;
	}
	static void AddEntity(unsigned int entity) {
		activeScene.entities.push_back(entity);
	}
};