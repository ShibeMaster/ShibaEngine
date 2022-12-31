#pragma once
#include <vector>
#include <map>
#include <string>
#include "Mesh.h"
struct SceneItem {
	unsigned int entity;
	std::string name = "New Entity";
	std::vector<SceneItem*> children;
	bool hasParent;
	SceneItem* parent;
};
class Scene {
private:
	Mesh skybox;
	unsigned int skyTexture;

public:
	std::string name = "New Scene";
	std::vector<unsigned int> entities;
	std::vector<SceneItem*> hierachy;

	std::map<unsigned int, SceneItem> items;
	std::map<std::string, SceneItem*> names;
	Shader shader;
	std::string path = "No Path";

	void OnCreateEntity(unsigned int entity);
	void DestroyEntityHierachyInstances(unsigned int entity, SceneItem* item);
	void OnDestroyEntity(unsigned int entity);
	void LoadSkybox();
	void RenderSkybox();
	unsigned int GetInstanceFromName(const std::string& name);
	void DestroyChildren(unsigned int entity);
	void MoveEntityToChild(unsigned int entity, unsigned int parent);
	

};