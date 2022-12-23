#pragma once
#include "EditorFrame.h"
#include "ViewManager.h"
class ViewportFrame : public EditorFrame {
public:
	bool sceneViewFrameOpen = true;
	bool gameViewFrameOpen;
	void Render() {
		if (sceneViewFrameOpen) {

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Scene View", &sceneViewFrameOpen, ImGuiWindowFlags_NoDecoration);


			ImVec2 dimensions{ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
			dimensions.y = (dimensions.x / 16) * 9;
			if (dimensions.y > ImGui::GetWindowHeight()) {
				dimensions.x = (ImGui::GetWindowHeight() / 9) * 16;
				dimensions.y = ImGui::GetWindowHeight();
			}

			ImGui::SetCursorPos({ (ImGui::GetWindowWidth() - dimensions.x) * 0.5f, (ImGui::GetWindowHeight() - dimensions.y) * 0.5f }) ;
			ImGui::Image((ImTextureID)ViewManager::sceneView.view.framebuffer.GetTexture(), dimensions, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
		if (gameViewFrameOpen) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Game View", &gameViewFrameOpen, ImGuiWindowFlags_NoDecoration);

			ImVec2 dimensions{ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
			dimensions.y = (dimensions.x / 16) * 9;
			if (dimensions.y > ImGui::GetWindowHeight()) {
				dimensions.x = (ImGui::GetWindowHeight() / 9) * 16;
				dimensions.y = ImGui::GetWindowHeight();
			}

			ImGui::SetCursorPos({ (ImGui::GetWindowWidth() - dimensions.x) * 0.5f, (ImGui::GetWindowHeight() - dimensions.y) * 0.5f });
			ImGui::Image((ImTextureID)ViewManager::gameView.view.framebuffer.GetTexture(), dimensions, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}
};