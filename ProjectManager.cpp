#include "ProjectManager.h"
#include "SerializationUtils.h"
#include "Scripting.h"
#include <filesystem>
#include "SceneLoader.h"
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "SceneManager.h"

Project ProjectManager::activeProject;
bool ProjectManager::projectLoaded = false;
void ProjectManager::CreateNewProject(const std::string& path) {
	activeProject = Project(path);
	activeProject.LoadProjectHierachy();
}
void ProjectManager::CreateNewProject(const std::string& name, const std::string& path) {
	activeProject = Project(path, name);
	activeProject.LoadProjectHierachy();
	activeProject.SaveProject();
	projectLoaded = true;

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
	activeProject = Project(settings.directory);
	activeProject.settings = settings;
	activeProject.settings.hasAssembly = std::filesystem::exists(activeProject.GetAssemblyPath());
	if (activeProject.settings.hasAssembly) {
		Scripting::ReloadAssembly(activeProject.GetAssemblyPath());
	}
	activeProject.LoadProjectHierachy();
	projectLoaded = true;
}