#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "InputManager.h"
#include "Scene.h"
#include <string>
#include "ProjectManager.h"
#include "Engine.h"
#include "SceneLoader.h"
#include "imgui_stdlib.h"
#include "Physics.h"
#include "BoundingBox.h"
#include "Primitives.h"
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
#include "FrameBuffer.h"
#include "ViewManager.h"
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include "Time.h"
#include "Collisions.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <glm/ext/matrix_transform.hpp>
#include "UIManager.h"
#include "Light.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "MeshCollisionBox.h"
#include "SpriteRenderer.h"

GLFWwindow* window;

bool inRuntime = false;
bool isPaused = false;
float runtimeStartTime = 0.0f;
int clipboardEntity = -1;

unsigned int CreateEntity() {

	unsigned int newEnt = Engine::CreateEntity();
	Engine::AddComponent<Transform>(newEnt, Transform{});
	Scripting::OnEntityCreated(newEnt);
	SceneManager::activeScene->OnCreateEntity(newEnt);
	return newEnt;
}
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
			std::cout << Raycast::CheckHit(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 10.0f, &outHits) << std::endl;
		}
		if (key == GLFW_KEY_F3) {
			Scripting::OnRuntimeStart();
			inRuntime = true;
			Console::LogMessage("Runtime Started");
			runtimeStartTime = glfwGetTime();
		}

		if (key == GLFW_KEY_D && mods == GLFW_MOD_CONTROL && UIManager::sceneFrame.selectedEntity > -1 ) {
			unsigned int newEntity = Engine::CreateEntity();
			for (auto& script : Engine::GetEntityScripts(UIManager::sceneFrame.selectedEntity)) {
				Engine::AddScript(newEntity, script);
				Scripting::LoadEntityScript(newEntity, script);
			}
			for (auto& comp : Engine::GetEntityComponents(UIManager::sceneFrame.selectedEntity)) {
				Engine::AddComponent(newEntity, comp);
			}
		}
		if (key == GLFW_KEY_DELETE && UIManager::sceneFrame.selectedEntity > -1) {
			Engine::DestroyEntity(UIManager::sceneFrame.selectedEntity);
			Scripting::OnEntityDestroyed(UIManager::sceneFrame.selectedEntity);
			SceneManager::OnEntityDestroyed(UIManager::sceneFrame.selectedEntity);
			UIManager::sceneFrame.selectedEntity = -1;
		}
	}
}
void HandleMouseInput(GLFWwindow* window, double xpos, double ypos) {
	InputManager::mouse.MouseCallback(window, xpos, ypos);
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);		
	if (UIManager::viewportFrame.sceneViewFrameOpen)
		ViewManager::sceneView.sceneCam.ProcessCameraMouse();
	else if (inRuntime)
		ViewManager::gameView.view.camera->ProcessCameraMouse();
}

void SetupDefaultScene() {

	unsigned int light = CreateEntity();
	SceneManager::activeScene->items[light].name = "Light";
	Engine::AddComponent<Light>(light, Light());
}

void RenderScene() {
	Engine::Render();
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = Display::Create();

	glewInit();
	glfwSwapInterval(1);

	mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", "C:\\Program Files (x86)\\Mono\\etc");

	Scripting::Initialize("");

	UIManager::Initialize();

	Engine::RegisterComponent<Transform>();
	Engine::RegisterComponent<MeshRenderer>();
	Engine::RegisterComponent<Camera>();
	Engine::RegisterComponent<MeshCollisionBox>();
	Engine::RegisterComponent<Physics>();
	Engine::RegisterComponent<SpriteRenderer>();
	Engine::RegisterComponent<Light>();

	ProjectManager::CreateNewProject("C:\\Users\\tombr\\OneDrive\\Desktop\\Downloads\\Test Hierachy\\");
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


	ShaderManager::LoadDefaultShaders();
	SceneManager::AddScene("temp.ShbaScene");
	SceneManager::ChangeScene("temp.ShbaScene");
	SceneManager::activeScene->LoadSkybox();
	Engine::Start();
	glfwSetKeyCallback(window, ProcessInput);
	glfwSetCursorPosCallback(window, HandleMouseInput); 

	Display::ShowWindow();

	ViewManager::sceneView.view.framebuffer.Generate();
	ViewManager::gameView.view.framebuffer.Generate();


	bool gameViewOpen;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SetupDefaultScene();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		for (auto& source : Engine::FindComponentsInScene<Light>()) {
			auto& light = Engine::GetComponent<Light>(source);
			Renderer::shaderData.lightColour = light.colour;
			Renderer::shaderData.lightPos = light.transform->position;
		}

		Time::currentTime = glfwGetTime() - runtimeStartTime;
		Time::deltaTime = glfwGetTime() - Time::lastFrameTime;
		Time::lastFrameTime = glfwGetTime();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (UIManager::viewportFrame.sceneViewFrameOpen) {
			Renderer::shaderData.view = ViewManager::sceneView.sceneCam.GetViewMatrix();
			Renderer::shaderData.viewPos = ViewManager::sceneView.sceneCam.transform->position;
			Renderer::shaderData.projection = glm::perspective(glm::radians(45.0f), ViewManager::sceneView.view.framebuffer.dimensions.x / ViewManager::sceneView.view.framebuffer.dimensions.y, 0.1f, 100.0f);
			ViewManager::sceneView.view.framebuffer.Bind();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ViewManager::sceneView.Update(inRuntime);
			Renderer::ChangeShader("ShibaEngine_Skybox");
			glDepthFunc(GL_LEQUAL);
			ShaderManager::shader->SetMat4("skyView", glm::mat4(glm::mat3(ViewManager::sceneView.sceneCam.GetViewMatrix())));
			SceneManager::activeScene->RenderSkybox();
			glDepthFunc(GL_LESS);
			RenderScene();
			ViewManager::sceneView.view.framebuffer.Unbind();
		}
		if (UIManager::viewportFrame.gameViewFrameOpen) {
			ViewManager::gameView.Update(inRuntime);
			if (ViewManager::gameView.view.hasCamera) {
				Renderer::shaderData.view = ViewManager::gameView.view.camera->GetViewMatrix();
				Renderer::shaderData.viewPos = ViewManager::gameView.view.camera->transform->position;
				Renderer::shaderData.projection = glm::perspective(glm::radians(45.0f), ViewManager::gameView.view.framebuffer.dimensions.x / ViewManager::gameView.view.framebuffer.dimensions.y, 0.1f, 100.0f);
				ViewManager::gameView.view.framebuffer.Bind();
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Renderer::ChangeShader("ShibaEngine_Skybox");
				glDepthFunc(GL_LEQUAL);
				ShaderManager::shader->SetMat4("skyView", glm::mat4(glm::mat3(ViewManager::gameView.view.camera->GetViewMatrix())));
				SceneManager::activeScene->RenderSkybox();
				glDepthFunc(GL_LESS);
				RenderScene();
				ViewManager::gameView.view.framebuffer.Unbind();
			}
		}

		if (inRuntime) {
			Scripting::Update();
			Engine::Update();
		}
		if(inRuntime)
			Collisions::HandleCollision();

		UIManager::Update();

		glfwSwapBuffers(window);
	}
}