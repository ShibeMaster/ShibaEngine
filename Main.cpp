#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include "imgui.h"
#include "Shaders.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "InputManager.h"
#include "Scene.h"
#include <string>
#include "ProjectManager.h"
#include "Engine.h"
#include "imgui_stdlib.h"
#include "Physics.h"
#include "BoundingBox.h"
#include "Raycast.h"
#include "GameView.h"
#include "SceneView.h"
#include "Console.h"
#include "Scripting.h"
#include "Camera.h"
#include <Mono/jit/jit.h>
#include "GUIExtensions.h"
#include "Project.h"
#include <Mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include "Mesh.h"
#include "Display.h"
#include "View.h"
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include "Time.h"
#include "Collisions.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <glm/ext/matrix_transform.hpp>
#include "Light.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "MeshCollisionBox.h"
#include "SpriteRenderer.h"

GLFWwindow* window;

std::vector<Vertex> vertices = { Vertex{ glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) } 
};

const char* defaultVertexSource = R"GLSL(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;


out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool billboard;

void main(){
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
	if(billboard){
		mat4 modelView = view * model;
		modelView[0][0] = 1.0;
		modelView[0][1] = 0.0;
		modelView[0][2] = 0.0;

		modelView[1][0] = 0.0;
		modelView[1][1] = 1.0;
		modelView[1][2] = 0.0;
		
		modelView[2][0] = 0.0;
		modelView[2][1] = 0.0;
		modelView[2][2] = 1.0;

		gl_Position = projection * modelView * vec4(position, 1.0);

	} else{
		gl_Position = projection * view * model * vec4(position, 1.0);
	}
}

)GLSL";

const char* defaultFragmentSource = R"GLSL(
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;


uniform bool hasTexture;
uniform sampler2D texture_diffuse1;

uniform vec3 lightColour;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform bool lightEffected;


void main(){
	vec4 colour = hasTexture ? texture(texture_diffuse1, TexCoords) : vec4(1.0, 1.0, 1.0, 1.0);

	if(colour.a < 0.1)
		discard;
	if(lightEffected){


		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColour;

		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColour;

		vec3 result = (ambient + diffuse) * vec3(colour);
		FragColor = vec4(result, 1.0);
	} else {
		FragColor = colour;
	}


}
)GLSL";
char entryPoint[128];
bool inRuntime = false;
float runtimeStartTime = 0.0f;
SceneView sceneView;
GameView gameView;
bool sceneViewActive = true;
int selectedEntity = -1;
int clipboardEntity = -1;

void ProcessInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
		if (key == GLFW_KEY_TAB) {
			if (InputManager::mouse.inputLocked)
				InputManager::SetMouseUnlocked();
			else
				InputManager::SetMouseLocked();
		}
		if (key == GLFW_KEY_F1) {
			std::vector<RayHit> outHits;
			std::cout << Raycast(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 10.0f, &outHits) << std::endl;
		}
		if (key == GLFW_KEY_F4) {
			std::vector<RayHit> outHits;
			std::cout << Raycast(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 5.0f, &outHits) << std::endl;;
		}
		if (key == GLFW_KEY_F3) {
			Scripting::OnRuntimeStart();
			inRuntime = true;
			Console::LogMessage("Runtime Started");
			runtimeStartTime = glfwGetTime();
		}

		if (key == GLFW_KEY_D && mods == GLFW_MOD_CONTROL && selectedEntity > -1 ) {
			unsigned int newEntity = Engine::CreateEntity();
			for (auto& script : Engine::GetEntityScripts(selectedEntity)) {
				Engine::AddScript(newEntity, script);
				Scripting::LoadEntityScript(newEntity, script);
			}
			for (auto& comp : Engine::GetEntityComponents(selectedEntity)) {
				Engine::AddComponent(newEntity, comp);
			}
		}
		if (key == GLFW_KEY_DELETE && selectedEntity > -1) {
			Engine::DestroyEntity(selectedEntity);
			Scripting::OnEntityDestroyed(selectedEntity);
			selectedEntity = -1;
		}
	}
}

void HandleMouseInput(GLFWwindow* window, double xpos, double ypos) {
	InputManager::mouse.MouseCallback(window, xpos, ypos);
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);		
	if (sceneViewActive)
		sceneView.sceneCam.ProcessCameraMouse();
	else if (inRuntime)
		gameView.view.camera->ProcessCameraMouse();
}

