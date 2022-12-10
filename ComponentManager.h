#pragma once
#include "ComponentArray.h"
#include <unordered_map>
#include <memory>
#include "ScriptingTypes.h"
#include <iostream>
#include <rapidjson/document.h>
#include <string>
#include <typeinfo>
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
	std::vector<std::string> GetRegisteredComponents() {
		return registeredComponents;
	}
	void Start() {
		for (auto& compArr : componentArrays) {
			componentArrays[compArr.first]->Start();
		}
	}
	void Update(bool inRuntime) {
		for (auto& compArr : componentArrays) {
			componentArrays[compArr.first]->Update(inRuntime);
		}
	}
	void SetCoreComponent(unsigned int entity, const std::string& name, ClassInstance* instance) {
		componentArrays[name]->SetObject(entity, instance);
	}
	void GetCoreComponentObject(unsigned int entity, const std::string& name, ClassInstance* instance) {
		componentArrays[name]->GetObject(entity, instance);
	}
	void DeserializeEntityComponents(unsigned int entity, rapidjson::Value& obj) {
		for (auto& compArr : componentArrays) {
			if (obj.HasMember(compArr.first.c_str())) {
				compArr.second->Add(entity);
				compArr.second->DeserializeComponent(entity, obj[compArr.first.c_str()]);
			}
		}
	}
	void SerializeEntityComponents(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		for (auto& compArr : componentArrays) {
			if (compArr.second->HasComponent(entity)) {
				json->Key(compArr.first.c_str());
				json->StartObject();
				std::cout << "here: " << compArr.first << std::endl;
				compArr.second->SerializeComponent(entity, json);
				std::cout << "here 2" << std::endl;
				json->EndObject();
			}
		}
	}
	std::vector<std::string> GetEntityComponents(unsigned int entity) {
		std::vector<std::string> comps;
		for (auto& comp : componentArrays) {
			if (comp.second->HasComponent(entity))
				comps.push_back(comp.first);
		}
		return comps;
	}
	void DrawEntityComponentGUI(unsigned int entity) {
		for (auto& comp : componentArrays) {
			if(comp.second->name != "Transform")
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
	T* GetComponentPointer(unsigned int entity) {
		return GetComponents<T>()->GetPointer(entity);
	}
	template<typename T>
	bool HasComponent(unsigned int entity) {
		auto comps = GetComponents<T>()->components;
		return comps.find(entity) != comps.end();
	}
	bool HasComponent(unsigned int entity, const std::string& name) {
		if (componentArrays.find(name) != componentArrays.end()) {
			std::cout << name << std::endl;
			return componentArrays[name]->HasComponent(entity);
		}
	}
	void OnEntityDestroyed(unsigned int entity) {
		for (auto comp : componentArrays) {
			comp.second->OnEntityDestroyed(entity);
		}
	}
};