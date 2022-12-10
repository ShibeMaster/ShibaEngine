#pragma once
#include <mono/metadata/object.h>
#include <unordered_map>
#include <iostream>
#include <rapidjson/document.h>
#include "imgui.h"
class ComponentArray {
public:
	std::string name;
	virtual ~ComponentArray() = default;
	virtual void OnEntityDestroyed(unsigned int entity) = 0;
	virtual void Start() = 0;
	virtual void Update(bool inRuntime) = 0;
	virtual void Add(unsigned int entity) = 0;
	virtual bool HasComponent(unsigned int entity) = 0;
	virtual void DrawComponentGUI(unsigned int entity) = 0;
	virtual void DeserializeComponent(unsigned int entity, rapidjson::Value& obj) = 0;
	virtual void SerializeComponent(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) = 0;
	virtual void GetObject(unsigned int entity, ClassInstance* instance) = 0;
	virtual void SetObject(unsigned int entity, ClassInstance* instance) = 0;
};

template<typename T>
class Components : public ComponentArray {
public:
	std::unordered_map<unsigned int, T> components;
	void Add(unsigned int entity, T component) {
		components[entity] = component;
		components[entity].entity = entity;
		components[entity].Initialize();
		components[entity].Start();
	}
	void Add(unsigned int entity) {
		T value;
		value.entity = entity;
		value.Initialize();
		components[entity] = value;
	}
	bool HasComponent(unsigned int entity) {
		return components.find(entity) != components.end();
	}
	void Remove(unsigned int entity) {
		components.erase(entity);
	}
	void DeserializeComponent(unsigned int entity, rapidjson::Value& obj) {
		components[entity].Deserialize(obj);
	}
	void SerializeComponent(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		components[entity].Serialize(json);
	}
	T& Get(unsigned int entity) {
		return components[entity];
	}
	T* GetPointer(unsigned int entity) {
		return &components[entity];
	}
	void DrawComponentGUI(unsigned int entity) {
		if (HasComponent(entity)) {

			bool componentExists = true;
			if (ImGui::CollapsingHeader(name.c_str(), &componentExists)) {
				components[entity].DrawGUI(entity);
			}
			if (!componentExists)
				Remove(entity);

		}
	}
	void Start() {
		for (auto& comp : components) {
			comp.second.Start();
		}
	}
	void Update(bool inRuntime) {
		for (auto& comp : components) {
			comp.second.Update(inRuntime);
		}
	}
	void GetObject(unsigned int entity, ClassInstance* instance) {
		components[entity].GetObject(instance);
	}
	void SetObject(unsigned int entity, ClassInstance* instance) {
		components[entity].SetObject(instance);
	}
	void OnEntityDestroyed(unsigned int entity) {
		if (components.find(entity) != components.end())
			Remove(entity);
	}
};