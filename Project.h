#pragma once
#include <string>
#include "ProjectItem.h"
#include "ProjectSettings.h"
#include <map>
#include <vector>

struct HierachyTreeNode {
	ProjectItem item;
	std::vector<HierachyTreeNode*> children;
};

class Project {
private:
	HierachyTreeNode hierachy;
	HierachyTreeNode scriptHierachy;
	HierachyTreeNode componentHierachy;
	HierachyTreeNode shaderHierachy;
	ProjectItem removingItem = { "", "No Path", "" };
	void LoadProjectScriptHierachy();
	void LoadDefaultComponentHierachy();
	void LoadShaderHierachy();
	void IterateDirectory(HierachyTreeNode& directoryNode);
	void RenderHierachyNode(HierachyTreeNode& node);
public:
	ProjectSettings settings;

	bool isFrameOpen = true;
	std::map<std::string, HierachyTreeNode> hierachyMap;

	Project(){}
	Project(const std::string& path) {
		settings.directory = path;
	}
	Project(const std::string& path, const std::string& name) {
		settings.directory = path;
		settings.name = name;
	}
	ProjectItem& GetItem(const std::string& path);
	void ReloadProject();
	HierachyTreeNode& CreateHierachyNode(const std::string& name, const std::string& path, const std::string& type, HierachyTreeNode& parentNode, bool isDirectory = false);
	void CreateNewSceneNode(const std::string& name, const std::string& path);
	void SaveProject(const std::string& specificPath = "None");
	void CreateProject();
	std::string GetAssemblyPath();
	void InitializeDirectories();
	void CreateNewShader(const std::string& name, const std::string& type, const std::string& vertexSource, const std::string& fragmentSource);
	void CreateNewScript(const std::string& name);
	void CreateNewBehaviour(const std::string& name, float interval);
	void LoadProjectHierachy();
	void RenderHierachy();
};