#pragma once
#include "Project.h"
#include <string>
class ProjectManager {
public:
	static Project activeProject;
	static void CreateNewProject(const std::string& path);
	static void LoadProject(const std::string& path);
};