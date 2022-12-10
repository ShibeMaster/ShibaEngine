#pragma once
#include <string>
#include "imgui.h"
#include <filesystem>
#include "ProjectItem.h"
#include "SceneLoader.h"
#include "SceneManager.h"
#include "Scripting.h"
#include <unordered_map>
#include <iostream>

struct HierachyTreeNode {
	ProjectItem item;
	std::vector<HierachyTreeNode*> children;
};

class Project {
public:
	std::filesystem::path baseDirectory = std::filesystem::path("C:\\Users\\tombr\\Downloads\\Test Hierachy");
	HierachyTreeNode hierachy;
	HierachyTreeNode scriptHierachy;
	HierachyTreeNode componentHierachy;
	std::unordered_map<std::string, HierachyTreeNode> hierachyMap;

	Project(){}
	Project(const std::string& path) {
		baseDirectory = std::filesystem::path(path);
	}
	ProjectItem& GetItem(const std::string& path) {
		if (hierachyMap.find(path) == hierachyMap.end())
			return hierachy.item;
		return hierachyMap[path].item;
	}

	HierachyTreeNode& CreateHierachyNode(std::string name, std::string path, std::string type, HierachyTreeNode& parentNode, bool isDirectory = false) {
		HierachyTreeNode node = { { name, path, type } };
		hierachyMap[node.item.path] = node;
		parentNode.children.push_back(&hierachyMap[node.item.path]);
		if (isDirectory)
			IterateDirectory(hierachyMap[node.item.path]);
		return hierachyMap[node.item.path];
	}
	void CreateNewSceneNode(const std::string& name, const std::string& path) {
		CreateHierachyNode(name, path, ".ShbaScene", hierachy);
	}
	void IterateDirectory(HierachyTreeNode& directoryNode) {
		for (auto& file : std::filesystem::directory_iterator(directoryNode.item.path)) {
			CreateHierachyNode(file.path().stem().string(), file.path().string(), file.is_directory() ? "" : file.path().extension().string(), directoryNode, file.is_directory());
		}
	}
	void LoadProjectHierachy() {
		hierachy.item.name = "Assets";
		hierachy.item.path = baseDirectory.string();
		IterateDirectory(hierachy);
		hierachyMap[hierachy.item.path] = hierachy;
		LoadProjectScriptHierachy();
		LoadDefaultComponentHierachy();
	}
	void LoadProjectScriptHierachy() {
		scriptHierachy.item.name = "Scripts";
		for (auto comp : Scripting::data.components) {
			CreateHierachyNode(comp.second.name, comp.second.nameSpace + "." + comp.second.name, "Script", scriptHierachy);
		}
		hierachyMap["Scripts"] = scriptHierachy;

	}
	void LoadDefaultComponentHierachy() {
		componentHierachy.item.name = "Components";
		for (auto comp : Engine::GetRegisteredComponents()) {
			CreateHierachyNode(comp, "Components\\" + comp, "Component", componentHierachy);
		}
		hierachyMap["Components"] = componentHierachy;
	}
	void RenderHierachyNode(HierachyTreeNode& node) {

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (node.children.size() > 0) {
			bool open = ImGui::TreeNodeEx(node.item.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted("");
			if (open) {
				for (auto child : node.children)
					RenderHierachyNode(hierachyMap[child->item.path]);
				ImGui::TreePop();
			}
		}
		else {
			ImGui::TreeNodeEx(node.item.name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload((std::string("DRAG_DROP_") + node.item.type).c_str(), node.item.path.c_str(), node.item.path.length() * sizeof(char*));
				ImGui::EndDragDropSource();
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (node.item.type == ".ShbaScene") {
					if (!SceneManager::IsSceneLoaded(node.item.path))
						SceneLoader::LoadScene(node.item.path);

					SceneManager::ChangeScene(node.item.path);
				}
			}
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(node.item.type.c_str());
		}
	}
	void RenderHierachy() {
		ImGui::BeginChild("Project", ImVec2(0, 0), true);
		ImGui::TextUnformatted("Project Hierachy");
		if (ImGui::BeginTable("Project", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody, ImVec2(ImGui::GetWindowSize().x - 15, 420))) {
			float textWidth = ImGui::CalcTextSize("A").x;
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, textWidth * 12.0f);
			ImGui::TableHeadersRow();
			RenderHierachyNode(hierachy);
			RenderHierachyNode(scriptHierachy);
			RenderHierachyNode(componentHierachy);
			// ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, textWidth * 12.0f);
			// ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, textWidth * 18.0f);
			ImGui::EndTable();
		}
		ImGui::EndChild();
	}
};