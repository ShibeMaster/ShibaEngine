#pragma once
#include "ComponentArray.h"
#include <unordered_map>
#include <memory>
#include <iostream>
#include <typeinfo>
class ComponentManager {
private:
	std::unordered_map<std::string, std::shared_ptr<ComponentArray>> componentArrays;
	std::unordered_map<unsigned int, std::vector<std::string>> entityScripts;
	std::vector<std::string> registeredComponents;
	template<typename T>
	std::shared_ptr<Components<T>> GetComponents() {
		const char* name = typeid(T).name();
		return std::static_pointer_cast<Components<T>>(componentArrays[std::string(name)]);
	}
public:
	template<typename T>
	void RegisterComponent() {
		const char* name = typeid(T).name();
		if (componentArrays.find(name) == componentArrays.end()) {
			componentArrays.insert({ name, std::make_shared<Components<T>>() });
			registeredComponents.push_back(name);
		}
	}
	std::vector<std::string> GetRegisteredComponents() {
		return registeredComponents;
	}
	void Start() {
		for (auto compArr : componentArrays) {
			componentArrays[compArr.first]->Start();
		}
	}
	void Update(bool inRuntime) {
		for (auto compArr : componentArrays) {
			componentArrays[compArr.first]->Update(inRuntime);
		}
	}
	void DrawEntityComponentGUI(unsigned int entity) {
		for (auto comp : componentArrays) {
			componentArrays[comp.first]->DrawComponentGUI(entity);
		}
	}
	void AddScript(unsigned int entity, const std::string& component) {
		if (std::find(entityScripts[entity].begin(), entityScripts[entity].end(), component) != entityScripts[entity].end()) {
			std::cout << "script already attatched to entity" << std::endl;
			return;
		}
		entityScripts[entity].push_back(component);
	
	}
	void RemoveScript(unsigned int entity, const std::string& component) {
		auto location = std::find(entityScripts[entity].begin(), entityScripts[entity].end(), component);
		if (location == entityScripts[entity].end()) {
			std::cout << "no script of that type is attatched to the entity" << std::endl;
			return;
		}
		entityScripts[entity].erase(location);
	}
	std::vector<std::string> GetEntityScripts(unsigned int entity) {
		return entityScripts[entity];
	}
	void AddComponent(unsigned int entity, const std::string& name) {
		if (componentArrays.find(name) != componentArrays.end()) {
			componentArrays[name]->Add(entity);
			std::cout << name << std::endl;
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
	bool HasComponent(unsigned int entity) {
		auto comps = GetComponents<T>()->components;
		return comps.find(entity) != comps.end();
	}
	void OnEntityDestroyed(unsigned int entity) {
		for (auto comp : componentArrays) {
			comp.second->OnEntityDestroyed(entity);
		}
	}
};