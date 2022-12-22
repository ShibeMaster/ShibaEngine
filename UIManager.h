#pragma once
#include "SceneHierachyFrame.h"
#include "ViewportFrame.h"
#include "InspectorFrame.h"
class UIManager {
private:

	static std::string addingScriptName;
	static float addingBehaviourInterval;
	static bool isAddingBehaviour;
	static void RenderMenuBar();
	static void RenderAddingScriptPopup();
	static void RenderProjectSettings();
	static void StartDockspace();

public:
	static SceneHierachyFrame sceneFrame;
	static ViewportFrame viewportFrame;
	static InspectorFrame inspectorFrame;
	static glm::vec2 viewportSize;
	static void SaveScene();

	static void Initialize();
	static void Update();

};