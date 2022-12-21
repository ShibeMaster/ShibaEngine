#include "SceneManager.h"
std::map<std::string, Scene> SceneManager::scenes;
Scene* SceneManager::activeScene;


void SceneManager::AddScene(const std::string& path) {
	scenes[path] = Scene();
}
void SceneManager::OnEntityDestroyed(unsigned int entity) {
	activeScene->OnDestroyEntity(entity);
}
bool SceneManager::IsSceneLoaded(const std::string& path) {
	return scenes.find(path) != scenes.end();
}
void SceneManager::ChangeScene(const std::string& path) {
	activeScene = &scenes[path];
}
void SceneManager::AddEntity(unsigned int entity) {
	activeScene->entities.push_back(entity);
}