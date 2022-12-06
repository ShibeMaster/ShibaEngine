#pragma once
#include <vector>
#include "ModelLoader.h"
#include <unordered_map>
#include <string>
#include "GL/glew.h"
#include "Mesh.h"
struct SceneItem {
	unsigned int entity;
	std::string name = "New Entity";
	std::vector<SceneItem*> children;
	bool hasParent;
	SceneItem* parent;
};
class Scene {
public:
	std::string name;
	std::vector<unsigned int> entities;
	std::unordered_map<unsigned int, SceneItem> items;
	std::vector<SceneItem*> hierachy;
	Shader shader;
	Mesh skybox;
	unsigned int skyTexture;

	void OnCreateEntity(unsigned int entity) {
		entities.push_back(entity);
		int newEntities = 0;
		for (auto& item : items) {
			if (item.second.name.find("New Entity") != std::string::npos)
				newEntities++;
		}
		SceneItem item = { entity, "New Entity" + ((newEntities > 0) ? " (" + std::to_string(newEntities) + ")" : ""), {}, false, nullptr};
		items[entity] = item;
		hierachy.push_back(&items[entity]);
	}
	void OnDestroyEntity(unsigned int entity) {
		DestroyChildren(entity);
		entities.erase(std::find(entities.begin(), entities.end(), entity));
		items.erase(entity);
	}
	void LoadSkybox() {
		const char* skyboxVertex = R"GLSL(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;


out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main(){
    TexCoords = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}
)GLSL";
		const char* skyboxFragment = R"GLSL(
#version 330 core

out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube cubemap;

void main(){
	FragColor = texture(cubemap, TexCoords);
}
)GLSL";

		shader = Shader(skyboxVertex, skyboxFragment);
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
			{ glm::vec3( 1.0f, -1.0f, -1.0f)},
			{ glm::vec3( 1.0f, -1.0f, -1.0f)},
			{ glm::vec3( 1.0f,  1.0f, -1.0f)},
			{ glm::vec3(-1.0f,  1.0f, -1.0f)},
						
			{ glm::vec3(-1.0f, -1.0f,  1.0f)},
			{ glm::vec3(-1.0f, -1.0f, -1.0f)},
			{ glm::vec3(-1.0f,  1.0f, -1.0f)},
			{ glm::vec3(-1.0f,  1.0f, -1.0f)},
			{ glm::vec3(-1.0f,  1.0f,  1.0f)},
			{ glm::vec3(-1.0f, -1.0f,  1.0f)},
						
			{ glm::vec3( 1.0f, -1.0f, -1.0f)},
			{ glm::vec3( 1.0f, -1.0f,  1.0f)},
			{ glm::vec3( 1.0f,  1.0f,  1.0f)},
			{ glm::vec3( 1.0f,  1.0f,  1.0f)},
			{ glm::vec3( 1.0f,  1.0f, -1.0f)},
			{ glm::vec3( 1.0f, -1.0f, -1.0f)},
						  
			{ glm::vec3(-1.0f, -1.0f,  1.0f)},
			{ glm::vec3(-1.0f,  1.0f,  1.0f)},
			{ glm::vec3( 1.0f,  1.0f,  1.0f)},
			{ glm::vec3( 1.0f,  1.0f,  1.0f)},
			{ glm::vec3( 1.0f, -1.0f,  1.0f)},
			{ glm::vec3(-1.0f, -1.0f,  1.0f)},
						 
			{ glm::vec3(-1.0f,  1.0f, -1.0f)},
			{ glm::vec3( 1.0f,  1.0f, -1.0f)},
			{ glm::vec3( 1.0f,  1.0f,  1.0f)},
			{ glm::vec3( 1.0f,  1.0f,  1.0f)},
			{ glm::vec3(-1.0f,  1.0f,  1.0f)},
			{ glm::vec3(-1.0f,  1.0f, -1.0f)},
						  
			{ glm::vec3(-1.0f, -1.0f, -1.0f)},
			{ glm::vec3(-1.0f, -1.0f,  1.0f)},
			{ glm::vec3( 1.0f, -1.0f, -1.0f)},
			{ glm::vec3( 1.0f, -1.0f, -1.0f)},
			{ glm::vec3(-1.0f, -1.0f,  1.0f)},
			{ glm::vec3( 1.0f, -1.0f,  1.0f)} 
		};
		skybox = Mesh(skyboxVertices, { 0 });
	
	}
	void RenderSkybox() {
		glDepthMask(false);
		glBindVertexArray(skybox.vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(true);
	}
	void DestroyChildren(unsigned int entity) {
		for (auto& child : items[entity].children) {
			if (child->children.size() > 0)
				DestroyChildren(entity);

			entities.erase(std::find(entities.begin(), entities.end(), child->entity));
			items.erase(child->entity);
		}
	}
	void MoveEntityToChild(unsigned int entity, unsigned int parent) {
		if (items[entity].hasParent) {
			auto position = std::find(items[entity].parent->children.begin(), items[entity].parent->children.end(), &items[entity]);
			items[entity].parent->children.erase(position);
		}
		items[parent].children.push_back(&items[entity]);
		items[entity].parent = &items[parent];
		items[entity].hasParent = true;
		if(std::find(hierachy.begin(), hierachy.end(), &items[entity]) != hierachy.end())
			hierachy.erase(std::find(hierachy.begin(), hierachy.end(), &items[entity]));
	}

};