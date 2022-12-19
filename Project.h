#pragma once
#include <string>
#include "imgui.h"
#include <filesystem>
#include "ProjectItem.h"
#include "SceneLoader.h"
#include "SceneManager.h"
#include "Scripting.h"
#include "ProjectSettings.h"
#include <unordered_map>
#include <iostream>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

struct HierachyTreeNode {
	ProjectItem item;
	std::vector<HierachyTreeNode*> children;
};

class Project {
public:
	ProjectSettings settings;
	HierachyTreeNode hierachy;
	HierachyTreeNode scriptHierachy;
	HierachyTreeNode componentHierachy;
	std::unordered_map<std::string, HierachyTreeNode> hierachyMap;

	Project(){}
	Project(const std::string& path) {
		settings.directory = path;
	}
	ProjectItem& GetItem(const std::string& path) {
		if (hierachyMap.find(path) == hierachyMap.end())
			return hierachy.item;
		return hierachyMap[path].item;
	}
	void ReloadProject() {
		hierachy = HierachyTreeNode();
		scriptHierachy = HierachyTreeNode();
		componentHierachy = HierachyTreeNode();
		hierachyMap.clear();
		LoadProjectHierachy();;
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
	void SaveProject() {
		if (settings.name == "")
			settings.name = "New Project";
		std::string path = settings.directory + settings.name + ".SHBAPROJ";
		rapidjson::StringBuffer str;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> json(str);
		json.StartObject();
		json.Key("Name");
		json.String(settings.name.c_str());
		json.Key("Directory");
		json.String(settings.directory.c_str());
		json.Key("Last Loaded Scene");
		json.String(SceneManager::activeScene->path.c_str());
		json.Key("Has Project");
		json.Bool(settings.hasProject);
		if (settings.hasProject) {
			json.Key("Project Path");
			json.String(settings.projectPath.c_str());
		}

		json.EndObject();
		auto file = std::ofstream(path, std::ofstream::out | std::ofstream::trunc);
		file << str.GetString();
		file.close();
	}
	void CreateProject() {
		if(!std::filesystem::exists(settings.directory + "Scripts\\"))
			std::filesystem::create_directory(settings.directory + "Scripts\\");
		settings.projectPath = settings.directory + "Scripts\\" + settings.name + ".csproj";
		settings.hasProject = true;
		Scripting::CreateVSProject(settings.projectPath);
		system(std::string("\"" + settings.projectPath + "\"").c_str());
	}
	std::string GetAssemblyPath() {
		return settings.directory + "Scripts\\bin\\Debug\\net5.0\\" + settings.name + ".dll";
	}
	void CreateNewScript(const std::string& name) {
			std::ofstream file(settings.directory + "Scripts\\" + name + ".cs", std::ofstream::out | std::ofstream::trunc);
			std::string fileData = R"C#(
using ShibaEngineCore;

class )C#" + name + R"C#( : Component
{
	public override void Start(){
		ShibaEngineCore.Console.LogMessage("Hello World!");
	}

	public override void Update(){}

}
)C#";
			file << fileData;
			file.close();
	}
	void LoadProjectHierachy() {
		hierachy.item.name = "Assets";
		hierachy.item.path = settings.directory;
		IterateDirectory(hierachy);
		hierachyMap[hierachy.item.path] = hierachy;
		LoadProjectScriptHierachy();
		LoadDefaultComponentHierachy();
	}
	void LoadProjectScriptHierachy() {
		scriptHierachy.item.name = "Scripts";
		for (auto& comp : Scripting::data.components) {
			CreateHierachyNode(comp.second.name, comp.second.nameSpace + "." + comp.second.name, "Script", scriptHierachy);
		}
		hierachyMap["Scripts"] = scriptHierachy;

	}
	void LoadDefaultComponentHierachy() {
		componentHierachy.item.name = "Components";
		for (auto& comp : Engine::GetRegisteredComponents()) {
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
				else {
					system(std::string("\"" + node.item.path + "\"").c_str());
				}
			}
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(node.item.type.c_str());
		}
	}
	void RenderHierachy() {
		ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::TextUnformatted("Project Hierachy");
		if (ImGui::BeginTable("ProjectTable", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody, ImVec2(ImGui::GetWindowSize().x - 15, 420))) {
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
		ImGui::End();
	}
};