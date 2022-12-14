#include "Project.h"
#include "SceneLoader.h"
#include "SceneManager.h"
#include "Scripting.h"
#include <filesystem>
#include <sstream>
#include "FileExtensions.h"
#include "Engine.h"
#include <rapidjson/PrettyWriter.h>
#include <stdio.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "imgui.h"

ProjectItem& Project::GetItem(const std::string& path) {
	if (hierachyMap.find(path) == hierachyMap.end())
		return hierachy.item;
	return hierachyMap[path].item;
}
void Project::ReloadProject() {
	hierachy = HierachyTreeNode();
	scriptHierachy = HierachyTreeNode();
	componentHierachy = HierachyTreeNode();
	shaderHierachy = HierachyTreeNode();
	hierachyMap.clear();
	LoadProjectHierachy();;
}
HierachyTreeNode& Project::CreateHierachyNode(const std::string& name, const std::string& path, const std::string& type, HierachyTreeNode& parentNode, bool isDirectory) {
	HierachyTreeNode node = { { name, path, type } };
	hierachyMap[node.item.path] = node;
	parentNode.children.push_back(&hierachyMap[node.item.path]);
	if (isDirectory)
		IterateDirectory(hierachyMap[node.item.path]);
	return hierachyMap[node.item.path];
}
void Project::CreateNewSceneNode(const std::string& name, const std::string& path) {
	CreateHierachyNode(name, path, ".ShbaScene", hierachy);
}
void Project::IterateDirectory(HierachyTreeNode& directoryNode) {
	for (auto& file : std::filesystem::directory_iterator(directoryNode.item.path)) {
		CreateHierachyNode(file.path().stem().string(), file.path().string(), file.is_directory() ? "" : file.path().extension().string(), directoryNode, file.is_directory());
	}
}
void Project::SaveProject(const std::string& specificPath) {
	if (settings.name == "")
		settings.name = "New Project";
	std::string path = specificPath == "None" ? settings.directory + settings.name + ".SHBAPROJ" : specificPath;
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
	json.Key("Starting Scene");
	json.String(settings.startingScenePath.c_str());
	json.Key("In Engine");
	json.Bool(settings.inEngine);
	if (settings.hasProject) {
		json.Key("Project Path");
		json.String(settings.projectPath.c_str());
	}

	json.EndObject();
	FileExtensions::CreateAndWriteToFile(path, str.GetString());
}
void Project::InitializeDirectories() {
	if (!std::filesystem::exists(settings.directory + "Scripts\\"))
		std::filesystem::create_directory(settings.directory + "Scripts\\");
	if (!std::filesystem::exists(settings.directory + "Assets\\"))
		std::filesystem::create_directory(settings.directory + "Assets\\");
	if (!std::filesystem::exists(settings.directory + "Shaders\\"))
		std::filesystem::create_directory(settings.directory + "Shaders\\");
}
void Project::CreateProject() {
	if (!std::filesystem::exists(settings.directory + "Scripts\\"))
		std::filesystem::create_directory(settings.directory + "Scripts\\");
	settings.projectPath = settings.directory + "Scripts\\" + settings.name + ".csproj";
	settings.hasProject = true;
	Scripting::CreateVSProject(settings.projectPath);
}
std::string Project::GetAssemblyPath() {
	return settings.inEngine ? settings.directory + "Scripts\\bin\\Debug\\netcoreapp3.1\\" + settings.name + ".dll" : std::filesystem::path(settings.directory).parent_path().parent_path().string() + "\\" + settings.name + ".dll";
}
void Project::CreateNewShader(const std::string& name, const std::string& type, const std::string& vertexSource, const std::string& fragmentSource) {
	rapidjson::StringBuffer str;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> json(str);
	json.StartObject();
	json.Key("name");
	json.String(name.c_str());
	json.Key("type");
	json.String(type.c_str());
	json.Key("vertex");
	json.String(vertexSource.c_str());
	json.Key("fragment");
	json.String(fragmentSource.c_str());
	json.EndObject();
	FileExtensions::CreateAndWriteToFile(settings.directory + "Shaders\\" + name + ".ShbaShader", str.GetString());
	ReloadProject();
}
void Project::CreateNewScript(const std::string& name) {
	FileExtensions::CreateAndWriteToFile(settings.directory + "Scripts\\" + name + ".cs", R"C#(using ShibaEngineCore;
public class )C#" + name + R"C#( : Component
{
	public void Start()
	{
		ShibaEngineCore.Console.LogMessage("Hello World!");
	}

	public void Update()
	{

	}

}
)C#");
}
void Project::CreateNewBehaviour(const std::string& name, float interval) {
	std::stringstream intvStr;
	intvStr << interval;
	FileExtensions::CreateAndWriteToFile(settings.directory + "Scripts\\" + name + ".cs", R"C#(using ShibaEngineCore;

[Behaviour()C#" + intvStr.str() + R"C#(f)]
public static class )C#" + name + R"C#(
{
	public static void Update()
	{
		Console.LogMessage("Interval Called");
	}
}
)C#");
}
void Project::LoadProjectHierachy() {
	hierachy.item.name = "Assets";
	hierachy.item.path = settings.directory + "Assets\\";
	IterateDirectory(hierachy);
	hierachyMap[hierachy.item.path] = hierachy;
	LoadProjectScriptHierachy();
	LoadDefaultComponentHierachy();
	LoadShaderHierachy();
}
void Project::LoadProjectScriptHierachy() {
	scriptHierachy.item.name = "Scripts";
	for (auto& comp : Scripting::data.components) {
		CreateHierachyNode(comp.second.name, comp.second.nameSpace + "." + comp.second.name, "Script", scriptHierachy);
	}
	hierachyMap["Scripts"] = scriptHierachy;

}
void Project::LoadDefaultComponentHierachy() {
	componentHierachy.item.name = "Components";
	for (auto& comp : Engine::GetRegisteredComponents()) {
		CreateHierachyNode(comp, "Components\\" + comp, "Component", componentHierachy);
	}
	hierachyMap["Components"] = componentHierachy;
}
void Project::LoadShaderHierachy() {
	shaderHierachy.item.name = "Shaders";

	for (auto& shaderFile : std::filesystem::directory_iterator(settings.directory + "Shaders\\")) {
		if (std::filesystem::path(shaderFile).extension().string() == ".ShbaShader") {
			rapidjson::Document doc;
			doc.Parse(SerializationUtils::ReadFile(shaderFile.path().string()).c_str());
			if(ShaderManager::shaders.find(doc["name"].GetString()) == ShaderManager::shaders.end())
				ShaderManager::AddShader(doc["name"].GetString(), Shader(doc["vertex"].GetString(), doc["fragment"].GetString()), doc["type"].GetString());
		}
	}

	for (auto& shader : ShaderManager::shaders) {
		CreateHierachyNode(shader.first, settings.directory + "Shaders\\" + shader.first + ".ShbaShader", shader.second.type + " Shader", shaderHierachy);
	}
	hierachyMap["Shaders"] = shaderHierachy;
}
void Project::RenderHierachyNode(HierachyTreeNode& node) {

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
				else
					SceneManager::ChangeScene(node.item.path);
			}
			else {
				system(std::string("\"" + node.item.path + "\"").c_str());
			}
		}
		if (node.item.type != "Component" && node.item.type != "Script" && node.children.size() == 0) {
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::Selectable("Delete")) {
					removingItem = node.item;
				}
				ImGui::EndPopup();
			}
		}
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(node.item.type.c_str());
	}
}
void Project::RenderHierachy() {
	ImGui::Begin("Project", &isFrameOpen, ImGuiWindowFlags_NoCollapse);
	ImGui::TextUnformatted("Project Hierachy");
	if (ImGui::BeginTable("ProjectTable", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody, ImVec2(ImGui::GetWindowSize().x - 15, 420))) {
		float textWidth = ImGui::CalcTextSize("A").x;
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, textWidth * 12.0f);
		ImGui::TableHeadersRow();
		RenderHierachyNode(hierachy);
		RenderHierachyNode(scriptHierachy);
		RenderHierachyNode(componentHierachy);
		RenderHierachyNode(shaderHierachy);
		// ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, textWidth * 12.0f);
		// ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, textWidth * 18.0f);
		ImGui::EndTable();

		if (removingItem.path != "No Path") {
			// just need to remove the shader from the shader manager if the user is deleting a shader, we also need to check whether that shader is a custom one as we don't want to delete the default engine shaders
			if (removingItem.type.find("Shader") != std::string::npos && removingItem.name.find("ShibaEngine_") != std::string::npos)
				ShaderManager::shaders.erase(removingItem.name);

			std::cout << removingItem.path << std::endl;
			try {
				std::filesystem::remove(std::filesystem::path(removingItem.path));

			}
			catch (std::filesystem::filesystem_error ex) {
				std::cout << ex.code() << " | " << ex.what() << std::endl;
			}

			ReloadProject();
			removingItem = { "", "No Path", "" };
		}
	}

		
	ImGui::End();
}