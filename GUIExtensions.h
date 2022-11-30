#pragma once
#include "ProjectItem.h"
#include <string>
#include "ProjectManager.h"
#include "imgui.h"

class GUIExtensions {
public:
	template<typename T>
	static bool CreateDragDropTarget(const std::string& targetType, T* outValue) {

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::string(std::string("DRAG_DROP_") + targetType).c_str())) {
				*outValue = *(T*)payload->Data;
				return true;
			}
		}
		return false;
	}
	static bool CreateProjectItemDropField(std::vector<std::string> fileTypes, ProjectItem* outItem) {

		if (ImGui::BeginDragDropTarget()) {
			for (auto& type : fileTypes) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::string("DRAG_DROP_" + type).c_str())) {
					auto script = ProjectManager::activeProject.GetItem((const char*)payload->Data);
					*outItem = script;
					return true;
				}
			}
		}
		return false;
	}

};