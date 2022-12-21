#include "UIManager.h"
#include <filesystem>

std::string UIManager::addingScriptName = "NewScript";
float UIManager::addingBehaviourInterval = 0.0f;
bool UIManager::isAddingBehaviour = false;
glm::vec2 UIManager::viewportSize;

SceneHierachyFrame UIManager::sceneFrame;
ViewportFrame UIManager::viewportFrame;
InspectorFrame UIManager::inspectorFrame;

void UIManager::RenderMenuBar() {
	bool addingScriptPopup = false;
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
						sceneFrame.CreateEntity();
					if (ImGui::BeginMenu("Primitives")) {
						Primitives::RenderPrimitiveSelection();
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Script")) {
					if (!ProjectManager::activeProject.settings.hasProject)
						ProjectManager::activeProject.CreateProject();
					addingScriptPopup = true;
					isAddingBehaviour = false;
				}
				if (ImGui::MenuItem("Behaviour")) {
					if (!ProjectManager::activeProject.settings.hasProject)
						ProjectManager::activeProject.CreateProject();
					addingScriptPopup = true;
					isAddingBehaviour = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Reload", "F5")) {
				ProjectManager::activeProject.settings.hasAssembly = std::filesystem::exists(ProjectManager::activeProject.GetAssemblyPath());
				if (ProjectManager::activeProject.settings.hasAssembly)
					Scripting::ReloadAssembly(ProjectManager::activeProject.GetAssemblyPath());

				ProjectManager::activeProject.ReloadProject();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scripts")) {
			if (ImGui::MenuItem("Reload Assembly")) {
				ProjectManager::activeProject.settings.hasAssembly = std::filesystem::exists(ProjectManager::activeProject.GetAssemblyPath());
				if (ProjectManager::activeProject.settings.hasAssembly)
					Scripting::ReloadAssembly(ProjectManager::activeProject.GetAssemblyPath());

			}
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
			ImGui::MenuItem("Scene", "", &viewportFrame.sceneViewFrameOpen);
			ImGui::MenuItem("Game", "", &viewportFrame.gameViewFrameOpen);
			ImGui::MenuItem("Inspector", "", &inspectorFrame.isOpen);
			ImGui::MenuItem("Console", "", &Console::isOpen);
			ImGui::MenuItem("Project", "", &ProjectManager::activeProject.isFrameOpen);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	if (addingScriptPopup)
		ImGui::OpenPopup("Adding Script");
	RenderAddingScriptPopup();


}
void UIManager::RenderAddingScriptPopup() {

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Adding Script", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::InputText("Name", &addingScriptName);
		if (isAddingBehaviour)
			ImGui::InputFloat("Interval", &addingBehaviourInterval);
		if (ImGui::Button("Add")) {
			if (!isAddingBehaviour)
				ProjectManager::activeProject.CreateNewScript(addingScriptName);
			else
				ProjectManager::activeProject.CreateNewBehaviour(addingScriptName, addingBehaviourInterval);
			addingScriptName = "NewScript";
			addingBehaviourInterval = 0.0f;
			ProjectManager::activeProject.settings.hasAssembly = std::filesystem::exists(ProjectManager::activeProject.GetAssemblyPath());
			if (ProjectManager::activeProject.settings.hasAssembly)
				Scripting::ReloadAssembly(ProjectManager::activeProject.GetAssemblyPath());
			ProjectManager::activeProject.ReloadProject();

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
void UIManager::RenderProjectSettings() {
	ImGui::BeginChild("Project Settings", ImVec2(360, 60), true);
	ImGui::InputText("Name", &ProjectManager::activeProject.settings.name);
	ImGui::InputText("Directory", &ProjectManager::activeProject.settings.directory);
	ImGui::EndChild();
}
#pragma region Scene
void UIManager::SaveScene() {
	std::cout << "Saving Scene" << std::endl;
	std::string path = SceneManager::activeScene->path == "No Path" ? ProjectManager::activeProject.settings.directory + SceneManager::activeScene->name + ".ShbaScene" : SceneManager::activeScene->path;
	std::cout << path << std::endl;
	if (SceneManager::activeScene->path == "No Path")
		ProjectManager::activeProject.CreateNewSceneNode(SceneManager::activeScene->name, path);

	SceneLoader::SaveScene(*SceneManager::activeScene, path);
}
void UIManager::StartDockspace() {
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
void UIManager::Initialize() {
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
void UIManager::Update() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();

	StartDockspace();

	ProjectManager::activeProject.RenderHierachy();
	RenderMenuBar();
	Console::Render();
	inspectorFrame.RenderEntity(sceneFrame.selectedEntity);
	sceneFrame.Render();
	viewportFrame.Render();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}