void RenderField(Field field, ClassInstance& instance) {
	// im sorrry for making this
	float floatVal;
	double doubleVal;
	bool boolVal;
	unsigned int entityValue;
	std::string stringVal;
	glm::vec2 vec2Val;
	glm::vec3 vec3Val;
	int intValue;
	switch (field.type) {
	case FieldType::Float:
		floatVal = instance.GetFieldValue<float>(field.name);
		ImGui::InputFloat(field.name.c_str(), &floatVal);
		if (floatVal != instance.GetFieldValue<float>(field.name))
			instance.SetFieldValue<float>(field.name, floatVal);
		break;
	case FieldType::Double:
		doubleVal = instance.GetFieldValue<double>(field.name);
		ImGui::InputDouble(field.name.c_str(), &doubleVal);
		if (doubleVal != instance.GetFieldValue<double>(field.name))
			instance.SetFieldValue<double>(field.name, doubleVal);
		break;
	case FieldType::Bool:
		boolVal = instance.GetFieldValue<bool>(field.name);
		ImGui::Checkbox(field.name.c_str(), &boolVal);
		if (boolVal != instance.GetFieldValue<bool>(field.name))
			instance.SetFieldValue<bool>(field.name, boolVal);
		break;
	case FieldType::UInt:
		entityValue = instance.GetFieldValue<unsigned int>(field.name);
		ImGui::Button(std::string(field.name + ": " + std::to_string(entityValue)).c_str());
		if (GUIExtensions::CreateDragDropTarget<unsigned int>("Entity", &entityValue)) {
			instance.SetFieldValue<unsigned int>(field.name, entityValue);
		}
		break;
	case FieldType::Vector2:
		vec2Val = instance.GetFieldValue<glm::vec2>(field.name);
		ImGui::InputFloat2(field.name.c_str(), &vec2Val[0]);
		if (vec2Val != instance.GetFieldValue<glm::vec2>(field.name))
			instance.SetFieldValue<glm::vec2>(field.name, vec2Val);
		break;
	case FieldType::Vector3:
		vec3Val = instance.GetFieldValue<glm::vec3>(field.name);
		ImGui::InputFloat3(field.name.c_str(), &vec3Val[0]);
		if (vec3Val != instance.GetFieldValue<glm::vec3>(field.name))
			instance.SetFieldValue<glm::vec3>(field.name, vec3Val);
		break;
	case FieldType::Int:
		intValue = instance.GetFieldValue<int>(field.name);
		ImGui::InputInt(field.name.c_str(), &intValue);
		if (intValue != instance.GetFieldValue<int>(field.name.c_str()))
			instance.SetFieldValue<int>(field.name, intValue);
		break;

	}
}
void CreateEntityDragAndDrop(SceneItem* item) {
	unsigned int entity;
	if (ImGui::IsItemClicked()) {
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
			SceneManager::activeScene.MoveEntityToChild(entity, item->entity);
		}
		ImGui::EndDragDropTarget();
	}
}
void RenderSceneItem(SceneItem* item) {

	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	unsigned int entity;
	if (item->children.size() > 0) {
		bool open = ImGui::TreeNodeEx(item->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanFullWidth);
		CreateEntityDragAndDrop(item);

		for (auto& child : item->children)
			RenderSceneItem(child);
		ImGui::TreePop();
	}
	else {
		ImGui::TreeNodeEx(item->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
		CreateEntityDragAndDrop(item);
	}

}
void SetupDefaultScene() {

	unsigned int light = Engine::CreateEntity();
	Engine::AddComponent<Transform>(light, Transform{});
	Scripting::OnEntityCreated(light);
	SceneManager::activeScene.OnCreateEntity(light);
	SceneManager::activeScene.items[light].name = "Light";
	Engine::AddComponent<Light>(light, Light());
}
void RenderSceneHierachy() {
	if (ImGui::BeginChild("Entities", ImVec2(0.0f, -450.0f), true)) {
		ImGui::TextUnformatted("Scene");
		//ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (ImGui::BeginTable("Entities", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody, ImVec2(ImGui::GetWindowSize().x - 15, -30))) {
			float textWidth = ImGui::CalcTextSize("A").x;
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
			ImGui::TableHeadersRow();
			for (auto& item : SceneManager::activeScene.hierachy) {
				RenderSceneItem(item);
			}
			ImGui::EndTable();
		}
		if (ImGui::Button("New Entity")) {
			unsigned int newEnt = Engine::CreateEntity();
			Engine::AddComponent<Transform>(newEnt, Transform{});
			Scripting::OnEntityCreated(newEnt);
			SceneManager::activeScene.OnCreateEntity(newEnt);
		}
		//if (ImGui::BeginListBox("###Entities", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y * 0.65f))) {
		//	for (unsigned int entity : SceneManager::activeScene.entities) {
		//		if (ImGui::Selectable(std::to_string(entity).c_str(), ImGuiSelectableFlags_SpanAllColumns)) {
		//			selectedEntity = entity;
		//		}
		//		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		//			ImGui::SetDragDropPayload("DRAG_DROP_Entity", &entity, sizeof(unsigned int));
		//			ImGui::EndDragDropSource();
		//		}
		//	}
		//	ImGui::EndListBox();


		//}
		//ImGui::End();
		ImGui::EndChild();
	}
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = Display::CreateWindow();

	glewInit();
	glfwSwapInterval(1);

	mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", "C:\\Program Files (x86)\\Mono\\etc");

	Scripting::Initialize();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	Engine::RegisterComponent<Transform>();
	Engine::RegisterComponent<MeshRenderer>();
	Engine::RegisterComponent<Camera>();
	Engine::RegisterComponent<MeshCollisionBox>();
	Engine::RegisterComponent<Physics>();
	Engine::RegisterComponent<SpriteRenderer>();
	Engine::RegisterComponent<Light>();

	ProjectManager::CreateNewProject("C:\\Users\\tombr\\OneDrive\\Desktop\\Downloads\\Test Hierachy");
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


	Shaders::activeShader = Shader(defaultVertexSource, defaultFragmentSource);
	Shaders::activeShader.Use();
	SceneManager::activeScene.LoadSkybox();
	Engine::Start();
	glfwSetKeyCallback(window, ProcessInput);
	glfwSetCursorPosCallback(window, HandleMouseInput); 

	Display::ShowWindow();
	
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

	bool gameViewOpen;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SetupDefaultScene();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		for (auto& source : Engine::FindComponentsInScene<Light>()) {
			auto& light = Engine::GetComponent<Light>(source);
			Shaders::activeShader.SetVec3("lightColour", light.colour);
			Shaders::activeShader.SetVec3("lightPos", light.transform->position);
		}

		Time::currentTime = glfwGetTime() - runtimeStartTime;
		Time::deltaTime = glfwGetTime() - Time::lastFrameTime;
		Time::lastFrameTime = glfwGetTime();


		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (sceneViewActive) {
			sceneView.Update(inRuntime);
			glDepthFunc(GL_LEQUAL);
			SceneManager::activeScene.shader.Use();
			SceneManager::activeScene.shader.SetMat4("view", glm::mat4(glm::mat3(sceneView.sceneCam.GetViewMatrix())));
			SceneManager::activeScene.shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), sceneView.view.dimensions.x / sceneView.view.dimensions.y, 0.1f, 100.0f));
			SceneManager::activeScene.RenderSkybox();
			Shaders::activeShader.Use();
			glDepthFunc(GL_LESS);
		}
		else
		{
			gameView.Update(inRuntime);

			SceneManager::activeScene.shader.Use();
			SceneManager::activeScene.shader.SetMat4("view", glm::mat4(glm::mat3(gameView.view.camera->GetViewMatrix())));
			SceneManager::activeScene.shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), gameView.view.dimensions.x / gameView.view.dimensions.y, 0.1f, 100.0f));
			SceneManager::activeScene.RenderSkybox();
			Shaders::activeShader.Use();
		}

		if(inRuntime)
			Scripting::Update();

		Engine::Update(inRuntime);
		if(inRuntime)
			Collisions::HandleCollision();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGui::ShowDemoWindow();
		ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(Display::width * 0.3f, Display::height));

		RenderSceneHierachy();
		ProjectManager::activeProject.RenderHierachy();
		ImGui::End();


		// ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Begin("View", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowPos("View", ImVec2(Display::width * 0.3f, 0.0f));
		ImGui::SetWindowSize("View", ImVec2(Display::width * 0.45f, Display::height * 0.75f));
		if (ImGui::BeginTabBar("View", ImGuiTabBarFlags_None)) {
			if(ImGui::BeginTabItem("Scene")) {
				if(!sceneViewActive)
					sceneViewActive = true;
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Game")) {
				if(sceneViewActive)
					sceneViewActive = false;
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
		/*
		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos("Settings", ImVec2(20, SCREEN_HEIGHT - 350));
		ImGui::InputText("Entry Point", entryPoint, IM_ARRAYSIZE(entryPoint));
		ImGui::SetWindowSize("Settings", ImVec2(350, 350));
		ImGui::End();
		*/
		Console::Render();
		ImGui::Begin("Components", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowPos("Components", ImVec2(Display::width * 0.75f, 0.0f));
		ImGui::SetWindowSize("Components", ImVec2(Display::width * 0.25f, Display::height));


		// Built-In Component Displays
		// this shit below was made in a rush, i definitely need to clean this up at some point

		if (selectedEntity != -1) {
			ImGui::InputText("###Entity_Name", &SceneManager::activeScene.items[selectedEntity].name);
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


			std::vector<std::string> removingScripts;
			for (auto script : Scripting::GetEntityComponentInstances(selectedEntity)) {
				bool scriptExists = true;
				if (ImGui::CollapsingHeader(script.first.c_str(), &scriptExists)) {

					for (auto field : script.second.classData.fields) {
						RenderField(field.second, script.second);
					}
				}
				if (!scriptExists)
					removingScripts.push_back(script.first);
			}
			for (auto script : removingScripts) {
				Engine::RemoveScript(selectedEntity, script);
				Scripting::RemoveScriptInstance(selectedEntity, script);
			}

			ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x - 30.0f, 30.0f));
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Script")) {
					auto script = ProjectManager::activeProject.GetItem((const char*)payload->Data).name;
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
			/*
			ImGui::InputText("Component", addingComponent, IM_ARRAYSIZE(addingComponent));
			if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowSize().x, 30))) {
				if (std::string(addingComponent) == "MeshRenderer") {
					Engine::AddComponent<MeshRenderer>(selectedEntity, MeshRenderer());
					std::cout << "was mesh renderer" << std::endl;
				}
				else
					Engine::AddScript(selectedEntity, addingComponent);
				Scripting::LoadEntityScripts(selectedEntity);
			} */


		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}