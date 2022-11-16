#pragma once
#include <unordered_map>
class ComponentArray {
public:
	virtual ~ComponentArray() = default;
	virtual void OnEntityDestroyed(unsigned int entity) = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;
};

template<typename T>
class Components : public ComponentArray {
private:
public:
	std::unordered_map<unsigned int, T> components;
	void Add(unsigned int entity, T component) {
		components[entity] = component;
	}
	void Remove(unsigned int entity) {
		components.erase(entity);
	}
	T& Get(unsigned int entity) {
		return components[entity];
	}
	void Start() {
		for (auto component : components) {
			components[component.first].Start();
		}
	}
	void Update() {
		for (auto component : components) {
			components[component.first].Update();
		}
	}
	void OnEntityDestroyed(unsigned int entity) {
		if (components.find(entity) != components.end())
			Remove(entity);
	}
};