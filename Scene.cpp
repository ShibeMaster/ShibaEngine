#include "Scene.h"
#include "Renderer.h"
#include "GL/glew.h"
#include "ModelLoader.h"

void Scene::OnCreateEntity(unsigned int entity) {
	entities.push_back(entity);
	int newEntities = 0;
	for (auto& item : items) {
		if (item.second.name.find("New Entity") != std::string::npos)
			newEntities++;
	}
	SceneItem item = { entity, "New Entity" + ((newEntities > 0) ? " (" + std::to_string(newEntities) + ")" : ""), {}, false, nullptr };
	items[entity] = item;
	hierachy.push_back(&items[entity]);
}
void Scene::DestroyEntityHierachyInstances(unsigned int entity, SceneItem* item) {
	if (item->entity == entity)
		if (!item->hasParent)
			hierachy.erase(std::find(hierachy.begin(), hierachy.end(), item));
		else
			item->parent->children.erase(std::find(item->parent->children.begin(), item->parent->children.end(), item));
	else if (item->children.size() > 0)
		for (auto& child : item->children)
			DestroyEntityHierachyInstances(entity, child);

}
void Scene::OnDestroyEntity(unsigned int entity) {
	DestroyChildren(entity);
	for (auto& item : hierachy) {
		DestroyEntityHierachyInstances(entity, item);
	}
	entities.erase(std::find(entities.begin(), entities.end(), entity));
	items.erase(entity);
}
void Scene::LoadSkybox() {

	std::vector<std::string> faces = {
		"Day_Right.png",
		"Day_Left.png",
		"Day_Top.png",
		"Day_Down.png",
		"Day_Front.png",
		"Day_Back.png"
	};
	skyTexture = ModelLoader::LoadCubeMap(faces);
	std::vector<Vertex> skyboxVertices = {
		{ glm::vec3(-1.0f,  1.0f, -1.0f)},
		{ glm::vec3(-1.0f, -1.0f, -1.0f)},
		{ glm::vec3(1.0f, -1.0f, -1.0f)},
		{ glm::vec3(1.0f, -1.0f, -1.0f)},
		{ glm::vec3(1.0f,  1.0f, -1.0f)},
		{ glm::vec3(-1.0f,  1.0f, -1.0f)},

		{ glm::vec3(-1.0f, -1.0f,  1.0f)},
		{ glm::vec3(-1.0f, -1.0f, -1.0f)},
		{ glm::vec3(-1.0f,  1.0f, -1.0f)},
		{ glm::vec3(-1.0f,  1.0f, -1.0f)},
		{ glm::vec3(-1.0f,  1.0f,  1.0f)},
		{ glm::vec3(-1.0f, -1.0f,  1.0f)},

		{ glm::vec3(1.0f, -1.0f, -1.0f)},
		{ glm::vec3(1.0f, -1.0f,  1.0f)},
		{ glm::vec3(1.0f,  1.0f,  1.0f)},
		{ glm::vec3(1.0f,  1.0f,  1.0f)},
		{ glm::vec3(1.0f,  1.0f, -1.0f)},
		{ glm::vec3(1.0f, -1.0f, -1.0f)},

		{ glm::vec3(-1.0f, -1.0f,  1.0f)},
		{ glm::vec3(-1.0f,  1.0f,  1.0f)},
		{ glm::vec3(1.0f,  1.0f,  1.0f)},
		{ glm::vec3(1.0f,  1.0f,  1.0f)},
		{ glm::vec3(1.0f, -1.0f,  1.0f)},
		{ glm::vec3(-1.0f, -1.0f,  1.0f)},

		{ glm::vec3(-1.0f,  1.0f, -1.0f)},
		{ glm::vec3(1.0f,  1.0f, -1.0f)},
		{ glm::vec3(1.0f,  1.0f,  1.0f)},
		{ glm::vec3(1.0f,  1.0f,  1.0f)},
		{ glm::vec3(-1.0f,  1.0f,  1.0f)},
		{ glm::vec3(-1.0f,  1.0f, -1.0f)},

		{ glm::vec3(-1.0f, -1.0f, -1.0f)},
		{ glm::vec3(-1.0f, -1.0f,  1.0f)},
		{ glm::vec3(1.0f, -1.0f, -1.0f)},
		{ glm::vec3(1.0f, -1.0f, -1.0f)},
		{ glm::vec3(-1.0f, -1.0f,  1.0f)},
		{ glm::vec3(1.0f, -1.0f,  1.0f)}
	};
	skybox = Mesh(skyboxVertices, { 0 });
	std::cout << "loaded skybox" << std::endl;

}
void Scene::RenderSkybox() {
	glDepthMask(false);
	glBindVertexArray(skybox.vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(true);
}
void Scene::DestroyChildren(unsigned int entity) {
	for (auto& child : items[entity].children) {
		if (child->children.size() > 0)
			DestroyChildren(entity);

		entities.erase(std::find(entities.begin(), entities.end(), child->entity));
		items.erase(child->entity);
	}
}
unsigned int Scene::GetInstanceFromName(const std::string& name) {
	if (names.find(name) != names.end())
		return names[name]->entity;
	return -1;
}
void Scene::MoveEntityToChild(unsigned int entity, unsigned int parent) {
	if (items[entity].hasParent) {
		auto position = std::find(items[entity].parent->children.begin(), items[entity].parent->children.end(), &items[entity]);
		items[entity].parent->children.erase(position);
	}
	items[parent].children.push_back(&items[entity]);
	items[entity].parent = &items[parent];
	items[entity].hasParent = true;
	if (std::find(hierachy.begin(), hierachy.end(), &items[entity]) != hierachy.end())
		hierachy.erase(std::find(hierachy.begin(), hierachy.end(), &items[entity]));
}
