#include "ProjectManager.h"
#include "SerializationUtils.h"
#include "Scripting.h"
#include <filesystem>
#include "SceneLoader.h"
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

Project ProjectManager::activeProject;
void ProjectManager::CreateNewProject(const std::string& path) {
	activeProject = Project(path);
	activeProject.LoadProjectHierachy();
}

void ProjectManager::LoadProject(const std::string& path) {
	ProjectSettings settings;
	rapidjson::Document doc;
	doc.Parse(SerializationUtils::ReadFile(path).c_str());
	settings.name = doc["Name"].GetString();
	settings.directory = doc["Directory"].GetString();
	settings.hasProject = doc["Has Project"].GetBool();
	if (settings.hasProject)
		settings.projectPath = doc["Project Path"].GetString();
	activeProject = Project(path);
	activeProject.settings = settings;
	activeProject.settings.hasAssembly = std::filesystem::exists(activeProject.GetAssemblyPath());
	if (settings.hasAssembly) {
		Scripting::ReloadAssembly(activeProject.GetAssemblyPath());
	}
	activeProject.LoadProjectHierachy();

	std::string lastScenePath = doc["Last Loaded Scene"].GetString();
	if (lastScenePath != "No Path") {
		SceneLoader::LoadScene(lastScenePath);
	}
}