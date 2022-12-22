#pragma once
#include "EditorFrame.h"
#include "ViewManager.h"
class ViewportFrame : public EditorFrame {
public:
	bool sceneViewFrameOpen;
	bool gameViewFrameOpen;
	void Render() {
		if (sceneViewFrameOpen) {

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Scene View", &sceneViewFrameOpen);

			ImGui::Image((ImTextureID)ViewManager::sceneView.view.framebuffer.GetTexture(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
		if (gameViewFrameOpen) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Game View", &gameViewFrameOpen);

			ImGui::Image((ImTextureID)ViewManager::gameView.view.framebuffer.GetTexture(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}
};