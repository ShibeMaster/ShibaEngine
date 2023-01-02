#pragma once
#include "SceneHierachyFrame.h"
#include "ViewportFrame.h"
#include "InspectorFrame.h"
#include "ManagementFrame.h"
class UIManager {
private:

	static std::string addingScriptName;
	static float addingBehaviourInterval;
	static bool isAddingBehaviour;

	static std::string creatingProjectName;
	static std::string creatingProjectDirectory;

	static std::string addingShaderName;
	static std::string addingShaderType;
	static std::string addingShaderVertexDirectory;
	static std::string addingShaderFragmentDirectory;
	static std::string compilingProjectDirectory;

	static void RenderMenuBar();
	static void RenderCreatingProjectPopup();
	static void RenderAddingShaderPopup();
	static void RenderAddingScriptPopup();
	static void RenderCompilingProjectPopup();
	static void RenderProjectSettings();
	static void StartDockspace();

public:
	static SceneHierachyFrame sceneFrame;
	static ViewportFrame viewportFrame;
	static InspectorFrame inspectorFrame;
	static ManagementFrame managementFrame;
	static glm::vec2 viewportSize;
	static void SaveScene(const std::string& specificPath = "");

	static void Initialize();
	static void Update();

};