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
#include "Engine.h"
#include "Scripting.h"
#include "Camera.h"
#include <Mono/jit/jit.h>
#include <Mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include "Mesh.h"
#include <GLFW/glfw3.h>
#include "Time.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

GLFWwindow* window;

std::vector<Vertex> vertices = { Vertex{ glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) } 
};

const char* defaultVertexSource = R"GLSL(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 Color;

void main(){
	gl_Position = projection * view * model * vec4(position, 1.0);
	Color = color;
}

)GLSL";

const char* defaultFragmentSource = R"GLSL(
#version 330 core

in vec4 Color;

out vec4 FragColor;

void main(){
	FragColor = Color;
}
)GLSL";
char entryPoint[128];
Camera camera;
Camera& activeCamera = camera;
bool inRuntime = false;

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
		if (key == GLFW_KEY_F2) {
			std::string script;
			std::cin >> script;
			Engine::AddScript(0, script);
		}
		if (key == GLFW_KEY_F3) {
			Scripting::OnRuntimeStart();

			auto camEntities = Engine::FindComponentsInScene<Camera>();
			if (camEntities.size() > 0) {
				std::cout << "set camera to entity: " << camEntities[0] << std::endl;
				activeCamera = Engine::GetComponent<Camera>(camEntities[0]);
			}
			inRuntime = true;
		}
	}
}

void HandleMouseInput(GLFWwindow* window, double xpos, double ypos) {
	InputManager::mouse.MouseCallback(window, xpos, ypos);
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);		
	activeCamera.ProcessCameraMouse();
	

}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shiba Engine", NULL, NULL);
	glfwMakeContextCurrent(window);
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
	auto entity = Engine::CreateEntity();
	Engine::AddComponent<Transform>(entity, Transform(glm::vec3(0.0f, 1.0f, 0.0f)));
	Engine::AddComponent<MeshRenderer>(entity, MeshRenderer(Mesh(vertices)));
	camera = Camera(glm::vec3(0.0f));
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


	Shaders::activeShader = Shader(defaultVertexSource, defaultFragmentSource);
	Shaders::activeShader.Use();
	Engine::Start();
	unsigned int selectedEntity = -1;
	glfwSetKeyCallback(window, ProcessInput);
	glfwSetCursorPosCallback(window, HandleMouseInput);
	glfwShowWindow(window);

	activeCamera = camera;
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


	char addingComponent[64] = "";
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		Time::deltaTime = glfwGetTime() - Time::lastFrameTime;
		Time::lastFrameTime = glfwGetTime();
		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT);

		if(inRuntime)
			Scripting::Update();

		activeCamera.position += activeCamera.forward * InputManager::MoveVert() * Time::deltaTime;
		activeCamera.position += activeCamera.right * InputManager::MoveHorz() * Time::deltaTime;
		activeCamera.position += activeCamera.worldUp * InputManager::MoveUpDown() * Time::deltaTime;
		activeCamera.UpdateCameraVectors();


		Shaders::activeShader.SetMat4("projection", glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f));
		Shaders::activeShader.SetMat4("view", activeCamera.GetViewMatrix());
		Engine::Update();

		

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Begin("Enities", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::BeginListBox("###Entities", ImVec2(280, SceneManager::activeScene.entities.size() * 19))) {
			for (unsigned int entity : SceneManager::activeScene.entities) {
				if (ImGui::Selectable(std::to_string(entity).c_str())) {
					selectedEntity = entity;
				}
			}
			ImGui::EndListBox();

			if (ImGui::Button("New Entity")) {
				Engine::CreateEntity();
			}
		}
		ImGui::End();

		/*
		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos("Settings", ImVec2(20, SCREEN_HEIGHT - 350));
		ImGui::InputText("Entry Point", entryPoint, IM_ARRAYSIZE(entryPoint));
		ImGui::SetWindowSize("Settings", ImVec2(350, 350));
		ImGui::End();
		*/

		if (selectedEntity != -1) {
			std::string windowName = std::to_string(selectedEntity);
			ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
			ImGui::SetWindowSize(windowName.c_str(), ImVec2(350, SCREEN_HEIGHT * 0.9));
			ImGui::SetWindowPos(windowName.c_str(), ImVec2(SCREEN_WIDTH - 380, 50));


			// Built-In Component Displays
			
			if (Engine::HasComponent<Transform>(selectedEntity)) {
				if (ImGui::CollapsingHeader("Transform")) {

					auto transform = &Engine::GetComponent<Transform>(selectedEntity);
					ImGui::InputFloat3("position", &transform->position[0]);
					ImGui::InputFloat3("rotation", &transform->rotation[0]);
					ImGui::InputFloat3("pivot", &transform->pivot[0]);
					ImGui::InputFloat3("scale", &transform->scale[0]);
				}
				ImGui::Separator();

			}

			if (Engine::HasComponent<MeshRenderer>(selectedEntity)) {
				if (ImGui::CollapsingHeader("Mesh Renderer")) {

				}
				ImGui::Separator();

			}

			if (Engine::HasComponent<Camera>(selectedEntity)) {
				if (ImGui::CollapsingHeader("Camera")) {
					auto camera = Engine::GetComponent<Camera>(selectedEntity);
					ImGui::InputFloat("Speed", &camera.speed);
					ImGui::InputFloat("Sensitivity", &camera.sensitivity);
				}
				ImGui::Separator();

			}

			if (ImGui::CollapsingHeader("Scripts")) {
				for (auto script : Scripting::GetEntityComponentInstances(selectedEntity)) {
					ImGui::Text(script.first.c_str());

					for (auto field : script.second.classData.fields) {
						switch (field.second.type) {
						case FieldType::Int:
							int value = script.second.GetFieldValue<int>(field.first);
							ImGui::InputInt(field.first.c_str(), &value);
							if(value != script.second.GetFieldValue<int>(field.first))
								script.second.SetFieldValue<int>(field.first, value);
						}
					}
					ImGui::Separator();
				}

			}

			ImGui::InputText("Component", addingComponent, IM_ARRAYSIZE(addingComponent));
			if (ImGui::Button("Add Component")) {
				Engine::AddScript(selectedEntity, addingComponent);
				Scripting::LoadEntityScripts(selectedEntity);
			}

			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}