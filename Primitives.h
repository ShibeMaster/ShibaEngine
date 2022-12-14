#pragma once
#include <vector>
#include <string>
#include "MeshCollisionBox.h"
#include "Scripting.h"
#include "Engine.h"
#include "MeshRenderer.h"
#include "imgui.h"
#include "Transform.h"

class Primitives {
public:
	/// <summary>
	/// An unordered map of all 
	/// </summary>
	static std::vector<std::string> primitives;
	static void RenderPrimitiveSelection() {
		for (auto& primitive : primitives) {
			if (ImGui::MenuItem(primitive.c_str()))
				CreatePrimitive(primitive);
		}
	}
	static void CreatePrimitive(const std::string& name) {
		unsigned int entity = Engine::CreateEntity();
		Engine::AddComponent<Transform>(entity, Transform{});
		Scripting::OnEntityCreated(entity);
		SceneManager::activeScene->OnCreateEntity(entity);
		Engine::AddComponent<MeshRenderer>(entity);
		auto& renderer = Engine::GetComponent<MeshRenderer>(entity);
		renderer.modelItem = { name, "Primitives\\" + name + ".fbx", ".fbx"};
		renderer.ReloadMesh();

		Engine::AddComponent<MeshCollisionBox>(entity);
		auto& collision = Engine::GetComponent<MeshCollisionBox>(entity);
		collision.meshItem = { name, "Primitives\\" + name + ".fbx", ".fbx" };
		collision.LoadMeshBox();
	}
};