#include "ComponentManager.h"
#include <typeinfo>
#include <memory>


std::vector<std::string> ComponentManager::GetRegisteredComponents() {
	return registeredComponents;
}
void ComponentManager::Start() {
	for (auto& compArr : componentArrays) {
		componentArrays[compArr.first]->Start();
	}
}
void ComponentManager::Update() {
	for (auto& compArr : componentArrays) {
		componentArrays[compArr.first]->Update();
	}
}
void ComponentManager::SetCoreComponent(unsigned int entity, const std::string& name, ClassInstance* instance) {
	componentArrays[name]->SetObject(entity, instance);
}
void ComponentManager::GetCoreComponentObject(unsigned int entity, const std::string& name, ClassInstance* instance) {
	componentArrays[name]->GetObject(entity, instance);
}
void ComponentManager::DeserializeEntityComponents(unsigned int entity, rapidjson::Value& obj) {
	for (auto& compArr : componentArrays) {
		if (obj.HasMember(compArr.first.c_str())) {
			if(!compArr.second->HasComponent(entity))
				compArr.second->Add(entity);
			compArr.second->DeserializeComponent(entity, obj[compArr.first.c_str()]);
		}
	}
}
void ComponentManager::SerializeEntityComponents(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
	for (auto& compArr : componentArrays) {
		if (compArr.second->HasComponent(entity)) {
			json->Key(compArr.first.c_str());
			json->StartObject();
			compArr.second->SerializeComponent(entity, json);
			json->EndObject();
		}
	}
}
std::vector<std::string> ComponentManager::GetEntityComponents(unsigned int entity) {
	std::vector<std::string> comps;
	for (auto& comp : componentArrays) {
		if (comp.second->HasComponent(entity))
			comps.push_back(comp.first);
	}
	return comps;
}
void ComponentManager::DrawEntityComponentGUI(unsigned int entity) {
	for (auto& comp : componentArrays) {
		if (comp.second->name != "Transform")
			componentArrays[comp.first]->DrawComponentGUI(entity);
	}
}
void ComponentManager::AddScript(unsigned int entity, const std::string& component) {
	if (std::find(entityScripts[entity].begin(), entityScripts[entity].end(), component) != entityScripts[entity].end()) {
		std::cout << "script already attatched to entity" << std::endl;
		return;
	}
	entityScripts[entity].push_back(component);

}
void ComponentManager::RemoveScript(unsigned int entity, const std::string& component) {
	auto location = std::find(entityScripts[entity].begin(), entityScripts[entity].end(), component);
	if (location == entityScripts[entity].end()) {
		std::cout << "no script of that type is attatched to the entity" << std::endl;
		return;
	}
	entityScripts[entity].erase(location);
}
std::vector<std::string> ComponentManager::GetEntityScripts(unsigned int entity) {
	return entityScripts[entity];
}
void ComponentManager::AddComponent(unsigned int entity, const std::string& name) {
	if (componentArrays.find(name) != componentArrays.end()) {
		componentArrays[name]->Add(entity);
	}
}

bool ComponentManager::HasComponent(unsigned int entity, const std::string& name) {
	if (componentArrays.find(name) != componentArrays.end()) {
		return componentArrays[name]->HasComponent(entity);
	}
}
void ComponentManager::OnEntityDestroyed(unsigned int entity) {
	for (auto& comp : componentArrays) {
		comp.second->OnEntityDestroyed(entity);
	}
}
void ComponentManager::Clear() {
	for (auto& comp : componentArrays) {
		comp.second->Clear();
	}
}
void ComponentManager::Render() {
	componentArrays["MeshRenderer"]->Render();
	componentArrays["ScreenLayer"]->Render();
}