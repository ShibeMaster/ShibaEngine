#pragma once
#include "Project.h"
#include <string>
class ProjectManager {
public:
	static Project activeProject;
	static void CreateNewProject(const std::string& path) {
		activeProject = Project(path);
		activeProject.LoadProjectHierachy();
	}
};