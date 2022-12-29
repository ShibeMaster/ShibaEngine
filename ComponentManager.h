#pragma once
#include "ComponentArray.h"
#include <unordered_map>
#include "ScriptingTypes.h"
#include <iostream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <string>
class ComponentManager {
private:
	std::unordered_map<std::string, std::shared_ptr<ComponentArray>> componentArrays;
	std::unordered_map<unsigned int, std::vector<std::string>> entityScripts;
	std::vector<std::string> registeredComponents;
	template<typename T>
	std::shared_ptr<Components<T>> GetComponents() {
		auto compName = std::string(typeid(T).name());
		std::string classTest = std::string("class ");
		compName.erase(compName.find(classTest), classTest.length());
		return std::static_pointer_cast<Components<T>>(componentArrays[compName]);
	}
public:

	template<typename T>
	void RegisterComponent() {
		auto compName = std::string(typeid(T).name());
		std::string classTest = std::string("class ");
		compName.erase(compName.find(classTest), classTest.length());
		std::cout << compName << std::endl;
		if (componentArrays.find(compName) == componentArrays.end()) {
			componentArrays.insert({ compName, std::make_shared<Components<T>>() });
			componentArrays[compName]->name = compName;
			registeredComponents.push_back(compName);
		}
	}
	template<typename T>
	void AddComponent(unsigned int entity, T component) {
		GetComponents<T>()->Add(entity, component);
	}
	template<typename T>
	void RemoveComponent(unsigned int entity) {
		GetComponents<T>()->Remove(entity);
	}
	template<typename T>
	T& GetComponent(unsigned int entity) {
		return GetComponents<T>()->Get(entity);
	}
	template<typename T>
	T* GetComponentPointer(unsigned int entity) {
		return GetComponents<T>()->GetPointer(entity);
	}
	template<typename T>
	bool HasComponent(unsigned int entity) {
		auto comps = GetComponents<T>()->components;
		return comps.find(entity) != comps.end();
	}
	std::vector<std::string> GetRegisteredComponents();
	void Start();
	void Update();
	void SetCoreComponent(unsigned int entity, const std::string& name, ClassInstance* instance);
	void GetCoreComponentObject(unsigned int entity, const std::string& name, ClassInstance* instance);
	void DeserializeEntityComponents(unsigned int entity, rapidjson::Value& obj);
	void SerializeEntityComponents(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	std::vector<std::string> GetEntityComponents(unsigned int entity);
	void DrawEntityComponentGUI(unsigned int entity);
	void AddScript(unsigned int entity, const std::string& component);
	void RemoveScript(unsigned int entity, const std::string& component);
	std::vector<std::string> GetEntityScripts(unsigned int entity);
	void AddComponent(unsigned int entity, const std::string& name);
	bool HasComponent(unsigned int entity, const std::string& name);
	void OnEntityDestroyed(unsigned int entity);
	void Clear();
	void Render();
};