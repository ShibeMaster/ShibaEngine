#pragma once
#include <string>
class ProjectSettings {
public:
	std::string directory;
	std::string name;
	bool hasProject = false;
	std::string projectPath;
	bool hasAssembly = false;
	bool inEngine = true;
	std::string startingScenePath = "None";
};