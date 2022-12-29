#pragma once
#include "Engine.h"
ComponentManager Engine::componentManager;
EntityManager Engine::entityManager;

void Engine::Update() {
	componentManager.Update();
}
void Engine::Start() {
	componentManager.Start();
}
unsigned int Engine::CreateEntity() {
	unsigned int entity = entityManager.CreateEntity();
	SceneManager::AddEntity(entity);
	return entity;
}
void Engine::GetCoreComponentObject(unsigned int entity, const std::string& name, ClassInstance* instance) {
	componentManager.GetCoreComponentObject(entity, name, instance);
}
void Engine::SetCoreComponent(unsigned int entity, const std::string& name, ClassInstance* instance) {
	componentManager.SetCoreComponent(entity, name, instance);
}
void Engine::DestroyEntity(unsigned int entity) {
	auto entPos = std::find(SceneManager::activeScene->entities.begin(), SceneManager::activeScene->entities.end(), entity);
	if (entPos != SceneManager::activeScene->entities.end())
		SceneManager::activeScene->entities.erase(entPos);
	entityManager.DestroyEntity(entity);
	componentManager.OnEntityDestroyed(entity);
}
void Engine::AddScript(unsigned int entity, const std::string& component) {
	componentManager.AddScript(entity, component);
}
void Engine::RemoveScript(unsigned int entity, const std::string& component) {
	componentManager.RemoveScript(entity, component);
}
std::vector<std::string> Engine::GetEntityScripts(unsigned int entity) {
	return componentManager.GetEntityScripts(entity);
}
void Engine::DrawEntityComponentGUI(unsigned int entity) {
	componentManager.DrawEntityComponentGUI(entity);
}
/// <summary>
/// This iterates through all components attached to an entity and serializes them into the existing json document
/// </summary>
/// <param name="entity"></param>
/// <param name="json"></param>
void Engine::SerializeEntityComponents(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
	componentManager.SerializeEntityComponents(entity, json);
}
void Engine::DeserializeEntityComponents(unsigned int entity, rapidjson::Value& obj) {
	componentManager.DeserializeEntityComponents(entity, obj);
}	
std::vector<unsigned int> Engine::FindScriptInScene(const std::string& name) {
	std::vector<unsigned int> entitiesWithComponent;
	for (unsigned int entity : SceneManager::activeScene->entities) {
		const auto& scripts = componentManager.GetEntityScripts(entity);
		if (std::find(scripts.begin(), scripts.end(), name) != scripts.end())
			entitiesWithComponent.push_back(entity);
	}
	return entitiesWithComponent;
}

void Engine::AddComponent(unsigned int entity, const std::string& name) {
	componentManager.AddComponent(entity, name);
}
std::vector<std::string> Engine::GetRegisteredComponents() {
	return componentManager.GetRegisteredComponents();
}
std::vector<std::string> Engine::GetEntityComponents(unsigned int entity) {
	return componentManager.GetEntityComponents(entity);
}
bool Engine::HasComponent(unsigned int entity, const std::string& name) {
	return componentManager.HasComponent(entity, name);
}

/// <summary>
/// We should have this method to seperate the renderering of the meshrenderer and spriterenderer components from the normal updating methods, this will also allow us to not require a "inRuntime" parameter with all of them.
/// </summary>
void Engine::Render() {
	componentManager.Render();
}