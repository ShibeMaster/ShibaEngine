#pragma once
#include "EditorFrame.h"
#include "RuntimeManager.h"

class ManagementFrame : public EditorFrame {
public:
	void Render() {
		if (ImGui::Begin("Management", &isOpen, ImGuiWindowFlags_NoDecoration)) {
			if (!RuntimeManager::inRuntime) {
				if (ImGui::Button("Play"))
					RuntimeManager::StartRuntime();
			}
			else {
				if (ImGui::Button("Stop"))
					RuntimeManager::EndRuntime();
				ImGui::SameLine();
				if (ImGui::Button(RuntimeManager::isPaused ? "Unpause" : "Pause"))
					RuntimeManager::TogglePause();
			}
			ImGui::End();
		}
	}
};