#pragma once
#include <string>
#include "imgui.h"
#include <filesystem>


struct HierachyTreeNode {
	std::filesystem::path path;
	std::string name;
	std::string type;
	std::vector<HierachyTreeNode> children;
};

class Project {
public:
	std::filesystem::path baseDirectory = std::filesystem::path("C:\\Users\\tombr\\Downloads\\Test Hierachy");
	std::vector<HierachyTreeNode> hierachy;


	void IterateDirectory(HierachyTreeNode& directoryNode, std::filesystem::directory_entry directory) {
		for (auto file : std::filesystem::directory_iterator(directory)) {
			HierachyTreeNode node;
			node.path = file.path();
			node.name = file.path().filename().string();
			if (file.is_directory())
				IterateDirectory(node, file);
			

			directoryNode.children.push_back(node);
		}
	}
	void LoadProjectHierachy() {
		for (auto file : std::filesystem::directory_iterator(baseDirectory)) {
			HierachyTreeNode node;
			node.name = file.path().filename().string();
			node.path = file.path();
			if (file.is_directory()) {
				IterateDirectory(node, file);
			}
			hierachy.push_back(node);
		}
	}
	void RenderHierachyNode(HierachyTreeNode node) {

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (node.children.size() > 0) {
			bool open = ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
			if (open) {
				for (auto child : node.children)
					RenderHierachyNode(child);
				ImGui::TreePop();
			}
		}
		else {
			ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
		}
	}
	void RenderHierachy() {
		ImGui::Begin("Project");
		if (ImGui::BeginTable("Project", 1, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody)) {
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();
			float textWidth = ImGui::CalcTextSize("A").x;
			for (auto node : hierachy) {
				RenderHierachyNode(node);
			}
			// ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, textWidth * 12.0f);
			// ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, textWidth * 18.0f);
			ImGui::EndTable();
		}
		ImGui::End();
	}
};