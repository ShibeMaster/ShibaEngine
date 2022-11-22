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
	static bool CreateProjectItemDropField(const std::string& fileType, ProjectItem* outItem) {

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::string("DRAG_DROP_" + fileType).c_str())) {
				auto script = ProjectManager::activeProject.GetItem((const char*)payload->Data);
				*outItem = script;
				return true;
			}
		}
		return false;
	}

};