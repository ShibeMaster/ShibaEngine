#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Transform.h"
#include "SceneManager.h"
class Engine {
private:
	static EntityManager entityManager;
	static ComponentManager componentManager;
public:
	static void Update(bool inRuntime) {
		componentManager.Update(inRuntime);
	}
	static void Start() {
		componentManager.Start();
	}
	static unsigned int CreateEntity() {
		unsigned int entity = entityManager.CreateEntity();
		AddComponent<Transform>(entity, Transform{});
		SceneManager::AddEntity(entity);
		return entity;
	}
	static void DestroyEntity(unsigned int entity) {
		entityManager.DestroyEntity(entity);
		componentManager.OnEntityDestroyed(entity);
	}
	static void AddScript(unsigned int entity, const std::string& component) {
		componentManager.AddScript(entity, component);
	}
	static void RemoveScript(unsigned int entity, const std::string& component) {
		std::cout << "removed script" << std::endl;
		componentManager.RemoveScript(entity, component);
	}
	static std::vector<std::string> GetEntityScripts(unsigned int entity) {
		return componentManager.GetEntityScripts(entity);
	}
	static void DrawEntityComponentGUI(unsigned int entity) {
		componentManager.DrawEntityComponentGUI(entity);
	}
	template<typename T>
	static std::vector<unsigned int> FindComponentsInScene() {
		std::vector<unsigned int> entitiesWithComponent;
		for (unsigned int entity : SceneManager::activeScene.entities) {
			if (Engine::HasComponent<T>(entity))
				entitiesWithComponent.push_back(entity);
		}
		return entitiesWithComponent;
	}

	static std::vector<unsigned int> FindScriptInScene(const std::string& name) {
		std::vector<unsigned int> entitiesWithComponent;
		for (unsigned int entity : SceneManager::activeScene.entities) {
			const auto& scripts = componentManager.GetEntityScripts(entity);
			if (std::find(scripts.begin(), scripts.end(), name) != scripts.end())
				entitiesWithComponent.push_back(entity);
		}
		return entitiesWithComponent;
	}
	template<class T>
	static T& GetComponent(unsigned int entity) {
		return componentManager.GetComponent<T>(entity);
	}
	template<class T>
	static void RegisterComponent() {
		componentManager.RegisterComponent<T>();
	}
	template<class T>
	static 	void AddComponent(unsigned int entity, T component) {
		component.entity = entity;
		componentManager.AddComponent<T>(entity, component);
	}
	static void AddComponent(unsigned int entity, const std::string& name) {
		componentManager.AddComponent(entity, name);
	}
	static std::vector<std::string> GetRegisteredComponents() {
		return componentManager.GetRegisteredComponents();
	}
	template<class T>
	static void RemoveComponent(unsigned int entity) {
		std::cout << "removed component" << std::endl;
		componentManager.RemoveComponent<T>(entity);
	}
	template<class T>
	static bool HasComponent(unsigned int entity) {
		return componentManager.HasComponent<T>(entity);
	}

};