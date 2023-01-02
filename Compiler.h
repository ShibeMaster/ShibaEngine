#pragma once
#include <filesystem>
#include "ProjectManager.h"
class Compiler {
private:
	void CreateDirectories() {
		std::filesystem::create_directory(directory);
		std::filesystem::create_directory(gameDataDirectory);
	}
	void MoveAssets() {
		
	}
public:
	std::string directory;
	std::string gameDataDirectory;
	Compiler(const std::string& dir){
		directory = dir + "\\" + ProjectManager::activeProject.settings.name + "\\";
		gameDataDirectory = directory + "GameData\\";
	}
	void Compile() {
		CreateDirectories();
		std::filesystem::copy(ProjectManager::activeProject.settings.directory + "Assets\\", gameDataDirectory + "Assets\\");
		Project project = ProjectManager::activeProject;
		project.settings.directory = gameDataDirectory;
		project.settings.inEngine = false;
		if (project.settings.startingScenePath == "None")
			project.settings.startingScenePath = "GameData\\" + std::filesystem::proximate(SceneManager::activeScene->path, ProjectManager::activeProject.settings.directory).string();
		project.SaveProject(gameDataDirectory + "Project.SHBAPROJ");
		std::filesystem::copy(ProjectManager::activeProject.GetAssemblyPath(), directory + ProjectManager::activeProject.settings.name + ".dll");
		std::filesystem::copy(std::filesystem::path(ProjectManager::activeProject.GetAssemblyPath()).parent_path().string() + "\\" + "ShibaEngineCore.dll", directory + "ShibaEngineCore.dll");
		// std::filesystem::copy("path of other installed project", directory);
	}

};