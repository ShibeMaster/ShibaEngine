#pragma once
#include <imgui.h>
#include "SceneManager.h"
#include "Engine.h"
#include "Transform.h"
#include "Scripting.h"
#include "Primitives.h"
#include <GLFW/glfw3.h>
#include "ProjectManager.h"
#include "ViewManager.h"
#include "ProjectItem.h"
#include "imgui.h"
#include <Windows.h>
#include "imgui_stdlib.h"
#include "ShaderManager.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "FileExtensions.h"
class UIManager {
public:

	static int selectedEntity;
	static bool sceneViewActive;
	static bool sceneViewFrameOpen;
	static bool gameViewFrameOpen;
	static glm::vec2 viewportSize;
	static void RenderMenuBar() {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Save", "CTRL+S")) {
					SaveScene();
					ProjectManager::activeProject.SaveProject();
				}
				if (ImGui::MenuItem("Open", "CTRL+O")) {
					std::string path;
					if (FileExtensions::OpenFileDialog("C:\\", ".SHBAPROJ", &path)) {
						ProjectManager::LoadProject(path);
					}
				}
				if (ImGui::BeginMenu("New", "CTRL+N")) {
					if (ImGui::BeginMenu("Entity")) {
						if (ImGui::MenuItem("Empty"))
							CreateEntity();
						if (ImGui::BeginMenu("Primitives")) {
							Primitives::RenderPrimitiveSelection();
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scripts")) {
				if (ImGui::MenuItem("Reload Assembly"))
					Scripting::ReloadAssembly(ProjectManager::activeProject.settings.assembly);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Project")) {
				if (ImGui::MenuItem("Reload Project")) {
					ProjectManager::activeProject.ReloadProject();
				}
				if (ImGui::BeginMenu("Settings")) {
					RenderProjectSettings();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window")) {
				ImGui::MenuItem("Scene", "", &sceneViewFrameOpen);
				ImGui::MenuItem("Game", "", &gameViewFrameOpen);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	static void RenderProjectSettings() {
		ImGui::BeginChild("Project Settings", ImVec2(180, 60), true);
		ImGui::InputText("Assembly Path", &ProjectManager::activeProject.settings.assembly);
		ImGui::InputText("Directory", &ProjectManager::activeProject.settings.directory);
		ImGui::EndChild();
	}

	static void CreateEntityDragAndDrop(SceneItem* item) {
		unsigned int entity;
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			selectedEntity = item->entity;
		}
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("DRAG_DROP_Entity", &item->entity, sizeof(unsigned int));
			ImGui::Text(item->name.c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Entity")) {
				entity = *(unsigned int*)payload->Data;
				SceneManager::activeScene->MoveEntityToChild(entity, item->entity);
			}
			ImGui::EndDragDropTarget();
		}
	}
	static unsigned int CreateEntity() {

		unsigned int newEnt = Engine::CreateEntity();
		Engine::AddComponent<Transform>(newEnt, Transform{});
		Scripting::OnEntityCreated(newEnt);
		SceneManager::activeScene->OnCreateEntity(newEnt);
		return newEnt;
	}
#pragma region Scene
	static void RenderSceneItemUI(SceneItem* item) {

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		unsigned int entity;
		if (item->children.size() > 0) {
			bool open = ImGui::TreeNodeEx(item->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanFullWidth);
			CreateEntityDragAndDrop(item);

			for (auto& child : item->children)
				RenderSceneItemUI(child);
			ImGui::TreePop();
		}
		else {
			ImGui::TreeNodeEx(item->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
			CreateEntityDragAndDrop(item);
		}

	}
	static void SaveScene() {
		std::cout << "Saving Scene" << std::endl;
		std::string path = SceneManager::activeScene->path == "No Path" ? ProjectManager::activeProject.settings.directory + SceneManager::activeScene->name + ".ShbaScene" : SceneManager::activeScene->path;
		std::cout << path << std::endl;
		if (SceneManager::activeScene->path == "No Path")
			ProjectManager::activeProject.CreateNewSceneNode(SceneManager::activeScene->name, path);

		SceneLoader::SaveScene(*SceneManager::activeScene, path);
	}
	static void RenderSceneHierachyUI() {
		if (ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoCollapse)) {
			ImGui::TextUnformatted("Scene");
			//ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			if (ImGui::BeginTable("EntitiesTable", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody, ImVec2(ImGui::GetWindowSize().x - 15, -30))) {
				float textWidth = ImGui::CalcTextSize("A").x;
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
				ImGui::TableHeadersRow();
				for (auto& item : SceneManager::activeScene->hierachy) {
					RenderSceneItemUI(item);
				}
				ImGui::EndTable();
			}
			if (ImGui::Button("New Entity")) {
				CreateEntity();
			}
			ImGui::End();
		}
	}
#pragma endregion
#pragma region Viewports
	static void RenderViewportUI() {
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
#pragma endregion
#pragma region Toolbar

#pragma endregion

#pragma region Inspector
	static void RenderInspectorUI() {
		ImGui::Begin("Components", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoCollapse);

		if (selectedEntity != -1) {
			ImGui::InputText("###Entity_Name", &SceneManager::activeScene->items[selectedEntity].name);
			ImGui::Separator();
			if (Engine::HasComponent<Transform>(selectedEntity)) {
				if (ImGui::CollapsingHeader("Transform")) {

					auto transform = &Engine::GetComponent<Transform>(selectedEntity);
					ImGui::InputFloat3("position", &transform->position[0]);
					ImGui::InputFloat3("rotation", &transform->rotation[0]);
					ImGui::InputFloat3("pivot", &transform->pivot[0]);
					ImGui::InputFloat3("scale", &transform->scale[0]);
				}
			}

			Engine::DrawEntityComponentGUI(selectedEntity);
			RenderEntityScriptUI();
			RenderAddingComponentUI();
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Entity Selected");
		}
		ImGui::End();

	}
	static void RenderAddingComponentUI() {
		ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - 30.0f, 30.0f));
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Script")) {
				auto script = ProjectManager::activeProject.GetItem((const char*)payload->Data).name;
				std::cout << "adding script " << script << std::endl;
				Engine::AddScript(selectedEntity, script);
				Scripting::LoadEntityScript(selectedEntity, script);
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Component")) {
				auto name = ProjectManager::activeProject.GetItem((const char*)payload->Data).name;
				Engine::AddComponent(selectedEntity, name);
				std::cout << name << std::endl;
				Scripting::OnAddComponent(selectedEntity, name);
			}
		}
	}
	static void RenderEntityScriptUI() {
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
#pragma region Dockspace
	static void StartDockspace() {
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Dockspace", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);
		ImGui::PopStyleVar(3);
		ImGuiID id = ImGui::GetID("Dockspace");
		ImGui::DockSpace(id, ImVec2(0.0f, 0.0f));
		ImGui::End();

	}
#pragma endregion

#pragma endregion
	static void Initialize() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");


		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_FrameBg] = ImVec4(0.28f, 0.28f, 0.28f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.33f, 0.33f, 0.33f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.28f, 0.28f, 0.28f, 0.80f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.80f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.33f, 0.33f, 0.33f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.33f, 0.33f, 0.33f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.31f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 0.31f);
	}
	static void Update() {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();

		StartDockspace();

		RenderSceneHierachyUI();
		ProjectManager::activeProject.RenderHierachy();
		RenderMenuBar();
		Console::Render();
		RenderInspectorUI();
		RenderViewportUI();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}

};