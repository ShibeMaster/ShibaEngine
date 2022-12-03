#pragma once
#include <mono/metadata/object.h>
#include <unordered_map>
#include <iostream>
class ComponentArray {
public:
	virtual ~ComponentArray() = default;
	virtual void OnEntityDestroyed(unsigned int entity) = 0;
	virtual void Start() = 0;
	virtual void Update(bool inRuntime) = 0;
	virtual void Add(unsigned int entity) = 0;
	virtual bool HasComponent(unsigned int) = 0;
	virtual void DrawComponentGUI(unsigned int entity) = 0;
	virtual void GetObject(unsigned int entity, ClassInstance* instance) = 0;
	virtual void SetObject(unsigned int entity, ClassInstance* instance) = 0;
};

template<typename T>
class Components : public ComponentArray {
private:
public:
	std::unordered_map<unsigned int, T> components;
	void Add(unsigned int entity, T component) {
		components[entity] = component;
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
	T& Get(unsigned int entity) {
		return components[entity];
	}
	T* GetPointer(unsigned int entity) {
		return &components[entity];
	}
	void DrawComponentGUI(unsigned int entity) {
		if (components.find(entity) != components.end())
			components[entity].DrawGUI(entity);
	}
	void Start() {
		for (auto component : components) {
			components[component.first].Start();
		}
	}
	void Update(bool inRuntime) {
		for (auto component : components) {
			components[component.first].Update(inRuntime);
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