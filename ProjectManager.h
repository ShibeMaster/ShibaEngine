#pragma once
#include "Project.h"
#include <string>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

class ProjectManager {
public:
	static Project activeProject;
	static void CreateNewProject(const std::string& path) {
		activeProject = Project(path);
		activeProject.LoadProjectHierachy();
	}

	static void LoadProject(const std::string& path) {
		ProjectSettings settings;
		rapidjson::Document doc;
		doc.Parse(SerializationUtils::ReadFile(path).c_str());
		settings.name = doc["Name"].GetString();
		settings.assembly = doc["Assembly Path"].GetString();
		settings.directory = doc["Directory"].GetString();
		activeProject = Project(path);
		activeProject.settings = settings;
		Scripting::ReloadAssembly(settings.assembly);
		activeProject.LoadProjectHierachy();

		std::string lastScenePath = doc["Last Loaded Scene"].GetString();
		if (lastScenePath != "No Path") {
			SceneLoader::LoadScene(lastScenePath);
		}
	}
};