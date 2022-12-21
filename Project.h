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
	void LoadProjectScriptHierachy();
	void LoadDefaultComponentHierachy();
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
	ProjectItem& GetItem(const std::string& path);
	void ReloadProject();
	HierachyTreeNode& CreateHierachyNode(const std::string& name, const std::string& path, const std::string& type, HierachyTreeNode& parentNode, bool isDirectory = false);
	void CreateNewSceneNode(const std::string& name, const std::string& path);
	void SaveProject();
	void CreateProject();
	std::string GetAssemblyPath();
	void CreateNewScript(const std::string& name);
	void CreateNewBehaviour(const std::string& name, float interval);
	void LoadProjectHierachy();
	void RenderHierachy();
};