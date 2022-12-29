#pragma once
#include "EditorFrame.h"
#include "imgui_stdlib.h"
#include "Engine.h"
#include "Transform.h"
#include "GUIExtensions.h"
#include "Scripting.h"
#include "Console.h"
#include "SceneLoader.h"
#include "FileExtensions.h"
#include "SceneManager.h"
#include "imgui.h"
#include "Engine.h"
#include "Scripting.h"
#include "Primitives.h"
#include <GLFW/glfw3.h>
#include "ProjectManager.h"
#include "imgui_stdlib.h"
#include "RuntimeManager.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "ViewManager.h"
#include "ProjectItem.h"

class InspectorFrame : public EditorFrame {
public:
	void Render() {}
	void RenderEntityScripts(unsigned int selectedEntity) {
		std::vector<std::string> removingScripts;
		for (auto script : Scripting::GetEntityComponentInstances(selectedEntity)) {
			bool scriptExists = true;
			if (ImGui::CollapsingHeader(script.first.c_str(), &scriptExists)) {

				for (auto field : script.second.classData.fields) {
					GUIExtensions::RenderField(field.second, script.second);
				}
			}
			if (!scriptExists)
				removingScripts.push_back(script.first);
		}
		for (auto script : removingScripts) {
			Engine::RemoveScript(selectedEntity, script);
			Scripting::RemoveScriptInstance(selectedEntity, script);
		}
	}
	void RenderEntityAddingComponent(unsigned int selectedEntity) {
		ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - 30.0f, 30.0f));
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Script")) {
				auto script = ProjectManager::activeProject.GetItem((const char*)payload->Data).name;
				Engine::AddScript(selectedEntity, script);
				Scripting::LoadEntityScript(selectedEntity, script);
				if (RuntimeManager::inRuntime)
					Scripting::data.entities[selectedEntity].scripts[script].InvokeMethod(Scripting::data.entities[selectedEntity].scripts[script].startMethod, 0, nullptr);
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Component")) {
				auto name = ProjectManager::activeProject.GetItem((const char*)payload->Data).name;
				Engine::AddComponent(selectedEntity, name);
				Scripting::OnAddComponent(selectedEntity, name);
			}
		}
	}
	void RenderEntity(unsigned int selectedEntity) {
		ImGui::Begin("Components", &isOpen, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoCollapse);

		if (selectedEntity != -1) {
			ImGui::InputText("###Entity_Name", &SceneManager::activeScene->items[selectedEntity].name);
			ImGui::Separator();
			if (Engine::HasComponent<Transform>(selectedEntity)) {
				if (ImGui::CollapsingHeader("Transform")) {

					auto transform = Engine::GetComponentPointer<Transform>(selectedEntity);
					ImGui::InputFloat3("position", &transform->position[0]);
					ImGui::InputFloat3("rotation", &transform->rotation[0]);
					ImGui::InputFloat3("pivot", &transform->pivot[0]);
					ImGui::InputFloat3("scale", &transform->scale[0]);
				}
			}

			Engine::DrawEntityComponentGUI(selectedEntity);
			RenderEntityScripts(selectedEntity);
			RenderEntityAddingComponent(selectedEntity);
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Entity Selected");
		}
		ImGui::End();
	}
};