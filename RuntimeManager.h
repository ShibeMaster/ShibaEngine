#pragma once
#include <filesystem>
#include "Scripting.h"
#include "Console.h"

class RuntimeManager {
private:
	static void SaveScene(const std::string& specificPath = "") {
		std::cout << "Saving Scene" << std::endl;
		std::string path = SceneManager::activeScene->path == "No Path" ? specificPath : SceneManager::activeScene->path;
		std::cout << path << std::endl;
		if (SceneManager::activeScene->path == "No Path")
			ProjectManager::activeProject.CreateNewSceneNode(SceneManager::activeScene->name, path);

		scenePath = path;
		SceneLoader::SaveScene(*SceneManager::activeScene, path);
	}
	static inline std::string scenePath;
public:
	static inline bool inRuntime = false;
	static inline float runtimeStartTime = 0.0f;
	static inline bool isPaused = false;

	static void StartRuntime() {

		std::string path;
		if (SceneManager::activeScene->path == "No Path") {
			if (FileExtensions::SaveFileAsDialog(ProjectManager::activeProject.settings.directory, "ShbaScene", &path)) {
				SceneManager::activeScene->name = std::filesystem::path(path).stem().string();
				SaveScene(path);
			}
		}
		else
			SaveScene();

		Console::LogMessage("Runtime Started");
		Scripting::OnRuntimeStart();
		inRuntime = true;
		isPaused = false;
		runtimeStartTime = glfwGetTime();
	}
	static void EndRuntime() {
		Console::LogMessage("Runtime Ended");
		SceneLoader::LoadScene(scenePath);
		inRuntime = false;
		isPaused = false;
		runtimeStartTime = 0.0f;
	}
	static void TogglePause() {
		isPaused = !isPaused;
	}
};