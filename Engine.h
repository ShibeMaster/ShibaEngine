#pragma once
#include "ScriptingTypes.h"
#include <rapidjson/PrettyWriter.h>
#include "EntityManager.h"
#include "ComponentManager.h"
#include <mono/metadata/object.h>
#include "SceneManager.h"
class Engine {
private:
	static EntityManager entityManager;
	static ComponentManager componentManager;
public:

	template<class T>
	static T& GetComponent(unsigned int entity) {
		return componentManager.GetComponent<T>(entity);
	}
	template<typename T>
	static T* GetComponentPointer(unsigned int entity) {
		return componentManager.GetComponentPointer<T>(entity);
	}
	template<class T>
	static void RegisterComponent() {
		componentManager.RegisterComponent<T>();
	}
	template<class T>
	static void AddComponent(unsigned int entity, T component = T()) {
		component.entity = entity;
		componentManager.AddComponent<T>(entity, component);
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

	template<typename T>
	static std::vector<unsigned int> FindComponentsInScene() {
		std::vector<unsigned int> entitiesWithComponent;
		for (unsigned int entity : SceneManager::activeScene->entities) {
			if (Engine::HasComponent<T>(entity))
				entitiesWithComponent.push_back(entity);
		}
		return entitiesWithComponent;
	}

	static void Update();
	static void Start();
	static unsigned int CreateEntity();
	static void GetCoreComponentObject(unsigned int entity, const std::string& name, ClassInstance* instance);
	static void SetCoreComponent(unsigned int entity, const std::string& name, ClassInstance* instance);
	static void DestroyEntity(unsigned int entity);
	static void AddScript(unsigned int entity, const std::string& component);
	static void RemoveScript(unsigned int entity, const std::string& component);
	static std::vector<std::string> GetEntityScripts(unsigned int entity);
	static void DrawEntityComponentGUI(unsigned int entity);
	/// <summary>
	/// This iterates through all components attached to an entity and serializes them into the existing json document
	/// </summary>
	/// <param name="entity"></param>
	/// <param name="json"></param>
	static void SerializeEntityComponents(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static void DeserializeEntityComponents(unsigned int entity, rapidjson::Value& obj);

	static std::vector<unsigned int> FindScriptInScene(const std::string& name);



	static void AddComponent(unsigned int entity, const std::string& name);
	static std::vector<std::string> GetRegisteredComponents();
	static std::vector<std::string> GetEntityComponents(unsigned int entity);

	static bool HasComponent(unsigned int entity, const std::string& name);

	/// <summary>
	/// We should have this method to seperate the renderering of the meshrenderer and spriterenderer components from the normal updating methods, this will also allow us to not require a "inRuntime" parameter with all of them.
	/// </summary>
	static void Render();

};